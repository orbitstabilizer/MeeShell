#include "user.h"
void get_parent_shell(char *resultBuffer, size_t bufferSize);

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

    char buf[BUFFER_SIZE];
    get_parent_shell(buf, BUFFER_SIZE);
    // remove newline
    buf[strlen(buf) - 1] = '\0';
    user->shell = strdup(buf);

    // user->shell = strdup(getenv("SHELL"));
    // find parent process name
    // user->shell = name;

    memset(user->last_command, 0, sizeof(user->last_command));
    user->bg_pids_count = 0;

    user->free = User__free_user;
    user->update = User__update;
    user->info = User__info;
    user->set_last_command = User__set_last_command;
    user->add_bg_process = User__add_bg_process;
    user->remove_bg_process = User__remove_bg_process;

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

void User__add_bg_process(User *self, pid_t pid) {
    self->bg_pids[self->bg_pids_count++] = pid;
}

void User__remove_bg_process(User *self, pid_t pid) {
    for (int i = 0; i < self->bg_pids_count; i++) {
        if (self->bg_pids[i] == pid) {
            for (int j = i; j < self->bg_pids_count - 1; j++) {
                self->bg_pids[j] = self->bg_pids[j + 1];
            }
            printf("\nProcess %d finished\n", pid);
            self->bg_pids_count--;
            break;
        }
    }
}

void User__info(User *self, FILE *stream, bool reverse) {

    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    char date_time[26];
    strftime(date_time, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    int num_procs = 1 + self->bg_pids_count; //+1 for the shell itself
    char *tty = ttyname(STDIN_FILENO);
    const char *text = "Username                     : %s\n"
                       "Hostname                     : %s\n"
                       "Current Shell                : %s\n"
                       "Home Location                : %s\n"
                       "Current Date and time        : %s\n"
                       "Last Executed Command        : %s\n"
                       "Number of Processes          : %d\n"
                       "Terminal Name(TTY)           : %s\n";

    if (!reverse) {
        fprintf(stream, text, self->username, self->hostname, self->shell,
                self->home, date_time, self->last_command, num_procs, tty);
    } else {
        // reverse the content
        char buf[BUFFER_SIZE];
        snprintf(buf, BUFFER_SIZE, text, self->username, self->hostname,
                 self->shell, self->home, date_time, self->last_command,
                 num_procs, tty);
        int len = strlen(buf);
        for (int i = len - 1; i >= 0; i--) {
            if (i == len - 1 && buf[i] == '\n')
                continue;
            fputc(buf[i], stream);
        }
        fputc('\n', stream);
    }
}

void get_parent_shell(char *resultBuffer, size_t bufferSize) {
    pid_t parent_pid = getppid();
    char parent_pid_str[MAX_SH_NAME];
    char cmd[MAX_SH_NAME];

    snprintf(parent_pid_str, MAX_SH_NAME, "%d", (int)parent_pid);
    snprintf(cmd, MAX_SH_NAME, "ps -p %s -o comm=", parent_pid_str);

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        return;
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return;
    }

    if (child_pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        // execute the ps command in the child process
        execlp("ps", "ps", "-p", parent_pid_str, "-o", "comm=", (char *)NULL);
    } else {
        close(pipe_fd[1]);

        // Read the output of the ps command from the pipe
        ssize_t bytesRead = read(pipe_fd[0], resultBuffer, bufferSize - 1);
        if (bytesRead > 0) {
            resultBuffer[bytesRead] = '\0';
        }
        close(pipe_fd[0]);
        wait(NULL);
    }
}
