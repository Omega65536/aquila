#include "token.h"
#include <stdio.h>

void printTokenType(FILE *file, TokenType *tokenType) {
	switch (*tokenType) {
		case TT_END:
			fprintf(file, "End");
			break;
		case TT_LET:
			fprintf(file, "'let'");
			break;
		case TT_SEMICOLON:
			fprintf(file, "';'");
			break;
		case TT_LPAREN:
			fprintf(file, "'('");
			break;
		case TT_RPAREN:
			fprintf(file, "')'");
			break;
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

void printToken(FILE *file, Token *token) {
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
