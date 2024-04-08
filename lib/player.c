#include "player.h"

#define SAMPLE_RATE 8000 // number of values per second
#define AMPLITUDE 256 // 2^8 for uint8_t

void prepare_player(snd_pcm_t **pcm) {
	int rc = snd_pcm_open(pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		exit(1);
	}
	snd_pcm_set_params(*pcm,
		SND_PCM_FORMAT_U8,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		1,           // number of channels
		SAMPLE_RATE,
		0,           // disable software resampling
		40000);      // latency
}

uint8_t *create_waves(int *buff_size, int bpm, char *pattern, int freq_accented, int freq_general) {
	int tick_frame_size = SAMPLE_RATE / bpm * 60; // SAMPLE_RATE / (bpm / 60)
	*buff_size = tick_frame_size * strlen(pattern);
	uint8_t *buffer = calloc(sizeof(uint8_t), *buff_size);
	int freq;
	for (int i = 0; pattern[i] != '\0'; i++) {
		switch (pattern[i]) {
		case '>':
			freq = freq_accented;
			break;
		case '.':
			freq = freq_general;
			break;
		default:
			freq = 0;
		}
		int inc = AMPLITUDE * freq / SAMPLE_RATE;
		int offset = i * tick_frame_size;
		for (int j = 0, osc = 0; j < tick_frame_size / 4; j++, osc += inc) {
			buffer[offset + j] = osc;
		}
	}
	return buffer;
}

void *start_player(void* args) {
	struct PlayerArgs *pa = args;
	snd_pcm_sframes_t frames;
	while (1) {
		frames = snd_pcm_writei(pa->pcm, pa->buffer, pa->buff_size);
		if (frames < 0) {
			frames = snd_pcm_recover(pa->pcm, frames, 0);
		}
		if (frames < 0) {
			fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(frames));
			break;
		} else if (frames < pa->buff_size) {
			fprintf(stderr, "short write: expected %i, wrote %li\n", pa->buff_size, frames);
		}
	}
	snd_pcm_close(pa->pcm);
	return NULL;
}
