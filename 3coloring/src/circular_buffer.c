/**
 * @file circular_buffer.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
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
  buffer->count = 0;

  return 0;
}

void cb_free(circular_buffer_t *buffer) {
  // Implementation for freeing the circular buffer can be added here if needed
}

bool cb_is_full(circular_buffer_t *buffer) {
  return BUFFER_SIZE == buffer->count;
}

bool cb_is_empty(circular_buffer_t *buffer) { return buffer->count == 0; }

int cb_write(circular_buffer_t *buffer, int value) {
  if (cb_is_full(buffer) == true) {
    return -1;
  }

  buffer->data[buffer->head] = value;
  buffer->head = (buffer->head + 1) % BUFFER_SIZE;
  buffer->count += 1;

  return 0;
}

int cb_read(circular_buffer_t *buffer) {
  if (cb_is_empty(buffer) == true) {
    return -1;
  }

  int value = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
  buffer->count -= 1;

  return value;
}
