/**
 * @file parser.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for parsing
 */

#ifndef _P
#define _P

#include <complex.h>
#include <stdbool.h>

/**
 * @struct string_list_t
 */
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
 * Prints the list to stdout.
 * @param list the list that should be printed.
 */
void sl_print(string_list_t *list);

/**
 * Splits the input into parts where the pattern occurs.
 * @brief Splits the input into parts where the pattern occurs and stores the
 * results into the given list.
 * @param input the input char*.
 * @param pattern the pattern to match at.
 * @param list the list to store the parts.
 * @return Returns 0 if non error has been encountered
 */
int p_split_at(const char *input, char pattern, string_list_t *list);

#endif /* _P */
