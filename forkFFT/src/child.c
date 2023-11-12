/**
 * @file child.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions useful for creating a child process and
 * inter process communication via pipes.
 */

#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "child.h"

/**
 * @details the len of pipes mut be at least 2
 */
int c_close_pipes(int *pipes) {
  for (int i = 0; i < 2; i++) {
    if (close(pipes[i]) != 0) {
      fprintf(stderr, "Error when closing pipe %d at index %d.\n", pipes[i], i);
      return -1;
    }
  }

  return 0;
}

void c_create_child(int *pipe_stdin, int *pipe_stdout) {
  dup2(pipe_stdin[READ], STDIN_FILENO);
  if (c_close_pipes(pipe_stdin) != 0) {
    fprintf(stderr, "Error in closing pipes 'pipe_stdin'.\n");
    exit(EXIT_FAILURE);
  }

  dup2(pipe_stdout[WRITE], STDOUT_FILENO);
  if (c_close_pipes(pipe_stdout) != 0) {
    fprintf(stderr, "Error in closing pipes 'pipe_stdout'.\n");
    exit(EXIT_FAILURE);
  }

  execlp("./forkFFT", "./forkFFT", NULL);

  fprintf(stderr, "Error in execlp.\n");
  exit(EXIT_FAILURE);
}

int c_read_n_complex_numbers_from_fd(int fd, float complex *array,
                                     int numbers) {
  // A line with one complex value from the child process can have a maximum of
  // 64 chars. This value can be increased in the future.
  size_t n_bytes = numbers * 64 * sizeof(char);

  char *buf = (char *)malloc(n_bytes);
  if (buf == NULL) {
    return -1;
  }

  if (read(fd, buf, n_bytes) == -1) {
    free(buf);
    return -1;
  }

  char *endptr = buf;
  int read_numbers = 0;

  for (int i = 0; i < numbers; i++) {
    // this can be replaced with the helper parsing functions from 'parser.h'.
    // But because the output format from the child process is always the same
    // this function should never throw an error or encounter a bug.
    float c_r = strtof(endptr, &endptr);
    float c_i = strtof(endptr, &endptr);
    endptr += 3;

    array[i] = c_r + c_i * I;

    read_numbers += 1;
  }

  free(buf);

  return read_numbers == numbers ? 0 : -1;
}
