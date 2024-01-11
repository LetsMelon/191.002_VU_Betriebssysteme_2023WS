#include <limits.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "file_helper.h"
#include "http.h"
#include "parser.h"

static char *USAGE =
    "SYNOPSIS\n\tserver [-p PORT] [-i INDEX] DOC_ROOT\nEXAMPLE\n\tserver -p "
    "1280 -i index.html ~/Documents/my_website/\n";

typedef struct {
  char *port, *index, *doc_root;
} arguments_t;

volatile bool in_shutdown = true;

void handle_exit_loop(int signal) { in_shutdown = false; }

void handle_shutdown(int signal) {
  handle_exit_loop(signal);

  exit(EXIT_SUCCESS);
}

int arguments_parse(int argc, char **argv, arguments_t *args) {
  args->port = "80";
  args->index = "index.html";

  int opt, encountered_p = 0, encountered_i = 0;
  while ((opt = getopt(argc, argv, "p:i:")) != -1) {
    switch (opt) {
    case 'p':
      if (encountered_p > 0) {
        return -1;
      }

      args->port = optarg;

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

int main(int argc, char **argv) {
  arguments_t args;
  if (arguments_parse(argc, argv, &args) != 0) {
    printf("%s", USAGE);

    return EXIT_FAILURE;
  }

  fprintf(stderr, "args: { port = %s, index = '%s', doc_root = '%s' }\n",
          args.port, args.index, args.doc_root);

  struct addrinfo *ai = NULL;
  if (get_addrinfo(NULL, args.port, &ai) < 0) {
    arguments_free(&args);
    return EXIT_FAILURE;
  }

  int sockfd;
  if (create_socket(ai, &sockfd) < 0) {
    freeaddrinfo(ai);
    arguments_free(&args);

    return EXIT_FAILURE;
  }

  if (bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
    close(sockfd);
    freeaddrinfo(ai);
    arguments_free(&args);

    return EXIT_FAILURE;
  }

  if (listen(sockfd, 1) < 0) {
    close(sockfd);
    freeaddrinfo(ai);
    arguments_free(&args);

    return EXIT_FAILURE;
  }

  while (in_shutdown) {
    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    int connection_fd;
    if ((connection_fd = accept(sockfd, ai->ai_addr, &ai->ai_addrlen)) < 0) {
      close(sockfd);
      freeaddrinfo(ai);
      arguments_free(&args);

      return EXIT_FAILURE;
    }
    fprintf(stderr, "Have connection!\n");

    signal(SIGINT, handle_exit_loop);
    signal(SIGTERM, handle_exit_loop);

    FILE *connection = fdopen(connection_fd, "r+");
    if (connection == NULL) {
      in_shutdown = false;

      break;
    }

    request_t request = {
        .method = -1, .version = -1, .file_path = NULL, .hostname = NULL};
    int buffer_size = 512;
    char h_buf[buffer_size];
    bool is_first_request_line = true;
    int read_lines = 0;

    while (fgets(h_buf, buffer_size, connection) != NULL) {
      fprintf(stderr, "DEBUG %d: '", read_lines);
      for (int i = 0; i < buffer_size; i += 1) {
        char c = h_buf[i];

        char *to_print;

        switch (c) {
        case '\0':
          i = buffer_size + 1;
          to_print = "";
          break;
        case '\n':
          to_print = "\\n";
          break;
        case '\t':
          to_print = "\\t";
          break;
        case '\r':
          to_print = "\\r";
          break;
        case '\v':
          to_print = "\\v";
          break;
        case '\f':
          to_print = "\\f";
          break;
        case '\a':
          to_print = "\\a";
          break;
        case '\\':
          to_print = "\\\\";
          break;
        case '\"':
          to_print = "\\\"";
          break;
        default:
          fprintf(stderr, "%c", c);
          to_print = NULL;

          break;
        }

        if (to_print != NULL) {
          fprintf(stderr, "%s", to_print);
        }
      }
      fprintf(stderr, "'\n");

      if (is_first_request_line == true) {
        string_list_t items;
        if (p_split_at(h_buf, ' ', &items) < 0) {

          request.method = -1;
          request.version = -1;
        }

        if (items.num > 2) {
          if (strcmp(items.values[0], "GET") == 0) {
            request.method = REQUEST_GET;
          }

          if (strlen(items.values[1]) == 1 && items.values[1][0] == '/') {
            request.file_path = strdup(args.index);
          } else {
            request.file_path = strdup(items.values[1]);
          }

          if (strcmp(items.values[2], "HTTP/1.1\r\n") == 0) {
            request.version = HTTP_1_1;
          }
        }

        sl_free(&items);

        is_first_request_line = false;
      }

      if (h_buf[0] == '\r' && h_buf[1] == '\n') {
        break;
      }

      read_lines += 1;
    }

    if (request.version != HTTP_1_1) {
      respond_error(connection, STATUS_BAD_REQUEST);
    } else if (request.method != REQUEST_GET) {
      respond_error(connection, STATUS_NOT_IMPLEMENTED);
    } else if (request.file_path != NULL) {
      char *path = NULL;
      combine_file_paths(args.doc_root, request.file_path, &path);

      if (path != NULL && file_at_path_exists(path) == false) {
        respond_error(connection, STATUS_NOT_FOUND);
      } else if (path != NULL) {
        FILE *content = fopen(path, "r");
        long body_size = file_size(content);

        response_t response = {.version = HTTP_1_1,
                               .status_code = STATUS_OK,
                               .body = content,
                               .body_len = body_size};

        respond(connection, response);
        fclose(content);
      }

      if (path != NULL) {
        free(path);
      }
    }

    if (request.file_path != NULL) {
      request_free(&request);
    }

    fflush(connection);
    fclose(connection);

    fprintf(stderr, "Finish!\n");
  }

  close(sockfd);
  freeaddrinfo(ai);
  arguments_free(&args);

  return EXIT_SUCCESS;
}
