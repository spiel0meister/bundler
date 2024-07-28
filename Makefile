CC=gcc -std=c11
CFLAGS=-Wall -Wextra -ggdb

all: bundler

out.h: bundler
	rm -f $@
	./$< * > $@

bundler: bundler.c
	$(CC) $(CFLAGS) -o $@ $<

