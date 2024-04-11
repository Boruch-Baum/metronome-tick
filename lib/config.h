#define MAX_LINE_LEN 64
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
	char name[MAX_LINE_LEN - 3]; // 64 - []\n
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

struct Config get_config(void);
void display_config(struct Config*);
