#include "request.h"
#include "bstrlib.h"
#include "dbg.h"
#include "map.h"

Request *Request_New(bstring data) {
  Request *req = calloc(1, sizeof(Request));
  check(req != NULL, "Could not create Request");

  req->Headers = Map_New(NULL, NULL);
  check(req->Headers != NULL, "Could not create Header map");

  req->Data = bsplits(data, &CRLF);
  check(req->Data->qty >= 2, "Malformed Request");

  // Parse Request Line
  // https://datatracker.ietf.org/doc/html/rfc2616#section-5.1
  // Method SP Request-URI SP HTTP-Version CRLF
  struct bstrList *line = bsplits(req->Data->entry[0], bfromcstr(" "));
  check(line->qty == 3, "Malformed Request");
  req->method = line->entry[0];
  req->uri = line->entry[1];
  req->version = line->entry[2];

  // Creates a map of headder key:valye pairs
  for (int i = 0; i < req->Data->qty; i++) {
    bstring value = req->Data->entry[i];
    if (blength(value) <= 0)
      continue;

    if (!strchr(bdata(value), ':'))
      continue;

    struct bstrList *head = bsplits(value, bfromcstr(":"));
    Map_Set(req->Headers, head->entry[0], head->entry[1]);
  }

  req->body = req->Data->entry[req->Data->qty];

  return req;
error:
  return NULL;
}
