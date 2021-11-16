#ifndef __Asock
#define __Asock

#ifdef _WIN32 // Windows
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifdef _WIN32 // Windows
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <c/dbg.h>
#include <stdio.h>
#include "bstrlib.h"


/*
 * @brief A unified socket struct
 */
typedef struct Asoc {
#ifdef _WIN32 // Windows
	struct addrinfo addr;
	struct addrinfo hints;
	SOCKET socket;	
#else 				// Linux
	int socket;
	struct sockaddr_in addr;
#endif
} Asoc; 

Asoc *Asoc_create(int Type,int Proto,int port);
Asoc *Asoc_TCP4_create(int port);
Asoc *Asoc_UDP4_create(int port);

#ifdef _WIN32 // Windows
int WSA_INIT=1;
WSADATA wsaData;

int AInit(){
	int result = WSAStartup(MAKEWORD(2,2),&wsaData);
  check(result == 0,"WSAStartup failed with error: %d",result);
	return 0;
error:
	return 1;
}
#endif

#endif
