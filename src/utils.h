#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_PATH_LENGTH 1024

/*
 * Given a command, search for it in the PATH environment variable.
 * If file exists and is executable, return the full path to the file.
 * Otherwise, return NULL.
 * The caller is responsible for freeing the memory allocated for the result.
 */
char* search_command(const char *command);

#endif
