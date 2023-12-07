#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"

static char *USAGE =
    "SYNOPSIS\n\tserver [-p PORT] [-i INDEX] DOC_ROOT\nEXAMPLE\n\tserver -p "
    "1280 -i index.html ~/Documents/my_website/\n";

typedef struct {
  int port;
  char *index, *doc_root;
} arguments_t;

int arguments_parse(int argc, char **argv, arguments_t *args) {
  args->port = 80;
  args->index = "index.html";

  int opt, encountered_p = 0, encountered_i = 0;
  while ((opt = getopt(argc, argv, "p:i:")) != -1) {
    switch (opt) {
    case 'p':
      if (encountered_p > 0) {
        return -1;
      }

      if (p_parse_as_int(optarg, &args->port) < 0) {
        return -1;
      }

      encountered_p += 1;

      break;
    case 'i':
      if (encountered_i > 0) {
        return -1;
      }

      args->index = optarg;

      encountered_i += 1;

      break;
    default:
      fprintf(stderr, "Unknown argument: '%c'\n", (char)opt);
      return -1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Missing DOC_ROOT\n");
    return -1;
  }

  args->doc_root = argv[optind];

  return 0;
}
void arguments_free(arguments_t *args) {}

int main(int argc, char **argv) {
  arguments_t args;
  if (arguments_parse(argc, argv, &args) != 0) {
    printf("%s", USAGE);

    return EXIT_FAILURE;
  };

  printf("args: port = %d, index = '%s', doc_root = '%s'\n", args.port,
         args.index, args.doc_root);

  arguments_free(&args);

  return EXIT_SUCCESS;
}
