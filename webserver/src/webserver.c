#include "webserver.h"
#include "dbg_a.h"
#include "request.h"
#include "ringbuffer.h"
#include "soc.h"
#include "ssl.h"
#include "tritree.h"
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <sys/socket.h>

Webserver *Webserver_New(int type, TriTree *urls, int port, bstring host,
                         AsocSSLConfig *conf) {
  Webserver *srv = calloc(1, sizeof(Webserver));
  check(srv != NULL, "could not allocate server");

  srv->port = (port == 0 ? (type == SSLFD ? 445 : 80) : port);
  srv->host = (host == NULL ? bfromcstr("0.0.0.0") : host);
  srv->current_threads = 0;

  srv->terminate = false;
  srv->type = type;

  if (urls != NULL)
    srv->urls = urls;

  if (srv->type == SSLFD) {
    srv->sock =
        AsocSSL_New(AF_INET, SOCK_STREAM, srv->port, srv->host, SERVER, conf);
  }

  if (srv->type == SOCKFD) {
    srv->sock = Asoc_New(AF_INET, SOCK_STREAM, srv->port, srv->host, 0);
  }

  check(srv->sock != NULL, "Socket error");

  return srv;
error:
  return NULL;
}

void Webserver_Destroy(Webserver *srv) {
  if (srv->type == SSLFD) {
    shutdown(((AsocSSL *)srv->sock)->as->io->fd, SHUT_RDWR);
    AsocSSL_Destroy((AsocSSL *)srv->sock);
  }
  if (srv->type == SOCKFD) {
    shutdown(((Asoc *)srv->sock)->io->fd, SHUT_RDWR);
    Asoc_Destroy((Asoc *)srv->sock);
  }
  TriTree_Destroy(srv->urls);
  free(srv);
}

void *Webserver_Run_T(void *inp) {
  Webserver_T_Data *in = (Webserver_T_Data *)inp;
  Webserver *srv = in->srv;

  if (srv->type == HTTPS) {
    AsocSSL *client = (AsocSSL *)in->client;
    check(Webserver_Route(srv->urls, client->as) == 0,
          "Error Happened"); // peer);

    SSL_shutdown(((AsocSSL *)srv->sock)->ssl);
    AsocSSL_Destroy((AsocSSL *)client);
  } else {
    Asoc *client = ((Asoc *)in->client);
    check(Webserver_Route(srv->urls, (Asoc *)client) == 0,
          "Error Happened"); // peer);
    shutdown(client->io->fd, SHUT_RDWR);
    Asoc_Destroy((Asoc *)client);
  }

  srv->current_threads--;

  return bfromcstr("0");
error:

  return bfromcstr("1");
}

void *Webserver_Main_T(void *inp) {
  Webserver *srv = (Webserver *)inp;
  for (;;) {
    check_break(!srv->terminate, "Termianting");
    void *client = NULL;

    if (srv->type == HTTPS)
      client = AsocSSL_Accept((AsocSSL *)srv->sock);
    if (srv->type == HTTP)
      client = AsocAccept((Asoc *)srv->sock, srv->type);

    check_continue(client != NULL, "Could not accept connection from peer");

    if (srv->type == HTTPS)
      log_info("ssl connection from %s:%s",
               bdata(((AsocSSL *)client)->as->host),
               bdata(((AsocSSL *)client)->as->port));

    if (srv->type == HTTP)
      log_info("connection from %s:%s", bdata(((Asoc *)client)->host),
               bdata(((Asoc *)client)->port));

    if (srv->thread_pool == NULL) {
      srv->thread_pool = calloc(srv->current_threads + 1, sizeof(pthread_t));
      check_continue(srv->thread_pool != NULL,
                     "could not allocate thread pool");
    } else {
      srv->thread_pool = realloc(srv->thread_pool,
                                 sizeof(pthread_t) * srv->current_threads + 1);
      check_continue(srv->thread_pool != NULL,
                     "could not allocate thread pool");
    }

    check_continue(sizeof(srv->thread_pool) >
                       sizeof(pthread_t) * srv->current_threads,
                   "Could not create thread");

    pthread_create(
        &srv->thread_pool[srv->current_threads + 1], NULL, Webserver_Run_T,
        (void *)(&((Webserver_T_Data){.client = client, .srv = srv})));

    srv->current_threads++;
  }
  return NULL;
}

int Webserver_Run(Webserver *srv) {
  check(srv != NULL, "no server given");
  log_info("Running on %s://%s:%d", srv->type == HTTPS ? "https" : "http",
           bdata(srv->host), srv->port);

  if (srv->type == HTTPS) {
    check(AsocBind(((AsocSSL *)srv->sock)->as) == 0, "Could not bind");
    check(AsocListen(((AsocSSL *)srv->sock)->as, 0) == 0, "Could not listen");
  } else {
    check(AsocBind((Asoc *)srv->sock) == 0, "Could not bind");
    check(AsocListen((Asoc *)srv->sock, 0) == 0, "Could not listen");
  }

  log_info("Listening..");

  pthread_create(&srv->main, NULL, Webserver_Main_T, (void *)srv);

  for (;;) {
    if (srv->terminate) {
      pthread_cancel(srv->main);
      break;
    }
  }

  return 0;
error:
  return -1;
}

int Webserver_Route(TriTree *urls, Asoc *peer) {
  // Read from socket
  IoStreamIoRead(peer->io);

  bstring data = IoStreamBuffRead(peer->io);
  check(data != NULL, "could not read form peer");

  Request *req = Request_New(data);

  url_handler handler =
      TriTree_Search(urls, bdata(req->uri), blength(req->uri));

  if (handler == NULL) {
    Response *four0four = Response_New(NULL, NULL, 404, NULL);
    bstring data = Response_To_String(four0four);

    IoStreamBuffWrite(peer->io, data);
    IoStreamIoWrite(peer->io);
    return 1;
  }

  Response *resp = handler(req);
  bstring resp_str = Response_To_String(resp);

  IoStreamBuffWrite(peer->io, resp_str);
  IoStreamIoWrite(peer->io);

  shutdown(peer->io->fd, SHUT_RDWR);
  return 0;
error:
  return -1;
}

int Webserver_AddRoute(Webserver *srv, bstring path, url_handler handler) {
  srv->urls = TriTree_Insert(srv->urls, bdata(path), blength(path), handler);
  if (srv->urls == NULL)
    return -1;
  return 0;
}
