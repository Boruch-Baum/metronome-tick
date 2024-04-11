#define MAX_LINE_LEN 64
#define MAX_PRESET_NAME_LEN (MAX_LINE_LEN - 3) // 3 = []\n
#define MAX_PATTERN_LEN 32

struct Keys {
	char up;
	char down;
	char next;
	char prev;
	char toggle_play;
	char show_prompt;
	char quit;
};

struct Preset {
	char name[MAX_PRESET_NAME_LEN];
	int bpm;
	char pattern[MAX_PATTERN_LEN];
};

struct Config {
	int freq_accented;
	int freq_general;
	int interval;
	struct Keys keys;
	struct Preset *presets;
	int presets_size;
};

void get_config(struct Config*);
void display_keybinds(struct Config*);
