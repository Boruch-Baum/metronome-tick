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
	} else if (strcmp(str, "delete\n") == 0) {
		return DELETE_KEY_SUBSTITUTION;
	} else if (strcmp(str, "backspace\n") == 0) {
		return BACKSPACE_KEY;
	} else if (strcmp(str, "tab\n") == 0) {
		return TAB_KEY;
	} else if (strcmp(str, "enter\n") == 0) {
		return ENTER_KEY;
	} else if (strcmp(str, "space\n") == 0) {
		return SPACE_KEY;
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
	case DELETE_KEY_SUBSTITUTION:
		strcpy(dst, "delete");
		break;
	case BACKSPACE_KEY:
		strcpy(dst, "backspace");
		break;
	case TAB_KEY:
		strcpy(dst, "tab");
		break;
	case ENTER_KEY:
		strcpy(dst, "enter");
		break;
	case SPACE_KEY:
		strcpy(dst, "space");
		break;
	default:
		dst[0] = c;
		dst[1] = '\0';
	}
}

