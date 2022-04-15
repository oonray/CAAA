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

  int web_port = (port == 0 ? (type == SSLFD ? 445 : 80) : port);
  bstring hst = (host == NULL ? bfromcstr("0.0.0.0") : host);

  srv->terminate = false;
  srv->type = type;

  if (urls != NULL)
    srv->urls = urls;

  if (srv->type == SSLFD) {
    srv->sock.https =
        AsocSSL_New(AF_INET, SOCK_STREAM, web_port, hst, SERVER, conf);
    check(srv->sock.https != NULL, "Socke error");
  }

  if (srv->type == SOCKFD) {
    srv->sock.http = Asoc_New(AF_INET, SOCK_STREAM, web_port, hst, 0);
    check(srv->sock.http != NULL, "Socket error");
  }

  return srv;
error:
  return NULL;
}

void Webserver_Destroy(Webserver *srv) {
  if (srv->type == SSLFD) {
    shutdown(srv->sock.https->as->io->fd, SHUT_RDWR);
    AsocSSL_Destroy(srv->sock.https);
  }
  if (srv->type == SOCKFD) {
    shutdown(srv->sock.http->io->fd, SHUT_RDWR);
    Asoc_Destroy(srv->sock.http);
  }
  TriTree_Destroy(srv->urls);
  free(srv);
}

int Webserver_Run(Webserver *srv) {
  Asoc *target = NULL;

  if (srv->type == HTTPS)
    target = srv->sock.https->as;

  if (srv->type == HTTP)
    target = srv->sock.http;

  check(target != NULL, "socket error");

  check(AsocBind(target) == 0, "Could not bind");
  check(AsocListen(target, 0) == 0, "Could not listen");

  while (!srv->terminate) {
    void *client;
    if (srv->type == HTTPS)
      client = AsocSSL_Accept(srv->sock.https);
    if (srv->type == HTTP)
      client = AsocAccept(target, srv->type);

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
