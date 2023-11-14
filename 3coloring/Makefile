# @file Makefile
# @author Domenic Melcher <e12220857@student.tuwien.ac.at>
# Makefile for 3coloring

CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS) -pthread

OBJECTS = parser.o edge.o matrix.o

.PHONY: all clean .FORCE build-container run-container check-memory-leaks package
all: supervisor generator

supervisor: supervisor.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

generator: generator.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

supervisor.o: ./src/supervisor.c
generator.o: ./src/generator.c
parser.o: ./src/parser.c
edge.o: ./src/edge.c
matrix.o: ./src/matrix.c

clean:
	rm -rf *.o supervisor generator melcher_3coloring.tar.gz ./html ./latex

doc: .FORCE
	doxygen ./doc/Doxyfile

build-container: .FORCE
	docker build -t uni_bet_3coloring:latest .

run-container: build-container
	docker run --rm -it uni_bet_3coloring:latest

check-valgrind: all
	valgrind \
		--leak-check=full \
    	--show-leak-kinds=all \
    	--track-origins=yes \
		--track-fds=yes \
		--trace-children=yes \
    	--verbose \
    	--log-file=valgrind-out.txt \
    	./generator

package:
	tar -cvzf melcher_3coloring.tar.gz ./Makefile ./src

.FORCE:
