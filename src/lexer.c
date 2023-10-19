#include "lexer.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static const char *KW_LET = "let";
static const char *KW_FUNC = "func";
static const char *KW_RETURN = "return";
static const char *KW_PRINT = "print";
static const char *KW_IF = "if";
static const char *KW_WHILE = "while";
static const char *KW_TRUE = "true";
static const char *KW_FALSE = "false";
static const char *KW_INTEGER = "integer";
static const char *KW_BOOLEAN = "boolean";

static Token make_token(Lexer *lexer, TokenType type);
static Token advance_lexer(Lexer *lexer);
static char next_char(Lexer *lexer);

void init_lexer(Lexer *lexer, char *source) {
	lexer->start = source;
	lexer->current = source;
	lexer->has_peeked = false;
}

Token make_token(Lexer *lexer, TokenType type) {
	Token token;
	token.type = type;
	token.start = lexer->start;
	token.length = (int) (lexer->current - lexer->start);

	return token;
}

Token get_next_token(Lexer *lexer) {
	if (lexer->has_peeked) {
		lexer->has_peeked = false;
		return lexer->peeked_token;
	}
	return advance_lexer(lexer);
}

Token peek_next_token(Lexer *lexer) {
	if (lexer->has_peeked) {
		return lexer->peeked_token;
	}

	lexer->has_peeked = true;
	lexer->peeked_token = advance_lexer(lexer);
	return lexer->peeked_token;
}

Token advance_lexer(Lexer *lexer) {
	for (;;) {
		if (*lexer->current == '\0') {
			return make_token(lexer, TT_END);
		}

		if (!isspace(*lexer->current)) {
			break;
		}

		next_char(lexer);
	}

	lexer->start = lexer->current;
	char ch = next_char(lexer);

	switch (ch) {
		case ';':
			return make_token(lexer, TT_SEMICOLON);
		case ':':
			return make_token(lexer, TT_COLON);
		case ',':
			return make_token(lexer, TT_COMMA);
		case '(':
			return make_token(lexer, TT_LPAREN);
		case ')':
			return make_token(lexer, TT_RPAREN);
		case '{':
			return make_token(lexer, TT_LCURLY);
		case '}':
			return make_token(lexer, TT_RCURLY);
		case '=':
			if (*lexer->current == '=') {
				next_char(lexer);
				return make_token(lexer, TT_DOUBLE_EQUAL);
			}
			return make_token(lexer, TT_EQUAL);
		case '+':
			return make_token(lexer, TT_PLUS);
		case '-':
			return make_token(lexer, TT_MINUS);
		case '*':
			return make_token(lexer, TT_STAR);
		case '/':
			return make_token(lexer, TT_SLASH);
		case '!':
			if (*lexer->current == '=') {
				next_char(lexer);
				return make_token(lexer, TT_NOT_EQUAL);
			}
			break;
		case '<':
			if (*lexer->current == '=') {
				next_char(lexer);
				return make_token(lexer, TT_LESS_EQUAL);
			}
			return make_token(lexer, TT_LESS);
		case '>':
			if (*lexer->current == '=') {
				next_char(lexer);
				return make_token(lexer, TT_GREATER_EQUAL);
			}
			return make_token(lexer, TT_GREATER);
		default:
			break;
	}

	if (isdigit(ch)) {
		while (isdigit(*lexer->current)) {
			next_char(lexer);
		}
		return make_token(lexer, TT_NUMBER);
	}

	if (isalpha(ch)) {
		while (isalpha(*lexer->current)) {
			next_char(lexer);
		}

		char *name = lexer->start;
		if (strncmp(name, KW_LET, strlen(KW_LET)) == 0) {
			return make_token(lexer, TT_LET);
		} else if (strncmp(name, KW_FUNC, strlen(KW_FUNC)) == 0) {
			return make_token(lexer, TT_FUNC);
		} else if (strncmp(name, KW_RETURN, strlen(KW_RETURN)) == 0) {
			return make_token(lexer, TT_RETURN);
		} else if (strncmp(name, KW_PRINT, strlen(KW_PRINT)) == 0) {
			return make_token(lexer, TT_PRINT);
		} else if (strncmp(name, KW_IF, strlen(KW_IF)) == 0) {
			return make_token(lexer, TT_IF);
		} else if (strncmp(name, KW_WHILE, strlen(KW_WHILE)) == 0) {
			return make_token(lexer, TT_WHILE);
		} else if (strncmp(name, KW_TRUE, strlen(KW_TRUE)) == 0) {
			return make_token(lexer, TT_TRUE);
		} else if (strncmp(name, KW_FALSE, strlen(KW_FALSE)) == 0) {
			return make_token(lexer, TT_FALSE);
		} else if (strncmp(name, KW_INTEGER, strlen(KW_INTEGER)) == 0) {
			return make_token(lexer, TT_INTEGER);
		} else if (strncmp(name, KW_BOOLEAN, strlen(KW_BOOLEAN)) == 0) {
			return make_token(lexer, TT_BOOLEAN);
		}

		return make_token(lexer, TT_NAME);
	}

	return make_token(lexer, TT_UNKNOWN);
}

static char next_char(Lexer *lexer) {
	return *lexer->current++;
}
