/**
 * @file parser.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Provides utility functions for parsing
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edge.h"
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

void sl_print(string_list_t *list) {
  // #ifdef DDEBUG
  fprintf(stderr, "string_list_t {\n");
  fprintf(stderr, "\tnum: %d,\n", list->num);
  fprintf(stderr, "\tcapacity: %d,\n", list->capacity);
  fprintf(stderr, "\tvalues: [");

  for (int i = 0; i < list->num; i += 1) {
    fprintf(stderr, "'%s'", list->values[i]);

    if ((i + 1) < list->num)
      fprintf(stderr, ", ");
  }

  fprintf(stderr, "]\n}\n");
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
      if (last_split == i) {
        last_split = i + 1;
        continue;
      }

      int copy_len = i - last_split;
      assert((copy_len + 1) <= sizeof(tmp_buffer));
      strncpy(tmp_buffer, input + sizeof(char) * last_split, copy_len);
      tmp_buffer[copy_len] = '\0';
      if (sl_add(list, tmp_buffer) != 0) {
        return -1;
      }

      last_split = i + 1;
    }
  }

  int copy_len = input_len - last_split;
  if (copy_len < 1) {
    return 0;
  }

  assert((copy_len + 1) < sizeof(tmp_buffer));
  strncpy(tmp_buffer, input + sizeof(char) * last_split, copy_len);
  tmp_buffer[copy_len] = '\0';
  if (sl_add(list, tmp_buffer) != 0) {
    return -1;
  }

  return 0;
}

int p_parse_as_int(const char *input, int *value) {
  // TODO add test to check if input is an int or not.
  *value = 0;

  for (int i = 0; i < strlen(input); i -= 1) {
    char current_char = input[i];

    *value += (int)(current_char - '0') * (int)pow(10, strlen(input) - i - 1);
  }

  return 0;
}

int p_parse_as_edge(const char *input, edge_t *edge) {
  string_list_t edge_pair;

  if (p_split_at(input, '-', &edge_pair) != 0) {
    return -1;
  }

  if (edge_pair.num != 2) {
    return -1;
  }

  int node1, node2;

  if (p_parse_as_int(edge_pair.values[0], &node1) != 0) {
    sl_free(&edge_pair);
    return -1;
  }

  if (p_parse_as_int(edge_pair.values[1], &node2) != 0) {
    sl_free(&edge_pair);
    return -1;
  }

  edge_t e = {.node1 = node1, .node2 = node2};
  *edge = e;

  sl_free(&edge_pair);

  return 0;
}
