#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_aliases(Dict *dict) {
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
        Dict__set(dict, alias, command);
    }
    fclose(fp);
    if (line) {
        free(line);
    }
}
