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

  int best_3coloring = -1;
  while (in_shutdown == false) {
    sem_wait(shared_memory.semaphore_buffer_mutex);

    while (shared_memory.buffer->count > 0) {
      edge_t *edges;
      int len = cbh_read_edges(shared_memory.buffer, &edges);

      if (len < 0) {
        in_shutdown = true;

        fprintf(stderr, "Error: when reading from cb\n");

        break;
      }

      if (len == 0) {
        in_shutdown = true;

        free(edges);

        printf("The graph is 3-colorable!\n");

        break;
      }

      if (best_3coloring == -1 || len < best_3coloring) {
        best_3coloring = len;

        printf("Solution with %d edges:", len);
        for (int i = 0; i < len; i += 1) {
          printf(" %d-%d", edges[i].node1, edges[i].node2);
        }
        printf("\n");

        free(edges);
      }
    }

    sem_post(shared_memory.semaphore_buffer_mutex);

    sleep(2);
  }

  printf("Shutdown ...\n");

  // TODO send signal

  sm_close(&shared_memory, true);

  return EXIT_SUCCESS;
}
