#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

typedef struct arguments
{
    char *output_file;

    char *keyword;

    bool case_sensitive;

    char **input_files;
    int input_files_num;
    int input_files_capacity;
} arguments_t;

void arguments_init(arguments_t *arg)
{
    arg->output_file = NULL;

    arg->keyword = NULL;

    arg->case_sensitive = true;

    arg->input_files = NULL;
    arg->input_files_num = 0;
    arg->input_files_capacity = 0;
}

void arguments_free(arguments_t *arg)
{
    free(arg->output_file);
    free(arg->keyword);

    for (int i = 0; i < arg->input_files_num; i += 1)
    {
        free(arg->input_files[i]);
    }
    free(arg->input_files);
    arg->input_files_num = 0;
    arg->input_files_capacity = 0;
}

void arguments_add_input_file(arguments_t *arg, char *file)
{
    if (arg->input_files == NULL || arg->input_files_capacity == 0)
    {
        arg->input_files_capacity = 8;
        arg->input_files = (char **)malloc(arg->input_files_capacity * sizeof(char *));
        arg->input_files_num = 0;
    }

    if (arg->input_files_num >= arg->input_files_capacity)
    {
        arg->input_files_capacity = arg->input_files_capacity * 3 / 2;
        arg->input_files = (char **)realloc(arg->input_files, arg->input_files_capacity * sizeof(char *));
    }

    arg->input_files[arg->input_files_num] = (char *)malloc(strlen(file) + 1);
    strcpy(arg->input_files[arg->input_files_num], file);
    arg->input_files_num += 1;
}

int arguments_parse(arguments_t *arg, int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "io:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            arg->case_sensitive = false;
            break;
        case 'o':
            arg->output_file = strdup(optarg);
            break;
        default:
            return -1;
        }
    }

    if (optind < argc)
    {
        arg->keyword = strdup(argv[optind]);
        optind += 1;
    }

    while (optind < argc)
    {
        char *file = strdup(argv[optind]);

        arguments_add_input_file(arg, file);

        optind += 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    // mygrep [-i] [-o outfile] keyword [file...]
    // [0-9][A-Z][a-z].,:-!=?% and whitespace

    arguments_t args;
    arguments_init(&args);

    if (arguments_parse(&args, argc, argv) == -1)
    {
        printf("Error occurred while trying to parse the arguments\n");
    }

    arguments_free(&args);

    return EXIT_SUCCESS;
}
