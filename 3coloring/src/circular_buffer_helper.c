/**
 * @file circular_buffer_helper.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Implementation file for helper functions related to circular buffer
 * operations.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "circular_buffer.h"
#include "edge.h"

uint64_t cbh_buffer_free_space(circular_buffer_t *buffer) {
  if (buffer->head >= buffer->tail) {
    return BUFFER_SIZE - (buffer->head - buffer->tail);
  }

  return buffer->tail - buffer->head;
}

uint64_t cbh_edges_size(int edges_len) { return 1l + (uint64_t)edges_len * 2l; }

/*
bool cbh_buffer_has_the_capacity_for_edges(circular_buffer_t *buffer, int len) {
  uint64_t values_to_write = cbh_edges_size(len);

  return values_to_write <= cbh_buffer_free_space(buffer);
}
*/

int cbh_read_edges(circular_buffer_t *buffer, edge_t **edges_ptr) {
  int len = cb_read(buffer);

  if (len < 0) {
    return -1;
  }

  if (len == 0) {
    return 0;
  }

  edge_t *edges = malloc(sizeof(edge_t) * len);
  if (edges == NULL) {
    // If allocation fails, clear buffer and return error
    for (int i = 0; i < len * 2; i++) {
      cb_read(buffer);
    }

    return -1;
  }

  bool encountered_error = false;
  for (int i = 0; i < len; i += 1) {
    edges[i].node1 = cb_read(buffer);
    edges[i].node2 = cb_read(buffer);

    // Check for negative values (error conditions), this should do nothing
    // after a while because the initial -1 values from the buffer should be
    // overriden with old values
    if (edges[i].node1 < 0 || edges[i].node2 < 0) {
      encountered_error = true;
    }
  }

  if (encountered_error == true) {
    free(edges);

    return -1;
  }

  *edges_ptr = edges;

  return len;
}

int cbh_write_edges(circular_buffer_t *buffer, edge_t *edges, int len) {
  int values_written = 0;

  // Write the length of edges to the buffer
  if (cb_write(buffer, len) < 0) {
    fprintf(stderr, "CBH: error when writing 'len'.\n");
    goto clean_up_written_values;
  } else {
    values_written += 1;
  }

  // Write each edge's nodes into the buffer
  for (int i = 0; i < len; i += 1) {
    if (cb_write(buffer, edges[i].node1) < 0) {
      fprintf(stderr, "CBH: error when writing 'node1'.\n");
      goto clean_up_written_values;
    } else {
      values_written += 1;
    }

    if (cb_write(buffer, edges[i].node2) < 0) {
      fprintf(stderr, "CBH: error when writing 'node2'.\n");
      goto clean_up_written_values;
    } else {
      values_written += 1;
    }
  }

  return 0;

clean_up_written_values:
  // If there's an error during write, clean up written values
  for (int i = 0; i < values_written; i += 1) {
    cb_read(buffer);
  }
  return -1;
}
