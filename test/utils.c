#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void write_file(char *path, char *str) {
	FILE *file = fopen(path, "w");
	assert(file != NULL);
	fputs(str, file);
	fclose(file);
}

void copy_file(char *dst, char *src) {
	FILE *fsrc = fopen(src, "r");
	FILE *fdst = fopen(dst, "w");
	assert(fsrc != NULL && fdst != NULL);
	char buff[1024];
	size_t b_read;
	while ((b_read = fread(buff, 1, 1024, fsrc)) > 0) {
		fwrite(buff, 1, b_read, fdst);
	}
	fclose(fsrc);
	fclose(fdst);
}

int fork_function(char *output, void (*func)(void)) {
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[1]);
		func();
		return EXIT_SUCCESS;
	} else {
		close(pipefd[1]);
		int status;
		waitpid(pid, &status, 0);
		assert(WIFEXITED(status));
		read(pipefd[0], output, MAX_ERROR_LEN);
		return WEXITSTATUS(status);
	}
}
