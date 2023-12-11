#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define INIT_PRESETS_CAPACITY 2

void throw_invalid_config(char *error, FILE *file, struct Config config) {
	fprintf(stderr, "Invalid configuration: %s\n", error);
	fclose(file);
	free(config.presets);
	exit(1);
}

char str_to_key(char *str) {
	if (strcmp(str, "space") == 0) {
		return 32;
	} else if (strcmp(str, "enter") == 0) {
		return 13;
	} else {
		return str[0];
	}
}

FILE *read_config(void) {
	char config_path[PATH_MAX];
	const char *XDG_CONFIG_HOME = getenv("XDG_CONFIG_HOME");
	if (XDG_CONFIG_HOME == NULL || XDG_CONFIG_HOME[0] == '\0') {
		const char* HOME = getenv("HOME");
		if (HOME == NULL || HOME[0] == '\0') {
			return NULL;
		}
		snprintf(config_path, PATH_MAX, "%s/.config/tick/tick.ini", HOME);
	} else {
		snprintf(config_path, PATH_MAX, "%s/tick/tick.ini", XDG_CONFIG_HOME);
	}
	return fopen(config_path, "r");
}

struct Config get_config(void) {
	int presets_capacity = INIT_PRESETS_CAPACITY;
	struct Config config = {
		.freq_accented = 587,
		.freq_general = 440,
		.interval = 20,
		.keys = {
			.up = 'k',
			.down = 'j',
			.play_pause = ' ',
			.show_prompt = ':',
		},
		.presets_size = 0,
		.presets = malloc(sizeof(struct Preset) * presets_capacity),
	};

	FILE *file = read_config();
	if (file == NULL) {
		return config;
	}

	char line[LINE_SIZE];
	while (fgets(line, LINE_SIZE, file) != NULL) {
		if (line[0] == '#' || line[0] == '\n') continue;
		if (line[0] == '[') {
			if (config.presets_size == presets_capacity) {
				presets_capacity *= 2;
				config.presets = realloc(config.presets, sizeof(struct Preset) * presets_capacity);
			}
			char *pos = strrchr(line, ']');
			if (pos == NULL) {
				throw_invalid_config("unmatched '['", file, config);
			} else if (pos - 1 == line) {
				throw_invalid_config("preset name must be non-empty", file, config);
			}
			strncpy(config.presets[config.presets_size].name, line+1, pos - line - 1);
			config.presets[config.presets_size].bpm = 0;
			config.presets[config.presets_size].pattern[0] = '\0';
			config.presets_size += 1;
		} else {
			char *pos = strchr(line, '=');
			if (strncmp(line, "freq>", pos - line) == 0) {
				config.freq_accented = atoi(pos+1);
			} else if (strncmp(line, "freq.", pos - line) == 0) {
				config.freq_general = atoi(pos+1);
			} else if (strncmp(line, "interval", pos - line) == 0) {
				config.interval = atoi(pos+1);
			} else if (strncmp(line, "up", pos - line) == 0) {
				config.keys.up = str_to_key(pos+1);
			} else if (strncmp(line, "down", pos - line) == 0) {
				config.keys.down = str_to_key(pos+1);
			} else if (strncmp(line, "play_pause", pos - line) == 0) {
				config.keys.play_pause = str_to_key(pos+1);
			} else if (strncmp(line, "show_prompt", pos - line) == 0) {
				config.keys.show_prompt = str_to_key(pos+1);
			} else if (strncmp(line, "bpm", pos - line) == 0) {
				config.presets[config.presets_size-1].bpm = atoi(pos+1);
			} else if (strncmp(line, "pattern", pos - line) == 0) {
				strncpy(config.presets[config.presets_size-1].pattern, pos+1, strlen(pos+1) - 1);
			}
		}
	}
	fclose(file);

	if (config.presets_size == 0) {
		config.presets[0] = (struct Preset){
			.name = "default",
			.bpm = 120,
			.pattern = ">...",
		};
		config.presets_size = 1;
	}
	return config;
}
