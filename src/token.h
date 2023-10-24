#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
#include <stdio.h>

typedef enum TokenType {
	// Special
	TT_END,
	TT_UNKNOWN,

	// Keywords
	TT_FUNC,
	TT_RETURN,
	TT_LET,
	TT_PRINT,
	TT_IF,
	TT_WHILE,
	TT_TRUE,
	TT_FALSE,
        TT_UNIT,
	TT_INTEGER,
	TT_BOOLEAN,

	// Delimiter
	TT_SEMICOLON,
	TT_COLON,
	TT_COMMA,
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

bool token_equal(Token *a, Token *b);
void print_token_type(FILE *file, TokenType *token_type);
void print_token(FILE *file, Token *token);

#endif
