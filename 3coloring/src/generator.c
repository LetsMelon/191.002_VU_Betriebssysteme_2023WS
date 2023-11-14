#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "edge.h"
#include "matrix.h"
#include "parser.h"

static char *USAGE = "SYNOPSIS\n\tgenerator EDGE1...\nEXAMPLE\n\tgenerator 0-1 "
                     "0-2 0-3 1-2 1-3 2-3\n";

/*
$ ./generator 0-1 0-5 1-3
Seed: 216039000
0-1
0-0 // ?
5-0
3-1
*/

int main(int argc, char **argv) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  long seed = ts.tv_nsec;
  printf("Seed: %ld\n", seed);
  srandom(seed);

  int input_len = 0;
  for (int i = 1; i < argc; i += 1) {
    input_len += strlen(argv[i]);
    if (i + 1 < argc) {
      input_len += 1;
    }
  }

  char *input_concat = (char *)malloc(sizeof(char) * (input_len + 1));
  if (input_concat == NULL) {
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

    printf("%s", USAGE);

    return EXIT_FAILURE;
  }

  // sl_print(&edges);

  edge_t *parsed_edges = (edge_t *)malloc(sizeof(edge_t) * edges.num);
  if (parsed_edges == NULL) {
    sl_free(&edges);

    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < edges.num; i += 1) {
    edge_t edge;
    if (p_parse_as_edge(edges.values[i], &edge) != 0) {
      sl_free(&edges);
      free(parsed_edges);

      return EXIT_FAILURE;
    }

    parsed_edges[i] = edge;
  }

  int n = edges.num;
  sl_free(&edges);

  graph_t graph;
  m_graph_init(&graph, parsed_edges, n);

  free(parsed_edges);

  // m_am_print(&graph.edges);

  for (int i = 0; i < graph.nodes_count; i += 1) {
    edge_t *color_neighbors;
    int color_neighbors_count = m_graph_get_same_color_edges(
        &graph, graph.nodes[i].id, &color_neighbors);

    if (color_neighbors_count == 0) {
      free(color_neighbors);
      continue;
    }

    if (color_neighbors_count == -1) {
      m_graph_free(&graph);
      return EXIT_FAILURE;
    }

    for (int i = 0; i < color_neighbors_count; i++) {
      edge_t e = color_neighbors[i];
      // print the edge that must be removed to stdout
      printf("%d-%d\n", e.node1, e.node2);
      m_graph_remove_edge(&graph, &e);
    }

    free(color_neighbors);
  }

  // m_am_print(&graph.edges);

  m_graph_free(&graph);

  return EXIT_SUCCESS;
}
