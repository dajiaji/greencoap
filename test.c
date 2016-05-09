#include "greencoap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void test_gcoap_serializer_size() {
  size_t size = gcoap_serializer_size();
  assert(size > 0);
  printf("sizeof(gcoap_serializer) = %zd\n", size);
  void* buf = malloc(size);
  assert(buf != NULL);
  gcoap_serializer* s = NULL;
  assert(gcoap_serializer_init(&s, buf, size) == GCOAP_OK);
  free(s);
  return;
}

void test_gcoap_parser_size() {
  size_t size = gcoap_parser_size();
  assert(size > 0);
  printf("sizeof(gcoap_parser) = %zd\n", size);
  void* buf = malloc(size);
  assert(buf != NULL);
  gcoap_parser* p = NULL;
  assert(gcoap_parser_init(&p, buf, size) == GCOAP_OK);
  free(p);
  return;
}

void test_gcoap_sample_readme() {
  char buf[2048] = {};
  gcoap_serializer* s = NULL;
  gcoap_parser* p = NULL;

  assert(gcoap_serializer_init(&s, malloc(gcoap_serializer_size()),
                               gcoap_serializer_size()) == GCOAP_OK);
  assert(gcoap_serializer_begin(s, buf, 2048) == GCOAP_OK);
  assert(gcoap_serializer_set_header(s, T_CON | C_GET, "token",
                                     sizeof("token") - 1) == GCOAP_OK);
  assert(gcoap_serializer_add_opt_string(s, O_URI_HOST, "example.com",
                                         sizeof("example.com") - 1) ==
         GCOAP_OK);
  assert(gcoap_serializer_add_opt_uint(s, O_URI_PORT, 5683) == GCOAP_OK);
  assert(gcoap_serializer_add_opt_string(s, O_URI_PATH, "/temperature",
                                         sizeof("/temperature") - 1) ==
         GCOAP_OK);

  assert(gcoap_parser_init(&p, malloc(gcoap_parser_size()),
                           gcoap_parser_size()) == GCOAP_OK);
  assert(gcoap_parser_exec(p, buf, 2048) == GCOAP_OK);

  free(s);
  free(p);
  return;
}

int main(void) {
  test_gcoap_serializer_size();
  test_gcoap_parser_size();
  test_gcoap_sample_readme();
  printf("ok.\n");
  return 0;
}
