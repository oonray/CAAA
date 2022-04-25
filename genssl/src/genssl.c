#include "genssl.h"
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

KeyPair *KeyPair_New(AsocSSLConfig *conf) {
  KeyPair *pair = calloc(1, sizeof(KeyPair));
  check(pair != NULL, "Could not allocate KeyPair");

  int rc = 0;

  pair->n = BN_New();
  rc = BN_set_word(pair->n, RSA_F4);
  check(rc == 0, "Could not set BNE");

  pair->r = RSA_new();
  rc = RSA_generate_key_ex(pair->r, BITS, pair->n, NULL);
  check(rc == 0, "Could not generate key");

  return pair;
error:
  KeyPair_Destroy(pair);
  return NULL;
}

void KeyPair_Destroy(KeyPair *pair) {
  if (pair->r != NULL)
    RSA_free(pair->r);
  if (pair->n != NULL)
    BN_free(pair->n);
  free(pair);
}

int KeyPair_Write(KeyPair *pki) {
  pki->pub = BIO_new_file(
      bdata(bformat("%s/%s", bdata(pki->conf->path), "public.pem")), "w+");
  pki->priv = BIO_new_file(bdata(pki->conf->pki), "w+");

  int rc = 0;
  rc =
      PEM_write_bio_RSAPrivateKey(pki->priv, pki->r, NULL, NULL, 0, NULL, NULL);
  check(rc == 0, "Could not write private key");

  rc = PEM_write_bio_RSAPublicKey(pki->priv, pki->r);
  check(rc == 0, "Could not write public key");

  return rc;
error:
  return -1;
}
