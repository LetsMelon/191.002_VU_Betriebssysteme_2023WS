/**
 * @file edge.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Header file defining the structure for an edge.
 */

#ifndef _E
#define _E

/**
 * @struct edge_t
 * @brief Structure representing an edge between two nodes.
 */
typedef struct {
  int node1; /**< First node of the edge */
  int node2; /**< Second node of the edge */
} edge_t;

#endif
