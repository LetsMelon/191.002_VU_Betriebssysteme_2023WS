/**
 * @file parser.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for parsing
 */

#include <assert.h>
#include <math.h>
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
int sl_add(string_list_t *list, char *value) {
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

void sl_print(string_list_t *list) {
  // #ifdef DDEBUG
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
  // #endif
}

int p_split_at(const char *input, char pattern, string_list_t *list) {
  if (sl_init(list) != 0) {
    return -1;
  }

  int input_len = strlen(input);
  if (input_len == 0) {
    return 0;
  }

  // 512 Bytes should be enough to hold char* temporary
  static char tmp_buffer[512];

  int last_split = 0;
  for (int i = 0; i < input_len; i += 1) {
    if (input[i] == pattern) {
      if (last_split != i) {
        int copy_len = i - last_split;
        strncpy(tmp_buffer, input + last_split, copy_len);
        tmp_buffer[copy_len] = '\0';

        if (sl_add(list, tmp_buffer) != 0) {
          return -1;
        }
      }

      last_split = i + 1;
    }
  }

  if (last_split != input_len) {
    int copy_len = input_len - last_split;
    strncpy(tmp_buffer, input + last_split, copy_len);
    tmp_buffer[copy_len] = '\0';

    if (sl_add(list, tmp_buffer) != 0) {
      return -1;
    }
  }

  return 0;
}
