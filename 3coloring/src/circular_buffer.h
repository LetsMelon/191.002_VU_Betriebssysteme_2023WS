/**
 * @file circular_buffer.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Header file defining a circular buffer structure and its functions.
 */

#ifndef _CB
#define _CB

#include <stdint.h>

#include "edge.h"

typedef struct {
  int len;                 /**< Count of deleted edges */
  edge_t edges[MAX_EDGES]; /**< Array of deleted edges */
} solution_t;

/*! \def BUFFER_SIZE
    \brief The struct 'circular_buffer_t' should have a size of roughly 2KB
*/

#define BUFFER_SIZE                                                            \
  (int)(((1024 * 4) - 2 * sizeof(uint64_t)) / sizeof(solution_t))

typedef struct {
  solution_t data[BUFFER_SIZE]; /**< Array to store buffer data */
  uint64_t head;                /**< Index for writing data */
  uint64_t tail;                /**< Index for reading data */
} circular_buffer_t;

/**
 * @brief Initializes the circular buffer.
 * @param buffer Pointer to the circular buffer to be initialized.
 * @return Returns 0 upon successful initialization.
 */
int cb_init(circular_buffer_t *buffer);

/**
 * @brief Frees the memory allocated for the circular buffer.
 * @param buffer Pointer to the circular buffer to be freed.
 */
void cb_free(circular_buffer_t *buffer);

/**
 * @brief Writes a value into the circular buffer.
 * @param buffer Pointer to the circular buffer.
 * @param value Value to be written into the buffer.
 */
void cb_write(circular_buffer_t *buffer, solution_t solution);

/**
 * @brief Reads a value from the circular buffer.
 * @param buffer Pointer to the circular buffer.
 * @return Returns the read value from the buffer.
 */
solution_t cb_read(circular_buffer_t *buffer);

#endif /* _CB */
