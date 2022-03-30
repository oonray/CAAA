#ifndef REQUEST_H_
#define REQUEST_H_

#include "fileio.h"
#include "map.h"
#include "bstrlib.h"
#include "dbg.h"

typedef struct Request {
    bstring Type; //GET
    bstring Path; // /
    bstring Version; //HTTP/1.1
    Map *Headders; //Host : www.google.com
    bstring Data;
} Request;

Request *Request_New(bstring data);
void Request_Destroy(Request *req);

typedef struct Response {

} Response;

int (*handler)(Request *req, Response *res);

#endif // REQUEST_H_
