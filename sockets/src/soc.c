#include "soc.h"

AsocServer *AsocServer_New(int proto, int type){
    AsocServer *srv = calloc(1,sizeof(AsocServer));
    check(srv != NULL,"Could not create Server");
    srv->io = NewIoStreamSocket(proto,type,1024*10);
    return srv;
error:
    if(srv!=NULL){free(srv);}
    return NULL;
}

void AsocServer_Destroy(AsocServer *srv){
    DestroyIoStream(srv->io);
    free(srv);
}
