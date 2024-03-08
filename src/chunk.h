#ifndef CHUNK_H
#define CHUNK_H

#include <inttypes.h>

typedef enum OpCode {
	OP_NOOP,
	OP_EXIT,
	OP_POP,

	OP_PUSH,
	OP_LOAD,
	OP_STORE,

	OP_PRINT_UNIT,
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

extern const int AQ_UNIT;
extern const int AQ_TRUE;
extern const int AQ_FALSE;

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
void write_into_chunk(Chunk *chunk, uint32_t word);
int reserve_place_in_chunk(Chunk *chunk);
void print_chunk(Chunk *chunk);
int print_op_code(Chunk *chunk, int index);

#endif
