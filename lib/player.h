#include <alsa/asoundlib.h>

#define SAMPLE_RATE 8000 // number of values per second

struct player_args {
	snd_pcm_t *pcm;
	uint8_t *buffer;
	int buff_size;
};

void prepare_player(snd_pcm_t**);
void *start_player(void*);
