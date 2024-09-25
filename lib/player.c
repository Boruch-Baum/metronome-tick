#include "player.h"
#include <math.h>

#define SAMPLE_RATE 8000 // number of values per second
#define PEAK_TO_PEAK 256 // 2^8 for uint8_t
#define AMPLITUDE (PEAK_TO_PEAK / 2)
#define SINE_FACTOR (2 * M_PI / (PEAK_TO_PEAK - 1))

void prepare_player(snd_pcm_t **pcm) {
	int rc = snd_pcm_open(pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		exit(EXIT_FAILURE);
	}
	snd_pcm_set_params(*pcm,
		SND_PCM_FORMAT_U8,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		1,           // number of channels
		SAMPLE_RATE,
		0,           // disable software resampling
		40000);      // latency
}

uint8_t *create_waves(int *buff_size, struct PlayerState *ps) {
	int tick_frame_size = SAMPLE_RATE / ps->bpm * 60; // SAMPLE_RATE / (bpm / 60)
	*buff_size = tick_frame_size * strlen(ps->rhythm);
	uint8_t *buffer = calloc(*buff_size, sizeof(uint8_t));
	int freq;
	for (int i = 0; ps->rhythm[i] != '\0'; i++) {
		switch (ps->rhythm[i]) {
		case '>':
			freq = ps->freq_accented;
			break;
		case '.':
			freq = ps->freq_general;
			break;
		default:
			freq = 0;
			continue;
		}
		int inc = PEAK_TO_PEAK * freq / SAMPLE_RATE;
		int offset = i * tick_frame_size;
		for (int j = 0, osc = 0; j < tick_frame_size / 4; j++, osc += inc) {
			// https://zserge.com/posts/etude-in-c/
			buffer[offset+j] = AMPLITUDE*sin(osc*SINE_FACTOR) + AMPLITUDE;
		}
	}
	return buffer;
}

void *start_player(void* args) {
	struct PlayerArgs *pa = args;
	snd_pcm_sframes_t frames;
	int ret;
	while (1) {
		frames = snd_pcm_writei(pa->pcm, pa->buffer, pa->buff_size);
		if (frames < 0) {
			ret = snd_pcm_recover(pa->pcm, frames, 1);
			if (ret < 0) {
				fprintf(stderr, "snd_pcm_recover failed: %s\n", snd_strerror(ret));
				break;
			}
		}
	}
	snd_pcm_close(pa->pcm);
	return NULL;
}
