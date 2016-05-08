# GreenCoAP

GreenCoAP is a CoAP (RFC7252: The Constrained Application Protocol) serializer/parser implementation written in C.

Features:

  * Support RFC7252. <http://tools.ietf.org/html/rfc7252>
  * Support static memory allocation. (No need to use malloc.)
  * No dependencies.

## Usage

```c
char* buf[2048] = {};
gcoap_serializer* s = gcoap_serializer_init(malloc(gcoap_serializer_size()), gcoap_serializer_size());
gcoap_parser* p = gcoap_parser_init(malloc(gcoap_parser_size(), gcoap_serializer_size());

// How to use the gcoap_serializer_t.
gcoap_serializer_begin(s, buf, 2048);
gcoap_serializer_set_header(s, T_CON | C_GET, "token", sizeof("token")-1);
//gcoap_serializer_add_opt(s, O_URI_HOST, "example.com", sizeof("example.com")-1);
//gcoap_serializer_add_opt_uint(s, O_URI_PORT, 5683);
gcoap_serializer_add_opt(s, O_URI_PATH, "/temperature", sizeof("/temperature")-1);

// How to use the gcoap_parser_t.
gcoap_parser_create(&p, workspace, 512);
gcoap_parser_exec(p, send_buf, 2048);

printf("type: %u\n", gcoap_parser_get_type(p));
printf("code: %u\n", gcoap_parser_get_code(p));

gcoap_serializer_destroy(s);
gcoap_parser_destroy(p);

```
