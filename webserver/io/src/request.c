#include "request.h"

Request *Request_New(bstring data){
    Request *req = calloc(1, sizeof(data));
    check(req!=NULL,"Could not allocate Request Space");

    struct bstrList *dt = bsplits(data,&CRLF);

    log_info("Items %d",dt->qty);
    bstring line = dt->entry[0];
    bstring agent = dt->entry[1];

    log_info("Line %s",bdata(line));
    log_info("Agent %s",bdata(agent));
    return req;
error:
    return NULL;
}
