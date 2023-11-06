#include <stdlib.h>
#include <complex.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"

#include "complex_number_list.h"
#include "complex_helper.h"

const int list_default_size = 16;

int cnl_init(complex_number_list_t *list)
{

    list->values = (float complex *)malloc(list_default_size * sizeof(float complex));
    if (list->values == NULL)
    {
        return -1;
    }

    list->num = 0;
    list->capacity = list_default_size;

    return 0;
}

void cnl_free(complex_number_list_t *list)
{
    free(list->values);

    list->num = 0;
    list->capacity = 0;
}

int cnl_add(complex_number_list_t *list, float complex number)
{
    if (list->capacity == 0)
    {
        int return_code;
        if ((return_code = (cnl_init(list))) != 0)
        {
            return return_code;
        }
    }

    if (list->num >= list->capacity)
    {
        list->capacity = list->capacity * 3 / 2;

        float complex *og_buffer = list->values;
        list->values = (float complex *)realloc(list->values, list->capacity * sizeof(float complex));
        if (list->values == NULL)
        {
            free(og_buffer);
            return -1;
        }
    }

    list->values[list->num] = number;

    list->num += 1;

    return 0;
}

void cnl_print(complex_number_list_t *list)
{
    fprintf(stderr, "ComplexNumberList {\n");

    fprintf(stderr, "\tnum: %i\n", list->num);
    fprintf(stderr, "\tcapacity: %i\n", list->capacity);
    fprintf(stderr, "\tvalues: [\n");

    for (int i = 0; i < list->num; i++)
    {
        fprintf(stderr, "\t\t");
        fprintf(stderr, "%f %f*I", crealf(list->values[i]), cimagf(list->values[i]));

        if (i < list->num - 1)
            fprintf(stderr, ",");

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\t]\n");

    fprintf(stderr, "}\n");
}

float complex cnl_get_at_index(complex_number_list_t *list, int index)
{
    if (index < 0 || index >= list->num)
    {
        return NAN + NAN * I;
    }

    return list->values[index];
}
