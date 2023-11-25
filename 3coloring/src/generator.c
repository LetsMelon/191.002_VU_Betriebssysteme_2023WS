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
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  long seed = ts.tv_nsec;
  // Something something random
  seed = (seed & ((seed >> 8) ^ (seed >> 12))) + getpid();
  printf("Seed: %ld\n", seed);
  srandom(seed);

  shared_memory_t shared_memory;
  if (sm_open(&shared_memory, false) < 0) {
    return EXIT_FAILURE;
  }

  int input_len = 0;
  for (int i = 1; i < argc; i += 1) {
    input_len += strlen(argv[i]);
    if (i + 1 < argc) {
      input_len += 1;
    }
  }

  char *input_concat = (char *)malloc(sizeof(char) * (input_len + 1));
  if (input_concat == NULL) {
    sm_close(&shared_memory, false);

    return EXIT_FAILURE;
  }

  int already_copied = 0;
  for (int i = 1; i < argc; i += 1) {
    char *argv_i = argv[i];

    strcpy(input_concat + already_copied, argv_i);
    already_copied += strlen(argv_i);

    input_concat[already_copied] = ' ';
    already_copied += 1;
  }

  input_concat[input_len] = '\0';

  string_list_t edges;

  p_split_at(input_concat, ' ', &edges);

  free(input_concat);

  if (edges.num < 1) {
    sl_free(&edges);
    sm_close(&shared_memory, false);

    printf("%s", USAGE);

    return EXIT_FAILURE;
  }

  edge_t *parsed_edges = (edge_t *)malloc(sizeof(edge_t) * edges.num);
  if (parsed_edges == NULL) {
    sl_free(&edges);
    sm_close(&shared_memory, false);

    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < edges.num; i += 1) {
    edge_t edge;
    if (p_parse_as_edge(edges.values[i], &edge) != 0) {
      sl_free(&edges);
      free(parsed_edges);
      sm_close(&shared_memory, false);

      return EXIT_FAILURE;
    }

    parsed_edges[i] = edge;
  }

  int n = edges.num;
  sl_free(&edges);

  for (int xx = 0; xx < 5; xx += 1) {
    graph_t graph;
    m_graph_init(&graph, parsed_edges, n);

    int edges_without_zero_count = 0;
    edge_t *edges_without_zero =
        (edge_t *)malloc(sizeof(edge_t) * graph.edges_count);
    if (edges_without_zero == NULL) {
      m_graph_free(&graph);
      free(parsed_edges);
      sm_close(&shared_memory, false);

      return EXIT_FAILURE;
    }

    int current_start_index = 0;
    while (m_graph_is_3colorable(&graph) == false &&
           current_start_index < graph.nodes_count) {

      edge_t *same_color_neighbors;
      int same_color_neighbors_count = m_graph_get_same_color_edges(
          &graph, graph.nodes[current_start_index].id, &same_color_neighbors);

      if (same_color_neighbors_count == 0) {
        free(same_color_neighbors);

        current_start_index += 1;

        continue;
      }

      if (same_color_neighbors_count == -1) {
        m_graph_free(&graph);
        free(parsed_edges);
        sm_close(&shared_memory, false);

        return EXIT_FAILURE;
      }

      for (int i = 0; i < same_color_neighbors_count; i += 1) {
        edge_t e = same_color_neighbors[i];

        printf("DEBUG: %d->%d\n", e.node1, e.node2);
        if (e.node1 != e.node2) {
          edges_without_zero[edges_without_zero_count] = e;
          edges_without_zero_count += 1;

          m_graph_remove_edge(&graph, &e);
        }
      }

      free(same_color_neighbors);
    }

    m_graph_free(&graph);

    // TODO check for shutdown signal
    sem_wait(shared_memory.semaphore_buffer_mutex);
    if (cbh_write_edges(shared_memory.buffer, edges_without_zero,
                        edges_without_zero_count) < 0) {
      m_graph_free(&graph);
      free(parsed_edges);
      sm_close(&shared_memory, false);

      return EXIT_FAILURE;
    }
    sem_post(shared_memory.semaphore_buffer_mutex);

    free(edges_without_zero);

    sleep(2);
  }

  free(parsed_edges);

  sm_close(&shared_memory, false);

  return EXIT_SUCCESS;
}
