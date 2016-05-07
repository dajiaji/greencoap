# GreenCoAP

GreenCoAP is a CoAP (RFC7252: The Constrained Application Protocol) serializer/parser implementation written in C.

Features:

  * Support RFC7252. <http://tools.ietf.org/html/rfc7252>
  * Support static memory allocation. (Can work without malloc.)
  * No dependencies.

## Usage

```c
char workspace[512] = {};
char send_buf[2048] = {};
gcoap_serializer_t* s = NULL;

// How to use the gcoap_serializer_t.
gcoap_serializer_create(&s, workspace, 512);
gcoap_serializer_init(s, T_CON | C_GET, "token", sizeof("token")-1, NULL, 0);
gcoap_serializer_add_opt(s, O_URI_HOST, "example.com", sizeof("example.com")-1);
gcoap_serializer_add_opt_uint(s, O_URI_PORT, 5683);
gcoap_serializer_add_opt(s, O_URI_PATH, "/temperature", sizeof("/temperature")-1);
gcoap_serializer_exec(s, send_buf, 2048);

// How to use the gcoap_parser_t.
gcoap_parser_create(&p, workspace, 512);
gcoap_parser_exec(p, send_buf, 2048);

printf("type: %u\n", gcoap_parser_get_type(p));
printf("code: %u\n", gcoap_parser_get_code(p));

gcoap_serializer_destroy(s);
gcoap_parser_destroy(p);

```
