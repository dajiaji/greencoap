# GreenCoAP

GreenCoAP is a CoAP (RFC7252) parser/serializer implementation written in C.

Features:

* Support following specifications related to CoAP.
  * RFC7252: "The Constained Application Protocol" <http://tools.ietf.org/html/rfc7252>
  * RFC7649: "Observing Resources in Constrained Application Protocol (CoAP)"  <http://tools.ietf.org/html/rfc7649>
  * draft-ietf-core-block-20: "Block-wise transfers in CoAP" <https://tools.ietf.org/html/draft-ietf-core-block-20>
* Support static memory allocation. (No need to use malloc.)
* No dependencies.
* Small footprint.

## Usage

```c

char buf[64] = {};
char token = 0x20;
size_t res_len = 0;
const char* res = NULL;

// How to use coap_serializer_t.
coap_serializer_t* s = NULL;
coap_serializer_create(&s, malloc(coap_serializer_size()), coap_serializer_size(), buf, 64);
coap_serializer_init(s, T_CON, C_POST, 1);
coap_serializer_add_opt(s, O_URI_HOST, "example.com", sizeof("example.com")-1);
coap_serializer_add_opt_uint(s, O_URI_PORT, 5683);
coap_serializer_add_opt(s, O_URI_PATH, "temperature", sizeof("temperature")-1);
coap_serializer_add_opt_uint(s, O_CONTENT_FORMAT, F_TEXT_PLAIN);
coap_serializer_exec(s, 0x7d34, &token, "22.3 C", sizeof("22.3 C")-1, &res_len);

// How to use coap_parser.
coap_parser_t* p = NULL;
coap_parser_create(&p, malloc(coap_parser_size()), coap_parser_size());
coap_parser_exec(p, buf, 64);
coap_parser_get_payload(p, &res, &res_len);

printf("payload: %.*s\n", res_len, res); // 22.3 C

free(s);
free(p);

```
