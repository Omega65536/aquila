#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stdbool.h>

typedef struct Lexer {
	char *start;
	char *current;
	bool has_peeked;
	Token peeked_token;
	int line_number;
} Lexer;

void init_lexer(Lexer *lexer, char *source);
Token get_next_token(Lexer *lexer);
Token peek_next_token(Lexer *lexer);

#endif
