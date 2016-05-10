#ifndef _GREENCOAP_H_
#define _GREENCOAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>

/**
 * Status codes for greencoap APIs.
 */
typedef enum gcoap_status_t {
  GCOAP_OK = 0,
  GCOAP_ERR_INVALID_ARGUMENT = -1,
  GCOAP_ERR_LIMIT = -2,
  GCOAP_ERR_INVALID_CALL = -3,
  GCOAP_ERR_SYNTAX = -4,
  GCOAP_ERR_SYSTEM = -5,
  GCOAP_ERR_INTERNAL = -6,
  GCOAP_ERR_UNKNOWN = -7,
} gcoap_status_t;

/**
 * CoAP message types.
 */
typedef enum gcoap_type_t {
  T_CON = 0,
  T_NON = 1 << 28,
  T_ACK = 2 << 28,
  T_RST = 3 << 28,
} gcoap_type_t;

/**
 * CoAP message codes.
 */
typedef enum gcoap_code_t {
  // 0.xx
  C_GET = 1 << 16,     // 0.01
  C_POST = 2 << 16,    // 0.02
  C_PUT = 3 << 16,     // 0.03
  C_DELETE = 4 << 16,  // 0.04
  // 2.xx
  C_CREATED = 2 << 21 | 1 << 16,  // 2.01
  C_DELETED = 2 << 21 | 2 << 16,  // 2.02
  C_VALID = 2 << 21 | 3 << 16,    // 2.03
  C_CHANGED = 2 << 21 | 4 << 16,  // 2.04
  C_CONTENT = 2 << 21 | 5 << 16,  // 2.05
  // 4.xx
  C_BAD_REQUEST = 4 << 21,                            // 4.00
  C_UNAUTHORIZED = 4 << 21 | 1 << 16,                 // 4.01
  C_BAD_OPTION = 4 << 21 | 2 << 16,                   // 4.02
  C_FORBIDDEN = 4 << 21 | 3 << 16,                    // 4.03
  C_NOT_FOUND = 4 << 21 | 4 << 16,                    // 4.04
  C_METHOD_NOT_ALLOWED = 4 << 21 | 5 << 16,           // 4.05
  C_NOT_ACCEPTABLE = 4 << 21 | 6 << 16,               // 4.06
  C_PRECONDITION_FAILED = 4 << 21 | 12 << 16,         // 4.12
  C_REQUEST_ENTITY_TOO_LARGE = 4 << 21 | 13 << 16,    // 4.13
  C_UNSUPPORTED_CONTENT_FORMAT = 4 << 21 | 15 << 16,  // 4.15
  // 5.xx
  C_INTERNAL_SERVER_ERROR = 5 << 21,             // 5.00
  C_NOT_IMPLEMENTED = 5 << 21 | 1 << 16,         // 5.01
  C_BAD_GATEWAY = 5 << 21 | 2 << 16,             // 5.02
  C_SERVICE_UNAVAILABLE = 5 << 21 | 3 << 16,     // 5.03
  C_GATEWAY_TIMEOUT = 5 << 21 | 4 << 16,         // 5.04
  C_PROXYING_NOT_SUPPORTED = 5 << 21 | 5 << 16,  // 5.05
} gcoap_code_t;

/**
 * CoAP options.
 */
typedef enum gcoap_opt_number_t {
  O_IF_MATCH = 1,
  O_URI_HOST = 3,
  O_ETAG = 4,
  O_IF_NONE_MATCH = 5,
  O_URI_PORT = 7,
  O_LOCATION_PATH = 8,
  O_URI_PATH = 11,
  O_CONTENT_FORMAT = 12,
  O_MAX_AGE = 14,
  O_URI_QUERY = 15,
  O_ACCEPT = 17,
  O_LOCATION_QUERY = 20,
  O_PROXY_URI = 35,
  O_PROXY_SCHEME = 39,
  O_SIZE1 = 60,
  // O_RESERVED = 128,
  // O_RESERVED = 132,
  // O_RESERVED = 136,
  // O_RESERVED = 140,
} gcoap_opt_number_t;

/** CoAP serializer */
typedef struct gcoap_serializer gcoap_serializer;

/** CoAP parser */
typedef struct gcoap_parser gcoap_parser;

/**
 * Create a CoAP serializer (gcoap_serializer) with fixed size memory space.
 */
int gcoap_serializer_init(gcoap_serializer** s, void* buf, size_t len);

/**
 * Initialize the CoAP serializer with fixed size write buffer.
 */
int gcoap_serializer_begin(gcoap_serializer* s, char* buf, size_t len);

/**
 * Set a CoAP header and a token.
 */
int gcoap_serializer_set_header(gcoap_serializer* s, uint32_t header,
                                const char* token, uint8_t token_len);

/**
 * Add a raw option.
 */
int gcoap_serializer_add_opt(gcoap_serializer* s, uint16_t opt, const char* val,
                             size_t len);
/**
 * Add an empty option.
 */
int gcoap_serializer_add_opt_empty(gcoap_serializer* s, uint16_t opt);

/**
 * Add an opaque option.
 */
int gcoap_serializer_add_opt_opaque(gcoap_serializer* s, uint16_t opt,
                                    const char* val, size_t len);

/**
 * Add a uint option.
 */
int gcoap_serializer_add_opt_uint(gcoap_serializer* s, uint16_t opt,
                                  uint32_t val);

/**
 * Add a string option.
 */
int gcoap_serializer_add_opt_string(gcoap_serializer* s, uint16_t opt,
                                    const char* val, size_t len);

/**
 * Set a payload.
 */
int gcoap_serializer_set_payload(gcoap_serializer* s, const char* buf,
                                 size_t len);

/**
 * Create a CoAP parser (gcoap_parser) with fixed size memory space.
 */
int gcoap_parser_init(gcoap_parser** p, const char* buf, size_t len);

/**
 * Parse a given buffer as a CoAP message.
 */
int gcoap_parser_exec(gcoap_parser* p, const char* buf, size_t len);

/**
 * Get the CoAP message type.
 */
int gcoap_parser_get_type(const gcoap_parser* p, gcoap_type_t* type);

/**
 * Get the CoAP message code.
 */
int gcoap_parser_get_code(const gcoap_parser* p, gcoap_code_t* code);

/**
 * Get the value of path.
 */
int gcoap_parser_get_path(const gcoap_parser* p, const char** buf, size_t* len);

/**
 * Get a CoAP message payload.
 */
int gcoap_parser_get_payload(const gcoap_parser* p, const char** buf,
                             size_t* len);

/**
 * Get the size of gcoap_serializer.
 */
size_t gcoap_serializer_size();

/**
 * Get the size of gcoap_parser.
 */
size_t gcoap_parser_size();

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_H_ */
