# GreenCoAP

GreenCoAP is a CoAP (RFC7252: The Constrained Application Protocol) serializer/parser implementation written in C.

Features:

  * Support RFC7252. <http://tools.ietf.org/html/rfc7252>
  * No allocations
  * No dependencies.

## Usage

```c
char buf[512] = {};
char send_buf[2048] = {};
gcoap_serializer_t* s = NULL;

gcoap_serializer_create(&s, buf, 2048);
gcoap_serializer_init(s, T_CON | C_GET, "token", sizeof("token")-1, NULL, 0);
gcoap_serializer_add_opt(s, O_URI_HOST, "example.com");
gcoap_serializer_add_opt(s, O_URI_PORT, 5683);
gcoap_serializer_add_opt(s, O_URI_PATH, "/temperature");
gcoap_serializer_exec(s, send_buf, 2048);

gcoap_serializer_destroy(s);

```
