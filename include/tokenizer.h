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
 * value: value of token if it is a literal
 */
typedef struct {
    TokenType type;
    char *start;
    size_t length;
} Token;

/* Lexer struct
 * input: input string
 * cur_pos: current position in input string
 * input_len: length of input string
 * token_list: list of tokens
 * cur_token: current token
 */

extern const char* Tokenizer__error_msg[];

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
    size_t special_token;
    size_t cur_token;
    bool escape;
    // methods
    void (*next)(Tokenizer *self);
    void (*free)(Tokenizer *self);
    void (*consume)(Tokenizer *self);
    char *(*next_literal)(Tokenizer *self, size_t ind);

};

Tokenizer *Tokenizer__new(char *input, size_t input_len);
void Tokenizer__free(Tokenizer *self);
void Tokenizer__next(Tokenizer *self);
void Tokenizer__consume(Tokenizer *self);
char *Tokenizer_next_literal(Tokenizer *self, size_t ind);
void print_tokenizer(Tokenizer *this);




#endif
