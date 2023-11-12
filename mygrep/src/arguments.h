/**
 * @file arguments.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions to store the executable arguments.
 */

#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

typedef struct arguments {
  char *output_file; ///< path to output file

  char *keyword; ///< keyword to search for

  bool case_sensitive; ///< 'true' if the search is case sensitive, default:
                       ///< 'true'

  char **input_files;       ///< array of paths of input files
  int input_files_num;      ///< how many input files there are
  int input_files_capacity; ///< the capacity of the 'dynamic' list of input
                            ///< files
} arguments_t;

/**
 * Modifies the input by transforming it to lowercase
 * @param str input to modify
 */
void to_lowercase(char *str);

/**
 * Initializes the given arguments.
 * @brief Initializes the arguments_t.
 * @param arg the arguments_t that should be initialized.
 */
void arguments_init(arguments_t *arg);

/**
 * Frees the given argument_t.
 * @brief Frees the arguments_t.
 * @param arg the arguments_t that should be freed.
 */
void arguments_free(arguments_t *arg);

/**
 * Prints the arg to stdout.
 * @param arg the arguments_t.
 */
void arguments_print(arguments_t *arg);

/**
 * Parses an argument_t via argc and argv from main.
 * @brief Parses an argument_t via argc and argv.
 * @param arg where to parse the arguments into.
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns 0 if non error has been encountered
 */
int arguments_parse(arguments_t *arg, int argc, char **argv);

#endif /* _ARGUMENTS_H */
