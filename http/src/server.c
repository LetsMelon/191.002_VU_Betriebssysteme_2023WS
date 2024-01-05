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

  hash_map_t hash_map;
  hm_map_init(&hash_map);

  header_t headers = {.name = "Host", .value = "www.myhost.at"};

  request_t request = {.method = REQUEST_GET,
                       .version = HTTP_1_1,
                       .file = "/index.html",
                       .headers = &headers,
                       .headers_count = 1};

  // request_free(&request);

  hash_map_entry_t item;
  if (hm_map_get(&hash_map, request.file, &item) < 0) {
    // TODO read_file
    printf("READ FILE FROM FS - '%s'\n", request.file);

    char *path = "/Users/domenic/Documents/Programming/"
                 "191.002_VU_Betriebssysteme_2023WS/http/root/index.html";

    FILE *file = fopen(path, "r");
    if (file == NULL) {
      // TODO send error
      return EXIT_FAILURE;
    }

    struct stat file_info;
    stat(path, &file_info);

    // Format last modified time according to HTTP date format
    // char lastModifiedTime[64];
    // strftime(lastModifiedTime, sizeof(lastModifiedTime),
    //          "%a, %d %b %Y %H:%M:%S GMT", gmtime(&file_info.st_mtime));
    //
    // printf("stat: st_size = %lld, lastModifiedTime = '%s'\n",
    // file_info.st_size,
    //        lastModifiedTime);

    long file_size = file_info.st_size;
    char *file_content = malloc(file_size + 1);
    if (file_content == NULL) {
      // TODO send error
      fclose(file);

      return EXIT_FAILURE;
    }

    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';

    fclose(file);

    hash_map_entry_t tmp_item;
    if (hm_entry_init(request.file, file_content, (long)file_info.st_mtime,
                      &tmp_item) < 0) {
      free(file_content);
      hm_map_free(&hash_map);
      arguments_free(&args);

      return EXIT_FAILURE;
    }
    free(file_content);

    if (hm_map_add(&hash_map, tmp_item) < 0) {
      hm_entry_free(&tmp_item);
      hm_map_free(&hash_map);
      arguments_free(&args);

      return EXIT_FAILURE;
    }
    if (hm_map_get(&hash_map, request.file, &item) < 0) {
      hm_map_free(&hash_map);
      arguments_free(&args);

      return EXIT_FAILURE;
    }

  } else {
    printf("ALREADY CACHED\n");
    // item has everything
  }

  printf("lm = %s\n", ctime(&item.last_modified));

  hm_entry_free(&item);

  /*
  {
    hash_map_entry_t item;
    hm_entry_init("Domenic Melcher", "<h1>title</h1>", &item);
    hm_map_add(&hash_map, item);
  }

  {
    hash_map_entry_t item;
    printf("get: %d, ", hm_map_get(&hash_map, "Domenic Melcher", &item));
    printf("name = '%s', content = '%s'\n", item.name, item.content);
    hm_entry_free(&item);
  }

  printf("remove: %d\n", hm_map_remove(&hash_map, "Domenic Melcher"));
  */
  hm_map_free(&hash_map);
  arguments_free(&args);

  return EXIT_SUCCESS;
}
