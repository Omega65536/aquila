#ifndef CHUNK_H
#define CHUNK_H

#include <inttypes.h>

typedef enum OpCode {
	OP_NOOP,
	OP_EXIT,
	OP_POP,

	OP_PUSH_INTEGER,
	OP_PUSH_TRUE,
	OP_PUSH_FALSE,
	OP_LOAD,

	OP_PRINT_INTEGER,
	OP_PRINT_BOOLEAN,

	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_NEGATE,

        OP_EQUAL,
        OP_NOT_EQUAL,
        OP_LESS,
        OP_LESS_EQUAL,
        OP_GREATER,
        OP_GREATER_EQUAL,
} OpCode;

typedef struct Chunk {
	uint32_t *code;
	int length;
	int capacity;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeIntoChunk(Chunk *chunk, uint32_t word);
void printChunk(Chunk *chunk);

#endif
