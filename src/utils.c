#include "utils.h"

char *search_command(const char *command) {
    char *path = getenv("PATH");
    /*  From man getenv:
     * The getenv() function obtains the current value of the environment
     variable, name.  The application should not modify the string pointed to by
     the getenv() function.
    */
    char *path_copy = strdup(path); // Duplicate the string pointed to by path
    //
    char *path_token = strtok(path_copy, ":");
    char *command_path =
        calloc(1, MAX_PATH_LENGTH); // Allocate memory for the command path
    while (path_token != NULL) {
        path_token = strtok(NULL, ":");
        if (path_token == NULL) {
            break;
        }
        snprintf(command_path, MAX_PATH_LENGTH, "%s/%s", path_token, command);

        /*
        *   The access() system call checks the accessibility of the file named
        by the path argument for the access permissions indicated by the mode
         argument.  The value of mode is either the bitwise-inclusive OR of the
         access permissions to be checked (R_OK for read permission, W_OK for
         write permission, and X_OK for execute/search permission), or the
         existence test (F_OK).
        */
        if (access(command_path, X_OK) ==
            0) { // Check if the file exists and is executable
            return command_path;
        }
    }
    return NULL;
}

void exec_command(char **argv, int background, char *std_in, char *std_out,
                  char *std_err, int mode) {
    char *path = search_command(argv[0]);
    if (path != NULL) {
        // save stdin, stdout, stderr
        int stdin_copy = dup(STDIN_FILENO);
        int stdout_copy = dup(STDOUT_FILENO);
        int stderr_copy = dup(STDERR_FILENO);
        // redirect stdin, stdout, stderr
        if (std_in != NULL) {
            freopen(std_in, "r", stdin);
        }
        if (std_out != NULL) {
            if (mode == 0) {
                freopen(std_out, "w", stdout);
            } else if (mode == 1) {
                freopen(std_out, "a", stdout);
            }
        }
        if (std_err != NULL) {
            freopen(std_err, "w", stderr);
        }
        pid_t pid = fork();
        if (pid == 0) { // child process
            execv(path, argv);
            // if execv returns, it must have failed
            printf("Command not found\n");
            exit(1);
        } else { // parent process
            // restore stdin, stdout, stderr
            dup2(stdin_copy, STDIN_FILENO);
            dup2(stdout_copy, STDOUT_FILENO);
            dup2(stderr_copy, STDERR_FILENO);
            close(stdin_copy);
            close(stdout_copy);
            close(stderr_copy);

            if (!background)
                wait(NULL);
            else {
                printf("\nProcess %d running in background\n", pid);
            }
        }
        free(path);
    } else {
        printf("Command not found\n");
    }
}
