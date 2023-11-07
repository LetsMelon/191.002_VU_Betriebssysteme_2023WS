#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

static bool is_strict = false;

void ch_init_output(bool strict) { is_strict = strict; }

void ch_print_number(float number) {

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

void ch_print_complex_number(float complex number) {
  float c_r = crealf(number);
  float c_i = cimagf(number);

  ch_print_number(c_r);

  printf(" ");

  ch_print_number(c_i);
  printf("*i");
}
