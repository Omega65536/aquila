#include <stdio.h>

#include "chunk.h"
#include "parser.h"
#include "token.h"

void test() {
	char *source = "let a = 2 * (30 + 400) / 5000; print(6 + 7);";
	Lexer lexer;
	initLexer(&lexer, source);

	Chunk chunk;
	initChunk(&chunk);

	Parser parser;
	initParser(&parser, &lexer, &chunk);

	parse(&parser);

	printChunk(&chunk);
}

int main(void) {
	printf("aquila 0.0.1\n");

	test();
}
