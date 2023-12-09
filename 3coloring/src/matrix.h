/**
 * @file matrix.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Header file defining a graph structure optimized for the usage of the
 * edges in between nodes.
 */

#ifndef _M
#define _M

#include <stdbool.h>
#include <stdint.h>

#include "edge.h"

/**
 * @struct adjacent_matrix_t
 * @brief Structure representing an adjacency matrix for a graph.
 */
// TODO replace with an 1d array instead of an 2d array
typedef struct {
  int n;      /**< Number of nodes */
  int **data; /**< 2D array representing the adjacency matrix */
} adjacent_matrix_t;

/**
 * @brief Prints the adjacency matrix human readable to stdout.
 * @param adjacent_matrix Pointer to the adjacency matrix to be printed.
 */
void m_am_print(adjacent_matrix_t *adjacent_matrix);

/**
 * @struct nodes_t
 * @brief Structure representing nodes in a graph.
 */
typedef struct {
  int id;         /**< Node ID */
  uint64_t color; /**< Color associated with the node */
} nodes_t;

/**
 * @struct graph_t
 * @brief Structure representing a graph.
 */
typedef struct {
  int edges_count;         /**< Number of edges */
  int nodes_count;         /**< Number of nodes */
  nodes_t *nodes;          /**< Array of nodes */
  adjacent_matrix_t edges; /**< Adjacency matrix representing edges */
} graph_t;

/**
 * @brief Initializes a graph with provided edges.
 * @param graph Pointer to the graph to be initialized.
 * @param edges Array of edges to initialize the graph.
 * @param edges_count Number of edges in the array.
 * @return Returns 0 upon successful initialization.
 */
int m_graph_init(graph_t *graph, edge_t *edges, int edges_count);

/**
 * @brief Frees memory allocated for a graph.
 * @param graph Pointer to the graph to be freed.
 */
void m_graph_free(graph_t *graph);

/**
 * @brief Removes same edge connections from the graph.
 * @param graph Pointer to the graph.
 * @param edges_to_remove Pointer to an array of edges that had been removed.
 * @return Returns the number of removed edges, -> len(edges_to_remove).
 */
int m_graph_remove_same_edge_connections(graph_t *graph,
                                         edge_t **edges_to_remove);

#endif
