#include <stdio.h>

#include "lexer.h"
#include "token.h"

void test() {
	char *source = "let a = 2 * (30 + 400) / 5000; print(1 + 1);";
	Lexer lexer;
	initLexer(&lexer, source);
	for (;;) {
		Token token = getNextToken(&lexer);
		printTokenType(stdout, &token.type);
		printf(" ");
		printToken(stdout, &token);
		printf("\n");
		if (token.type == TT_END) {
			break;
		}
	}
}

int main(void) {
	printf("aquila 0.0.1\n");

	test();
}
