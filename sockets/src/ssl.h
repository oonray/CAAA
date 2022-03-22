#ifndef ASOC_SSL_H_
#define ASOC_SSL_H_

#include <toml.h>
#include <sys/stat.h>

//OPENSSL
#include <openssl/bio.h>
#include <openssl/ssl3.h>
#include <openssl/err.h>

#include "soc.h"
#include "fileio.h"

#ifndef ASOC_SSL_H_CONST_
#define ASOC_SSL_H_CONST_
#define CONFIG_FOLDER "/etc/asoc/"
#define CONFIG_FILE "ssl.conf"
#define EXAMPLE_CONTENT "\n[ssl]\npki = \"<path>.pem\"\ncert = \"<path>.pem\"\n"
#define RIGHTS S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH
#endif

typedef struct AsocSSLConfig {
    int init;
    toml_table_t *conf_data;
    bstring pki;
    bstring cert;
} AsocSSLConfig;

AsocSSLConfig *AsocSSLConfig_New(bstring folder);
void AsocSSLConfig_Destroy(AsocSSLConfig *conf);

typedef struct AsocSSL {
    Asoc *as;
    AsocSSLConfig *config;
}AsocSSL;

AsocSSL *AsocSSL_New();
void AsocSSL_Destroy(AsocSSL *ssl);

#endif // SSL_H_
