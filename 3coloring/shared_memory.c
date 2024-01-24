/**
 * @file shared_memory.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Contains functions for managing shared memory and semaphores.
 */

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>

#include "circular_buffer.h"
#include "shared_memory.h"

/*! \def SEM_SHUTDOWN
    \brief Name for the shutdown semaphore
*/

/*! \def SEM_MUTEX
    \brief Name for the buffer mutex semaphore
*/

/*! \def SEM_BUFFER_FREE_SPACE
    \brief Name for the buffer free space semaphore
*/

/*! \def SEM_BUFFER_USED_SPACE
    \brief Name for the buffer used space semaphore
*/

#define SEM_SHUTDOWN "/12220857_sh"
#define SEM_MUTEX "/12220857_mut"
#define SEM_BUFFER_FREE_SPACE "/12220857_bfs"
#define SEM_BUFFER_USED_SPACE "/12220857_bus"

/*! \def SHM_BUFFER
    \brief Name for the shared memory
*/

#define SHM_BUFFER "/12220857_buffer"

int sm_open(shared_memory_t *shared_memory, bool is_master) {
  // ! Comment out if a crash happened
  if (is_master == true) {
    sem_unlink(SEM_SHUTDOWN);
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_BUFFER_FREE_SPACE);
    sem_unlink(SEM_BUFFER_USED_SPACE);
  }

  // Initialize or open semaphores

  shared_memory->semaphore_in_shutdown =
      (is_master == true) ? sem_open(SEM_SHUTDOWN, O_CREAT | O_EXCL, 0660, 0)
                          : sem_open(SEM_SHUTDOWN, 0);
  if (shared_memory->semaphore_in_shutdown == SEM_FAILED) {
    fprintf(stderr, "Error: Semaphore 'shutdown' failed.\n");
    return -1;
  }

  shared_memory->semaphore_buffer_mutex =
      (is_master == true) ? sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0660, 0)
                          : sem_open(SEM_MUTEX, O_RDWR);
  if (shared_memory->semaphore_buffer_mutex == SEM_FAILED) {
    fprintf(stderr, "Error: Semaphore 'buffer_mutex' failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    return -1;
  }

  shared_memory->semaphore_buffer_used_space =
      (is_master == true)
          ? sem_open(SEM_BUFFER_USED_SPACE, O_CREAT | O_EXCL, 0660, 0)
          : sem_open(SEM_BUFFER_USED_SPACE, O_RDWR);
  if (shared_memory->semaphore_buffer_used_space == SEM_FAILED) {
    fprintf(stderr, "Error: Semaphore 'semaphore_buffer_used_space' failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    sem_close(shared_memory->semaphore_buffer_mutex);
    sem_unlink(SEM_MUTEX);

    return -1;
  }

  shared_memory->semaphore_buffer_free_space =
      (is_master == true)
          ? sem_open(SEM_BUFFER_FREE_SPACE, O_CREAT | O_EXCL, 0660, BUFFER_SIZE)
          : sem_open(SEM_BUFFER_FREE_SPACE, O_RDWR);
  if (shared_memory->semaphore_buffer_free_space == SEM_FAILED) {
    fprintf(stderr, "Error: Semaphore 'semaphore_buffer_free_space' failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    sem_close(shared_memory->semaphore_buffer_mutex);
    sem_unlink(SEM_MUTEX);

    sem_close(shared_memory->semaphore_buffer_used_space);
    sem_unlink(SEM_BUFFER_USED_SPACE);

    return -1;
  }

  // Create or open shared memory file descriptor
  shared_memory->fd = (is_master == true)
                          ? shm_open(SHM_BUFFER, O_CREAT | O_RDWR, 0660)
                          : shm_open(SHM_BUFFER, O_RDWR, 0660);
  if (shared_memory->fd == -1) {
    fprintf(stderr, "Error: Opening shared memory failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    sem_close(shared_memory->semaphore_buffer_mutex);
    sem_unlink(SEM_MUTEX);

    sem_close(shared_memory->semaphore_buffer_used_space);
    sem_unlink(SEM_BUFFER_USED_SPACE);

    sem_close(shared_memory->semaphore_buffer_free_space);
    sem_unlink(SEM_BUFFER_FREE_SPACE);

    return -1;
  }

  if (is_master == true) {
    if (ftruncate(shared_memory->fd, sizeof(circular_buffer_t)) == -1) {
      fprintf(stderr, "Error: ftruncate on shared memory failed.\n");

      sem_close(shared_memory->semaphore_in_shutdown);
      sem_unlink(SEM_SHUTDOWN);

      sem_close(shared_memory->semaphore_buffer_mutex);
      sem_unlink(SEM_MUTEX);

      sem_close(shared_memory->semaphore_buffer_used_space);
      sem_unlink(SEM_BUFFER_USED_SPACE);

      sem_close(shared_memory->semaphore_buffer_free_space);
      sem_unlink(SEM_BUFFER_FREE_SPACE);

      close(shared_memory->fd);
      shm_unlink(SHM_BUFFER);

      return -1;
    }
  }

  // Map shared memory to buffer
  shared_memory->buffer =
      mmap(NULL, sizeof(circular_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED,
           shared_memory->fd, 0);
  if (shared_memory->buffer == MAP_FAILED) {
    fprintf(stderr, "Error: mmap on shared memory failed.\n");

    sem_close(shared_memory->semaphore_in_shutdown);
    sem_unlink(SEM_SHUTDOWN);

    sem_close(shared_memory->semaphore_buffer_mutex);
    sem_unlink(SEM_MUTEX);

    sem_close(shared_memory->semaphore_buffer_used_space);
    sem_unlink(SEM_BUFFER_USED_SPACE);

    sem_close(shared_memory->semaphore_buffer_free_space);
    sem_unlink(SEM_BUFFER_FREE_SPACE);

    close(shared_memory->fd);
    shm_unlink(SHM_BUFFER);

    return -1;
  }

  // Initialize the circular buffer if the process is the master
  if (is_master == true) {
    if (cb_init(shared_memory->buffer) < 0) {
      sem_close(shared_memory->semaphore_in_shutdown);
      sem_unlink(SEM_SHUTDOWN);

      sem_close(shared_memory->semaphore_buffer_mutex);
      sem_unlink(SEM_MUTEX);

      sem_close(shared_memory->semaphore_buffer_used_space);
      sem_unlink(SEM_BUFFER_USED_SPACE);

      sem_close(shared_memory->semaphore_buffer_free_space);
      sem_unlink(SEM_BUFFER_FREE_SPACE);

      munmap(shared_memory->buffer, sizeof(circular_buffer_t));
      close(shared_memory->fd);
      shm_unlink(SHM_BUFFER);

      return -1;
    }
  }

  return 0;
}

int sm_close(shared_memory_t *shared_memory, bool is_master) {
  int error = 0;

  // Close semaphores and unmap shared memory
  error |= sem_close(shared_memory->semaphore_in_shutdown);
  error |= sem_close(shared_memory->semaphore_buffer_mutex);
  error |= sem_close(shared_memory->semaphore_buffer_free_space);
  error |= sem_close(shared_memory->semaphore_buffer_used_space);

  error |= munmap(shared_memory->buffer, sizeof(circular_buffer_t));
  error |= close(shared_memory->fd);

  // Unlink named semaphores and shared memory if the process is the master
  if (is_master == true) {
    error |= sem_unlink(SEM_SHUTDOWN);
    error |= sem_unlink(SEM_MUTEX);
    error |= sem_unlink(SEM_BUFFER_USED_SPACE);
    error |= sem_unlink(SEM_BUFFER_FREE_SPACE);

    error |= shm_unlink(SHM_BUFFER);
  }

  // Free circular buffer
  cb_free(shared_memory->buffer);

  // Check for errors
  return error == 0 ? 0 : -1;
}
