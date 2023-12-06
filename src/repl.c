#include "repl.h"
#include "debug.h"
#include "tokenizer.h"
#include "utils.h"
#include <stdarg.h> // variadic functions

char buffer[BUFFER_SIZE];
User *current_user;

void display(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void print_prompt(User *user) {
    User__update(user);
    printf("%s@%s %s --- ", user->username, user->hostname, user->cwd);
    fflush(stdout);
}
// Ctrl-C was pressed
void sigint_handler(int sig) {
    UNUSED(sig);
    printf("\n");
    print_prompt(current_user);
    memset(buffer, 0, BUFFER_SIZE);
}

char *next_literal(Tokenizer *tokenizer, size_t *ind) {
    size_t err;
    char *literal = Tokenizer_next_literal(tokenizer, ind, &err);
    if (err == 1) {
        display("next_literal: Quoted string not terminated\n");
    } else if (err == 2) {
        display("next_literal: Invalid input sequence\n");
    }
    return literal;
}

void handle_cd(Tokenizer *tokenizer, User *user) {
    // cd command must have exactly 1 argument
    size_t token_ind = 1;
    char *path = next_literal(tokenizer, &token_ind);
    if (path == NULL) {
        return;
    }
    // change directory
    User__set_last_command(user, "cd");
    if (chdir(path) != 0) {
        display("Invalid path\n");
    }
}

void handle_alias(Tokenizer *tokenizer, Dict *aliases, User *user) {
#define token_list tokenizer->list
    if (token_list[1].type != TOKEN_LITERAL ||
        token_list[2].type != TOKEN_EQUAL) {
        display("Invalid input sequence\n");
        return;
    }
    char key[token_list[1].length + 1];
    Tokenizer__get_token(tokenizer, 1, key);

    size_t ind = 3;
    if (token_list[ind].type == TOKEN_QUOTE) {
        ind++;
        if (token_list[ind].type == TOKEN_QUOTE) { // empty string
            display("Alias unset\n");
            Dict__del(aliases, key);
            return;
        }
        if (token_list[ind + 1].type != TOKEN_QUOTE) {
            display("Quoted string not terminated\n");
            return;
        }
    }
    if (token_list[ind].type != TOKEN_LITERAL) {
        display("Invalid input sequence\n");
        return;
    }
    char value[token_list[ind].length + 1];
    Tokenizer__get_token(tokenizer, ind, value);

    Dict__set(aliases, key, value);
    User__set_last_command(user, "alias");
    display("Alias set\n");
#undef token_list
    return;
}

int handle_external_command(Tokenizer *tokenizer, User *user, char *command) {
    // first argument should be the current command
    size_t token_count = tokenizer->cur_token;
    char *argv[token_count + 1]; // +1 for NULL terminator
    argv[0] = command;
    size_t ind = 1;
    int quote = 0;
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
        case TOKEN_QUOTE:
            quote = !quote;
            break;
            if (redirect != -1) {
                display("Error: faulty redirection\n");
                return 2;
            }
        case TOKEN_LITERAL:
            tokenizer->list[i].start[tokenizer->list[i].length] = '\0';
            argv[ind] = tokenizer->list[i].start;
            ind++;
            break;
        case TOKEN_ANGLE_BRACKET1:
            if (redirect != -1) {
                display("Error: multiple redirection\n");
                return 3;
            } else {
                redirect = 1;
                mode = 0;
                i++;
                char *filename = next_literal(tokenizer, &i);
                if (filename == NULL) {
                    return 4;
                }
                std_out = filename;
            }
            break;
        case TOKEN_ANGLE_BRACKET2:
            if (redirect != -1) {
                display("Error: multiple redirection\n");
                return 5;
            } else {
                redirect = 1;
                mode = 1;
                i++;
                char *filename = next_literal(tokenizer, &i);
                if (filename == NULL) {
                    return 6;
                }
                std_out = filename;
            }
            break;
        case TOKEN_ANGLE_BRACKET3:
            break;
        default:
            break;
        }
    }
    argv[ind] = NULL;
    User__set_last_command(user, argv[0]);
    exec_command(argv, backgroud, std_in, std_out, std_err, mode);
    return 0;
}

void main_loop(User *user, Dict *aliases) {
    current_user = user;
    bool is_alias = false;

    while (1) {
        if (is_alias) {
        } else {
            print_prompt(user);
            memset(buffer, 0, BUFFER_SIZE);
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
                break;
        }
        Tokenizer *tokenizer = Tokenizer__new(buffer, BUFFER_SIZE);
        Tokenizer__consume(tokenizer);

        Token *token_list = tokenizer->list;

        // enter without any input
        if (token_list[0].type == TOKEN_EOF) {
            continue;
        }
        // command must start with some kind of literal
        if (token_list[0].type != TOKEN_LITERAL) {
            display("Invalid input sequence\n");
            continue;
        }

        char command[token_list[0].length + 1];
        Tokenizer__get_token(tokenizer, 0, command);

        /* builtin commands: cd, exit, alias, bello */
        if (strcmp(command, "cd") == 0) {
            handle_cd(tokenizer, user);
            continue;
        }
        if (strcmp(command, "exit") == 0) { 
            break;
        }
        if (!is_alias && strcmp(command, "alias") == 0) { 
            handle_alias(tokenizer, aliases, user);
            continue;
        }
        if (strcmp(command, "bello") == 0) { 
            User__info(user);
            User__set_last_command(user, "bello");
            continue;
        }
        // this must be a external command, first check if it is an alias
        if (!is_alias) { // avoid infinite loop
            char *value = Dict__get(aliases, command);
            if (value != NULL) {
                display("`%s` is an alias for `%s`\n", command, value);
                char tmp[BUFFER_SIZE];
                strncpy(tmp, value, strlen(value));
                tmp[strlen(value)] = ' ';
                strncpy(tmp + strlen(value) + 1, buffer + strlen(command),
                        BUFFER_SIZE);
                strncpy(buffer, tmp, BUFFER_SIZE);
                buffer[strlen(tmp)] = '\n';
                is_alias = true;
                goto end;
            }
        } else {
            is_alias = false;
        }

        handle_external_command(tokenizer, user, command);
    end:
        Tokenizer__free(tokenizer);
    }
}
