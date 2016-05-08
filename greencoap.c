#include "greencoap_i.h"
#include <assert.h>
#include <stdio.h>
#include <arpa/inet.h>

inline void gcoap_s_clear_(gcoap_serializer* s) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  s->cursor = 0;
  s->sum_of_delta = 0;
  s->state = GCOAP_S_STATE_INIT;
  return;
}

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
    case O_URI_HOST:
      break;
    case O_ETAG:
      break;
    case O_IF_NONE_MATCH:
      break;
    case O_URI_PORT:
      break;
    case O_LOCATION_PATH:
      break;
    case O_URI_PATH:
      break;
    case O_CONTENT_FORMAT:
      break;
    case O_MAX_AGE:
      break;
    case O_URI_QUERY:
      break;
    case O_ACCEPT:
      break;
    case O_LOCATION_QUERY:
      break;
    case O_PROXY_URI:
      break;
    case O_PROXY_SCHEME:
      break;
    case O_SIZE1:
      break;
    default:
      return -1;
  }
  return 0;
}

inline int gcoap_s_write_uint8_(gcoap_serializer* s, uint8_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 1) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 1);
  s->cursor++;
  return 0;
}

inline int gcoap_s_write_uint16_(gcoap_serializer* s, uint16_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 2) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 2);
  s->cursor += 2;
  return 0;
}

inline int gcoap_s_write_uint32_(gcoap_serializer* s, uint32_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 4) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 4);
  s->cursor += 4;
  return 0;
}

inline int gcoap_s_write_buf_(gcoap_serializer* s, const char* src,
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

inline void gcoap_p_read_(gcoap_parser* p, void* dst, size_t dst_len,
                          const char* src, size_t src_len) {
  assert(p != NULL && src_len - p->cursor >= dst_len);
  memcpy(dst, &src[p->cursor], dst_len);
  p->cursor += dst_len;
  return;
}

inline void gcoap_p_read_uint16_(gcoap_parser* p, uint16_t* dst,
                                 const char* src, size_t len) {
  gcoap_p_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

inline void gcoap_p_read_uint32_(gcoap_parser* p, uint32_t* dst,
                                 const char* src, size_t len) {
  gcoap_p_read_(p, (void*)dst, sizeof(*dst), src, len);
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
      (token != NULL && token_len == 0)) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (gcoap_validate_header_(header)) {  // != 0
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  s->cursor = 0;
  s->sum_of_delta = 0;
  gcoap_s_write_uint32_(s, htonl(GCOAP_COAP_VER | header | (token_len << 24)));
  if (token != NULL && gcoap_s_write_buf_(s, token, token_len)) {  // != 0
    gcoap_s_clear_(s);
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
  if (s->state != GCOAP_S_STATE_WRITTEN_HEADER || s->sum_of_delta > opt) {
    return GCOAP_ERR_INVALID_CALL;
  }
  // if (delta <= 12) {
  //} else if (delta <= 255 + 13) {
  //} else if (delta <= 65535 + 269) {
  //} else {
  //  return GOAP_ERR_INVALID_AR
  //}
  return GCOAP_OK;
}

int gcoap_serializer_add_opt_empty(gcoap_serializer* s,
                                   gcoap_opt_number_t opt) {
  if (s == NULL || opt != O_IF_NONE_MATCH) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state != GCOAP_S_STATE_WRITTEN_HEADER || s->sum_of_delta >= opt) {
    return GCOAP_ERR_INVALID_CALL;
  }
  uint8_t delta = (opt - s->sum_of_delta) << 4;
  if (gcoap_s_write_uint8_(s, delta)) {
    gcoap_s_clear_(s);
    return GCOAP_ERR_LIMIT;
  }
  s->sum_of_delta = opt;
  return GCOAP_OK;
}

int gcoap_serializer_add_opt_uint(gcoap_serializer* s, gcoap_opt_number_t opt,
                                  uint32_t val) {
  if (s == NULL) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state != GCOAP_S_STATE_WRITTEN_HEADER) {
    return GCOAP_ERR_INVALID_CALL;
  }
  uint8_t val_len = 4;
  switch (opt) {
    case O_URI_PORT:
    case O_CONTENT_FORMAT:
    case O_ACCEPT:
      if (s->sum_of_delta >= opt) {
        return GCOAP_ERR_INVALID_CALL;
      }
      if (val == 0) {
        val_len = 0;
      } else if (val < 256) {
        val_len = 1;
      } else if (val < 65536) {
        val_len = 2;
      }
      if (val_len == 4) {
        return GCOAP_ERR_INVALID_ARGUMENT;
      }
      break;
    case O_MAX_AGE:
    case O_SIZE1:
      if (s->sum_of_delta >= opt) {
        return GCOAP_ERR_INVALID_CALL;
      }
      if (val == 0) {
        val_len = 0;
      } else if (val < 256) {
        val_len = 1;
      } else if (val < 65536) {
        val_len = 2;
      }
      break;
    default:
      return GCOAP_ERR_INVALID_ARGUMENT;
  }
  uint8_t opt_delta_len = 0;

  // Option Delta | Option Length
  // Option Delta (extended)
  uint32_t delta = opt - s->sum_of_delta;
  if (delta < 13) {
    opt_delta_len = delta << 4 | val_len;
    if (gcoap_s_write_uint8_(s, opt_delta_len)) {
      goto error;
    }
  } else if (delta < 255 + 13) {
    opt_delta_len = 13 << 4 | val_len;
    if (gcoap_s_write_uint8_(s, opt_delta_len)) {
      goto error;
    }
    uint8_t extended_opt = delta - 13;
    if (gcoap_s_write_uint8_(s, extended_opt)) {
      goto error;
    }
  } else if (delta < 65535 + 269) {
    opt_delta_len = 14 << 4 | val_len;
    if (gcoap_s_write_uint8_(s, opt_delta_len)) {
      goto error;
    }
    uint16_t extended_opt = delta - 269;
    if (gcoap_s_write_uint16_(s, htons(extended_opt))) {
      goto error;
    }
  }
  // Option Value
  if (val_len == 1) {
    if (gcoap_s_write_uint8_(s, (uint8_t)val)) {
      goto error;
    }
  } else if (val_len == 2) {
    if (gcoap_s_write_uint16_(s, htons((uint16_t)val))) {
      goto error;
    }
  } else if (val_len == 4) {
    if (gcoap_s_write_uint32_(s, htonl(val))) {
      goto error;
    }
  }
  s->sum_of_delta = opt;
  return GCOAP_OK;
error:
  gcoap_s_clear_(s);
  return GCOAP_ERR_LIMIT;
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
