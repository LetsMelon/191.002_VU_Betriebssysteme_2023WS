#include "file_helper.h"

#include <stdbool.h>
#include <stdio.h>

bool file_at_path_exists(const char *path) { return access(path, F_OK) != -1; }

long file_size(FILE *file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);

  fseek(file, 0, SEEK_SET);

  return size;
}
