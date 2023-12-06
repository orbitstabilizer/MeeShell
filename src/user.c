#include "user.h"

User *User__new_user() {
    User *user = malloc(sizeof(User));
    user->uid = getuid();
    struct passwd *pws = getpwuid(user->uid);

    user->username = strdup(pws->pw_name);
    char tmp[BUFFER_SIZE];
    gethostname(tmp, BUFFER_SIZE);
    user->hostname = strdup(tmp);
    getcwd(tmp, BUFFER_SIZE);
    user->cwd = strdup(tmp);
    user->home = strdup(pws->pw_dir);
    user->shell = strdup(getenv("SHELL"));
    memset(user->last_command, 0, sizeof(user->last_command));
    return user;
}

void User__free_user(User *self) {
    free(self->username);
    free(self->hostname);
    free(self->cwd);
    free(self->home);
    free(self->shell);
    free(self);
}

void User__update(User *self) {
    char tmp[BUFFER_SIZE];
    getcwd(tmp, BUFFER_SIZE);
    free(self->cwd);
    self->cwd = strdup(tmp);
}

void User__set_last_command(User *self, char *command) {
    size_t len = strlen(command);
    strncpy(self->last_command, command, len);
    self->last_command[len] = '\0';
}

void User__info(User *self) {

    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    char date_time[26];
    strftime(date_time, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    printf("Username                     : %s\n", self->username);
    printf("Hostname                     : %s\n", self->hostname);
    printf("Current Shell                : %s\n", self->shell);
    printf("Home Location                : %s\n", self->home);
    printf("Current Date and time        : %s\n", date_time);
    printf("Last Executed Command        : %s\n", self->last_command);
}
// Get TTY and number of processes
// char tty[32];
// FILE *ttyFile = popen("tty", "r");
// fgets(tty, sizeof(tty), ttyFile);
// pclose(ttyFile);

// FILE *processesFile = popen("ps -e | wc -l", "r");
// char numberOfProcesses[16];
// fgets(numberOfProcesses, sizeof(numberOfProcesses), processesFile);
// pclose(processesFile);

// // Display information
// printf("Username                     : %s\n", username);
// printf("Hostname                     : %s\n", hostname);
// // printf("Last Executed Command        : %s", lastCommand);
// printf("TTY                          : %s", tty);
// printf("Current Shell Name           : %s\n", shellName);
// printf("Home Location                : %s\n", homeLocation);
// printf("Current Number of Processes  : %s", numberOfProcesses);
