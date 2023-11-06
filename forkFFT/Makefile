CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o complex_number_list.o complex_helper.o child.o

.PHONY: all clean
all: forkFFT

forkFFT: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: ./src/main.c
complex_number_list.o: ./src/complex_number_list.c
complex_helper.o: ./src/complex_helper.c
child.o: ./src/child.c

clean:
	rm -rf *.o forkFFT
