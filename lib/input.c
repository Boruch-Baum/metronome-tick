#include "input.h"
#include "keys.h"
#include <stdarg.h>
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

void get_command(char *line, int max_line_size, char *prompt) {
	printf("\n%s", prompt);
	fflush(stdout);
	int c;
	int i = 0;
	int len = 0;
	while (1) {
		c = get_input();
		// https://stackoverflow.com/a/26423857/10254049
		if (c == ENTER_KEY) {
			break;
		} else if (c == RIGHT_ARROW_SUBSTITUTION) {
			if (i < len) {
				i += 1;
				printf("\033[1C");
				fflush(stdout);
			}
		} else if (c == LEFT_ARROW_SUBSTITUTION) {
			if (i > 0) {
				i -= 1;
				printf("\033[1D");
				fflush(stdout);
			}
		} else if (c == BACKSPACE_KEY) {
			if (i > 0) {
				for (int j = i - 1; j < len - 1; j++) {
					line[j] = line[j+1];
				}
				i -= 1;
				len -= 1;
				line[len] = '\0';
				printf("\033[1D");
				printf("\033[K");
				fputs(line+i, stdout);
				if (len != i) { // "\033[0D" moves the cursor 1 to the right instead of 0
					printf("\033[%dD", len-i);
				}
				fflush(stdout);
			}
		} else if (c == DELETE_KEY_SUBSTITUTION) {
			if (i < len) {
				for (int j = i; j < len - 1; j++) {
					line[j] = line[j+1];
				}
				len -= 1;
				line[len] = '\0';
				printf("\033[K");
				fputs(line+i, stdout);
				if (len != i) {
					printf("\033[%dD", len-i);
				}
				fflush(stdout);
			}
		} else if (len < max_line_size - 1) {
			for (int j = len; j > i; j--) {
				line[j] = line[j-1];
			}
			line[i] = c;
			i += 1;
			len += 1;
			line[len] = '\0';
			line[max_line_size-1] = '\0';
			printf("\033[K");
			fputs(line+i-1, stdout);
			if (len != i) {
				printf("\033[%dD", len-i);
			}
			fflush(stdout);
		}
	}
	printf("\33[2K\033[A\33[2K");
	// TODO handle SIGINT
}

void print_error(const char *restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	printf("\n");
	vprintf(fmt, ap);
	va_end(ap);
	fflush(stdout);
	printf("\33[2K\033[A\33[2K"); // purposely not flushing until next flush
}
