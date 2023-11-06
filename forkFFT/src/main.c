#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "math.h"

#include "complex_helper.h"
#include "complex_number_list.h"
#include "child.h"

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

    int parent_to_child1[2], child1_to_parent[2], parent_to_child2[2], child2_to_parent[2];
    pipe(parent_to_child1);
    pipe(child1_to_parent);
    pipe(parent_to_child2);
    pipe(child2_to_parent);

    pid_t p1 = fork();
    if (p1 < 0)
    {
        cnl_free(&complex_list);

        close_pipes(&parent_to_child1[0]);
        close_pipes(&child1_to_parent[0]);
        close_pipes(&parent_to_child2[0]);
        close_pipes(&child2_to_parent[0]);

        return EXIT_FAILURE;
    }
    else if (p1 == 0)
    {
        close_pipes(&parent_to_child2[0]);
        close_pipes(&child2_to_parent[0]);

        create_child(&parent_to_child1[0], &child1_to_parent[0]);

        return EXIT_FAILURE;
    }

    pid_t p2 = fork();
    if (p2 < 0)
    {
        cnl_free(&complex_list);

        close_pipes(&parent_to_child1[0]);
        close_pipes(&child1_to_parent[0]);
        close_pipes(&parent_to_child2[0]);
        close_pipes(&child2_to_parent[0]);

        return EXIT_FAILURE;
    }
    else if (p2 == 0)
    {
        close_pipes(&parent_to_child1[0]);
        close_pipes(&child1_to_parent[0]);

        create_child(&parent_to_child2[0], &child2_to_parent[0]);

        return EXIT_FAILURE;
    }

    close(parent_to_child1[READ]);
    close(parent_to_child2[READ]);

    close(child1_to_parent[WRITE]);
    close(child2_to_parent[WRITE]);

    int fd_to_children[2] = {parent_to_child1[WRITE], parent_to_child2[WRITE]};
    for (int i = 0; i < n; i++)
    {
        int fd_to_child = fd_to_children[i % 2];

        float complex n = cnl_get_at_index(&complex_list, i);
        if (crealf(n) == NAN || cimagf(n) == NAN)
        {
            close(parent_to_child1[WRITE]);
            close(parent_to_child2[WRITE]);

            close(child1_to_parent[READ]);
            close(child2_to_parent[READ]);

            cnl_free(&complex_list);

            return EXIT_FAILURE;
        }

        dprintf(fd_to_child, "%.6f %.6f*i\n", crealf(n), cimagf(n));
    }
    close(parent_to_child1[WRITE]);
    close(parent_to_child2[WRITE]);

    cnl_free(&complex_list);

    int status1;
    waitpid(p1, &status1, 0);
    fprintf(stderr, "Fork status1: %d\n", WEXITSTATUS(status1));

    int status2;
    waitpid(p2, &status2, 0);
    fprintf(stderr, "Fork status2: %d\n", WEXITSTATUS(status2));

    if ((WEXITSTATUS(status1) | WEXITSTATUS(status2)) != EXIT_SUCCESS)
    {
        close(child1_to_parent[READ]);
        close(child2_to_parent[READ]);

        return EXIT_FAILURE;
    }

    float complex *temp_left, *temp_right, *result;
    temp_left = (float complex *)malloc((n / 2) * sizeof(float complex));
    if (temp_left == NULL)
    {
        close(child1_to_parent[READ]);
        close(child2_to_parent[READ]);

        return EXIT_FAILURE;
    }
    temp_right = (float complex *)malloc((n / 2) * sizeof(float complex));
    if (temp_right == NULL)
    {
        close(child1_to_parent[READ]);
        close(child2_to_parent[READ]);

        free(temp_left);

        return EXIT_FAILURE;
    }

    c_read_n_complex_numbers_from_fd(child1_to_parent[READ], &temp_left[0], n / 2);
    close(child1_to_parent[READ]);

    c_read_n_complex_numbers_from_fd(child2_to_parent[READ], &temp_right[0], n / 2);
    close(child2_to_parent[READ]);

    result = (float complex *)malloc(n * sizeof(float complex));
    if (result == NULL)
    {
        free(temp_left);
        free(temp_right);

        return EXIT_FAILURE;
    }

    for (int i = 0; i < n / 2; i++)
    {
        float complex re = temp_left[i];
        float complex rk = temp_right[i];

        float complex middle_term = cosf(-(2.0 * PI) / (float)n) + I * sinf(-(2.0 * PI) / (float)n);

        result[i] = re + middle_term * rk;
        result[i + n / 2] = re - middle_term * rk;
    }

    fprintf(stderr, "n = %d\n", n);

    for (int i = 0; i < n; ++i)
    {
        ch_print_complex_number(result[i]);
        printf("\n");
    }

    free(temp_left);
    free(temp_right);
    free(result);

    return EXIT_SUCCESS;
}
