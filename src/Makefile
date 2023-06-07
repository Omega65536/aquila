CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700 -g

.PHONY: all clean

all: aquila

clean:
	rm -f aquila *.o

aquila: aquila.o lexer.o token.o
	$(CC) $(CFLAGS) -o aquila aquila.o lexer.o token.o

aquila.o: aquila.c
	$(CC) $(CFLAGS) -c aquila.c 

lexer.o: lexer.h token.o
	$(CC) $(CFLAGS) -c lexer.c

token.o: token.h
	$(CC) $(CFLAGS) -c token.c
