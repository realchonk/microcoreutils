CC=cc
CFLAGS=-std=gnu99 -Wall -Wextra -O3

programs=$(patsubst src/%.c,bin/%,$(wildcard src/*.c))

all: progs

progs: bin $(programs)

bin:
	mkdir -p bin

bin/%: src/%.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf bin

.PHONY: all progs clean

