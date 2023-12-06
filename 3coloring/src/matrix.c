#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

static uint64_t COLORS[] = {0xffff0000, 0xff00ff00, 0xff0000ff};
static uint64_t COLORS_COUNT = 3;

static uint64_t random_color(void) { return COLORS[random() % COLORS_COUNT]; }

static void m_graph_color_randomly(graph_t *graph) {
  for (int i = 0; i < graph->nodes_count; i++) {
    graph->nodes[i].color = random_color();
  }
}

static int am_init(adjacent_matrix_t *adjacent_matrix, int n) {
  int **am = (int **)malloc(sizeof(int *) * n);
  if (am == NULL) {
    return -1;
  }

  for (int i = 0; i < n; i += 1) {
    am[i] = (int *)calloc(sizeof(int), n);
    if (am[i] == NULL) {
      for (int j = 0; j < i; j += 1) {
        free(am[j]);
      }
      free(am);

      return -1;
    }
  }

  adjacent_matrix->n = n;
  adjacent_matrix->data = am;

  return 0;
}

static void am_free(adjacent_matrix_t *adjacent_matrix) {
  for (int i = 0; i < adjacent_matrix->n; i += 1) {
    free(adjacent_matrix->data[i]);
  }
  free(adjacent_matrix->data);
}

void m_am_print(adjacent_matrix_t *adjacent_matrix) {
  printf("        |");
  for (int i = 0; i < adjacent_matrix->n; i += 1) {
    printf("\t%d |", i);
  }
  printf("\n--------|--");
  for (int i = 0; i < adjacent_matrix->n; i += 1) {
    printf("--------");
  }
  printf("\n");

  for (int i = 0; i < adjacent_matrix->n; i += 1) {
    printf("%d\t|", i);

    for (int j = 0; j < adjacent_matrix->n; j += 1) {

      printf("\t%d", adjacent_matrix->data[i][j]);
    }
    printf(" |\n");
  }
}

static int m_graph_get_index_from_node(graph_t *graph, int id) {
  for (int i = 0; i < graph->nodes_count; ++i) {
    if (graph->nodes[i].id == id) {
      return i;
    }
  }

  return -1;
}

int m_graph_init(graph_t *graph, edge_t *edges, int edges_count) {
  graph->nodes = (nodes_t *)malloc(sizeof(nodes_t) * edges_count * 2);
  graph->nodes_count = 0;
  if (graph->nodes == NULL) {
    return -1;
  }

  for (int i = 0; i < edges_count; i += 1) {
    bool already_counted = false;
    for (int j = 0; j < graph->nodes_count; j += 1) {
      if (graph->nodes[j].id == edges[i].node1) {
        already_counted = true;
        break;
      }
    }

    if (already_counted == false) {
      graph->nodes[graph->nodes_count].id = edges[i].node1;
      graph->nodes_count += 1;
    }

    already_counted = false;
    for (int j = 0; j < graph->nodes_count; j += 1) {
      if (graph->nodes[j].id == edges[i].node2) {
        already_counted = true;
        break;
      }
    }

    if (already_counted == false) {
      graph->nodes[graph->nodes_count].id = edges[i].node2;
      graph->nodes_count += 1;
    }
  }

  m_graph_color_randomly(graph);

  graph->edges_count = edges_count;

  if (am_init(&graph->edges, graph->nodes_count) != 0) {
    return -1;
  }

  for (int i = 0; i < graph->edges_count; i += 1) {
    edge_t e = edges[i];

    int n1_index = m_graph_get_index_from_node(graph, e.node1);
    int n2_index = m_graph_get_index_from_node(graph, e.node2);

    if (n1_index < 0 || n2_index < 0) {
      return -1;
    }

    graph->edges.data[n1_index][n2_index] = 1;
    graph->edges.data[n2_index][n1_index] = 1;
  }

  return 0;
}

void m_graph_free(graph_t *graph) {
  am_free(&graph->edges);
  free(graph->nodes);

  graph->edges_count = 0;
  graph->nodes_count = 0;
}

static bool m_graph_nodes_have_connection(graph_t *graph, int n1_index,
                                          int n2_index) {
  return graph->edges.data[n1_index][n2_index] == 1;
}

static void m_graph_remove_edge(graph_t *graph, int n1_index, int n2_index) {
  graph->edges.data[n1_index][n2_index] = 0;
  graph->edges.data[n2_index][n1_index] = 0;
}

int m_graph_remove_same_edge_connections(graph_t *graph,
                                         edge_t **edges_to_remove) {
  int edges_had_been_removed = 0;

  for (int node_index = 0; node_index < graph->nodes_count; node_index += 1) {
    // if (m_graph_is_3colorable(graph) == 1) {
    //   break;
    // }

    nodes_t node = graph->nodes[node_index];

    for (int other_node_index = 0; other_node_index < graph->nodes_count;
         other_node_index += 1) {
      if (node_index == other_node_index) {
        continue;
      }

      if (m_graph_nodes_have_connection(graph, node_index, other_node_index) ==
          false) {
        continue;
      }

      nodes_t other_node = graph->nodes[other_node_index];

      if (node.color != other_node.color) {
        continue;
      }

      // printf("CHECK: %d (%llu) -> %d (%llu)\n", node.id, node.color,
      //        other_node.id, other_node.color);

      (*edges_to_remove)[edges_had_been_removed].node1 = node.id;
      (*edges_to_remove)[edges_had_been_removed].node2 = other_node.id;

      m_graph_remove_edge(graph, node_index, other_node_index);

      edges_had_been_removed += 1;
    }
  }

  return edges_had_been_removed;
}
