#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "arguments.h"
#include "output.h"
#include "logic.h"

const char *USAGE = "SYNOPSIS\n\tmygrep[-i][-o outfile] keyword [file...]\n";

int main(int argc, char **argv)
{
    // mygrep [-i] [-o outfile] keyword [file...]
    // [0-9][A-Z][a-z].,:-!=?% and whitespace

    arguments_t args;
    arguments_init(&args);

    if (arguments_parse(&args, argc, argv) == -1)
    {
        printf("%s", USAGE);
        return EXIT_FAILURE;
    }

    if (args.output_file == NULL)
    {
        output_init_stdout();
    }
    else
    {
        if (output_init_file(args.output_file) != 0)
        {
            printf("Error while trying to initialize output file.\n");
            return EXIT_FAILURE;
        }
    }

    arguments_print(&args);
    if (args.input_files_num == 0)
    {
        if (process_file(stdin, &args) != 0)
        {
            printf("Error while trying to read from stdin.\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        if (process_files(&args) != 0)
        {
            printf("Error while trying to read from the given files.\n");
            return EXIT_FAILURE;
        }
    }

    arguments_free(&args);
    output_free();

    return EXIT_SUCCESS;
}
