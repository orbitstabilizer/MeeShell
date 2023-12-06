#ifndef USER_H
#define USER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pwd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>

#define BUFFER_SIZE 1024

typedef struct User {
    uid_t uid;
    char *username;
    char *hostname;
    char *cwd;
    char *home;
    char *shell;
    char last_command[BUFFER_SIZE];
} User;

User *User__new_user();
void User__free_user(User *self);
void User__update(User *self);
void User__info(User *self);
void User__set_last_command(User *self, char *command);

#endif
