#ifndef _c
#define _c

#include <complex.h>

#define READ 0
#define WRITE 1

int close_pipes(int *pipes);
void create_child(int *pipe_stdin, int *pipe_stdout);

int c_read_n_complex_numbers_from_fd(int fd, float complex *array, int numbers);

#endif
