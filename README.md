# GreenCoAP

GreenCoAP is a CoAP (RFC7252: The Constrained Application Protocol) serializer/parser implementation written in C.

Features:

  * Support RFC7252. <http://tools.ietf.org/html/rfc7252>
  * Support static memory allocation. (No need to use malloc.)
  * No dependencies.

## Usage

```c

char* buf[2048] = {};

// How to use gcoap_serializer.
gcoap_serializer* s = NULL;
gcoap_serializer_init(&s, malloc(gcoap_serializer_size()), gcoap_serializer_size());
gcoap_serializer_begin(s, buf, 2048);
gcoap_serializer_set_header(s, T_CON | C_GET, "token", sizeof("token")-1);
gcoap_serializer_add_opt(s, O_URI_HOST, "example.com", sizeof("example.com")-1);
gcoap_serializer_add_opt_uint(s, O_URI_PORT, 5683);
gcoap_serializer_add_opt(s, O_URI_PATH, "/temperature", sizeof("/temperature")-1);

// How to use gcoap_parser.
gcoap_parser* p = NULL;
gcoap_parser_init(&p, malloc(gcoap_parser_size(), gcoap_parser_size());
gcoap_parser_exec(p, buf, 2048);

printf("\n", );

free(s);
free(p);

```
