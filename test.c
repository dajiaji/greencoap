#include "greencoap.h"
#include <assert.h>
#include <stdio.h>

void test_serizalize_and_parse() {
  gcoap_serializer_t* s = NULL;
  gcoap_parser_t* p = NULL;
  assert(gcoap_serializer_create(&s) == GCOAP_OK);
  assert(gcoap_serializer_init(s, T_CON | C_GET, "token", 5, NULL, 0) ==
         GCOAP_OK);
  assert(gcoap_parser_create(&p) == GCOAP_OK);
  assert(gcoap_serializer_destroy(s) == GCOAP_OK);
  assert(gcoap_parser_destroy(p) == GCOAP_OK);
  return;
}

int main(void) {
  test_serizalize_and_parse();
  printf("ok.\n");
  return 0;
}
