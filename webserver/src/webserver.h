#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include "dbg_a.h"
#include "request.h"
#include "soc.h"
#include "ssl.h"
#include "tritree.h"
#include <pthread.h>

#undef CONFIG_FOLDER
#define CONFIG_FOLDER "/etc/aweb/"

#define HTTP SOCKFD
#define HTTPS SSLFD

typedef Response *(*url_handler)(Request *req);

typedef struct Webserver {
  TriTree *urls;
  bool terminate;
  int type;
  void *sock;
  int port;
  bstring host;
  pthread_t main;
  pthread_t *thread_pool;
  int current_threads;
} Webserver;

typedef struct Webserver_T_Data {
  Webserver *srv;
  void *client;
} Webserver_T_Data;

Webserver *Webserver_New(int type, TriTree *urls, int port, bstring host,
                         AsocSSLConfig *conf);
void Webserver_Destroy(Webserver *srv);

int Webserver_Run(Webserver *srv);
int Webserver_Route(TriTree *urls, Asoc *peer);
int Webserver_AddRoute(Webserver *srv, bstring path, url_handler handler);

#endif // WEBSERVER_H_
