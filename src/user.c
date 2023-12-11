#include "user.h"
char * get_parent_shell();

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

    user->shell = get_parent_shell();
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

/*
 * Update the current working directory of the user
 * 
 * Arguments:
 * - User *self: the user object
 */
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

    char* buffer = NULL;
    size_t buffer_size = 0;
    FILE* ss = open_memstream(&buffer, &buffer_size);
    fprintf(ss, text, self->username, self->hostname, self->shell,
            self->home, date_time, self->last_command, num_procs, tty);
    fclose(ss);    //This will set buffer and bufferSize.
    if (!reverse) {
        fprintf(stream, "%s", buffer);
        free(buffer);
    } else {
        // reverse the content
        for (int i = buffer_size - 1; i >= 0; i--) {
            if (i == (int)buffer_size - 1 && buffer[i] == '\n')
                continue;
            fputc(buffer[i], stream);
        }
        fputc('\n', stream);
    }
}


/*
 * Utility for getting the name of the parent shell
 * note that the function otputs the name of parent process,
 * so, if the parent process is a shell, 
 * then the output will be the name of the shell,
 * otherwise, it will be the name of the parent process.
 * E.g. if the parent process is login, then the output will be login.
 * 
 * Returns:
 * - char *: the name of the parent shell
 */
char *get_parent_shell(){
    pid_t parent_pid = getppid();
    char parent_pid_str[MAX_PID_LENGTH];
    snprintf(parent_pid_str, MAX_PID_LENGTH, "%d", (int)parent_pid);

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        return NULL;
    }
    pid_t child_pid = fork();

    if (child_pid < 0) {
        close(pipe_fd[0]);
        close(pipe_fd[1]);
    }
    else if (child_pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        // execute the ps command in the child process
        execlp("ps", "ps", "-p", parent_pid_str, "-o", "comm=", (char *)NULL);
    } else {
        close(pipe_fd[1]);
        char *buffer = NULL;
        size_t buffer_size = 0;
        // Read the output of the ps command from the pipe
        // ssize_t len = read(pipe_fd[0], buffer, buffer_size - 1);
        ssize_t len = getline(&buffer, &buffer_size, fdopen(pipe_fd[0], "r"));
        close(pipe_fd[0]);
        wait(NULL);
        if (len > 0) {
            // remove the trailing newline character
            buffer[len - 1] = '\0';
            return buffer;
        }
        return buffer;
    }
    return NULL;

}
