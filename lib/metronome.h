#include "config.h"
#include "player.h"
#include "preset.h"

struct Metronome {
	pthread_t tid;
	struct PlayerArgs pa;
	struct PlayerState ps;
	struct Config config;
	struct Presets presets;
	int preset_index;
};

void init_metronome(struct Metronome *m);
void start_metronome(struct Metronome *m);
void stop_metronome(struct Metronome *m);
void set_bpm(struct Metronome*, int);
void set_pattern(struct Metronome*, char*);
void set_preset(struct Metronome*, int);
void display_player_state(struct Metronome*);
