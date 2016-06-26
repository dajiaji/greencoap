#if defined(TARGET_LIKE_MBED)
#include "sal-stack-lwip/lwip/include/lwip/def.h"
#else
#include <arpa/inet.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "greencoap.h"

#define COAP_VERSION (1 << 30)
#define COAP_LEN_HEADER 4
#define COAP_MAXLEN_TOKEN 8

/**
 * CoAP serializer.
 */
struct coap_serializer_t {
  size_t buf_len;
  char* buf;
  size_t cursor;
  size_t payload;
  uint16_t sum_of_delta;
  uint8_t token_len;
  uint8_t executed;
};

/**
 * CoAP parser.
 */
struct coap_parser_t {
  size_t buf_len;
  const char* buf;
  size_t cursor;
  size_t payload;
  uint8_t version;
  uint8_t type;
  uint8_t code;
  uint16_t mid;
  uint8_t token_len;
  uint8_t executed;
  void* cookie;
  coap_parser_cb_t on_begin;
  coap_parser_cb_header_t on_header;
  coap_parser_cb_opt_t on_opt;
  coap_parser_cb_payload_t on_payload;
  coap_parser_cb_t on_complete;
};

static uint8_t is_well_known_(uint16_t opt) {
  if (opt <= 255) return 1;
  return 0;
}

static int validate_type_code_(uint8_t type, uint8_t code) {
  switch (code) {
    case 0:
      switch (type) {
        case T_ACK:
        case T_RST:
          break;
        default:
          return -1;
      }
      break;
    case C_GET:
    case C_POST:
    case C_PUT:
    case C_DELETE:
      switch (type) {
        case T_CON:
        case T_NON:
          break;
        default:
          return -1;
      }
      break;
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
      switch (type) {
        case T_RST:
          return -1;
        default:
          break;
      }
      break;
    default:
      return -1;
  }
  return 0;
}

static int coap_s_write_(coap_serializer_t* s, const char* src,
                         size_t src_len) {
  if (s->buf_len - s->cursor < src_len) {
    return -1;
  }
  memcpy(&s->buf[s->cursor], src, src_len);
  s->cursor += src_len;
  return 0;
}

static int coap_s_write_uint8_(coap_serializer_t* s, uint8_t src) {
  return coap_s_write_(s, (const char*)&src, 1);
}

static int coap_s_write_uint16_(coap_serializer_t* s, uint16_t src) {
  return coap_s_write_(s, (const char*)&src, 2);
}

static int coap_s_write_uint32_(coap_serializer_t* s, uint32_t src) {
  return coap_s_write_(s, (const char*)&src, 4);
}

static int coap_p_read_(coap_parser_t* p, char* dst, size_t dst_len) {
  if (p->buf_len - p->cursor < dst_len) {
    return -1;
  }
  memcpy(dst, &p->buf[p->cursor], dst_len);
  p->cursor += dst_len;
  return 0;
}

int coap_serializer_create(coap_serializer_t** s, void* buf, size_t len,
                           char* dst_buf, size_t dst_len) {
  if (buf == NULL || sizeof(coap_serializer_t) > len || dst_buf == NULL ||
      dst_len < COAP_LEN_HEADER) {
    return COAP_ERR_ARG;
  }
  *s = (coap_serializer_t*)buf;
  memset(*s, 0, sizeof(coap_serializer_t));
  (*s)->buf_len = dst_len;
  (*s)->buf = dst_buf;
  return COAP_OK;
}

int coap_serializer_init(coap_serializer_t* s, uint8_t type, uint8_t code,
                         uint8_t token_len) {
  if (s == NULL || token_len > COAP_MAXLEN_TOKEN) {
    return COAP_ERR_ARG;
  }
  if (validate_type_code_(type, code)) {
    return COAP_ERR_ARG;
  }
  s->cursor = 0;
  s->sum_of_delta = 0;
  s->token_len = token_len;
  s->executed = 0;
  coap_s_write_uint32_(
    s, htonl(COAP_VERSION | (type << 28) | (code << 16) | (token_len << 24)));
  if (s->cursor + token_len > s->buf_len) {
    return COAP_ERR_LIMIT;
  }
  s->cursor += token_len;
  return COAP_OK;
}

