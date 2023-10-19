#include "token.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool token_equal(Token *a, Token *b) {
	if (a->length != b->length) {
		return false;
	}
	return strncmp(a->start, b->start, a->length) == 0;
}

void print_token_type(FILE *file, TokenType *token_type) {
	switch (*token_type) {
		// Special
		case TT_END:
			fprintf(file, "End");
			break;

		// Keywords
		case TT_LET:
			fprintf(file, "'let'");
			break;
		case TT_FUNC:
			fprintf(file, "'func'");
			break;
		case TT_RETURN:
			fprintf(file, "'return'");
			break;
		case TT_PRINT:
			fprintf(file, "'print'");
			break;
		case TT_IF:
			fprintf(file, "'if'");
			break;
		case TT_WHILE:
			fprintf(file, "'while'");
			break;
		case TT_TRUE:
			fprintf(file, "'true'");
			break;
		case TT_FALSE:
			fprintf(file, "'false'");
			break;
		case TT_INTEGER:
			fprintf(file, "'integer'");
			break;
		case TT_BOOLEAN:
			fprintf(file, "'boolean'");
			break;

		// Delimiter
		case TT_SEMICOLON:
			fprintf(file, "';'");
			break;
		case TT_COLON:
			fprintf(file, "':'");
			break;
		case TT_COMMA:
			fprintf(file, "','");
			break;
		case TT_LPAREN:
			fprintf(file, "'('");
			break;
		case TT_RPAREN:
			fprintf(file, "')'");
			break;
		case TT_LCURLY:
			fprintf(file, "'{'");
			break;
		case TT_RCURLY:
			fprintf(file, "'}'");
			break;

		// Operator
		case TT_EQUAL:
			fprintf(file, "'='");
			break;
		case TT_PLUS:
			fprintf(file, "'+'");
			break;
		case TT_MINUS:
			fprintf(file, "'-'");
			break;
		case TT_STAR:
			fprintf(file, "'*'");
			break;
		case TT_SLASH:
			fprintf(file, "'/'");
			break;

		case TT_DOUBLE_EQUAL:
			fprintf(file, "'=='");
			break;
		case TT_NOT_EQUAL:
			fprintf(file, "'!='");
			break;
		case TT_LESS:
			fprintf(file, "'<'");
			break;
		case TT_LESS_EQUAL:
			fprintf(file, "'<='");
			break;
		case TT_GREATER:
			fprintf(file, "'>'");
			break;
		case TT_GREATER_EQUAL:
			fprintf(file, "'>='");
			break;

		// Other
		case TT_NUMBER:
			fprintf(file, "Number");
			break;
		case TT_NAME:
			fprintf(file, "Name");
			break;
		default:
			fprintf(file, "Unknown");
			break;
	}
}

void print_token(FILE *file, Token *token) {
	switch (token->type) {
		case TT_END:
			fprintf(file, "END");
			break;
		case TT_UNKNOWN:
			fprintf(file, "UNKNOWN");
			break;
		default:
			fprintf(file, "'%.*s'", token->length, token->start);
			break;
	}
}
