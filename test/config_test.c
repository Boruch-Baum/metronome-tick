#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include "../lib/config.h"
#include "config_test.h"

int configs_equal(struct Config c1, struct Config c2) {
	if (c1.freq_accented != c2.freq_accented ||
		c1.freq_general != c2.freq_general ||
		c1.interval != c2.interval ||
		c1.keys.up != c2.keys.up ||
		c1.keys.down != c2.keys.down ||
		c1.keys.play_pause != c2.keys.play_pause ||
		c1.keys.show_prompt != c2.keys.show_prompt ||
		c1.presets_size != c2.presets_size) {
		return 0;
	}
	for (int i = 0; i < c1.presets_size; i++) {
		if (c1.presets[i].bpm != c2.presets[i].bpm ||
				strcmp(c1.presets[i].name, c2.presets[i].name) != 0 ||
				strcmp(c1.presets[i].pattern, c2.presets[i].pattern) != 0) {
			return 0;
		}
	}
	return 1;
}

int _test_default_config(void) {
	struct Config actual = get_config();
	struct Config expected = {
		.freq_accented = 587,
		.freq_general = 440,
		.interval = 20,
		.keys = {
			.up = 'k',
			.down = 'j',
			.play_pause = ' ',
			.show_prompt = ':',
		},
		.presets_size = 1,
		.presets = malloc(sizeof(struct Preset)),
	};
	expected.presets[0] = (struct Preset){
		.name = "default",
		.bpm = 120,
		.pattern = ">...",
	};
	int pass = configs_equal(actual, expected);
	free(actual.presets);
	free(expected.presets);
	return pass;
}

int test_default_config(void) {
	printf("Running %s... ", __func__);
	int pass = _test_default_config();
	if (pass) {
		printf("Passed 😊\n");
	}
	return !pass;
}

void setup(void) {
	mkdir("/fakehome", 0775);
	mkdir("/fakehome/.config", 0775);
	mkdir("/fakehome/.config/tick", 0775);
	mkdir("/home/fakeuser", 0775);
	mkdir("/home/fakeuser/.config", 0775);
	mkdir("/home/fakeuser/.config/tick", 0775);

	FILE *file = fopen("/fakehome/.config/tick/tick.ini", "w");
	assert(file != NULL);
	fprintf(file, "freq>=1\n");
	fclose(file);

	file = fopen("/home/fakeuser/.config/tick/tick.ini", "w");
	assert(file != NULL);
	fprintf(file, "freq>=2\n");
	fclose(file);
}

void teardown(void) {
	system("rm -r /fakehome /home/fakeuser");
}

int test_xdg_config_home(void) {
	printf("Running %s... ", __func__);
	setenv("XDG_CONFIG_HOME", "/home/fakeuser/.config", 1);
	struct Config config = get_config();
	int pass = config.freq_accented == 2;
	free(config.presets);
	if (pass) {
		printf("Passed 😊\n");
	}
	return !pass;
}

int test_no_xdg_config_home(void) {
	printf("Running %s... ", __func__);
	setenv("HOME", "/fakehome", 1);
	unsetenv("XDG_CONFIG_HOME");
	struct Config config = get_config();
	int pass = config.freq_accented == 1;
	free(config.presets);
	if (pass) {
		printf("Passed 😊\n");
	}
	return !pass;
}

int test_no_home(void) {
	printf("Running %s... ", __func__);
	unsetenv("HOME");
	unsetenv("XDG_CONFIG_HOME");
	int pass = _test_default_config();
	if (pass) {
		printf("Passed 😊\n");
	}
	return test_default_config();
}

int test_full_config(void) {
	printf("Running %s... ", __func__);
	setenv("XDG_CONFIG_HOME", "/home/fakeuser/.config", 1);
	FILE *src = fopen("test/data/tick.ini", "r");
	FILE *dst = fopen("/home/fakeuser/.config/tick/tick.ini", "w");
	assert(src != NULL && dst != NULL);
	char buff[1024];
	size_t b_read;
	while ((b_read = fread(buff, 1, 1024, src)) > 0) {
		fwrite(buff, 1, b_read, dst);
	}
	fclose(src);
	fclose(dst);

	struct Config actual = get_config();
	struct Config expected = {
		.freq_accented = 2,
		.freq_general = 3,
		.interval = 4,
		.keys = {
			.up = 'a',
			.down = 'b',
			.play_pause = 'c',
			.show_prompt = ';',
		},
		.presets_size = 4,
		.presets = malloc(sizeof(struct Preset) * 4),
	};
	expected.presets[0] = (struct Preset){
		.name = "Ballade No. 1 in G minor, Op. 23 (Chopin) \"quote\"",
		.bpm = 120,
		.pattern = ">.....",
	};
	expected.presets[1] = (struct Preset){
		.name = "Jazz swing",
		.bpm = 360,
		.pattern = ">'.",
	};
	expected.presets[2] = (struct Preset){
		.name = "Jazz 147",
		.bpm = 360,
		.pattern = ">..>..>.",
	};
	expected.presets[3] = (struct Preset){
		.name = "Latin",
		.bpm = 320,
		.pattern = ">.>.>>.>.>..>.>.",
	};
	int pass = configs_equal(actual, expected);
	free(actual.presets);
	free(expected.presets);
	if (pass) {
		printf("Passed 😊\n");
	}
	return !pass;
}

int test_config(void) {
	int rc = test_default_config();
	setup();
	rc = test_xdg_config_home() ||
		test_no_xdg_config_home() ||
		test_no_home() ||
		test_full_config();
	teardown();
	return rc;
}
