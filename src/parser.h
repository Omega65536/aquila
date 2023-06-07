#ifndef PARSER_H
#define PARSER_H

#include "chunk.h"
#include "lexer.h"
#include "token.h"
#include <stdbool.h>

typedef struct Parser {
	Lexer *lexer;
	Chunk *chunk;

	bool hasError;
} Parser;

void initParser(Parser *parser, Lexer *lexer, Chunk *chunk);
void parse(Parser *parser);

#endif
