#include "lib/input.h"
#include "lib/metronome.h"

int main(void) {
	struct Metronome m;
	init_metronome(&m);
	start_metronome(&m);

	prepare_for_input();
	int c;
	while (1) {
		c = get_input();
		if (c == m.config.keys.up) {
			set_bpm(&m, m.ps.bpm + m.config.interval);
		} else if (c == m.config.keys.down) {
			set_bpm(&m, m.ps.bpm - m.config.interval);
		} else if (c == m.config.keys.next) {
			set_preset(&m, (m.preset_index + 1) % m.config.presets_size);
		} else if (c == m.config.keys.prev) {
			set_preset(&m, (m.preset_index + m.config.presets_size - 1) % m.config.presets_size);
		} else if (c == m.config.keys.toggle_play) {
			if (m.ps.playing) {
				stop_metronome(&m);
			} else {
				start_metronome(&m);
			}
		} else if (c == m.config.keys.show_prompt) {
		} else if (c == m.config.keys.quit) {
			stop_metronome(&m);
			printf("\n");
			break;
		}
	}
	free(m.pa.buffer);
	free(m.config.presets);
	return 0;
}
