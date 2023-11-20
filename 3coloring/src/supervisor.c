#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "shared_memory.h"

static char *USAGE = "SYNOPSIS\n\tsupervisor [-n limit] [-w delay] [-p]\n";

typedef struct {
  int limit, delay;

  bool flag_p;
} arguments_t;

int main(void) {
  // ! The struct 'shared_memory_t' must be smaller than 4096 bytes.
  assert(sizeof(shared_memory_t) <= 4096);

  printf("Hello from the supervisor!\n");

  circular_buffer_t *cb = cb_open_master();

  if (cb == NULL) {
    printf("Error in creating circular buffer!\n");
    return EXIT_FAILURE;
  }

  if (cb_close_master(cb) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
