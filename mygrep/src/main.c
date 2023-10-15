#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "arguments.h"
#include "output.h"

int main(int argc, char **argv)
{
    // mygrep [-i] [-o outfile] keyword [file...]
    // [0-9][A-Z][a-z].,:-!=?% and whitespace

    // output_init_stdout();
    output_init_file("output.txt");

    arguments_t args;
    arguments_init(&args);

    if (arguments_parse(&args, argc, argv) == -1)
    {
        printf("Error occurred while trying to parse the arguments\n");
    }

    output_printf("Hello World!\n");

    arguments_free(&args);
    output_free();

    return EXIT_SUCCESS;
}
