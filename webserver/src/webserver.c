#include "webserver.h"
#include "request.h"
#include "soc.h"
#include "ssl.h"
#include "tritree.h"
#include <netinet/in.h>
#include <sys/socket.h>

Webserver *Webserver_New(int type, TriTree *urls, int port, bstring host,
                         AsocSSLConfig *conf) {
  Webserver *srv = calloc(1, sizeof(Webserver));
  check(srv != NULL, "could not allocate server");

  srv->port = (port == 0 ? (type == SSLFD ? 445 : 80) : port);
  srv->host = (host == NULL ? bfromcstr("0.0.0.0") : host);

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

int Webserver_Run(Webserver *srv) {
  log_info("Running on %s://%s:%d", srv->type == HTTPS ? "https" : "http",
           bdata(srv->host), srv->port);
  if (srv->type == HTTPS) {
    check(AsocBind(((AsocSSL *)srv->sock)->as) == 0, "Could not bind");
    check(AsocListen(((AsocSSL *)srv->sock)->as, 0) == 0, "Could not listen");
  } else {
    check(AsocBind((Asoc *)srv->sock) == 0, "Could not bind");
    check(AsocListen((Asoc *)srv->sock, 0) == 0, "Could not listen");
  }

  while (!srv->terminate) {
    void *client = NULL;

    if (srv->type == HTTPS)
      client = AsocSSL_Accept(((AsocSSL *)srv->sock)->ssl);
    if (srv->type == HTTP)
      client = AsocAccept((Asoc *)srv->sock, srv->type);

    check_continue(client != NULL, "Could not accept connection from peer");

    int i = fork();
    if (i == 0) {
      if (srv->type == HTTPS) {
        Webserver_Route(srv->urls, ((AsocSSL *)client)->as); // peer);
        AsocSSL_Destroy((AsocSSL *)client);
      } else {
        Webserver_Route(srv->urls, ((Asoc *)client)); // peer);
        Asoc_Destroy((Asoc *)client);
      }
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
  log_info("Recieved: %s", bdata(data));

  Request *req = Request_New(data);

  url_handler handler =
      TriTree_Search(urls, bdata(req->uri), blength(req->uri));

  if (handler == NULL) {
    Response *four0four = Response_New(req, NULL, 404, NULL);
    bstring data = Response_To_String(four0four);
    log_info("%s", bdata(data));

    IoStreamBuffWrite(peer->io, data);
    IoStreamIoWrite(peer->io);
    shutdown(peer->io->fd, SHUT_RDWR);
    Asoc_Destroy(peer);
    return 1;
  }

  Response *resp = handler(req);
  bstring resp_str = Response_To_String(resp);
  log_info("%s", bdata(resp_str));

  IoStreamBuffWrite(peer->io, resp_str);
  IoStreamIoWrite(peer->io);
  shutdown(peer->io->fd, SHUT_RDWR);
  Asoc_Destroy(peer);
  return 0;
}

int Webserver_AddRoute(Webserver *srv, bstring path, url_handler handler) {
  srv->urls = TriTree_Insert(srv->urls, bdata(path), blength(path), handler);
  if (srv->urls == NULL)
    return -1;
  return 0;
}