int coap_serializer_add_opt(coap_serializer_t* s, uint16_t opt, const char* val,
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
    return COAP_ERR_ARG;
  }
  if (s->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  if (is_well_known_(opt)) {
    switch (opt) {
      // empty
      case O_IF_NONE_MATCH:
        if (val != NULL || len != 0) {
          return COAP_ERR_ARG;
        }
        if (s->sum_of_delta == opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      // opaque
      case O_ETAG:
        if (val == NULL || len == 0) {
          return COAP_ERR_ARG;
        }
      case O_IF_MATCH:
        if (len > 8) {
          return COAP_ERR_ARG;
        }
        break;
      // uint
      case O_URI_PORT:
      case O_CONTENT_FORMAT:
      case O_ACCEPT:
        if (len > 2) {
          return COAP_ERR_ARG;
        }
        if (s->sum_of_delta == opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_OBSERVE:
        if (len > 3) {
          return COAP_ERR_ARG;
        }
        if (s->sum_of_delta == opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_MAX_AGE:
      case O_SIZE1:
        if (len > 4) {
          return COAP_ERR_ARG;
        }
        if (s->sum_of_delta == opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      // string
      case O_URI_HOST:
      case O_PROXY_SCHEME:
        if (val == NULL || len == 0 || len > 255) {
          return COAP_ERR_ARG;
        }
        if (s->sum_of_delta == opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      case O_LOCATION_PATH:
      case O_URI_PATH:
      case O_URI_QUERY:
      case O_LOCATION_QUERY:
        if (len > 255) {
          return COAP_ERR_ARG;
        }
        break;
      case O_PROXY_URI:
        if (val == NULL || len == 0 || len > 1034) {
          return COAP_ERR_ARG;
        }
        if (s->sum_of_delta == opt) {
          return COAP_ERR_INVALID_CALL;
        }
        break;
      default:
        return COAP_ERR_ARG;
    }
  }
  // Except well-known options, all other options are regarded as Repeatable
  // options.
  if (s->sum_of_delta > opt) {
    return COAP_ERR_INVALID_CALL;
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
  } else if (len <= 255 + 13) {  // 255 + 13
    opt_delta_and_len |= 13;
  } else {
    opt_delta_and_len |= 14;
  }
  if (coap_s_write_uint8_(s, opt_delta_and_len)) {
    return COAP_ERR_LIMIT;
  }

  // Option Delta (extended)
  if (opt - s->sum_of_delta < 13) {
  } else if (opt - s->sum_of_delta <= 255 + 13) {
    if (coap_s_write_uint8_(s, (opt - s->sum_of_delta) - 13)) {
      return COAP_ERR_LIMIT;
    }
  } else {
    if (coap_s_write_uint16_(s, (opt - s->sum_of_delta) - 269)) {
      return COAP_ERR_LIMIT;
    }
  }

  // Option Length (extended)
  if (len < 13) {
  } else if (len <= 255 + 13) {
    if (coap_s_write_uint8_(s, len - 13)) {
      return COAP_ERR_LIMIT;
    }
  } else {
    if (coap_s_write_uint16_(s, len - 269)) {
      return COAP_ERR_LIMIT;
    }
  }

  // Option Value
  if (coap_s_write_(s, val, len)) {
    return COAP_ERR_LIMIT;
  }
  s->sum_of_delta = opt;
  return COAP_OK;
}

int coap_serializer_add_opt_uint(coap_serializer_t* s, uint16_t opt,
                                 uint32_t val) {
  uint8_t nbo8;
  uint16_t nbo16;
  uint32_t nbo32;
  if (is_well_known_(opt)) {
    switch (opt) {
      case O_URI_PORT:
      case O_CONTENT_FORMAT:
      case O_ACCEPT:
      case O_OBSERVE:
      case O_MAX_AGE:
      case O_SIZE1:
        break;
      default:
        return COAP_ERR_ARG;
    }
  }
  if (val == 0) {
    return coap_serializer_add_opt(s, opt, NULL, 0);
  }
  if (val < 256) {
    nbo8 = (uint8_t)val;
    return coap_serializer_add_opt(s, opt, (const char*)&nbo8, 1);
  }
  if (val < 65536) {
    nbo16 = htons((uint16_t)val);
    return coap_serializer_add_opt(s, opt, (const char*)&nbo16, 2);
  }
  nbo32 = htonl(val);
  return coap_serializer_add_opt(s, opt, (const char*)&nbo32, 4);
}

int coap_serializer_exec(coap_serializer_t* s, uint16_t mid, const char* token,
                         const char* payload, size_t payload_len,
                         size_t* msg_len) {
  uint16_t nbo_mid = htons(mid);
  if (s == NULL) {
    return COAP_ERR_ARG;
  }
  memcpy(&s->buf[2], &nbo_mid, 2);
  if (token) {
    memcpy(&s->buf[4], token, s->token_len);
  }
  if (payload == NULL || payload_len == 0) {
    *msg_len = s->cursor;
    s->executed = 1;
    return COAP_OK;
  }
  if (s->executed) {
    s->cursor = s->payload;
  }
  if (!s->executed || s->sum_of_delta == 0) {
    if (coap_s_write_uint8_(s, 0xFF)) {
      return COAP_ERR_LIMIT;
    }
  }
  s->payload = s->cursor;
  if (coap_s_write_(s, payload, payload_len)) {
    return COAP_ERR_LIMIT;
  }
  *msg_len = s->cursor;
  s->executed = 1;
  return COAP_OK;
}

int coap_parser_create(coap_parser_t** p, const char* buf, size_t len) {
  if (buf == NULL || sizeof(coap_parser_t) > len) {
    return COAP_ERR_ARG;
  }
  *p = (coap_parser_t*)buf;
  memset(*p, 0, sizeof(coap_parser_t));
  return COAP_OK;
}

int coap_parser_init(coap_parser_t* p, const coap_parser_settings_t* s) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (s == NULL) {
    p->cookie = NULL;
    p->on_begin = NULL;
    p->on_header = NULL;
    p->on_opt = NULL;
    p->on_payload = NULL;
    p->on_complete = NULL;
  } else {
    p->cookie = s->cookie;
    p->on_begin = s->on_begin;
    p->on_header = s->on_header;
    p->on_opt = s->on_opt;
    p->on_payload = s->on_payload;
    p->on_complete = s->on_complete;
  }
  return COAP_OK;
}

int coap_parser_exec(coap_parser_t* p, const char* buf, size_t len) {
  uint32_t header;
  if (p == NULL || buf == NULL || len == 0) {
    return COAP_ERR_ARG;
  }
  p->buf_len = len;
  p->buf = buf;
  p->cursor = 0;
  p->executed = 0;

  // Parse CoAP header.
  if (p->on_begin) {
    p->on_begin(p->cookie);
  }
  if (coap_p_read_(p, (char*)&header, 4)) {
    return COAP_ERR_SYNTAX;
  }
  header = ntohl(header);
  p->version = header >> 30;
  if (p->version != 1) {
    return COAP_ERR_SYNTAX;
  }
  p->type = (header & 0x30000000) >> 28;
  p->code = (header & 0x00FF0000) >> 16;
  if (validate_type_code_(p->type, p->code)) {
    return COAP_ERR_SYNTAX;
  }
  p->mid = (header & 0x0000FFFF);
  p->token_len = (header & 0x0F000000) >> 24;
  if (p->token_len > 8) {
    return COAP_ERR_SYNTAX;
  }
  if (p->on_header) {
    p->on_header(p->cookie, p->type, p->code, p->mid, &p->buf[p->cursor],
                 p->token_len);
  }
  p->cursor += p->token_len;

  // Parse CoAP options.
  uint16_t sum_of_delta = 0;
  uint16_t opt;
  uint16_t opt_len;
  uint8_t b;
  while (p->cursor < p->buf_len) {
    coap_p_read_(p, (char*)&b, 1);
    if (b == 0xFF) {
      p->payload = p->cursor;
      p->executed = 1;
      if (p->on_payload) {
        p->on_payload(p->cookie, &p->buf[p->cursor], p->buf_len - p->cursor);
      }
      if (p->on_complete) {
        p->on_complete(p->cookie);
      }
      return COAP_OK;
    }
    opt = b >> 4;
    opt_len = 0x0F & b;
    // if (i < 4) printf("opt:%d,%d\n", opt, opt_len);

    // Get the number of an option.
    if (opt < 13) {
    } else if (opt == 13) {
      if (coap_p_read_(p, (char*)&b, 1)) {
        return COAP_ERR_SYNTAX;
      }
      opt = b + 13;
    } else if (opt == 14) {
      if (coap_p_read_(p, (char*)&opt, 2)) {
        return COAP_ERR_SYNTAX;
      }
      opt = ntohs(opt) + 269;
    } else {
      return COAP_ERR_SYNTAX;
    }

    // Get the length of an option.
    if (opt_len < 13) {
    } else if (opt_len == 13) {
      if (coap_p_read_(p, (char*)&b, 1)) {
        return COAP_ERR_SYNTAX;
      }
      opt_len = b + 13;
    } else if (opt_len == 14) {
      if (coap_p_read_(p, (char*)&opt_len, 2)) {
        return COAP_ERR_SYNTAX;
      }
      opt_len = ntohs(opt_len) + 269;
    } else {
      return COAP_ERR_SYNTAX;
    }

    // Validate the option.
    if (is_well_known_(sum_of_delta + opt)) {
      switch (sum_of_delta + opt) {
        // empty
        case O_IF_NONE_MATCH:
          if (opt_len != 0) {
            return COAP_ERR_SYNTAX;
          }
          if (opt == 0) {
            return COAP_ERR_SYNTAX;
          }
          break;
        // opaque
        case O_ETAG:
          if (opt_len == 0) {
            return COAP_ERR_SYNTAX;
          }
        case O_IF_MATCH:
          if (opt_len > 8) {
            return COAP_ERR_SYNTAX;
          }
          break;
        // uint
        case O_URI_PORT:
        case O_CONTENT_FORMAT:
        case O_ACCEPT:
          if (opt_len > 2) {
            return COAP_ERR_SYNTAX;
          }
          if (opt == 0) {
            return COAP_ERR_SYNTAX;
          }
          break;
        case O_OBSERVE:
          if (opt_len > 3) {
            return COAP_ERR_SYNTAX;
          }
          if (opt == 0) {
            return COAP_ERR_SYNTAX;
          }
          break;
        case O_MAX_AGE:
        case O_SIZE1:
          if (opt_len > 4) {
            return COAP_ERR_SYNTAX;
          }
          if (opt == 0) {
            return COAP_ERR_SYNTAX;
          }
          break;
        // string
        case O_URI_HOST:
        case O_PROXY_SCHEME:
          if (opt_len == 0 || opt_len > 255) {
            return COAP_ERR_SYNTAX;
          }
          if (opt == 0) {
            return COAP_ERR_SYNTAX;
          }
          break;
        case O_LOCATION_PATH:
        case O_URI_PATH:
        case O_URI_QUERY:
        case O_LOCATION_QUERY:
          if (opt_len > 255) {
            return COAP_ERR_SYNTAX;
          }
          break;
        case O_PROXY_URI:
          if (opt_len == 0 || opt_len > 1034) {
            return COAP_ERR_SYNTAX;
          }
          if (opt == 0) {
            return COAP_ERR_SYNTAX;
          }
          break;
        default:
          return COAP_ERR_SYNTAX;
      }
    }
    opt += sum_of_delta;
    sum_of_delta = opt;
    if (p->on_opt) {
      p->on_opt(p->cookie, opt, &p->buf[p->cursor], opt_len);
    }
    p->cursor += opt_len;
  }
  p->executed = 1;
  if (p->on_complete) {
    p->on_complete(p->cookie);
  }
  return COAP_OK;
}

int coap_parser_get_type(const coap_parser_t* p, coap_type_t* res) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (!p->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  *res = p->type;
  return COAP_OK;
}

int coap_parser_get_code(const coap_parser_t* p, coap_code_t* res) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (!p->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  *res = p->code;
  return COAP_OK;
}

int coap_parser_get_mid(const coap_parser_t* p, uint16_t* res) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (!p->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  *res = p->mid;
  return COAP_OK;
}

int coap_parser_get_token(const coap_parser_t* p, const char** res,
                          uint8_t* len) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (!p->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  *len = p->token_len;
  if (p->token_len > 0) {
    *res = &p->buf[4];
  } else {
    *res = NULL;
  }
  return COAP_OK;
}

int coap_parser_get_path(const coap_parser_t* p, const char** res,
                         size_t* len) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (!p->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  return COAP_OK;
}

int coap_parser_get_payload(const coap_parser_t* p, const char** res,
                            size_t* len) {
  if (p == NULL) {
    return COAP_ERR_ARG;
  }
  if (!p->executed) {
    return COAP_ERR_INVALID_CALL;
  }
  *res = &p->buf[p->payload];
  *len = p->buf_len - p->payload;
  return COAP_OK;
}

size_t coap_serializer_size() { return sizeof(coap_serializer_t); }
size_t coap_parser_size() { return sizeof(coap_parser_t); }
