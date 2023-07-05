#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "token.h"

typedef struct Lexer {
	char *start;
	char *current;
	bool has_peeked;
	Token peeked_token;
} Lexer;

void init_lexer(Lexer *lexer, char *source);
Token get_next_token(Lexer *lexer);
Token peek_next_token(Lexer *lexer);

#endif
