#include <stdio.h>
#include <stdarg.h>

#include "output.h"

static output_type_e output_type;
static FILE *out_file = NULL;

int output_init_stdout(void)
{
    output_type = E_STDOUT;
    out_file = stdout;

    return 0;
}

int output_init_file(const char *file_path)
{
    output_type = E_FILE;

    FILE *file = fopen(file_path, "w");
    if (file == NULL)
    {
        perror("Failed to open the file");
        return 1;
    }

    out_file = file;

    return 0;
}

int output_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int return_value = vfprintf(out_file, format, args);
    va_end(args);

    return return_value;
}

void output_free(void)
{
    if (output_type == E_FILE)
    {
        fclose(out_file);
    }
}
