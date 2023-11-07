#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "child.h"
#include "complex_helper.h"
#include "complex_number_list.h"

#define PI 3.141592654f

int main(void)
{
    // TODO read flag from the command line arguments
    ch_init_output(true);

    complex_number_list_t complex_list;
    if (cnl_init(&complex_list) != 0)
    {
        return EXIT_FAILURE;
    }

    size_t len = 256;
    char *line = malloc(len * sizeof(char));
    if (line == NULL)
    {
        return -1;
    }

    while (getline(&line, &len, stdin) != -1)
    {
        char *input = line;
        float complex number = 0.0f;

        for (int i = 0; i < 2; ++i)
        {
            char *endptr = NULL;

            float n = strtof(input, &endptr);
            if (i == 0)
            {
                number += n;
            }
            else
            {
                number += n * I;
            }

            if (endptr == NULL)
                break;

            input = endptr;
        }

        if (cnl_add(&complex_list, number) != 0)
        {
            free(line);
            cnl_free(&complex_list);
            return EXIT_FAILURE;
        }
    }
    free(line);

    int n = complex_list.num;

    if (n == 0)
    {
        cnl_free(&complex_list);

        return EXIT_FAILURE;
    }

    if (n == 1)
    {
        float complex n = cnl_get_at_index(&complex_list, 0);
        ch_print_complex_number(n);
        printf("\n");

        cnl_free(&complex_list);

        return EXIT_SUCCESS;
    }

    // https://stackoverflow.com/a/600306
    bool is_power_of_two = (n & (n - 1)) == 0;
    if (is_power_of_two == false)
    {
        cnl_free(&complex_list);

        return EXIT_FAILURE;
    }

    int parent_to_child_odd[2],
        child_odd_to_parent[2],
        parent_to_child_even[2],
        child_even_to_parent[2];

    pipe(parent_to_child_odd);
    pipe(child_odd_to_parent);
    pipe(parent_to_child_even);
    pipe(child_even_to_parent);

    pid_t p_odd = fork();
    if (p_odd < 0)
    {
        cnl_free(&complex_list);

        close_pipes(&parent_to_child_odd[0]);
        close_pipes(&child_odd_to_parent[0]);
        close_pipes(&parent_to_child_even[0]);
        close_pipes(&child_even_to_parent[0]);

        return EXIT_FAILURE;
    }
    else if (p_odd == 0)
    {
        close_pipes(&parent_to_child_even[0]);
        close_pipes(&child_even_to_parent[0]);

        create_child(&parent_to_child_odd[0], &child_odd_to_parent[0]);

        return EXIT_FAILURE;
    }

    pid_t p_even = fork();
    if (p_even < 0)
    {
        cnl_free(&complex_list);

        close_pipes(&parent_to_child_odd[0]);
        close_pipes(&child_odd_to_parent[0]);
        close_pipes(&parent_to_child_even[0]);
        close_pipes(&child_even_to_parent[0]);

        return EXIT_FAILURE;
    }
    else if (p_even == 0)
    {
        close_pipes(&parent_to_child_odd[0]);
        close_pipes(&child_odd_to_parent[0]);

        create_child(&parent_to_child_even[0], &child_even_to_parent[0]);

        return EXIT_FAILURE;
    }

    close(parent_to_child_odd[READ]);
    close(parent_to_child_even[READ]);

    close(child_odd_to_parent[WRITE]);
    close(child_even_to_parent[WRITE]);

    int fd_to_children[2] = {parent_to_child_even[WRITE], parent_to_child_odd[WRITE]};
    for (int i = 0; i < n; i++)
    {
        int fd_to_child = fd_to_children[i % 2];

        float complex n = cnl_get_at_index(&complex_list, i);
        dprintf(fd_to_child, "%.6f %.6f*i\n", crealf(n), cimagf(n));
    }
    close(parent_to_child_odd[WRITE]);
    close(parent_to_child_even[WRITE]);

    cnl_free(&complex_list);

    int status_odd;
    waitpid(p_odd, &status_odd, 0);
    fprintf(stderr, "Fork status_odd: %d\n", WEXITSTATUS(status_odd));

    int status_even;
    waitpid(p_even, &status_even, 0);
    fprintf(stderr, "Fork status_even: %d\n", WEXITSTATUS(status_even));

    if ((WEXITSTATUS(status_odd) | WEXITSTATUS(status_even)) != EXIT_SUCCESS)
    {
        close(child_odd_to_parent[READ]);
        close(child_even_to_parent[READ]);

        return EXIT_FAILURE;
    }

    float complex *result_odd, *result_even, *result;
    result_odd = (float complex *)malloc((n / 2) * sizeof(float complex));
    if (result_odd == NULL)
    {
        close(child_odd_to_parent[READ]);
        close(child_even_to_parent[READ]);

        return EXIT_FAILURE;
    }
    result_even = (float complex *)malloc((n / 2) * sizeof(float complex));
    if (result_even == NULL)
    {
        close(child_odd_to_parent[READ]);
        close(child_even_to_parent[READ]);

        free(result_odd);

        return EXIT_FAILURE;
    }

    c_read_n_complex_numbers_from_fd(child_odd_to_parent[READ], &result_odd[0], n / 2);
    close(child_odd_to_parent[READ]);

    c_read_n_complex_numbers_from_fd(child_even_to_parent[READ], &result_even[0], n / 2);
    close(child_even_to_parent[READ]);

    result = (float complex *)malloc(n * sizeof(float complex));
    if (result == NULL)
    {
        free(result_odd);
        free(result_even);

        return EXIT_FAILURE;
    }

    for (int k = 0; k < n / 2; k++)
    {
        float complex re = result_even[k];
        float complex ro = result_odd[k];

        float complex middle_term = cosf((-(2.0 * PI) / (float)n) * (float)k) + I * sinf((-(2.0 * PI) / (float)n) * (float)k);

        result[k] = re + middle_term * ro;
        result[k + n / 2] = re - middle_term * ro;
    }

    free(result_odd);
    free(result_even);

    fprintf(stderr, "n = %d\n", n);

    for (int i = 0; i < n; ++i)
    {
        ch_print_complex_number(result[i]);
        printf("\n");
    }

    free(result);

    return EXIT_SUCCESS;
}
