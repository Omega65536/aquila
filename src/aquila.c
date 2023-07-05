#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "chunk.h"
#include "compiler.h"
#include "interpreter.h"

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

void run(char *source) {
	Lexer lexer;
	init_lexer(&lexer, source);

	Chunk chunk;
	init_chunk(&chunk);

	Compiler compiler;
	init_compiler(&compiler, &lexer, &chunk);

        printf("=== PARSING ===\n");
	compile(&compiler);

        print_chunk(&chunk);

        Interpreter interpreter;
        init_interpreter(&interpreter, &chunk);

        printf("=== COMPILING ===\n");
        interpret(&interpreter);

	free_chunk(&chunk);
	free_interpreter(&interpreter);
}

void test(char *path) {
	char *source = read_source(path);
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
