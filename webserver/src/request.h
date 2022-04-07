#ifndef REQUEST_H_
#define REQUEST_H_

#include "bstrlib.h"
#include "fileio.h"
#include "map.h"
#include "time.h"

// https://datatracker.ietf.org/doc/html/rfc2616
//---------------------------
//  Request
//  --------------------------
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

  //                     *(( general-header        ; Section 4.5
  Map *Headders;
  /*      general-header = Cache-Control            ; Section 14.9
                       | Connection               ; Section 14.10
                       | Date                     ; Section 14.18
                       | Pragma                   ; Section 14.32
                       | Trailer                  ; Section 14.40
                       | Transfer-Encoding        ; Section 14.41
                       | Upgrade                  ; Section 14.42
                       | Via                      ; Section 14.45
                       | Warning                  ; Section 14.4

      response-header = Accept-Ranges           ; Section 14.5
                       | Age                     ; Section 14.6
                       | ETag                    ; Section 14.19
                       | Location                ; Section 14.30
                       | Proxy-Authenticate      ; Section 14.33
       entity-header  = Allow                    ; Section 14.7
                      | Content-Encoding         ; Section 14.11
                      | Content-Language         ; Section 14.12
                      | Content-Length           ; Section 14.13
                      | Content-Location         ; Section 14.14
                      | Content-MD5              ; Section 14.15
                      | Content-Range            ; Section 14.16
                      | Content-Type             ; Section 14.17
                      | Expires                  ; Section 14.21
                      | Last-Modified            ; Section 14.29
                      | extension-header
                       */
  bstring body;
} Response;

// Fill in the fields that can be fetched from the request and return.
Response *Response_New(Request *req, bstring content, int status,
                       bstring redirect);
void Response_Destroy(Response *req);
bstring Response_To_String(Response *rsp);

int Response_Set_Status(Response *req, int status);
bstring Response_Status_TXT(int code);

int Response_Set_Date(Response *rsp);

#endif // REQUEST_H_
