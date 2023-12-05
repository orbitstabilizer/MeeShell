#include "repl.h"

char buffer[BUFFER_SIZE];
struct user *current_user;

enum command_type check_shell_commands(char *buffer, size_t buffer_size){
    // special characters: > , >>, >>>, &, = 
    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer[i] == '>') {
            printf("> not implemented\n");
            return NOT_IMPLEMENTED;
        } else if (buffer[i] == '&') {
            return BACKGROUND;
        } else if (buffer[i] == '=') {
            return ALIAS;
        }
    }
    return NORMAL;
}

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

void main_loop(struct user *user){
    current_user = user;
    
    while (1) {
        print_prompt(user);
        // clear the buffer
        memset(buffer, 0, BUFFER_SIZE);


        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;

        // C-l -> clear screen
        // if (buffer[0] == 12) {
        //     printf("\033[H\033[J");
        //     continue;
        // }

        if (buffer[0] == '\0') {
            continue;
        }

        // remove newline character
        buffer[strlen(buffer) - 1] = '\0';
        if (strlen(buffer) == 0) {
            continue;
        }

        // exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // check for shell commands
        enum command_type type = check_shell_commands(buffer, BUFFER_SIZE);

        switch (type) {
            case REDIRECT:
                printf("Redirect not implemented\n");
                continue;
            case APPEND:
                printf("Append not implemented\n");
                continue;
            case BACKGROUND:
                {
                char * ampersand = strchr(buffer, '&');
                *ampersand = '\0';
                char **argv = parse_commandline_args(buffer, BUFFER_SIZE);
                exec_command(argv, 1);
                continue;
                }
            case ALIAS:
                printf("Alias not implemented\n");
                continue;
            case REVERSE:
                printf("Reverse not implemented\n");
                continue;
            case NOT_IMPLEMENTED:
                printf("Not implemented\n");
                continue;
            case NORMAL:
                {
                char **argv = parse_commandline_args(buffer, BUFFER_SIZE);
                exec_command(argv, 0);
                free(argv);
                }

        }

        // parse commandline arguments
        
        // run command
    }
}
