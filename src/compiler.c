#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "lexer.h"
#include "chunk.h"
#include "token.h"
#include "type.h"

static void compileStatement(Compiler *compiler);
static void compileBlock(Compiler *compiler);
static void compileLet(Compiler *compiler);
static Type compileType(Compiler *compiler);
static void compilePrint(Compiler *compiler);

static void compileExpression(Compiler *compiler);
static void compileComparison(Compiler *compiler);
static void compileAdditionOrSubtraction(Compiler *compiler);
static void compileAddition(Compiler *compiler);
static void compileSubtraction(Compiler *compiler);
static void compileMultiplicationOrDivision(Compiler *compiler);
static void compileMultiplication(Compiler *compiler);
static void compileDivision(Compiler *compiler);
static void compileUnary(Compiler *compiler);
static void compileName(Compiler *compiler);
static void compileNegation(Compiler *compiler);

static Token match(Compiler *compiler, TokenType type);

static int resolveVariable(Compiler *compiler, Token *name);
static void enterBlock(Compiler *compiler);
static void exitBlock(Compiler *compiler);
static void declareVariable(Compiler *compiler, Token name, Type type);
static void markInitialized(Compiler *compiler);

static void pushType(Compiler *compiler, Type type);
static Type popType(Compiler *compiler); 
static void typeError(Type expected, Type found);

void initCompiler(Compiler *compiler, Lexer *lexer, Chunk *chunk) {
	compiler->lexer = lexer;
	compiler->chunk = chunk;

	memset(compiler->variables, 0, 256 * sizeof(Variable));
	compiler->variableCount = 0;
	compiler->depth = 0;

        compiler->typeStackSize = 0;
}

void compile(Compiler *compiler) {
	compileBlock(compiler);
	match(compiler, TT_END);
	writeIntoChunk(compiler->chunk, OP_EXIT);
}

static void compileStatement(Compiler *compiler) {
	Token token = peekNextToken(compiler->lexer);
	switch (token.type) {
		case TT_LCURLY:
			compileBlock(compiler);
			break;
		case TT_LET:
			compileLet(compiler);
			break;
		case TT_PRINT:
			compilePrint(compiler);
			break;
		default:
			fprintf(stderr, "Syntax Error: Unexpected token ");
			printToken(stderr, &token);
			fprintf(stderr, " while parsing statement\n");
                        exit(EXIT_FAILURE);
	}
}

static void compileBlock(Compiler *compiler) {
	match(compiler, TT_LCURLY);
	enterBlock(compiler);
	for (;;) {
		Token token = peekNextToken(compiler->lexer);
		if (token.type == TT_RCURLY || token.type == TT_END) {
			break;
		}
		compileStatement(compiler);
	}
	exitBlock(compiler);
	match(compiler, TT_RCURLY);
}

static void compileLet(Compiler *compiler) {
	match(compiler, TT_LET);
	Token name = match(compiler, TT_NAME);

	for (int i = compiler->variableCount - 1; i >= 0; --i) {
		Variable *variable = &compiler->variables[i];
		if (compiler->depth != -1 && variable->depth < compiler->depth) {
			break;
		}

		if (tokenEqual(&name, &variable->name)) {
			fprintf(stderr, "Variable already declared\n");
                        exit(EXIT_FAILURE);
		}
	}

        match(compiler, TT_COLON);
        Type type = compileType(compiler);

	declareVariable(compiler, name, type);

	match(compiler, TT_EQUAL);
	compileExpression(compiler);
	match(compiler, TT_SEMICOLON);

        Type expressionType = popType(compiler);
        if (expressionType != type) {
            typeError(type, expressionType);
        }

	markInitialized(compiler);
}

static Type compileType(Compiler *compiler) {
        Token token = getNextToken(compiler->lexer);
        switch (token.type) {
            case TT_INTEGER:
                return TY_INTEGER;
            case TT_BOOLEAN:
                return TY_BOOLEAN;
            default:
                fprintf(stderr, "Unknown type\n");
                exit(EXIT_FAILURE);
        }           
}

