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
  COAP_ERR_INVALID_ARGUMENT = -1,
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
  T_NON = 1 << 28,
  T_ACK = 2 << 28,
  T_RST = 3 << 28,
} coap_type_t;

/**
 * CoAP message codes.
 */
typedef enum coap_code_t {
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
  // RESERVED = 128,
  // RESERVED = 132,
  // RESERVED = 136,
  // RESERVED = 140,
} coap_opt_number_t;

/** CoAP serializer */
typedef struct coap_serializer coap_serializer;

/** CoAP parser */
typedef struct coap_parser coap_parser;

/**
 * Create a CoAP serializer (coap_serializer) with fixed size memory space.
 */
int coap_serializer_create(coap_serializer** s, void* buf, size_t len,
                           char* dst_buf, size_t dst_len);

/**
 * Initialize a CoAP serializer with a CoAP header and a token.
 */
int coap_serializer_init(coap_serializer* s, uint32_t header,
                         uint8_t token_len);

/**
 * Add an option.
 */
int coap_serializer_add_opt(coap_serializer* s, uint16_t opt, const char* val,
                            size_t len);
/**
 * Add a uint option.
 */
int coap_serializer_add_opt_uint(coap_serializer* s, uint16_t opt,
                                 uint32_t val);

/**
 * Execute (Finalize) the CoAP serializer with a payload.
 */
int coap_serializer_exec(coap_serializer* s, uint16_t mid, const char* token,
                         const char* payload, size_t payload_len,
                         size_t* msg_len);

/**
 * Create a CoAP parser (coap_parser) with fixed size memory space.
 */
int coap_parser_create(coap_parser** p, const char* buf, size_t len);

/**
 * Parse a given buffer as a CoAP message.
 */
int coap_parser_exec(coap_parser* p, const char* buf, size_t len);

/**
 * Get the CoAP message type.
 */
int coap_parser_get_type(const coap_parser* p, coap_type_t* res);

/**
 * Get the CoAP message code.
 */
int coap_parser_get_code(const coap_parser* p, coap_code_t* res);

/**
 * Get the CoAP message id.
 */
int coap_parser_get_mid(const coap_parser* p, uint16_t* res);

/**
 * Get the CoAP message token.
 */
int coap_parser_get_token(const coap_parser* p, const char** res, uint8_t* len);

/**
 * Get the value of path.
 */
int coap_parser_get_path(const coap_parser* p, const char** buf, size_t* len);

/**
 * Get a CoAP message payload.
 */
int coap_parser_get_payload(const coap_parser* p, const char** buf,
                            size_t* len);

/**
 * Get the size of coap_serializer.
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
