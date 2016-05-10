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

int gcoap_serializer_add_opt(gcoap_serializer* s, uint16_t opt, const char* val,
                             size_t len) {
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
  uint8_t opt_delta_and_len = 0;
  if (s == NULL || len > 65535 + 269) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state != GCOAP_S_STATE_WRITTEN_HEADER) {
    return GCOAP_ERR_INVALID_CALL;
  }
  if (opt <= 255) {  // well-known option
    switch (opt) {
      // empty
      case O_IF_NONE_MATCH:
        if (val != NULL || len != 0) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      // opaque
      case O_IF_MATCH:
        if (len > 8) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta > opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      case O_ETAG:
        if (val == NULL || len == 0 || len > 8) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      // uint
      case O_URI_PORT:
      case O_CONTENT_FORMAT:
      case O_ACCEPT:
        if (len > 2) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      case O_MAX_AGE:
      case O_SIZE1:
        if (len > 4 || len == 3) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      // string
      case O_URI_HOST:
      case O_PROXY_SCHEME:
        if (val == NULL || len == 0 || len > 255) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      case O_LOCATION_PATH:
      case O_URI_PATH:
      case O_URI_QUERY:
      case O_LOCATION_QUERY:
        if (len > 255) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta > opt) {
          return GCOAP_ERR_INVALID_CALL;
        }
        break;
      case O_PROXY_URI:
        if (val == NULL || len == 0 || len > 1034) {
          return GCOAP_ERR_INVALID_ARGUMENT;
        }
        break;
      default:
        return GCOAP_ERR_INVALID_ARGUMENT;
    }
  } else {
    // Except well-known options, all other options are regarded as Repeatable
    // options.
    if (s->sum_of_delta >= opt) {
      return GCOAP_ERR_INVALID_CALL;
    }
  }

  // Option Delta and Length
  if (opt - s->sum_of_delta < 13) {
    opt_delta_and_len = (opt - s->sum_of_delta) << 4;
  } else if (opt - s->sum_of_delta <= 255 + 13) {
    opt_delta_and_len = 13 << 4;
  } else {
    opt_delta_and_len = 14 << 4;
  }
  if (len < 13) {
    opt_delta_and_len |= len;
  } else if (len <= 255 + 13) {
    opt_delta_and_len |= 13;
  } else {
    opt_delta_and_len |= 14;
  }
  if (gcoap_s_write_uint8_(s, opt_delta_and_len)) {
    goto error;
  }

  // Option Delta (extended)
  if (opt - s->sum_of_delta < 13) {
  } else if (opt - s->sum_of_delta <= 255 + 13) {
    if (gcoap_s_write_uint8_(s, (opt - s->sum_of_delta) - 13)) {
      goto error;
    }
  } else {
    if (gcoap_s_write_uint16_(s, (opt - s->sum_of_delta) - 269)) {
      goto error;
    }
  }

  // Option Length (extended)
  if (len < 13) {
  } else if (len <= 255 + 13) {
    if (gcoap_s_write_uint8_(s, len - 13)) {
      goto error;
    }
  } else {
    if (gcoap_s_write_uint16_(s, len - 269)) {
      goto error;
    }
  }

  // Option Value
  if (gcoap_s_write_buf_(s, val, len)) {
    goto error;
  }
  s->sum_of_delta = opt;
  return GCOAP_OK;
error:
  gcoap_s_clear_(s);
  return GCOAP_ERR_LIMIT;
}

int gcoap_serializer_add_opt_empty(gcoap_serializer* s, uint16_t opt) {
  if (opt <= 255) {
    switch (opt) {
      case O_IF_NONE_MATCH:
        break;
      default:
        return GCOAP_ERR_INVALID_ARGUMENT;
    }
  }
  return gcoap_serializer_add_opt(s, opt, NULL, 0);
}

int gcoap_serializer_add_opt_opaque(gcoap_serializer* s, uint16_t opt,
                                    const char* val, size_t len) {
  if (opt <= 255) {
    switch (opt) {
      case O_IF_MATCH:
      case O_ETAG:
        break;
      default:
        return GCOAP_ERR_INVALID_ARGUMENT;
    }
  }
  return gcoap_serializer_add_opt(s, opt, val, len);
}

int gcoap_serializer_add_opt_uint(gcoap_serializer* s, uint16_t opt,
                                  uint32_t val) {
  if (opt <= 255) {
    switch (opt) {
      case O_URI_PORT:
      case O_CONTENT_FORMAT:
      case O_ACCEPT:
      case O_MAX_AGE:
      case O_SIZE1:
        break;
      default:
        return GCOAP_ERR_INVALID_ARGUMENT;
    }
  }
  if (val == 0) {
    return gcoap_serializer_add_opt(s, opt, NULL, 0);
  }
  if (val < 256) {
    uint8_t nbo_val = (uint8_t)val;
    return gcoap_serializer_add_opt(s, opt, (const char*)&nbo_val, 1);
  }
  if (val < 65536) {
    uint16_t nbo_val = htons((uint16_t)val);
    return gcoap_serializer_add_opt(s, opt, (const char*)&nbo_val, 2);
  }
  uint32_t nbo_val = htonl(val);
  return gcoap_serializer_add_opt(s, opt, (const char*)&nbo_val, 4);
}

int gcoap_serializer_add_opt_string(gcoap_serializer* s, uint16_t opt,
                                    const char* val, size_t len) {
  if (opt <= 255) {
    switch (opt) {
      case O_URI_HOST:
      case O_PROXY_SCHEME:
      case O_LOCATION_PATH:
      case O_URI_PATH:
      case O_URI_QUERY:
      case O_LOCATION_QUERY:
      case O_PROXY_URI:
        break;
      default:
        return GCOAP_ERR_INVALID_ARGUMENT;
    }
  }
  return gcoap_serializer_add_opt(s, opt, val, len);
}

int gcoap_serializer_set_payload(gcoap_serializer* s, const char* payload,
                                 size_t payload_len) {
  if (s == NULL || payload == NULL || payload_len == 0) {
    return GCOAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state == GCOAP_S_STATE_INIT) {
    return GCOAP_ERR_INVALID_CALL;
  }
  if (s->state == GCOAP_S_STATE_WRITTEN_PAYLOAD) {
    s->cursor = s->payload;
  }
  if (gcoap_s_write_uint8_(s, 0xFF)) {
    goto error;
  }
  s->payload = s->cursor;
  if (gcoap_s_write_buf_(s, payload, payload_len)) {
    goto error;
  }
  s->state = GCOAP_S_STATE_WRITTEN_PAYLOAD;
  return GCOAP_OK;
error:
  gcoap_s_clear_(s);
  return GCOAP_ERR_LIMIT;
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
