#include <stdio.h>
#include <stdlib.h>
#include "../lib/linenoise.h"

#define PROMPT "$ "
#define HISTORY_LENGTH 1024

int main(void) {
	if (!linenoiseHistorySetMaxLen(HISTORY_LENGTH)) {
		fprintf(stderr, "Could not set linenoise history!\n");
		return 1;
	}

	char *line;
	while ((line = linenoise(PROMPT)) != NULL) {
		fprintf(stdout, "%s\n", line);
		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}

	return 0;
}
