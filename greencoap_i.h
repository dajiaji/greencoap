#ifndef _GREENCOAP_I_H_
#define _GREENCOAP_I_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "greencoap.h"
#include <assert.h>
#include <string.h>

#define GCOAP_COAP_VERSION (1 << 30)

#define GCOAP_TRUE 1
#define GCOAP_FALSE 0

#define GCOAP_LEN_HEADER 4
#define GCOAP_MAXLEN_TOKEN 8

#define IS_REQUEST_(msg_type) (((msg_type)&0x1001) == 1)

typedef uint8_t gcoap_bool_t;

/**
 * Internal states for gcoap_serializer.
 */
typedef enum gcoap_serializer_state_t {
  GCOAP_S_STATE_INIT = 0,
  GCOAP_S_STATE_WRITTEN_HEADER,
  GCOAP_S_STATE_WRITTEN_PAYLOAD,
} gcoap_serializer_state_t;

/**
 * Internal states for gcoap_parser.
 */
typedef enum gcoap_parser_state_t {
  GCOAP_P_STATE_INIT = 0,
  GCOAP_P_STATE_READ_HEADER,
  GCOAP_P_STATE_READ_PAYLOAD,
} gcoap_parser_state_t;

/**
 * General buffer.
 */
typedef struct gcoap_buf_t {
  uint16_t len;
  const char* val;
} gcoap_buf_t;

/**
 * CoAP serializer.
 */
struct gcoap_serializer {
  size_t buf_len;
  char* buf;
  size_t cursor;
  gcoap_serializer_state_t state;
};

/**
 * CoAP parser.
 */
struct gcoap_parser {
  size_t size;
  size_t cursor;
  const char* token;
  size_t token_len;
  gcoap_parser_state_t state;
};

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_I_H_ */
