#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "logic.h"
#include "arguments.h"
#include "output.h"

int process_files(arguments_t *args)
{

    for (int i = 0; i < args->input_files_num; i++)
    {
        char *file_path = args->input_files[i];
        FILE *file = fopen(file_path, "r");

        char *line = NULL;
        size_t len = 0;

        if (file == NULL)
        {
            perror("Failed to open the file");
            return 1;
        }

        process_file(file, &args);

        fclose(file);
    }

    return 0;
}

void process_file(FILE *file, arguments_t *args)
{
    char *line;
    size_t len = 0;

    while (getline(&line, &len, file) != -1)
    {
        process_line(line, args);
    }

    free(line);
}

void process_line(const char *line, arguments_t *args)
{
    char *current_line = strdup(line);

    char *result;
    if (args->case_sensitive)
    {
        result = strstr(current_line, args->keyword);
    }
    else
    {
        to_lowercase(current_line);
        result = strstr(current_line, args->keyword);
        // The strcasestr() function is a nonstandard extension.
        // https://linux.die.net/man/3/strcasestr
        // result = strcasestr(current_line, args->keyword);
    }

    if (result != NULL)
    {
        output_printf(line);
    }

    free(current_line);
}
