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

/**
 * CoAP serializer.
 */
struct coap_serializer {
  size_t buf_len;
  char* buf;
  size_t cursor;
  size_t payload;
  uint16_t sum_of_delta;
  uint8_t executed;
};

/**
 * CoAP parser.
 */
struct coap_parser {
  size_t buf_len;
  const char* buf;
  size_t cursor;
  size_t payload;
  uint32_t header;
  uint8_t token_len;
  uint8_t executed;
};

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_I_H_ */
