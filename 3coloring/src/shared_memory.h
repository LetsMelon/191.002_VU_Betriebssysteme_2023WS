#ifndef _SM
#define _SM

#include <semaphore.h>
#include <stdbool.h>

#include "circular_buffer.h"

typedef struct {
  circular_buffer_t *buffer;

  sem_t *semaphore_in_shutdown, *semaphore_buffer_mutex;
  int fd;
} shared_memory_t;

int sm_open(shared_memory_t *shared_memory, bool is_master);
int sm_close(shared_memory_t *shared_memory, bool is_master);

#endif
