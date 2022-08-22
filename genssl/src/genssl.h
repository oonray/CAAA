#ifndef GENSSL_H_
#define GENSSL_H_

#include "dbg.h"
#include "ssl.h"
#include <openssl/ossl_typ.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <stdio.h>

#define BITS 4096

typedef struct KeyPair {
  RSA *r;
  BIGNUM *n;
  BIO *pub;
  BIO *priv;
  AsocSSLConfig *conf;
} KeyPair;

typedef struct X509_Self_Signed {
  KeyPair *pair;
  X509 *cert;
  bstring ca;
  bstring company;
  bstring host;
  bstring issuer;
  BIO *out;
} X509_Self_Signed;

KeyPair *KeyPair_New(AsocSSLConfig *conf);
void KeyPair_Destroy(KeyPair *pki);
int KeyPair_Write(KeyPair *pki);
int KeyPair_Read(KeyPair *pki);

X509_Self_Signed *SelfSigned_New(bstring loc, bstring company, bstring host,
                                 int not_after, KeyPair *pair);
#define GET_CERT(X) (x->cert)

void SelfSigned_Destroy(X509_Self_Signed *cert);
int SelfSigned_Write(X509_Self_Signed *cert);

#endif // GENSSL_H_
