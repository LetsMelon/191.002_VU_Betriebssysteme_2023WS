#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "edge.h"
#include "shared_memory.h"

static circular_buffer_t *cb_open(bool is_master) {
  int fd;
  shared_memory_t *shared_memory;

  if (is_master) {
    shared_memory = sm_open_master(&fd);
  } else {
    shared_memory = sm_open_slave(&fd);
  }

  if (shared_memory == NULL) {
    fprintf(stderr, "Error in opening shared memory\n");
    return NULL;
  }

  circular_buffer_t *circular_buffer =
      (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
  if (circular_buffer == NULL) {
    fprintf(stderr, "Error in allocating memory for 'circular_buffer_t'\n");
    if (is_master) {
      if (sm_close_master(shared_memory, fd) != 0) {
        return NULL;
      }
    } else {
      if (sm_close_slave(shared_memory, fd) != 0) {
        return NULL;
      }
    }
  }

  circular_buffer->shared_memory = shared_memory;
  circular_buffer->fd = fd;

  if (is_master == true) {
    circular_buffer->semaphore_write_to_buffer = NULL;
    circular_buffer->semaphore_used = NULL;

    circular_buffer->semaphore_used =
        sem_open(SEMAPHORE_USED, O_CREAT | O_EXCL, 0600, MAX_DATA_COUNT);

    if (circular_buffer->semaphore_used == SEM_FAILED) {
      cb_close_master(circular_buffer);
      free(circular_buffer);

      return NULL;
    }

    circular_buffer->semaphore_write_to_buffer = sem_open(
        SEMAPHORE_WRITE_TO_BUFFER, O_CREAT | O_EXCL, 0600, MAX_DATA_COUNT);

    if (circular_buffer->semaphore_write_to_buffer == SEM_FAILED) {
      cb_close_master(circular_buffer);
      free(circular_buffer);

      return NULL;
    }
  } else {
    if (((circular_buffer->semaphore_write_to_buffer =
              sem_open(SEMAPHORE_WRITE_TO_BUFFER, 0)) == SEM_FAILED) ||
        ((circular_buffer->semaphore_used = sem_open(SEMAPHORE_USED, 0)) ==
         SEM_FAILED)) {

      if (circular_buffer->semaphore_write_to_buffer == SEM_FAILED) {
        sem_close(circular_buffer->semaphore_write_to_buffer);
      }

      if (circular_buffer->semaphore_used == SEM_FAILED) {
        sem_close(circular_buffer->semaphore_used);
      }

      free(circular_buffer);

      return NULL;
    }
  }

  return circular_buffer;
}

circular_buffer_t *cb_open_master() { return cb_open(true); }

circular_buffer_t *cb_open_slave() { return cb_open(false); }

static int cb_close(circular_buffer_t *circular_buffer, bool is_master) {
  if (is_master == true) {
    circular_buffer->shared_memory->in_shutdown = true;
  }

  sem_t *semaphores[3];
  int semaphores_to_close = 0;

  if (circular_buffer->semaphore_used != NULL) {
    semaphores[semaphores_to_close] = circular_buffer->semaphore_used;
    semaphores_to_close += 1;
  }

  if (circular_buffer->semaphore_write_to_buffer != NULL) {
    semaphores[semaphores_to_close] =
        circular_buffer->semaphore_write_to_buffer;
    semaphores_to_close += 1;
  }

  int status_code_from_closing_semaphores = 0;
  for (int i = 0; i < semaphores_to_close; i++) {
    if (sem_close(semaphores[i]) != 0) {
      status_code_from_closing_semaphores = -1;
    }
  }
  if (status_code_from_closing_semaphores != 0) {
    return -1;
  }

  if (is_master == true) {
    if ((sem_unlink(SEMAPHORE_USED) || sem_unlink(SEMAPHORE_WRITE_TO_BUFFER)) !=
        0) {
      return -1;
    }

    if (sm_close_master(circular_buffer->shared_memory, circular_buffer->fd) !=
        0) {
      return -1;
    }
  } else {
    if (sm_close_slave(circular_buffer->shared_memory, circular_buffer->fd) !=
        0) {
      return -1;
    }
  }

  return 0;
}

int cb_close_master(circular_buffer_t *circular_buffer) {
  return cb_close(circular_buffer, true);
}

int cb_close_slave(circular_buffer_t *circular_buffer) {
  return cb_close(circular_buffer, false);
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
