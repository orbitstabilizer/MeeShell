#ifndef UTILS_H
#define UTILS_H

#include "user.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 1024
#define MAX_ARGS 256
#define PIPE_BUF 4096
#define MAX_PATH 256

/*
 * Given a command, search for it in the PATH environment variable.
 * If file exists and is executable, return the full path to the file.
 * If the command is not found in path, return the command itself,
 * which means that the command is in the current directory or
 * the command is referenced by an absolute or relative path.
 * The caller is responsible for freeing the memory allocated for the result.
 *
 * Arguments:
 *  - const char *command: the command string to search path for
 *
 * Returns:
 *  - char *: the full path to the command, or duplicate of given 
 *     command string
 *
 * Example:
 *      search_command("ls") returns "/bin/ls"
 *      search_command("/home/main") returns "/home/main" // absolute path
 */
char *search_command(const char *command);



/*
 * Handles >>> redirection. 
 * This function spawns an auxiliary process if bg is set to 1.
 * The auxiliary process forks again to execute the command,
 * the auxuliary process will wait for the command to finish and
 * write the output to the given file in reverse order.
 *
 * Arguments:
 * - char **argv:  null-terminated array of strings
 * - char *file: the file name to write to
 * - int bg: whether the command should be executed in the background
 *   0: no
 *   1: yes
 * - User *user: the user object
 *
 *  Returns:
 *  - int status: the exit status of the command
 *  0: success
 *  1: Command not found
 *  2: Fork failed
 *  3: getdelim failed
 *  4: write_in_reverse failed
 *
 */
int exec_with_pipe(char **argv, char *file, int bg, User *user);


/*
 * Execute the given command in fork/exec pattern.
 * Arguments:
 * - char **argv:  null-terminated array of strings
 *   argv[0] is the command to execute, argv[1] to argv[n] are
 *   the arguments to the command, and argv[n+1] is NULL
 * - int background: 1 if the command should be executed in background,
 *   0 if the command should be executed in foreground
 * - char *std_out: the file to redirect stdout to, or NULL if stdout
 *   should not be redirected
 *   If the file does not exist, it will be created.
 * - int mode: the mode to use when creating the file, if it does not exist
 *   Ignored if std_out is NULL. Takes values 0 (overwrite) or positve (append).
 *   If mode is positive, the file will be created if it does not exist.
 * - User *user: the user struct
 *
 * Returns:
 * - int status: the exit status of the command
 *      0: success
 *      1: fork failed
 *      2: Command not found
 *  
 */
int exec_command(char **argv, int background, char *std_out, int mode,
                  User *user);


/*
 * Sleep for the requested number of milliseconds.
 * Return 0 if completed, or the number of milliseconds left to sleep if
 * interrupted by a signal handler.
 * From: 
 * https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
 *
 * Arguments:
 * - long msec: the number of milliseconds to sleep
 *           must be non-negative
 *
 * Returns:
 * - long: the number of milliseconds left to sleep if interrupted by a signal
 *      handler, or 0 if completed
 *      If the argument is invalid, return -1.
 */
int msleep(long msec);
#endif
