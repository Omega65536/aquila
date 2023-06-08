#ifndef CHUNK_H
#define CHUNK_H

#include <inttypes.h>

typedef enum OpCode {
	OP_NOOP,
	OP_EXIT,
	OP_POP,

	OP_CONST,
	OP_LOAD,

	OP_PRINT,

	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_NEGATE,
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
