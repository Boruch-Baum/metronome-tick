#include <alsa/asoundlib.h>
#include <string.h>

int main(void) {
	const int sample_rate = 8000; // number of values per second
	char *pattern = ">..";
	int bpm = 120;

	snd_pcm_t *pcm;
	int rc = snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return 1;
	}

	snd_pcm_set_params(pcm,
		SND_PCM_FORMAT_U8,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		1,           // number of channels
		sample_rate,
		0,           // disable software resampling
		40000);      // latency

	int tick_frame_size = sample_rate / bpm * 60; // sample_rate / (bpm / 60)
	int buff_size = tick_frame_size * strlen(pattern);
	uint8_t buffer[buff_size];

	for (int i = 0; pattern[i] != '\0'; i++) {
		int freq = pattern[i] == '>' ? 440 : 348;
		int inc = 256 * freq / sample_rate;
		int offset = i * tick_frame_size;
		for (int j = 0, osc = 0; j < tick_frame_size; j++, osc += inc) {
			buffer[offset + j] = osc;
		}
	}

	while (1) {
		rc = snd_pcm_writei(pcm, buffer, buff_size);
		if (rc == -EPIPE) {
			fprintf(stderr, "underrun\n");
			snd_pcm_prepare(pcm);
		} else if (rc < 0) {
			fprintf(stderr, "error on writei: %s\n", snd_strerror(rc));
		} else if (rc != buff_size) {
			fprintf(stderr, "short write: %d\n", rc);
		}
	}

	snd_pcm_close(pcm);
	return 0;
}
