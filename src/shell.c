#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/linenoise.h"

#define PROMPT "$ "
#define HISTORY_LENGTH 1024
#define MAX_ARGS 1024
#define TOKEN_SEPARATORS " \t"

int s_read(char *input, char **args);

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

		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}

	return 0;
}
