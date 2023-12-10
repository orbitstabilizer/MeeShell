#include "dictionary.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Chain *put(Dict *dict, char *key, char *value);
static Chain *get(Dict *dict, char *key);
static unsigned hash(char *s);

Dict *Dict__new() {
    Dict *dict = (Dict *)malloc(sizeof(Dict));
    for (int i = 0; i < HASHSIZE; i++)
        dict->hashtab[i] = NULL;
    return dict;
}

/* hash: polynomial hash for string s */
unsigned hash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
Chain *get(Dict *dict, char *key) {
    Chain *np;
    for (np = dict->hashtab[hash(key)]; np != NULL; np = np->next)
        if (strcmp(key, np->key) == 0)
            return np; /* found */
    return NULL;       /* not found */
}

Chain *put(Dict *dict, char *key, char *value) {
    Chain *np;
    unsigned hashval;
    if ((np = get(dict, key)) == NULL) { /* not found */
        np = (Chain *)malloc(sizeof(*np));
        if (np == NULL || (np->key = strdup(key)) == NULL)
            return NULL;
        hashval = hash(key);
        np->next = dict->hashtab[hashval];
        dict->hashtab[hashval] = np;
    }
    np->value = value;
    return np;
}

void Dict__print(Dict *dict) {
    Chain *bin;
    for (int i = 0; i < HASHSIZE; i++) {
        bin = dict->hashtab[i];
        if (bin != NULL) {
            printf("bin %d: ", i);
            while (bin != NULL) {
                printf("%s = %s, ", bin->key, bin->value);
                bin = bin->next;
            }
            printf("\n");
        }
    }
}

void Dict__free(Dict *dict) {
    Chain *bin;
    Chain *next;
    for (int i = 0; i < HASHSIZE; i++) {
        bin = dict->hashtab[i];
        while (bin != NULL) {
            next = bin->next;
            free(bin->key);
            if (bin->value != NULL)
                free(bin->value);
            free(bin);
            bin = next;
        }
    }
    free(dict);
}

void Dict__set(Dict *dict, char *key, char *value) {
    char *val_copy = strdup(value);
    put(dict, key, val_copy);
}

char *Dict__get(Dict *dict, char *key) {
    Chain *bin = get(dict, key);
    if (bin == NULL) {
        return 0;
    }
    return bin->value;
}

int Dict__del(Dict *dict, char *key) {
    Chain *bin = get(dict, key);
    if (bin == NULL) {
        return 0;
    }
    unsigned hashval = hash(key);
    dict->hashtab[hashval] = bin->next;
    free(bin->key);
    free(bin->value);
    free(bin);
    return 1;
}

void Dict__dump(Dict *dict, char *filename) {
    // if file exists, overwrite it
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        LOG_ERROR("Cannot open file %s", filename);
        exit(1);
    }
    Chain *bin;
    for (int i = 0; i < HASHSIZE; i++) {
        bin = dict->hashtab[i];
        if (bin != NULL) {
            while (bin != NULL) {
                fprintf(fp, "%s\n", bin->key);
                fprintf(fp, "%s\n", bin->value);
                bin = bin->next;
            }
        }
    }
}

void Dict__load(Dict *dict, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        LOG_ERROR("Cannot open file %s", filename);
        // create a new file
        fp = fopen(filename, "w");
        if (fp == NULL) {
            LOG_ERROR("Cannot create file %s", filename);
            exit(1);
        } else {
            fclose(fp);
        }

        return;
    }
    char key[KEY_LEN];
    char value[VALUE_LEN];
    while (fgets(key, KEY_LEN, fp) != NULL) {
        key[strlen(key) - 1] = '\0';
        fgets(value, VALUE_LEN, fp);
        value[strlen(value) - 1] = '\0';
        Dict__set(dict, key, value);
    }
}
