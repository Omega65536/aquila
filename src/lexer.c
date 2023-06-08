#include "lexer.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static const char *KW_LET = "let";
static const char *KW_PRINT = "print";

static Token makeToken(Lexer *lexer, TokenType type);
static Token advanceLexer(Lexer *lexer);

void initLexer(Lexer *lexer, char *source) {
	lexer->start = source;
	lexer->current = source;
	lexer->hasPeeked = false;
}

Token makeToken(Lexer *lexer, TokenType type) {
	Token token;
	token.type = type;
	token.start = lexer->start;
	token.length = (int) (lexer->current - lexer->start);
	return token;
}

Token getNextToken(Lexer *lexer) {
	if (lexer->hasPeeked) {
		lexer->hasPeeked = false;
		return lexer->peekedToken;
	}
	return advanceLexer(lexer);
}

Token peekNextToken(Lexer *lexer) {
	if (lexer->hasPeeked) {
		return lexer->peekedToken;
	}
	lexer->hasPeeked = true;
	lexer->peekedToken = advanceLexer(lexer);
	return lexer->peekedToken;
}

Token advanceLexer(Lexer *lexer) {
	while (isspace(*lexer->current)) {
		lexer->current++;
	}

	if (*lexer->current == '\0') {
		return makeToken(lexer, TT_END);
	}

	char ch = *lexer->current;
	lexer->start = lexer->current;
	lexer->current++;

	switch (ch) {
		case ';':
			return makeToken(lexer, TT_SEMICOLON);
		case '(':
			return makeToken(lexer, TT_LPAREN);
		case ')':
			return makeToken(lexer, TT_RPAREN);
		case '{':
			return makeToken(lexer, TT_LCURLY);
		case '}':
			return makeToken(lexer, TT_RCURLY);
		case '=':
			return makeToken(lexer, TT_EQUAL);
		case '+':
			return makeToken(lexer, TT_PLUS);
		case '-':
			return makeToken(lexer, TT_MINUS);
		case '*':
			return makeToken(lexer, TT_STAR);
		case '/':
			return makeToken(lexer, TT_SLASH);
		default:
			break;
	}

	if (isdigit(ch)) {
		while (isdigit(*lexer->current)) {
			lexer->current++;
		}
		return makeToken(lexer, TT_NUMBER);
	}

	if (isalpha(ch)) {
		while (isalpha(*lexer->current)) {
			lexer->current++;
		}

		char *name = lexer->start;
		if (strncmp(name, KW_LET, 3) == 0) {
			return makeToken(lexer, TT_LET);
		} else if (strncmp(name, KW_PRINT, 3) == 0) {
			return makeToken(lexer, TT_PRINT);
		}

		return makeToken(lexer, TT_NAME);
	}

	return makeToken(lexer, TT_UNKNOWN);
}
