#include "genssl.h"
#include "ssl.h"
#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

KeyPair *KeyPair_New(AsocSSLConfig *conf) {
  KeyPair *pair = calloc(1, sizeof(KeyPair));
  check(pair != NULL, "Could not allocate KeyPair");

  int rc = 0;

  pair->conf = conf;

  pair->n = BN_new();
  rc = BN_set_word(pair->n, RSA_F4);
  check(rc >= 0, "Could not set BNE");

  pair->r = RSA_new();
  rc = RSA_generate_key_ex(pair->r, BITS, pair->n, NULL);
  check(rc >= 0, "Could not generate key");

  return pair;
error:
  KeyPair_Destroy(pair);
  return NULL;
}

void KeyPair_Destroy(KeyPair *pair) {
  if (pair->pub != NULL)
    BIO_free_all(pair->pub);
  if (pair->priv != NULL)
    BIO_free_all(pair->priv);
  if (pair->r != NULL)
    RSA_free(pair->r);
  if (pair->n != NULL)
    BN_free(pair->n);
  free(pair);
}

int KeyPair_Write(KeyPair *pki) {
  int rc = 0;
  bstring pub = bformat("%s%s", bdata(pki->conf->folder), "public.pem");

  pki->pub = BIO_new_file(bdata(pub), "w+");
  check(pki->pub != NULL, "Could not open file %s", bdata(pub));

  rc = PEM_write_bio_RSAPublicKey(pki->pub, pki->r);
  check(rc > 0, "Could not write public key");

  bstring priv = bformat("%s%s", bdata(pki->conf->folder), "private.pem");
  pki->priv = BIO_new_file(bdata(priv), "w+");
  check(pki->priv != NULL, "Could not open private key %s",
        bdata(pki->conf->pki));

  rc =
      PEM_write_bio_RSAPrivateKey(pki->priv, pki->r, NULL, NULL, 0, NULL, NULL);
  check(rc > 0, "Could not write private key");

  return rc;
error:
  return -1;
}

int KeyPair_Read(KeyPair *pki) {
  int rc = 0;

  bstring pub = bformat("%s%s", bdata(pki->conf->folder), "public.pem");
  pki->pub = BIO_new_file(bdata(pub), "r");

  pki->r = PEM_read_bio_RSAPublicKey(pki->pub, &pki->r, NULL, NULL);
  check(pki->r != NULL, "Could not read public key");

  bstring priv = bformat("%s%s", bdata(pki->conf->folder), "private.pem");
  pki->priv = BIO_new_file(bdata(priv), "r");

  pki->r = PEM_read_bio_RSAPrivateKey(pki->priv, &pki->r, NULL, NULL);
  check(pki->r != NULL, "Could not read private key");

  return rc;
error:
  return -1;
}

X509_Self_Signed *SelfSigned_New(bstring loc, bstring company, bstring host,
                                 int not_after, KeyPair *pair) {
  X509_Self_Signed *out = calloc(1, sizeof(X509_Self_Signed));
  check(out != NULL, "Could not allocate cert");

  out->pair = pair;

  if (out->pair == NULL) {
    AsocSSLConfig *conf = AsocSSLConfig_New(bfromcstr("./certs.out/"));
    check(conf != NULL, "conf is null");
    out->pair = KeyPair_New(conf);
    KeyPair_Write(out->pair);
  }
  KeyPair_Read(out->pair);

  // we have keys anyways
  out->cert = X509_new();
  check(out->cert != NULL, "509 is not allocated");
  ASN1_INTEGER_set_int64(X509_get_serialNumber(out->cert), 1);
  X509_gmtime_adj(X509_get_notBefore(out->cert), 0);

  // default is one year
  // i hope my math is right
  long end = not_after == 0 ? 31536000L : not_after;
  X509_gmtime_adj(X509_get_notAfter(out->cert), end);

  EVP_PKEY *key = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(key, out->pair->r);
  check(key != NULL, "Could not read key");
  // now we have a EVP_KEY from the RSA

  X509_set_pubkey(out->cert, key);

  X509_NAME *name = X509_get_subject_name(out->cert);

  bstring loc_local = loc == NULL ? bfromcstr("AQ") : loc;
  X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC,
                             (unsigned char *)bdata(loc_local), -1, -1, 0);

  bstring company_local = company == NULL ? bfromcstr("Anonymoyus") : company;
  X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC,
                             (unsigned char *)bdata(company_local), -1, -1, 0);

  bstring host_local = host == NULL ? bfromcstr("localhost") : host;
  X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                             (unsigned char *)bdata(host_local), -1, -1, 0);

  X509_set_issuer_name(out->cert, name);
  int rc = X509_sign(out->cert, key, EVP_sha512());
  check(rc > 0, "Could not sign");

  return out;
error:
  SelfSigned_Destroy(out);
  return NULL;
}

void SelfSigned_Destroy(X509_Self_Signed *cert) {
  if (cert->pair != NULL)
    KeyPair_Destroy(cert->pair);
  if (cert->cert != NULL)
    X509_free(cert->cert);
  if (cert != NULL)
    free(cert);
}

int SelfSigned_Write(X509_Self_Signed *cert) {
  bstring target = bformat("%s%s", bdata(cert->pair->conf->folder), "cert.pem");
  cert->out = BIO_new_file(bdata(target), "w+");
  int rc = PEM_write_bio_X509(cert->out, cert->cert);
  return rc;
}
