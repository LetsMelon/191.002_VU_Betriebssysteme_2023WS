/**
 * @file main.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Main program module.
 *
 * mygrep
 **/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "logic.h"
#include "output.h"

const char *USAGE =
    "SYNOPSIS\n\tmygrep[-i][-o outfile] keyword [file...]\n"; /**< Usage message
                                                                 for this
                                                                 program */

/**
 * Program entry point.
 * @brief The program starts here.
 * @details The program starts here.
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS if no error occurred while running.
 */
int main(int argc, char **argv) {
  arguments_t args;
  arguments_init(&args);

  if (arguments_parse(&args, argc, argv) == -1) {
    fprintf(stderr, "%s\n%s", argv[0], USAGE);
    return EXIT_FAILURE;
  }

  if (args.output_file == NULL) {
    output_init_stdout();
  } else {
    if (output_init_file(args.output_file) != 0) {
      fprintf(stderr, "%s\nError while trying to initialize output file.\n",
              argv[0]);
      return EXIT_FAILURE;
    }
  }

  // arguments_print(&args);

  if (args.input_files_num == 0) {
    if (process_file(stdin, &args) != 0) {
      fprintf(stderr, "%s\nError while trying to read from stdin.\n", argv[0]);
      return EXIT_FAILURE;
    }
  } else {
    if (process_files(&args) != 0) {
      fprintf(stderr, "%s\nError while trying to read from the given files.\n",
              argv[0]);
      return EXIT_FAILURE;
    }
  }

  arguments_free(&args);
  output_free();

  return EXIT_SUCCESS;
}
