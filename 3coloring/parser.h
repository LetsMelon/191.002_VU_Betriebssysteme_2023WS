/**
 * @file parser.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.12.2023
 *
 * @brief Provides utility functions for parsing
 */

#ifndef _P
#define _P

#include <complex.h>
#include <stdbool.h>

#include "edge.h"

/**
 * @struct string_list_t
 * @brief Structure to hold an array of strings.
 */
typedef struct {
  char **values; ///< Internal array of owned char*
  int num;       ///< How many items are stored in the array 'values'
  int capacity;  ///< How many items the array 'values' can store
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

/**
 * Parses the input as an int
 * @brief Parses the input as an int.
 * @param input the input char*.
 * @param value Pointer to where the value should be stored.
 * @return Returns 0 if non error has been encountered
 */
int p_parse_as_int(const char *input, int *value);

/**
 * Parses the input as an edge_t
 * @brief Parses the input as an edge_t.
 * @param input the input char*.
 * @param edge Pointer to where the edge should be stored.
 * @return Returns 0 if non error has been encountered
 */
int p_parse_as_edge(const char *input, edge_t *edge);

#endif /* _P */
