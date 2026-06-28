#include "player.h"
#include <math.h>

#define SAMPLE_RATE 8000 // number of values per second
#define PEAK_TO_PEAK 256 // 2^8 for uint8_t
#define AMPLITUDE (PEAK_TO_PEAK / 2)
#define TICK_MS 22
#define TICK_GAIN 2.0
#define BODY_DECAY 125.0
#define IMPULSE_FRAMES 4
#define IMPULSE_LEVEL 120.0
#define IMPULSE_STEP 28.0
#define TONE_LEVEL 88.0
#define HARMONIC_LEVEL 36.0
#define HARMONIC_RATIO 2.7

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

static uint8_t to_sample(double centered) {
	double sample = AMPLITUDE + TICK_GAIN * centered;

	if (sample < 0) {
		return 0;
	}
	if (sample > PEAK_TO_PEAK - 1) {
		return PEAK_TO_PEAK - 1;
	}
	return sample + 0.5;
}

uint8_t *create_waves(int *buff_size, struct PlayerState *ps) {
	int tick_frame_size = SAMPLE_RATE * 60 / ps->bpm; // SAMPLE_RATE / (bpm / 60)
	if (tick_frame_size == 0) {
		*buff_size = 0;
		return NULL;
	}
	*buff_size = tick_frame_size * strlen(ps->rhythm);
	uint8_t *buffer = malloc(*buff_size);
	if (buffer == NULL) {
		*buff_size = 0;
		return NULL;
	}
	memset(buffer, AMPLITUDE, *buff_size);
	int tick_frames = SAMPLE_RATE * TICK_MS / 1000;
	if (tick_frames > tick_frame_size) {
		tick_frames = tick_frame_size;
	}
	double body_step = exp(-BODY_DECAY / SAMPLE_RATE);
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
			continue;
		}
		int offset = i * tick_frame_size;
		double phase_step = 2 * M_PI * freq / SAMPLE_RATE;
		double phase = 0.0;
		double body = 1.0;
		for (int j = 0; j < tick_frames; j++) {
			double impulse = j < IMPULSE_FRAMES ? IMPULSE_LEVEL - j * IMPULSE_STEP : 0;
			double tone = TONE_LEVEL * sin(phase) + HARMONIC_LEVEL * sin(phase * HARMONIC_RATIO);

			buffer[offset+j] = to_sample(impulse + tone * body);
			body *= body_step;
			phase += phase_step;
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
