#include "utils.h"
#include <errno.h>
#include <time.h>

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

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
    char *command_path = calloc(1, MAX_PATH_LENGTH); // Allocate memory for the command path
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
        if (access(command_path, X_OK) == 0) { // Check if the file exists and is executable
            free(path_copy);
            return command_path;
        }
    }
    free(path_copy);
    free(command_path);
    return strdup(command);
}

void exec_command(char **argv, int background, char *std_in, char *std_out,
                  char *std_err, int mode, User *user) {
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
                user->add_bg_process(user, pid);
                msleep(100);
                // printf("\nProcess %d running in background\n", pid);
            }
        }
        free(path);
    } else {
        printf("Command not found in path\n");
    }
}

/*
 * 0: success
 * 1: command not found
 * 2: fork failed
 * 3: file open failed
 */
int exec_with_pipe(char **argv, char *file, int bg, User *user) {
    char *path = search_command(argv[0]);
    int status = 0;
    if (path == NULL) {
        status = 1;
        goto end;
    }
    pid_t pid = -1;
    if (bg) {
        pid = fork();
    }
    if (pid > 0) {
        user->add_bg_process(user, pid);
        goto end;
    }
    int fd[2];
    pipe(fd);
    pid = fork();
    if (pid < 0) {
        status = 2;
        goto end;
    }
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execv(path, argv);
    } else {
        close(fd[1]);
        size_t capacity = 256;
        size_t size = 0;
        char *buffer = malloc(capacity);
        while (1) {
            ssize_t count = read(fd[0], buffer + size, capacity - size);
            if (count == 0) {
                break;
            }
            size += count;
            if (capacity > 1000000) {
                break;
            }
            if (size == capacity) {
                capacity *= 2;
                buffer = realloc(buffer, capacity);
            }

        }

        FILE *fp = fopen(file, "a");
        if (fp == NULL) {
            free(buffer);
            status = 3;
            goto end;
        }
        if (buffer[size - 1] == '\n') {
            size--;
        }
        for (int i = size - 1; i >= 0; i--) {
            fputc(buffer[i], fp);
        }
        fputc('\n', fp);
        fclose(fp);
        close(fd[0]);

        free(buffer);
        wait(NULL);
    }
    if (bg) {
        exit(0);
    }

end:
    free(path);
    return status;
}


