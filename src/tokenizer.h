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
  TOKEN_EOF,   
  TOKEN_QUOTE,  
} TokenType;



/* Token struct
 * type: type of token
 * start: start location of token
 * length: length of token
 * value: value of token if it is a literal
 */
typedef struct{
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
}Tokenizer;


Tokenizer *tokenizer_new(char *input, size_t input_len);
void tokenizer_free(Tokenizer *self);
void tokenizer_next(Tokenizer *self);


#ifdef DEBUG
void token_name(TokenType type, char *name){
    #define MATCH(type) \
        case type: \
            strcpy(name, #type); \
            break;
    switch (type){
        MATCH(TOKEN_LITERAL);
        MATCH(TOKEN_ANGLE_BRACKET1);
        MATCH(TOKEN_ANGLE_BRACKET2);
        MATCH(TOKEN_ANGLE_BRACKET3);
        MATCH(TOKEN_AMPERSAND);
        MATCH(TOKEN_EOF);
        MATCH(TOKEN_QUOTE);
        default:
            strcpy(name, "UNKNOWN");
    }
    #undef MATCH
}


void print_tokenizer(Tokenizer *this){
    printf("Tokenizer{\n\tinput: `%s`\n\tcur_pos: %zu\n\tinput_len: %zu\n\tcur_token: %zu\n",
           this->input, this->cur_pos, this->input_len, this->cur_token);
    printf("\tToken List:{\n");
    for (size_t i = 0; i < this->cur_token; i++){
        Token *token = &this->list[i];
        char sub_str[token->length + 1];
        strncpy(sub_str, token->start, token->length);
        sub_str[token->length] = '\0';

        char name[20];
        token_name(token->type, name);
        
        printf("\t\tToken{\n\t\t\ttype: %s\n\t\t\tlength: %zu\n\t\t\tstart: '%s'\n\t\t},\n",
                name, token->length, sub_str);
    }
    printf("\t}\n");
    printf("}\n");

}

int main(){
    char *input = "echo \"hello world\" > file.txt >> file2.txt >>>  amu \n";
    // char * input = "a >>>b";
    printf("input: `%s`\n", input);
    size_t input_len = strlen(input);
    Tokenizer *tokenizer = tokenizer_new(input, input_len);
    char token[100];
    do {
        tokenizer_next(tokenizer);
    } while (tokenizer->list[tokenizer->cur_token -1].type != TOKEN_EOF);

    print_tokenizer(tokenizer);

    tokenizer_free(tokenizer);

}


#endif



#endif

