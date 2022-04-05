#ifndef REQUEST_H_
#define REQUEST_H_

#include "fileio.h"
#include "map.h"

//---------------------------
// Request
// --------------------------

typedef struct Request {
  // Use bsplitstr (bstring.h 122)
  // Use CRLF [\n\r] from fileio.h
  struct bstrList *Data;
  Map *Headers;
  bstring uri;
  bstring method;
  bstring version;
  bstring body;
} Request;

Request *Request_New(bstring data);

//---------------------------
// Response
// --------------------------

typedef struct Response {

} Response;

#endif // REQUEST_H_
