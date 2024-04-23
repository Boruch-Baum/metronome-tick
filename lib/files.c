#include "files.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void get_xdg_path(char *path, char *xdg_dir, char *default_dir, char *subpath) {
	const char *dir = getenv(xdg_dir);
	if (dir == NULL || dir[0] == '\0') {
		const char *HOME = getenv("HOME");
		if (HOME == NULL || HOME[0] == '\0') {
			*path = '\0';
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

int mktemp_in_tmpdir(char *template) {
	char temp_dir[PATH_MAX];
	const char *TMPDIR = getenv("TMPDIR");
	if (TMPDIR == NULL || TMPDIR[0] == '\0') {
		strcpy(temp_dir, "/tmp");
	} else {
		strcpy(temp_dir, TMPDIR);
	}

	int dir_len = strlen(temp_dir);
	int temp_len = strlen(template);
	memmove(template+dir_len+1, template, temp_len);
	template[dir_len] = '/';
	memcpy(template, temp_dir, dir_len);
	strcpy(template+dir_len+1+temp_len, "-XXXXXX");
	return mkstemp(template);
}

void rename_file(char *old_path, char *new_path) {
	if (rename(old_path, new_path) == -1) {
		if (errno == EXDEV) {
			FILE *new_file = fopen(new_path, "w");
			FILE *old_file = fopen(old_path, "r");
			char line[MAX_LINE_LEN];
			while (fgets(line, MAX_LINE_LEN, old_file) != NULL) {
				fputs(line, new_file);
			}
			fclose(new_file);
			fclose(old_file);
			unlink(old_path);
		}
	}
}
