/**
 * @file complex_number_list.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for printing float complex values.
 */

#ifndef _CNL /* prevent multiple inclusion */
#define _CNL

#include <complex.h>

typedef struct {
  float complex *values; ///< Internal array of the float complex values

  int num;      ///< How many items are stored in the array 'values'
  int capacity; ///< How many items the array 'values' can store
} complex_number_list_t;

/**
 * Initializes the given list.
 * @brief Initializes the list.
 * @param list the list that should be initialized.
 * @return Returns 0 if non error has been encountered
 */
int cnl_init(complex_number_list_t *list);

/**
 * Frees the given list.
 * @brief Frees the list.
 * @param list the list that should be freed.
 */
void cnl_free(complex_number_list_t *list);

/**
 * Add an item to the list.
 * @brief Adds an item to the list. If the list is full, the the function
 * reallocates it's memory, so that the new item can be stored internally.
 * @param list the list to be added to.
 * @param number the item (float complex) to be added.
 * @return Returns 0 if non error has been encountered
 */
int cnl_add(complex_number_list_t *list, float complex number);

/**
 * Prints the list to stdout.
 * @param list the list.
 */
void cnl_print(complex_number_list_t *list);

/**
 * Returns the item from the given index.
 * @brief Get's the item from the given index and returns it. If the list does
 * not store an item at the given index then the complex float 'Nan+Nan*I' is
 * returned.
 * @param list the list.
 * @param index the index into the list.
 * @return the item if the list holds it, otherwise 'Nan+Nan*I'.
 */
float complex cnl_get_at_index(complex_number_list_t *list, int index);

#endif /* _CNL */
