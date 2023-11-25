#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "circular_buffer_helper.h"
#include "edge.h"
#include "matrix.h"
#include "parser.h"
#include "shared_memory.h"

static char *USAGE = "SYNOPSIS\n\tgenerator EDGE1...\nEXAMPLE\n\tgenerator 0-1 "
                     "0-2 0-3 1-2 1-3 2-3\n";

int main(int argc, char **argv) {
  shared_memory_t shared_memory;
  if (sm_open(&shared_memory, false) < 0) {
    return EXIT_FAILURE;
  }

  printf("Wait...!\n");
  sem_wait(shared_memory.semaphore_buffer_mutex);

  printf("Lets go!\n");

  int len = 2;
  edge_t *edges = malloc(sizeof(edge_t) * len);
  if (edges != NULL) {

    edges[0].node1 = 0;
    edges[0].node2 = 2;

    edges[1].node1 = 1;
    edges[1].node2 = 2;

    cbh_write_edges(shared_memory.buffer, edges, len);

    free(edges);
  }

  printf("Wrote everything to the shared buffer!\n");

  sem_post(shared_memory.semaphore_buffer_mutex);

  sm_close(&shared_memory, false);

  return EXIT_SUCCESS;
}
