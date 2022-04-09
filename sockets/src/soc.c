#include "soc.h"

Asoc *Asoc_New(int proto, int type, int port, bstring ip) {
  Asoc *srv = calloc(1, sizeof(Asoc));
  check(srv != NULL, "Could not create Server");

#ifdef HEADER_SSL_H
  if (type == SSLFD) {
    srv->io = NewIoStreamSocketSSL(proto, type, 1024 * 10);
  } else {
    srv->io = NewIoStreamSocketSOC(proto, type, 1024 * 10);
  }
#else
  srv->io = NewIoStreamSocketSOC(proto, type, 1024 * 10);
#endif
  check(srv->io != NULL, "Could not create io");

  srv->addr.sin_family = proto;
  srv->addr.sin_port = htons(port);
  srv->addr.sin_addr.s_addr = inet_addr(bdata(ip));

  return srv;
error:
  if (srv != NULL) {
    free(srv);
  }
  return NULL;
}

void Asoc_Destroy(Asoc *srv) {
  DestroyIoStream(srv->io);
  free(srv);
}

int AsocBind(Asoc *srv) {
  return bind(srv->io->fd, (struct sockaddr *)&srv->addr, sizeof(srv->addr));
}

int AsocListen(Asoc *srv, int backlog) {
  return listen(srv->io->fd, (backlog == 0 ? 10 : backlog));
}

int AsocConnect(Asoc *srv) {
  int c_soc =
      connect(srv->io->fd, (struct sockaddr *)&srv->addr, sizeof(srv->addr));
  return c_soc;
}

Asoc *AsocAccept(Asoc *srv) {
  Asoc *client = calloc(1, sizeof(Asoc));
  check(client != NULL, "Could not create client soc");

  socklen_t peer_len = sizeof(client->addr);
  int c_soc = accept(srv->io->fd, (struct sockaddr *)&client->addr,
                     (socklen_t *)&peer_len);

  check(c_soc != 0, "Could not accept connection");
  // Creates errno 10 no child processes

  client->io = NewIoStream(c_soc, SOCKFD, 1024 * 10);
  return client;
error:
  return NULL;
}
