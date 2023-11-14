#ifndef _M
#define _M

#include <stdbool.h>
#include <stdint.h>

#include "edge.h"

typedef struct {
  int n;

  int **data;
} adjacent_matrix_t;

void m_am_print(adjacent_matrix_t *adjacent_matrix);

typedef struct {
  int id;
  uint64_t color;
} nodes_t;

typedef struct {
  int edges_count, nodes_count;

  nodes_t *nodes;

  adjacent_matrix_t edges;
} graph_t;

int m_graph_init(graph_t *graph, edge_t *edges, int edges_count);

void m_graph_free(graph_t *graph);

int m_graph_get_same_color_edges(graph_t *graph, int u, edge_t **neighbors);

void m_graph_remove_edge(graph_t *graph, edge_t *edge);

bool m_graph_is_3colorable(graph_t *graph);

#endif
