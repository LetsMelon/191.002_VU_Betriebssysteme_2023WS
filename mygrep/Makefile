CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o arguments.o

.PHONY: all clean
all: mygrep

mygrep: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: ./src/main.c
arguments.o: ./src/arguments.c

clean:
	rm -rf *.o mygrep
