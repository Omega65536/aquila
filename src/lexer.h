#ifndef LEXER_H
#define LEXER_H

#include "stdbool.h"
#include "token.h"

typedef struct Lexer {
        char *start;
        char *current;
        Token peekedToken;
        bool hasPeeked;
} Lexer;

void initLexer(Lexer *lexer, char *source);
Token makeToken(Lexer *lexer, TokenType type);
Token getNextToken(Lexer *lexer);
Token peekNextToken(Lexer *lexer);

#endif
