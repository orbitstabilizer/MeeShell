#ifndef DICTIONARY_H
#define DICTIONARY_H
#define HASHSIZE 31013
#define KEY_LEN 1000
#define VALUE_LEN 1000

/*
 * Chain struct
 * next: pointer to next entry in chain
 * name: name of variable
 * value: value of variable
 */
typedef struct Chain { /* table entry: */
    struct Chain *next; /* next entry in chain */
    char *key; /* defined name */
    char *value;
}Chain;

/*
 * Dictionary struct
 * hashtab: array of pointers to chains
 */
typedef struct Dictionary{
  Chain* hashtab[HASHSIZE];   
} Dictionary;

/*
 * Create a new dictionary
 */
Dictionary* new_dictionary();

/*
 * Set a variable in the dictionary
 * dict: dictionary to set variable in
 * name: name of variable
 * value: value of variable
 */
void set_var(Dictionary *dict, char *key, char *value);

/*
 * Get a variable from the dictionary
 * dict: dictionary to get variable from
 * name: name of variable
 */
char* get_var(Dictionary *dict, char *key);

/*
 * Free a dictionary
 * dict: dictionary to free
 */
void free_dict(Dictionary* dict);

/*
 * Print a dictionary
 * dict: dictionary to print
 */
void print_dict(Dictionary* dict);

/*
 * Hash function
 * s: string to hash
 */
unsigned hash(char *s);



/*
 * Load a dictionary from a file
 * dict: dictionary to load into
 * filename: file to load from
 */
void load_dict(Dictionary *dict, char *filename);


/*
 * Save a dictionary to a file
 * dict: dictionary to save
 * filename: file to save to
 */
void dump_dict(Dictionary *dict, char *filename);

#endif



