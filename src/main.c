#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
// #include "debug.h"

#define BUFFER_SIZE 256
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>



int main() {
    // plus 1 for null terminator 
    char buffer[BUFFER_SIZE];

    char username[BUFFER_SIZE];
    char cwd[BUFFER_SIZE];
    char hostname[BUFFER_SIZE];
    // Dictionary *dict = new_dictionary();

    struct passwd *pws;
    uid_t uid = getuid();
    pws = getpwuid(uid);

    strcpy(username, pws->pw_name);
    gethostname(hostname, BUFFER_SIZE);
    getcwd(cwd, BUFFER_SIZE);

    while (1) {
        printf("%s@%s %s --- ", username, hostname, cwd);
        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL)
            break;

        // remove newline character
        buffer[strlen(buffer) - 1] = '\0';

        // exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // search command
        if (strncmp(buffer, "which ", 6) == 0) {
            char *result = search_command(buffer + 6);
            if (result != NULL) {
                printf("%s\n", result);
                free(result);
            } else {
                printf("Command not found\n");
            }
        }

        // whoami command
        if (strcmp(buffer, "whoami") == 0) {
            printf("%s\n", username);
        }

    }

    return 0;
}


