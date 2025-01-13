#include "files.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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

int mkpath(const char* file_path) {
	if (!file_path || !*file_path) {
		errno = EINVAL;
		return -1;
	}

	char *path_copy = strndup(file_path, PATH_MAX);
	if (!path_copy) {
		return -1;
	}

	for (char *p = strchr(path_copy+1, '/'); p; p = strchr(p+1, '/')) {
		*p = '\0';
		if (mkdir(path_copy, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
			if (errno != EEXIST) {
				free(path_copy);
				return -1;
			}
		}
		*p = '/';
	}

	free(path_copy);
	return 0;
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
