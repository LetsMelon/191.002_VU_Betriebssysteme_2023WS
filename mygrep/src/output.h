#ifndef OUTPUT_H
#define OUTPUT_H

typedef enum OUTPUT_TYPE
{
    E_STDOUT,
    E_FILE
} output_type_e;

int output_init_stdout(void);
int output_init_file(const char *file_path);

int output_printf(const char *format, ...);

void output_free(void);

#endif
