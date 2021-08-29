#include "soc.h"

Asoc *Asoc_create(int Type,int Proto,int port){
#ifdef _WIN32 // Windows
	if(WSA_INIT != 0)
		WSA_INIT=AInit();
#else
#endif

Asoc *out = (Asoc *)calloc(1,sizeof(Asoc));

#ifdef _WIN32
out->socket = INVALID_SOCKET;
memset(&out->hints,0,sizeof(out->hints));

out->hints.ai_family = AF_UNSPEC;
out->hints.ai_socktype = Type;
out->hints.ai_protocol = Proto;

out->socket = socket(out->hints.ai_family,out->hints.ai_socktype,out->hints.ai_protocol);
check(socket!=INVALID_SOCKET,"Socket create error: %ld",WSAGetLastError());
#else
out->addr.sin_family 	= Type;
out->addr.sin_port 		= port;
out->socket = socket(out->addr.sin_family,Proto,0);
check(socket!=0,"Socket Creation error");
#endif
return out;
error:
	return NULL;
}
