#include "files.h"
#include <limits.h>
#include <stdlib.h>

FILE *read_xdg_file(char *xdg_dir, char *default_dir, char *subpath) {
	char path[PATH_MAX];
	const char *dir = getenv(xdg_dir);
	if (dir == NULL || dir[0] == '\0') {
		const char *HOME = getenv("HOME");
		if (HOME == NULL || HOME[0] == '\0') {
			return NULL;
		}
		snprintf(path, PATH_MAX, "%s/%s/%s", HOME, default_dir, subpath);
	} else {
		snprintf(path, PATH_MAX, "%s/%s", dir, subpath);
	}
	return fopen(path, "r");
}
