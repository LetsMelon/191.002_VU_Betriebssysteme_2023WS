/**
 * @file child.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions useful for creating a child process and
 * inter process communication via pipes.
 *
 * The util module. It contains functions for closing all pipes, creating a
 * child process and reading complex float numbers from an fd.
 */

#ifndef _C /* prevent multiple inclusion */
#define _C

#include <complex.h>

/*! \def READ
    \brief Returns the index of the READ fd given back from pipes
*/

/*! \def WRITE
    \brief Returns the index of the WRITE fd given back from pipes
*/

#define READ 0
#define WRITE 1

/**
 * Closes all fd's inside pipes.
 * @brief This function closes the fd at index 0 and 1 from 'pipes.s'
 * @param pipes int[2] array with the fd's
 * @return Returns 0 if non error has been encountered, otherwise -1.
 */
int c_close_pipes(int *pipes);

/**
 * Duplicates the pipes and replaces the current process.
 * @brief Duplicates the pipes and pipes the them STDIN, STDOUT specific. Then
 * replaces the current process with a new 'forkFFT'. Exits with EXIT_FAILURE if
 * an error occurs.
 * @param pipe_stdin int[2] array with fd's for STDIN.
 * @param pipe_stdout int[2] array with fd's for STDOUT.
 */
void c_create_child(int *pipe_stdin, int *pipe_stdout);

/**
 * Reads numbers of float complex numbers from fd.
 * @brief This function reads from the given fd and tries to read and parse n
 * (numbers) * float complex values.
 * @param fd file descriptor to read from.
 * @param array float complex array with minimum of numbers elements.
 * @param numbers how many values this function should read.
 * @return Returns 0 if the functions was able to read all n (numbers) values,
 * otherwise -1 or if an error occurred.
 */
int c_read_n_complex_numbers_from_fd(int fd, float complex *array, int numbers);

#endif /* _C */
