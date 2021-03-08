CC=cc
CFLAGS=-std=gnu99 -Wall -Wextra -O3

programs=$(patsubst src/%.c,bin/%,$(wildcard src/*.c))

all: progs

progs: bin $(programs)
	ln -sf test bin/[

bin:
	mkdir -p bin

bin/%: src/%.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf bin

todo:
	@grep -n TODO $(wildcard src/*.c)

help:
	@cat README.MD

.PHONY: all progs clean todo help

