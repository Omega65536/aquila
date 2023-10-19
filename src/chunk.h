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
	OP_STORE,

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

	OP_JUMP,
	OP_JUMP_IF_FALSE,

	OP_CALL,
	OP_RETURN,
} OpCode;

typedef struct Chunk {
	uint32_t *code;
	int length;
	int capacity;
} Chunk;

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
void write_into_chunk(Chunk *chunk, uint32_t word);
void print_chunk(Chunk *chunk);

#endif
