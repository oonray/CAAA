#ifndef GENSSL_H_
#define GENSSL_H_

#include "ssl.h"
#include <caaa/dbg.h>
#include <openssl/ossl_typ.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdio.h>

#define BITS 2048

typedef struct KeyPair {
  RSA *r;
  BIGNUM *n;
  BIO *pub;
  BIO *priv;
  AsocSSLConfig *conf;
} KeyPair;

KeyPair *KeyPair_New(AsocSSLConfig *conf);
void KeyPair_Destroy(KeyPair *pki);

int KeyPair_Write(KeyPair *pki);

#endif // GENSSL_H_
