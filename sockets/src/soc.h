#ifndef _Asock
#define _Asock

#include <c/fileio.h>
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct AsocServer {
	bstring host;
	bstring port;
	ioStream *io;
	struct sockaddr_in addr;
} AsocServer;

AsocServer *AsocServer_New(int proto, int type,int port,bstring ip);
void AsocServer_Destroy(AsocServer *srv);
int AsocBind(AsocServer *srv);
int AsocListen(AsocServer *srv,int backlog);
struct sockaddr_in *AsocAccept(AsocServer *srv);

#endif
