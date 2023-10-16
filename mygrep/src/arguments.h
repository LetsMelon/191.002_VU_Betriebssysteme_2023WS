#ifndef ARGUMENTS_H
#define ARGUMENTS_H

typedef struct arguments
{
    char *output_file;

    char *keyword;

    bool case_sensitive;

    char **input_files;
    int input_files_num;
    int input_files_capacity;
} arguments_t;

void to_lowercase(char *str);

void arguments_init(arguments_t *arg);
void arguments_free(arguments_t *arg);
void arguments_print(arguments_t *arg);

void arguments_add_input_file(arguments_t *arg, char *file);
int arguments_parse(arguments_t *arg, int argc, char **argv);

#endif
