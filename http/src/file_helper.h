#ifndef _FH
#define _FH

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

bool file_at_path_exists(const char *path);

long file_size(FILE *file);

int combine_file_paths(const char *path1, const char *path2, char **out);

#endif
