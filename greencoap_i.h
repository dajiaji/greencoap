#ifndef _GREENCOAP_I_H_
#define _GREENCOAP_I_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "greencoap.h"
#include <assert.h>
#include <string.h>

#define GCOAP_TRUE 1
#define GCOAP_FALSE 0

#define GCOAP_CAPACITY_VARIABLE 0
#define GCOAP_MAXLEN_TOKEN 8

#define IS_REQUEST_(msg_type) (((msg_type)&0x1001) == 1)

/**
 * Status codes for coap-parser APIs.
 */
typedef uint8_t gcoap_bool_t;

/**
 * General buffer structure.
 */
typedef struct gcoap_buf_t {
  size_t len;
  const char* val;
} gcoap_buf_t;

/**
 * CoAP option structure.
 */
typedef struct gcoap_opt_t {
  uint32_t delta;
  uint32_t len;
  const char* val;
} gcoap_opt_t;

/**
 * CoAP serializer.
 */
struct gcoap_serializer_t {
  size_t capacity;
  size_t cursor;
  uint32_t header;
  const char* token;
  size_t opts_len;
  const char* opts;
  size_t payload_len;
  const char* payload;
};

/**
 * CoAP parser.
 */
struct gcoap_parser_t {
  size_t capacity;
  size_t cursor;
  const char* token;
  size_t token_len;
};

static inline void gcoap_serializer_write_uint8_(gcoap_serializer_t* s,
                                                 char* dst, size_t dst_len,
                                                 uint8_t src) {
  assert(s != NULL && dst != NULL && dst_len > 0 && dst_len - s->cursor >= 1);
  dst[s->cursor] = src;
  s->cursor++;
  return;
}

static inline void gcoap_serializer_write_uint16_(gcoap_serializer_t* s,
                                                  char* dst, size_t dst_len,
                                                  uint16_t src) {
  assert(s != NULL && dst != NULL && dst_len > 0 && dst_len - s->cursor >= 2);
  memcpy(&dst[s->cursor], &src, 2);
  s->cursor += 2;
  return;
}

static inline void gcoap_serializer_write_uint32_(gcoap_serializer_t* s,
                                                  char* dst, size_t dst_len,
                                                  uint32_t src) {
  assert(s != NULL && dst != NULL && dst_len > 0 && dst_len - s->cursor >= 4);
  memcpy(&dst[s->cursor], &src, 4);
  s->cursor += 4;
  return;
}

static inline void gcoap_serializer_write_buf_(gcoap_serializer_t* s, char* dst,
                                               size_t dst_len, const char* src,
                                               size_t src_len) {
  assert(s != NULL && dst != NULL && dst_len > 0 &&
         dst_len - s->cursor >= src_len);
  assert(src != NULL && src_len > 0);
  memcpy(&dst[s->cursor], src, src_len);
  s->cursor += src_len;
  return;
}

static inline void gcoap_parser_read_(gcoap_parser_t* p, void* dst,
                                      size_t dst_len, const char* src,
                                      size_t src_len) {
  assert(p != NULL && src_len - p->cursor >= dst_len);
  memcpy(dst, &src[p->cursor], dst_len);
  p->cursor += dst_len;
  return;
}

static inline void gcoap_parser_read_uint16_(gcoap_parser_t* p, uint16_t* dst,
                                             const char* src, size_t len) {
  gcoap_parser_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

static inline void gcoap_parser_read_uint32_(gcoap_parser_t* p, uint32_t* dst,
                                             const char* src, size_t len) {
  gcoap_parser_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_I_H_ */
