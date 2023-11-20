#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "circular_buffer.h"
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
    return NULL;
  }

  circular_buffer_t *circular_buffer =
      (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
  if (circular_buffer == NULL) {
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
    circular_buffer->semaphore_free = NULL;
    circular_buffer->semaphore_mutex = NULL;
    circular_buffer->semaphore_used = NULL;

    circular_buffer->semaphore_free =
        sem_open(SEMAPHORE_FREE, O_CREAT | O_EXCL, 0600, MAX_DATA_COUNT);

    if (circular_buffer->semaphore_free == SEM_FAILED) {
      cb_close_master(circular_buffer);
      free(circular_buffer);

      return NULL;
    }

    circular_buffer->semaphore_used =
        sem_open(SEMAPHORE_USED, O_CREAT | O_EXCL, 0600, MAX_DATA_COUNT);

    if (circular_buffer->semaphore_used == SEM_FAILED) {
      cb_close_master(circular_buffer);
      free(circular_buffer);

      return NULL;
    }

    circular_buffer->semaphore_mutex =
        sem_open(SEMAPHORE_MUTEX, O_CREAT | O_EXCL, 0600, MAX_DATA_COUNT);

    if (circular_buffer->semaphore_mutex == SEM_FAILED) {
      cb_close_master(circular_buffer);
      free(circular_buffer);

      return NULL;
    }
  } else {
    if (((circular_buffer->semaphore_free = sem_open(SEMAPHORE_FREE, 0)) ==
         SEM_FAILED) ||
        ((circular_buffer->semaphore_mutex = sem_open(SEMAPHORE_MUTEX, 0)) ==
         SEM_FAILED) ||
        ((circular_buffer->semaphore_used = sem_open(SEMAPHORE_USED, 0)) ==
         SEM_FAILED)) {
      if (circular_buffer->semaphore_free == SEM_FAILED) {
        sem_close(circular_buffer->semaphore_free);
      }

      if (circular_buffer->semaphore_mutex == SEM_FAILED) {
        sem_close(circular_buffer->semaphore_mutex);
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
    sem_post(circular_buffer->semaphore_free);
    sem_post(circular_buffer->semaphore_free);

    circular_buffer->shared_memory->in_shutdown = true;
  }

  sem_t *semaphores[3];
  int semaphores_to_close = 0;

  if (circular_buffer->semaphore_free != NULL) {
    semaphores[semaphores_to_close] = circular_buffer->semaphore_free;
    semaphores_to_close += 1;
  }

  if (circular_buffer->semaphore_used != NULL) {
    semaphores[semaphores_to_close] = circular_buffer->semaphore_used;
    semaphores_to_close += 1;
  }

  if (circular_buffer->semaphore_mutex != NULL) {
    semaphores[semaphores_to_close] = circular_buffer->semaphore_mutex;
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
    if ((sem_unlink(SEMAPHORE_FREE) || sem_unlink(SEMAPHORE_USED) ||
         sem_unlink(SEMAPHORE_MUTEX)) != 0) {
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
