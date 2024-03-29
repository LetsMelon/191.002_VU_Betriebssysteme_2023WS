/**
 * @file logic.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions to process files.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "logic.h"
#include "output.h"

int process_line(const char *line, arguments_t *args);

int process_files(arguments_t *args) {
  for (int i = 0; i < args->input_files_num; i++) {
    char *file_path = args->input_files[i];
    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
      perror("Failed to open the file");
      return 1;
    }

    process_file(file, args);

    fclose(file);
  }

  return 0;
}

int process_file(FILE *file, arguments_t *args) {
  size_t len = 256;
  char *line = malloc(len * sizeof(char));
  if (line == NULL) {
    return -1;
  }

  while (getline(&line, &len, file) != -1) {
    if (process_line(line, args) != 0) {
      return -1;
    }
  }

  free(line);

  return 0;
}

int process_line(const char *line, arguments_t *args) {
  char *current_line = strdup(line);
  if (current_line == NULL) {
    return -1;
  }

  char *result;
  if (args->case_sensitive) {
    result = strstr(current_line, args->keyword);
  } else {
    to_lowercase(current_line);
    result = strstr(current_line, args->keyword);
    // The strcasestr() function is a nonstandard extension.
    // https://linux.die.net/man/3/strcasestr
    // result = strcasestr(current_line, args->keyword);
  }

  if (result != NULL) {
    output_printf(line);
  }

  free(current_line);

  return 0;
}
