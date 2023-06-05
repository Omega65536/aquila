CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Werror -D_XOPEN_SOURCE=700

.PHONY: all clean

all: aquila

clean:
	rm -f aquila *.o

aquila: aquila.o
	$(CC) $(CFLAGS) -o aquila aquila.o

