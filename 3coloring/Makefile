# @file Makefile
# @author Domenic Melcher <e12220857@student.tuwien.ac.at>
# Makefile for 3coloring

# Platform-specific commands
UNAME := $(shell uname)

CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

ifeq ($(UNAME),Darwin)
LDFLAGS = -pthread -lpthread -lm
else
LDFLAGS = -pthread -lpthread -lm -lrt
endif

OBJECTS = parser.o edge.o matrix.o circular_buffer.o circular_buffer_helper.o shared_memory.o

.PHONY: all clean .FORCE build-container run-container check-memory-leaks package
all: supervisor generator

supervisor: supervisor.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

generator: generator.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

supervisor.o: ./src/supervisor.c
generator.o: ./src/generator.c
parser.o: ./src/parser.c
edge.o: ./src/edge.c
matrix.o: ./src/matrix.c
circular_buffer.o: ./src/circular_buffer.c
shared_memory.o: ./src/shared_memory.c
circular_buffer_helper.o: ./src/circular_buffer_helper.c

clean:
	rm -rf *.o supervisor generator dummy_generator melcher_3coloring.tar.gz ./html ./latex *.stderr *.stdout *.returncodes

doc: .FORCE
	doxygen ./doc/Doxyfile

build-container: .FORCE
	docker build -t uni_bet_3coloring:latest .

run-container: build-container
	docker run --rm -it uni_bet_3coloring:latest

terminal-container: .FORCE
	docker exec -it $$(docker ps -qf "ancestor=uni_bet_3coloring" | awk 'NR==1{print $1}') /bin/bash

check-valgrind: all
	valgrind \
		--leak-check=full \
    	--show-leak-kinds=all \
    	--track-origins=yes \
		--track-fds=yes \
		--trace-children=yes \
    	--verbose \
    	--log-file=valgrind-out.txt \
    	./generator 0-1 0-2 1-3 2-3 4-5 4-6

package: .FORCE
	tar -cvzf melcher_3coloring.tar.gz ./Makefile ./src

.FORCE:
