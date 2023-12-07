/**
 * @file parser.h
 * @author Domenic Melcher <e12220857@student.tuwien.ac.at>
 * @date 12.11.2023
 *
 * @brief Provides utility functions for parsing float complex values
 */

#ifndef _R
#define _R

// https://de.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Geschichte
typedef enum {
  REQUEST_GET,
  REQUEST_POST,
  REQUEST_HEAD,
  REQUEST_PUT,
  REQUEST_PATCH,
  REQUEST_DELETE,
  REQUEST_TRACE,
  REQUEST_OPTIONS,
  REQUEST_CONNECT,
} request_method_e;

typedef enum {
  HTTP_1_0,
  HTTP_1_1,
  HTTP_2,
  HTTP_3,
} http_version_e;

typedef struct {
  char *name, *value;
} header_t;

void header_free(header_t *header);

typedef struct {
  request_method_e method;
  http_version_e version;

  char *file;

  header_t *headers;
  int headers_count;
} request_t;

void request_free(request_t *request);

typedef struct {
  http_version_e version;

  int status_code;
  char *status_description;

  header_t *headers;
  int headers_count;

  char *body;
} response_t;

void response_free(response_t *response);

#endif /* _R */
