#include "ssl.h"
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

AsocSSLClientConfig *AsocSSLClientConfig_New() {
  AsocSSLClientConfig *conf = calloc(1, sizeof(AsocSSLClientConfig));
  check(conf != NULL, "Could not create conf");

  // conf->method = SSLv23_client_method();
  //  conf->certbio = BIO_new(BIO_s_file());
  return conf;
error:
  return NULL;
}

void AsocSSLClientConfig_Destroy(AsocSSLClientConfig *client) {}

AsocSSLConfig *AsocSSLConfig_New(bstring folder) {
  ioStream *stream = NULL;

  AsocSSLConfig *conf = calloc(1, sizeof(AsocSSLConfig));
  check(conf != NULL, "Cold not allocate conf");

  conf->init = -1;

  if (IoFileStream_DirectoryExists(folder) < 0) {
    check(IoFileStream_DirectoryCreate(folder, RIGHTS) >= 0,
          "Config dir could not be created");
  }

  bstring path = bformat("%s%s", bdata(folder), CONFIG_FILE);

  int rc = IoFileStream_FileExists(path);
  if (rc < 0) {
    int fd = IoFileStream_FileCreate(path, RIGHTS);
    check(fd >= 0, "Config file could not be created");
    close(fd);
    log_info("File %s Created", bdata(path));
    stream = NewIoStreamFile(path, O_RDWR, RIGHTS, 10 * 1024);
    check(IoStreamBuffWrite(stream, bfromcstr(EXAMPLE_CONTENT)) >= 0,
          "Could not write example text to buffer");
    check(IoStreamIoWrite(stream) >= 0, "Could not write to file fd");
    DestroyIoStream(stream);
  }

  char errbuff[200];
  FILE *f = fopen(bdata(path), "r");
  check(f, "Could not open file %s", bdata(path));

  conf->conf_data = toml_parse_file(f, errbuff, sizeof(errbuff));
  toml_table_t *ssl = toml_table_in(conf->conf_data, "ssl");
  check(ssl != NULL, "Could not fetch ssl, check config");
  fclose(f);

  toml_datum_t pki = toml_string_in(ssl, "pki");
  toml_datum_t cert = toml_string_in(ssl, "cert");

  conf->pki = bfromcstr(pki.u.s);
  check(bstrcmp(conf->pki, bfromcstr("")) != 0, "Could not get pki");
  conf->cert = bfromcstr(cert.u.s);
  check(bstrcmp(conf->pki, bfromcstr("")) != 0, "Could not get cert");

  return conf;
error:
  if (stream != NULL) {
    DestroyIoStream(stream);
  }
  if (conf != NULL)
    free(conf);
  return NULL;
}

void AsocSSL_Destroy(AsocSSL *conf) { free(conf); }

void AsocSSL_Init() {
  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
}

void AsocSSL_deInit() {
  ERR_free_strings();
  EVP_cleanup();
}

AsocSSL *AsocSSL_New(int proto, int type, int port, bstring ip, int stype) {
  AsocSSL *asoc_ssl = calloc(1, sizeof(AsocSSL));
  check(asoc_ssl != NULL, "Could not Create socket");

  asoc_ssl->type = stype;

  AsocSSL_Init();

  asoc_ssl->as = Asoc_New(proto, type, port, ip);

  if (asoc_ssl->type == SERVER) {
    asoc_ssl->config = AsocSSLConfig_New(bfromcstr(CONFIG_FOLDER));
    check(asoc_ssl->config->conf_data != NULL, "Could not Create config");

    asoc_ssl->ctx = SSL_CTX_new(TLS_server_method());
    check(asoc_ssl->ctx != NULL, "SSL CTX is null");

    log_info("Using %s", bdata(asoc_ssl->config->cert));
    check(SSL_CTX_use_certificate_file(asoc_ssl->ctx,
                                       bdata(asoc_ssl->config->cert),
                                       SSL_FILETYPE_PEM) >= 0,
          "Could not load cert");

    log_info("Using %s", bdata(asoc_ssl->config->pki));
    check(SSL_CTX_use_PrivateKey_file(asoc_ssl->ctx,
                                      bdata(asoc_ssl->config->pki),
                                      SSL_FILETYPE_PEM) >= 0,
          "Could not use pki");
  }

  if (asoc_ssl->type == CLIENT) {
    asoc_ssl->ctx = SSL_CTX_new(TLS_client_method());
    asoc_ssl->client = AsocSSLClientConfig_New();
  }

  asoc_ssl->as->ssl = asoc_ssl->ssl;

  return asoc_ssl;
error:
  return NULL;
}

int AsocSSLConnect(AsocSSL *srv) {
  int c_soc = connect(srv->as->io->fd, (struct sockaddr *)&srv->as->addr,
                      sizeof(srv->as->addr));
  check(c_soc >= 0, "Could not connect to server");
  check(SSL_connect(srv->ssl) != 1, "Could not connect using SSL");

  srv->client->cert = SSL_get_peer_certificate(srv->ssl);
  check(srv->client->cert != NULL, "Could not get cert");

  srv->client->issuername = X509_get_issuer_name(srv->client->cert);
  srv->client->certname = X509_get_subject_name(srv->client->cert);
  return c_soc;
error:
  return -1;
}

int AsocSSL_doSSL(AsocSSL *srv) {
  srv->ssl = SSL_new(srv->ctx);
  SSL_set_fd(srv->ssl, srv->as->io->fd);
  check(SSL_accept(srv->ssl), "Could not accept request");
  return 0;
error:
  return -1;
}
