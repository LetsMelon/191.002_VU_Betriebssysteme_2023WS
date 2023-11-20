#ifndef _SM
#define _SM

#include <stdbool.h>

#define MAX_DATA_COUNT (4096 - sizeof(bool) - 2 * sizeof(int)) / sizeof(int)

typedef struct {
  bool in_shutdown;

  int write_index, read_index;

  int data[MAX_DATA_COUNT];
} shared_memory_t;
// TODO maybe a comp-time check to ensure that sizeof(shared_memory_t) <= 4096

shared_memory_t *sm_open_slave(int *fd);
shared_memory_t *sm_open_master(int *fd);

int sm_close_slave(shared_memory_t *shared_memory, int fd);
int sm_close_master(shared_memory_t *shared_memory, int fd);

#endif
