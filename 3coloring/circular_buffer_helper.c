/**
 * @file circular_buffer_helper.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Implementation file for helper functions related to circular buffer
 * operations.
 */

#include <semaphore.h>

#include "circular_buffer.h"
#include "shared_memory.h"

int cbh_read_solution(shared_memory_t *shared_memory, solution_t *solution) {
  if (sem_wait(shared_memory->semaphore_buffer_used_space) != 0) {
    return -1;
  }

  *solution = cb_read(shared_memory->buffer);

  if (sem_post(shared_memory->semaphore_buffer_free_space) != 0) {
    return -2;
  }

  return 0;
}

int cbh_write_solution(shared_memory_t *shared_memory, solution_t solution) {
  if (sem_wait(shared_memory->semaphore_buffer_free_space) != 0) {
    return -1;
  }

  cb_write(shared_memory->buffer, solution);

  if (sem_post(shared_memory->semaphore_buffer_used_space) != 0) {
    return -1;
  }

  return 0;
}
