#include "metronome.h"
#include <pthread.h>

void apply_preset(struct Metronome *m) {
	m->ps.bpm = m->presets.items[m->preset_index].bpm;
	memcpy(m->ps.pattern, m->presets.items[m->preset_index].pattern, MAX_PATTERN_LEN);
}

void init_metronome(struct Metronome *m) {
	get_config(&m->config);
	get_presets(&m->presets);
	m->ps = (struct PlayerState){
		.playing = 0,
		.freq_accented = m->config.freq_accented,
		.freq_general = m->config.freq_general,
	};
	m->preset_index = 0;
	apply_preset(m);

	snd_pcm_t *pcm;
	prepare_player(&pcm);
	m->pa = (struct PlayerArgs){.pcm = pcm};

	display_keybinds(&m->config);
	display_player_state(m);
}

void start_metronome(struct Metronome *m) {
	if (m->ps.playing) {
		pthread_cancel(m->tid);
	}
	if (m->ps.bpm <= 0) {
		printf("\nBPM cannot be <= 0\n");
	} else {
		if (m->pa.buffer) {
			free(m->pa.buffer);
		}
		m->pa.buffer = create_waves(&m->pa.buff_size, &m->ps);
		pthread_create(&m->tid, NULL, start_player, &m->pa);
		m->ps.playing = 1;
	}
}

void stop_metronome(struct Metronome *m) {
	pthread_cancel(m->tid);
	m->ps.playing = 0;
}

void set_bpm(struct Metronome *m, int bpm) {
	m->ps.bpm = bpm;
	display_player_state(m);
	if (m->ps.playing) {
		start_metronome(m);
	}
}

void set_pattern(struct Metronome *m, char *pattern) {
	memcpy(m->ps.pattern, pattern, MAX_PATTERN_LEN);
	display_player_state(m);
	if (m->ps.playing) {
		start_metronome(m);
	}
}

void set_preset(struct Metronome *m, int preset_index) {
	m->preset_index = preset_index;
	apply_preset(m);
	display_player_state(m);
	if (m->ps.playing) {
		start_metronome(m);
	}
}

void display_player_state(struct Metronome *m) {
	printf("\33[2K\r"); // https://stackoverflow.com/a/35190285/10254049
	printf("\033[1m(%s)\033[0m %s @ %d", m->presets.items[m->preset_index], m->ps.pattern, m->ps.bpm);
	fflush(stdout);
}
