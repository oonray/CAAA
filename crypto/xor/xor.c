#include "xor.h"
#include "bstrlib.h"
#include "ringbuffer.h"

ioStream *xor_default_key(int size) {
  int sz = size + 1;
  ioStream *key = NewIoStream(0, STRINGFD, sz);
  bstring data = bfromcstr("f");
  bsetstr(data, sz, NULL, 'f');
  IoStreamBuffWrite(key, data);
  bdestroy(data);
  return key;
}

XOR *XORNew(xor_key key) {
  XOR * xor = calloc(1, sizeof(XOR));
  check_memory(xor);

  xor->key = (key == NULL ? xor_default_key : key);

  return xor;
error:
  return NULL;
}

ioStream *XOR_encrypt(XOR *x, ioStream *data) {
  if (data->fd_t != STRINGFD) {
    IoStreamIoRead(data);
  }

  check(!RingBuffer_Empty(data->in), "Needs input data");

  ioStream *out = NewIoStream(0, STRINGFD, 0);
  check(out != NULL, "Could not Create output stream");

  ioStream *key = x->key(data->in->length);
  check(!RingBuffer_Empty(key->in), "Needs a key to use");

  while (!RingBuffer_Empty(data->in)) {
    char h;
    char k_h;

    RingBuffer_Read(key->in, &k_h, 1);
    RingBuffer_Read(data->in, &h, 1);

    if (h == 0x0)
      break;

    char o = h ^ k_h;

    RingBuffer_Write(out->in, &o, 1);
  }

  return out;
error:
  return NULL;
}
