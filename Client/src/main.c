#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h> /* gethostbyname */
#include <errno.h>
#include <unistd.h>

#include "Messages.h"
#include "Connect.h"

int InteractWithServer(Socket server);
void PrintMessage(const Message* msg);
void AskValue(const Message* msg, Socket server);

int main(int argc, char** argv)
{
    char hostName[30];
    GetHostName(hostName);
    Socket serverSocket = ConnectToServer(hostName, GetPort());

    // Boucle principale.
    while(InteractWithServer(serverSocket) != ENDGAME);

    close(serverSocket);

    return 0;
}

int InteractWithServer(Socket server)
{
    Message rcv = { 0 };

    ReceiveMessage(server, &rcv);

    switch(rcv.action)
    {
    case MessageAction_Print:
        PrintMessage(&rcv);
        break;
    case MessageAction_Ask:
        AskValue(&rcv, server);
        break;
    case MessageAction_Notify:
        //printf("NOTIFIED: \"%s\"\n", rcv.content);
        if(rcv.type == MessageType_String && strcmp(rcv.content, "ENDGAME") == 0)
            return ENDGAME;
        break;
    default:
        fprintf(stderr, "Erreur dans la fonction InteractWithServer().\n");
        exit(EXIT_FAILURE);
        break;
    }

    return CONTINUE;
}

void PrintMessage(const Message* msg)
{
    switch(msg->type)
    {
    case MessageType_String:
        printf("%s", (char *) msg->content);
        break;
    case MessageType_Int:
        printf("%d", *(int *) msg->content);
        break;
    default:
        fprintf(stderr, "Erreur dans la fonction PrintMessage().\n");
        exit(EXIT_FAILURE);
        break;
    }
}

void AskValue(const Message* msg, Socket server)
{
    Message sending = { 0 };
        sending.action = MessageAction_Notify;
        sending.type = msg->type;

    switch(msg->type)
    {
    case MessageType_String:
        scanf("%s", (char *)sending.content);
        sending.contentSize = strlen(sending.content) + 1;
        break;
    case MessageType_Int:
        scanf("%d", (int *)sending.content);
        sending.contentSize = sizeof(int);
        break;
    default:
        fprintf(stderr, "Erreur dans la fonction AskValue().\n");
        exit(EXIT_FAILURE);
        break;
    }

    SendMessage(server, &sending);
}