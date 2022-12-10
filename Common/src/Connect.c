#include "Connect.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <errno.h>

Socket ConnectToServer(const char* hostName, in_port_t port)
{
    Socket serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(serverSocket == INVALID_SOCKET)
    {
        perror("Impossible de creer la socket client ");
        exit(errno);
    }
    else
        printf("Socket client creee.\n");

    // Data
    struct hostent *hostinfo = NULL;
    sockaddr_in sin = { 0 };

    hostinfo = gethostbyname(hostName); // Informations sur l'hôte

    if (hostinfo == NULL)
    {
        perror("Hote inconnu");
        exit(errno);
    }

    sin.sin_addr = *(in_addr *) hostinfo->h_addr;
    sin.sin_port = htons((uint16_t)port);
    sin.sin_family = AF_INET;

    if(connect(serverSocket,(sockaddr *) &sin, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        perror("Connexion au serveur impossible");
        exit(errno);
    }

    printf("Connection etablie.\n\n\n");

    return serverSocket;
}

void GetHostName(char* hostName)
{
#ifndef USE_DEFAULT_ADRESS
    // Récuperation du nom de l'hôte (adresse IP) auprès de l'utilisateur.
    printf("Nom de l'hote : "); scanf("%s", hostName);
#else
    strcpy(hostName, DEFAULT_HOST_NAME);
#endif
}

in_port_t GetPort()
{
#ifndef USE_DEFAULT_ADRESS
    // Récupération du port de connexion auprès de l'utilisateur.
    uint32_t port = 0;
    printf("Port de connexion : "); scanf("%u", &port);
    return port;
#else
    return DEFAULT_PORT;
#endif
}