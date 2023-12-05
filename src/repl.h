#ifndef REPL_H
#define REPL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#include "debug.h"

#define BUFFER_SIZE 256

enum command_type {
    REDIRECT,
    APPEND,
    BACKGROUND,
    ALIAS,
    REVERSE,
    NOT_IMPLEMENTED,
    NORMAL
};
enum command_type check_shell_commands(char *buffer, size_t buffer_size);

void print_prompt(struct user *user);

void sigint_handler(int sig);

void main_loop(struct user *user);

#endif
