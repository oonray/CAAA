#ifndef XOR_H_
#define XOR_H_

#include "bstrlib.h"
#include "dbg.h"
#include "fileio.h"

typedef ioStream *(*xor_key)(int size);

typedef struct XOR {
  xor_key key;
} XOR;

ioStream *xor_default_key(int size);
XOR *XORNew(xor_key key);

ioStream *XOR_encrypt(XOR *x, ioStream *data);
#define XOR_decrypt(X, D) XOR_encrypt(X, D)

#endif // XOR_H_
