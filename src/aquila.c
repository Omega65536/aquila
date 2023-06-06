#include <stdio.h>

#include "lexer.h"
#include "token.h"

void test() {
        char *source = "2 * (30 + 400) / 5000";
        Lexer lexer;
        initLexer(&lexer, source);
        for (;;) {
                Token token = getNextToken(&lexer);
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
