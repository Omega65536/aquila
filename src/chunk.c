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
	for (int i = 0; i < chunk->length; ++i) {
		printf("%-8d", i);
		switch (chunk->code[i]) {
			case OP_NOOP:
				printf("NOOP");
				break;
			case OP_EXIT:
				printf("EXIT");
				break;
			case OP_POP:
				printf("POP");
				break;
			case OP_PUSH_INTEGER:
				printf("PUSH_INTEGER %d", chunk->code[++i]);
				break;
			case OP_PUSH_TRUE:
				printf("PUSH_TRUE");
				break;
			case OP_PUSH_FALSE:
				printf("PUSH_FALSE");
				break;
			case OP_LOAD:
				printf("LOAD %d", chunk->code[++i]);
				break;
			case OP_STORE:
				printf("STORE %d", chunk->code[++i]);
				break;
			case OP_PRINT_INTEGER:
				printf("PRINT_INTEGER");
				break;
			case OP_PRINT_BOOLEAN:
				printf("PRINT_BOOLEAN");
				break;
			case OP_ADD:
				printf("ADD");
				break;
			case OP_SUB:
				printf("SUB");
				break;
			case OP_MUL:
				printf("MUL");
				break;
			case OP_DIV:
				printf("DIV");
				break;
			case OP_NEGATE:
				printf("NEGATE");
				break;
			case OP_EQUAL:
				printf("EQUAL");
				break;
			case OP_NOT_EQUAL:
				printf("NOT_EQUAL");
				break;
			case OP_LESS:
				printf("LESS");
				break;
			case OP_LESS_EQUAL:
				printf("LESS_EQUAL");
				break;
			case OP_GREATER:
				printf("GREATER");
				break;
			case OP_GREATER_EQUAL:
				printf("GREATER_EQUAL");
				break;
			case OP_JUMP:
				printf("JUMP %d", chunk->code[++i]);
				break;
			case OP_JUMP_IF_FALSE:
				printf("JUMP_IF_FALSE %d", chunk->code[++i]);
				break;
			case OP_CALL:
				printf("CALL %d", chunk->code[++i]);
				break;
			case OP_RETURN:
				printf("RETURN");
				break;
			default:
				printf("UNKNOWN OP: %d", chunk->code[i]);
				break;
		}
		printf("\n");
	}
}
