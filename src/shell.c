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
#define PATH_MAX 4096

char CWD[PATH_MAX];

typedef enum Builtin {
	CD,
	PWD,
	INVALID
} Builtin;

void builtin_cd(char **args, size_t n_args);
void builtin_pwd(char **args, size_t n_args);
int is_builtin(char *cmd);
void refresh_cwd(void);
int s_read(char *input, char **args);
int s_execute(char *cmd, char **args);
void s_execute_builtin(Builtin code, char **args, size_t n_args);

void (*BUILTIN_TABLE[]) (char **args, size_t n_args) = {
	[CD] = builtin_cd,
	[PWD] = builtin_pwd,
};

Builtin builtin_code(char *cmd) {
	if (!strncmp(cmd, "cd", 2)) return CD;
	else if (!strncmp(cmd, "pwd", 3)) return PWD;
	else return INVALID;
}

void builtin_cd(char **args, size_t n_args) {
	char *new_dir = *(args + 1);
	if (chdir(new_dir) != 0) {
		fprintf(stderr, "Could not change directory!\n");
		return;
	}
	refresh_cwd();
}

void builtin_pwd(char **args, size_t n_args) {
	fprintf(stdout, "%s\n", CWD);
}

int is_builtin(char *cmd) {
	return builtin_code(cmd) != INVALID;
}

void refresh_cwd(void) {
	if (getcwd(CWD, sizeof(CWD)) == NULL) {
		fprintf(stderr, "Could not read working directory!\n");
	}
}

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

void s_execute_builtin(Builtin code, char **args, size_t n_args) {
	BUILTIN_TABLE[code](args, n_args);
}

int main(void) {
	refresh_cwd();
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
		Builtin cmd_builtin_code = builtin_code(cmd);
		if (cmd_builtin_code == INVALID) {
			s_execute(cmd, args);
		} else {
			s_execute_builtin(cmd_builtin_code, args, args_read);
		}

		linenoiseHistoryAdd(line);
		linenoiseFree(line);
	}

	return 0;
}
