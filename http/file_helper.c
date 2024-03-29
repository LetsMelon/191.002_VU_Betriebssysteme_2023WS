/**
 * @file file_helper.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.01.2024
 *
 * @brief Contains implementation for file helper functions.
 */

#include "file_helper.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool file_at_path_exists(const char *path) { return access(path, F_OK) != -1; }

long file_size(FILE *file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);

  fseek(file, 0, SEEK_SET);

  return size;
}

int combine_file_paths(const char *path1, const char *path2, char **out) {
  int p1_len = strlen(path1);
  int p2_len = strlen(path2);

  int char_to_add;
  if (path1[p1_len - 1] == '/' || path2[0] == '/') {
    char_to_add = 0; // No extra '/' needed
  } else {
    char_to_add = 1; // Need to add '/'
  }

  int n = p1_len + p2_len + char_to_add + 1;
  char *tmp_out = calloc(n, sizeof(char));
  if (tmp_out == NULL) {
    return -1;
  }

  if (strcpy(tmp_out, path1) == NULL) {
    free(tmp_out);
    return -1;
  }

  int write_head = p1_len;
  if (char_to_add == 1) {
    tmp_out[write_head] = '/'; // Add '/' between paths
    write_head += 1;
  }

  if (strcpy(tmp_out + sizeof(char) * write_head, path2) == NULL) {
    free(tmp_out);
    return -1;
  }

  *out = tmp_out;
  return 0;
}
