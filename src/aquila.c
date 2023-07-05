#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "chunk.h"
#include "compiler.h"
#include "interpreter.h"

char *readSource(char *path) {
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		perror("fopen");
		exit(EXIT_SUCCESS);
	}

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *source = malloc(fsize + 1);
	fread(source, fsize, 1, file);
	source[fsize] = '\0';

	fclose(file);

	return source;
}

void run(char *source) {
	Lexer lexer;
	initLexer(&lexer, source);

	Chunk chunk;
	initChunk(&chunk);

	Compiler compiler;
	initCompiler(&compiler, &lexer, &chunk);

        printf("=== PARSING ===\n");
	compile(&compiler);

        printChunk(&chunk);

        Interpreter interpreter;
        initInterpreter(&interpreter, &chunk);

        printf("=== INTERPRETING ===\n");
        interpret(&interpreter);

	freeChunk(&chunk);
	freeInterpreter(&interpreter);
}

void test(char *path) {
	char *source = readSource(path);
	run(source);
	free(source);

}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: aquila <path>\n");
		exit(EXIT_FAILURE);
	}
	test(argv[1]);
}
