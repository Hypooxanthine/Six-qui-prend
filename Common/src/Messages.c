#include "Messages.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

void SendMessage(const Socket s, const Message* msg)
{
    send(s, (const void *) msg, MIN_MESSAGE_SIZE + msg->contentSize, 0);
}

void ReceiveMessage(const Socket s, Message* msg)
{
    int receivedBytes = 0;

    // Réception de la partie du message de taille fixe.

    receivedBytes = recv(s, (void *) msg, MIN_MESSAGE_SIZE, 0);

    if(receivedBytes < 0)
    {
        perror("Erreur lors de la reception de la partie fixe d'un message ");
        exit(errno);
    }
    else if (receivedBytes < MIN_MESSAGE_SIZE)
    {
        fprintf(stderr, "Erreur lors de la reception de la partie fixe d'un message : reception de %d bytes, mais %lu etaient attendus.\n",
                receivedBytes, MIN_MESSAGE_SIZE);
        exit(EXIT_FAILURE);
    }

    // Réception de la partie du message de taille variable seulement si elle est non nulle. Cette taille est indiquée dans la partie de taille fixe.

    if(msg->contentSize > 0)
    {
        receivedBytes = recv(s, (void *) msg->content, msg->contentSize, 0);

        if(receivedBytes < 0)
        {
            perror("Erreur lors de la reception de la partie variable d'un message ");
            exit(errno);
        }
        else if (receivedBytes < msg->contentSize)
        {
            fprintf(stderr, "Erreur lors de la reception de la partie variable d'un message : reception de %d bytes, mais %lu etaient attendus.\n",
                    receivedBytes, msg->contentSize);
            exit(EXIT_FAILURE);
        }
    }
}