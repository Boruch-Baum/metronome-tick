#include "metronome.h"
#include "input.h"
#include <pthread.h>

static void apply_preset(struct Metronome *m) {
	m->ps.bpm = m->presets.items[m->preset_index].bpm;
	memcpy(m->ps.rhythm, m->presets.items[m->preset_index].rhythm, MAX_RHYTHM_LEN);
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
		if (m->pa.buffer == NULL) {
			print_error("Failed to create audio buffer");
			m->ps.playing = 0;
			display_player_state(m);
			return;
		}
		pthread_create(&m->tid, NULL, start_player, &m->pa);
		m->ps.playing = 1;
		display_player_state(m);
	}
}

void stop_metronome(struct Metronome *m) {
	pthread_cancel(m->tid);
	m->ps.playing = 0;
	display_player_state(m);
}

void set_bpm(struct Metronome *m, int bpm) {
	m->ps.bpm = bpm < 0 ? 0 : bpm;
	display_player_state(m);
	if (m->ps.playing) {
		start_metronome(m);
	}
}

void set_rhythm(struct Metronome *m, char *rhythm) {
	memcpy(m->ps.rhythm, rhythm, MAX_RHYTHM_LEN);
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
	char *unsaved_prefix = "";
	if (m->presets.size == 0 ||
			m->ps.bpm != m->presets.items[m->preset_index].bpm ||
			strcmp(m->ps.rhythm, m->presets.items[m->preset_index].rhythm) != 0) {
		unsaved_prefix = "*\033[3m";
	}
	printf("%s\033[1m(%s)\033[0m %s @ %d", unsaved_prefix, m->presets.items[m->preset_index].name, m->ps.rhythm, m->ps.bpm);
	if (!m->ps.playing) {
		printf(" (paused)");
	}
	fflush(stdout);
}
