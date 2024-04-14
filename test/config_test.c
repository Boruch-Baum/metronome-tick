#include "config_test.h"
#include "minunit.h"
#include "../lib/config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_ERROR_LEN (MAX_LINE_LEN + 19 + 24) // error len + 24 from "Invalid configuration: \n"

void configs_equal(struct Config c1, struct Config c2) {
	mu_assert_int_eq(c1.freq_accented, c2.freq_accented);
	mu_assert_int_eq(c1.freq_general, c2.freq_general);
	mu_assert_int_eq(c1.interval, c2.interval);
	mu_assert_int_eq(c1.keys.up, c2.keys.up);
	mu_assert_int_eq(c1.keys.down, c2.keys.down);
	mu_assert_int_eq(c1.keys.next, c2.keys.next);
	mu_assert_int_eq(c1.keys.prev, c2.keys.prev);
	mu_assert_int_eq(c1.keys.toggle_play, c2.keys.toggle_play);
	mu_assert_int_eq(c1.keys.show_prompt, c2.keys.show_prompt);
	mu_assert_int_eq(c1.keys.quit, c2.keys.quit);
	mu_assert_int_eq(c1.presets_size, c2.presets_size);
	for (int i = 0; i < c1.presets_size; i++) {
		mu_assert_int_eq(c1.presets[i].bpm, c2.presets[i].bpm);
		mu_assert_string_eq(c1.presets[i].name, c2.presets[i].name);
		mu_assert_string_eq(c1.presets[i].pattern, c2.presets[i].pattern);
	}
}

void config_test_setup(void) {
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

void config_test_teardown(void) {
	system("rm -r /fakehome /home/fakeuser");
}

void invalid_config_test_setup(void) {
	mkdir("/home/fakeuser", 0755);
	mkdir("/home/fakeuser/.config", 0755);
	mkdir("/home/fakeuser/.config/tick", 0755);
}

void invalid_config_test_teardown(void) {
	system("rm -r /home/fakeuser");
}

void _test_default_config(void) {
	struct Config expected = {
		.freq_accented = 587,
		.freq_general = 440,
		.interval = 20,
		.keys = {
			.up = 'k',
			.down = 'j',
			.next = 'l',
			.prev = 'h',
			.toggle_play = ' ',
			.show_prompt = ':',
			.quit = 'q',
		},
		.presets_size = 1,
		.presets = malloc(sizeof(struct Preset)),
	};
	expected.presets[0] = (struct Preset){
		.name = "Default",
		.bpm = 120,
		.pattern = ">...",
	};
	struct Config result;
	get_config(&result);
	configs_equal(expected, result);
	free(expected.presets);
	free(result.presets);
}

MU_TEST(test_default_config) {
	_test_default_config();
}

MU_TEST_SUITE(default_config_test_suite) {
	MU_RUN_TEST(test_default_config);
}

MU_TEST(test_xdg_config_home) {
	setenv("XDG_CONFIG_HOME", "/home/fakeuser/.config", 1);
	struct Config config;
	get_config(&config);
	mu_assert_int_eq(config.freq_accented, 2);
	free(config.presets);
}

MU_TEST(test_no_xdg_config_home) {
	setenv("HOME", "/fakehome", 1);
	unsetenv("XDG_CONFIG_HOME");
	struct Config config;
	get_config(&config);
	mu_assert_int_eq(config.freq_accented, 1);
	free(config.presets);
}

MU_TEST(test_no_home) {
	unsetenv("HOME");
	unsetenv("XDG_CONFIG_HOME");
	_test_default_config();
}

MU_TEST(test_full_config) {
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

	struct Config expected = {
		.freq_accented = 2,
		.freq_general = 3,
		.interval = 4,
		.keys = {
			.up = 'a',
			.down = 'b',
			.next = 'c',
			.prev = 'd',
			.toggle_play = 'e',
			.show_prompt = ';',
			.quit = 'w',
		},
		.presets_size = 6,
		.presets = malloc(sizeof(struct Preset) * 6),
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
	expected.presets[4] = (struct Preset) {
		.name = "Too long",
		.bpm = 120,
		.pattern = ">..>..>..>..>..>..>..>..>..>..>",
	};
	expected.presets[5] = (struct Preset) {
		.name = "Just short enough",
		.bpm = 120,
		.pattern = ">..>..>..>..>..>..>..>..>..>..>",
	};
	struct Config result;
	get_config(&result);
	configs_equal(expected, result);
	free(expected.presets);
	free(result.presets);
}

MU_TEST_SUITE(config_test_suite) {
	MU_SUITE_CONFIGURE(&config_test_setup, &config_test_teardown);
	MU_RUN_TEST(test_xdg_config_home);
	MU_RUN_TEST(test_no_xdg_config_home);
	MU_RUN_TEST(test_no_home);
	MU_RUN_TEST(test_full_config);
}

void fork_get_config(char *output) {
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[1]);
		struct Config config;
		get_config(&config);
	} else {
		close(pipefd[1]);
		int status;
		waitpid(pid, &status, 0);
		mu_assert(WIFEXITED(status), "get_config exited normally");
		mu_assert_int_eq(WEXITSTATUS(status), 1);
		read(pipefd[0], output, MAX_ERROR_LEN);
	}
}

void write_file(char *path, char *str) {
	FILE *file = fopen(path, "w");
	assert(file != NULL);
	fprintf(file, "%s", str);
	fclose(file);
}

MU_TEST(test_unmatched_bracket) {
	write_file("/home/fakeuser/.config/tick/tick.ini", "[preset name\n");
	char output[MAX_ERROR_LEN];
	fork_get_config(output);
	mu_assert_string_eq("Invalid configuration: unmatched '['\n", output);
}

MU_TEST(test_empty_preset_name) {
	write_file("/home/fakeuser/.config/tick/tick.ini", "[]\n");
	char output[MAX_ERROR_LEN];
	fork_get_config(output);
	mu_assert_string_eq("Invalid configuration: preset name must be non-empty\n", output);
}

MU_TEST(test_missing_equal_sign) {
	write_file("/home/fakeuser/.config/tick/tick.ini", "freq> 23\n");
	char output[MAX_ERROR_LEN];
	fork_get_config(output);
	mu_assert_string_eq("Invalid configuration: expect key=value\n", output);
}

MU_TEST(test_unrecognized_key) {
	write_file("/home/fakeuser/.config/tick/tick.ini", "freq=23\n");
	char output[MAX_ERROR_LEN];
	fork_get_config(output);
	mu_assert_string_eq("Invalid configuration: unrecognized key 'freq'\n", output);
}

MU_TEST_SUITE(invalid_config_test_suite) {
	MU_SUITE_CONFIGURE(&invalid_config_test_setup, &invalid_config_test_teardown);
	MU_RUN_TEST(test_unmatched_bracket);
	MU_RUN_TEST(test_empty_preset_name);
	MU_RUN_TEST(test_missing_equal_sign);
	MU_RUN_TEST(test_unrecognized_key);
}

int config_test_suites(void) {
	MU_RUN_SUITE(default_config_test_suite);
	MU_RUN_SUITE(config_test_suite);
	MU_RUN_SUITE(invalid_config_test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
