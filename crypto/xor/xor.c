#include "xor.h"
#include "ringbuffer.h"

ioStream *xor_default_key(int size) {
  ioStream *key = NewIoStream(0, STRINGFD, size);
  bstring data = bfromcstr("f");
  bsetstr(data, size, NULL, 'f');
  IoStreamBuffWrite(key, data);
  bdestroy(data);
  return key;
}

XOR *XORNew() {
  XOR * xor = calloc(1, sizeof(XOR));
  check_memory(xor);
  return xor;
error:
  return NULL;
}
