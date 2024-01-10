# @file Makefile
# @author Domenic Melcher <e12220857@student.tuwien.ac.at>
# Makefile for http

# Platform-specific commands
UNAME := $(shell uname)

CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

ifeq ($(UNAME),Darwin)
LDFLAGS = -lm
else
LDFLAGS = -lm
endif

OBJECTS = parser.o http.o file_helper.o

.PHONY: all clean .FORCE build-container run-container check-memory-leaks package
all: server client

server: server.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

client: client.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

dummy_generator: dummy_generator.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDFLAGS)

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

server.o: ./src/server.c
client.o: ./src/client.c
parser.o: ./src/parser.c
http.o: ./src/http.c
file_helper.o: ./src/file_helper.c

clean:
	rm -rf *.o server client *.tar.gz ./html ./latex *.stderr *.stdout *.returncodes

doc: .FORCE
	doxygen ./doc/Doxyfile

build-container: .FORCE
	docker build -t uni_bet_http:latest .

run-container: build-container
	docker run --rm -it uni_bet_http:latest

terminal-container: .FORCE
	docker exec -it $$(docker ps -qf "ancestor=uni_bet_http" | awk 'NR==1{print $1}') /bin/bash

check-valgrind: all
	valgrind \
		--leak-check=full \
    	--show-leak-kinds=all \
    	--track-origins=yes \
		--track-fds=yes \
		--trace-children=yes \
    	--verbose \
    	--log-file=valgrind-out.txt \
    	./server

package: .FORCE
	tar -cvzf melcher_http.tar.gz ./Makefile ./src

.FORCE:
