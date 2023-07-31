#ifndef COMPILER_H
#define COMPILER_H

#include "chunk.h"
#include "lexer.h"
#include "token.h"
#include "type.h"
#include "variable.h"
#include <stdbool.h>

typedef struct Compiler {
	Lexer *lexer;
	Chunk *chunk;

	VariableStack variable_stack;
	Type type_stack[256];
	int type_stackSize;
} Compiler;

void init_compiler(Compiler *compiler, Lexer *lexer, Chunk *chunk);
void compile(Compiler *compile);

#endif
