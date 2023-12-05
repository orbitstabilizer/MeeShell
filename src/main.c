#include "repl.h"
#include "dictionary.h"

#define ALIAS_FILE "aliases.txt"

struct user user;
Dictionary *aliases;


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
    main_loop(&user);
    teardown();
    return 0;
}

