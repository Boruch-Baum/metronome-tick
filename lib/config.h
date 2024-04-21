struct Keys {
	char up;
	char down;
	char next;
	char prev;
	char toggle_play;
	char save;
	char edit;
	char add;
	char delete;
	char show_prompt;
	char quit;
};

struct Config {
	int freq_accented;
	int freq_general;
	int interval;
	struct Keys keys;
};

void get_config(struct Config*);
void display_keybinds(struct Config*);
