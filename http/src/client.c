#include <limits.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_helper.h"
#include "hash_map.h"
#include "http.h"
#include "parser.h"

static char *USAGE = "SYNOPSIS\n"
                     "\tclient [-p PORT] [ -o FILE | -d DIR ] URL\n"
                     "EXAMPLE\n"
                     "\tclient http://www.example.com/\n";

typedef struct {
  char *port, *file, *dir, *url;
} arguments_t;

static int arguments_parse(int argc, char **argv, arguments_t *args) {
  args->port = "80";
  args->file = NULL;
  args->dir = NULL;

  int opt, encountered_p = 0, encountered_o = 0, encountered_d = 0;
  while ((opt = getopt(argc, argv, "p:o:d:")) != -1) {
    switch (opt) {
    case 'p':
      if (encountered_p > 0) {
        return -1;
      }
      args->port = optarg;
      encountered_p += 1;

      break;
    case 'o':
      if (encountered_o > 0 || encountered_d > 0) {
        return -1;
      }
      args->file = optarg;
      encountered_o += 1;

      break;
    case 'd':
      if (encountered_o > 0 || encountered_d > 0) {
        return -1;
      }
      args->dir = optarg;
      encountered_d += 1;

      break;
    default:
      fprintf(stderr, "Unknown argument: '%c'\n", (char)opt);
      return -1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Missing URL\n");
    return -1;
  }

  args->url = argv[optind];

  return 0;
}

static bool str_starts_with(const char *input, const char *search_pattern) {
  return memcmp(input, search_pattern, strlen(search_pattern)) == 0;
}

int main(int argc, char **argv) {
  arguments_t args;
  if (arguments_parse(argc, argv, &args) != 0) {
    printf("%s", USAGE);

    return EXIT_FAILURE;
  }

  if (str_starts_with(args.url, "http://") == false) {
    printf("%s", USAGE);

    return EXIT_FAILURE;
  }

  fprintf(stderr, "args: { port = %s, file = '%s', dir = '%s', url = '%s' }\n",
          args.port, args.file == NULL ? "" : args.file,
          args.dir == NULL ? "" : args.dir, args.url);

  int hostname_start = strlen("http://"), hostname_stop = hostname_start;
  int filepath_start = hostname_stop, filepath_stop = strlen(args.url);
  int filename_start = hostname_stop, filename_stop = strlen(args.url);

  bool encountered_hostname = false;
  for (int i = hostname_start; i < strlen(args.url); i += 1) {
    char c = args.url[i];

    if (c == ';' || c == '/' || c == '?' || c == ':' || c == '@' || c == '=' ||
        c == '&') {
      encountered_hostname = true;

      hostname_stop = i;
      filepath_start = i;
      filename_start = i;

      for (int i = filepath_start; i < filepath_stop; i += 1) {
        if (args.url[i] == '/') {
          filename_start = i;
        } else if (args.url[i] == '?') {
          filename_stop = i;
          break;
        }
      }

      break;
    }
  }

  if (encountered_hostname == false) {
    return EXIT_FAILURE;
  }

  int hostname_len = hostname_stop - hostname_start;
  int filepath_len = filepath_stop - filepath_start;
  int filename_len = filename_stop - filename_start;

  char *hostname = calloc(hostname_len + 1, sizeof(char));
  char *filepath = calloc(filepath_len + 1, sizeof(char));
  char *filename = calloc(filename_len + 1, sizeof(char));

  if (hostname == NULL || filepath == NULL || filename == NULL) {
    if (hostname != NULL) {
      free(hostname);
    }

    if (filepath != NULL) {
      free(filepath);
    }

    if (filename != NULL) {
      free(filename);
    }

    return EXIT_FAILURE;
  }

  stpncpy(hostname, &args.url[hostname_start], hostname_len);
  stpncpy(filepath, &args.url[filepath_start], filepath_len);
  if (args.file == NULL) {
    stpncpy(filename, &args.url[filename_start], filename_len);

    if (filename_len == 1) {
      free(filename);

      filename = strdup("/index.html");

      if (filename == NULL) {
        free(hostname);
        free(filepath);

        return EXIT_FAILURE;
      }
    }

    if (filename[0] == '/') {
      char *tmp_filename = calloc(filename_len + 2, sizeof(char));
      tmp_filename[0] = '.';
      strcpy(&tmp_filename[1], filename);
      free(filename);
      filename = tmp_filename;
    }
  } else {
    filename = strdup(args.file);

    if (filename == NULL) {
      free(hostname);
      free(filepath);

      return EXIT_FAILURE;
    }
  }

  fprintf(stderr, "HOSTNAME: '%s'\n", hostname);
  fprintf(stderr, "FILEPATH: '%s'\n", filepath);
  fprintf(stderr, "FILENAME: '%s'\n", filename);

  struct addrinfo *ai = NULL;
  if (get_addrinfo(hostname, args.port, &ai) < 0) {
    free(hostname);
    free(filepath);

    return EXIT_FAILURE;
  }

  fprintf(stderr, "Created socket\n");

  int sockfd;
  if (create_socket(ai, &sockfd) < 0) {
    freeaddrinfo(ai);

    return EXIT_FAILURE;
  }

  fprintf(stderr, "Connect\n");
  connect(sockfd, ai->ai_addr, ai->ai_addrlen);

  FILE *connection = fdopen(sockfd, "r+");

  freeaddrinfo(ai);

  fprintf(stderr, "Request\n");
  request_t tmp_request = {.method = REQUEST_GET,
                           .version = HTTP_1_1,
                           .file_path = filepath,
                           .hostname = hostname};
  request(connection, tmp_request);

  fprintf(stderr, "Response\n");
  int buffer_size = 512;
  char h_buf[buffer_size];
  int read_lines = 0;
  bool is_body = false;

  FILE *output = NULL;
  if (args.file != NULL) {
    fprintf(stderr, "out file: '%s'\n", filename);

    output = fopen(filename, "w");
  } else if (args.dir != NULL) {
    char *tmp_path = NULL;
    combine_file_paths(args.dir, filename, &tmp_path);

    fprintf(stderr, "out dir: '%s'\n", tmp_path);

    output = fopen(tmp_path, "w");

    free(tmp_path);
  } else {
    output = stdout;
  }

  free(filename);

  if (output == NULL) {
    fclose(connection);
    close(sockfd);

    return EXIT_FAILURE;
  }

  while (fgets(h_buf, buffer_size, connection) != NULL) {
    fprintf(stderr, "DEBUG %c %d:\t'", is_body == true ? 'b' : 'h', read_lines);
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

    if (read_lines == 0) {
      string_list_t items;
      if (p_split_at(h_buf, ' ', &items) < 0) {
        fclose(connection);
        close(sockfd);

        return EXIT_FAILURE;
      }

      if (items.num < 3) {
        sl_free(&items);

        fclose(connection);
        close(sockfd);

        return EXIT_FAILURE;
      }

      long raw_status = strtol(items.values[1], NULL, 10);
      if (strcmp(items.values[0], "HTTP/1.1") != 0 || raw_status < 1) {
        fprintf(stderr, "Protocol error!");

        sl_free(&items);

        fclose(connection);
        close(sockfd);

        return 2;
      }

      sl_free(&items);

      status_code_e status = status_code_from_int((int)raw_status);
      if (status != STATUS_OK) {
        fprintf(stderr, "%d %s\n", (int)status, status_to_text(&status));

        fclose(connection);
        close(sockfd);

        return 3;
      }
    }

    if (h_buf[0] == '\r' && h_buf[1] == '\n') {
      is_body = true;
    } else if (is_body == true) {
      fputs(h_buf, output);
    }

    read_lines += 1;
  }

  fprintf(stderr, "Clean-up\n");

  fclose(connection);
  close(sockfd);
  fclose(output);

  return EXIT_SUCCESS;
}
