#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

#define MAX_TOKENS 64
#define MAX_TOKEN_LENGTH 256
#define DELIMITERS " \t\r\n\a"

// Function declarations
void executeCommands(char *command);
char** tokenizeCommand(char *command, int *token_count);
FILE* checkRedirection(char **tokens, int *token_count);
void freeTokens(char **tokens, int token_count);

#endif