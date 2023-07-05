#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "chunk.h"
#include "token.h"
#include "type.h"

static void parseStatement(Parser *parser);
static void parseBlock(Parser *parser);
static void parseLet(Parser *parser);
static Type parseType(Parser *parser);
static void parsePrint(Parser *parser);

static void parseExpression(Parser *parser);
static void parseComparison(Parser *parser);
static void parseAdditionOrSubtraction(Parser *parser);
static void parseAddition(Parser *parser);
static void parseSubtraction(Parser *parser);
static void parseMultiplicationOrDivision(Parser *parser);
static void parseMultiplication(Parser *parser);
static void parseDivision(Parser *parser);
static void parseUnary(Parser *parser);
static void parseName(Parser *parser);
static void parseNegation(Parser *parser);

static Token match(Parser *parser, TokenType type);

static int resolveVariable(Parser *parser, Token *name);
static void enterBlock(Parser *parser);
static void exitBlock(Parser *parser);
static void declareVariable(Parser *parser, Token name, Type type);
static void markInitialized(Parser *parser);

static void pushType(Parser *parser, Type type);
static Type popType(Parser *parser); 
static void typeError(Type expected, Type found);

void initParser(Parser *parser, Lexer *lexer, Chunk *chunk) {
	parser->lexer = lexer;
	parser->chunk = chunk;

	memset(parser->variables, 0, 256 * sizeof(Variable));
	parser->variableCount = 0;
	parser->depth = 0;

        parser->typeStackSize = 0;
}

void parse(Parser *parser) {
	parseBlock(parser);
	match(parser, TT_END);
	writeIntoChunk(parser->chunk, OP_EXIT);
}

static void parseStatement(Parser *parser) {
	Token token = peekNextToken(parser->lexer);
	switch (token.type) {
		case TT_LCURLY:
			parseBlock(parser);
			break;
		case TT_LET:
			parseLet(parser);
			break;
		case TT_PRINT:
			parsePrint(parser);
			break;
		default:
			fprintf(stderr, "Syntax Error: Unexpected token ");
			printToken(stderr, &token);
			fprintf(stderr, " while parsing statement\n");
                        exit(EXIT_FAILURE);
	}
}

static void parseBlock(Parser *parser) {
	match(parser, TT_LCURLY);
	enterBlock(parser);
	for (;;) {
		Token token = peekNextToken(parser->lexer);
		if (token.type == TT_RCURLY || token.type == TT_END) {
			break;
		}
		parseStatement(parser);
	}
	exitBlock(parser);
	match(parser, TT_RCURLY);
}

static void parseLet(Parser *parser) {
	match(parser, TT_LET);
	Token name = match(parser, TT_NAME);

	for (int i = parser->variableCount - 1; i >= 0; --i) {
		Variable *variable = &parser->variables[i];
		if (parser->depth != -1 && variable->depth < parser->depth) {
			break;
		}

		if (tokenEqual(&name, &variable->name)) {
			fprintf(stderr, "Variable already declared\n");
                        exit(EXIT_FAILURE);
		}
	}

        match(parser, TT_COLON);
        Type type = parseType(parser);

	declareVariable(parser, name, type);

	match(parser, TT_EQUAL);
	parseExpression(parser);
	match(parser, TT_SEMICOLON);

        Type expressionType = popType(parser);
        if (expressionType != type) {
            typeError(type, expressionType);
        }

	markInitialized(parser);
}

static Type parseType(Parser *parser) {
        Token token = getNextToken(parser->lexer);
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

static void parsePrint(Parser *parser) {
	match(parser, TT_PRINT);
	match(parser, TT_LPAREN);
	parseExpression(parser);
	match(parser, TT_RPAREN);
	match(parser, TT_SEMICOLON);

        Type type = popType(parser);
        switch (type) {
            case TY_INTEGER:
                writeIntoChunk(parser->chunk, OP_PRINT_INTEGER);
                break;
            case TY_BOOLEAN:
                writeIntoChunk(parser->chunk, OP_PRINT_BOOLEAN);
                break;
        }
}

static void parseExpression(Parser *parser) {
	parseComparison(parser);
}

static void parseComparison(Parser *parser) {
        parseAdditionOrSubtraction(parser);
        OpCode comparison;
        Token token = peekNextToken(parser->lexer);
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
        getNextToken(parser->lexer);
        parseAdditionOrSubtraction(parser);

        Type firstType = popType(parser);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(parser);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        pushType(parser, TY_BOOLEAN);

        writeIntoChunk(parser->chunk, comparison);
}

static void parseAdditionOrSubtraction(Parser *parser) {
	parseMultiplicationOrDivision(parser);
	for (;;) {
		Token token = peekNextToken(parser->lexer);
		switch (token.type) {
			case TT_PLUS:
                                parseAddition(parser);
                                break;
			case TT_MINUS:
                                parseSubtraction(parser);
				break;
			default:
				return;
		}
	}
}

static void parseAddition(Parser *parser) {
        getNextToken(parser->lexer);
        parseMultiplicationOrDivision(parser);

        Type firstType = popType(parser);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(parser);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        pushType(parser, TY_INTEGER);

        writeIntoChunk(parser->chunk, OP_ADD);
}

static void parseSubtraction(Parser *parser) {
        getNextToken(parser->lexer);
        parseMultiplicationOrDivision(parser);

        Type firstType = popType(parser);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(parser);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, secondType);
        }
        pushType(parser, TY_INTEGER);

        writeIntoChunk(parser->chunk, OP_SUB);
}

