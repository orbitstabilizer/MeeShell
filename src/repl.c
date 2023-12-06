#include "repl.h"
#include "tokenizer.h"
#include "debug.h"


char buffer[BUFFER_SIZE];
struct user *current_user;

void print_prompt(struct user *user) {
    get_user(user);
    printf("%s@%s %s --- ", user->username, user->hostname, user->cwd);
    fflush(stdout);
}
// Ctrl-C was pressed
void  sigint_handler(int sig) {
    UNUSED(sig);
    printf("\n");
    print_prompt(current_user);
    memset(buffer, 0, BUFFER_SIZE);
}


void main_loop(struct user *user, Dictionary *aliases){
    current_user = user;
    int skip_read = 0;
    
    while (1) {
        print_prompt(user);
        // clear the buffer
        memset(buffer, 0, BUFFER_SIZE);


        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;
        skip_read = 0;

        // C-l -> clear screen
        // if (buffer[0] == 12) {
        //     printf("\033[H\033[J");
        //     continue;
        // }

        Tokenizer *tokenizer = tokenizer_new(buffer, BUFFER_SIZE);
        tokenizer_consume(tokenizer);

        Token *token_list = tokenizer->list;
        size_t token_count = tokenizer->cur_token;

        // enter without any input
        if (token_list[0].type == TOKEN_EOF) {
            continue;
        }
        // command must start with some kind of literal
        if (token_list[0].type != TOKEN_LITERAL) {
            printf("Invalid input sequence\n");
            continue;
        }
        // now we know the first token is a literal, it must be a command
        // check if it is a shell command
        // shell commands:
        //   cd
        //   exit
        //   alias

        // check for cd command

        char command[token_list[0].length + 1];
        get_token(tokenizer, 0, command);

        // check for cd command
        if (strcmp(command, "cd") == 0) {
            // cd command must have exactly 1 argument
            size_t token_ind = 1;
            if (token_list[token_ind].type == TOKEN_QUOTE) {
                token_ind++;
                if (token_list[token_ind + 1].type != TOKEN_QUOTE) {
                    printf("Quoted string not terminated\n");
                    continue;
                }
            }
            if (token_list[token_ind].type != TOKEN_LITERAL) {
                printf("Invalid input sequence\n");
                continue;
            }
            // change directory
            char path[token_list[token_ind].length + 1];
            strncpy(path, token_list[token_ind].start, token_list[token_ind].length);
            path[token_list[token_ind].length] = '\0';
            if (chdir(path) != 0) {
                printf("Invalid path\n");
            }
            continue;
        }
        // check for exit command
        if (strcmp(command, "exit") == 0) {
            break;
        }
        // check for alias command
        if (strcmp(command, "alias") == 0) {
            if (token_list[1].type != TOKEN_LITERAL || token_list[2].type != TOKEN_EQUAL ){
                printf("Invalid input sequence\n");
                continue;
            }
            char key[token_list[1].length + 1];
            get_token(tokenizer, 1, key);

            size_t ind = 3;
            if (token_list[ind].type == TOKEN_QUOTE) {
                ind++;
                if (token_list[ind + 1].type != TOKEN_QUOTE) {
                    printf("Quoted string not terminated\n");
                    continue;
                }
            }
            if (token_list[ind].type != TOKEN_LITERAL) {
                printf("Invalid input sequence\n");
                continue;
            }
            char value[token_list[ind].length + 1];
            get_token(tokenizer, ind, value);

            set_var(aliases, key, value);
            printf("alias set\n");
            continue;
        }

        // this must be a external command, first check if it is an alias
        // if it is an alias, replace the command with the alias

        

        char *value = get_var(aliases, command);
        if (value != NULL) {
            printf("`%s` is an alias for `%s`\n", command, value);
            strncpy(buffer, value, BUFFER_SIZE);
            tokenizer_free(tokenizer);

            skip_read = 1;
            continue;

        }
        
        char *argv[token_count + 1]; // +1 for NULL terminator
        // first argument should be the current command
        argv[0] = command;
        size_t ind = 1;
        int quote = 0;
        int backgroud = 0;
        for (size_t i = 1; i < token_count; i++) {
            if (token_list[i].type == TOKEN_QUOTE) {
                quote = !quote;
                continue;
            }
            if (token_list[i].type == TOKEN_LITERAL) {
                token_list[i].start[token_list[i].length] = '\0';
                argv[ind] = token_list[i].start;
                ind++;
            }
            if (token_list[i].type == TOKEN_AMPERSAND) {
                backgroud = 1;
            }
        }
        argv[ind] = NULL;
        if (quote) {
            printf("Quoted string not terminated\n");
            continue;
        }
        exec_command(argv, backgroud);


        tokenizer_free(tokenizer);



    }
}
