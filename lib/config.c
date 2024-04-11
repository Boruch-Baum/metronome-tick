#include "config.h"
#include "keys.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define INIT_PRESETS_CAPACITY 2
#define MAX_KEY_STR_LEN 20 // "right arrow\0" + 8 escape characters from bolding

char str_to_key(char *str) {
	if (strcmp(str, "up") == 0) {
		return UP_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "down") == 0) {
		return DOWN_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "right") == 0) {
		return RIGHT_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "left") == 0) {
		return LEFT_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "tab") == 0) {
		return 9;
	} else if (strcmp(str, "enter") == 0) {
		return 13;
	} else if (strcmp(str, "space") == 0) {
		return 32;
	} else {
		return str[0];
	}
}

void key_to_str(char *dst, char c) {
	switch (c) {
	case UP_ARROW_SUBSTITUTION:
		strcpy(dst, "up arrow");
		break;
	case DOWN_ARROW_SUBSTITUTION:
		strcpy(dst, "down arrow");
		break;
	case RIGHT_ARROW_SUBSTITUTION:
		strcpy(dst, "right arrow");
		break;
	case LEFT_ARROW_SUBSTITUTION:
		strcpy(dst, "left arrow");
		break;
	case 13:
		strcpy(dst, "enter");
		break;
	case 32:
		strcpy(dst, "space");
		break;
	default:
		dst[0] = c;
		dst[1] = '\0';
	}
}

void boldify(char *str) {
	int len = strlen(str);
	char tmp[len + 1];
	memcpy(tmp, str, len);
	tmp[len] = '\0';
	memcpy(str, "\033[1m", 4);
	memcpy(str + 4, tmp, len);
	memcpy(str + len + 4, "\033[0m", 4);
	str[len + 8] = '\0';
}

FILE *read_config(void) {
	char config_path[PATH_MAX];
	const char *XDG_CONFIG_HOME = getenv("XDG_CONFIG_HOME");
	if (XDG_CONFIG_HOME == NULL || XDG_CONFIG_HOME[0] == '\0') {
		const char *HOME = getenv("HOME");
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
			.next = 'l',
			.prev = 'h',
			.toggle_play = ' ',
			.show_prompt = ':',
			.quit = 'q',
		},
		.presets_size = 0,
		.presets = malloc(sizeof(struct Preset) * presets_capacity),
	};

	FILE *file = read_config();
	if (file == NULL) {
		goto fill_return;
	}

	char line[MAX_LINE_LEN];
	char *error;
	while (fgets(line, MAX_LINE_LEN, file) != NULL) {
		if (line[0] == '#' || line[0] == '\n') {
			continue;
		}
		if (line[0] == '[') {
			if (config.presets_size == presets_capacity) {
				presets_capacity *= 2;
				config.presets = realloc(config.presets, sizeof(struct Preset) * presets_capacity);
			}
			char *pos = strrchr(line, ']');
			if (pos == NULL) {
				error = "unmatched '['";
				goto invalid_config_exit;
			} else if (pos - 1 == line) {
				error = "preset name must be non-empty";
				goto invalid_config_exit;
			}
			memcpy(config.presets[config.presets_size].name, line + 1, pos - line - 1);
			config.presets[config.presets_size].name[pos - line - 1] = '\0';
			config.presets[config.presets_size].bpm = 0;
			config.presets[config.presets_size].pattern[0] = '\0';
			config.presets_size += 1;
		} else {
			char *pos = strchr(line, '=');
			if (strncmp(line, "freq>", pos - line) == 0) {
				config.freq_accented = atoi(pos + 1);
			} else if (strncmp(line, "freq.", pos - line) == 0) {
				config.freq_general = atoi(pos + 1);
			} else if (strncmp(line, "interval", pos - line) == 0) {
				config.interval = atoi(pos + 1);
			} else if (strncmp(line, "up", pos - line) == 0) {
				config.keys.up = str_to_key(pos + 1);
			} else if (strncmp(line, "down", pos - line) == 0) {
				config.keys.down = str_to_key(pos + 1);
			} else if (strncmp(line, "next", pos - line) == 0) {
				config.keys.next = str_to_key(pos + 1);
			} else if (strncmp(line, "prev", pos - line) == 0) {
				config.keys.prev = str_to_key(pos + 1);
			} else if (strncmp(line, "toggle_play", pos - line) == 0) {
				config.keys.toggle_play = str_to_key(pos + 1);
			} else if (strncmp(line, "show_prompt", pos - line) == 0) {
				config.keys.show_prompt = str_to_key(pos + 1);
			} else if (strncmp(line, "quit", pos - line) == 0) {
				config.keys.quit = str_to_key(pos + 1);
			} else if (strncmp(line, "bpm", pos - line) == 0) {
				config.presets[config.presets_size - 1].bpm = atoi(pos + 1);
			} else if (strncmp(line, "pattern", pos - line) == 0) {
				// strnlen - 1 to remove \n if < max len, and leave room for \0 if > max len
				int len = strnlen(pos + 1, MAX_PATTERN_LEN) - 1;
				memcpy(config.presets[config.presets_size - 1].pattern, pos + 1, len);
				config.presets[config.presets_size - 1].pattern[len] = '\0';
			}
		}
	}
	fclose(file);

fill_return:
	if (config.presets_size == 0) {
		config.presets[0] = (struct Preset){
			.name = "Default",
			.bpm = 120,
			.pattern = ">...",
		};
		config.presets_size = 1;
	}
	return config;
invalid_config_exit:
	fprintf(stderr, "Invalid configuration: %s\n", error);
	fclose(file);
	free(config.presets);
	exit(1);
}

void display_config(struct Config *c) {
	char up_key[MAX_KEY_STR_LEN];
	char down_key[MAX_KEY_STR_LEN];
	char next_key[MAX_KEY_STR_LEN];
	char prev_key[MAX_KEY_STR_LEN];
	char toggle_play_key[MAX_KEY_STR_LEN];
	char show_prompt_key[MAX_KEY_STR_LEN];
	char quit_key[MAX_KEY_STR_LEN];

	key_to_str(up_key, c->keys.up);
	key_to_str(down_key, c->keys.down);
	key_to_str(next_key, c->keys.next);
	key_to_str(prev_key, c->keys.prev);
	key_to_str(toggle_play_key, c->keys.toggle_play);
	key_to_str(show_prompt_key, c->keys.show_prompt);
	key_to_str(quit_key, c->keys.quit);

	boldify(up_key);
	boldify(down_key);
	boldify(next_key);
	boldify(prev_key);
	boldify(toggle_play_key);
	boldify(show_prompt_key);
	boldify(quit_key);

	printf("%s +BPM | %s -BPM | %s Next preset | %s Prev preset | %s Toggle play | %s Show prompt | %s Quit\n",
			up_key, down_key, next_key, prev_key, toggle_play_key, show_prompt_key, quit_key);
}
