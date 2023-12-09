#ifndef UTILS_H
#define UTILS_H

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#include "user.h"

#define MAX_PATH_LENGTH 1024
#define MAX_ARGS 256
#define PIPE_BUF 4096
#define MAX_PATH 256

/*
 * Given a command, search for it in the PATH environment variable.
 * If file exists and is executable, return the full path to the file.
 * Otherwise, return NULL.
 * The caller is responsible for freeing the memory allocated for the result.
 */
char *search_command(const char *command);
int exec_with_pipe(char **argv, char *file, int bg, User* user);

void exec_command(char **argv, int background, char *std_in, char *std_out,
                  char *std_err, int mode, User* user);

#endif
