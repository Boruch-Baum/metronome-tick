#include "lib/config.h"
#include "lib/input.h"
#include "lib/player.h"
#include <alsa/asoundlib.h>
#include <string.h>
#include <pthread.h>

#define AMPLITUDE 256 // 2^8 for uint8_t

int main(void) {
	struct Config config = get_config();
	snd_pcm_t *pcm;
	prepare_player(&pcm);

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

	struct player_args args = { .pcm = pcm, .buffer = buffer, .buff_size = buff_size };
	start_player(&args);
	return 0;
}
