#include "ssl.h"
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

AsocSSLClientConfig *AsocSSLClientConfig_New() {
  AsocSSLClientConfig *conf = calloc(1, sizeof(AsocSSLClientConfig));
  check(conf != NULL, "Could not create conf");

  OpenSSL_add_all_algorithms();
  conf->ctx = SSL_CTX_new(TLS_client_method());
  conf->method = TLS_client_method();
  return conf;
error:
  return NULL;
}

void AsocSSLClientConfig_Destroy(AsocSSLClientConfig *client) { free(client); }

int AsocSSLConfig_Write(AsocSSLConfig *conf) {
  bstring data = bformat("\n\r"
                         "[pki]\n\r"
                         "pki=\"%s\""
                         "cert=\"%s\"",
                         bdata(conf->pki), bdata(conf->cert));
  ioStream *stream = NewIoStreamFile(conf->path, O_RDWR, 0);
  check(stream != NULL, "Could not open file %s", bdata(conf->path));
  check(IoStreamBuffWrite(stream, data) > 0, "Cold not write to %s",
        bdata(conf->path));
  check(IoStreamIoWrite(stream) > 0, "Could not write to file %s",
        bdata(conf->path));
  DestroyIoStream(stream);
  return 0;
error:
  return -1;
}

AsocSSLConfig *AsocSSLConfig_New(bstring folder) {
  AsocSSLConfig *conf = calloc(1, sizeof(AsocSSLConfig));
  check(conf != NULL, "Cold not allocate conf");

  conf->init = -1;
  conf->folder = folder;

  if (IoFileStream_DirectoryExists(folder) < 0) {
    check(IoFileStream_DirectoryCreate(folder, S_IRWXU | S_IRGRP | S_IROTH) >=
              0,
          "Config dir could not be created");
  }

  bstring path = bformat("%s%s", bdata(folder), CONFIG_FILE);
  conf->path = path;

  int rc = IoFileStream_FileExists(path);
  if (rc < 0) {
    ioStream *stream = NewIoStreamFile(conf->path, O_RDWR, 0);
    check(stream != NULL, "Could not open file %s", bdata(conf->path));
    check(IoStreamBuffWrite(stream, bfromcstr(EXAMPLE_CONTENT)) > 0,
          "Cold not write to %s", bdata(conf->path));
    check(IoStreamIoWrite(stream) > 0, "Could not write to file %s",
          bdata(conf->path));
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

  conf->method = TLS_server_method();
  check(conf->method != NULL, "SSL method is null");
  conf->ctx = SSL_CTX_new(conf->method);
  check(conf->ctx != NULL, "SSL CTX is null");

  conf->pki = bfromcstr(pki.u.s);
  check(bstrcmp(conf->pki, bfromcstr("")) != 0, "Could not get pki");
  conf->cert = bfromcstr(cert.u.s);
  check(bstrcmp(conf->pki, bfromcstr("")) != 0, "Could not get cert");

  log_info("Using cert %s", bdata(conf->cert));
  check(SSL_CTX_use_certificate_file(conf->ctx, bdata(conf->cert),
                                     SSL_FILETYPE_PEM) >= 0,
        "Could not load cert");

  log_info("Using key %s", bdata(conf->pki));
  check(SSL_CTX_use_PrivateKey_file(conf->ctx, bdata(conf->pki),
                                    SSL_FILETYPE_PEM) >= 0,
        "Could not use pki");

  return conf;
error:
  if (conf != NULL)
    free(conf);
  return NULL;
}

AsocSSLConfig *AsocSSLConfig_New_B(bstring folder) {
  AsocSSLConfig *conf = calloc(1, sizeof(AsocSSLConfig));
  check(conf != NULL, "Cold not allocate conf");

  conf->init = -1;
  conf->folder = folder;
  if (IoFileStream_DirectoryExists(folder) < 0) {
    check(IoFileStream_DirectoryCreate(folder, S_IRWXU | S_IRGRP | S_IROTH) >=
              0,
          "Config dir could not be created");
  }

  conf->path = bformat("%s%s", bdata(folder), CONFIG_FILE);
  conf->pki = bformat("%s%s", bdata(folder), "private.pem");

  return conf;
error:
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

AsocSSL *AsocSSL_New(int proto, int type, int port, bstring ip, int stype,
                     void *conf) {
  AsocSSL *asoc_ssl = calloc(1, sizeof(AsocSSL));
  check(asoc_ssl != NULL, "Could not Create socket");

  asoc_ssl->type = stype;
  AsocSSL_Init();

  asoc_ssl->as = Asoc_New(proto, type, port, ip, SSLFD);
  asoc_ssl->as->ssl = asoc_ssl->ssl;

  check(asoc_ssl->as != NULL, "Could not create socket");
  check(asoc_ssl->as->host != NULL, "Error Creating socket, host is NULL");
  check(asoc_ssl->as->port != NULL, "Error Creating socket, port is NULL");

  if (asoc_ssl->type == SERVER) {
    asoc_ssl->config = conf == NULL
                           ? AsocSSLConfig_New(bfromcstr(CONFIG_FOLDER))
                           : (AsocSSLConfig *)conf;
    check(asoc_ssl->config->conf_data != NULL, "Could not Create config");
  }

  if (asoc_ssl->type == CLIENT) {
    asoc_ssl->client =
        conf == NULL ? AsocSSLClientConfig_New() : (AsocSSLClientConfig *)conf;
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

AsocSSL *AsocSSL_Accept(AsocSSL *srv) {
  AsocSSL *client = calloc(1, sizeof(AsocSSL));
  check(client != NULL, "could not client allocate");

  client->as = calloc(1, sizeof(Asoc));
  check(client->as != NULL, "could not allocate socket");

  socklen_t peer_len = sizeof(client->as->addr);
  int c_soc = accept(srv->as->io->fd, (struct sockaddr *)&client->as->addr,
                     (socklen_t *)&peer_len);

  check(c_soc != 0, "Could not accept connection");
  srv->ssl = SSL_new(srv->config->ctx);

  check(srv->ssl != NULL, "Could not create SSL: %d",
        SSL_get_error(srv->ssl, 0));

  SSL_set_fd(srv->ssl, c_soc);

  int accept = SSL_accept(srv->ssl);
  check(accept > 0, "Could not accept request: fatal error : %d",
        SSL_get_error(srv->ssl, 0));
  check(accept != 0, "Could not accept request: %d",
        SSL_get_error(srv->ssl, 0));

  client->as->host = bfromcstr(inet_ntoa(client->as->addr.sin_addr));
  client->as->port = bformat("%d", client->as->addr.sin_port);

  client->as->io = NewIoStream(c_soc, SSLFD, 1024 * 10);
  client->as->io->ssl = srv->ssl;
  client->as->ssl = srv->ssl;
  client->ssl = srv->ssl;
  return client;
error:
  return NULL;
}
