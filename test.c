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
  assert(coap_serializer_init(s, T_CON, C_GET, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_POST, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_PUT, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_DELETE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_GET, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_POST, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_PUT, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_DELETE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_GET, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_ACK, C_POST, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_ACK, C_PUT, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_ACK, C_DELETE, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_GET, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_POST, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_PUT, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_DELETE, 1) == COAP_ERR_ARG);
  free(s);
  return;
}

void test_coap_serializer_init_response_2xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON, C_CREATED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_DELETED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_VALID, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_CHANGED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_CONTENT, 1) == COAP_OK);

  assert(coap_serializer_init(s, T_NON, C_CREATED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_DELETED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_VALID, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_CHANGED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_CONTENT, 1) == COAP_OK);

  assert(coap_serializer_init(s, T_ACK, C_CREATED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_DELETED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_VALID, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_CHANGED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_CONTENT, 1) == COAP_OK);

  assert(coap_serializer_init(s, T_RST, C_CREATED, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_DELETED, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_VALID, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_CHANGED, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_CONTENT, 1) == COAP_ERR_ARG);
  free(s);
  return;
}

void test_coap_serializer_init_response_4xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON, C_BAD_REQUEST, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_UNAUTHORIZED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_BAD_OPTION, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_FORBIDDEN, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_NOT_FOUND, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_METHOD_NOT_ALLOWED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_NOT_ACCEPTABLE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_PRECONDITION_FAILED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_REQUEST_ENTITY_TOO_LARGE, 1) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_UNSUPPORTED_CONTENT_FORMAT, 1) ==
         COAP_OK);

  assert(coap_serializer_init(s, T_NON, C_BAD_REQUEST, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_UNAUTHORIZED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_BAD_OPTION, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_FORBIDDEN, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_NOT_FOUND, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_METHOD_NOT_ALLOWED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_NOT_ACCEPTABLE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_PRECONDITION_FAILED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_REQUEST_ENTITY_TOO_LARGE, 1) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_UNSUPPORTED_CONTENT_FORMAT, 1) ==
         COAP_OK);

  assert(coap_serializer_init(s, T_ACK, C_BAD_REQUEST, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_UNAUTHORIZED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_BAD_OPTION, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_FORBIDDEN, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_NOT_FOUND, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_METHOD_NOT_ALLOWED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_NOT_ACCEPTABLE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_PRECONDITION_FAILED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_REQUEST_ENTITY_TOO_LARGE, 1) ==
         COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_UNSUPPORTED_CONTENT_FORMAT, 1) ==
         COAP_OK);

  assert(coap_serializer_init(s, T_RST, C_BAD_REQUEST, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_UNAUTHORIZED, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_BAD_OPTION, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_FORBIDDEN, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_NOT_FOUND, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_METHOD_NOT_ALLOWED, 1) ==
         COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_NOT_ACCEPTABLE, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_PRECONDITION_FAILED, 1) ==
         COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_REQUEST_ENTITY_TOO_LARGE, 1) ==
         COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_UNSUPPORTED_CONTENT_FORMAT, 1) ==
         COAP_ERR_ARG);
  free(s);
  return;
}

void test_coap_serializer_init_response_5xx() {
  coap_serializer* s = NULL;
  char buf[2048] = {};
  coap_serializer_create(&s, malloc(coap_serializer_size()),
                         coap_serializer_size(), buf, 2048);
  assert(coap_serializer_init(s, T_CON, C_INTERNAL_SERVER_ERROR, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_NOT_IMPLEMENTED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_BAD_GATEWAY, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_SERVICE_UNAVAILABLE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_GATEWAY_TIMEOUT, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_PROXYING_NOT_SUPPORTED, 1) ==
         COAP_OK);

  assert(coap_serializer_init(s, T_NON, C_INTERNAL_SERVER_ERROR, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_NOT_IMPLEMENTED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_BAD_GATEWAY, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_SERVICE_UNAVAILABLE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_GATEWAY_TIMEOUT, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_NON, C_PROXYING_NOT_SUPPORTED, 1) ==
         COAP_OK);

  assert(coap_serializer_init(s, T_ACK, C_INTERNAL_SERVER_ERROR, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_NOT_IMPLEMENTED, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_BAD_GATEWAY, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_SERVICE_UNAVAILABLE, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_GATEWAY_TIMEOUT, 1) == COAP_OK);
  assert(coap_serializer_init(s, T_ACK, C_PROXYING_NOT_SUPPORTED, 1) ==
         COAP_OK);

  assert(coap_serializer_init(s, T_RST, C_INTERNAL_SERVER_ERROR, 1) ==
         COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_NOT_IMPLEMENTED, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_BAD_GATEWAY, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_SERVICE_UNAVAILABLE, 1) ==
         COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_GATEWAY_TIMEOUT, 1) == COAP_ERR_ARG);
  assert(coap_serializer_init(s, T_RST, C_PROXYING_NOT_SUPPORTED, 1) ==
         COAP_ERR_ARG);
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
  char token = 0x20;
  coap_serializer* s = NULL;
  coap_parser* p = NULL;
  const char* payload = NULL;
  size_t payload_len = 0;
  size_t msg_size = 0;
  coap_type_t type;
  coap_code_t code;

  assert(coap_serializer_create(&s, malloc(coap_serializer_size()),
                                coap_serializer_size(), buf, 64) == COAP_OK);
  assert(coap_serializer_init(s, T_CON, C_POST, 1) == COAP_OK);
  assert(coap_serializer_add_opt(s, O_URI_HOST, "example.com",
                                 L("example.com")) == COAP_OK);
  assert(coap_serializer_add_opt_uint(s, O_URI_PORT, 5683) == COAP_OK);
  assert(coap_serializer_add_opt(s, O_URI_PATH, "temperature",
                                 L("temperature")) == COAP_OK);
  assert(coap_serializer_exec(s, 0, &token, "22.3 C", L("22.3 C"), &msg_size) ==
         COAP_OK);
  assert(coap_parser_create(&p, malloc(coap_parser_size()),
                            coap_parser_size()) == COAP_OK);
  assert(coap_parser_exec(p, buf, 64) == COAP_OK);
  assert(coap_parser_get_payload(p, &payload, &payload_len) == COAP_OK);
  assert(payload_len = L("22.3 C"));
  assert(strncmp(payload, "22.3 C", payload_len) == 0);
  assert(coap_parser_get_type(p, &type) == COAP_OK);
  assert(type == T_CON);
  assert(coap_parser_get_code(p, &code) == COAP_OK);
  assert(code == C_POST);
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
