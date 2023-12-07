#include "http.h"

#include <stdlib.h>

void header_free(header_t *header) {
  free(header->name);
  free(header->value);
}

void request_free(request_t *request) {
  if (request->headers != NULL) {
    for (int i = 0; i < request->headers_count; i++) {
      header_free(&request->headers[i]);
    }

    free(request->headers);
  }
  request->headers_count = 0;

  free(request->file);
}

void response_free(response_t *response) {
  if (response->headers != NULL) {
    for (int i = 0; i < response->headers_count; i++) {
      header_free(&response->headers[i]);
    }
    free(response->headers);
  }
  response->headers_count = 0;

  free(response->body);
}
