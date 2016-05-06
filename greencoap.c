#include "greencoap_i.h"
//#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int gcoap_serializer_create(gcoap_serializer_t** s) {
  *s = calloc(1, sizeof(gcoap_serializer_t));
  if (*s == NULL) {
    return GCOAP_ERR_SYSTEM;
  }
  printf("sizeof(gcoap_serializer_t) = %zd\n", sizeof(gcoap_serializer_t));
  (*s)->capacity = GCOAP_CAPACITY_VARIABLE;
  return GCOAP_OK;
}

int gcoap_fixed_size_serializer_create(gcoap_serializer_t** s, const char* buf,
                                       size_t len) {
  if (buf == NULL || sizeof(gcoap_serializer_t) > len) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  printf("sizeof(gcoap_serializer_t) = %zd\n", sizeof(gcoap_serializer_t));
  *s = (gcoap_serializer_t*)buf;
  (*s)->capacity = len;
  return GCOAP_OK;
}

int gcoap_serializer_destroy(gcoap_serializer_t* s) {
  if (s == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (s->capacity == GCOAP_CAPACITY_VARIABLE) {
    free(s);
  }
  return GCOAP_OK;
}

int gcoap_serializer_init(gcoap_serializer_t* s, uint32_t header,
                          const char* token, uint8_t token_len,
                          const char* payload, size_t payload_len) {
  if (s == NULL || token == NULL || token_len > GCOAP_MAXLEN_TOKEN) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_serializer_add_opt(gcoap_serializer_t* s, gcoap_opt_number_t no,
                             const char* val, size_t len) {
  if (s == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_serializer_exec(gcoap_serializer_t* s, char* buf, size_t len) {
  if (s == NULL || buf == NULL || len == 0) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  s->cursor = 0;
  return GCOAP_OK;
}

int gcoap_parser_create(gcoap_parser_t** p) {
  *p = calloc(1, sizeof(gcoap_parser_t));
  if (*p == NULL) {
    return GCOAP_ERR_SYSTEM;
  }
  return GCOAP_OK;
}

int gcoap_fixed_size_parser_create(gcoap_parser_t** p, const char* buf,
                                   size_t len) {
  if (buf == NULL || sizeof(gcoap_parser_t) > len) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  *p = (gcoap_parser_t*)buf;
  (*p)->capacity = len;
  return GCOAP_OK;
}

int gcoap_parser_destroy(gcoap_parser_t* p) {
  if (p == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (p->capacity == GCOAP_CAPACITY_VARIABLE) {
    free(p);
  }
  return GCOAP_OK;
}

int gcoap_parser_execute(gcoap_parser_t* p, const char* buf, size_t len) {
  if (p == NULL || buf == NULL || len == 0) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  p->cursor = 0;
  return GCOAP_OK;
}
