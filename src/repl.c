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

void handle_cd(Tokenizer *tokenizer) {
    // cd command must have exactly 1 argument
    size_t token_ind = 1;
#define token_list tokenizer->list
    if (token_list[token_ind].type == TOKEN_QUOTE) {
        token_ind++;
        if (token_list[token_ind + 1].type != TOKEN_QUOTE) {
            display("Quoted string not terminated\n");
            return;
        }
    }
    if (token_list[token_ind].type != TOKEN_LITERAL) {
        display("Invalid input sequence\n");
        return;
    }
    // change directory
    char path[token_list[token_ind].length + 1];
    strncpy(path, token_list[token_ind].start, token_list[token_ind].length);
    path[token_list[token_ind].length] = '\0';
    if (chdir(path) != 0) {
        display("Invalid path\n");
    }
#undef token_list
}

void handle_alias(Tokenizer *tokenizer, Dict *aliases) {
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
    display("Alias set\n");
#undef token_list
    return;
}

char *next_literal(Tokenizer *tokenizer, size_t *ind) {
#define token_list tokenizer->list
    // quoted literal
    if (token_list[*ind].type == TOKEN_QUOTE) {
        (*ind)++;
        if (token_list[(*ind) + 1].type != TOKEN_QUOTE) {
            display("next_literal: Quoted string not terminated\n");
            return NULL;
        }
    }
    if (token_list[*ind].type != TOKEN_LITERAL) {
        display(token_list[*ind].start);
        display("next_literal: Invalid input sequence\n");
        return NULL;
    }
    token_list[*ind].start[token_list[*ind].length] = '\0';
    return token_list[*ind].start;
#undef token_list
}

void main_loop(User *user, Dict *aliases) {
    current_user = user;
    bool is_alias = false;

    while (1) {
        if (is_alias) {
        } else {
            // read input
            print_prompt(user);
            memset(buffer, 0, BUFFER_SIZE);
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
                break;
        }
        // C-l -> clear screen
        // if (buffer[0] == 12) {
        //     printf("\033[H\033[J");
        //     continue;
        // }

        Tokenizer *tokenizer = Tokenizer__new(buffer, BUFFER_SIZE);
        Tokenizer__consume(tokenizer);

        Token *token_list = tokenizer->list;
        size_t token_count = tokenizer->cur_token;
        char *argv[token_count + 1]; // +1 for NULL terminator

        // enter without any input
        if (token_list[0].type == TOKEN_EOF) {
            continue;
        }
        // command must start with some kind of literal
        if (token_list[0].type != TOKEN_LITERAL) {
            display("Invalid input sequence\n");
            continue;
        }
        // now we know the first token is a literal, it must be a command
        // check if it is a shell command
        // shell commands:
        //   cd
        //   exit
        //   alias
        //   bello

        // check for cd command

        char command[token_list[0].length + 1];
        Tokenizer__get_token(tokenizer, 0, command);

        // check for cd command
        if (strcmp(command, "cd") == 0) {
            handle_cd(tokenizer);
            continue;
        }
        // check for exit command
        if (strcmp(command, "exit") == 0) {
            break;
        }
        // check for alias command
        if (!is_alias && strcmp(command, "alias") == 0) {
            handle_alias(tokenizer, aliases);
            continue;
        }
        // check for bello command
        if (strcmp(command, "bello") == 0) {
            User__info(user);
            continue;
        }

        // this must be a external command, first check if it is an alias
        // if it is an alias, replace the command with the alias

        if (!is_alias) { // avoid infinite loop
            char *value = Dict__get(aliases, command);
            if (value != NULL) {
                display("`%s` is an alias for `%s`\n", command, value);
                // strncpy(buffer, value, BUFFER_SIZE);
                // buffer[strlen(value)] = '\n';
                // display("Executing `%s`\n", buffer);
                // replace the command with the alias
                char tmp[BUFFER_SIZE];
                strncpy(tmp, value, strlen(value));
                tmp[strlen(value)] = ' ';
                strncpy(tmp + strlen(value) + 1, buffer + strlen(command),
                        BUFFER_SIZE);
                strncpy(buffer, tmp, BUFFER_SIZE);
                buffer[strlen(tmp)] = '\n';
                is_alias = true;

                goto end;
                continue;
            }
        } else {
            is_alias = false;
        }

        // first argument should be the current command

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
            switch (token_list[i].type) {
            case TOKEN_AMPERSAND:
                backgroud = 1;
                if (i != token_count - 2) {
                    display("Error: & must be the last token\n");
                    goto error;
                }
                break;
            case TOKEN_QUOTE:
                quote = !quote;
                break;
                if (redirect != -1) {
                    display("Error: faulty redirection\n");
                    goto error;
                }
            case TOKEN_LITERAL:
                token_list[i].start[token_list[i].length] = '\0';
                argv[ind] = token_list[i].start;
                ind++;
                break;
            case TOKEN_ANGLE_BRACKET1:
                if (redirect != -1) {
                    display("Error: multiple redirection\n");
                    goto error;
                } else {
                    redirect = 1;
                    mode = 0;
                    i++;
                    char *filename = next_literal(tokenizer, &i);
                    if (filename == NULL) {
                        goto error;
                    }
                    std_out = filename;
                }
                break;
            case TOKEN_ANGLE_BRACKET2:
                if (redirect != -1) {
                    display("Error: multiple redirection\n");
                    goto error;
                } else {
                    redirect = 1;
                    mode = 1;
                    i++;
                    char *filename = next_literal(tokenizer, &i);
                    if (filename == NULL) {
                        goto error;
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
        // if (quote) {
        //     display("Quoted string not terminated\n");
        //     continue;
        // }
        exec_command(argv, backgroud, std_in, std_out, std_err, mode);

    error:
    end:
        Tokenizer__free(tokenizer);
    }
}
