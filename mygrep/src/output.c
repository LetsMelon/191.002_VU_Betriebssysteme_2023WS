#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "output.h"

typedef enum OUTPUT_TYPE
{
    E_STDOUT,
    E_FILE
} output_type_e;

static output_type_e output_type;
static FILE *out_file = NULL;

void output_init_stdout(void)
{
    output_type = E_STDOUT;
    out_file = stdout;
}

int output_init_file(const char *file_path)
{
    output_type = E_FILE;

    FILE *file = fopen(file_path, "w");
    if (file == NULL)
    {
        return -1;
    }

    out_file = file;

    return 0;
}

int output_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    char modified_format[strlen(format) * 2 + 1];
    int j = 0;
    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%')
        {
            modified_format[j] = '%';
            j += 1;
        }

        modified_format[j] = format[i];
        j += 1;
    }
    modified_format[j] = '\0';

    int return_value = vfprintf(out_file, modified_format, args);
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
