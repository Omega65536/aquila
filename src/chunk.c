#include "chunk.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void init_chunk(Chunk *chunk) {
	chunk->code = malloc(4 * sizeof(uint32_t));
	chunk->length = 0;
	chunk->capacity = 4;
}

void free_chunk(Chunk *chunk) {
	free(chunk->code);
}

void write_into_chunk(Chunk *chunk, uint32_t word) {
	if (chunk->length == chunk->capacity) {
		chunk->capacity *= 2;
		chunk->code =
		    realloc(chunk->code, chunk->capacity * sizeof(uint32_t));
	}
	chunk->code[chunk->length] = word;
	chunk->length++;
}

void print_chunk(Chunk *chunk) {
        int index = 0;
        while (index < chunk->length) { 
		index = print_op_code(chunk, index);
	}
}

int print_op_code(Chunk *chunk, int index) {
	printf("%-8d", index);
	switch (chunk->code[index]) {
		case OP_NOOP:
			printf("NOOP\n");
			return index + 1;
		case OP_EXIT:
			printf("EXIT\n");
			return index + 1;
		case OP_POP:
			printf("POP\n");
			return index + 1;
		case OP_PUSH_INTEGER:
			printf("PUSH_INTEGER %d\n", chunk->code[index + 1]);
			return index + 2;
		case OP_PUSH_TRUE:
			printf("PUSH_TRUE\n");
			return index + 1;
		case OP_PUSH_FALSE:
			printf("PUSH_FALSE\n");
			return index + 1;
		case OP_LOAD:
			printf("LOAD %d\n", chunk->code[index + 1]);
			return index + 2;
		case OP_STORE:
			printf("STORE %d\n", chunk->code[index + 1]);
			return index + 2;
		case OP_PRINT_UNIT:
			printf("PRINT_UNIT\n");
			return index + 1;
		case OP_PRINT_INTEGER:
			printf("PRINT_INTEGER\n");
			return index + 1;
		case OP_PRINT_BOOLEAN:
			printf("PRINT_BOOLEAN\n");
			return index + 1;
		case OP_ADD:
			printf("ADD\n");
			return index + 1;
		case OP_SUB:
			printf("SUB\n");
			return index + 1;
		case OP_MUL:
			printf("MUL\n");
			return index + 1;
		case OP_DIV:
			printf("DIV\n");
			return index + 1;
		case OP_NEGATE:
			printf("NEGATE\n");
			return index + 1;
		case OP_EQUAL:
			printf("EQUAL\n");
			return index + 1;
		case OP_NOT_EQUAL:
			printf("NOT_EQUAL\n");
			return index + 1;
		case OP_LESS:
			printf("LESS\n");
			return index + 1;
		case OP_LESS_EQUAL:
			printf("LESS_EQUAL\n");
			return index + 1;
		case OP_GREATER:
			printf("GREATER\n");
			return index + 1;
		case OP_GREATER_EQUAL:
			printf("GREATER_EQUAL\n");
			return index + 1;
		case OP_JUMP:
			printf("JUMP %d\n", chunk->code[index + 1]);
			return index + 2;
		case OP_JUMP_IF_FALSE:
			printf("JUMP_IF_FALSE %d\n", chunk->code[index + 1]);
			return index + 2;
		case OP_CALL:
			printf("CALL %d %d\n", chunk->code[index + 1],
			       chunk->code[index + 2]);
			return index + 3;
		case OP_RETURN:
			printf("RETURN %d\n", chunk->code[index + 1]);
			return index + 2;
		default:
			printf("UNKNOWN OP: %d", chunk->code[index]);
                        return index + 1;
			break;
	}
}
