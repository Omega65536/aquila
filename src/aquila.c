#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "parser.h"
#include "token.h"
#include "interpreter.h"

void test() {
	FILE *file = fopen("../test.aq", "r");
	if (file == NULL) {
		perror("fopen");
		exit(EXIT_SUCCESS);
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *source = malloc(fsize + 1);
	fread(source, fsize, 1, file);

	fclose(file);

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
