#define LINE_SIZE 64

struct Keys {
	char up;
	char down;
	char play_pause;
	char show_prompt;
};

struct Preset {
	char name[LINE_SIZE - 3]; // 64 - []\n
	int bpm;
	char pattern[32];
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
