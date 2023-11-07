#ifndef CNL_
#define CNL_

#include <complex.h>

typedef struct {
  float complex *values;

  int num;
  int capacity;
} complex_number_list_t;

int cnl_init(complex_number_list_t *list);
void cnl_free(complex_number_list_t *list);
int cnl_add(complex_number_list_t *list, float complex number);
void cnl_print(complex_number_list_t *list);
float complex cnl_get_at_index(complex_number_list_t *list, int index);

#endif
