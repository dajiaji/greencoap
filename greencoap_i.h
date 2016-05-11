#ifndef _GREENCOAP_I_H_
#define _GREENCOAP_I_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "greencoap.h"
#include <assert.h>
#include <string.h>

#define COAP_VERSION (1 << 30)

#define COAP_LEN_HEADER 4
#define COAP_MAXLEN_TOKEN 8

//#define IS_REQUEST_(msg_type) (((msg_type)&0x1001) == 1)

// typedef uint8_t coap_bool_t;

/**
 * Internal states for coap_serializer.
 */
typedef enum coap_serializer_state_t {
  COAP_S_STATE_INIT = 0,
  COAP_S_STATE_ASSIGNED_BUFFER,
  COAP_S_STATE_WRITTEN_HEADER,
  COAP_S_STATE_WRITTEN_PAYLOAD,
} coap_serializer_state_t;

/**
 * Internal states for coap_parser.
 */
typedef enum coap_parser_state_t {
  COAP_P_STATE_INIT = 0,
  COAP_P_STATE_READ_HEADER,
  COAP_P_STATE_READ_PAYLOAD,
} coap_parser_state_t;

/**
 * General buffer.
 */
// typedef struct coap_buf_t {
//  uint16_t len;
//  const char* val;
//} coap_buf_t;

/**
 * CoAP serializer.
 */
struct coap_serializer {
  size_t buf_len;
  char* buf;
  size_t cursor;
  size_t payload;
  coap_serializer_state_t state;
  uint32_t sum_of_delta;
};

/**
 * CoAP parser.
 */
struct coap_parser {
  size_t buf_len;
  const char* buf;
  size_t cursor;
  uint32_t header;
  uint8_t token_len;
  size_t payload;
  coap_parser_state_t state;
};

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_I_H_ */
