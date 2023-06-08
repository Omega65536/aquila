#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

static void match(Parser *parser, TokenType type);

void initParser(Parser *parser, Lexer *lexer, Chunk *chunk) {
	parser->lexer = lexer;
	parser->chunk = chunk;
	parser->hasError = false;
}

void parse(Parser *parser) {
	parseBlock(parser);
	match(parser, TT_END);
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
	for (;;) {
		Token token = peekNextToken(parser->lexer);
		if (token.type == TT_RCURLY || token.type == TT_END) {
			break;
		}
		parseStatement(parser);
	}
	match(parser, TT_RCURLY);
}

static void parseLet(Parser *parser) {
	match(parser, TT_LET);
	match(parser, TT_NAME);
	match(parser, TT_EQUAL);
	parseExpression(parser);
	match(parser, TT_SEMICOLON);

	writeIntoChunk(parser->chunk, OP_STORE);
	writeIntoChunk(parser->chunk, 1);
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
		writeIntoChunk(parser->chunk, 1);
	} else {
		fprintf(stderr, "Syntax Error: Unexpected token ");
		printToken(stderr, &token);
		fprintf(stderr, "\n");
		parser->hasError = true;
	}
}

static void match(Parser *parser, TokenType type) {
	Token token = getNextToken(parser->lexer);
	if (token.type != type) {
		fprintf(stderr, "Syntax Error: Expected ");
		printTokenType(stderr, &type);
		fprintf(stderr, " but found ");
		printToken(stderr, &token);
		fprintf(stderr, "\n");
		parser->hasError = true;
	}
}
