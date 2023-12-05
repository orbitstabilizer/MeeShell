#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <pwd.h>


#define MAX_PATH_LENGTH 1024
#define BUFFER_SIZE 256
#define MAX_ARGS 256

/*
 * Given a command, search for it in the PATH environment variable.
 * If file exists and is executable, return the full path to the file.
 * Otherwise, return NULL.
 * The caller is responsible for freeing the memory allocated for the result.
 */
char* search_command(const char *command);

void exec_command(char **argv, int background);


char **parse_commandline_args(char *buffer, size_t buffer_size);

struct user {
    char username[BUFFER_SIZE];
    char hostname[BUFFER_SIZE];
    char cwd[BUFFER_SIZE];
};

void get_user(struct user *user);




#endif
