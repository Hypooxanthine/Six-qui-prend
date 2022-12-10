#include "Clients.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>

#include "Messages.h"

Client CreateClient(Socket serverSocket)
{
    Client out;

    socklen_t size = sizeof out.sin;

    out.socket = accept(serverSocket, (sockaddr*) &(out.socket), &size);

    if(out.socket == INVALID_SOCKET)
    {
        perror("Socket client invalide ");
        exit(errno);
    }

    return out;
}
void DeleteClient(Client* client)
{
    close(client->socket);
}

void SetClientListCapacity(ClientList* cl, size_t cap)
{
    ClientList temp = { 0 };
    temp.capacity = cap;

    size_t bytesToCopy = 0;

    if(cap < cl->size)
    {
        temp.size = cap;
        bytesToCopy = cap * sizeof(Client);
    }
    else
    {
        temp.size = cl->size;
        bytesToCopy = cl->size * sizeof(Client);
    }
    
    temp.list = (Client*)malloc(cap * sizeof(Client));

    memcpy(temp.list, cl->list, bytesToCopy);

    if(cl->list != NULL)
        free(cl->list);
    
    *cl = temp;
}

void AddClient(ClientList* cl, Client c)
{
    if(cl->size == cl->capacity)
        SetClientListCapacity(cl, cl->capacity + 1);
    
    cl->list[cl->size++] = c;
}

void FreeClientList(ClientList* cl)
{
    for(size_t i = 0 ; i < cl->size ; i++)
        close(cl->list[i].socket);
    
    free(cl->list);
}

void ClientPrint(const Client* c, const char* text)
{
    Message msg;
        msg.action = MessageAction_Print;
        msg.type = MessageType_String;
        strcpy(msg.content, text);
        msg.contentSize = strlen(msg.content) + 1;
    
    SendMessage(c->socket, &msg);
}

void ClientPrintf(const Client* c, const char* format, ...)
{
    va_list params;
    va_start(params, format);

    vClientPrintf(c, format, params);
    
    va_end(params);
}

void vClientPrintf(const Client* c, const char* format, va_list args)
{
    Message msg;
        msg.action = MessageAction_Print;
        msg.type = MessageType_String;
        vsprintf(msg.content, format, args);
        msg.contentSize = strlen(msg.content) + 1;
    
    SendMessage(c->socket, &msg);
}

void ClientNotifyf(const Client* c, const char* format, ...)
{
    va_list params;
    va_start(params, format);

    vClientNotifyf(c, format, params);
    
    va_end(params);
}

void vClientNotifyf(const Client* c, const char* format, va_list args)
{
    Message msg;
        msg.action = MessageAction_Notify;
        msg.type = MessageType_String;
        vsprintf(msg.content, format, args);
        msg.contentSize = strlen(msg.content) + 1;
    
    SendMessage(c->socket, &msg);
}

void ClientScanf(const Client* c, const char* format, ...)
{
    Message msg;
        msg.action = MessageAction_Ask;
        msg.type = MessageType_String;
        msg.contentSize = 0;
    
    SendMessage(c->socket, &msg);

    memset(&msg, 0, sizeof(Message));
    ReceiveMessage(c->socket, &msg);

    va_list params;
    va_start(params, format);

    vsscanf(msg.content, format, params);

    va_end(params);
}