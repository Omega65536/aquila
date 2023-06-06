#include "lexer.h"
#include "token.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
        token.length = (int)(lexer->current - lexer->start);
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
        while (*lexer->current == ' ') {
                lexer->current++;
        }

        if (*lexer->current == '\0') {
                return makeToken(lexer, TT_END);
        }

        char ch = *lexer->current;
        lexer->start = lexer->current;
        lexer->current++;

        // printf("LEXER: [%s] - [%s]\n", lexer->start, lexer->current);

        switch (ch) {
                case ';':
                        return makeToken(lexer, TT_SEMICOLON);
                case '(':
                        return makeToken(lexer, TT_LPAREN);
                case ')':
                        return makeToken(lexer, TT_RPAREN);
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

		char* name = lexer->start;
		if (name[0] == 'l' && name[1] == 'e' && name[2] == 't') {
			return makeToken(lexer, TT_LET);
		}
		return makeToken(lexer, TT_NAME);
	}

        return makeToken(lexer, TT_UNKNOWN);
}
