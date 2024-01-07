#include "http.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

void request_free(request_t *request) { free(request->file_path); }

void response_free(response_t *response) {}

int get_addrinfo(const char *addr, const char *port, struct addrinfo **out) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(addr, port, &hints, out) != 0) {
    return -1;
  }

  return 0;
}

int create_socket(const struct addrinfo *info, int *sockfd) {
  int tmp_sockfd =
      socket(info->ai_family, info->ai_socktype, info->ai_protocol);
  if (tmp_sockfd < 0) {
    return -1;
  }

  int optval = 1;
  setsockopt(tmp_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

  *sockfd = tmp_sockfd;

  return 0;
}

static char *status_to_text(status_code_e *status) {
  switch (*status) {
  case STATUS_OK:
    return "OK";

  case STATUS_BAD_REQUEST:
    return "Bad Request";

  case STATUS_NOT_IMPLEMENTED:
    return "Not implemented";

  case STATUS_NOT_FOUND:
    return "Not Found";

  default:
    fprintf(stderr, "DEBUG: Status code not implemented in switch: %d",
            (int)*status);

    *status = STATUS_INTERNAL_SERVER_ERROR;

    return "Internal Server Error";
  }
}

static int time_as_rfc822(char *buffer, int size) {
  time_t tmp_time;
  if (time(&tmp_time) == (time_t)-1) {
    return -1;
  }

  struct tm *info = gmtime(&tmp_time);
  if (info == NULL) {
    return -1;
  }

  return (int)strftime(buffer, size, "%a, %d %b %g %T GMT", info);
}

int respond(FILE *socket, response_t response) {
  if (response.version != HTTP_1_1) {
    response_free(&response);

    int code = respond_error(socket, STATUS_INTERNAL_SERVER_ERROR);
    return code < 0 ? code : 1;
  }

  char *status_text = status_to_text(&response.status_code);

  fprintf(socket, "HTTP/1.1 %d %s\r\n", (int)response.status_code, status_text);

  if (response.status_code == STATUS_OK) {
    char time_header_buffer[64];
    if (time_as_rfc822(time_header_buffer, 64) < 0) {
      return -1;
    }

    fprintf(socket, "Date: %s\r\n", time_header_buffer);
  }

  if (response.body != NULL) {
    fprintf(socket,
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: %ld\r\n",
            response.body_len);
  }

  fprintf(socket, "Connection: close\r\n\r\n");

  if (fflush(socket) < 0) {
    response_free(&response);

    return -1;
  }

  if (response.body != NULL) {
    int file_reading = 1024;
    char buffer[file_reading];

    while (fgets(buffer, file_reading, response.body) != NULL) {
      fputs(buffer, socket);
      memset(buffer, 0, file_reading);
    }
  }

  response_free(&response);

  if (fflush(socket) < 0) {
    return -1;
  }

  return 0;
}

int respond_error(FILE *socket, status_code_e status) {
  response_t response = {
      .version = HTTP_1_1, .status_code = status, .body = NULL, .body_len = 0};

  return respond(socket, response);
}
