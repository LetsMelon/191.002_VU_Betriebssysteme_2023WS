#ifndef OUTPUT_H
#define OUTPUT_H

void output_init_stdout(void);
int output_init_file(const char *file_path);

int output_printf(const char *format, ...);

void output_free(void);

#endif
