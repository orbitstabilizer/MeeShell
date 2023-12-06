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
    TOKEN_ANGLE_BRACKET1,
    TOKEN_ANGLE_BRACKET2,
    TOKEN_ANGLE_BRACKET3,
    TOKEN_AMPERSAND,
    TOKEN_EQUAL,
    TOKEN_EOF,
    TOKEN_QUOTE,
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
typedef struct {
    char *input;
    size_t cur_pos;
    size_t input_len;

    Token list[MAX_TOKEN_LEN];
    size_t special_token;
    size_t cur_token;
    bool quote;
    bool escape;
} Tokenizer;

Tokenizer *Tokenizer__new(char *input, size_t input_len);
void Tokenizer__free(Tokenizer *self);
void Tokenizer__next(Tokenizer *self);
void Tokenizer__consume(Tokenizer *self);
void Tokenizer__get_token(Tokenizer *self, size_t index, char *buffer);
char *Tokenizer_next_literal(Tokenizer *self, size_t *ind, size_t *err);




#endif
