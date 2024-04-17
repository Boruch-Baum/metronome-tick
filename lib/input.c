#include "input.h"
#include "keys.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define ESCAPE_SEQUENCE 27

struct termios original_termios;

void reset_terminal_mode(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void prepare_for_input(void) {
	tcgetattr(STDIN_FILENO, &original_termios);
	atexit(reset_terminal_mode);
	struct termios tui_termios = original_termios;
	tui_termios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tui_termios);
}

int get_input(void) {
	int c = getchar();
	if (c == ESCAPE_SEQUENCE) { // https://stackoverflow.com/a/15306494/10254049
		getchar();
		c = getchar();
		switch (c) {
		case 'A':
			return UP_ARROW_SUBSTITUTION;
		case 'B':
			return DOWN_ARROW_SUBSTITUTION;
		case 'C':
			return RIGHT_ARROW_SUBSTITUTION;
		case 'D':
			return LEFT_ARROW_SUBSTITUTION;
		case '3':
			c = getchar();
			if (c == '~') {
				return DELETE_KEY_SUBSTITUTION;
			} else {
				return c;
			}
		default:
			return c;
		}
	} else {
		return c;
	}
}

void get_command(char *line, char *prompt) {
	printf("\n%s", prompt);
	fflush(stdout);
	reset_terminal_mode();
	fgets(line, sizeof(line), stdin);
	printf("\033[A\33[2K\033[A\33[2K"); // https://stackoverflow.com/a/35190285/10254049
	prepare_for_input();
	// TODO handle SIGINT
}
