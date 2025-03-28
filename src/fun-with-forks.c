#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(void) {
	fprintf(stdout, "This is executed from the parent process\n");
	clock_t starttime = clock();
	int pid = fork();
	if (pid == 0) {
		fprintf(stdout, "This is executed from the child process\n");
		printf("Time elapsed: %lu\n", (clock() - starttime));
	} else {
		fprintf(stdout, "This is again executed from the parent process\n");
		printf("Time elapsed: %lu\n", (clock() - starttime));
	}
	return 0;
}
