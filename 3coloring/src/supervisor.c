#include <assert.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "circular_buffer_helper.h"
#include "parser.h"
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

int arguments_init(arguments_t *arguments, int argc, char **argv) {
  // ? Ignore
  arguments->flag_p = false;

  arguments->limit = -1;
  arguments->delay = 0;

  int opt;

  while ((opt = getopt(argc, argv, "n:w:p")) != -1) {
    switch (opt) {
    case 'n':
      if (p_parse_as_int(optarg, &arguments->limit) < 0) {
        return -1;
      }

      break;
    case 'w':
      if (p_parse_as_int(optarg, &arguments->delay) < 0) {
        return -1;
      }

      break;
    case 'p':
      arguments->flag_p = true;
      break;
    default:
      fprintf(stderr, "Unknown arguments: '%d'", opt);
      return -1;
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  // ! The struct 'circular_buffer_t' must be smaller than 4096 bytes.
  assert(sizeof(circular_buffer_t) <= 4096);

  if (sm_open(&shared_memory, true) < 0) {
    return EXIT_FAILURE;
  };

  signal(SIGINT, handle_shutdown);

  arguments_t arguments;
  if (arguments_init(&arguments, argc, argv) < 0) {
    printf("%s", USAGE);

    sem_post(shared_memory.semaphore_in_shutdown);
    sm_close(&shared_memory, true);

    return EXIT_FAILURE;
  }

#ifdef DDEBUG
  printf("args: limt = %d, delay = %d, flag = %d\n", arguments.limit,
         arguments.delay, (int)arguments.flag_p);
#endif

  sem_post(shared_memory.semaphore_buffer_mutex);

  sleep(arguments.delay);

  int solutions_encountered = 0;
  int best_3coloring = -1;
  while (in_shutdown == false) {
    sem_wait(shared_memory.semaphore_buffer_mutex);

    while (shared_memory.buffer->count > 0) {
      edge_t *edges;
      // printf("DEBUG: new solution.\n");

      int len = cbh_read_edges(shared_memory.buffer, &edges);
      // printf("DEBUG: have it, len = %d\n", len);

      if (len < 0) {
        in_shutdown = true;

        fprintf(stderr, "Error: when reading from cb\n");

        break;
      }

      if (len == 0) {
        in_shutdown = true;

        printf("The graph is 3-colorable!\n");

        break;
      }

      if (best_3coloring == -1 || len < best_3coloring) {
        // printf("DEBUG: is better\n");

        best_3coloring = len;

        fprintf(stderr, "Solution with %d edges:", len);
        for (int i = 0; i < len; i += 1) {
          printf(" %d-%d", edges[i].node1, edges[i].node2);
        }
        printf("\n");
      }

      // printf("DEBUG: next solution.\n");

      free(edges);
    }

    sem_post(shared_memory.semaphore_buffer_mutex);

    solutions_encountered += 1;

    if (arguments.limit != -1 && solutions_encountered >= arguments.limit) {
      printf("The graph might not be 3-colorable, best solution removes %d "
             "edges.\n",
             best_3coloring);

      break;
    }

    // sleep for 100ms, so that other generators can have the opportunity to
    // lock the mutex
    usleep(100);
  }

  // printf("Shutdown ...\n");

  sem_post(shared_memory.semaphore_in_shutdown);

  sm_close(&shared_memory, true);

  return EXIT_SUCCESS;
}
