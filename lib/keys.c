#include "keys.h"
#include <string.h>

char str_to_key(char *str) {
	if (strcmp(str, "up\n") == 0) {
		return UP_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "down\n") == 0) {
		return DOWN_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "right\n") == 0) {
		return RIGHT_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "left\n") == 0) {
		return LEFT_ARROW_SUBSTITUTION;
	} else if (strcmp(str, "tab\n") == 0) {
		return 9;
	} else if (strcmp(str, "enter\n") == 0) {
		return 13;
	} else if (strcmp(str, "space\n") == 0) {
		return 32;
	} else {
		return str[0];
	}
}

void key_to_str(char *dst, char c) {
	switch (c) {
	case UP_ARROW_SUBSTITUTION:
		strcpy(dst, "up arrow");
		break;
	case DOWN_ARROW_SUBSTITUTION:
		strcpy(dst, "down arrow");
		break;
	case RIGHT_ARROW_SUBSTITUTION:
		strcpy(dst, "right arrow");
		break;
	case LEFT_ARROW_SUBSTITUTION:
		strcpy(dst, "left arrow");
		break;
	case 13:
		strcpy(dst, "enter");
		break;
	case 32:
		strcpy(dst, "space");
		break;
	default:
		dst[0] = c;
		dst[1] = '\0';
	}
}

