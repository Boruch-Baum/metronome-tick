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
			set_bpm(&m, m.ps.bpm+m.config.interval);
		} else if (c == m.config.keys.down) {
			set_bpm(&m, m.ps.bpm-m.config.interval);
		} else if (c == m.config.keys.next) {
			set_preset(&m, (m.preset_index+1)%m.presets.size);
		} else if (c == m.config.keys.prev) {
			set_preset(&m, (m.preset_index+m.presets.size-1)% m.presets.size);
		} else if (c == m.config.keys.toggle_play) {
			if (m.ps.playing) {
				stop_metronome(&m);
			} else {
				start_metronome(&m);
			}
		} else if (c == m.config.keys.add) {
			char preset_name[MAX_PRESET_NAME_LEN];
			get_command(preset_name, MAX_PRESET_NAME_LEN, "Preset name: ");
			struct Preset preset = { .bpm = m.ps.bpm };
			memcpy(preset.name, preset_name, MAX_PRESET_NAME_LEN);
			memcpy(preset.pattern, m.ps.pattern, MAX_PATTERN_LEN);
			add_preset(&m.presets, &preset);
			write_preset(&preset);
			set_preset(&m, m.presets.size - 1);
		} else if (c == m.config.keys.show_prompt) {
			char line[MAX_COMMAND_LEN];
			get_command(line, MAX_COMMAND_LEN, ":");
			char *pos = strchr(line, ' ');
			if (strncmp(line, "bpm", pos-line) == 0) {
				set_bpm(&m, atoi(pos+1));
			} else if (strncmp(line, "pattern", pos-line) == 0) {
				set_pattern(&m, pos+1);
			} else if (strncmp(line, "preset", pos-line) == 0) {
				int found = 0;
				for (int i = 0; i < m.presets.size; i++) {
					if (strcmp(m.presets.items[i].name, pos+1) == 0) {
						set_preset(&m, i);
						found = 1;
					}
				}
				if (!found) {
					printf("Cannot find preset with name %s", pos+1);
					fflush(stdout);
				}
			} else if (strncmp(line, "set", pos-line) == 0) {
				char *sep = strchr(pos+1, '@');
				if (sep-pos > MAX_PATTERN_LEN) { // sep - (pos + 1) > MAX_PATTERN_LEN - 1
					printf("Pattern cannot be longer than %d characters", MAX_PATTERN_LEN-1);
					fflush(stdout);
				} else {
					*sep = '\0';
					set_pattern(&m, pos+1);
					set_bpm(&m, atoi(sep+1));
				}
			} else if (strncmp(line, "quit", pos-line) == 0) {
				stop_metronome(&m);
				printf("\n");
				break;
			}
		} else if (c == m.config.keys.quit) {
			stop_metronome(&m);
			printf("\n");
			break;
		}
	}
	free(m.pa.buffer);
	free(m.presets.items);
	return EXIT_SUCCESS;
}
