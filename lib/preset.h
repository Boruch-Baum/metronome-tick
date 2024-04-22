#include "files.h"

#define MAX_PRESET_NAME_LEN (MAX_LINE_LEN - 3) // 3 = []\n
#define MAX_RHYTHM_LEN 32

struct Preset {
	char name[MAX_PRESET_NAME_LEN];
	int bpm;
	char rhythm[MAX_RHYTHM_LEN];
};

struct Presets {
	struct Preset *items;
	int size;
	int capacity;
};

void get_presets(struct Presets*);
void rename_preset(struct Preset*, char*);
void edit_preset_settings(struct Preset*, int, char*);
void create_preset(struct Presets*, char*, int, char*);
void delete_preset(struct Presets*, int);
