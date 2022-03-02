#ifndef __Asock
#define __Asock

#include <c/fileio.h>
#include <sys/socket.h>

typedef struct AsocServer {
	bstring host;
	bstring port;
	ioStream *io;
} AsocServer;

AsocServer *AsocServer_New();
void AsocServer_Destroy(AsocServer *srv);
#endif
