#ifndef USER_H
#define USER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_BG_PIDS 1024
#define MAX_SH_NAME 256

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
} User;

User *User__new_user();
void User__free_user(User *self);
void User__update(User *self);
void User__info(User *self);
void User__set_last_command(User *self, char *command);
void User__add_bg_process(User *self, pid_t pid);
void User__remove_bg_process(User *self, pid_t pid);
void User__print_bg_processes(User *self);

#endif
