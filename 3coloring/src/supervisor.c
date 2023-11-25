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

  // printf("Hello from the supervisor!\n");
  //
  // circular_buffer_t *cb = cb_open_master();
  //
  // if (cb == NULL) {
  //   printf("Error in creating circular buffer!\n");
  //   return EXIT_FAILURE;
  // }
  //
  // if (cb_close_master(cb) != 0) {
  //   return EXIT_FAILURE;
  // }

  // circular_buffer_t buffer;
  // cb_init(&buffer);
  //
  // printf("size = %llu\n", BUFFER_SIZE);
  //
  // printf("value = %d\n", cb_read(&buffer));
  //
  // {
  //   int len = 2;
  //   edge_t *edges = (edge_t *)malloc(sizeof(edge_t) * len);
  //   edges[0].node1 = 1;
  //   edges[0].node2 = 2;
  //   edges[1].node1 = 2;
  //   edges[1].node2 = 3;
  //   cbh_write_edges(&buffer, edges, len);
  //   free(edges);
  // }
  //
  // edge_t *edges;
  // int len = cbh_read_edges(&buffer, &edges);
  // for (int i = 0; i < len; i += 1) {
  //   printf("%d->%d\n", edges[i].node1, edges[i].node2);
  // }
  // free(edges);
  //
  // cb_free(&buffer);

  if (sm_open(&shared_memory, true) < 0) {
    return EXIT_FAILURE;
  };

  signal(SIGINT, handle_shutdown);

  while (!in_shutdown) {
    /* code */
    sleep(1);
  }

  printf("Shutdown ...\n");

  sm_close(&shared_memory, true);

  return EXIT_SUCCESS;
}
