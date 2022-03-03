#ifndef _Asock
#define _Asock

#include <c/fileio.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct Asoc {
	bstring host;
	bstring port;
	ioStream *io;
	struct sockaddr_in addr;
} Asoc;

Asoc *Asoc_New(int proto, int type,int port,bstring ip);
void Asoc_Destroy(Asoc *srv);
int AsocBind(Asoc *srv);
int AsocListen(Asoc *srv,int backlog);
int AsocConnect(Asoc *srv);
struct sockaddr_in *AsocAccept(Asoc *srv);

#endif
