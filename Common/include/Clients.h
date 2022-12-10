#ifndef CLIENTS_H
#define CLIENTS_H

#include "Types.h"
#include <stdarg.h>

typedef struct Client
{
    Socket socket;
    sockaddr_in sin;
} Client;

typedef struct ClientList
{
    Client* list;
    size_t size;
    size_t capacity;
} ClientList;

Client CreateClient(Socket serverSocket);
void DeleteClient(Client* client);

void SetClientListCapacity(ClientList* cl, size_t cap);
void AddClient(ClientList* cl, Client c);

void FreeClientList(ClientList* cl);

void ClientPrint(const Client* c, const char* text);
void ClientPrintf(const Client* c, const char* format, ...);
void vClientPrintf(const Client* c, const char* format, va_list args);

void ClientNotifyf(const Client* c, const char* format, ...);
void vClientNotifyf(const Client* c, const char* format, va_list args);

void ClientScanf(const Client* c, const char* format, ...);

#endif