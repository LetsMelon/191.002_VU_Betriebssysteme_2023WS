#include <stdbool.h>
#include <stdlib.h>

#include "circular_buffer.h"
#include "edge.h"

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
    for (int i = 0; i < len * 2; i++) {
      cb_read(buffer);
    }

    return -1;
  }

  bool encountered_error = false;
  for (int i = 0; i < len; i += 1) {
    edges[i].node1 = cb_read(buffer);
    edges[i].node2 = cb_read(buffer);

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

  if (cb_write(buffer, len) < 0) {
    goto clean_up_written_values;
  } else {
    values_written += 1;
  }

  for (int i = 0; i < len; i += 1) {
    if (cb_write(buffer, edges[i].node1) < 0) {
      goto clean_up_written_values;
    } else {
      values_written += 1;
    }

    if (cb_write(buffer, edges[i].node2) < 0) {
      goto clean_up_written_values;
    } else {
      values_written += 1;
    }
  }

  return 0;

clean_up_written_values:
  for (int i = 0; i < values_written; i += 1) {
    cb_read(buffer);
  }
  return -1;
}
