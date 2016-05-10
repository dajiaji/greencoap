#include "greencoap_i.h"
#include <assert.h>
#include <stdio.h>
#include <arpa/inet.h>

#define IS_WELL_KNOWN_(x) (x <= 255)

static void coap_s_clear_(coap_serializer* s) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0);
  s->cursor = 0;
  s->sum_of_delta = 0;
  s->state = COAP_S_STATE_INIT;
  return;
}

static int coap_s_write_(coap_serializer* s, const char* src, size_t src_len) {
  assert(s != NULL && s->buf != NULL && s->buf_len > 0 && src != NULL &&
         src_len > 0);
  if (s->buf_len - s->cursor < src_len) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], src, src_len);
  s->cursor += src_len;
  return 0;
}

static int coap_s_write_uint8_(coap_serializer* s, uint8_t src) {
  return coap_s_write_(s, (const char*)&src, 1);
}

static int coap_s_write_uint16_(coap_serializer* s, uint16_t src) {
  return coap_s_write_(s, (const char*)&src, 2);
}

static int coap_s_write_uint32_(coap_serializer* s, uint32_t src) {
  return coap_s_write_(s, (const char*)&src, 4);
}

static int coap_p_read_(coap_parser* p, char* dst, size_t dst_len) {
  assert(p != NULL && p->buf != NULL && p->buf_len > 0 && dst != NULL &&
         dst_len > 0);
  if (p->buf_len - p->cursor < dst_len) {
    return -1;
  }
  memcpy(dst, &p->buf[p->cursor], dst_len);
  p->cursor += dst_len;
  return 0;
}

// inline int coap_p_read_uint8_(coap_parser* p, uint8_t* dst) {
//  return coap_p_read_(p, (void*)dst, sizeof(*dst));
//}
//
// inline int coap_p_read_uint16_(coap_parser* p, uint16_t* dst) {
//  return coap_p_read_(p, (void*)dst, sizeof(*dst));
//}
//
// inline int coap_p_read_uint32_(coap_parser* p, uint32_t* dst) {
//  return coap_p_read_(p, (void*)dst, sizeof(*dst));
//}

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
  switch (header & 0x00FF0000) {
    case C_GET:
    case C_POST:
    case C_PUT:
    case C_DELETE:
    case C_CREATED:
    case C_DELETED:
    case C_VALID:
    case C_CHANGED:
    case C_CONTENT:
    case C_BAD_REQUEST:
    case C_UNAUTHORIZED:
    case C_BAD_OPTION:
    case C_FORBIDDEN:
    case C_NOT_FOUND:
    case C_METHOD_NOT_ALLOWED:
    case C_NOT_ACCEPTABLE:
    case C_PRECONDITION_FAILED:
    case C_REQUEST_ENTITY_TOO_LARGE:
    case C_UNSUPPORTED_CONTENT_FORMAT:
    case C_INTERNAL_SERVER_ERROR:
    case C_NOT_IMPLEMENTED:
    case C_BAD_GATEWAY:
    case C_SERVICE_UNAVAILABLE:
    case C_GATEWAY_TIMEOUT:
    case C_PROXYING_NOT_SUPPORTED:
      break;
    default:
      return COAP_ERR_INVALID_ARGUMENT;
  }
  s->cursor = 0;
  s->sum_of_delta = 0;
  coap_s_write_uint32_(s, htonl(COAP_VERSION | header | (token_len << 24)));
  if (token != NULL && coap_s_write_(s, token, token_len)) {  // != 0
    coap_s_clear_(s);
    return COAP_ERR_LIMIT;
  }
  s->state = COAP_S_STATE_WRITTEN_HEADER;
  return COAP_OK;
}

static int coap_serializer_add_opt(coap_serializer* s, uint16_t opt,
                                   const char* val, size_t len) {
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
  if (IS_WELL_KNOWN_(opt)) {
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
      case O_ETAG:
        if (val == NULL || len == 0) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
      case O_IF_MATCH:
        if (len > 8) {
          return COAP_ERR_INVALID_ARGUMENT;
        }
        if (s->sum_of_delta > opt) {
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
    if (s->sum_of_delta > opt) {
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
  if (coap_s_write_(s, val, len)) {
    goto error;
  }
  s->sum_of_delta = opt;
  return COAP_OK;
error:
  coap_s_clear_(s);
  return COAP_ERR_LIMIT;
}

int coap_serializer_add_opt_empty(coap_serializer* s, uint16_t opt) {
  if (IS_WELL_KNOWN_(opt)) {
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
  if (IS_WELL_KNOWN_(opt)) {
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
  if (IS_WELL_KNOWN_(opt)) {
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
  if (IS_WELL_KNOWN_(opt)) {
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
  } else {
    if (coap_s_write_uint8_(s, 0xFF)) {
      goto error;
    }
    s->payload = s->cursor;
  }
  if (coap_s_write_(s, payload, payload_len)) {
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
  (*p)->buf_len = 0;
  (*p)->buf = NULL;
  (*p)->cursor = 0;
  (*p)->state = COAP_P_STATE_INIT;
  return COAP_OK;
}

int coap_parser_exec(coap_parser* p, const char* buf, size_t len) {
  if (p == NULL || buf == NULL || len == 0) {
    return COAP_ERR_INVALID_ARGUMENT;
  }
  p->buf_len = len;
  p->buf = buf;
  p->cursor = 0;

  // Parse a CoAP header.
  if (coap_p_read_(p, (char*)&p->header, 4)) {
    goto error;
  }
  p->header = ntohl(p->header);
  if ((p->header & 0xC0000000) != COAP_VERSION) {
    goto error;
  }
  switch (p->header & 0x00FF0000) {
    case C_GET:
    case C_POST:
    case C_PUT:
    case C_DELETE:
    case C_CREATED:
    case C_DELETED:
    case C_VALID:
    case C_CHANGED:
    case C_CONTENT:
    case C_BAD_REQUEST:
    case C_UNAUTHORIZED:
    case C_BAD_OPTION:
    case C_FORBIDDEN:
    case C_NOT_FOUND:
    case C_METHOD_NOT_ALLOWED:
    case C_NOT_ACCEPTABLE:
    case C_PRECONDITION_FAILED:
    case C_REQUEST_ENTITY_TOO_LARGE:
    case C_UNSUPPORTED_CONTENT_FORMAT:
    case C_INTERNAL_SERVER_ERROR:
    case C_NOT_IMPLEMENTED:
    case C_BAD_GATEWAY:
    case C_SERVICE_UNAVAILABLE:
    case C_GATEWAY_TIMEOUT:
    case C_PROXYING_NOT_SUPPORTED:
      break;
    default:
      goto error;
  }
  p->token_len = (p->header & 0x0F000000) >> 24;
  if (p->token_len > 8) {
    goto error;
  }
  p->cursor += p->token_len;

  // Parse a CoAP options.
  return COAP_OK;
error:
  return COAP_ERR_SYNTAX;
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
