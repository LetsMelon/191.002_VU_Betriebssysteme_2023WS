/**
 * @file http.c
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 09.01.2024
 *
 * @brief Provides utility functions for a http server
 */

#include "http.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

void request_free(request_t *request) {
  if (request->file_path != NULL) {
    free(request->file_path);
  }

  if (request->hostname != NULL) {
    free(request->hostname);
  }
}

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
  setsockopt(tmp_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  *sockfd = tmp_sockfd;

  return 0;
}

/**
 * @brief Converts an HTTP status code to its corresponding textual
 * representation.
 * @param status The HTTP status code enumeration.
 * @return Returns the textual representation of the status code.
 */
static const char *status_to_text(status_code_e *status) {
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
    // fprintf(stderr, "DEBUG: Status code not implemented in switch: %d",
    //         (int)*status);

    *status = STATUS_INTERNAL_SERVER_ERROR;

    return "Internal Server Error";
  }
}

/**
 * @brief Converts the current time to RFC822 format and stores it in the
 * provided buffer.
 *
 * This function retrieves the current time, converts it to the RFC822 date-time
 * format, and stores it in the provided buffer.
 *
 * @param buffer Pointer to the buffer where the RFC822-formatted time will be
 * stored.
 * @param size Size of the buffer.
 * @return Returns the length of the formatted string (excluding the null
 * terminator) on success, or -1 on failure.
 *
 * @note The caller is responsible for ensuring that the buffer has sufficient
 * space to store the formatted time.
 */
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

  const char *status_text = status_to_text(&response.status_code);

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

int request(FILE *socket, request_t request) {
  if (request.version != HTTP_1_1 || request.method != REQUEST_GET) {
    request_free(&request);
    return -1;
  }

  fprintf(socket,
          "GET %s HTTP/1.1\r\n"
          "Host: %s\r\n"
          "Connection: close\r\n"
          "\r\n",
          request.file_path, request.hostname);

  fflush(socket);
  request_free(&request);

  return 0;
}

status_code_e status_code_from_int(int value) {
  if (value == 100 || value == 101 || value == 102 || value == 103 ||
      value == 200 || value == 201 || value == 202 || value == 203 ||
      value == 204 || value == 205 || value == 206 || value == 207 ||
      value == 208 || value == 226 || value == 300 || value == 301 ||
      value == 302 || value == 303 || value == 304 || value == 305 ||
      value == 306 || value == 307 || value == 308 || value == 400 ||
      value == 401 || value == 402 || value == 403 || value == 404 ||
      value == 405 || value == 406 || value == 407 || value == 408 ||
      value == 409 || value == 410 || value == 411 || value == 412 ||
      value == 413 || value == 414 || value == 415 || value == 416 ||
      value == 417 || value == 421 || value == 422 || value == 423 ||
      value == 424 || value == 425 || value == 426 || value == 428 ||
      value == 429 || value == 431 || value == 451 || value == 500 ||
      value == 501 || value == 502 || value == 503 || value == 504 ||
      value == 505 || value == 506 || value == 507 || value == 508 ||
      value == 509 || value == 510 || value == 511) {
    return (status_code_e)value;
  } else {
    return -1;
  }
}
