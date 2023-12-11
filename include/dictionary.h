#ifndef DICTIONARY_H
#define DICTIONARY_H
#define HASHSIZE 31013
#define KEY_LEN 1000
#define VALUE_LEN 1000

/*
 * Chain struct
 * next: pointer to next entry in chain
 * key: name of variable
 * value: value of variable
 */
typedef struct Chain {  /* table entry: */
    struct Chain *next; /* next entry in chain */
    char *key;          /* defined name */
    char *value;
} Chain;

/*
 * Dictionary struct
 * hashtab: array of pointers to chains
 */
typedef struct {
    Chain *hashtab[HASHSIZE];
} Dict;

/*
 * Create a new dictionary, which is a hash map implemented with chaining
 */
Dict *Dict__new();

/*
 * Set a variable in the dictionary
 * dict: dictionary to set variable in
 * key: name of variable
 * value: value of variable
 */
void Dict__set(Dict *dict, char *key, char *value);

/*
 * Get a variable from the dictionary
 * dict: dictionary to get variable from
 * key: name of variable
 */
char *Dict__get(Dict *dict, char *key);

/*
 * Free a dictionary
 * dict: dictionary to free
 */
void Dict__free(Dict *dict);

/*
 * Print a dictionary
 * dict: dictionary to print
 */
void Dict__print(Dict *dict);

/*
 * Load a dictionary from a file
 * 
 * dict: dictionary to load into
 * filename: file to load from
 */
void Dict__load(Dict *dict, char *filename);

/*
 * Save a dictionary to a file
 * dict: dictionary to save
 * filename: file to save to
 */
void Dict__dump(Dict *dict, char *filename);

/*
 * Unset a variable in the dictionary
 * dict: dictionary to unset variable in
 * key: name of variable
 */
int Dict__del(Dict *dict, char *key);

#endif
