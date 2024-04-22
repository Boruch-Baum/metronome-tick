#include <alsa/asoundlib.h>

struct PlayerState {
	int playing;
	int bpm;
	char rhythm[32];
	int freq_accented;
	int freq_general;
};

struct PlayerArgs {
	snd_pcm_t *pcm;
	uint8_t *buffer;
	int buff_size;
};

void prepare_player(snd_pcm_t**);
uint8_t *create_waves(int*, struct PlayerState*);
void *start_player(void*);