static void compilePrint(Compiler *compiler) {
	match(compiler, TT_PRINT);
	match(compiler, TT_LPAREN);
	compileExpression(compiler);
	match(compiler, TT_RPAREN);
	match(compiler, TT_SEMICOLON);

        Type type = popType(compiler);
        switch (type) {
            case TY_INTEGER:
                writeIntoChunk(compiler->chunk, OP_PRINT_INTEGER);
                break;
            case TY_BOOLEAN:
                writeIntoChunk(compiler->chunk, OP_PRINT_BOOLEAN);
                break;
        }
}

static void compileExpression(Compiler *compiler) {
	compileComparison(compiler);
}

static void compileComparison(Compiler *compiler) {
        compileAdditionOrSubtraction(compiler);
        OpCode comparison;
        Token token = peekNextToken(compiler->lexer);
        switch (token.type) {
                case TT_DOUBLE_EQUAL:
                        comparison = OP_EQUAL;
                        break;
                case TT_NOT_EQUAL:
                        comparison = OP_NOT_EQUAL;
                        break;
                case TT_LESS:
                        comparison = OP_LESS;
                        break;
                case TT_LESS_EQUAL:
                        comparison = OP_LESS_EQUAL;
                        break;
                case TT_GREATER:
                        comparison = OP_GREATER;
                        break;
                case TT_GREATER_EQUAL:
                        comparison = OP_GREATER_EQUAL;
                        break;
                default:
                        return;
        }
        getNextToken(compiler->lexer);
        compileAdditionOrSubtraction(compiler);

        Type firstType = popType(compiler);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(compiler);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        pushType(compiler, TY_BOOLEAN);

        writeIntoChunk(compiler->chunk, comparison);
}

static void compileAdditionOrSubtraction(Compiler *compiler) {
	compileMultiplicationOrDivision(compiler);
	for (;;) {
		Token token = peekNextToken(compiler->lexer);
		switch (token.type) {
			case TT_PLUS:
                                compileAddition(compiler);
                                break;
			case TT_MINUS:
                                compileSubtraction(compiler);
				break;
			default:
				return;
		}
	}
}

static void compileAddition(Compiler *compiler) {
        getNextToken(compiler->lexer);
        compileMultiplicationOrDivision(compiler);

        Type firstType = popType(compiler);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(compiler);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        pushType(compiler, TY_INTEGER);

        writeIntoChunk(compiler->chunk, OP_ADD);
}

static void compileSubtraction(Compiler *compiler) {
        getNextToken(compiler->lexer);
        compileMultiplicationOrDivision(compiler);

        Type firstType = popType(compiler);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(compiler);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, secondType);
        }
        pushType(compiler, TY_INTEGER);

        writeIntoChunk(compiler->chunk, OP_SUB);
}

static void compileMultiplicationOrDivision(Compiler *compiler) {
	compileUnary(compiler);
	for (;;) {
		Token token = peekNextToken(compiler->lexer);
		switch (token.type) {
			case TT_STAR:
				compileMultiplication(compiler);
				break;
			case TT_SLASH:
				compileDivision(compiler);
				break;
			default:
				return;
		}
	}
}

static void compileMultiplication(Compiler *compiler) {
        getNextToken(compiler->lexer);
        compileUnary(compiler);

        Type firstType = popType(compiler);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(compiler);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, secondType);
        }
        pushType(compiler, TY_INTEGER);

        writeIntoChunk(compiler->chunk, OP_MUL);
}

static void compileDivision(Compiler *compiler) {
        getNextToken(compiler->lexer);
        compileUnary(compiler);

        Type firstType = popType(compiler);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(compiler);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, secondType);
        }
        pushType(compiler, TY_INTEGER);

        writeIntoChunk(compiler->chunk, OP_DIV);
}

