#include <stdio.h>

#include "chunk.h"
#include "parser.h"
#include "token.h"
#include "interpreter.h"

void test() {
	char *source = 
		"{"
		"	print(1);"
		"	{"
		"		print(2);"
		"	}"
		"}";
	Lexer lexer;
	initLexer(&lexer, source);

	Chunk chunk;
	initChunk(&chunk);

	Parser parser;
	initParser(&parser, &lexer, &chunk);

        printf("=== PARSING ===\n");
	parse(&parser);

        printChunk(&chunk);

        Interpreter interpreter;
        initInterpreter(&interpreter, &chunk);

        printf("=== INTERPRETING ===\n");
        interpret(&interpreter);
}

int main(void) {
	printf("aquila 0.0.1\n");

	test();
}
