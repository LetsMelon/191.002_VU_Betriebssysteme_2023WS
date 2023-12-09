/**
 * @file supervisor.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Supervisor process managing shared memory for solutions
 *
 * The supervisor process oversees the shared memory containing solutions
 * generated by other processes (generators). It initializes, manages, and
 * retrieves solutions from shared memory, maintaining synchronization and
 * handling shutdown signals.
 *
 * Usage:
 * supervisor [-n limit] [-w delay] [-p]
 *
 * Options:
 *   -n limit    Maximum number of solutions to process
 *   -w delay    Delay in seconds before starting to process solutions
 *   -p          Flag for visualization (true/false)
 */

#include <assert.h>
#include <limits.h>
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

/**
 * @brief Usage information for the supervisor program.
 */
static char *USAGE = "SYNOPSIS\n\tsupervisor [-n limit] [-w delay] [-p]\n";

shared_memory_t shared_memory;     // Shared memory structure
volatile bool in_shutdown = false; // Signal for shutdown

/**
 * @brief Signal handler function for handling SIGINT.
 *
 * @param signal The signal received by the handler.
 * @details global variables: in_shutdown
 */
void handle_shutdown(int signal) {
  sem_post(shared_memory.semaphore_in_shutdown);

  in_shutdown = true;
}

/**
 * @struct arguments_t
 * @brief Structure to hold command-line arguments.
 */
typedef struct {
  int limit;   /**< Integer value for limit how many solutions should be
                  considered for evaluating the graph */
  int delay;   /**< Integer value for delay before receiving any solutions */
  bool flag_p; /**< Flag indicating 'p' presence in arguments. */
} arguments_t;

/**
 * @brief Initialize argument structure with default values and read values from
 * argv.
 *
 * @param arguments Pointer to the arguments_t structure to be initialized.
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments as an array of strings.
 * @return Returns 0 on success, -1 on encountering an error.
 */
int arguments_init(arguments_t *arguments, int argc, char **argv) {
  // ? Ignore
  arguments->flag_p = false;

  arguments->limit = -1;
  arguments->delay = 0;

  int opt, encountered_flag_p = 0, encountered_limit = 0, encountered_delay = 0;

  while ((opt = getopt(argc, argv, "n:w:p")) != -1) {
    switch (opt) {
    case 'n':
      if (encountered_limit > 0) {
        return -1;
      }

      if (p_parse_as_int(optarg, &arguments->limit) < 0) {
        return -1;
      }

      encountered_limit += 1;

      break;
    case 'w':
      if (encountered_delay > 0) {
        return -1;
      }

      if (p_parse_as_int(optarg, &arguments->delay) < 0) {
        return -1;
      }

      encountered_delay += 1;

      break;
    case 'p':
      if (encountered_flag_p > 0) {
        return -1;
      }

      arguments->flag_p = true;

      encountered_flag_p += 1;

      break;
    default:
      fprintf(stderr, "Unknown arguments: '%d'", opt);
      return -1;
    }
  }

  return 0;
}

/**
 * @brief Main function for gathering solutions from the generators.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments as an array of strings.
 * @return Exit status of the program.
 */
int main(int argc, char **argv) {
  // ! The struct 'circular_buffer_t' must be smaller than 4096 bytes.
  assert(sizeof(circular_buffer_t) <= 4096);
#ifdef DDEBUG
  fprintf(stderr, "sizeof(circular_buffer_t) = %d, BUFFER_SIZE = %d\n",
          (int)sizeof(circular_buffer_t), BUFFER_SIZE);
#endif

  if (sm_open(&shared_memory, true) < 0) {
    fprintf(stderr, "%s: Error while opening shared memory.\n", argv[0]);
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
  fprintf(stderr, "limit = %d, delay = %d, visualization = %s\n",
          arguments.limit, arguments.delay,
          arguments.flag_p == true ? "true" : "false");
#endif

  sem_post(shared_memory.semaphore_buffer_mutex);

  sleep(arguments.delay);

  int solutions_encountered = 0;
  int best_3coloring = INT_MAX;
  while (in_shutdown == false) {
    solution_t solution;
    int read_solution_error = cbh_read_solution(&shared_memory, &solution);

    if (read_solution_error == -1) {
      continue;
    } else if (read_solution_error == -2) {
      fprintf(stderr, "%s: Error while reading from shared memory.\n", argv[0]);

      sem_post(shared_memory.semaphore_in_shutdown);
      sm_close(&shared_memory, true);

      return EXIT_FAILURE;
    } else {
      solutions_encountered += 1;
      int len = solution.len;

      if (len == 0) {
        in_shutdown = true;
        best_3coloring = 0;

        break;
      }

      if (len < best_3coloring) {
        best_3coloring = len;

        fprintf(stderr, "Solution with %d edges:", len);
        for (int i = 0; i < len; i += 1) {
          fprintf(stderr, " %d-%d", solution.edges[i].node1,
                  solution.edges[i].node2);
        }
        fprintf(stderr, "\n");
      }

      if (arguments.limit != -1 && solutions_encountered >= arguments.limit &&
          in_shutdown == false) {
        break;
      }
    }

    // sleep for 50ms, so that other generators can have the opportunity to
    // lock the mutex
    // usleep(50);
  }

  switch (best_3coloring) {
  case INT_MAX:
    fprintf(stderr,
            "%s: Supervisor wasn't able to read a single value from a "
            "generator\n",
            argv[0]);

    sem_post(shared_memory.semaphore_in_shutdown);
    sm_close(&shared_memory, true);

    return EXIT_FAILURE;

  case 0:
    printf("The graph is 3-colorable!\n");
    break;

  default:
    printf("The graph might not be 3-colorable, best solution removes %d "
           "edges.\n",
           best_3coloring);
    break;
  }

#ifdef DDEBUG
  fprintf(stderr, "Shutdown ...\n");
#endif

  sem_post(shared_memory.semaphore_in_shutdown);

  sm_close(&shared_memory, true);

  return EXIT_SUCCESS;
}