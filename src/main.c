#include "dictionary.h"
#include "repl.h"
#include "user.h"

#define ALIAS_FILE ".meeshrc"

User *user = NULL;
Dict *aliases;

// TODO: bello redirection
// TODO: finish bello
// TODO: recursive alias
// TODO: clear screen
        // C-l -> clear screen
        // if (buffer[0] == 12) {
        //     printf("\033[H\033[J");
        //     continue;
        // }

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
    User__free_user(user);
}

int main() {
    setup();
    main_loop(user, aliases);
    teardown();
    return 0;
}
