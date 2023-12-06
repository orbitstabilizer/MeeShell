#include "repl.h"
#include "dictionary.h"

#define ALIAS_FILE "aliases.txt"

struct user user;
Dictionary *aliases;

// TODO: empty space after quote is ignored
// TODO: handle aliaess expansion
// TODO: handle redirection
//
void setup(){
    // load aliases
    aliases = new_dictionary();
    load_dict(aliases, ALIAS_FILE);
    // catch ctrl-c
    signal(SIGINT, sigint_handler);
}

void teardown(){
    printf("\nExiting shell\n");
    // save aliases
    dump_dict(aliases, ALIAS_FILE);
    // free aliases
    free_dict(aliases);
}


int main() {
    setup();
    main_loop(&user, aliases);
    teardown();
    return 0;
}

