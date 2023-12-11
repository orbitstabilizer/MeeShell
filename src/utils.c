#include "utils.h"
#include <errno.h>
#include <time.h>


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
     should not modify the string pointed to by
     the getenv() function.
    */
    char *path_copy = strdup(path); // Duplicate the string pointed to by path
    char *path_token = strtok(path_copy, ":");
    char *command_path =
        calloc(1, MAX_PATH_LENGTH); // Allocate memory for the command path
    while (path_token != NULL) {
        path_token = strtok(NULL, ":");
        if (path_token == NULL) {
            break;
        }
        snprintf(command_path, MAX_PATH_LENGTH, "%s/%s", path_token, command);
        if (access(command_path, X_OK) ==
            0) { // Check if the file exists and is executable
            free(path_copy);
            return command_path;
        }
    }
    free(path_copy);
    free(command_path);
    return strdup(command);
}


int exec_command(char **argv, int background, char *std_out, int mode,
                 User *user) {
    char *path = search_command(argv[0]);
    if (path != NULL) {
        pid_t pid = fork();
        if (pid < 0) {
            return 1;
        } else if (pid == 0) { // child process
            freopen(std_out, mode == 0 ? "w" : "a", stdout);
            execv(path, argv);
            return 2;
        } else { // parent process
            if (!background)
                wait(NULL);
            else {
                user->add_bg_process(user, pid);
                msleep(100);
            }
        }
        free(path);
        return 0;
    }
    return 2;
}

/*
 * Utility function to write the given buffer to the given file in reverse order.
 * Arguments:
 * - char *file: the file name to write to
 *   If the file does not exist, it will be created.
 * - char *buffer: the buffer to read data from
 * - size_t count: the size of the buffer
 *   If the buffer ends with a newline character, it will be ignored.
 * Returns:
 * - int status: the exit status of the command
 *   0: success
 *   3: fopen failed
 *
 */
int write_in_reverse(char *file, char *buffer, size_t count) {
    FILE *fp = fopen(file, "a");
    if (fp == NULL) {
        return 3;
    }
    if (count >= 1 && buffer[count - 1] == '\n') {
        count--;
    }
    // print in reverse order
    for (ssize_t i = count - 1; i >= 0; i--) {
        fputc(buffer[i], fp);
    }
    fputc('\n', fp);
    fclose(fp);
    return 0;
}

/*
 * Utility for >>> redirection.
 * Arguments:
 * - char *file: the file name to write to
 * - char **argv:  null-terminated array of strings
 * - char *path: the path to the command to execute
 * - int *status: the exit status of the command
 *   0: success
 *   2: Fork failed
 *   3: getdelim failed
 *   4: write_in_reverse failed
 *   
 */
void handle_rappend(char *file, char **argv, char *path, int *status) {
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid < 0) {
        *status = 2;
        return;
    } else if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execv(path, argv);
    } else {
        close(fd[1]);
        char *buffer = NULL;
        size_t size = 0;
        ssize_t count = getdelim(&buffer, &size, '\0', fdopen(fd[0], "r"));
        if (count == -1) {
            *status = 3;
        }
        else if (write_in_reverse(file, buffer, count) != 0) {
            *status = 4;
        }
        if (buffer != NULL)
            free(buffer);
        close(fd[0]);
        wait(NULL);
    }
    status = 0;
}



int exec_with_pipe(char **argv, char *file, int bg, User *user) {
    char *_path = search_command(argv[0]);
    if (_path == NULL) {
        return 1;
    }
    size_t len = strlen(_path);
    char path[len + 1];
    strncpy(path, _path, len);
    path[len] = '\0';

    int status = 0;
    pid_t pid = -1;
    if (bg) {
        pid = fork();
    }
    if (pid > 0) {
        user->add_bg_process(user, pid);
        return 0;
    }
    handle_rappend(file, argv, path, &status);
    if (bg){
        exit(status);
    }
    return status;
}
