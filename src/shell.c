#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../lib/linenoise.h"

#define PROMPT "$ "
#define HISTORY_LENGTH 1024
#define MAX_ARGS 1024
#define TOKEN_SEPARATORS " \t"

int s_read(char *input, char **args);
int s_execute(char *cmd, char **args);

int s_read(char *input, char **args) {
	int i = 0;
	char *token = strtok(input, TOKEN_SEPARATORS);
	while (token != NULL && i < (MAX_ARGS - 1)) {
		args[i++] = token;
		token = strtok(NULL, TOKEN_SEPARATORS);
	}
	args[i] = NULL;
	return i;
}

int s_execute(char *cmd, char **args) {
	fprintf(stdout, "Executing command: %s\n", cmd);

	int status;
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Could not execute command!\n");
		return -1;
	}

	if (pid == 0) {
		execvp(cmd, args);
	} else if (waitpid(pid, &status, 0) != pid) {
		fprintf(stderr, "Could not wait for kiddo!\n");
		return -1;
	}

	return status;
}

int main(void) {
	if (!linenoiseHistorySetMaxLen(HISTORY_LENGTH)) {
		fprintf(stderr, "Could not set linenoise history!\n");
		return 1;
	}

	char *line;
	char *args[MAX_ARGS];
	while ((line = linenoise(PROMPT)) != NULL) {
		// READ
		int args_read = s_read(line, args);
		fprintf(stdout, "Read %d args \n", args_read);

		for (int i = 0; i < args_read; ++i) {
			fprintf(stdout, "arg[%d] = %s\n", i, args[i]);
		}

		if(args_read == 0) {
			linenoiseFree(line);
			continue;
		}

		// TODO: EVAL + PRINT
		char *cmd = args[0];
		s_execute(cmd, args);

		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}

	return 0;
}
