#include "lib/input.h"
#include "lib/player.h"

void apply_preset(struct PlayerState *ps, struct Config *config) {
	ps->bpm = config->presets[ps->preset_index].bpm;
	memcpy(ps->pattern, config->presets[ps->preset_index].pattern, MAX_PATTERN_LEN);
	memcpy(ps->preset_name, config->presets[ps->preset_index].name, MAX_PRESET_NAME_LEN);
}

int main(void) {
	struct Config config;
	get_config(&config);
	struct PlayerState ps = {
		.playing = 0,
		.freq_accented = config.freq_accented,
		.freq_general = config.freq_general,
		.preset_index = 0,
	};
	apply_preset(&ps, &config);

	pthread_t tid;
	snd_pcm_t *pcm;
	prepare_player(&pcm);
	struct PlayerArgs pa = {.pcm = pcm, .buffer = NULL};
	start_player(&tid, &ps, &pa);
	display_config(&config);
	display_player_state(&ps);

	prepare_for_input();
	int c;
	while (1) {
		c = get_input();
		if (c == config.keys.up) {
			ps.bpm += config.interval;
			display_player_state(&ps);
			if (ps.playing) {
				start_player(&tid, &ps, &pa);
			}
		} else if (c == config.keys.down) {
			ps.bpm -= config.interval;
			display_player_state(&ps);
			if (ps.playing) {
				start_player(&tid, &ps, &pa);
			}
		} else if (c == config.keys.next) {
			ps.preset_index = (ps.preset_index + 1) % config.presets_size;
			apply_preset(&ps, &config);
			display_player_state(&ps);
			if (ps.playing) {
				start_player(&tid, &ps, &pa);
			}
		} else if (c == config.keys.prev) {
			ps.preset_index = (ps.preset_index + config.presets_size - 1) % config.presets_size;
			apply_preset(&ps, &config);
			display_player_state(&ps);
			if (ps.playing) {
				start_player(&tid, &ps, &pa);
			}
		} else if (c == config.keys.toggle_play) {
			if (ps.playing) {
				pthread_cancel(tid);
				ps.playing = 0;
			} else {
				start_player(&tid, &ps, &pa);
			}
		} else if (c == config.keys.show_prompt) {
		} else if (c == config.keys.quit) {
			printf("\n");
			pthread_cancel(tid);
			break;
		}
	}
	free(pa.buffer);
	free(config.presets);
	return 0;
}
