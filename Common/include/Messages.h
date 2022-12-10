#ifndef MESSAGES_H
#define MESSAGES_H

#include "Types.h"

typedef enum
{
    MessageAction_Ask, MessageAction_Print, MessageAction_Notify
} MessageAction;

typedef enum
{
    MessageType_String, MessageType_Int
} MessageType;

#define MAX_MESSAGE_SIZE (4096UL)
#define MIN_MESSAGE_SIZE (sizeof(MessageAction) + sizeof(MessageType) + sizeof(size_t))
#define MAX_MESSAGE_CONTENT_SIZE (MAX_MESSAGE_SIZE - MIN_MESSAGE_SIZE)

// Structure du message :
// 1. Action
// 2. Type
// 3. Taille du contenu
// 4. Contenu
typedef struct __attribute__((__packed__))
{
    MessageAction action; // Taille fixe au regard du contenu utile
    MessageType type; // Taille fixe au regard du contenu utile
    size_t contentSize; // Taille fixe au regard du contenu utile
    char content[MAX_MESSAGE_CONTENT_SIZE]; // Taille variable au regard du contenu utile
} Message;

void SendMessage(const Socket s, const Message* msg);
void ReceiveMessage(const Socket s, Message* msg);

#endif