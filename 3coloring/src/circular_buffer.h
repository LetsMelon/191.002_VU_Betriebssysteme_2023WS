#ifndef _CB
#define _CB

#include <semaphore.h>

#include "shared_memory.h"

#define SEMAPHORE_FREE "12220857_free"
#define SEMAPHORE_USED "12220857_used"
#define SEMAPHORE_MUTEX "12220857_mutex"

typedef struct {
  shared_memory_t *shared_memory;

  sem_t *semaphore_free, *semaphore_used, *semaphore_mutex;

  int fd;
} circular_buffer_t;

circular_buffer_t *cb_open_master();
circular_buffer_t *cb_open_slave();

int cb_close_master(circular_buffer_t *circular_buffer);
int cb_close_slave(circular_buffer_t *circular_buffer);

#endif