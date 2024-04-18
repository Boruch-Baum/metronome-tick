#include "preset.h"
#include <stdlib.h>
#include <string.h>

#define INIT_PRESETS_CAPACITY 2

void add_preset(struct Presets *presets, struct Preset *preset) {
	if (presets->size == presets->capacity) {
		presets->capacity *= 2;
		presets->items = realloc(presets->items, sizeof(struct Preset) * presets->capacity);
	}
	presets->items[presets->size] = *preset;
	presets->size += 1;
}

void write_preset(struct Preset *preset) {
	char str[MAX_LINE_LEN * 3 + 2]; // 2 from 2 '\n'
	sprintf(str, "\n[%s]\nbpm=%d\npattern=%s\n", preset->name, preset->bpm, preset->pattern);
	write_xdg_file("XDG_DATA_HOME", ".local/share", "tick/presets.ini", str);
}

void process_presets_file(struct Presets *presets, FILE *file) {
	char line[MAX_LINE_LEN];
	char error[MAX_LINE_LEN+18]; // 19 from "unrecognized key ''" - 1 from "="
	while (fgets(line, MAX_LINE_LEN, file) != NULL) {
		if (line[0] == '[') {
			char *pos = strrchr(line, ']');
			if (pos == NULL) {
				strcpy(error, "unmatched '['");
				goto invalid_config_exit;
			} else if (pos-1 == line) {
				strcpy(error, "preset name must be non-empty");
				goto invalid_config_exit;
			}
			struct Preset preset = { .bpm = 120, .pattern = "" };
			memcpy(preset.name, line+1, pos-line-1);
			add_preset(presets, &preset);
		} else if (line[0] != '\n') {
			char *pos = strchr(line, '=');
			if (pos == NULL) {
				strcpy(error, "expect key=value");
				goto invalid_config_exit;
			}
			*pos = '\0';
			pos += 1;
			if (strcmp(line, "bpm") == 0) {
				presets->items[presets->size-1].bpm = atoi(pos);
			} else if (strcmp(line, "pattern") == 0) {
				// strnlen - 1 to remove \n if < max len, and leave room for \0 if > max len
				int len = strnlen(pos, MAX_PATTERN_LEN) - 1;
				memcpy(presets->items[presets->size-1].pattern, pos, len);
				presets->items[presets->size-1].pattern[len] = '\0';
			} else {
				sprintf(error, "unrecognized key '%s'", line);
				goto invalid_config_exit;
			}
		}
	}
	return;

invalid_config_exit:
	fclose(file);
	free(presets->items);
	fprintf(stderr, "Invalid configuration: %s\n", error);
	exit(EXIT_FAILURE);
}

void get_presets(struct Presets *presets) {
	*presets = (struct Presets){
		.items = malloc(sizeof(struct Preset) * INIT_PRESETS_CAPACITY),
		.size = 0,
		.capacity = INIT_PRESETS_CAPACITY,
	};
	FILE *file = read_xdg_file("XDG_DATA_HOME", ".local/share", "tick/presets.ini");
	if (file != NULL) {
		process_presets_file(presets, file);
		fclose(file);
	}
	if (presets->size == 0) {
		presets->items[0] = (struct Preset){
			.name = "Default",
			.bpm = 120,
			.pattern = ">...",
		};
	}
}
