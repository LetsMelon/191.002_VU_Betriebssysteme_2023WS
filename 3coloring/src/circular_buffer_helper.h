/**
 * @file circular_buffer_helper.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Header file defining helper functions for circular buffer operations.
 */

#ifndef _CBH
#define _CBH

#include <stdint.h>

#include "circular_buffer.h"
#include "edge.h"

/**
 * @brief Calculates the free space available in the circular buffer.
 * @param buffer Pointer to the circular buffer.
 * @return Returns the amount of free space in the buffer.
 */
uint64_t cbh_buffer_free_space(circular_buffer_t *buffer);

/**
 * @brief Calculates the total size occupied by edges.
 * @param edges_len The number of edges.
 * @return Returns the total size occupied by the edges.
 */
uint64_t cbh_edges_size(int edges_len);

// busy waiting
// bool cbh_buffer_has_the_capacity_for_edges(circular_buffer_t *buffer, int
// len);

/**
 * @brief Reads edges from the circular buffer.
 * @param buffer Pointer to the circular buffer.
 * @param edges Pointer to the array of edges.
 * @return Returns the number of edges read from the buffer, -1 if an error
 * occurred and stores the edges read into edges.
 */
int cbh_read_edges(circular_buffer_t *buffer, edge_t **edges);

/**
 * @brief Writes edges into the circular buffer.
 * @param buffer Pointer to the circular buffer.
 * @param edges Pointer to the array of edges to be written.
 * @param len Number of edges to be written.
 * @return Returns 0 if no error occurred and otherwise -1.
 */
int cbh_write_edges(circular_buffer_t *buffer, edge_t *edges, int len);

#endif
