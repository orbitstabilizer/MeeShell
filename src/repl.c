#include "repl.h"
#include "tokenizer.h"
#include "utils.h"

void display(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

char *next_literal(Tokenizer *tokenizer, size_t ind) {
    char *literal = tokenizer->next_literal(tokenizer, ind);
    if (literal == NULL) {
        display("Error: expected literal\n");
    }
    return literal;
}

Repl *Repl__new(User *user, Dict *aliases) {
    Repl *self = malloc(sizeof(Repl));
    // fields
    self->user = user;
    self->aliases = aliases;
    self->tokenizer = NULL;

    // methods
    self->read_prompt = Repl__read_prompt;
    self->free = Repl__free;
    self->main_loop = Repl__main_loop;
    self->print_prompt = Repl__print_prompt;
    self->handle_cd = Repl__handle_cd;
    self->handle_alias = Repl__handle_alias;
    self->handle_external_command = Repl__handle_external_command;
    self->check_alias = Repl__check_alias;
    self->handle_bello = Repl__handle_bello;
    return self;
}

void Repl__free(Repl *self) {
    if (self->tokenizer != NULL) {
        self->tokenizer->free(self->tokenizer);
    }
    free(self); }

bool Repl__read_prompt(Repl *self) {
    memset(self->buffer, 0, BUFFER_SIZE);
    return (fgets(self->buffer, BUFFER_SIZE, stdin) != NULL);
}

void Repl__print_prompt(Repl *self) {
    User *user = self->user;
    user->update(user);
    char *cwd = user->cwd;
    char *home = user->home;
    char path[strlen(cwd) + 1];
    if (strncmp(cwd, home, strlen(home)) == 0) {
        path[0] = '~';
        strncpy(path + 1, cwd + strlen(home), strlen(cwd) - strlen(home));
        path[strlen(cwd) - strlen(home) + 1] = '\0';
    } else {
        strncpy(path, cwd, strlen(cwd));
        path[strlen(cwd)] = '\0';
    }
    display("%s@%s %s --- ", user->username, user->hostname, path);
    fflush(stdout);
}

void Repl__handle_cd(Repl *self) {
    // cd command must have exactly 1 argument
    char *path = next_literal(self->tokenizer, 1);
    if (path == NULL) {
        return;
    }
    // change directory
    self->user->set_last_command(self->user, "cd");
    if (chdir(path) != 0) {
        display("cd: %s: No such file or directory\n", path);
    }
}

void Repl__handle_alias(Repl *self) {
    Token *token_list = self->tokenizer->list;
    if (token_list[1].type != TOKEN_LITERAL ||
        token_list[2].type != TOKEN_EQUAL) {
        display("Invalid input sequence\n");
        return;
    }
    char *key = next_literal(self->tokenizer, 1);   
    if (strlen(key) == 0) {
        display("Alias key cannot be empty\n");
        return;
    }
    if (token_list[3].type != TOKEN_LITERAL) {
        display("Invalid input sequence\n");
        return;
    }
    char *value = next_literal(self->tokenizer, 3);
    if (strlen(value) == 0) {
        Dict__del(self->aliases, key);
        display("Alias unset\n");
        return;
    }
    Dict__set(self->aliases, key, value);
    self->user->set_last_command(self->user, "alias");
    display("Alias set\n");
    return;
}

int Repl__handle_external_command(Repl *self, char *command) {
    Tokenizer *tokenizer = self->tokenizer;
    // first argument should be the current command
    size_t token_count = tokenizer->cur_token;
    char *argv[token_count + 1]; // +1 for NULL terminator
    argv[0] = command;
    size_t ind = 1;
    int backgroud = 0;
    int redirect = -1;
    char *std_in = NULL;
    char *std_out = NULL;
    char *std_err = NULL;
    int mode = 0;
    for (size_t i = 1; i < token_count; i++) {
        switch (tokenizer->list[i].type) {
        case TOKEN_AMPERSAND:
            backgroud = 1;
            if (i != token_count - 2) {
                display("Error: & must be the last token\n");
                return 1;
            }
            break;
        case TOKEN_LITERAL:
            argv[ind] = next_literal(tokenizer, i);
            ind++;
            break;
        case TOKEN_REDIRECT:
            if (redirect != -1) {
                display("Error: multiple redirection\n");
                return 3;
            } else {
                redirect = 1;
                mode = 0;
                i++;
                char *filename = next_literal(tokenizer, i);
                if (filename == NULL) {
                    return 4;
                }
                std_out = filename;
            }
            break;
        case TOKEN_APPEND:
            if (redirect != -1) {
                display("Error: multiple redirection\n");
                return 5;
            } else {
                redirect = 1;
                mode = 1;
                i++;
                char *filename = next_literal(tokenizer, i);
                if (filename == NULL) {
                    return 6;
                }
                std_out = filename;
            }
            break;
        case TOKEN_RAPPEND:
            if (redirect != -1) {
                display("Error: multiple redirection\n");
                return 7;
            } else {
                redirect = 2;
                i++;
                char *filename = next_literal(tokenizer, i);
                if (filename == NULL) {
                    return 8;
                }
                std_out = filename;
            }
            break;
        default:
            break;
        }
    }
    argv[ind] = NULL;
    self->user->set_last_command(self->user, argv[0]);
    if (redirect == 2) {
        exec_with_pipe(argv, std_out, backgroud, self->user);
    } else {
        exec_command(argv, backgroud, std_in, std_out, std_err, mode,
                     self->user);
    }
    return 0;
}

void Repl__handle_bello(Repl *self){
    self->user->info(self->user);
    self->user->set_last_command(self->user, "bello");
}

void Repl__main_loop(Repl *self) {
    bool is_alias = false;
    Tokenizer *tokenizer = NULL;
    Token *token_list;

    while (1) {
        if (!is_alias) {
            self->print_prompt(self);
            if (!self->read_prompt(self)) {
                break;
            }
        }
        if (tokenizer != NULL) {
            tokenizer->free(tokenizer);
            tokenizer = NULL;
        }
        tokenizer = Tokenizer__new(self->buffer, BUFFER_SIZE);
        if (tokenizer->err != 0) {
            display(Tokenizer__error_msg[tokenizer->err]);
            continue;
        }

        self->tokenizer = tokenizer;
        token_list = tokenizer->list;

        // enter without any input
        if (token_list[0].type == TOKEN_EOF) {
            continue;
        }
        // command must start with some kind of literal
        if (token_list[0].type != TOKEN_LITERAL) {
            display("Error: invalid input sequence\n");
            continue;
        }
        char *command = next_literal(tokenizer, 0);
        //  first check if it is an alias
        is_alias = self->check_alias(self, is_alias, command);
        if (is_alias) {
            continue;
        }
        /* builtin commands: cd, exit, alias, bello */
        if (strcmp(command, "cd") == 0) {
            self->handle_cd(self);
            continue;
        }
        if (strcmp(command, "exit") == 0) {
            break;
        }
        if (!is_alias && strcmp(command, "alias") == 0) {
            self->handle_alias(self);
            continue;
        }
        if (strcmp(command, "bello") == 0) {
            self->handle_bello(self);
            continue;
        }
        self->handle_external_command(self, command);
    }
}

bool Repl__check_alias(Repl *self, bool is_alias, char *command) {
    if (!is_alias) { // avoid infinite loop
        char *value = Dict__get(self->aliases, command);
        if (value != NULL) {
            display("`%s` is an alias for `%s`\n", command, value);
            // substitute the alias
            char tmp[BUFFER_SIZE];
            strncpy(tmp, self->buffer, BUFFER_SIZE);
            size_t len_c = strlen(command);
            size_t len_v = strlen(value);
            strncpy(self->buffer, value, len_v);
            self->buffer[len_v] = ' ';
            strncpy(self->buffer + len_v + 1, tmp + len_c,
                    BUFFER_SIZE - len_v - 1);
            return true;
        }
    }
    return false;
}
