/**
 * @file circular_buffer_helper.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Header file defining helper functions for circular buffer operations.
 */

#ifndef _CBH
#define _CBH

#include <semaphore.h>
#include <stdint.h>

#include "circular_buffer.h"
#include "edge.h"
#include "shared_memory.h"

/**
 * @brief Reads a value from the shared memory. Waits with semaphores until
 * there is something to be read.
 * @param buffer Pointer to the shared memory.
 * @param solution Pointer to where th read value should be stored
 * @return Returns 0 on success, -1 if there is nothing to read and -2 when
 * encountering an error.
 */
int cbh_read_solution(shared_memory_t *shared_memory, solution_t *solution);

/**
 * @brief Writes a value to the shared memory. Waits with semaphores until
 * there is enough space to write to.
 * @param buffer Pointer to the shared memory.
 * @param solution Value to be written.
 * @return Returns 0 on success, -1 when encountering an error.
 */
int cbh_write_solution(shared_memory_t *shared_memory, solution_t solution);

#endif /* _CBH */
