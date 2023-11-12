/**
 * @file complex_helper.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for printing float complex values.
 */

#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

/** If the output should be strict or not.
 * @brief By default the value is false (floating point precision = 6) but the
 * value can be changed via 'ch_init_output'.
 */
static bool is_strict = false;

void ch_init_output(bool strict) { is_strict = strict; }

static void ch_print_number(float number) {
  if (is_strict) {
    // this should remove all '-0.000' when printing to the terminal
    if (number > (0.0f - 0.001f) && number <= 0.0f) {
      number = fabsf(number);
    }
    printf("%.3f", number);
  } else {
    // this should remove all '-0.000' when printing to the terminal
    if (number > (0.0f - 0.000001f) && number <= 0.0f) {
      number = fabsf(number);
    }
    printf("%.6f", number);
  }
}

/**
 * @details this function does not write a new line to the stdout when been
 * called. number = 1+3i => "1 3*i"
 */
void ch_print_complex_number(float complex number) {
  float c_r = crealf(number);
  float c_i = cimagf(number);

  ch_print_number(c_r);

  printf(" ");

  ch_print_number(c_i);
  printf("*i");
}
