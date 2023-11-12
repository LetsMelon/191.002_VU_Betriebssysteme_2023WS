/**
 * @file logic.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions to process files.
 */

#ifndef _LOGIC_H
#define _LOGIC_H

#include <stdio.h>

#include "arguments.h"

/**
 * Goes threw all files inside the args, opens the and calls 'process_file' for
 * each file.
 * @brief Goes threw all files inside the args, opens the and calls
 * 'process_file' for each file.
 * @param args pointer to an arguments_t
 * @return Returns 0 if non error has been encountered.
 */
int process_files(arguments_t *args);

/**
 * Reads the given file line per line and process it.
 * @brief This functions reads the given file line per line, process it via the
 * private function in logic.c 'process_line' and prints the result if any.
 * @param file file to read from
 * @param args pointer to an arguments_t
 * @return Returns 0 if non error has been encountered.
 */
int process_file(FILE *file, arguments_t *args);

#endif /* _LOGIC_H */
