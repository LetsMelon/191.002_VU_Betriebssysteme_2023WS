/**
 * @file complex_helper.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for printing float complex values.
 */

#ifndef _CH /* prevent multiple inclusion */
#define _CH

#include <complex.h>
#include <stdbool.h>

/**
 * Initializes a static value output strictness.
 * @brief Initializes a static value.
 * @param strict true => precision = 3; false => precision = 6;
 */
void ch_init_output(bool strict);

/**
 * Prints a float complex to stdout. The printing precision can be changed via
 * 'ch_init_output'.
 * @brief Prints a float complex to stdout. The printing precision can be
 * changed via 'ch_init_output'.
 * @param float complex number to print.
 */
void ch_print_complex_number(float complex number);

#endif /* _CH */
