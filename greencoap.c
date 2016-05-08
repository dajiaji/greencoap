#include "greencoap_i.h"
#include <assert.h>
#include <stdio.h>
#include <arpa/inet.h>

inline int gcoap_validate_header_(uint32_t header) { return 0; }

inline int gcoap_validate_opt_(gcoap_serializer* s, gcoap_opt_number_t opt) {
  /**
  +-----+---+---+---+---+----------------+--------+--------+----------+
  | No. | C | U | N | R | Name           | Format | Length | Default  |
  +-----+---+---+---+---+----------------+--------+--------+----------+
  |   1 | x |   |   | x | If-Match       | opaque | 0-8    | (none)   |
  |   3 | x | x | - |   | Uri-Host       | string | 1-255  | (see     |
  |     |   |   |   |   |                |        |        | below)   |
  |   4 |   |   |   | x | ETag           | opaque | 1-8    | (none)   |
  |   5 | x |   |   |   | If-None-Match  | empty  | 0      | (none)   |
  |   7 | x | x | - |   | Uri-Port       | uint   | 0-2    | (see     |
  |     |   |   |   |   |                |        |        | below)   |
  |   8 |   |   |   | x | Location-Path  | string | 0-255  | (none)   |
  |  11 | x | x | - | x | Uri-Path       | string | 0-255  | (none)   |
  |  12 |   |   |   |   | Content-Format | uint   | 0-2    | (none)   |
  |  14 |   | x | - |   | Max-Age        | uint   | 0-4    | 60       |
  |  15 | x | x | - | x | Uri-Query      | string | 0-255  | (none)   |
  |  17 | x |   |   |   | Accept         | uint   | 0-2    | (none)   |
  |  20 |   |   |   | x | Location-Query | string | 0-255  | (none)   |
  |  35 | x | x | - |   | Proxy-Uri      | string | 1-1034 | (none)   |
  |  39 | x | x | - |   | Proxy-Scheme   | string | 1-255  | (none)   |
  |  60 |   |   | x |   | Size1          | uint   | 0-4    | (none)   |
  +-----+---+---+---+---+----------------+--------+--------+----------+
  */

  // 000001 1
  // 000011 3
  // 000100 4
  // 000101 5
  // 000111 7
  // 001000 8
  // 001011 11
  // 001100 12
  // 001110 14
  // 001111 15
  // 010001 17
  // 010100 20
  // 100011 35
  // 101111 39
  // 111100 60
  switch (opt) {
    case O_IF_MATCH:
      break;
    default:
      return -1;
  }
  return 0;
}

inline void gcoap_serializer_clear_(gcoap_serializer* s) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  s->cursor = 0;
  s->state = GCOAP_S_STATE_INIT;
  return;
}

inline void gcoap_serializer_write_uint8_(gcoap_serializer* s, char* dst,
                                          size_t dst_len, uint8_t src) {
  assert(s != NULL && dst != NULL && dst_len > 0 && dst_len - s->cursor >= 1);
  dst[s->cursor] = src;
  s->cursor++;
  return;
}

inline void gcoap_serializer_write_uint16_(gcoap_serializer* s, char* dst,
                                           size_t dst_len, uint16_t src) {
  assert(s != NULL && dst != NULL && dst_len > 0 && dst_len - s->cursor >= 2);
  memcpy(&dst[s->cursor], &src, 2);
  s->cursor += 2;
  return;
}

inline int gcoap_serializer_write_uint32_(gcoap_serializer* s, uint32_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 4) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 4);
  s->cursor += 4;
  return 0;
}

inline int gcoap_serializer_write_buf_(gcoap_serializer* s, const char* src,
                                       size_t src_len) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0 && src != NULL &&
         src_len > 0);
  if (s->buf_len - s->cursor < src_len) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], src, src_len);
  s->cursor += src_len;
  return 0;
}

