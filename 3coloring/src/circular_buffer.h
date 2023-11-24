#ifndef _CB
#define _CB

#include <semaphore.h>

#include "edge.h"
#include "shared_memory.h"

#define SEMAPHORE_USED "12220857_used"
#define SEMAPHORE_WRITE_TO_BUFFER "12220857_mutex"

typedef struct {
  shared_memory_t *shared_memory;

  sem_t *semaphore_used, *semaphore_write_to_buffer;

  int fd;
} circular_buffer_t;

circular_buffer_t *cb_open_master();
circular_buffer_t *cb_open_slave();

int cb_close_master(circular_buffer_t *circular_buffer);
int cb_close_slave(circular_buffer_t *circular_buffer);

int cb_write_solution(circular_buffer_t *circular_buffer,
                      edge_t *edges_to_remove, int edges_to_remove_len);

int cb_read_edges(circular_buffer_t *circular_buffer, edge_t **edges);

#endif
