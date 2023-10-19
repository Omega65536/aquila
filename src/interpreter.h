#ifndef INTERPETER_H
#define INTERPRETE_H

#include "chunk.h"
#include <stdbool.h>

typedef struct Frame {
	int return_address;
	int offset;
} Frame;

typedef union Object {
	int integer;
	bool boolean;
} Object;

typedef struct Interpreter {
	Chunk *chunk;
	int index;
	int offset;
	Object *stack;
	int stack_length;
	Frame *frames;
	int frame_count;
} Interpreter;

void init_interpreter(Interpreter *interpreter, Chunk *chunk);
void free_interpreter(Interpreter *interpreter);

int interpret(Interpreter *interpreter);

#endif
