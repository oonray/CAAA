#include "request.h"
#include "bstrlib.h"
#include "dbg.h"
#include "map.h"

static struct tagbstring def = bsStatic("Non supported code");

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
  for (int i = latest+1; i < req->Data->qty; i++) {
    bstring value = req->Data->entry[i];
    if (blength(value) <= 0)
      continue;

    if(blength(req->body)>0)
      bconcat(req->body,&NL);

    bconcat(req->body,value);
  }

  return req;
error:
  return NULL;
}

Response *Response_New(Request *req, int status) {
  Response *resp = (Response *)calloc(1, sizeof(Response));
  check(resp != NULL, "Could not create response");
  resp->version = req->version;

  Response_Set_Status(resp,status);

  return resp;
error:
  return NULL;
}

int Response_Set_Status(Response *req, int status){
  req->status_code = status;
  req->status_code = req->status_code==0?200:req->status_code; //if ok or not stated 200
  req->status_code = req->status_code<0 ?500:req->status_code; //if failure(-1) somwhere 500

  req->reason_phrase = Response_Status_TXT(req->status_code);

  req->status_line = bformat("%s %d %s\n\r",bdata(req->version),
                req->status_code,
                bdata(req->reason_phrase));

  //we succsessfully set the status but it was an invalif one
  //return value is thefore -1 even if the data is ok
  if(bstrcmp(req->reason_phrase,&def)==0)
    goto error;
    
  return 0;
error:
  return -1;
}

static struct tagbstring r418 = bsStatic("I'm a teapot");
static struct tagbstring r505 = bsStatic("HTTP Version not supported");
static struct tagbstring r504 = bsStatic("Gateway Time-out");
static struct tagbstring r503 = bsStatic("Service Unavailable");
static struct tagbstring r502 = bsStatic("Bad Gateway");
static struct tagbstring r501 = bsStatic("Not Implemented");
static struct tagbstring r500 = bsStatic("Internal Server Error");
static struct tagbstring r417 = bsStatic("Expectation Failed");
static struct tagbstring r416 = bsStatic("Requested range not satisfiable");
static struct tagbstring r415 = bsStatic("Unsupported Media Type");
static struct tagbstring r414 = bsStatic("Request-URI Too Large");
static struct tagbstring r413 = bsStatic("Request Entity Too Large");
static struct tagbstring r412 = bsStatic("Precondition Failed");
static struct tagbstring r411 = bsStatic("Length Required");
static struct tagbstring r410 = bsStatic("Gone");
static struct tagbstring r409 = bsStatic("Conflict");
static struct tagbstring r408 = bsStatic("Request Time-out");
static struct tagbstring r407 = bsStatic("Proxy Authentication Required");
static struct tagbstring r406 = bsStatic("Not Acceptable");
static struct tagbstring r405 = bsStatic("Method Not Allowed");
static struct tagbstring r404 = bsStatic("Not Found");
static struct tagbstring r403 = bsStatic("Forbidden");
static struct tagbstring r402 = bsStatic("Payment Required");
static struct tagbstring r401 = bsStatic("Unauthorized");
static struct tagbstring r400 = bsStatic("Bad Request");
static struct tagbstring r307 = bsStatic("Temporary Redirect");
static struct tagbstring r305 = bsStatic("Use Proxy");
static struct tagbstring r304 = bsStatic("Not Modified");
static struct tagbstring r303 = bsStatic("See Other");
static struct tagbstring r302 = bsStatic("Found");
static struct tagbstring r301 = bsStatic("Moved Permanently");
static struct tagbstring r300 = bsStatic("Multiple Choices");
static struct tagbstring r206 = bsStatic("Partial Content");
static struct tagbstring r205 = bsStatic("Reset Content");
static struct tagbstring r204 = bsStatic("No Content");
static struct tagbstring r203 = bsStatic("Non-Authoritative Information");
static struct tagbstring r202 = bsStatic("Accepted");
static struct tagbstring r201 = bsStatic("Created");
static struct tagbstring r200 = bsStatic("OK");
static struct tagbstring r101 = bsStatic("Switching Protocols");
static struct tagbstring r100 = bsStatic("Continue");

