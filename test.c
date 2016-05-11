#include "greencoap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define L(x) (sizeof(x) - 1)

void test_coap_serializer_size() {
  size_t size = coap_serializer_size();
  assert(size > 0);
  printf("sizeof(coap_serializer) = %zd\n", size);
  void* buf = malloc(size);
  assert(buf != NULL);
  coap_serializer* s = NULL;
  assert(coap_serializer_init(&s, buf, size) == COAP_OK);
  free(s);
  return;
}

void test_coap_serializer_begin() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_init(&s, malloc(coap_serializer_size()),
                       coap_serializer_size());
  assert(coap_serializer_begin(s, buf, 2048) == COAP_OK);
  free(s);
  return;
}

void test_coap_serializer_set_header_request() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_init(&s, malloc(coap_serializer_size()),
                       coap_serializer_size());
  coap_serializer_begin(s, buf, 2048);
  assert(coap_serializer_set_header(s, T_CON | C_GET, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_POST, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_PUT, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_DELETE, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_GET, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_POST, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_PUT, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_DELETE, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_GET, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_ACK | C_POST, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_ACK | C_PUT, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_ACK | C_DELETE, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_GET, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_POST, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_PUT, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_DELETE, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_serializer_set_header_response_2xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_init(&s, malloc(coap_serializer_size()),
                       coap_serializer_size());
  coap_serializer_begin(s, buf, 2048);
  assert(coap_serializer_set_header(s, T_CON | C_CREATED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_DELETED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_VALID, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_CHANGED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_CONTENT, "token",
                                    L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_NON | C_CREATED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_DELETED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_VALID, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_CHANGED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_CONTENT, "token",
                                    L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_ACK | C_CREATED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_DELETED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_VALID, "token", L("token")) ==
         COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_CHANGED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_CONTENT, "token",
                                    L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_RST | C_CREATED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_DELETED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_VALID, "token", L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_CHANGED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_CONTENT, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_serializer_set_header_response_4xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_init(&s, malloc(coap_serializer_size()),
                       coap_serializer_size());
  coap_serializer_begin(s, buf, 2048);
  assert(coap_serializer_set_header(s, T_CON | C_BAD_REQUEST, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_UNAUTHORIZED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_BAD_OPTION, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_FORBIDDEN, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_NOT_FOUND, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_METHOD_NOT_ALLOWED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_NOT_ACCEPTABLE, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_PRECONDITION_FAILED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_REQUEST_ENTITY_TOO_LARGE,
                                    "token", L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_UNSUPPORTED_CONTENT_FORMAT,
                                    "token", L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_NON | C_BAD_REQUEST, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_UNAUTHORIZED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_BAD_OPTION, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_FORBIDDEN, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_NOT_FOUND, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_METHOD_NOT_ALLOWED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_NOT_ACCEPTABLE, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_PRECONDITION_FAILED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_REQUEST_ENTITY_TOO_LARGE,
                                    "token", L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_UNSUPPORTED_CONTENT_FORMAT,
                                    "token", L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_ACK | C_BAD_REQUEST, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_UNAUTHORIZED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_BAD_OPTION, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_FORBIDDEN, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_NOT_FOUND, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_METHOD_NOT_ALLOWED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_NOT_ACCEPTABLE, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_PRECONDITION_FAILED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_REQUEST_ENTITY_TOO_LARGE,
                                    "token", L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_UNSUPPORTED_CONTENT_FORMAT,
                                    "token", L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_RST | C_BAD_REQUEST, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_UNAUTHORIZED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_BAD_OPTION, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_FORBIDDEN, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_NOT_FOUND, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_METHOD_NOT_ALLOWED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_NOT_ACCEPTABLE, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_PRECONDITION_FAILED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_REQUEST_ENTITY_TOO_LARGE,
                                    "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_UNSUPPORTED_CONTENT_FORMAT,
                                    "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_serializer_set_header_response_5xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_init(&s, malloc(coap_serializer_size()),
                       coap_serializer_size());
  coap_serializer_begin(s, buf, 2048);
  assert(coap_serializer_set_header(s, T_CON | C_INTERNAL_SERVER_ERROR, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_NOT_IMPLEMENTED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_BAD_GATEWAY, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_SERVICE_UNAVAILABLE, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_GATEWAY_TIMEOUT, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_PROXYING_NOT_SUPPORTED,
                                    "token", L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_NON | C_INTERNAL_SERVER_ERROR, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_NOT_IMPLEMENTED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_BAD_GATEWAY, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_SERVICE_UNAVAILABLE, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_GATEWAY_TIMEOUT, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_NON | C_PROXYING_NOT_SUPPORTED,
                                    "token", L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_ACK | C_INTERNAL_SERVER_ERROR, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_NOT_IMPLEMENTED, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_BAD_GATEWAY, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_SERVICE_UNAVAILABLE, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_GATEWAY_TIMEOUT, "token",
                                    L("token")) == COAP_OK);
  assert(coap_serializer_set_header(s, T_ACK | C_PROXYING_NOT_SUPPORTED,
                                    "token", L("token")) == COAP_OK);

  assert(coap_serializer_set_header(s, T_RST | C_INTERNAL_SERVER_ERROR, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_NOT_IMPLEMENTED, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_BAD_GATEWAY, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_SERVICE_UNAVAILABLE, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_GATEWAY_TIMEOUT, "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_set_header(s, T_RST | C_PROXYING_NOT_SUPPORTED,
                                    "token",
                                    L("token")) == COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_parser_size() {
  size_t size = coap_parser_size();
  assert(size > 0);
  printf("sizeof(coap_parser) = %zd\n", size);
  void* buf = malloc(size);
  assert(buf != NULL);
  coap_parser* p = NULL;
  assert(coap_parser_init(&p, buf, size) == COAP_OK);
  free(p);
  return;
}

void test_coap_sample_readme() {
  char buf[2048] = {};
  coap_serializer* s = NULL;
  coap_parser* p = NULL;

  assert(coap_serializer_init(&s, malloc(coap_serializer_size()),
                              coap_serializer_size()) == COAP_OK);
  assert(coap_serializer_begin(s, buf, 2048) == COAP_OK);
  assert(coap_serializer_set_header(s, T_CON | C_GET, "token",
                                    L("token") - 1) == COAP_OK);
  assert(coap_serializer_add_opt_string(s, O_URI_HOST, "example.com",
                                        L("example.com")) == COAP_OK);
  assert(coap_serializer_add_opt_uint(s, O_URI_PORT, 5683) == COAP_OK);
  assert(coap_serializer_add_opt_string(s, O_URI_PATH, "temperature",
                                        L("temperature")) == COAP_OK);
  assert(coap_parser_init(&p, malloc(coap_parser_size()), coap_parser_size()) ==
         COAP_OK);
  assert(coap_parser_exec(p, buf, 2048) == COAP_OK);

  free(s);
  free(p);
  return;
}

int main(void) {
  test_coap_serializer_size();
  test_coap_serializer_begin();
  test_coap_serializer_set_header_request();
  test_coap_serializer_set_header_response_2xx();
  test_coap_serializer_set_header_response_4xx();
  test_coap_serializer_set_header_response_5xx();

  test_coap_parser_size();

  test_coap_sample_readme();
  printf("ok.\n");
  return 0;
}
