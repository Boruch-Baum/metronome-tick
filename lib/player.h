#include <alsa/asoundlib.h>

struct PlayerArgs {
	snd_pcm_t *pcm;
	uint8_t *buffer;
	int buff_size;
};

void prepare_player(snd_pcm_t**);
uint8_t *create_waves(int*, int, char*, int, int);
void *start_player(void*);
