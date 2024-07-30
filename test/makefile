
PROJECT = midi2cv-test

CC = gcc
CFLAGS = -Wall -Og --std=c11 -g -DDEBUG

INCLUDE = -I../src\

SRC = $(wildcard *.c) ../src/ringbuffer.c ../src/notemem.c ../src/turing.c

unittest:
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) -o bin/$(PROJECT)
	./bin/$(PROJECT)

clean:
	@rm -fr bin
