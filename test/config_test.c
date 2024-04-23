#include "config_test.h"
#include "utils.h"
#include "../lib/config.h"
#include "minunit.h"
#include <stdlib.h>
#include <sys/stat.h>

void configs_equal(struct Config *c1, struct Config *c2) {
	mu_assert_int_eq(c1->freq_accented, c2->freq_accented);
	mu_assert_int_eq(c1->freq_general, c2->freq_general);
	mu_assert_int_eq(c1->interval, c2->interval);
	mu_assert_int_eq(c1->keys.up, c2->keys.up);
	mu_assert_int_eq(c1->keys.down, c2->keys.down);
	mu_assert_int_eq(c1->keys.next, c2->keys.next);
	mu_assert_int_eq(c1->keys.prev, c2->keys.prev);
	mu_assert_int_eq(c1->keys.toggle_play, c2->keys.toggle_play);
	mu_assert_int_eq(c1->keys.save, c2->keys.save);
	mu_assert_int_eq(c1->keys.rename, c2->keys.rename);
	mu_assert_int_eq(c1->keys.create, c2->keys.create);
	mu_assert_int_eq(c1->keys.delete, c2->keys.delete);
	mu_assert_int_eq(c1->keys.open_prompt, c2->keys.open_prompt);
	mu_assert_int_eq(c1->keys.quit, c2->keys.quit);
}

void config_test_setup(void) {
	mkdir(".test_data", 0775);
	mkdir(".test_data/.config", 0775);
	mkdir(".test_data/.config/tick", 0775);
	mkdir(".test_data/fakeuser", 0775);
	mkdir(".test_data/fakeuser/.config", 0775);
	mkdir(".test_data/fakeuser/.config/tick", 0775);
}

void config_test_teardown(void) {
	system("rm -r .test_data");
}

MU_TEST(test_xdg_config_home) {
	write_file(".test_data/fakeuser/.config/tick/tick.ini", "freq>=2\n");
	setenv("XDG_CONFIG_HOME", ".test_data/fakeuser/.config", 1);
	struct Config config;
	get_config(&config);
	mu_assert_int_eq(2, config.freq_accented);
}

MU_TEST(test_no_xdg_config_home) {
	write_file(".test_data/.config/tick/tick.ini", "freq>=1\n");
	setenv("HOME", ".test_data", 1);
	unsetenv("XDG_CONFIG_HOME");
	struct Config config;
	get_config(&config);
	mu_assert_int_eq(1, config.freq_accented);
}

MU_TEST(test_no_home) {
	unsetenv("HOME");
	unsetenv("XDG_CONFIG_HOME");
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
			.save = 's',
			.rename = 'r',
			.create = 'c',
			.delete = 'd',
			.open_prompt = ':',
			.quit = 'q',
		},
	};
	struct Config result;
	get_config(&result);
	configs_equal(&expected, &result);
}

MU_TEST(test_full_config) {
	setenv("XDG_CONFIG_HOME", ".test_data/fakeuser/.config", 1);
	copy_file(".test_data/fakeuser/.config/tick/tick.ini", "test/data/tick.ini");
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
			.save = 'f',
			.rename = 'g',
			.create = 'h',
			.delete = 'i',
			.open_prompt = ';',
			.quit = 'w',
		},
	};
	struct Config result;
	get_config(&result);
	configs_equal(&expected, &result);
}

MU_TEST_SUITE(config_test_suite) {
	MU_SUITE_CONFIGURE(&config_test_setup, &config_test_teardown);
	MU_RUN_TEST(test_xdg_config_home);
	MU_RUN_TEST(test_no_xdg_config_home);
	MU_RUN_TEST(test_no_home);
	MU_RUN_TEST(test_full_config);
}

void invalid_config_test_setup(void) {
	mkdir(".test_data", 0755);
	mkdir(".test_data/fakeuser", 0755);
	mkdir(".test_data/fakeuser/.config", 0755);
	mkdir(".test_data/fakeuser/.config/tick", 0755);
}

void fork_config_callback(void) {
	struct Config config;
	get_config(&config);
}

MU_TEST(test_missing_equal_sign) {
	write_file(".test_data/fakeuser/.config/tick/tick.ini", "freq> 23\n");
	char output[MAX_ERROR_LEN];
	int ret = fork_function(output, fork_config_callback);
	mu_assert_int_eq(1, ret);
	mu_assert_string_eq("Invalid configuration: expect key=value\n", output);
}

MU_TEST(test_unrecognized_key) {
	write_file(".test_data/fakeuser/.config/tick/tick.ini", "freq=23\n");
	char output[MAX_ERROR_LEN];
	int ret = fork_function(output, fork_config_callback);
	mu_assert_int_eq(1, ret);
	mu_assert_string_eq("Invalid configuration: unrecognized key 'freq'\n", output);
}

MU_TEST_SUITE(invalid_config_test_suite) {
	MU_SUITE_CONFIGURE(&invalid_config_test_setup, &config_test_teardown);
	MU_RUN_TEST(test_missing_equal_sign);
	MU_RUN_TEST(test_unrecognized_key);
}

int config_test_suites(void) {
	MU_RUN_SUITE(config_test_suite);
	MU_RUN_SUITE(invalid_config_test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
