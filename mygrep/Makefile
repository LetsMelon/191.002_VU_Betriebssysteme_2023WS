CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o arguments.o output.o logic.o

.PHONY: all clean package doc .FORCE
all: mygrep

mygrep: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: ./src/main.c
arguments.o: ./src/arguments.c
output.o: ./src/output.c
logic.o: ./src/logic.c

clean:
	rm -rf *.o mygrep melcher_mygrep.tar.gz ./html ./latex

package:
	tar -cvzf melcher_mygrep.tar.gz ./Makefile ./src ./doc

doc: .FORCE
	doxygen ./doc/Doxyfile

.FORCE:
