#ifndef LOGIC_H
#define LOGIC_H

#include <stdio.h>

#include "arguments.h"

int process_files(arguments_t *args);
void process_file(FILE *file, arguments_t *args);
void process_line(const char *line, arguments_t *args);

#endif
