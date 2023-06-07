#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

typedef enum TokenType {
	TT_END,
	TT_UNKNOWN,

	TT_LET,
	TT_PRINT,

	TT_SEMICOLON,
	TT_LPAREN,
	TT_RPAREN,

	TT_EQUAL,
	TT_PLUS,
	TT_MINUS,
	TT_STAR,
	TT_SLASH,

	TT_NUMBER,
	TT_NAME,
} TokenType;

typedef struct Token {
	TokenType type;
	char *start;
	int length;
} Token;

void printTokenType(FILE *file, TokenType *tokenType);
void printToken(FILE *file, Token *token);

#endif
