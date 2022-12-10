#ifndef TYPES_H
#define TYPES_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct in_addr in_addr;

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int Socket;

// Ceci permet d'utilser le nom d'hôte et le port par défaut
#define USE_DEFAULT_ADRESS

#define DEFAULT_HOST_NAME "localhost"
#define DEFAULT_PORT 8000

#define ENDGAME 1
#define CONTINUE 0

#endif