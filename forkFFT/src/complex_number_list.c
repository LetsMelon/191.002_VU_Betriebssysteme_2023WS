/**
 * @file complex_number_list.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for printing float complex values.
 */

#include "math.h"
#include <complex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "complex_helper.h"
#include "complex_number_list.h"

/*!
 * The initial default capacity from an complex_number_list_t.
 */
const int LIST_DEFAULT_SIZE = 16;

int cnl_init(complex_number_list_t *list) {
  list->values =
      (float complex *)malloc(LIST_DEFAULT_SIZE * sizeof(float complex));
  if (list->values == NULL) {
    return -1;
  }

  list->num = 0;
  list->capacity = LIST_DEFAULT_SIZE;

  return 0;
}

void cnl_free(complex_number_list_t *list) {
  free(list->values);

  list->num = 0;
  list->capacity = 0;
}

int cnl_add(complex_number_list_t *list, float complex number) {
  // capacity = 0, list is not initialized
  if (list->capacity == 0) {
    int return_code;
    if ((return_code = (cnl_init(list))) != 0) {
      return return_code;
    }
  }

  // no more space left to store any new item, the list must be resized
  if (list->num >= list->capacity) {
    // new size/capacity
    list->capacity = list->capacity * 3 / 2;

    float complex *og_buffer = list->values;
    list->values = (float complex *)realloc(
        list->values, list->capacity * sizeof(float complex));
    if (list->values == NULL) {
      free(og_buffer);
      return -1;
    }
  }

  list->values[list->num] = number;

  list->num += 1;

  return 0;
}

void cnl_print(complex_number_list_t *list) {
// only write to stderr if 'DDEBUG' is defined
#ifdef DDEBUG
  fprintf(stderr, "ComplexNumberList {\n");

  fprintf(stderr, "\tnum: %i\n", list->num);
  fprintf(stderr, "\tcapacity: %i\n", list->capacity);
  fprintf(stderr, "\tvalues: [\n");

  for (int i = 0; i < list->num; i++) {
    fprintf(stderr, "\t\t");
    fprintf(stderr, "%f %f*I", crealf(list->values[i]),
            cimagf(list->values[i]));

    if (i < list->num - 1)
      fprintf(stderr, ",");

    fprintf(stderr, "\n");
  }

  fprintf(stderr, "\t]\n");

  fprintf(stderr, "}\n");
#endif
}

float complex cnl_get_at_index(complex_number_list_t *list, int index) {
  if (index < 0 || index >= list->num) {
    return NAN + NAN * I;
  }

  return list->values[index];
}
