#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "shared_memory.h"

volatile sig_atomic_t quit = 0;
void handle_signal(int signal) { quit = 1; }

static char *USAGE = "SYNOPSIS\n\tsupervisor [-n limit] [-w delay] [-p]\n";

typedef struct {
  int limit, delay;

  bool flag_p;
} arguments_t;

int main(void) {
  // ! The struct 'shared_memory_t' must be smaller than 4096 bytes.
  assert(sizeof(shared_memory_t) <= 4096);

  struct sigaction sa = {.sa_handler = handle_signal};
  sigaction(SIGINT, &sa, NULL);

  printf("Hello from the supervisor!\n");

  circular_buffer_t *circular_buffer = cb_open_master();
  if (circular_buffer == NULL) {
    fprintf(stderr, "Error in creating circular buffer!\n");
    return EXIT_FAILURE;
  }

  while (!quit) {
    edge_t *edges;
    int edges_read = cb_read_edges(circular_buffer, &edges);

    if (edges_read < 0) {
      continue;
    }

    printf("Edges to delete: %d\n", edges_read);

    if (edges_read == 0) {
      continue;
    }

    free(edges);

    printf("Freed edges.\n");

    sleep(5);
  }

  if (cb_close_master(circular_buffer) != 0) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// ./generator 0-1 0-2 1-2 1-3 2-3
