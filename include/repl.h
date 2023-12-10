#ifndef REPL_H
#define REPL_H
#include "dictionary.h"
#include "tokenizer.h"
#include "user.h"
#include <signal.h>
#include <stdarg.h> 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct Repl Repl;
void print_prompt(User *user);
void sigint_handler(int sig);
void sigchld_handler(int sig);

struct Repl {
    char buffer[BUFFER_SIZE];
    User *user;
    Dict *aliases;
    Tokenizer *tokenizer;
    bool (*read_prompt)(struct Repl *self);
    void (*free)(struct Repl *self);
    void (*main_loop)(struct Repl *self);

    /*
     * Read a line of input from stdin.
     * Return true if the input is not EOF.
     */
    void (*print_prompt)(struct Repl *self);

    void (*handle_cd)(struct Repl *self);
    void (*handle_alias)(struct Repl *self);
    int (*handle_external_command)(struct Repl *self, char *command);
    bool (*check_alias)(struct Repl *self, bool is_alias, char *command);
    void (*handle_bello)(struct Repl *self, size_t argc, bool bg, char*std_out, int mode);
};

Repl *Repl__new(User *user, Dict *aliases);
bool Repl__read_prompt(Repl *self);
void Repl__free(Repl *self);
void Repl__main_loop(Repl *self);
void Repl__print_prompt(Repl *self);
void Repl__handle_cd(Repl *self);
void Repl__handle_alias(Repl *self);
int Repl__handle_external_command(Repl *self, char *command);
bool Repl__check_alias(Repl *self, bool is_alias, char *command);
void Repl__handle_bello(Repl *self, size_t argc,  bool bg, char*std_outm, int mode);

#endif
