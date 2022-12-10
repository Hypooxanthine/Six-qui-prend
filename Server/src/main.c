#include <stdio.h>
#include <stdlib.h>

#include <netdb.h> /* gethostbyname */
#include <errno.h>

#include <string.h>

#include "Types.h"
#include "Game.h"

int main(int argc, char** argv)
{
#ifndef USE_DEFAULT_ADRESS
    // Récupération du port auprès de l'utilisateur.
    uint32_t serverPort = 0;
    printf("Port du serveur : "); scanf("%u", &serverPort);
#else
    uint32_t serverPort = DEFAULT_PORT;
#endif    

    // Création de la socket serveur.
    Socket serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == INVALID_SOCKET)
    {
        perror("Impossible de créer serverSocket ");
        exit(errno);
    }
    else
        printf("Socket serveur cree.\n");

    // Création de l'interface d'écoute.
    sockaddr_in sin = { 0 };
        sin.sin_addr.s_addr = htonl(INADDR_ANY); // Puisqu'on est sur le serveur, on ne spécifie aucune adresse en particulier.
        sin.sin_family = AF_INET;
        sin.sin_port = htons((uint16_t)serverPort); // Port d'écoute défini par l'utilisateur.

    // Liaison de la socket serveur avec son interface.
    if(bind(serverSocket, (sockaddr *) &sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        perror("Impossible de lier la serverSocket avec son interface ");
        exit(errno);
    }
    else
        printf("Le socket serveur a ete lie a l'interface d'ecoute.\n");

    // L'utilisateur défini le nombre de clients pour la partie.
    size_t clientsNumber = 0;
    printf("Entrer le nombre de joueurs : "); scanf("%lu", &clientsNumber);

    if(clientsNumber < 2 || clientsNumber > 10)
    {
        fprintf(stderr, "Au moins 2 joueurs, et maximum 10 joueurs doivent pouvoir rejoindre la partie.\n");
        exit(EXIT_FAILURE);
    }

    // Préparation de la mise à l'écoute du serveur.
    if(listen(serverSocket, clientsNumber) == SOCKET_ERROR)
    {
        perror("Impossible de mettre le serveur à l'écoute ");
        exit(errno);
    }
    else
        printf("Le serveur est pret a ecouter %lu connexions.\n", clientsNumber);

    ClientList clients = { 0 };
    SetClientListCapacity(&clients, clientsNumber);

    printf("Mise a l'ecoute du serveur...\n");
    for(size_t i = 0 ; i < clientsNumber ; i++)
    {
        AddClient(&clients, CreateClient(serverSocket));
        printf("Client %lu/%lu accepte.\n", i + 1, clientsNumber);
    }

    printf("Lancement de la partie.\n");
    StartGame(&clients);

    FreeClientList(&clients);
    close(serverSocket);

    return 0;
}