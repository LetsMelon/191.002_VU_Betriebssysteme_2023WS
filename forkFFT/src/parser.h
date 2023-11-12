/**
 * @file parser.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for parsing float complex values
 */

#ifndef _P
#define _P

#include <complex.h>
#include <stdbool.h>

typedef struct {
  char **values; ///< Internal array of owned char*

  int num;      ///< How many items are stored in the array 'values'
  int capacity; ///< How many items the array 'values' can store
} string_list_t;

/**
 * Frees the given list.
 * @brief Frees the list.
 * @param list the list that should be freed.
 */
void sl_free(string_list_t *list);

/**
 * Splits the input into parts where the pattern occurs.
 * @brief Splits the input into parts where the pattern occurs and stores the
 * results into the given list.
 * @param input the input char*.
 * @param pattern the pattern to match at.
 * @param list the list to store the parts.
 * @return Returns 0 if non error has been encountered
 */
int p_split_at(char *input, char pattern, string_list_t *list);

/**
 * Parses the values from the given list into one float complex value.
 * @param list the list to store the parts.
 * @param n pointer to an float complex to store the result.
 * @return Returns 0 if non error has been encountered
 */
int p_parse_as_complex_float(string_list_t *list, float complex *n);

#endif /* _P */
