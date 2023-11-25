#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"
static Chain *put(Dictionary* dict, char *name, long value);
static Chain *get(Dictionary* dict, char *s);

Dictionary* new_dictionary(){
  Dictionary* dict = (Dictionary*) malloc(sizeof(Dictionary));
  for (int i = 0; i < HASHSIZE; i++)
    dict->hashtab[i] = NULL;
  return dict;
}

/* hash: polynomial hash for string s */
unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
      hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
Chain *get(Dictionary* dict, char *s)
{
    Chain *np;
    for (np = dict->hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
          return np; /* found */
    return NULL; /* not found */
}


Chain *put(Dictionary* dict, char *name, long value)
{
    Chain *np;
    unsigned hashval;
    if ((np = get(dict, name)) == NULL) { /* not found */
        np = (Chain *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
          return NULL;
        hashval = hash(name);
        np->next = dict->hashtab[hashval];
        dict->hashtab[hashval] = np;
    } 
    np->value = value;
    return np;
}

void print_dict(Dictionary* dict){
  Chain* bin;
  for (int i = 0; i < HASHSIZE; i++){
    bin = dict->hashtab[i];
    if (bin != NULL){
      printf("bin %d: ", i);
      while (bin != NULL){
        printf("%s = %ld, ", bin->name, bin->value);
        bin = bin->next;
      }
      printf("\n");
    }
  }
}

void free_dict(Dictionary* dict){
  Chain* bin;
  Chain* next;
  for (int i = 0; i < HASHSIZE; i++){
    bin = dict->hashtab[i];
    while (bin != NULL){
      next = bin->next;
      free(bin->name);
      free(bin);
      bin = next;
    }
  }
  free(dict);
}

void set_var(Dictionary *dict, char *name, long value){
  put(dict, name, value);
}

long get_var(Dictionary *dict, char *name){
  Chain* bin = get(dict, name);
  if(bin == NULL){
    // if varialbe is not declared 
    return 0;
  }
  return bin->value;
}

/* int main(){
  // put("alis", "1");
  Dictionary* dict = new_dictionary();
  set_var(dict, "alis", 31);
  // put("b", "2");
  set_var(dict, "aD", 2);
  printf("alis = %ld\n", get_var(dict, "alis"));
  set_var(dict,  "alis", 54);
  printf("alis = %ld\n", get_var(dict, "alis"));
  printf("aD = %ld\n", get_var(dict, "aD"));
  printf("var = %ld\n", get_var(dict, "var"));
  set_var(dict, "one", 1);  
  set_var(dict, "two", 2);
  set_var(dict, "three", 3);
  set_var(dict, "four", 4);
  set_var(dict, "five", 5);
  set_var(dict, "six", 6);
  set_var(dict, "seven", 7);
  set_var(dict, "eight", 8);
  set_var(dict, "nine", 9);
  set_var(dict, "ten", 10);
  set_var(dict, "eleven", 11);
  set_var(dict, "twelve", 12);
  set_var(dict, "thirteen", 13);
  set_var(dict, "fourteen", 14);
  set_var(dict, "fifteen", 15);
  set_var(dict, "sixteen", 16);
  set_var(dict, "seventeen", 17);
  set_var(dict, "eighteen", 18);
  set_var(dict, "nineteen", 19);
  set_var(dict, "twenty", 20);
  set_var(dict, "twentyone", 21);
  set_var(dict, "twentytwo", 22);
  set_var(dict, "twentythree", 23);
  set_var(dict, "twentyfour", 24);
  set_var(dict, "twentyfive", 25);
  set_var(dict, "twentysix", 26);
  set_var(dict, "twentyseven", 27);
  set_var(dict, "twentyeight", 28);
  set_var(dict, "twentynine", 29);
  set_var(dict, "thirty", 30);
  set_var(dict, "thirtyone", 31);
  
  // print_table();
  printf("twentytwo = %ld\n", get_var(dict, "twentytwo"));
  printf("twentynine = %ld\n", get_var(dict, "twentynine"));
  print_dict(dict);
  free_dict(dict);
  return 0;

  } */

