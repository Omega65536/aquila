#include "chunk.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void initChunk(Chunk *chunk) {
	chunk->code = malloc(4 * sizeof(uint32_t));
	chunk->length = 0;
	chunk->capacity = 4;
}

void freeChunk(Chunk *chunk) {
	free(chunk->code);
}

void writeIntoChunk(Chunk *chunk, uint32_t word) {
	if (chunk->length == chunk->capacity) {
		chunk->capacity *= 2;
		chunk->code =
		    realloc(chunk->code, chunk->capacity * sizeof(uint32_t));
	}
	chunk->code[chunk->length] = word;
	chunk->length++;
}

void printChunk(Chunk *chunk) {
	for (int i = 0; i < chunk->length; ++i) {
		printf("%-8d", i);
		switch (chunk->code[i]) {
			case OP_NOOP:
				printf("NOOP");
				break;
			case OP_RETURN:
				printf("RETURN");
				break;
			case OP_CONST:
				printf("CONST %d", chunk->code[++i]);
				break;
			case OP_STORE:
				printf("STORE %d", chunk->code[++i]);
				break;
			case OP_LOAD:
				printf("LOAD %d", chunk->code[++i]);
				break;
			case OP_PRINT:
				printf("PRINT");
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
			default:
				printf("UNKNOWN OP: %d", chunk->code[i]);
				break;
		}
		printf("\n");
	}
}
