#include "dictionary.h"
#include "repl.h"
#include "user.h"

#define ALIAS_FILE ".meeshrc"

User *user = NULL;
Dict *aliases;

// TODO: finish bello
// TODO: in >> if file doesn't exist it creates it, but it should not
// TODO: should use parent name for shell?
// TODO: it doens't recognize ~
// TODO: implement >>>
// TODO: implement ProcessList
// TODO: empty space after quote is ignored
// TODO: after moving process to background shell output gets messed up
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
}

void teardown() {
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
