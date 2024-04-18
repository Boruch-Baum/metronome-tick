#include "files.h"
#include <limits.h>
#include <stdlib.h>

void get_xdg_path(char *path, char *xdg_dir, char *default_dir, char *subpath) {
	const char *dir = getenv(xdg_dir);
	if (dir == NULL || dir[0] == '\0') {
		const char *HOME = getenv("HOME");
		if (HOME == NULL || HOME[0] == '\0') {
			path = NULL;
			return;
		}
		snprintf(path, PATH_MAX, "%s/%s/%s", HOME, default_dir, subpath);
	} else {
		snprintf(path, PATH_MAX, "%s/%s", dir, subpath);
	}
}

FILE *read_xdg_file(char *xdg_dir, char *default_dir, char *subpath) {
	char path[PATH_MAX];
	get_xdg_path(path, xdg_dir, default_dir, subpath);
	return fopen(path, "r");
}

void append_xdg_file(char *xdg_dir, char *default_dir, char *subpath, char *data) {
	char path[PATH_MAX];
	get_xdg_path(path, xdg_dir, default_dir, subpath);
	FILE *file = fopen(path, "a");
	if (file == NULL) {
		exit(EXIT_FAILURE);
	}
	fprintf(file, "%s", data);
	fclose(file);
}
