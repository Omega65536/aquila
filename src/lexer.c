#include "lexer.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static const char *KW_LET = "let";
static const char *KW_PRINT = "print";
static const char *KW_TRUE = "true";
static const char *KW_FALSE = "false";
static const char *KW_INTEGER = "integer";
static const char *KW_BOOLEAN = "boolean";

static Token makeToken(Lexer *lexer, TokenType type);
static Token advanceLexer(Lexer *lexer);
static char nextChar(Lexer *lexer);

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
	for (;;) {
		if (*lexer->current == '\0') {
			return makeToken(lexer, TT_END);
		}

		if (!isspace(*lexer->current)) {
			break;
		}

		lexer->current++;
	}

	lexer->start = lexer->current;
        char ch = nextChar(lexer);

	switch (ch) {
		case ';':
			return makeToken(lexer, TT_SEMICOLON);
                case ':':
			return makeToken(lexer, TT_COLON);
		case '(':
			return makeToken(lexer, TT_LPAREN);
		case ')':
			return makeToken(lexer, TT_RPAREN);
		case '{':
			return makeToken(lexer, TT_LCURLY);
		case '}':
			return makeToken(lexer, TT_RCURLY);
		case '=':
                        if (*lexer->current == '=') {
                            nextChar(lexer);
                            return makeToken(lexer, TT_DOUBLE_EQUAL);
                        }
			return makeToken(lexer, TT_EQUAL);
		case '+':
			return makeToken(lexer, TT_PLUS);
		case '-':
			return makeToken(lexer, TT_MINUS);
		case '*':
			return makeToken(lexer, TT_STAR);
		case '/':
			return makeToken(lexer, TT_SLASH);
                case '!':
                        if (*lexer->current == '=') {
                            nextChar(lexer);
                            return makeToken(lexer, TT_NOT_EQUAL);
                        }
                        break;
                case '<':
                        if (*lexer->current == '=') {
                            nextChar(lexer);
                            return makeToken(lexer, TT_LESS_EQUAL);
                        }
                        return makeToken(lexer, TT_LESS);
                case '>':
                        if (*lexer->current == '=') {
                            nextChar(lexer);
                            return makeToken(lexer, TT_GREATER_EQUAL);
                        }
                        return makeToken(lexer, TT_GREATER);
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
		if (strncmp(name, KW_LET, strlen(KW_LET)) == 0) {
			return makeToken(lexer, TT_LET);
		} else if (strncmp(name, KW_PRINT, strlen(KW_PRINT)) == 0) {
			return makeToken(lexer, TT_PRINT);
		} else if (strncmp(name, KW_TRUE, strlen(KW_TRUE)) == 0) {
			return makeToken(lexer, TT_TRUE);
		} else if (strncmp(name, KW_FALSE, strlen(KW_FALSE)) == 0) {
			return makeToken(lexer, TT_FALSE);
		} else if (strncmp(name, KW_INTEGER, strlen(KW_INTEGER)) == 0) {
			return makeToken(lexer, TT_INTEGER);
		} else if (strncmp(name, KW_BOOLEAN, strlen(KW_BOOLEAN)) == 0) {
			return makeToken(lexer, TT_BOOLEAN);
		}

		return makeToken(lexer, TT_NAME);
	}

	return makeToken(lexer, TT_UNKNOWN);
}

static char nextChar(Lexer *lexer) {
	char ch = *lexer->current;
	lexer->current++;
        return ch;
}
