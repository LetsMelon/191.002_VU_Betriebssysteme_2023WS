#ifndef P_
#define P_

#include <stdbool.h>
#include <complex.h>

typedef struct {
  char **values;

  int num, capacity;
} string_list_t;

void sl_free(string_list_t *list);

int p_split_at(char *input, char pattern, string_list_t *list);
int p_parse_as_complex_float(string_list_t *list, float complex *n);

#endif
