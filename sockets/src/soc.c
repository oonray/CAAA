#include "soc.h"

AsocServer *AsocServer_New(int proto, int type,int port,bstring ip){
    AsocServer *srv = calloc(1,sizeof(AsocServer));
    check(srv != NULL,"Could not create Server");
    srv->io = NewIoStreamSocket(proto,type,1024*10);

    srv->addr.sin_family = proto;
    srv->addr.sin_port = htons(port);
    srv->addr.sin_addr.s_addr = inet_addr(bdata(ip));

    return srv;
error:
    if(srv!=NULL){free(srv);}
    return NULL;
}

void AsocServer_Destroy(AsocServer *srv){
    DestroyIoStream(srv->io);
    free(srv);
}

int AsocBind(AsocServer *srv){
    return bind(srv->io->fd,(struct sockaddr *)&srv->addr,sizeof(srv->addr));
}

int AsocListen(AsocServer *srv,int backlog){
    return listen(srv->io->fd,backlog<=0?50:backlog);
}

struct sockaddr_in *AsocAccept(AsocServer *srv){
    struct sockaddr_in *peer = calloc(1,sizeof(struct sockaddr_in));
    socklen_t peer_size = sizeof(struct sockaddr_in);
    check(accept(srv->io->fd,(struct sockaddr*)peer,&peer_size)>=0,"Could not accept connection");
    return peer;
error:
    return NULL;
}
