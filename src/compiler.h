#ifndef COMPILER_H
#define COMPILER_H

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


typedef struct Compiler {
	Lexer *lexer;
	Chunk *chunk;

	Variable variables[256];
	int variable_count;
	int depth;

	Type type_stack[256];
	int type_stackSize;
} Compiler;

void init_compiler(Compiler *compiler, Lexer *lexer, Chunk *chunk);
void compile(Compiler *compile);

#endif
