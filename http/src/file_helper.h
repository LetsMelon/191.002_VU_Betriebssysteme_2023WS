/**
 * @file file_helper.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.01.2024
 *
 * @brief Contains file helper functions.
 */

#ifndef _FH
#define _FH

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

/**
 * @brief Checks if a file exists at the specified path.
 * @param path The path of the file to check.
 * @return Returns true if the file exists, otherwise false.
 */
bool file_at_path_exists(const char *path);

/**
 * @brief Gets the size of the file.
 * @param file Pointer to the file for which to get the size.
 * @return Returns the size of the file in bytes, or -1 if the size cannot be
 * determined.
 */
long file_size(FILE *file);

/**
 * @brief Combines two file paths into a single path.
 * @param path1 The first part of the path.
 * @param path2 The second part of the path.
 * @param out Pointer to a character pointer that will hold the combined
 * path. Memory will be allocated for the combined path and should be freed by
 * the caller.
 * @return Returns 0 on success, or an error code if combining the paths fails.
 */
int combine_file_paths(const char *path1, const char *path2, char **out);

#endif
