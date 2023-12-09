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

/*! \def BUFFER_SIZE
    \brief The struct 'circular_buffer_t' should have a size of roughly 2KB
*/

#define BUFFER_SIZE                                                            \
  (int)((2048 - 2 * sizeof(uint64_t) - sizeof(int)) / sizeof(int))

typedef struct {
  int data[BUFFER_SIZE]; /**< Array to store buffer data */
  uint64_t head;         /**< Index for writing data */
  uint64_t tail;         /**< Index for reading data */
  int count;             /**< Number of elements in the buffer */
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
 * @return Returns 0 if the write operation is successful, -1 if an error
 * occurred.
 */
int cb_write(circular_buffer_t *buffer, int value);

/**
 * @brief Reads a value from the circular buffer.
 * @param buffer Pointer to the circular buffer.
 * @return Returns the read value from the buffer, or -1 if an error occurred.
 */
int cb_read(circular_buffer_t *buffer);

#endif
