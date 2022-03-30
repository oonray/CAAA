#ifndef _ASOCK_H
#define _ASOCK_H

#include "fileio.h"
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct Asoc {
  bstring host;
  bstring port;
  ioStream *io;
  struct sockaddr_in addr;
#ifdef HEADER_SSL_H
  SSL *ssl;
#endif
} Asoc;

Asoc *Asoc_New(int proto, int type, int port, bstring ip);
void Asoc_Destroy(Asoc *srv);
int AsocBind(Asoc *srv);
int AsocListen(Asoc *srv, int backlog);
int AsocConnect(Asoc *srv);
Asoc *AsocAccept(Asoc *srv);

#endif
