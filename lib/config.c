#include "config.h"
#include "files.h"
#include "keys.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY_STR_LEN 20 // "right arrow\0" + 8 escape characters from bolding

void get_config_path(char *path) {
	get_xdg_path(path, "XDG_CONFIG_HOME", ".config", "tick/tick.ini");
}

void process_config_file(struct Config *config, FILE *file) {
	char line[MAX_LINE_LEN];
	char error[MAX_LINE_LEN+19]; // 19 from "unrecognized key ''"
	while (fgets(line, MAX_LINE_LEN, file) != NULL) {
		if (line[0] == '#' && line[0] == '\n') {
			continue;
		}
		char *pos = strchr(line, '=');
		if (pos == NULL) {
			strcpy(error, "expect key=value");
			goto invalid_config_exit;
		}
		*pos = '\0';
		pos += 1;
		if (strcmp(line, "freq>") == 0) {
			config->freq_accented = atoi(pos);
		} else if (strcmp(line, "freq.") == 0) {
			config->freq_general = atoi(pos);
		} else if (strcmp(line, "fine_interval") == 0) {
			config->fine_interval = atoi(pos);
		} else if (strcmp(line, "interval") == 0) {
			config->interval = atoi(pos);
		} else if (strcmp(line, "up_fine") == 0) {
			config->keys.up = str_to_key(pos);
		} else if (strcmp(line, "down_fine") == 0) {
			config->keys.down = str_to_key(pos);
		} else if (strcmp(line, "up") == 0) {
			config->keys.up = str_to_key(pos);
		} else if (strcmp(line, "down") == 0) {
			config->keys.down = str_to_key(pos);
		} else if (strcmp(line, "next") == 0) {
			config->keys.next = str_to_key(pos);
		} else if (strcmp(line, "prev") == 0) {
			config->keys.prev = str_to_key(pos);
		} else if (strcmp(line, "toggle_play") == 0) {
			config->keys.toggle_play = str_to_key(pos);
		} else if (strcmp(line, "save") == 0) {
			config->keys.save = str_to_key(pos);
		} else if (strcmp(line, "rename") == 0) {
			config->keys.rename = str_to_key(pos);
		} else if (strcmp(line, "create") == 0) {
			config->keys.create = str_to_key(pos);
		} else if (strcmp(line, "delete") == 0) {
			config->keys.delete = str_to_key(pos);
		} else if (strcmp(line, "open_prompt") == 0) {
			config->keys.open_prompt = str_to_key(pos);
		} else if (strcmp(line, "quit") == 0) {
			config->keys.quit = str_to_key(pos);
		} else {
			sprintf(error, "unrecognized key '%s'", line);
			goto invalid_config_exit;
		}
	}
	return;

invalid_config_exit:
	fclose(file);
	fprintf(stderr, "Invalid configuration: %s\n", error);
	exit(EXIT_FAILURE);
}

void get_config(struct Config *config) {
	*config = (struct Config){
		.freq_accented = 587,
		.freq_general = 440,
                .fine_interval = 3,
		.interval = 20,
		.keys = {
			.up_fine = 'k',
			.down_fine = 'j',
			.up = 'K',
			.down = 'J',
			.next = 'l',
			.prev = 'h',
			.toggle_play = ' ',
			.save = 's',
			.rename = 'r',
			.create = 'c',
			.delete = 'd',
			.open_prompt = ':',
			.quit = 'q',
		},
	};
	char path[PATH_MAX];
	get_config_path(path);
	FILE *file = fopen(path, "r");
	if (file != NULL) {
		process_config_file(config, file);
		fclose(file);
	}
}

void boldify(char *str) {
	int len = strlen(str);
	memmove(str+4, str, len);
	memcpy(str, "\033[1m", 4);
	memcpy(str+len+4, "\033[0m", 4);
	str[len+8] = '\0';
}

void display_keybinds(struct Config *c) {
	char up_fine_key[MAX_KEY_STR_LEN];
	char down_fine_key[MAX_KEY_STR_LEN];
	char up_key[MAX_KEY_STR_LEN];
	char down_key[MAX_KEY_STR_LEN];
	char next_key[MAX_KEY_STR_LEN];
	char prev_key[MAX_KEY_STR_LEN];
	char toggle_play_key[MAX_KEY_STR_LEN];
	char save_key[MAX_KEY_STR_LEN];
	char rename_key[MAX_KEY_STR_LEN];
	char create_key[MAX_KEY_STR_LEN];
	char delete_key[MAX_KEY_STR_LEN];
	char open_prompt_key[MAX_KEY_STR_LEN];
	char quit_key[MAX_KEY_STR_LEN];

	key_to_str(up_fine_key, c->keys.up_fine);
	key_to_str(down_fine_key, c->keys.down_fine);
	key_to_str(up_key, c->keys.up);
	key_to_str(down_key, c->keys.down);
	key_to_str(next_key, c->keys.next);
	key_to_str(prev_key, c->keys.prev);
	key_to_str(toggle_play_key, c->keys.toggle_play);
	key_to_str(save_key, c->keys.save);
	key_to_str(rename_key, c->keys.rename);
	key_to_str(create_key, c->keys.create);
	key_to_str(delete_key, c->keys.delete);
	key_to_str(open_prompt_key, c->keys.open_prompt);
	key_to_str(quit_key, c->keys.quit);

	boldify(up_fine_key);
	boldify(down_fine_key);
	boldify(up_key);
	boldify(down_key);
	boldify(next_key);
	boldify(prev_key);
	boldify(toggle_play_key);
	boldify(save_key);
	boldify(rename_key);
	boldify(create_key);
	boldify(delete_key);
	boldify(open_prompt_key);
	boldify(quit_key);

	printf("%s/%s +BPM | %s/%s -BPM | %s Next preset | %s Prev preset | %s Toggle play\n"
			"%s Save | %s Rename | %s Create | %s Delete | %s Open prompt | %s Quit\n",
			up_fine_key, up_key, down_fine_key, down_key,
                        next_key, prev_key,
                        toggle_play_key, save_key,
			rename_key, create_key, delete_key,
                        open_prompt_key, quit_key);
}
