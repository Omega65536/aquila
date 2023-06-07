#ifndef LEXER_H
#define LEXER_H

#include "stdbool.h"
#include "token.h"

typedef struct Lexer {
        char *start;
        char *current;
        bool hasPeeked;
        Token peekedToken;
} Lexer;

void initLexer(Lexer *lexer, char *source);
Token getNextToken(Lexer *lexer);
Token peekNextToken(Lexer *lexer);

#endif
