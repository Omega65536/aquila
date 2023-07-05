#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdbool.h>

typedef enum TokenType {
        // Special
	TT_END,
	TT_UNKNOWN,

        // Keywords
	TT_LET,
	TT_PRINT,
	TT_TRUE,
	TT_FALSE,
        TT_INTEGER,
        TT_BOOLEAN,

        // Delimiter
	TT_SEMICOLON,
        TT_COLON,
	TT_LPAREN,
	TT_RPAREN,
	TT_LCURLY,
	TT_RCURLY,

        // Operator
	TT_EQUAL,
	TT_PLUS,
	TT_MINUS,
	TT_STAR,
	TT_SLASH,

        TT_DOUBLE_EQUAL,
        TT_NOT_EQUAL,
        TT_LESS,
        TT_LESS_EQUAL,
        TT_GREATER,
        TT_GREATER_EQUAL,

        // Other
	TT_NUMBER,
	TT_NAME,
} TokenType;

typedef struct Token {
	TokenType type;
	char *start;
	int length;
} Token;

bool tokenEqual(Token* a, Token *b);
void printTokenType(FILE *file, TokenType *tokenType);
void printToken(FILE *file, Token *token);

#endif
