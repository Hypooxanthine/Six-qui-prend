#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define IA_NUMBER 4

static char* serverCall = NULL;
static char* IACall = NULL;

static char* serverRawCall = "./Server/Server ";
static char* IARawCall = "./IA/IA ";

typedef struct ClientThread
{
    pthread_t t;
    size_t clientNb;
} ClientThread;

pthread_t server_thread;
ClientThread client_threads[IA_NUMBER];

void* server(void* data)
{
    FILE *fp;

    fp = popen(serverCall, "r");

    if(fp == NULL)
    {
        fprintf(stderr, "Impossible d'executer la commande : %s", serverCall);
        exit(EXIT_FAILURE);
    }

    char buffer[2048];

    while (fgets(buffer, 2048, fp) != NULL)
    {
        //printf("SERVEUR : %s", buffer);
    }

    pclose(fp);
}

void* ia(void* data)
{
    FILE *fp;

    fp = popen(IACall, "r");

    if(fp == NULL)
    {
        fprintf(stderr, "Impossible d'executer la commande : %s", serverCall);
        exit(EXIT_FAILURE);
    }

    char outputLine[2048];

    while (fgets(outputLine, 2048, fp) != NULL)
    {
        //printf("CLIENT %lu : %s", *(size_t*)data, outputLine);
    }

    pclose(fp);
}

int main(int argc, char** argv)
{
    // Si on utilise un argument d'appel, on appelle les clients avec cet argument.
    if(argc > 1)
    {
        IACall = (char*)malloc(sizeof(char) * (strlen(IARawCall) + strlen(argv[1]) + 1));
        strcpy(IACall, IARawCall);
        strcat(IACall, argv[1]);
    }
    else // Sinon on appelle les clients sans argument.
    {
        IACall = IARawCall;
    }

    // L'appel du serveur se fait avec le nombre d'IA que l'on souhaite utiliser.
    char buffer[16];
    sprintf(buffer, "%d", IA_NUMBER);
    serverCall = (char*)malloc(sizeof(char) * (strlen(serverRawCall) + strlen(buffer) + 1));
    strcpy(serverCall, serverRawCall);
    strcat(serverCall, buffer);

    // Valeur d'erreur de retour quand on crée un thread.
    int t_creation_ret = pthread_create(&server_thread, NULL, server, NULL);

    // Création du thread serveur
    if(t_creation_ret != 0)
    {
        fprintf(stderr, "Erreur lors de la creation du thread serveur : %s\n", strerror(t_creation_ret));
        return EXIT_FAILURE;
    }

    // Création des threads clients (les IA)
    for(size_t i = 0 ; i < IA_NUMBER ; i++)
    {
        client_threads[i].clientNb = i;
        t_creation_ret = pthread_create(&client_threads[i].t, NULL, ia, (void*)(&client_threads[i].clientNb));

        if(t_creation_ret != 0)
        {
            fprintf(stderr, "Erreur lors de la creation du thread client numero %lu : %s\n", i, strerror(t_creation_ret));
            return EXIT_FAILURE;
        }
    }

    // On attend la fin de tous les threads.
    for(size_t i = 0 ; i < IA_NUMBER ; i++)
        pthread_join(client_threads[i].t, NULL);
    
    pthread_join(server_thread, NULL);

    if(argc > 1)
    {
        free(serverCall);
        free(IACall);
    }

    return EXIT_SUCCESS;
}