#ifndef XOR_H_
#define XOR_H_

#include "bstrlib.h"
#include "dbg.h"
#include "fileio.h"

typedef struct XOR {
  ioStream *key;
  ioStream *in;
  ioStream *out;
} XOR;

ioStream *xor_default_key(int size);
XOR *XORNew();
#endif // XOR_H_
