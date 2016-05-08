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

// void test_gcoap_serialize_and_parse() {
//  gcoap_serializer* s = NULL;
//  gcoap_parser* p = NULL;
//  assert(gcoap_serializer_create(&s, malloc(gcoap_serializer_size()),
//                                 gcoap_serializer_size()) == GCOAP_OK);
//  assert(gcoap_serializer_init(s, T_CON | C_GET, "token", 5) == GCOAP_OK);
//  assert(gcoap_parser_create(&p, p_workspace, 512) == GCOAP_OK);
//  assert(gcoap_serializer_destroy(s) == GCOAP_OK);
//  assert(gcoap_parser_destroy(p) == GCOAP_OK);
//  return;
//}

int main(void) {
  test_gcoap_serializer_size();
  test_gcoap_parser_size();
  // test_serizalize_and_parse();
  printf("ok.\n");
  return 0;
}
