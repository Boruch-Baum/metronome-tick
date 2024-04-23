#include "preset_test.h"
#include "utils.h"
#include "../lib/preset.h"
#include "minunit.h"
#include <stdlib.h>
#include <sys/stat.h>

void presets_equal(struct Presets *p1, struct Presets *p2) {
	mu_assert_int_eq(p1->size, p2->size);
	for (int i = 0; i < p1->size; i++) {
		mu_assert_string_eq(p1->items[i].name, p2->items[i].name);
		mu_assert_int_eq(p1->items[i].bpm, p2->items[i].bpm);
		mu_assert_string_eq(p1->items[i].rhythm, p2->items[i].rhythm);
	}
}

void preset_test_setup(void) {
	mkdir("/fakehome", 0775);
	mkdir("/fakehome/.local", 0775);
	mkdir("/fakehome/.local/share", 0775);
	mkdir("/fakehome/.local/share/tick", 0775);
	mkdir("/home/fakeuser", 0775);
	mkdir("/home/fakeuser/.local", 0775);
	mkdir("/home/fakeuser/.local/share", 0775);
	mkdir("/home/fakeuser/.local/share/tick", 0775);
}

void preset_test_teardown(void) {
	system("rm -r /fakehome /home/fakeuser");
}

MU_TEST(test_xdg_data_home) {
	write_file("/home/fakeuser/.local/share/tick/presets.ini", "[a]\n");
	setenv("XDG_DATA_HOME", "/home/fakeuser/.local/share", 1);
	struct Presets presets;
	get_presets(&presets);
	mu_assert_string_eq(presets.items[0].name, "a");
	free(presets.items);
}

MU_TEST(test_no_xdg_data_home) {
	write_file("/fakehome/.local/share/tick/presets.ini", "[b]\nbpm=1\nrhythm=.\n");
	setenv("HOME", "/fakehome", 1);
	unsetenv("XDG_DATA_HOME");
	struct Presets presets;
	get_presets(&presets);
	mu_assert_string_eq(presets.items[0].name, "b");
	free(presets.items);
}

MU_TEST(test_no_home) {
	unsetenv("HOME");
	unsetenv("XDG_DATA_HOME");
	struct Presets expected = {
		.items = malloc(sizeof(struct Preset)),
		.size = 0,
	};
	expected.items[0] = (struct Preset){
		.name = "Default",
		.bpm = 120,
		.rhythm = ">...",
	};
	struct Presets result;
	get_presets(&result);
	presets_equal(&expected, &result);
	free(expected.items);
	free(result.items);
}

MU_TEST(test_full_presets) {
	setenv("XDG_DATA_HOME", "/home/fakeuser/.local/share", 1);
	copy_file("/home/fakeuser/.local/share/tick/presets.ini", "test/data/presets.ini");
	struct Presets expected = {
		.items = malloc(sizeof(struct Preset) * 6),
		.size = 6,
	};
	expected.items[0] = (struct Preset){
		.name = "Ballade No. 1 in G minor, Op. 23 (Chopin) \"quote\"]",
		.bpm = 120,
		.rhythm = ">.....",
	};
	expected.items[1] = (struct Preset){
		.name = "Jazz swing",
		.bpm = 360,
		.rhythm = ">'.",
	};
	expected.items[2] = (struct Preset){
		.name = "Jazz 147",
		.bpm = 360,
		.rhythm = ">..>..>.",
	};
	expected.items[3] = (struct Preset){
		.name = "Latin",
		.bpm = 320,
		.rhythm = ">.>.>>.>.>..>.>.",
	};
	expected.items[4] = (struct Preset) {
		.name = "Too long",
		.bpm = 120,
		.rhythm = ">..>..>..>..>..>..>..>..>..>..>",
	};
	expected.items[5] = (struct Preset) {
		.name = "Just short enough",
		.bpm = 120,
		.rhythm = ">..>..>..>..>..>..>..>..>..>..>",
	};
	struct Presets result;
	get_presets(&result);
	presets_equal(&expected, &result);
	free(expected.items);
	free(result.items);
}

MU_TEST_SUITE(preset_test_suite) {
	MU_SUITE_CONFIGURE(&preset_test_setup, &preset_test_teardown);
	MU_RUN_TEST(test_xdg_data_home);
	MU_RUN_TEST(test_no_xdg_data_home);
	MU_RUN_TEST(test_no_home);
	MU_RUN_TEST(test_full_presets);
}

void invalid_preset_test_setup(void) {
	mkdir("/home/fakeuser", 0755);
	mkdir("/home/fakeuser/.local", 0755);
	mkdir("/home/fakeuser/.local/share", 0755);
	mkdir("/home/fakeuser/.local/share/tick", 0755);
}

void invalid_preset_test_teardown(void) {
	system("rm -r /home/fakeuser");
}

void fork_presets_callback(void) {
	struct Presets presets;
	get_presets(&presets);
}

MU_TEST(test_unmatched_bracket) {
	write_file("/home/fakeuser/.local/share/tick/presets.ini", "[preset name\n");
	char output[MAX_ERROR_LEN];
	int ret = fork_function(output, fork_presets_callback);
	mu_assert_int_eq(ret, 1);
	mu_assert_string_eq("Invalid configuration: unmatched '['\n", output);
}

MU_TEST(test_empty_preset_name) {
	write_file("/home/fakeuser/.local/share/tick/presets.ini", "[]\n");
	char output[MAX_ERROR_LEN];
	int ret = fork_function(output, fork_presets_callback);
	mu_assert_int_eq(ret, 1);
	mu_assert_string_eq("Invalid configuration: preset name must be non-empty\n", output);
}

MU_TEST(test_missing_equal_sign) {
	write_file("/home/fakeuser/.local/share/tick/presets.ini", "bpm 23\n");
	char output[MAX_ERROR_LEN];
	int ret = fork_function(output, fork_presets_callback);
	mu_assert_int_eq(ret, 1);
	mu_assert_string_eq("Invalid configuration: expect key=value\n", output);
}

MU_TEST(test_unrecognized_key) {
	write_file("/home/fakeuser/.local/share/tick/presets.ini", "up=23\n");
	char output[MAX_ERROR_LEN];
	int ret = fork_function(output, fork_presets_callback);
	mu_assert_int_eq(ret, 1);
	mu_assert_string_eq("Invalid configuration: unrecognized key 'up'\n", output);
}

MU_TEST_SUITE(invalid_preset_test_suite) {
	MU_SUITE_CONFIGURE(&invalid_preset_test_setup, &invalid_preset_test_teardown);
	MU_RUN_TEST(test_unmatched_bracket);
	MU_RUN_TEST(test_empty_preset_name);
	MU_RUN_TEST(test_missing_equal_sign);
	MU_RUN_TEST(test_unrecognized_key);
}

int preset_test_suites(void) {
	MU_RUN_SUITE(preset_test_suite);
	MU_RUN_SUITE(invalid_preset_test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
