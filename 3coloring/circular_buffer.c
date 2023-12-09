/**
 * @file circular_buffer.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Implementation file for circular buffer functions.
 */

#include <stdbool.h>
#include <stdint.h>

#include "circular_buffer.h"
#include "edge.h"

int cb_init(circular_buffer_t *buffer) {
  buffer->head = 0;
  buffer->tail = 0;

  return 0;
}

void cb_free(circular_buffer_t *buffer) {
  // Implementation for freeing the circular buffer can be added here if needed
}

void cb_write(circular_buffer_t *buffer, solution_t solution) {
  buffer->data[buffer->head] = solution;
  buffer->head = (buffer->head + 1) % BUFFER_SIZE;
}

solution_t cb_read(circular_buffer_t *buffer) {
  solution_t solution = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;

  return solution;
}
