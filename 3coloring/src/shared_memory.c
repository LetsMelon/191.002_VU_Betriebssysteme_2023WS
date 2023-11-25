#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "shared_memory.h"

#define SEM_SHUTDOWN "/12220857_sh"
#define SEM_MUTEX "/12220857_mut"

#define SHM_BUFFER "/12220857_buffer"

int sm_open(shared_memory_t *shared_memory, bool is_master) {
  // ! If a crash happened
  if (is_master == true) {
    sem_unlink(SEM_SHUTDOWN);
    sem_unlink(SEM_MUTEX);
  }

  if (is_master == true) {
    shared_memory->semaphore_in_shutdown =
        sem_open(SEM_SHUTDOWN, O_CREAT | O_EXCL, 0660, 0);
  } else {
    shared_memory->semaphore_in_shutdown = sem_open(SEM_SHUTDOWN, 0);
  }
  if (shared_memory->semaphore_in_shutdown == SEM_FAILED) {
    fprintf(stderr, "Error: Semaphore 'shutdown' failed.\n");
    return -1;
  }

  if (is_master == true) {
    shared_memory->semaphore_buffer_mutex =
        sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0660, 1);
  } else {
    shared_memory->semaphore_buffer_mutex = sem_open(SEM_SHUTDOWN, 0);
  }
  if (shared_memory->semaphore_buffer_mutex == SEM_FAILED) {
    fprintf(stderr, "Error: Semaphore 'buffer_mutex' failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    return -1;
  }

  if (is_master == true) {
    shared_memory->fd = shm_open(SHM_BUFFER, O_CREAT | O_RDWR, 0660);
  } else {
    shared_memory->fd = shm_open(SHM_BUFFER, O_RDWR, 0660);
  }
  if (shared_memory->fd == -1) {
    fprintf(stderr, "Error: Opening shared memory failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    sem_close(shared_memory->semaphore_buffer_mutex);
    sem_unlink(SEM_MUTEX);

    return -1;
  }

  if (is_master == true) {
    if (ftruncate(shared_memory->fd, sizeof(circular_buffer_t)) == -1) {
      fprintf(stderr, "Error: ftruncate on shared memory failed.\n");

      sem_close(shared_memory->semaphore_in_shutdown);
      sem_unlink(SEM_SHUTDOWN);

      sem_close(shared_memory->semaphore_buffer_mutex);
      sem_unlink(SEM_MUTEX);

      close(shared_memory->fd);
      shm_unlink(SHM_BUFFER);

      return -1;
    }
  }

  shared_memory->buffer =
      mmap(NULL, sizeof(circular_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED,
           shared_memory->fd, 0);
  if (shared_memory->buffer == MAP_FAILED) {
    fprintf(stderr, "Error: mmap on shared memory failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    sem_close(shared_memory->semaphore_buffer_mutex);
    sem_unlink(SEM_MUTEX);

    close(shared_memory->fd);
    shm_unlink(SHM_BUFFER);

    return -1;
  }

  if (is_master == true) {
    if (cb_init(shared_memory->buffer) < 0) {
      sem_close(shared_memory->semaphore_in_shutdown);
      sem_close(shared_memory->semaphore_buffer_mutex);

      munmap(shared_memory->buffer, sizeof(circular_buffer_t));

      close(shared_memory->fd);

      sem_unlink(SEM_SHUTDOWN);
      sem_unlink(SEM_MUTEX);

      shm_unlink(SHM_BUFFER);

      return -1;
    }
  }

  return 0;
}

int sm_close(shared_memory_t *shared_memory, bool is_master) {
  int error = 0;

  error |= sem_close(shared_memory->semaphore_in_shutdown);
  error |= sem_close(shared_memory->semaphore_buffer_mutex);

  error |= munmap(shared_memory->buffer, sizeof(circular_buffer_t));

  error |= close(shared_memory->fd);

  if (is_master == true) {
    error |= sem_unlink(SEM_SHUTDOWN);
    error |= sem_unlink(SEM_MUTEX);

    error |= shm_unlink(SHM_BUFFER);
  }

  cb_free(shared_memory->buffer);

  if (error != 0) {
    return -1;
  }

  return 0;
}
