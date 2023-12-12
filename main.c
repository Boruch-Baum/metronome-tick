#include "lib/config.h"
#include <alsa/asoundlib.h>
#include <string.h>

#define SAMPLE_RATE 8000 // number of values per second
#define AMPLITUDE 256    // 2^8 for uint8_t

int main(void) {
	struct Config config = get_config();
	snd_pcm_t *pcm;
	int rc = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return 1;
	}

	snd_pcm_set_params(pcm,
		SND_PCM_FORMAT_U8,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		1,           // number of channels
		SAMPLE_RATE,
		0,           // disable software resampling
		40000);      // latency

	int tick_frame_size = SAMPLE_RATE / config.presets[0].bpm * 60; // SAMPLE_RATE / (bpm / 60)
	int buff_size = tick_frame_size * strlen(config.presets[0].pattern);
	uint8_t buffer[buff_size];
	memset(buffer, 0, buff_size);
	int freq;

	for (int i = 0; config.presets[0].pattern[i] != '\0'; i++) {
		switch (config.presets[0].pattern[i]) {
		case '>':
			freq = config.freq_accented;
			break;
		case '.':
			freq = config.freq_general;
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
	free(config.presets);

	snd_pcm_sframes_t frames;
	while (1) {
		frames = snd_pcm_writei(pcm, buffer, buff_size);
		if (frames < 0) {
			frames = snd_pcm_recover(pcm, frames, 0);
		}
		if (frames < 0) {
			fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(frames));
			break;
		} else if (frames < buff_size) {
			fprintf(stderr, "short write: expected %i, wrote %li\n", buff_size, frames);
		}
	}

	snd_pcm_close(pcm);
	return 0;
}
