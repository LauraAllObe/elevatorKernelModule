all: part1.trace empty.trace

part1.trace: part1
	strace -o part1.trace ./part1

empty.trace: empty
	strace -o empty.trace ./empty

part1: part1.c
	gcc -o part1 part1.c

empty: empty.c
	gcc -o empty empty.c

.PHONY: all clean

clean:
	rm -f part1 empty part1.trace empty.trace
