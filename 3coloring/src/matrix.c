#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

static uint64_t COLORS[] = {0xffff0000, 0xff00ff00, 0xff0000ff};
static uint64_t COLORS_COUNT = 3;

static uint64_t random_color(void) { return COLORS[random() % COLORS_COUNT]; }

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

  for (int i = 0; i < graph->nodes_count; i++) {
    graph->nodes[i].color = random_color();
  }

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

static int m_graph_find_node(graph_t *graph, int id, nodes_t *node) {
  for (int i = 0; i < graph->nodes_count; i++) {
    if (graph->nodes[i].id == id) {
      *node = graph->nodes[i];
      return 0;
    }
  }

  return -1;
}

int m_graph_get_same_color_edges(graph_t *graph, int u, edge_t **neighbors) {
  *neighbors = (edge_t *)malloc(sizeof(edge_t) * graph->nodes_count);
  if (neighbors == NULL) {
    return -1;
  }

  nodes_t node;
  if (m_graph_find_node(graph, u, &node) != 0) {
    return -1;
  }

  int u_index = m_graph_get_index_from_node(graph, u);
  if (u_index < 0) {
    return -1;
  }

  int *u_neighbors = graph->edges.data[u_index];
  int found_neighbors = 0;
  for (int i = 0; i < graph->nodes_count; i += 1) {
    nodes_t neighbor_node = graph->nodes[i];

    if (neighbor_node.id == node.id || u_neighbors[i] == 0) {
      continue;
    }

    if (node.color == neighbor_node.color) {
      edge_t *e = malloc(sizeof(edge_t));
      if (e == NULL) {
        return -1;
      }

      e->node1 = node.id;
      e->node2 = neighbor_node.id;

      neighbors[found_neighbors] = e;

      found_neighbors += 1;
    }
  }

  return found_neighbors;
}

void m_graph_remove_edge(graph_t *graph, edge_t *edge) {
  int n1_index = m_graph_get_index_from_node(graph, edge->node1);
  int n2_index = m_graph_get_index_from_node(graph, edge->node2);

  if (n1_index < 0 || n2_index < 0) {
    return;
  }

  graph->edges_count -= 2;

  graph->edges.data[n1_index][n2_index] = 0;
  graph->edges.data[n2_index][n1_index] = 0;
}

bool m_graph_is_3colorable(graph_t *graph) {
  for (int i = 0; i < graph->nodes_count; i += 1) {
    edge_t *neighbors;
    int n = m_graph_get_same_color_edges(graph, graph->nodes[i].id, &neighbors);

    if (neighbors != NULL) {
      free(neighbors);
    }

    if (n != 0) {
      return false;
    }
  }

  return true;
}
