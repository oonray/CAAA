#ifndef REQUEST_H_
#define REQUEST_H_

#include "fileio.h"
#include "tritree.h"

//---------------------------
// Request
// --------------------------

typedef struct Request {
  // Use bsplitstr (bstring.h 122)
  // Use CRLF [\n\r] from fileio.h
  struct bstrList *Data;
  TriTree *Headers;
  bstring uri;
  bstring method;
  bstring version;
  bstring body;
} Request;

#define Request_Line(A) (A)->Data->entry[0]
#define Request_Header(A, I) (A)->Data->entry + (I)
#define Request_Data(A) (A)->Data->entry[(A)->Data->qty - 1]

Request *Request_New(bstring data);

//---------------------------
// Response
// --------------------------

typedef struct Response {

} Response;

#endif // REQUEST_H_
