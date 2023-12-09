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
#include "edge.h"
#include "matrix.h"

int main(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  long seed = ts.tv_nsec;
  // Something something random
  seed = (seed & ((seed >> 8) ^ (seed >> 12))) + getpid();
  printf("Seed: %ld\n", seed);
  srandom(seed);

  int edges_count = 15;
  edge_t edges[] = {
      {.node1 = 0, .node2 = 1}, {.node1 = 0, .node2 = 2},
      {.node1 = 0, .node2 = 3}, {.node1 = 1, .node2 = 4},
      {.node1 = 1, .node2 = 5}, {.node1 = 2, .node2 = 6},
      {.node1 = 2, .node2 = 9}, {.node1 = 3, .node2 = 7},
      {.node1 = 3, .node2 = 8}, {.node1 = 4, .node2 = 6},
      {.node1 = 4, .node2 = 8}, {.node1 = 5, .node2 = 7},
      {.node1 = 5, .node2 = 9}, {.node1 = 6, .node2 = 7},
      {.node1 = 8, .node2 = 9},
  };

  edge_t *edges_to_remove = malloc(sizeof(edge_t) * edges_count);
  if (edges_to_remove == NULL) {
    return EXIT_FAILURE;
  }

  int best = edges_count + 1;
  while (1) {
    graph_t graph;
    m_graph_init(&graph, edges, edges_count);

    solution_t solution;
    m_graph_remove_same_edge_connections(&graph, &solution);

    if (solution.len < best) {
      best = solution.len;
      printf("%d\n", solution.len);
    }

    m_graph_free(&graph);

    if (solution.len == 0) {
      break;
    }
  }

  free(edges_to_remove);

  return EXIT_SUCCESS;
}
