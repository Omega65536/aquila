#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "interpreter.h"
#include "lexer.h"

char *read_source(char *path) {
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

void run(char *source, bool only_compile) {
	Lexer lexer;
	init_lexer(&lexer, source);

	Chunk chunk;
	init_chunk(&chunk);

	Compiler compiler;
	init_compiler(&compiler, &lexer, &chunk);

	compile(&compiler);

	if (only_compile) {
		print_chunk(&chunk);
		free_chunk(&chunk);
		return;
	}

	Interpreter interpreter;
	init_interpreter(&interpreter, &chunk);

	interpret(&interpreter);

	free_chunk(&chunk);
	free_interpreter(&interpreter);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage: aquila <path>\n");
		exit(EXIT_FAILURE);
	}

	bool only_compile = false;
	for (int i = 2; i < argc; i++) {
		if (strcmp(argv[i], "-C") == 0) {
			only_compile = true;
		}
	}

	char *source = read_source(argv[1]);
	run(source, only_compile);
	free(source);
}
