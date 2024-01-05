#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "hash_map.h"
#include "http.h"
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

  args->doc_root = malloc(sizeof(char) * PATH_MAX);
  if (args->doc_root == NULL) {
    fprintf(stderr, "Error in alloc\n");
    return -1;
  }

  if (realpath(argv[optind], args->doc_root) == NULL) {
    free(args->doc_root);
    return -1;
  }

  return 0;
}

void arguments_free(arguments_t *args) {
  free(args->doc_root);
  args->doc_root = NULL;
}

int path_combine(const char *p1, const char *p2, char **out) {
  int p1_len = strlen(p1);
  int p2_len = strlen(p2);

  int char_to_add;
  if (p1[p1_len - 1] == '/' || p2[0] == '/') {
    char_to_add = 0;
  } else {
    char_to_add = 1;
  }

  int n = p1_len + p2_len + char_to_add + 1;
  char *tmp_out = malloc(sizeof(char) * n);
  if (tmp_out == NULL) {
    return -1;
  }
  tmp_out[n - 1] = '\0';

  if (strcpy(tmp_out, p1) == NULL) {
    free(tmp_out);
    return -1;
  }

  int write_head = p1_len;
  if (char_to_add == 1) {
    tmp_out[write_head] = '/';
    write_head += 1;
  }

  if (strcpy(tmp_out + sizeof(char) * write_head, p2) == NULL) {
    free(tmp_out);
    return -1;
  }

  *out = tmp_out;
  return 0;
}

int main(int argc, char **argv) {
  arguments_t args;
  if (arguments_parse(argc, argv, &args) != 0) {
    printf("%s", USAGE);

    return EXIT_FAILURE;
  }

  printf("args: port = %d, index = '%s', doc_root = '%s'\n", args.port,
         args.index, args.doc_root);

  header_t headers = {.name = "Host", .value = "www.myhost.at"};

  request_t request = {.method = REQUEST_GET,
                       .version = HTTP_1_1,
                       .file = "/index.html",
                       .headers = &headers,
                       .headers_count = 1};

  char *tmp = NULL;
  path_combine(args.doc_root, request.file, &tmp);
  printf("out: %s\n", tmp);
  free(tmp);

  arguments_free(&args);

  return EXIT_SUCCESS;
}
