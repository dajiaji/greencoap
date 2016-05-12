#include "greencoap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define L(x) (sizeof(x) - 1)

void test_coap_serializer_size() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  size_t size = coap_serializer_size();
  assert(size > 0);
  printf("sizeof(coap_serializer) = %zd\n", size);
  assert(coap_serializer_create(&s, malloc(size), size, buf, 2048) == COAP_OK);
  free(s);
  return;
}

void test_coap_serializer_create() {
  // coap_serializer* s = NULL;
  // char buf[2048] = {};
  // coap_serializer_init(&s, malloc(coap_serializer_size()),
  //                     coap_serializer_size(), buf, 2048);
  // assert(coap_serializer_begin(s, buf, 2048) == COAP_OK);
  // free(s);
  return;
}

void test_coap_serializer_init_request() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON | C_GET, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_POST, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_PUT, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_DELETE, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_GET, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_POST, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_PUT, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_DELETE, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_GET, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_ACK | C_POST, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_ACK | C_PUT, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_ACK | C_DELETE, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_GET, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_POST, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_PUT, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_DELETE, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_serializer_init_response_2xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON | C_CREATED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_DELETED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_VALID, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_CHANGED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_CONTENT, L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_NON | C_CREATED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_DELETED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_VALID, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_CHANGED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_CONTENT, L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_ACK | C_CREATED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_DELETED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_VALID, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_CHANGED, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_CONTENT, L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_RST | C_CREATED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_DELETED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_VALID, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_CHANGED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_CONTENT, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_serializer_init_response_4xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON | C_BAD_REQUEST, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_UNAUTHORIZED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_BAD_OPTION, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_FORBIDDEN, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_NOT_FOUND, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_METHOD_NOT_ALLOWED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_NOT_ACCEPTABLE, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_PRECONDITION_FAILED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_REQUEST_ENTITY_TOO_LARGE,
                              L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_UNSUPPORTED_CONTENT_FORMAT,
                              L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_NON | C_BAD_REQUEST, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_UNAUTHORIZED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_BAD_OPTION, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_FORBIDDEN, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_NOT_FOUND, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_METHOD_NOT_ALLOWED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_NOT_ACCEPTABLE, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_PRECONDITION_FAILED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_REQUEST_ENTITY_TOO_LARGE,
                              L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_UNSUPPORTED_CONTENT_FORMAT,
                              L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_ACK | C_BAD_REQUEST, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_UNAUTHORIZED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_BAD_OPTION, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_FORBIDDEN, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_NOT_FOUND, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_METHOD_NOT_ALLOWED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_NOT_ACCEPTABLE, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_PRECONDITION_FAILED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_REQUEST_ENTITY_TOO_LARGE,
                              L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_UNSUPPORTED_CONTENT_FORMAT,
                              L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_RST | C_BAD_REQUEST, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_UNAUTHORIZED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_BAD_OPTION, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_FORBIDDEN, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_NOT_FOUND, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_METHOD_NOT_ALLOWED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_NOT_ACCEPTABLE, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_PRECONDITION_FAILED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_REQUEST_ENTITY_TOO_LARGE,
                              L("token")) == COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_UNSUPPORTED_CONTENT_FORMAT,
                              L("token")) == COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_serializer_init_response_5xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON | C_INTERNAL_SERVER_ERROR, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_NOT_IMPLEMENTED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_BAD_GATEWAY, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_SERVICE_UNAVAILABLE, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_GATEWAY_TIMEOUT, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_PROXYING_NOT_SUPPORTED,
                              L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_NON | C_INTERNAL_SERVER_ERROR, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_NOT_IMPLEMENTED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_BAD_GATEWAY, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_SERVICE_UNAVAILABLE, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_GATEWAY_TIMEOUT, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON | C_PROXYING_NOT_SUPPORTED,
                              L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_ACK | C_INTERNAL_SERVER_ERROR, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_NOT_IMPLEMENTED, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_BAD_GATEWAY, L("token")) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_SERVICE_UNAVAILABLE, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_GATEWAY_TIMEOUT, L("token")) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK | C_PROXYING_NOT_SUPPORTED,
                              L("token")) == COAP_OK);

  assert(coap_serializer_init(s, T_RST | C_INTERNAL_SERVER_ERROR, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_NOT_IMPLEMENTED, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_BAD_GATEWAY, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_SERVICE_UNAVAILABLE, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_GATEWAY_TIMEOUT, L("token")) ==
         COAP_ERR_INVALID_ARGUMENT);
  assert(coap_serializer_init(s, T_RST | C_PROXYING_NOT_SUPPORTED,
                              L("token")) == COAP_ERR_INVALID_ARGUMENT);
  free(s);
  return;
}

void test_coap_parser_size() {
  coap_parser* p = NULL;
  size_t size = coap_parser_size();
  assert(size > 0);
  printf("sizeof(coap_parser) = %zd\n", size);
  assert(coap_parser_create(&p, malloc(size), size) == COAP_OK);
  free(p);
  return;
}

void test_coap_sample_readme() {
  char buf[64] = {};
  coap_serializer* s = NULL;
  coap_parser* p = NULL;
  const char* payload = NULL;
  size_t payload_len = 0;
  size_t msg_size = 0;

  assert(coap_serializer_create(&s, malloc(coap_serializer_size()),
                                coap_serializer_size(), buf, 64) == COAP_OK);
  assert(coap_serializer_init(s, T_CON | C_POST, L("token")) == COAP_OK);
  assert(coap_serializer_add_opt(s, O_URI_HOST, "example.com",
                                 L("example.com")) == COAP_OK);
  assert(coap_serializer_add_opt_uint(s, O_URI_PORT, 5683) == COAP_OK);
  assert(coap_serializer_add_opt(s, O_URI_PATH, "temperature",
                                 L("temperature")) == COAP_OK);
  assert(coap_serializer_exec(s, 0, "token", "22.3 C", L("22.3 C"),
                              &msg_size) == COAP_OK);
  assert(coap_parser_create(&p, malloc(coap_parser_size()),
                            coap_parser_size()) == COAP_OK);
  assert(coap_parser_exec(p, buf, 64) == COAP_OK);
  assert(coap_parser_get_payload(p, &payload, &payload_len) == COAP_OK);
  assert(payload_len = L("22.3 C"));
  assert(strncmp(payload, "22.3 C", payload_len) == 0);
  free(s);
  free(p);
  return;
}

int main(void) {
  test_coap_serializer_size();
  test_coap_serializer_create();
  test_coap_serializer_init_request();
  test_coap_serializer_init_response_2xx();
  test_coap_serializer_init_response_4xx();
  test_coap_serializer_init_response_5xx();

  test_coap_parser_size();

  test_coap_sample_readme();
  printf("ok.\n");
  return 0;
}
