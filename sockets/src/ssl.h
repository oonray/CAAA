#ifndef ASOC_SSL_H_
#define ASOC_SSL_H_

#define _WITH_OPEN_SSL

#include <sys/stat.h>
#include <toml.h>

// OPENSSL
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl3.h>

#include "fileio.h"
#include "soc.h"

#ifndef ASOC_SSL_H_CONST_
#define ASOC_SSL_H_CONST_
#define CONFIG_FOLDER "/etc/asoc/"
#define CONFIG_FILE "ssl.conf"
#define EXAMPLE_CONTENT                                                        \
  "\nr[ssl]\nrpki = \"<path>.pem\"\nrcert = \"<path>.pem\"\nr"
#define RIGHTS S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
#endif

#define CLIENT 0x01
#define SERVER 0x02

typedef struct AsocSSLClientConfig {
  BIO *certbio;
  X509 *cert;
  X509_NAME *issuername;
  X509_NAME *certname;
  SSL_CTX *ctx;
  const SSL_METHOD *method;
} AsocSSLClientConfig;

AsocSSLClientConfig *AsocSSLClientConfig_New();
void AsocSSLClientConfig_Destroy(AsocSSLClientConfig *client);

typedef struct AsocSSLConfig {
  int init;
  toml_table_t *conf_data;
  bstring pki;
  bstring cert;
  bstring folder;
  bstring path;
  bstring AsocSSLConfig;
  const SSL_METHOD *method;
  SSL_CTX *ctx;

} AsocSSLConfig;

AsocSSLConfig *AsocSSLConfig_New(bstring folder);
AsocSSLConfig *AsocSSLConfig_New_B(bstring folder);
void AsocSSLConfig_Destroy(AsocSSLConfig *conf);
int AsocSSLConfig_Write(AsocSSLConfig *conf);

typedef struct AsocSSL {
  int type;
  Asoc *as;
  AsocSSLConfig *config;
  AsocSSLClientConfig *client;
  SSL *ssl;
} AsocSSL;

AsocSSL *AsocSSL_New(int proto, int type, int port, bstring ip, int stype,
                     void *conf);
void AsocSSL_Destroy(AsocSSL *ssl);

void AsocSSL_Init();
void AsocSSL_deInit();

AsocSSL *AsocSSL_Accept(AsocSSL *ssl);
int AsocSSLConnect(AsocSSL *srv);
#endif // SSL_H_
