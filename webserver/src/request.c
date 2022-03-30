#include "request.h"
#include "dbg.h"

Request *Request_New(bstring data) {
  Request *req = calloc(1, sizeof(Request));
  check(req != NULL, "Could not create Request");

  req->Data = bsplits(data, &CRLF);
  check(req->Data->qty >= 2, "Malformed Request");

  // Parse Request Line
  // https://datatracker.ietf.org/doc/html/rfc2616#section-5.1
  // Method SP Request-URI SP HTTP-Version CRLF
  struct bstrList *line = bsplits(Request_Line(req), bfromcstr(" "));
  check(line->qty == 3, "Malformed Request");
  req->method = line->entry[0];
  req->uri = line->entry[1];
  req->version = line->entry[2];

  // Creates a map of headder key:valye pairs
  for (int i = 1; i < req->Data->qty; i++) {
    if (req->Data->entry[i * 2] == NULL) {
      break;
    }
    if (bstrcmp(req->Data->entry[i * 2], bfromcstr("")) == 0) {
      break;
    }
    struct bstrList *keyval = bsplits(req->Data->entry[i], bfromcstr(":"));
    if (keyval->qty == 2) {
      TriTree_Insert(req->Headers, bdata(keyval->entry[0]),
                     blength(keyval->entry[0]), (void *)keyval->entry[1]);
    }
    bstrListDestroy(keyval);
  }

  if (req->Data->qty > 2)
    req->body = Request_Data(req);

  return req;
error:
  return NULL;
}
