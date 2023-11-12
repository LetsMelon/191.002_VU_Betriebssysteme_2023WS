/**
 * @file main.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Main program module.
 *
 * ForkFFT
 **/

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "child.h"
#include "complex_helper.h"
#include "complex_number_list.h"
#include "parser.h"

/*! \def PI
    \brief Returns an approximation of the value PI.

    This value is used in the fft calculation.
*/
#define PI 3.141592654f

const char *USAGE =
    "SYNOPSIS\n\tforkFFT [-p]"; /**< Usage message for this program */

/**
 * Program entry point.
 * @brief The program starts here.
 * @details The program starts here.
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS if no error occurred while running.
 */
int main(int argc, char **argv) {
  int opt;
  bool precise_flag = false;
  while ((opt = getopt(argc, argv, "p")) != -1) {
    switch (opt) {
    case 'p':
      precise_flag = true;
      break;
    default:
      printf("%s\n", USAGE);
      return EXIT_FAILURE;
    }
  }

  ch_init_output(precise_flag);

  complex_number_list_t complex_list;
  if (cnl_init(&complex_list) != 0) {
    return EXIT_FAILURE;
  }

  size_t len = 256;
  char *line = malloc(len * sizeof(char));
  if (line == NULL) {
    return -1;
  }

  while (getline(&line, &len, stdin) != -1) {
    string_list_t list;
    char *input = line;

    if (strlen(input) > 0 && input[0] == '\n') {
      free(line);

      return EXIT_FAILURE;
    }

    if (p_split_at(input, ' ', &list) != 0) {
      sl_free(&list);
      free(line);

      return EXIT_FAILURE;
    }

    float complex number = 0.0f;

    if (p_parse_as_complex_float(&list, &number) != 0) {
      sl_free(&list);
      free(line);

      return EXIT_FAILURE;
    }
    sl_free(&list);

    if (cnl_add(&complex_list, number) != 0) {
      free(line);
      cnl_free(&complex_list);

      return EXIT_FAILURE;
    }
  }
  free(line);

  int n = complex_list.num;

  if (n == 0) {
    cnl_free(&complex_list);

    return EXIT_FAILURE;
  }

  if (n == 1) {
    float complex n = cnl_get_at_index(&complex_list, 0);
    ch_print_complex_number(n);
    printf("\n");

    cnl_free(&complex_list);

    return EXIT_SUCCESS;
  }

  // https://stackoverflow.com/a/600306
  bool is_power_of_two = (n & (n - 1)) == 0;
  if (is_power_of_two == false) {
    cnl_free(&complex_list);

    return EXIT_FAILURE;
  }

  int parent_to_child_odd[2], child_odd_to_parent[2], parent_to_child_even[2],
      child_even_to_parent[2];

  pipe(parent_to_child_odd);
  pipe(child_odd_to_parent);
  pipe(parent_to_child_even);
  pipe(child_even_to_parent);

  pid_t p_odd = fork();
  if (p_odd < 0) {
    cnl_free(&complex_list);

    c_close_pipes(&parent_to_child_odd[0]);
    c_close_pipes(&child_odd_to_parent[0]);
    c_close_pipes(&parent_to_child_even[0]);
    c_close_pipes(&child_even_to_parent[0]);

    return EXIT_FAILURE;
  } else if (p_odd == 0) {
    c_close_pipes(&parent_to_child_even[0]);
    c_close_pipes(&child_even_to_parent[0]);

    c_create_child(&parent_to_child_odd[0], &child_odd_to_parent[0]);

    return EXIT_FAILURE;
  }

  pid_t p_even = fork();
  if (p_even < 0) {
    cnl_free(&complex_list);

    c_close_pipes(&parent_to_child_odd[0]);
    c_close_pipes(&child_odd_to_parent[0]);
    c_close_pipes(&parent_to_child_even[0]);
    c_close_pipes(&child_even_to_parent[0]);

    return EXIT_FAILURE;
  } else if (p_even == 0) {
    c_close_pipes(&parent_to_child_odd[0]);
    c_close_pipes(&child_odd_to_parent[0]);

    c_create_child(&parent_to_child_even[0], &child_even_to_parent[0]);

    return EXIT_FAILURE;
  }

  close(parent_to_child_odd[READ]);
  close(parent_to_child_even[READ]);

  close(child_odd_to_parent[WRITE]);
  close(child_even_to_parent[WRITE]);

  int fd_to_children[2] = {parent_to_child_even[WRITE],
                           parent_to_child_odd[WRITE]};
  for (int i = 0; i < n; i++) {
    int fd_to_child = fd_to_children[i % 2];

    float complex n = cnl_get_at_index(&complex_list, i);
    dprintf(fd_to_child, "%.6f %.6f*i\n", crealf(n), cimagf(n));
  }
  close(parent_to_child_odd[WRITE]);
  close(parent_to_child_even[WRITE]);

  cnl_free(&complex_list);

  int status_odd;
  waitpid(p_odd, &status_odd, 0);

  int status_even;
  waitpid(p_even, &status_even, 0);

#ifdef DDEBUG
  fprintf(stderr, "Fork status_odd: %d\n", WEXITSTATUS(status_odd));
  fprintf(stderr, "Fork status_even: %d\n", WEXITSTATUS(status_even));
#endif

  if ((WEXITSTATUS(status_odd) | WEXITSTATUS(status_even)) != EXIT_SUCCESS) {
    close(child_odd_to_parent[READ]);
    close(child_even_to_parent[READ]);

    return EXIT_FAILURE;
  }

  float complex *result_odd, *result_even, *result;
  result_odd = (float complex *)malloc((n / 2) * sizeof(float complex));
  if (result_odd == NULL) {
    close(child_odd_to_parent[READ]);
    close(child_even_to_parent[READ]);

    return EXIT_FAILURE;
  }
  result_even = (float complex *)malloc((n / 2) * sizeof(float complex));
  if (result_even == NULL) {
    close(child_odd_to_parent[READ]);
    close(child_even_to_parent[READ]);

    free(result_odd);

    return EXIT_FAILURE;
  }

  c_read_n_complex_numbers_from_fd(child_odd_to_parent[READ], &result_odd[0],
                                   n / 2);
  close(child_odd_to_parent[READ]);

  c_read_n_complex_numbers_from_fd(child_even_to_parent[READ], &result_even[0],
                                   n / 2);
  close(child_even_to_parent[READ]);

  result = (float complex *)malloc(n * sizeof(float complex));
  if (result == NULL) {
    free(result_odd);
    free(result_even);

    return EXIT_FAILURE;
  }

  for (int k = 0; k < n / 2; k++) {
    float complex re = result_even[k];
    float complex ro = result_odd[k];

    float complex middle_term = cosf((-(2.0 * PI) / (float)n) * (float)k) +
                                I * sinf((-(2.0 * PI) / (float)n) * (float)k);

    result[k] = re + middle_term * ro;
    result[k + n / 2] = re - middle_term * ro;
  }

  free(result_odd);
  free(result_even);

#ifdef DDEBUG
  fprintf(stderr, "n = %d\n", n);
#endif

  for (int i = 0; i < n; ++i) {
    ch_print_complex_number(result[i]);
    printf("\n");
  }

  free(result);

  return EXIT_SUCCESS;
}
