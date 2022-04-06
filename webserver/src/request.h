#ifndef REQUEST_H_
#define REQUEST_H_

#include "bstrlib.h"
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
void Request_Destroy(Request *req);
bstring Request_To_String(Request *req);

//---------------------------
// Response
// --------------------------

//
//        Response      = Status-Line               ; Section 6.1
//                     *(( general-header        ; Section 4.5
//                      | response-header        ; Section 6.2
//                      | entity-header ) CRLF)  ; Section 7.1
//                     CRLF
//                     [ message-body ]          ; Section 7.2

typedef struct Response {
  // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
  bstring version;
  int status_code;
  bstring reason_phrase;
  bstring status_line;

} Response;

// Fill in the fields that can be fetched from the request and return.
Response *Response_New(Request *req, int status);
void Response_Destroy(Request *req);
bstring Response_To_String(Response *rsp);
int Response_Set_Status(Response *req, int status);
bstring Response_Status_TXT(int code);

#endif // REQUEST_H_
