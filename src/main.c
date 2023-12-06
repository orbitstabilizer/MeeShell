#include "dictionary.h"
#include "repl.h"
#include "user.h"

#define ALIAS_FILE "aliases.txt"

User *user = NULL;
Dict *aliases;

// TODO: finish bello
// TODO: implement >>>
// TODO: implement ProcessList
// TODO: empty space after quote is ignored
// TODO: after moving process to background shell output gets messed up

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
