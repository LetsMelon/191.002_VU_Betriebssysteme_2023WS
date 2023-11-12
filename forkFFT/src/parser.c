#include <complex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/**
 * @brief Helper function to initialise a string_list_t.
 */
static int sl_init(string_list_t *list) {
  list->capacity = 16;

  list->values = (char **)malloc(list->capacity * sizeof(char *));
  if (list->values == NULL) {
    list->capacity = 0;
    return -1;
  }

  list->num = 0;

  return 0;
}

void sl_free(string_list_t *list) {
  for (int i = 0; i < list->num; i += 1) {
    free(list->values[i]);
  }
  free(list->values);

  list->num = 0;
  list->capacity = 0;
}

/**
 * @brief Helper function to add an item to the given list.
 */
static int sl_add(string_list_t *list, char *value) {
  if (list->capacity == 0) {
    if (sl_init(list) != 0) {
      return -1;
    }
  }

  if (list->num >= list->capacity) {
    list->capacity = list->capacity * 3 / 2;

    char **og_values = list->values;
    list->values =
        (char **)realloc(list->values, list->capacity * sizeof(char *));
    if (list->values == NULL) {
      free(og_values);
      return -1;
    }
  }

  list->values[list->num] = strdup(value);

  list->num += 1;

  return 0;
}

#ifdef DDEBUG
static void sl_print(string_list_t *list) {
  printf("string_list_t {\n");
  printf("\tnum: %d,\n", list->num);
  printf("\tcapacity: %d,\n", list->capacity);
  printf("\tvalues: [");

  for (int i = 0; i < list->num; i += 1) {
    printf("'%s'", list->values[i]);

    if ((i + 1) < list->num)
      printf(", ");
  }

  printf("]\n}\n");
}
#endif

int p_split_at(char *input, char pattern, string_list_t *list) {
  if (sl_init(list) != 0) {
    return -1;
  }

  int input_len = strlen(input);
  if (input_len == 0) {
    return 0;
  }

  int last_split = 0;
  for (int i = 0; i < input_len; i += 1) {
    if (input[i] == pattern) {
      if (last_split == i) {
        last_split = i + 1;
        continue;
      }

      int copy_len = i - last_split;
      char *tmp = (char *)malloc(sizeof(char) * (copy_len + 1));
      if (tmp == NULL) {
        return -1;
      }

      strncpy(tmp, input + sizeof(char) * last_split, copy_len);
      tmp[copy_len] = '\0';
      if (sl_add(list, tmp) != 0) {
        free(tmp);
        return -1;
      }
      free(tmp);

      last_split = i + 1;
    }
  }

  int copy_len = input_len - last_split;
  if (copy_len < 1) {
    return 0;
  }

  char *tmp = (char *)malloc(sizeof(char) * (copy_len + 1));
  if (tmp == NULL) {
    return -1;
  }

  strncpy(tmp, input + sizeof(char) * last_split, copy_len);
  tmp[copy_len] = '\0';
  if (sl_add(list, tmp) != 0) {
    free(tmp);
    return -1;
  }
  free(tmp);

  return 0;
}

static bool p_has_complex_number_chars(char *input) {
  // Allowed chars for an complex float
  char *possible_values = "0123456789.+-*i \n";

  bool has_all_chars = true;
  for (int i = 0; i < strlen(input); i += 1) {
    bool contains = false;
    for (int j = 0; j < strlen(possible_values); j += 1) {
      if (input[i] == possible_values[j]) {
        contains = true;
        break;
      }
    }

    has_all_chars = has_all_chars & contains;
    if (has_all_chars == false) {
      break;
    }
  }

  return has_all_chars;
}

int p_parse_as_complex_float(string_list_t *list, float complex *n) {
  if (list->num < 1 || list->num > 2) {
    return -1;
  }

  *n = 0.0f;

  for (int i = 0; i < list->num; i += 1) {
    char *inp = list->values[i];

    // Checks if inp only has allowed chars
    if (p_has_complex_number_chars(inp) != true) {
      return -1;
    }

    float current_n = strtof(inp, NULL);

    if (i == 0) {
      *n += current_n;
    } else {
      *n += current_n * I;
    }
  }

  return 0;
}

/*
int main(void) {
  string_list_t list;
  char *str = "3 -5b";

  p_split_at(str, ' ', &list);
  sl_print(&list);

  float complex n;
  p_parse_as_complex_float(&list, &n);

  sl_free(&list);

  return 0;
}
*/
