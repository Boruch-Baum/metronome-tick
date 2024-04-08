#include "lib/config.h"
#include "lib/input.h"
#include "lib/player.h"
#include <pthread.h>

int main(void) {
	struct Config config = get_config();
	snd_pcm_t *pcm;
	prepare_player(&pcm);

	int buff_size;
	uint8_t *buffer = create_waves(&buff_size, config.presets[0].bpm, config.presets[0].pattern, config.freq_accented, config.freq_general);
	struct PlayerState player_state = {
		.playing = 1,
		.bpm = config.presets[0].bpm,
		.freq_accented = config.freq_accented,
		.freq_general = config.freq_general,
	};
	strncpy(player_state.pattern, config.presets[0].pattern, 32);
	free(config.presets);

	pthread_t player_tid;
	struct PlayerArgs player_args = { .pcm = pcm, .buffer = buffer, .buff_size = buff_size };
	pthread_create(&player_tid, NULL, start_player, &player_args);

	prepare_for_input();
	printf("\n");
	display_player_state(&player_state);
	int c;
	while (1) {
		c = get_input();
		printf("pressed %d\n", c);
	}
	return 0;
}
