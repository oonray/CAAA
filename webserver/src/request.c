#include "request.h"
#include "bstrlib.h"
#include "dbg.h"
#include "map.h"
#include "status_codes.c"

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

  int latest = 0;
  // Creates a map of headder key:valye pairs
  for (int i = 0; i < req->Data->qty; i++) {
    bstring value = req->Data->entry[i];
    if (blength(value) <= 0)
      continue;

    if (!strchr(bdata(value), ':'))
      continue;

    struct bstrList *head = bsplits(value, bfromcstr(":"));
    Map_Set(req->Headers, head->entry[0], head->entry[1]);
    latest = i;
  }

  req->body = bfromcstr("");
  for (int i = latest + 1; i < req->Data->qty; i++) {
    bstring value = req->Data->entry[i];
    if (blength(value) <= 0)
      continue;

    if (blength(req->body) > 0)
      bconcat(req->body, &NL);

    bconcat(req->body, value);
  }

  return req;
error:
  return NULL;
}

void Request_Destroy(Request *req) {
  bdestroy(req->version);
  bdestroy(req->uri);
  bdestroy(req->method);
  bdestroy(req->body);
  bstrListDestroy(req->Data);
  Map_Destroy(req->Headers);
  free(req);
}

Response *Response_New(Request *req, bstring content, int status,
                       struct tagbstring *redirect) {
  Response *resp = (Response *)calloc(1, sizeof(Response));
  check(resp != NULL, "Could not create response");

  resp->Headders = Map_New(NULL, NULL);
  check(resp->Headders != NULL, "Could not create headder map");

  resp->version = req->version;

  if (Response_Set_Status(resp, status) != 0)
    log_info("Error when setting status");

  if (Response_Set_Date(resp) != 0)
    log_info("Error when setting date");

  // https://datatracker.ietf.org/doc/html/rfc2616#section-14.5
  // Accept-Ranges
  Map_Set(resp->Headders, bfromcstr("Accept-Rages"), bfromcstr("none"));
  /*
  **  Servers that do not accept any kind of range request for a
        resource MAY send

            Accept-Ranges: none
   */

  if (redirect != NULL)
    Map_Set(resp->Headders, bfromcstr("Locaton"), redirect);

  resp->body = content;
  return resp;
error:
  return NULL;
}

void Response_Destroy(Response *req) {
  bdestroy(req->version);
  bdestroy(req->reason_phrase);
  bdestroy(req->status_line);
  Map_Destroy(req->Headders);
  free(req);
}

int Response_Set_Status(Response *req, int status) {
  req->status_code = status;
  req->status_code =
      req->status_code == 0 ? 200 : req->status_code; // if ok or not stated 200
  req->status_code = req->status_code < 0
                         ? 500
                         : req->status_code; // if failure(-1) somwhere 500

  req->reason_phrase = Response_Status_TXT(req->status_code);

  req->status_line = bformat("%s %d %s\n\r", bdata(req->version),
                             req->status_code, bdata(req->reason_phrase));

  // we succsessfully set the status but it was an invalif one
  // return value is thefore -1 even if the data is ok
  if (bstrcmp(req->reason_phrase, &def) == 0)
    goto error;

  return 0;
error:
  return -1;
}

static char wday_name[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static char mon_name[12][3] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int Response_Set_Date(Response *rsp) {
  time_t t = time(NULL);
  struct tm *lt = gmtime(&t);

  bstring date =
      bformat("%.3s, %d %.3s %.4d %.2d:%.2d:%.2d GMT", wday_name[lt->tm_wday],
              lt->tm_mday, mon_name[lt->tm_mon], 1900 + lt->tm_year,
              lt->tm_hour, lt->tm_min, lt->tm_sec);

  if (Map_Set(rsp->Headders, bfromcstr("Date"), date) != 0)
    return -1;

  return 0;
}

bstring Response_To_String(Response *rsp) {
  bstring out = bfromcstr("");
  bconcat(out, rsp->status_line);

  for (int i = 0; i < Vector_End(rsp->Headders->buckets); i++) {
    Vector *bucket = (Vector *)Vector_Get(rsp->Headders->buckets, i);
    if (bucket) {
      for (int j = 0; j < Vector_End(bucket); j++) {
        Map_Node *node = (Map_Node *)Vector_Get(bucket, j);
        bconcat(out, bformat("%s: %s\n", bdata((bstring)node->key),
                             bdata((bstring)node->data)));
      }
    }
  }

  bconcat(out, &CRLF);
  bconcat(out, rsp->body);
  return out;
}
