#ifndef REPL_H
#define REPL_H
#include "dictionary.h"
#include "user.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_prompt(User *user);

void sigint_handler(int sig);
void sigchld_handler(int sig);

void main_loop(User *user, Dict *alias_dict);

#endif
