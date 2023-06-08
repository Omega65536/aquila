#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "chunk.h"
#include "token.h"

static void parseStatement(Parser *parser);
static void parseBlock(Parser *parser);
static void parseLet(Parser *parser);
static void parsePrint(Parser *parser);

static void parseExpression(Parser *parser);
static void parseAddition(Parser *parser);
static void parseMultiplication(Parser *parser);
static void parseUnary(Parser *parser);

static Token match(Parser *parser, TokenType type);

static int resolveVariable(Parser *parser, Token *name);
static void enterBlock(Parser *parser);
static void exitBlock(Parser *parser);
static void declareVariable(Parser *parser, Token token);

void initParser(Parser *parser, Lexer *lexer, Chunk *chunk) {
	parser->lexer = lexer;
	parser->chunk = chunk;

	memset(parser->variables, 0, 256 * sizeof(Variable));
	parser->variableCount = 0;
	parser->depth = 0;

	parser->hasError = false;
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
			fprintf(stderr, "\n");
			parser->hasError = true;
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
			parser->hasError = true;
		}
	}
	declareVariable(parser, name);

	match(parser, TT_EQUAL);
	parseExpression(parser);
	match(parser, TT_SEMICOLON);
}

static void parsePrint(Parser *parser) {
	match(parser, TT_PRINT);
	match(parser, TT_LPAREN);
	parseExpression(parser);
	match(parser, TT_RPAREN);
	match(parser, TT_SEMICOLON);

	writeIntoChunk(parser->chunk, OP_PRINT);
}

static void parseExpression(Parser *parser) {
	parseAddition(parser);
}

static void parseAddition(Parser *parser) {
	parseMultiplication(parser);
	for (;;) {
		Token token = peekNextToken(parser->lexer);
		switch (token.type) {
			case TT_PLUS:
				getNextToken(parser->lexer);
				parseMultiplication(parser);
				writeIntoChunk(parser->chunk, OP_ADD);
				break;
			case TT_MINUS:
				getNextToken(parser->lexer);
				parseMultiplication(parser);
				writeIntoChunk(parser->chunk, OP_SUB);
				break;
			default:
				return;
		}
	}
}

static void parseMultiplication(Parser *parser) {
	parseUnary(parser);
	for (;;) {
		Token token = peekNextToken(parser->lexer);
		switch (token.type) {
			case TT_STAR:
				getNextToken(parser->lexer);
				parseUnary(parser);
				writeIntoChunk(parser->chunk, OP_MUL);
				break;
			case TT_SLASH:
				getNextToken(parser->lexer);
				parseUnary(parser);
				writeIntoChunk(parser->chunk, OP_DIV);
				break;
			default:
				return;
		}
	}
}

static void parseUnary(Parser *parser) {
	Chunk *chunk = parser->chunk;
	Token token = getNextToken(parser->lexer);
	if (token.type == TT_NUMBER) {
		writeIntoChunk(chunk, OP_CONST);
		int n = atoi(token.start);
		writeIntoChunk(chunk, n);
	} else if (token.type == TT_LPAREN) {
		parseAddition(parser);
		match(parser, TT_RPAREN);
	} else if (token.type == TT_MINUS) {
		parseUnary(parser);
		writeIntoChunk(chunk, OP_NEGATE);
	} else if (token.type == TT_NAME) {
		writeIntoChunk(parser->chunk, OP_LOAD);
		int i = resolveVariable(parser, &token);
		writeIntoChunk(parser->chunk, i);
	} else {
		fprintf(stderr, "Syntax Error: Unexpected token ");
		printToken(stderr, &token);
		fprintf(stderr, "\n");
		parser->hasError = true;
	}
}

static Token match(Parser *parser, TokenType type) {
	Token token = getNextToken(parser->lexer);
	if (token.type != type) {
		fprintf(stderr, "Syntax Error: Expected ");
		printTokenType(stderr, &type);
		fprintf(stderr, " but found ");
		printToken(stderr, &token);
		fprintf(stderr, "\n");
		parser->hasError = true;
	}
	return token;
}

static int resolveVariable(Parser *parser, Token *name) {
	for (int i = parser->variableCount - 1; i >= 0; --i) {
		Variable *variable = &parser->variables[i];
		if (tokenEqual(&variable->name, name)) {
			return i;
		}
	}
	fprintf(stderr, "Undeclared variable!\n");
	parser->hasError = true;
	return -1;
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

static void declareVariable(Parser *parser, Token name) {
	Variable* variable = &parser->variables[parser->variableCount++];
	variable->name = name;
	variable->depth = parser->depth;
}
