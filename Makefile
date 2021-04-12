CC=$(CROSS_COMPILE)gcc
CFLAGS += -std=gnu99 -Wall -Wextra -O3

programs=bin/[ $(patsubst src/%.c,bin/%,$(wildcard src/*.c)) $(patsubst src/%.sh,bin/%,$(wildcard src/*.sh))

DESTDIR ?= /usr/local
BINDIR ?= bin
DATADIR ?= share
MANDIR ?= $(DATADIR)/man/man1

programs_BINDIR=$(patsubst bin/%,$(DESTDIR)/$(BINDIR)/%,$(programs))
doc_MANDIR=$(patsubst doc/%.1,$(DESTDIR)/$(MANDIR)/%.1,$(wildcard doc/*.1))

all: progs

progs: bin $(programs)

bin:
	mkdir -p bin

bin/[: bin/test
	ln -sf test bin/[

bin/login: src/login.c
	$(CC) -o $@ $< $(CFLAGS) -lcrypt

bin/%: src/%.c
	$(CC) -o $@ $< $(CFLAGS)

bin/%: src/%.sh
	install -m755 $< $@

clean:
	rm -rf bin

todo:
	@grep -n TODO $(wildcard src/*.c)

help:
	@cat README.MD
	@echo
	@echo "TODOs:"
	@grep -n TODO $(wildcard src/*.c)

$(DESTDIR)/$(BINDIR):
	install -dm755 $@
$(DESTDIR)/$(BINDIR)/%: bin/% $(BINDIR)
	install -m755 $< $@

$(DESTDIR)/$(MANDIR):
	install -dm755 $@
$(DESTDIR)/$(MANDIR)/%.1: doc/%.1 $(DESTDIR)/$(MANDIR)
	install -m644 $< $@

install: $(programs_BINDIR) $(doc_MANDIR)
	

.PHONY: all progs clean todo help

