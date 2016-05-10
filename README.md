# GreenCoAP

GreenCoAP is a CoAP (RFC7252: The Constrained Application Protocol) serializer/parser implementation written in C.

Features:

  * Support RFC7252. <http://tools.ietf.org/html/rfc7252>
  * Support static memory allocation. (No need to use malloc.)
  * No dependencies.

## Usage

```c

char buf[2048] = {};
size_t res_len = 0;
const char* res = NULL;

// How to use coap_serializer.
coap_serializer* s = NULL;
coap_serializer_init(&s, malloc(coap_serializer_size()), coap_serializer_size());
coap_serializer_begin(s, buf, 2048);
coap_serializer_set_header(s, T_CON | C_POST, "token", sizeof("token")-1);
coap_serializer_add_opt_string(s, O_URI_HOST, "example.com", sizeof("example.com")-1);
coap_serializer_add_opt_uint(s, O_URI_PORT, 5683);
coap_serializer_add_opt_string(s, O_URI_PATH, "temperature", sizeof("temperature")-1);
coap_serializer_set_payload(s, "22.3 C", sizeof("22.3 C")-1);

// How to use coap_parser.
coap_parser* p = NULL;
coap_parser_init(&p, malloc(coap_parser_size()), coap_parser_size());
coap_parser_exec(p, buf, 2048);
coap_parser_get_payload(p, &res, &len);

printf("payload: %.*s\n", res_len, res); // 22.3 C

free(s);
free(p);

```