bstring Response_Status_TXT(int code) {
  switch (code) {
    // Status-Code    =
    //         | "100"  ; Section 10.1.1: Continue
    case 100:
      return &r100;
      break;
    //         | "101"  ; Section 10.1.2: Switching Protocols
    case 101:
      return &r101;
      break;
    //         | "200"  ; Section 10.2.1: OK
    case 200:
      return &r200;
      break;
    //         | "201"  ; Section 10.2.2: Created
    case 201:
      return &r201;
      break;
    //         | "202"  ; Section 10.2.3: Accepted
    case 202:
      return &r202;
      break;
    //         | "203"  ; Section 10.2.4: Non-Authoritative Information
    case 203:
      return &r203;
      break;
    //         | "204"  ; Section 10.2.5: No Content
    case 204:
      return &r204;
      break;
    //         | "205"  ; Section 10.2.6: Reset Content
    case 205:
      return &r205;
      break;
    //         | "206"  ; Section 10.2.7: Partial Content
    case 206:
      return &r206;
      break;
    //         | "300"  ; Section 10.3.1: Multiple Choices
    case 300:
      return &r300;
      break;
    //         | "301"  ; Section 10.3.2: Moved Permanently
    case 301:
      return &r301;
      break;
    //         | "302"  ; Section 10.3.3: Found
    case 302:
      return &r302;
      break;
    //         | "303"  ; Section 10.3.4: See Other
    case 303:
      return &r303;
      break;
    //         | "304"  ; Section 10.3.5: Not Modified
    case 304:
      return &r304;
      break;
    //         | "305"  ; Section 10.3.6: Use Proxy
    case 305:
      return &r305;
      break;
    //         | "307"  ; Section 10.3.8: Temporary Redirect
    case 307:
      return &r307;
      break;
    //         | "400"  ; Section 10.4.1: Bad Request
    case 400:
      return &r400;
      break;
    //         | "401"  ; Section 10.4.2: Unauthorized
    case 401:
      return &r401;
      break;
    //         | "402"  ; Section 10.4.3: Payment Required
    case 402:
      return &r402;
      break;
    //         | "403"  ; Section 10.4.4: Forbidden
    case 403:
      return &r403;
      break;
    //         | "404"  ; Section 10.4.5: Not Found
    case 404:
      return &r404;
      break;
    //         | "405"  ; Section 10.4.6: Method Not Allowed
    case 405:
      return &r405;
      break;
    //         | "406"  ; Section 10.4.7: Not Acceptable
    case 406:
      return &r406;
      break;
    //         | "407"  ; Section 10.4.8: Proxy Authentication Required
    case 407:
      return &r407;
      break;
    //         | "408"  ; Section 10.4.9: Request Time-out
    case 408:
      return &r408;
      break;
    //         | "409"  ; Section 10.4.10: Conflict
    case 409:
      return &r409;
      break;
    //         | "410"  ; Section 10.4.11: Gone
    case 410:
      return &r410;
      break;
    //         | "411"  ; Section 10.4.12: Length Required
    case 411:
      return &r411;
      break;
    //         | "412"  ; Section 10.4.13: Precondition Failed
    case 412:
      return &r412;
      break;
    //         | "413"  ; Section 10.4.14: Request Entity Too Large
    case 413:
      return &r413;
      break;
    //         | "414"  ; Section 10.4.15: Request-URI Too Large
    case 414:
      return &r414;
      break;
    //         | "415"  ; Section 10.4.16: Unsupported Media Type
    case 415:
      return &r415;
      break;
    //         | "416"  ; Section 10.4.17: Requested range not satisfiable
    case 416:
      return &r416;
      break;
    //         | "417"  ; Section 10.4.18: Expectation Failed
    case 417:
      return &r417;
      break;
    //         | "500"  ; Section 10.5.1: Internal Server Error
    case 500:
      return &r500;
      break;
    //         | "501"  ; Section 10.5.2: Not Implemented
    case 501:
      return &r501;
      break;
    //         | "502"  ; Section 10.5.3: Bad Gateway
    case 502:
      return &r502;
      break;
    //         | "503"  ; Section 10.5.4: Service Unavailable
    case 503:
      return &r503;
      break;
    //         | "504"  ; Section 10.5.5: Gateway Time-out
    case 504:
      return &r504;
      break;
    //         | "505"  ; Section 10.5.6: HTTP Version not supported
    case 505:
      return &r505;
      break;
    //         | "418"  ; I'm a teapot
    case 418:
      return &r418;
      break;
    default:
      return &def;
      break;
  }
}
