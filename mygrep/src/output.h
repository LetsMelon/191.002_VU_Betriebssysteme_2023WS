/**
 * @file output.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions to write to the correct output (stdout or a
 * file)
 */

#ifndef _OUTPUT_H
#define _OUTPUT_H

/**
 * Initializes the output for stdout.
 * @brief This functions does not check if it or 'output_init_file' has been
 * called before, so if this happens you have to 'free' the state beforehand via
 * 'output_free'.
 */
void output_init_stdout(void);

/**
 * Initializes the output to go to the given file.
 * @brief This function creates and opens the given filepath as a file. This
 * functions does not check if it or 'output_init_stdout' has been called
 * before, so if this happens you have to 'free' the state beforehand via
 * 'output_free'.
 * @param file_path path for the output file
 * @return Returns 0 if non error has been encountered.
 */
int output_init_file(const char *file_path);

/**
 * Mimics what printf from stdio does.
 * @return Returns 0 if non error has been encountered.
 */
int output_printf(const char *format, ...);

/**
 * Frees the internal state.
 * @brief This function only closes the open file when the state was
 * 'output_init_file'.
 */
void output_free(void);

#endif /* _OUTPUT_H */
