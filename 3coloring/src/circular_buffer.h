#ifndef _CB
#define _CB

#include <stdint.h>

#include "edge.h"

// The struct 'circular_buffer_t' should have a size of roughly 2KB
#define BUFFER_SIZE (uint64_t)((2048 - 3 * sizeof(int)) / sizeof(int))

typedef struct {
  int data[BUFFER_SIZE];
  uint64_t head;  // Index for writing data
  uint64_t tail;  // Index for reading data
  uint64_t count; // Number of elements in the buffer
} circular_buffer_t;

int cb_init(circular_buffer_t *buffer);
void cb_free(circular_buffer_t *buffer);

int cb_write(circular_buffer_t *buffer, int value);
int cb_read(circular_buffer_t *buffer);

#endif
