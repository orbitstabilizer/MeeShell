
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"



void load_aliases(Dictionary *dict) {
    FILE *fp = fopen("aliases.txt", "r");
    if (fp == NULL) {
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1) {
        char *alias = strtok(line, " ");
        char *command = strtok(NULL, "\n");
        if (alias == NULL || command == NULL) {
            continue;
        }
        // printf("alias: %s, command: %s\n", alias, command);
        dictionary_set(dict, alias, command);
    }
    fclose(fp);
    if (line) {
        free(line);
    }
}
