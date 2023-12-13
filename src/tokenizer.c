#include "tokenizer.h"
static void init_token(Token *token, TokenType type, size_t len, char *start);
 
const char* Tokenizer__error_msg[] = {
    "No error",
    "Quoted string not terminated\n",
};


Tokenizer *Tokenizer__new(char *input, size_t input_len) {
    Tokenizer *self = (Tokenizer *)malloc(sizeof(Tokenizer));
    self->input = strdup(input);
    self->cur_pos = 0;
    self->input_len = input_len;
    self->cur_token = 0;
    self->escape = 0;
    self->err = 0;

    self->next = Tokenizer__next;
    self->free = Tokenizer__free;
    self->consume = Tokenizer__consume;
    // self->get_token = Tokenizer__get_token;
    self->next_literal = Tokenizer_next_literal;

    self->consume(self);
    return self;
}


void Tokenizer__free(Tokenizer *self) {
    free(self->input);
    free(self); 
}

/*
 * Initialize a token object
 * 
 * Arguments:
 * - Token *token: the token object
 * - TokenType type: the type of the token
 * - size_t len: the length of the token
 * - char *start: the start of the token
 */
void init_token(Token *token, TokenType type, size_t len, char *start) {
    token->type = type;
    token->length = len;
    token->start = start;
}


void Tokenizer__next(Tokenizer *self) {
    Token *token = &self->list[self->cur_token];
    if (self->cur_pos >= self->input_len) {
        init_token(token, TOKEN_EOF, 0, NULL);
        self->cur_token++;
        return;
    }
    char c = self->input[self->cur_pos];
    if (c == ' ' || c == '\t' || c == '\n') {
        while (c == ' ' || c == '\t' || c == '\n') {
            self->cur_pos++;
            c = self->input[self->cur_pos];
        }
    }
    else if (c == '\0' || c == EOF) {
        init_token(token, TOKEN_EOF, 0, NULL);
        self->cur_token++;
        return;
    }
    else if (c == '&'){
        init_token(token, TOKEN_AMPERSAND, 1, &self->input[self->cur_pos]);
        self->cur_pos++;
        self->cur_token++;
        return;
    }
    else if (c == '='){
        init_token(token, TOKEN_EQUAL, 1, &self->input[self->cur_pos]);
        self->cur_pos++;
        self->cur_token++;
        return;
    }
    // >, >>, >>>
    else if (c == '>') {
        self->cur_pos++;
        c = self->input[self->cur_pos];
        if (c == '>') {
            self->cur_pos++;
            c = self->input[self->cur_pos];
            if (c == '>') {
                self->cur_pos++;
                init_token(token, TOKEN_RAPPEND, 3,
                           &self->input[self->cur_pos - 3]);
            } else {
                init_token(token, TOKEN_APPEND, 2,
                           &self->input[self->cur_pos - 2]);
            }
        } else {
            init_token(token, TOKEN_REDIRECT, 1,
                       &self->input[self->cur_pos - 1]);
        }
        self->cur_token++;
        return;
    }
    else if (c == '"') {
        self->cur_pos++;
        size_t start = self->cur_pos;
        c = self->input[self->cur_pos];
        while (c != '"' && c != '\0' && c != EOF) {
            self->cur_pos++;
            c = self->input[self->cur_pos];
        }
        if ( c != '"'){// quote not closed
            self->err = 1;
            return;
        }
        init_token(token, TOKEN_LITERAL, self->cur_pos - start,
                   &self->input[start]);
        self->cur_token++;
        self->cur_pos++;
        return;
    }
    else{
        // go to next empty space or end of input , if quote is on, go to next quote
        c = self->input[self->cur_pos];
        size_t len = 0;
        while (c != ' ' && c != '\t' && c != '\n' && c != '\0' && c != EOF &&
               c != '"' && c != '&' && c != '=' && c != '>') {
            len++;
            self->cur_pos++;
            c = self->input[self->cur_pos];
        }
        init_token(token, TOKEN_LITERAL, len,
                   &self->input[self->cur_pos - len]);
        self->cur_token++;
    }
    return;
}


void Tokenizer__consume(Tokenizer *self) {
    do {
        Tokenizer__next(self);
        if (self->err != 0){
            return;
        }
    } while ( self->list[self->cur_token - 1].type != TOKEN_EOF);
}

char *Tokenizer_next_literal(Tokenizer *self, size_t ind){
    if (self->list[ind].type != TOKEN_LITERAL) {
        return NULL;
    }
    self->list[ind].start[self->list[ind].length] = '\0';
    return self->list[ind].start;
}

void token_name(TokenType type, char *name) {
#define MATCH(type)                                                            \
    case type:                                                                 \
        strcpy(name, #type);                                                   \
        break;
    switch (type) {
        MATCH(TOKEN_LITERAL);
        MATCH(TOKEN_REDIRECT);
        MATCH(TOKEN_APPEND);
        MATCH(TOKEN_RAPPEND);
        MATCH(TOKEN_AMPERSAND);
        MATCH(TOKEN_EOF);
        MATCH(TOKEN_EQUAL);
    default:
        strcpy(name, "UNKNOWN");
    }
#undef MATCH
}

void print_tokenizer(Tokenizer *this) {
    printf("Tokenizer{\n\tinput: `%s`\n\tcur_pos: %zu\n\tinput_len: "
           "%zu\n\tcur_token: %zu\n",
           this->input, this->cur_pos, this->input_len, this->cur_token);
    printf("\tToken List:{\n");
    for (size_t i = 0; i < this->cur_token; i++) {
        Token *token = &this->list[i];
        char sub_str[token->length + 1];
        strncpy(sub_str, token->start, token->length);
        sub_str[token->length] = '\0';

        char name[20];
        token_name(token->type, name);

        printf("\t\tToken{\n\t\t\ttype: %s\n\t\t\tlength: %zu\n\t\t\tstart: "
               "'%s'\n\t\t},\n",
               name, token->length, sub_str);
    }
    printf("\t}\n");
    printf("}\n");
}

