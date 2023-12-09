/**
 * @file edge.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Header file defining the structure for an edge.
 */

#ifndef _E
#define _E

/*! \def MAX_EDGES
    \brief Max edges the program allows to be deleted from the graph
*/

#define MAX_EDGES 32

/**
 * @struct edge_t
 * @brief Structure representing an edge between two nodes.
 */
typedef struct {
  int node1; /**< First node of the edge */
  int node2; /**< Second node of the edge */
} edge_t;

#endif