static void compileUnary(Compiler *compiler) {
	Chunk *chunk = compiler->chunk;
	Token token = peekNextToken(compiler->lexer);
	switch (token.type) {
		case TT_NUMBER: {
			getNextToken(compiler->lexer);
			writeIntoChunk(chunk, OP_PUSH_INTEGER);
			int n = atoi(token.start);
			writeIntoChunk(chunk, n);
			pushType(compiler, TY_INTEGER);
			break;
		}
		case TT_TRUE: {
			getNextToken(compiler->lexer);
			writeIntoChunk(chunk, OP_PUSH_TRUE);
			pushType(compiler, TY_BOOLEAN);
			break;
		}
		case TT_FALSE: {
			getNextToken(compiler->lexer);
			writeIntoChunk(chunk, OP_PUSH_FALSE);
			pushType(compiler, TY_BOOLEAN);
			break;
		}
		case TT_LPAREN: {
			getNextToken(compiler->lexer);
			compileExpression(compiler);
			match(compiler, TT_RPAREN);
			break;
		}
		case TT_MINUS: {
			compileNegation(compiler);
			break;
		}
		case TT_NAME: {
                        compileName(compiler);
			break;
		}
		default: {
			fprintf(stderr, "Syntax Error: Unexpected token ");
			printToken(stderr, &token);
			fprintf(stderr, " while parsing expression\n");
                        exit(EXIT_FAILURE);
		}
	}
}

static void compileName(Compiler *compiler) {
        Token token = getNextToken(compiler->lexer);
        writeIntoChunk(compiler->chunk, OP_LOAD);

        int i = resolveVariable(compiler, &token);
        writeIntoChunk(compiler->chunk, i);
}
        

static void compileNegation(Compiler *compiler) {
	getNextToken(compiler->lexer);
	compileUnary(compiler);

        Type type = popType(compiler);
        if (type != TY_INTEGER) {
		typeError(TY_INTEGER, type);
        }
        pushType(compiler, TY_INTEGER);

	writeIntoChunk(compiler->chunk, OP_NEGATE);
}

static Token match(Compiler *compiler, TokenType type) {
	Token token = peekNextToken(compiler->lexer);
	if (token.type != type) {
		fprintf(stderr, "Syntax Error: Expected ");
		printTokenType(stderr, &type);
		fprintf(stderr, " but found ");
		printToken(stderr, &token);
		fprintf(stderr, "\n");
                exit(EXIT_FAILURE);
	}
	return getNextToken(compiler->lexer);
}

static int resolveVariable(Compiler *compiler, Token *name) {
	for (int i = compiler->variableCount - 1; i >= 0; --i) {
		Variable *variable = &compiler->variables[i];
		if (tokenEqual(&variable->name, name)) {
			if (variable->depth != -1) {
                            pushType(compiler, variable->type);
                            return i;
			}
		}
	}

	fprintf(stderr, "Undeclared variable!\n");
        exit(EXIT_FAILURE);
}

static void enterBlock(Compiler *compiler) {
	compiler->depth++;
}

static void exitBlock(Compiler *compiler) {
	compiler->depth--;

	while (compiler->variableCount > 0 &&
			compiler->variables[compiler->variableCount - 1].depth > compiler->depth) {
		writeIntoChunk(compiler->chunk, OP_POP);
		compiler->variableCount--;
	}
}

static void declareVariable(Compiler *compiler, Token name, Type type) {
	Variable* variable = &compiler->variables[compiler->variableCount++];
	variable->name = name;
        variable->type = type;
	variable->depth = -1;
}

static void markInitialized(Compiler *compiler) {
	compiler->variables[compiler->variableCount - 1].depth = compiler->depth;
}

static void pushType(Compiler *compiler, Type type) {
        compiler->typeStack[compiler->typeStackSize++] = type;
}

static Type popType(Compiler *compiler) {
        return compiler->typeStack[--compiler->typeStackSize];
}

static void typeError(Type expected, Type found) {
	fprintf(stderr, "TypeError: Expected ");
	printType(stderr, expected);
	fprintf(stderr, " but found ");
	printType(stderr, found);
	fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
}
