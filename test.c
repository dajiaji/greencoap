#include "greencoap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
                                    sizeof("token") - 1) == COAP_OK);
  assert(coap_serializer_add_opt_string(s, O_URI_HOST, "example.com",
                                        sizeof("example.com") - 1) == COAP_OK);
  assert(coap_serializer_add_opt_uint(s, O_URI_PORT, 5683) == COAP_OK);
  assert(coap_serializer_add_opt_string(s, O_URI_PATH, "/temperature",
                                        sizeof("/temperature") - 1) == COAP_OK);

  assert(coap_parser_init(&p, malloc(coap_parser_size()), coap_parser_size()) ==
         COAP_OK);
  assert(coap_parser_exec(p, buf, 2048) == COAP_OK);

  free(s);
  free(p);
  return;
}

int main(void) {
  test_coap_serializer_size();
  test_coap_parser_size();
  test_coap_sample_readme();
  printf("ok.\n");
  return 0;
}
