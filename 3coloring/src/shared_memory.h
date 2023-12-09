/**
 * @file shared_memory.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Header file defining shared memory operations and structures.
 */

#ifndef _SM
#define _SM

#include <semaphore.h>
#include <stdbool.h>

#include "circular_buffer.h"

/**
 * @struct shared_memory_t
 * @brief Structure representing shared memory operations.
 */
typedef struct {
  circular_buffer_t
      *buffer; /**< Pointer to the circular buffer in shared memory */

  sem_t *semaphore_in_shutdown;  /**< Semaphore for signaling shutdown */
  sem_t *semaphore_buffer_mutex; /**< Semaphore for buffer access control */

  int fd; /**< File descriptor for shared memory */
} shared_memory_t;

/**
 * @brief Opens or creates shared memory.
 *
 * @param shared_memory Pointer to the shared memory structure to initialize.
 * @param is_master Flag indicating if the process is the master process or not.
 * @return Returns 0 on success, -1 on failure.
 */
int sm_open(shared_memory_t *shared_memory, bool is_master);

/**
 * @brief Closes shared memory.
 *
 * @param shared_memory Pointer to the shared memory structure to close.
 * @param is_master Flag indicating if the process is the master or not.
 * @return Returns 0 on success, -1 on failure.
 */
int sm_close(shared_memory_t *shared_memory, bool is_master);

#endif /* _SM */
