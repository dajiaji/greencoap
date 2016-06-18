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
typedef enum coap_status_t {
  COAP_OK = 0,
  COAP_ERR_ARG = -1,
  COAP_ERR_LIMIT = -2,
  COAP_ERR_INVALID_CALL = -3,
  COAP_ERR_SYNTAX = -4,
  COAP_ERR_SYSTEM = -5,
  COAP_ERR_INTERNAL = -6,
  COAP_ERR_UNKNOWN = -7,
} coap_status_t;

/**
 * CoAP message types.
 */
typedef enum coap_type_t {
  T_CON = 0,
  T_NON = 1,
  T_ACK = 2,
  T_RST = 3,
} coap_type_t;

/**
 * CoAP message codes.
 */
typedef enum coap_code_t {
  // 0.xx
  C_GET = 1,     // 0.01
  C_POST = 2,    // 0.02
  C_PUT = 3,     // 0.03
  C_DELETE = 4,  // 0.04
  // 2.xx
  C_CREATED = 2 << 5 | 1,  // 2.01
  C_DELETED = 2 << 5 | 2,  // 2.02
  C_VALID = 2 << 5 | 3,    // 2.03
  C_CHANGED = 2 << 5 | 4,  // 2.04
  C_CONTENT = 2 << 5 | 5,  // 2.05
  // 4.xx
  C_BAD_REQUEST = 4 << 5,                      // 4.00
  C_UNAUTHORIZED = 4 << 5 | 1,                 // 4.01
  C_BAD_OPTION = 4 << 5 | 2,                   // 4.02
  C_FORBIDDEN = 4 << 5 | 3,                    // 4.03
  C_NOT_FOUND = 4 << 5 | 4,                    // 4.04
  C_METHOD_NOT_ALLOWED = 4 << 5 | 5,           // 4.05
  C_NOT_ACCEPTABLE = 4 << 5 | 6,               // 4.06
  C_PRECONDITION_FAILED = 4 << 5 | 12,         // 4.12
  C_REQUEST_ENTITY_TOO_LARGE = 4 << 5 | 13,    // 4.13
  C_UNSUPPORTED_CONTENT_FORMAT = 4 << 5 | 15,  // 4.15
  // 5.xx
  C_INTERNAL_SERVER_ERROR = 5 << 5,       // 5.00
  C_NOT_IMPLEMENTED = 5 << 5 | 1,         // 5.01
  C_BAD_GATEWAY = 5 << 5 | 2,             // 5.02
  C_SERVICE_UNAVAILABLE = 5 << 5 | 3,     // 5.03
  C_GATEWAY_TIMEOUT = 5 << 5 | 4,         // 5.04
  C_PROXYING_NOT_SUPPORTED = 5 << 5 | 5,  // 5.05
} coap_code_t;

/**
 * CoAP options.
 */
typedef enum coap_opt_number_t {
  // RESERVED = 0,
  O_IF_MATCH = 1,
  O_URI_HOST = 3,
  O_ETAG = 4,
  O_IF_NONE_MATCH = 5,
  O_OBSERVE = 6,
  O_URI_PORT = 7,
  O_LOCATION_PATH = 8,
  O_URI_PATH = 11,
  O_CONTENT_FORMAT = 12,
  O_MAX_AGE = 14,
  O_URI_QUERY = 15,
  O_ACCEPT = 17,
  O_LOCATION_QUERY = 20,
  O_BLOCK2 = 23,
  O_BLOCK1 = 27,
  O_PROXY_URI = 35,
  O_PROXY_SCHEME = 39,
  O_SIZE1 = 60,
  // RESERVED = 128,
  // RESERVED = 132,
  // RESERVED = 136,
  // RESERVED = 140,
} coap_opt_number_t;

/**
 * CoAP media types.
 */
const uint16_t F_TEXT_PLAIN = 0;
const uint16_t F_APPLICATION_LINK_FORMAT = 40;
const uint16_t F_APPLICATION_XML = 41;
const uint16_t F_APPLICATION_OCTET_STREAM = 42;
const uint16_t F_APPLICATION_EXI = 47;
const uint16_t F_APPLICATION_JSON = 50;

/** CoAP serializer */
typedef struct coap_serializer_t coap_serializer_t;

/** CoAP parser */
typedef struct coap_parser_t coap_parser_t;

/** CoAP parser callback functions */
typedef void (*coap_parser_cb_t)(void*);
typedef void (*coap_parser_cb_header_t)(void*, coap_type_t, coap_code_t,
                                        uint16_t, const char*, uint8_t);
typedef void (*coap_parser_cb_opt_t)(void*, uint16_t, const void*, uint16_t);
typedef void (*coap_parser_cb_payload_t)(void*, const char*, size_t);

/** CoAP parser */
typedef struct coap_parser_settings_t {
  void* cookie;
  coap_parser_cb_t on_begin;
  coap_parser_cb_header_t on_header;
  coap_parser_cb_opt_t on_opt;
  coap_parser_cb_payload_t on_payload;
  coap_parser_cb_t on_complete;
} coap_parser_settings_t;

/**
 * Create a CoAP serializer with fixed size memory space.
 */
int coap_serializer_create(coap_serializer_t** s, void* buf, size_t len,
                           char* dst_buf, size_t dst_len);

/**
 * Initialize the CoAP serializer with message type, code and token
 * length.
 */
int coap_serializer_init(coap_serializer_t* s, uint8_t type, uint8_t code,
                         uint8_t token_len);

/**
 * Add an option.
 */
int coap_serializer_add_opt(coap_serializer_t* s, uint16_t opt, const char* val,
                            size_t len);
/**
 * Add a uint option.
 */
int coap_serializer_add_opt_uint(coap_serializer_t* s, uint16_t opt,
                                 uint32_t val);

/**
 * Execute (Finalize) the CoAP serializer with message id, token, and payload.
 */
int coap_serializer_exec(coap_serializer_t* s, uint16_t mid, const char* token,
                         const char* payload, size_t payload_len,
                         size_t* msg_len);

/**
 * Create a CoAP parser (coap_parser_t) with fixed size memory space.
 */
int coap_parser_create(coap_parser_t** p, const char* buf, size_t len);

/**
 * Initialize a CoAP parser (coap_parser_t) with fixed size memory space.
 */
int coap_parser_init(coap_parser_t* p, const coap_parser_settings_t* s);

/**
 * Parse a given buffer as a CoAP message.
 */
int coap_parser_exec(coap_parser_t* p, const char* buf, size_t len);

/**
 * Get the CoAP message type.
 */
int coap_parser_get_type(const coap_parser_t* p, coap_type_t* res);

/**
 * Get the CoAP message code.
 */
int coap_parser_get_code(const coap_parser_t* p, coap_code_t* res);

/**
 * Get the CoAP message id.
 */
int coap_parser_get_mid(const coap_parser_t* p, uint16_t* res);

/**
 * Get the CoAP message token.
 */
int coap_parser_get_token(const coap_parser_t* p, const char** res,
                          uint8_t* len);

/**
 * Get the value of path.
 */
int coap_parser_get_path(const coap_parser_t* p, const char** buf, size_t* len);

/**
 * Get a CoAP message payload.
 */
int coap_parser_get_payload(const coap_parser_t* p, const char** buf,
                            size_t* len);

/**
 * Get the size of coap_serializer_t.
 */
size_t coap_serializer_size();

/**
 * Get the size of coap_parser.
 */
size_t coap_parser_size();

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_H_ */
