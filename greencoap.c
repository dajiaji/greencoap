#include "greencoap_i.h"
#include <assert.h>
#include <stdio.h>
#include <arpa/inet.h>

inline void coap_s_clear_(coap_serializer* s) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  s->cursor = 0;
  s->sum_of_delta = 0;
  s->state = COAP_S_STATE_INIT;
  return;
}

inline int coap_validate_header_(uint32_t header) { return 0; }

inline int coap_s_write_uint8_(coap_serializer* s, uint8_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 1) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 1);
  s->cursor++;
  return 0;
}

inline int coap_s_write_uint16_(coap_serializer* s, uint16_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 2) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 2);
  s->cursor += 2;
  return 0;
}

inline int coap_s_write_uint32_(coap_serializer* s, uint32_t src) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  if (s->buf_len - s->cursor < 4) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], &src, 4);
  s->cursor += 4;
  return 0;
}

inline int coap_s_write_buf_(coap_serializer* s, const char* src,
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

inline void coap_p_read_(coap_parser* p, void* dst, size_t dst_len,
                         const char* src, size_t src_len) {
  assert(p != NULL && src_len - p->cursor >= dst_len);
  memcpy(dst, &src[p->cursor], dst_len);
  p->cursor += dst_len;
  return;
}

inline void coap_p_read_uint16_(coap_parser* p, uint16_t* dst, const char* src,
                                size_t len) {
  coap_p_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

inline void coap_p_read_uint32_(coap_parser* p, uint32_t* dst, const char* src,
                                size_t len) {
  coap_p_read_(p, (void*)dst, sizeof(*dst), src, len);
  return;
}

int coap_serializer_init(coap_serializer** s, void* buf, size_t len) {
  if (buf == NULL || sizeof(coap_serializer) > len) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  *s = (coap_serializer*)buf;
  (*s)->buf_len = 0;
  (*s)->buf = NULL;
  (*s)->cursor = 0;
  (*s)->state = COAP_S_STATE_INIT;
  return COAP_OK;
}

int coap_serializer_begin(coap_serializer* s, char* buf, size_t len) {
  if (s == NULL || buf == NULL || len < COAP_LEN_HEADER) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  s->buf_len = len;
  s->buf = buf;
  s->cursor = 0;
  s->state = COAP_S_STATE_INIT;
  return COAP_OK;
}

int coap_serializer_set_header(coap_serializer* s, uint32_t header,
                               const char* token, uint8_t token_len) {
  if (s == NULL || token_len > COAP_MAXLEN_TOKEN ||
      (token != NULL && token_len == 0)) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  if (coap_validate_header_(header)) {  // != 0
    return COAP_ERR_INVALID_ARGUMENT;
  }
  s->cursor = 0;
  s->sum_of_delta = 0;
  coap_s_write_uint32_(s, htonl(COAP_COAP_VER | header | (token_len << 24)));
  if (token != NULL && coap_s_write_buf_(s, token, token_len)) {  // != 0
    coap_s_clear_(s);
    return COAP_ERR_LIMIT;
  }
  s->state = COAP_S_STATE_WRITTEN_HEADER;
  return COAP_OK;
}

int coap_serializer_add_opt(coap_serializer* s, uint16_t opt, const char* val,
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
    return COAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state != COAP_S_STATE_WRITTEN_HEADER) {
    return COAP_ERR_INVALID_CALL;
  }
  if (opt <= 255) {  // well-known option
    switch (opt) {
      // empty
      case O_IF_NONE_MATCH:
        if (val != NULL || len != 0) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      // opaque
      case O_IF_MATCH:
        if (len > 8) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta > opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_ETAG:
        if (val == NULL || len == 0 || len > 8) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      // uint
      case O_URI_PORT:
      case O_CONTENT_FORMAT:
      case O_ACCEPT:
        if (len > 2) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_MAX_AGE:
      case O_SIZE1:
        if (len > 4 || len == 3) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      // string
      case O_URI_HOST:
      case O_PROXY_SCHEME:
        if (val == NULL || len == 0 || len > 255) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta >= opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_LOCATION_PATH:
      case O_URI_PATH:
      case O_URI_QUERY:
      case O_LOCATION_QUERY:
        if (len > 255) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta > opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_PROXY_URI:
        if (val == NULL || len == 0 || len > 1034) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        break;
      default:
        return COAP_ERR_INVALID_ARGUMENT;
    }
  } else {
    // Except well-known options, all other options are regarded as Repeatable
    // options.
    if (s->sum_of_delta >= opt) {
      return COAP_ERR_INVALID_CALL;
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
  if (coap_s_write_uint8_(s, opt_delta_and_len)) {
    goto error;
  }

  // Option Delta (extended)
  if (opt - s->sum_of_delta < 13) {
  } else if (opt - s->sum_of_delta <= 255 + 13) {
    if (coap_s_write_uint8_(s, (opt - s->sum_of_delta) - 13)) {
      goto error;
    }
  } else {
    if (coap_s_write_uint16_(s, (opt - s->sum_of_delta) - 269)) {
      goto error;
    }
  }

  // Option Length (extended)
  if (len < 13) {
  } else if (len <= 255 + 13) {
    if (coap_s_write_uint8_(s, len - 13)) {
      goto error;
    }
  } else {
    if (coap_s_write_uint16_(s, len - 269)) {
      goto error;
    }
  }

  // Option Value
  if (coap_s_write_buf_(s, val, len)) {
    goto error;
  }
  s->sum_of_delta = opt;
  return COAP_OK;
error:
  coap_s_clear_(s);
  return COAP_ERR_LIMIT;
}

int coap_serializer_add_opt_empty(coap_serializer* s, uint16_t opt) {
  if (opt <= 255) {
    switch (opt) {
      case O_IF_NONE_MATCH:
        break;
      default:
        return COAP_ERR_INVALID_ARGUMENT;
    }
  }
  return coap_serializer_add_opt(s, opt, NULL, 0);
}

int coap_serializer_add_opt_opaque(coap_serializer* s, uint16_t opt,
                                   const char* val, size_t len) {
  if (opt <= 255) {
    switch (opt) {
      case O_IF_MATCH:
      case O_ETAG:
        break;
      default:
        return COAP_ERR_INVALID_ARGUMENT;
    }
  }
  return coap_serializer_add_opt(s, opt, val, len);
}

int coap_serializer_add_opt_uint(coap_serializer* s, uint16_t opt,
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
        return COAP_ERR_INVALID_ARGUMENT;
    }
  }
  if (val == 0) {
    return coap_serializer_add_opt(s, opt, NULL, 0);
  }
  if (val < 256) {
    uint8_t nbo_val = (uint8_t)val;
    return coap_serializer_add_opt(s, opt, (const char*)&nbo_val, 1);
  }
  if (val < 65536) {
    uint16_t nbo_val = htons((uint16_t)val);
    return coap_serializer_add_opt(s, opt, (const char*)&nbo_val, 2);
  }
  uint32_t nbo_val = htonl(val);
  return coap_serializer_add_opt(s, opt, (const char*)&nbo_val, 4);
}

int coap_serializer_add_opt_string(coap_serializer* s, uint16_t opt,
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
        return COAP_ERR_INVALID_ARGUMENT;
    }
  }
  return coap_serializer_add_opt(s, opt, val, len);
}

