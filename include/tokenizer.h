#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LEN 257

extern const char* Tokenizer__error_msg[];
typedef enum {
    TOKEN_LITERAL,
    TOKEN_REDIRECT,
    TOKEN_APPEND,
    TOKEN_RAPPEND,
    TOKEN_AMPERSAND,
    TOKEN_EQUAL,
    TOKEN_EOF,
} TokenType;

/* Token struct
 * type: type of token
 * start: start location of token
 * length: length of token
 */
typedef struct {
    TokenType type;
    char *start;
    size_t length;
} Token;



/* Tokenizer struct
 * input: input string
 * cur_pos: current position in input string
 * input_len: length of input string
 * token_list: list of tokens
 * cur_token: current token
 * escape: whether the current character is escaped
 * err: error code
 *
 */
typedef struct Tokenizer Tokenizer;
struct Tokenizer {
    char *input;
    size_t cur_pos;
    size_t input_len;
    /*
     * 1 : Quoted string not terminated
     */
    int err;
    

    Token list[MAX_TOKEN_LEN];
    size_t cur_token;
    bool escape;
    // methods
    void (*next)(Tokenizer *self);
    void (*free)(Tokenizer *self);
    void (*consume)(Tokenizer *self);
    char *(*next_literal)(Tokenizer *self, size_t ind);

};

/*
 * Create a new tokenizer object, 
 * which will tokenize the input string into tokens
 * Free with obj->free(obj)
 * 
 * Arguments:
 * - char *input: the input string
 * - size_t input_len: the length of the input string
 * 
 * Returns:
 * - Tokenizer *: the tokenizer object
 */
Tokenizer *Tokenizer__new(char *input, size_t input_len);
/*
 * Free the tokenizer object
 * 
 * Arguments:
 * - Tokenizer *self: the tokenizer object
 */
void Tokenizer__free(Tokenizer *self);
/*
 * The main workhorse of the tokenizer, 
 * it iterates through the input string and match a token
 * the function is intended to be called multiple times on the same tokenizer
 * 
 * Arguments:
 * - Tokenizer *self: the tokenizer object
 */
void Tokenizer__next(Tokenizer *self);
/*
 * Consume all the tokens in the tokenizer by repeatedly calling Tokenizer__next
 * 
 * Arguments:
 * - Tokenizer *self: the tokenizer object
 */
void Tokenizer__consume(Tokenizer *self);

/*
 * Get the literal string of the token at index ind, 
 * if the token is not a literal, return NULL
 * Note: the Tokenizer buffer is modified in place,
 * the last character of the literal is replaced with '\0'
 * 
 * Arguments:
 * - Tokenizer *self: the tokenizer object
 * - size_t ind: the index of the token
 * 
 * Returns:
 * - char *: the literal string of the token
 */
char *Tokenizer_next_literal(Tokenizer *self, size_t ind);

/*
 * Print the tokenizer object, for debugging purposes
 * 
 * Arguments:
 * - Tokenizer *self: the tokenizer object
 */
void print_tokenizer(Tokenizer *this);




#endif
