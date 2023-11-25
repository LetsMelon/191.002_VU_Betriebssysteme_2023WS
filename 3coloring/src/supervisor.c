#include <assert.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "circular_buffer_helper.h"
#include "shared_memory.h"

static char *USAGE = "SYNOPSIS\n\tsupervisor [-n limit] [-w delay] [-p]\n";

shared_memory_t shared_memory;
volatile bool in_shutdown = false;

void handle_shutdown(int signal) {
  sem_post(shared_memory.semaphore_in_shutdown);

  in_shutdown = true;
}

typedef struct {
  int limit, delay;

  bool flag_p;
} arguments_t;

int main(void) {
  // ! The struct 'circular_buffer_t' must be smaller than 4096 bytes.
  assert(sizeof(circular_buffer_t) <= 4096);

  if (sm_open(&shared_memory, true) < 0) {
    return EXIT_FAILURE;
  };

  signal(SIGINT, handle_shutdown);

  sem_post(shared_memory.semaphore_buffer_mutex);

  while (!in_shutdown) {
    sem_wait(shared_memory.semaphore_buffer_mutex);

    printf("count: %llu\n", shared_memory.buffer->count);

    sem_post(shared_memory.semaphore_buffer_mutex);
    /* code */
    sleep(5);
  }

  printf("Shutdown ...\n");

  sm_close(&shared_memory, true);

  return EXIT_SUCCESS;
}