int coap_serializer_set_payload(coap_serializer* s, const char* payload,
                                size_t payload_len) {
  if (s == NULL || payload == NULL || payload_len == 0) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  if (s->state == COAP_S_STATE_INIT) {
    return COAP_ERR_INVALID_CALL;
  }
  if (s->state == COAP_S_STATE_WRITTEN_PAYLOAD) {
    s->cursor = s->payload;
  }
  if (coap_s_write_uint8_(s, 0xFF)) {
    goto error;
  }
  s->payload = s->cursor;
  if (coap_s_write_buf_(s, payload, payload_len)) {
    goto error;
  }
  s->state = COAP_S_STATE_WRITTEN_PAYLOAD;
  return COAP_OK;
error:
  coap_s_clear_(s);
  return COAP_ERR_LIMIT;
}

int coap_parser_init(coap_parser** p, const char* buf, size_t len) {
  if (buf == NULL || sizeof(coap_parser) > len) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  *p = (coap_parser*)buf;
  return COAP_OK;
}

int coap_parser_exec(coap_parser* p, const char* buf, size_t len) {
  if (p == NULL || buf == NULL || len == 0) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  p->cursor = 0;
  return COAP_OK;
}

int coap_parser_get_type(const coap_parser* p, coap_type_t* res) {
  if (p == NULL) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  return COAP_OK;
}

int coap_parser_get_code(const coap_parser* p, coap_code_t* res) {
  if (p == NULL) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  return COAP_OK;
}

int coap_parser_get_path(const coap_parser* p, const char** res, size_t* len) {
  if (p == NULL) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  return COAP_OK;
}

int coap_parser_get_payload(const coap_parser* p, const char** res,
                            size_t* len) {
  if (p == NULL) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  return COAP_OK;
}

size_t coap_serializer_size() { return sizeof(coap_serializer); }
size_t coap_parser_size() { return sizeof(coap_parser); }
