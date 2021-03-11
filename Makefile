CC=cc
CFLAGS=-std=gnu99 -Wall -Wextra -O3

programs=bin/[ $(patsubst src/%.c,bin/%,$(wildcard src/*.c))

DESTDIR ?= /usr/local
BINDIR ?= $(DESTDIR)/bin
MANDIR ?= $(DESTDIR)/share/man/man1

programs_BINDIR=$(patsubst bin/%,$(BINDIR)/%,$(programs))
doc_MANDIR=$(patsubst doc/%.1,$(MANDIR)/%.1,$(wildcard doc/*.1))

all: progs

progs: bin $(programs)

bin:
	mkdir -p bin

bin/[: bin/test
	ln -sf test bin/[

bin/%: src/%.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf bin

todo:
	@grep -n TODO $(wildcard src/*.c)

help:
	@cat README.MD
	@echo
	@echo "TODOs:"
	@grep -n TODO $(wildcard src/*.c)

$(BINDIR):
	install -dm755 $@
$(BINDIR)/%: bin/% $(BINDIR)
	install -m755 $< $@

$(MANDIR):
	install -dm755 $@
$(MANDIR)/%.1: doc/%.1 $(MANDIR)
	install -m644 $< $@

install: $(programs_BINDIR) $(doc_MANDIR)
	

.PHONY: all progs clean todo help

