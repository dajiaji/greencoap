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

#ifdef __cplusplus
}
#endif

#endif /* !_GREENCOAP_I_H_ */
