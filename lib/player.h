#include <alsa/asoundlib.h>
#include <pthread.h>

struct PlayerState {
	int playing;
	int bpm;
	char pattern[32];
	int freq_accented;
	int freq_general;
};

// Compound arguments for the thread function `_start_player`.
// It is necessary to expose this so we can have an instance of it in main
// that never gets dropped from the stack, for the thread to access.
struct PlayerArgs {
	snd_pcm_t *pcm;
	uint8_t *buffer;
	int buff_size;
};

void prepare_player(snd_pcm_t**);
void start_player(pthread_t*, struct PlayerState*, struct PlayerArgs*);
void display_player_state(struct PlayerState*);
