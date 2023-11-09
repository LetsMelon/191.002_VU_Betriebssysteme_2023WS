CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o complex_number_list.o complex_helper.o child.o

.PHONY: all clean .FORCE build-container run-container check-memory-leaks package
all: forkFFT

forkFFT: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: ./src/main.c
complex_number_list.o: ./src/complex_number_list.c
complex_helper.o: ./src/complex_helper.c
child.o: ./src/child.c

clean:
	rm -rf *.o forkFFT melcher_forkfft.tar.gz

build-container: .FORCE
	docker build -t uni_bet_forkfft:latest .

run-container: build-container
	docker run --rm -it uni_bet_forkfft:latest

check-valgrind: forkFFT
	valgrind \
		--leak-check=full \
    	--show-leak-kinds=all \
    	--track-origins=yes \
		--track-fds=yes \
		--trace-children=yes \
    	--verbose \
    	--log-file=valgrind-out-sine.txt \
    	./forkFFT < ./test/sine.txt

package:
	tar -cvzf melcher_forkfft.tar.gz ./Makefile ./src

.FORCE:
