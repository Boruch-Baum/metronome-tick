#include "files.h"

#define MAX_PRESET_NAME_LEN (MAX_LINE_LEN - 3) // 3 = []\n
#define MAX_PATTERN_LEN 32

struct Preset {
	char name[MAX_PRESET_NAME_LEN];
	int bpm;
	char pattern[MAX_PATTERN_LEN];
};

struct Presets {
	struct Preset *items;
	int size;
	int capacity;
};

void get_presets(struct Presets*);
void add_preset(struct Presets*, struct Preset*);
