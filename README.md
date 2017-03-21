WSSS
===================

We've got websocket server in plain C. Server sending C struct:
```
struct {
    uint8_t  uint8;
    uint32_t uint32;
}
```
In case of receiveing "1" - server sending struct, aligned by default, in case of receiveing "2" - server sending struct, aligned by 1 byte.

Implemented on <a href="http://disq.us/p/12hmrh7">some kind of a request
