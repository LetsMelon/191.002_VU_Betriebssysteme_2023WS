#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>

#include "arguments.h"

void to_lowercase(char *str)
{
    if (str == NULL)
        return;

    for (int i = 0; i < strlen(str); i++)
    {
        str[i] = tolower(str[i]);
    }
}

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

void arguments_print(arguments_t *arg)
{
    printf("arguments_t { output_file: \"%s\", keyword: \"%s\", case_sensitive: %s, input_files: [",
           arg->output_file ? arg->output_file : "None",
           arg->keyword ? arg->keyword : "None",
           arg->case_sensitive ? "true" : "false");

    for (int i = 0; i < arg->input_files_num; i++)
    {
        printf("\"%s\"", arg->input_files[i]);
        if (i < arg->input_files_num - 1)
        {
            printf(", ");
        }
    }

    printf("], input_files_num: %d, input_files_capacity: %d }\n",
           arg->input_files_num, arg->input_files_capacity);
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
    if (argc < 2)
    {
        return -1;
    }

    int opt;
    int have_seen_i = 0;

    while ((opt = getopt(argc, argv, "io:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            if (have_seen_i > 0)
            {
                return -1;
            }
            arg->case_sensitive = false;
            have_seen_i += 1;
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

        if (arg->case_sensitive == false)
        {
            to_lowercase(arg->keyword);
        }

        optind += 1;
    }

    while (optind < argc)
    {
        arguments_add_input_file(arg, argv[optind]);

        optind += 1;
    }

    return 0;
}