static void parseMultiplicationOrDivision(Parser *parser) {
	parseUnary(parser);
	for (;;) {
		Token token = peekNextToken(parser->lexer);
		switch (token.type) {
			case TT_STAR:
				parseMultiplication(parser);
				break;
			case TT_SLASH:
				parseDivision(parser);
				break;
			default:
				return;
		}
	}
}

static void parseMultiplication(Parser *parser) {
        getNextToken(parser->lexer);
        parseUnary(parser);

        Type firstType = popType(parser);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(parser);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, secondType);
        }
        pushType(parser, TY_INTEGER);

        writeIntoChunk(parser->chunk, OP_MUL);
}

static void parseDivision(Parser *parser) {
        getNextToken(parser->lexer);
        parseUnary(parser);

        Type firstType = popType(parser);
        if (firstType != TY_INTEGER) {
		typeError(TY_INTEGER, firstType);
        }
        Type secondType = popType(parser);
        if (secondType != TY_INTEGER) {
		typeError(TY_INTEGER, secondType);
        }
        pushType(parser, TY_INTEGER);

        writeIntoChunk(parser->chunk, OP_DIV);
}

static void parseUnary(Parser *parser) {
	Chunk *chunk = parser->chunk;
	Token token = peekNextToken(parser->lexer);
	switch (token.type) {
		case TT_NUMBER: {
			getNextToken(parser->lexer);
			writeIntoChunk(chunk, OP_PUSH_INTEGER);
			int n = atoi(token.start);
			writeIntoChunk(chunk, n);
			pushType(parser, TY_INTEGER);
			break;
		}
		case TT_TRUE: {
			getNextToken(parser->lexer);
			writeIntoChunk(chunk, OP_PUSH_TRUE);
			pushType(parser, TY_BOOLEAN);
			break;
		}
		case TT_FALSE: {
			getNextToken(parser->lexer);
			writeIntoChunk(chunk, OP_PUSH_FALSE);
			pushType(parser, TY_BOOLEAN);
			break;
		}
		case TT_LPAREN: {
			getNextToken(parser->lexer);
			parseExpression(parser);
			match(parser, TT_RPAREN);
			break;
		}
		case TT_MINUS: {
			parseNegation(parser);
			break;
		}
		case TT_NAME: {
                        parseName(parser);
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

static void parseName(Parser *parser) {
        Token token = getNextToken(parser->lexer);
        writeIntoChunk(parser->chunk, OP_LOAD);

        int i = resolveVariable(parser, &token);
        writeIntoChunk(parser->chunk, i);
}
        

static void parseNegation(Parser *parser) {
	getNextToken(parser->lexer);
	parseUnary(parser);

        Type type = popType(parser);
        if (type != TY_INTEGER) {
		typeError(TY_INTEGER, type);
        }
        pushType(parser, TY_INTEGER);

	writeIntoChunk(parser->chunk, OP_NEGATE);
}

static Token match(Parser *parser, TokenType type) {
	Token token = peekNextToken(parser->lexer);
	if (token.type != type) {
		fprintf(stderr, "Syntax Error: Expected ");
		printTokenType(stderr, &type);
		fprintf(stderr, " but found ");
		printToken(stderr, &token);
		fprintf(stderr, "\n");
                exit(EXIT_FAILURE);
	}
	return getNextToken(parser->lexer);
}

static int resolveVariable(Parser *parser, Token *name) {
	for (int i = parser->variableCount - 1; i >= 0; --i) {
		Variable *variable = &parser->variables[i];
		if (tokenEqual(&variable->name, name)) {
			if (variable->depth != -1) {
                            pushType(parser, variable->type);
                            return i;
			}
		}
	}

	fprintf(stderr, "Undeclared variable!\n");
        exit(EXIT_FAILURE);
}

static void enterBlock(Parser *parser) {
	parser->depth++;
}

static void exitBlock(Parser *parser) {
	parser->depth--;

	while (parser->variableCount > 0 &&
			parser->variables[parser->variableCount - 1].depth > parser->depth) {
		writeIntoChunk(parser->chunk, OP_POP);
		parser->variableCount--;
	}
}

static void declareVariable(Parser *parser, Token name, Type type) {
	Variable* variable = &parser->variables[parser->variableCount++];
	variable->name = name;
        variable->type = type;
	variable->depth = -1;
}

static void markInitialized(Parser *parser) {
	parser->variables[parser->variableCount - 1].depth = parser->depth;
}

static void pushType(Parser *parser, Type type) {
        parser->typeStack[parser->typeStackSize++] = type;
}

static Type popType(Parser *parser) {
        return parser->typeStack[--parser->typeStackSize];
}

static void typeError(Type expected, Type found) {
	fprintf(stderr, "TypeError: Expected ");
	printType(stderr, expected);
	fprintf(stderr, " but found ");
	printType(stderr, found);
	fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
}
