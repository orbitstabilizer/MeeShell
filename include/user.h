#ifndef USER_H
#define USER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <time.h>


#define BUFFER_SIZE 1024
#define MAX_BG_PIDS 1024
#define MAX_PID_LENGTH 15

typedef struct User {
    uid_t uid;
    char *username;
    char *hostname;
    char *cwd;
    char *home;
    char *shell;
    char last_command[BUFFER_SIZE];
    pid_t bg_pids[MAX_BG_PIDS];
    int bg_pids_count;

    // methods
    void (*free)(struct User *self);
    void (*update)(struct User *self);
    void (*info)(struct User *self, FILE *fp, bool reverse);
    void (*set_last_command)(struct User *self, char *command);
    void (*add_bg_process)(struct User *self, pid_t pid);
    void (*remove_bg_process)(struct User *self, pid_t pid);

} User;
/*
 * Constructor for User struct.
 * Sets the fields, and links the methods appropriate function pointers.
 * The caller should free by calling obj->free(obj).
 *
 * Returns:
 *  - User *: the new User object
 */
User *User__new_user();

/*
 * Destructor for User struct.
 *
 * Arguments:
 *  - User *self: the User object to free
 */
void User__free_user(User *self);

void User__update(User *self);
/*
 * Prints the user information to the given stream, 
 *  used in `bello` command.
 * 
 * Arguments:
 * - User *self: the user object
 * - FILE *stream: the stream to print to
 * - bool reverse: whether to print in reverse or not
 */
void User__info(User *self, FILE *fp, bool reverse);

/*
 * Set the last command executed by the user
 * 
 * Arguments:
 * - User *self: the user object
 * - char *command: the command to set
 */
void User__set_last_command(User *self, char *command);


/*
 * Adds a background process to the user process list
 * 
 * Arguments:
 * - User *self: the user object
 * - pid_t pid: the process id of the background process
 */
void User__add_bg_process(User *self, pid_t pid);

/*
 * Removes a background process from the user process list
 * 
 * Arguments:
 * - User *self: the user object
 * - pid_t pid: the process id of the background process
 */
void User__remove_bg_process(User *self, pid_t pid);

#endif
