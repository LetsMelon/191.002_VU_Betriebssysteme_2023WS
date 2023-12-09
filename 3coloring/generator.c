/**
 * @file generator.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Generates graph solutions and writes them to shared memory
 *
 * This program generates graph solutions based on the provided edge parameters.
 * It initializes and writes solutions to shared memory (circular buffer) for
 * the supervisor process to process and determine the best 3-coloring solution.
 *
 * Usage:
 * generator EDGE1...
 *
 * Example:
 * generator 0-1 0-2 0-3 1-2 1-3 2-3
 */

#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "circular_buffer.h"
#include "circular_buffer_helper.h"
#include "edge.h"
#include "matrix.h"
#include "parser.h"
#include "shared_memory.h"

/**
 * @brief Usage information for the generator program.
 */
static char *USAGE = "SYNOPSIS\n\tgenerator EDGE1...\nEXAMPLE\n\tgenerator 0-1 "
                     "0-2 0-3 1-2 1-3 2-3\n";

/**
 * @brief Main function for generating graph solutions.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments as an array of strings.
 * @return Exit status of the program.
 */
int main(int argc, char **argv) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  long seed = ts.tv_nsec;
  // Something something random
  seed = (seed & ((seed >> 8) ^ (seed >> 12))) + getpid();
  printf("Seed: %ld\n", seed);
  srandom(seed);

  shared_memory_t shared_memory;
  if (sm_open(&shared_memory, false) < 0) {
    fprintf(stderr, "%s: Error while trying to open shared memory.\n", argv[0]);
    return EXIT_FAILURE;
  }

  int input_len = 0;
  for (int i = 1; i < argc; i += 1) {
    input_len += strlen(argv[i]);
    if (i + 1 < argc) {
      input_len += 1;
    }
  }

  char *input_concat = (char *)malloc(sizeof(char) * (input_len + 1));
  if (input_concat == NULL) {
    sm_close(&shared_memory, false);

    fprintf(stderr, "%s: Error while allocating memory for the raw edges.\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  int already_copied = 0;
  for (int i = 1; i < argc; i += 1) {
    char *argv_i = argv[i];

    strcpy(input_concat + already_copied, argv_i);
    already_copied += strlen(argv_i);

    input_concat[already_copied] = ' ';
    already_copied += 1;
  }

  input_concat[input_len] = '\0';

  string_list_t edges;

  p_split_at(input_concat, ' ', &edges);

  free(input_concat);

  if (edges.num < 1) {
    sl_free(&edges);
    sm_close(&shared_memory, false);

    printf("%s", USAGE);

    fprintf(stderr, "%s: Graph must have at least one edge.\n", argv[0]);
    return EXIT_FAILURE;
  }

  edge_t *parsed_edges = (edge_t *)malloc(sizeof(edge_t) * edges.num);
  if (parsed_edges == NULL) {
    sl_free(&edges);
    sm_close(&shared_memory, false);

    fprintf(stderr, "%s: Error while allocating memory for the edges.\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  for (size_t i = 0; i < edges.num; i += 1) {
    edge_t edge;
    if (p_parse_as_edge(edges.values[i], &edge) != 0) {
      sl_free(&edges);
      free(parsed_edges);
      sm_close(&shared_memory, false);

      fprintf(stderr, "%s: Error while parsing edges.\n", argv[0]);
      return EXIT_FAILURE;
    }

    parsed_edges[i] = edge;
  }

  int n = edges.num;
  sl_free(&edges);

  edge_t *edges_to_remove = malloc(sizeof(edge_t) * n);
  if (edges_to_remove == NULL) {
    free(parsed_edges);

    sm_close(&shared_memory, false);

    fprintf(stderr,
            "%s: Error while allocating memory for the removed edges.\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  while (true) {
    graph_t graph;
    m_graph_init(&graph, parsed_edges, n);

    solution_t solution;
    m_graph_remove_same_edge_connections(&graph, &solution);
    m_graph_free(&graph);

    if (sem_trywait(shared_memory.semaphore_in_shutdown) == 0) {
      fprintf(stderr, "Received shutdown\n");
      sem_post(shared_memory.semaphore_in_shutdown);
      break;
    }

    if (sem_wait(shared_memory.semaphore_buffer_mutex) != 0) {
      free(edges_to_remove);
      free(parsed_edges);

      sm_close(&shared_memory, false);

      return EXIT_FAILURE;
    }

    fprintf(stderr, "Writing edges to shared memory: %d\n", solution.len);

    if (cbh_write_solution(&shared_memory, solution) != 0) {
      free(edges_to_remove);
      free(parsed_edges);

      sm_close(&shared_memory, false);

      return EXIT_FAILURE;
    }

    if (sem_post(shared_memory.semaphore_buffer_mutex) != 0) {
      free(edges_to_remove);
      free(parsed_edges);

      sm_close(&shared_memory, false);

      return EXIT_FAILURE;
    }
  }

  free(edges_to_remove);
  free(parsed_edges);

  sm_close(&shared_memory, false);

  return EXIT_SUCCESS;
}
