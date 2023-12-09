#include "dictionary.h"
#include "repl.h"
#include "user.h"
#define UNUSED(x) (void)(x)

#define ALIAS_FILE ".meeshrc"

User *user = NULL;
Dict *aliases;
Repl *repl;

// TODO: bello redirection
// TODO: finish bello
// TODO: recursive alias
// TODO: clear screen
        // C-l -> clear screen
        // if (buffer[0] == 12) {
        //     printf("\033[H\033[J");
        //     continue;
        // }
/*
Ctrl-C was pressed
*/
void sigint_handler(int _) {
    UNUSED(_);
    printf("\n");
    repl->print_prompt(repl);
}

void sigchld_handler(int _) { // walking dead!
    UNUSED(_);
    int status;
    for (int i = 0; i < user->bg_pids_count; i++) {
        pid_t pid = waitpid(user->bg_pids[i], &status, WNOHANG);
        if (pid > 0) {
            user->remove_bg_process(user, pid);
        }
    }
}

void setup() {
    // load aliases
    aliases = Dict__new();
    user = User__new_user();
    Dict__load(aliases, ALIAS_FILE);
    // catch ctrl-c
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);
}

void teardown() {
    for (int i = 0 ; i< user->bg_pids_count; i++) {
        kill(user->bg_pids[i], SIGHUP);
    }
    printf("\nExiting shell\n");
    // save aliases
    Dict__dump(aliases, ALIAS_FILE);
    // free aliases
    Dict__free(aliases);
    user->free(user);
    repl->free(repl);
}

int main() {
    setup();
    repl = Repl__new( user, aliases);
    repl->main_loop(repl);
    teardown();
    return 0;
}