inline void gcoap_parser_read_(gcoap_parser* p, void* dst, size_t dst_len,
                               const char* src, size_t src_len) {
  assert(p != NULL && src_len - p->cursor >= dst_len);
  memcpy(dst, &src[p->cursor], dst_len);
  p->cursor += dst_len;
  return;
}

inline void gcoap_parser_read_uint16_(gcoap_parser* p, uint16_t* dst,
                                      const char* src, size_t len) {
  gcoap_parser_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

inline void gcoap_parser_read_uint32_(gcoap_parser* p, uint32_t* dst,
                                      const char* src, size_t len) {
  gcoap_parser_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

int gcoap_serializer_init(gcoap_serializer** s, void* buf, size_t len) {
  if (buf == NULL || sizeof(gcoap_serializer) > len) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  *s = (gcoap_serializer*)buf;
  (*s)->buf_len = 0;
  (*s)->buf = NULL;
  (*s)->cursor = 0;
  (*s)->state = GCOAP_S_STATE_INIT;
  return GCOAP_OK;
}

int gcoap_serializer_begin(gcoap_serializer* s, char* buf, size_t len) {
  if (s == NULL || buf == NULL || len < GCOAP_LEN_HEADER) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  s->buf_len = len;
  s->buf = buf;
  s->cursor = 0;
  s->state = GCOAP_S_STATE_INIT;
  return GCOAP_OK;
}

int gcoap_serializer_set_header(gcoap_serializer* s, uint32_t header,
                                const char* token, uint8_t token_len) {
  if (s == NULL || token_len > GCOAP_MAXLEN_TOKEN ||
      (token != NULL && token == 0)) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (gcoap_validate_header_(header)) {  // != 0
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  s->cursor = 0;
  gcoap_serializer_write_uint32_(
    s, htonl(GCOAP_COAP_VERSION | header | (token_len << 24)));
  if (token != NULL &&
      gcoap_serializer_write_buf_(s, token, token_len)) {  // != 0
    gcoap_serializer_clear_(s);
    return GCOAP_ERR_LIMIT;
  }
  s->state = GCOAP_S_STATE_WRITTEN_HEADER;
  return GCOAP_OK;
}

int gcoap_serializer_add_opt(gcoap_serializer* s, gcoap_opt_number_t opt,
                             const char* val, size_t len) {
  if (s == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state != GCOAP_S_STATE_WRITTEN_HEADER) {
    return GCOAP_ERR_INVALID_CALL;
  }
  return GCOAP_OK;
}

int gcoap_serializer_add_opt_empty(gcoap_serializer* s,
                                   gcoap_opt_number_t opt) {
  if (s == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_serializer_add_opt_uint(gcoap_serializer* s, gcoap_opt_number_t opt,
                                  uint32_t val) {
  if (s == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_serializer_set_payload(gcoap_serializer* s, const char* payload,
                                 size_t payload_len) {
  if (s == NULL || payload == NULL || payload_len == 0) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_parser_init(gcoap_parser** p, const char* buf, size_t len) {
  if (buf == NULL || sizeof(gcoap_parser) > len) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  *p = (gcoap_parser*)buf;
  return GCOAP_OK;
}

int gcoap_parser_exec(gcoap_parser* p, const char* buf, size_t len) {
  if (p == NULL || buf == NULL || len == 0) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  p->cursor = 0;
  return GCOAP_OK;
}

int gcoap_parser_get_type(const gcoap_parser* p, gcoap_type_t* res) {
  if (p == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_parser_get_code(const gcoap_parser* p, gcoap_code_t* res) {
  if (p == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_parser_get_path(const gcoap_parser* p, const char** res,
                          size_t* len) {
  if (p == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

int gcoap_parser_get_payload(const gcoap_parser* p, const char** res,
                             size_t* len) {
  if (p == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  return GCOAP_OK;
}

size_t gcoap_serializer_size() { return sizeof(gcoap_serializer); }
size_t gcoap_parser_size() { return sizeof(gcoap_parser); }
