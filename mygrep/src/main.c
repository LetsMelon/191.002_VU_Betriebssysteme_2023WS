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
        output_init_file(args.output_file);
    }

    arguments_print(&args);
    if (args.input_files_num == 0)
    {
        process_file(stdin, &args);
    }
    else
    {
        process_files(&args);
    }

    arguments_free(&args);
    output_free();

    return EXIT_SUCCESS;
}
