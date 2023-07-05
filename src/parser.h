#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "chunk.h"
#include "lexer.h"
#include "type.h"
#include <stdbool.h>

typedef struct Variable {
	Token name;
        Type type;
	int depth;
} Variable;


typedef struct Parser {
	Lexer *lexer;
	Chunk *chunk;

	Variable variables[256];
	int variableCount;
	int depth;

	Type typeStack[256];
	int typeStackSize;
} Parser;

void initParser(Parser *parser, Lexer *lexer, Chunk *chunk);
void parse(Parser *parser);

#endif
