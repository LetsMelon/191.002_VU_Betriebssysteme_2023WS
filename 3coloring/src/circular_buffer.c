#include <stdbool.h>
#include <stdint.h>

#include "circular_buffer.h"

int cb_init(circular_buffer_t *buffer) {
  buffer->head = 0;
  buffer->tail = 0;
  buffer->count = 0;

  return 0;
}

void cb_free(circular_buffer_t *buffer) {}

bool cb_is_full(circular_buffer_t *buffer) {
  return BUFFER_SIZE == buffer->count;
}

bool cb_is_empty(circular_buffer_t *buffer) { return buffer->count == 0; }

int cb_write(circular_buffer_t *buffer, int value) {
  if (cb_is_full(buffer) == true) {
    return -1;
  }

  buffer->data[buffer->head] = value;
  buffer->head = (buffer->head + 1) % BUFFER_SIZE;
  buffer->count += 1;

  return 0;
}

int cb_read(circular_buffer_t *buffer) {
  if (cb_is_empty(buffer) == true) {
    return -1;
  }

  int value = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
  buffer->count -= 1;

  return value;
}

static void write_value(circular_buffer_t *circular_buffer, int value) {
  printf("Index: %d, value: %d\n", circular_buffer->shared_memory->write_index,
         value);

  circular_buffer->shared_memory
      ->data[circular_buffer->shared_memory->write_index] = value;

  circular_buffer->shared_memory->write_index += 1;
  circular_buffer->shared_memory->write_index %= MAX_DATA_COUNT;
}

int cb_write_solution(circular_buffer_t *circular_buffer,
                      edge_t *edges_to_remove, int edges_to_remove_len) {
  if (sem_wait(circular_buffer->semaphore_write_to_buffer) == -1) {
    sem_post(circular_buffer->semaphore_write_to_buffer);
    return -1;
  }

  circular_buffer->shared_memory->generators_wrote_to_shared_memory += 1;

  write_value(circular_buffer, edges_to_remove_len);

  for (int i = 0; i < edges_to_remove_len; i += 1) {
    if (circular_buffer->shared_memory->in_shutdown) {
      sem_post(circular_buffer->semaphore_write_to_buffer);

      return -1;
    }

    for (int j = 0; j < 2; j += 1) {
      int value_to_write =
          (int[]){edges_to_remove[i].node1, edges_to_remove[i].node2}[j];

      write_value(circular_buffer, value_to_write);
    }
  }

  sem_post(circular_buffer->semaphore_write_to_buffer);

  return 0;
}

void add_to_index(int *index, int value) {
  *index += 1;
  *index %= MAX_DATA_COUNT;
}

int cb_read_single_value(circular_buffer_t *circular_buffer) {
  printf("DEBUG: Read: %d\n", circular_buffer->shared_memory->read_index);

  int data = circular_buffer->shared_memory
                 ->data[circular_buffer->shared_memory->read_index];

  add_to_index(&circular_buffer->shared_memory->read_index, 1);

  return data;
}

void cb_read_edge(circular_buffer_t *circular_buffer, edge_t *edge) {
  int start = cb_read_single_value(circular_buffer);
  int end = cb_read_single_value(circular_buffer);

  edge->node1 = start;
  edge->node2 = end;
}

int cb_read_edges(circular_buffer_t *circular_buffer, edge_t **edges) {
  if (sem_wait(circular_buffer->semaphore_write_to_buffer) == -1) {
    sem_post(circular_buffer->semaphore_write_to_buffer);
    return -1;
  }

  if (circular_buffer->shared_memory->generators_wrote_to_shared_memory < 1) {
    sem_post(circular_buffer->semaphore_write_to_buffer);
    return -1;
  }

  int edges_to_read = cb_read_single_value(circular_buffer);

  if (edges_to_read == 0) {
    *edges = NULL;

    return 0;
  }

  printf("DEBUG: Edges: %d\n", edges_to_read);

  *edges = (edge_t *)malloc(sizeof(edge_t) * edges_to_read);
  if (*edges == NULL) {
    add_to_index(&circular_buffer->shared_memory->read_index,
                 edges_to_read * 2);

    circular_buffer->shared_memory->generators_wrote_to_shared_memory -= 1;

    sem_post(circular_buffer->semaphore_write_to_buffer);

    return -1;
  }

  printf("DEBUG: I'm now reading values from the buffer\n");

  for (int i = 0; i < edges_to_read; i++) {
    printf("DEBUG: i = %d\n", i);

    edge_t e;
    cb_read_edge(circular_buffer, &e);

    printf("DEBUG: edge {%d, %d}\n", e.node1, e.node2);

    *edges[i] = e;
  }

  printf("DEBUG: after loop\n");

  circular_buffer->shared_memory->generators_wrote_to_shared_memory -= 1;

  printf("Hello\n");

  sem_post(circular_buffer->semaphore_write_to_buffer);
  printf("Hello_2\n");
  return edges_to_read;
}
