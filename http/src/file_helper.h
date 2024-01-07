#ifndef _FH
#define _FH

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

bool file_at_path_exists(const char *path);

long file_size(FILE *file);

#endif
