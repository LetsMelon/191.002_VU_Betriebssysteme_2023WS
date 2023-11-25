#ifndef _CBH
#define _CBH

#include "circular_buffer.h"
#include "edge.h"

int cbh_read_edges(circular_buffer_t *buffer, edge_t **edges);
int cbh_write_edges(circular_buffer_t *buffer, edge_t *edges, int len);

#endif
