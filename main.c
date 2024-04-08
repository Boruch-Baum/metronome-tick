#include "lib/config.h"
#include "lib/input.h"
#include "lib/player.h"

int main(void) {
	struct Config config = get_config();
	struct PlayerState ps = {
		.playing = 0,
		.bpm = config.presets[0].bpm,
		.freq_accented = config.freq_accented,
		.freq_general = config.freq_general,
	};
	strncpy(ps.pattern, config.presets[0].pattern, 32);

	pthread_t tid;
	snd_pcm_t *pcm;
	prepare_player(&pcm);
	struct PlayerArgs pa = {.pcm = pcm};
	start_player(&tid, &ps, &pa);
	display_player_state(&ps);

	prepare_for_input();
	int c;
	while (1) {
		c = get_input();
		printf("pressed %d\n", c);
	}
	free(config.presets);
	return 0;
}
