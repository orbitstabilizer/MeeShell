#include "tokenizer.h"
static void init_token(Token *token, TokenType type, size_t len, char *start);

Tokenizer *Tokenizer__new(char *input, size_t input_len) {
    Tokenizer *self = (Tokenizer *)malloc(sizeof(Tokenizer));
    self->input = input;
    self->cur_pos = 0;
    self->input_len = input_len;
    self->cur_token = 0;
    self->quote = 0;
    self->escape = 0;
    return self;
}

void Tokenizer__free(Tokenizer *self) { free(self); }

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
    if (!self->quote)
        if (c == ' ' || c == '\t' || c == '\n') {
            while (c == ' ' || c == '\t' || c == '\n') {
                self->cur_pos++;
                c = self->input[self->cur_pos];
            }
        }
    if (c == '\0' || c == EOF) {
        init_token(token, TOKEN_EOF, 0, NULL);
        self->cur_token++;
        return;
    }
    bool MATCHED = true;

    switch (c) {
    case '"':
        init_token(token, TOKEN_QUOTE, 1, &self->input[self->cur_pos]);
        self->quote = !self->quote;
        break;
    case '&':
        init_token(token, TOKEN_AMPERSAND, 1, &self->input[self->cur_pos]);
        break;
    case '=':
        init_token(token, TOKEN_EQUAL, 1, &self->input[self->cur_pos]);
        break;
    // case '\\':
    //     self->escape = !self->escape;
    //     break;
    default:
        MATCHED = false;
    }
#undef MATCH

    if (MATCHED) {
        self->cur_pos++;
        self->cur_token++;
        return;
    }

    // >, >>, >>>
    if (c == '>') {
        self->cur_pos++;
        c = self->input[self->cur_pos];
        if (c == '>') {
            self->cur_pos++;
            c = self->input[self->cur_pos];
            if (c == '>') {
                self->cur_pos++;
                init_token(token, TOKEN_ANGLE_BRACKET3, 3,
                           &self->input[self->cur_pos - 3]);
            } else {
                init_token(token, TOKEN_ANGLE_BRACKET2, 2,
                           &self->input[self->cur_pos - 2]);
            }
        } else {
            init_token(token, TOKEN_ANGLE_BRACKET1, 1,
                       &self->input[self->cur_pos - 1]);
        }
        self->cur_token++;
        return;
    }

    // go to next empty space or end of input , if quote is on, go to next quote
    if (self->quote) {
        c = self->input[self->cur_pos];
        size_t len = 0;
        while (c != '"' && c != '\0' && c != EOF) {
            len++;
            self->cur_pos++;
            c = self->input[self->cur_pos];
        }
        init_token(token, TOKEN_LITERAL, len,
                   &self->input[self->cur_pos - len]);
        self->cur_token++;
        return;
    } else {
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
        return;
    }
    return;
}

void Tokenizer__consume(Tokenizer *self) {
    do {
        Tokenizer__next(self);
    } while (self->list[self->cur_token - 1].type != TOKEN_EOF);
}

void Tokenizer__get_token(Tokenizer *self, size_t index, char *buffer) {
    if (index >= self->cur_token) {
        buffer[0] = '\0';
        return;
    }
    Token *token = &self->list[index];
    strncpy(buffer, token->start, token->length);
    buffer[token->length] = '\0';
    return;
}

char *Tokenizer_next_literal(Tokenizer *self, size_t *ind, size_t *err) {
    // quoted literal
    if (self->list[*ind].type == TOKEN_QUOTE) {
        (*ind)++;
        if (self->list[(*ind) + 1].type != TOKEN_QUOTE) {
            *err = 1;
            return NULL;
        }
    }
    if (self->list[*ind].type != TOKEN_LITERAL) {
        *err = 2;
        return NULL;
    }
    self->list[*ind].start[self->list[*ind].length] = '\0';
    return self->list[*ind].start;
}

#ifdef DEBUG
void token_name(TokenType type, char *name) {
#define MATCH(type)                                                            \
    case type:                                                                 \
        strcpy(name, #type);                                                   \
        break;
    switch (type) {
        MATCH(TOKEN_LITERAL);
        MATCH(TOKEN_ANGLE_BRACKET1);
        MATCH(TOKEN_ANGLE_BRACKET2);
        MATCH(TOKEN_ANGLE_BRACKET3);
        MATCH(TOKEN_AMPERSAND);
        MATCH(TOKEN_EOF);
        MATCH(TOKEN_QUOTE);
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

// int main(){
//     char *input = "echo \"hello world\" > file.txt >> file2.txt >>>  amu \n";
//     // char * input = "a >>>b";
//     printf("input: `%s`\n", input);
//     size_t input_len = strlen(input);
//     Tokenizer *tokenizer = tokenizer_new(input, input_len);
//     char token[100];
//     do {
//         tokenizer_next(tokenizer);
//     } while (tokenizer->list[tokenizer->cur_token -1].type != TOKEN_EOF);

//     print_tokenizer(tokenizer);

//     tokenizer_free(tokenizer);

// }

#endif
