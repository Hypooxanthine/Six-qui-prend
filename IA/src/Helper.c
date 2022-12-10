#include "Helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Messages.h"
#include "Players.h"

void Notify(Socket server, const char* content)
{
    Notifyf(server, content);
}

void Notifyf(Socket server, const char* format, ...)
{
    va_list params;
    va_start(params, format);

    vNotifyf(server, format, params);
    
    va_end(params);
}

void vNotifyf(Socket server, const char* format, va_list args)
{
    Message msg;
        msg.action = MessageAction_Notify;
        msg.type = MessageType_String;
        vsprintf(msg.content, format, args);
        msg.contentSize = strlen(msg.content) + 1;
    
    printf("Envoi d'un message au serveur : %s\n", msg.content);
    SendMessage(server, &msg);
}

int StartsWith(const char* str, const char* start)
{
    if(strncmp(str, start, strlen(start)) == 0) return 1;
    else return 0;
}

const char* ExtractVariable(const char* str)
{
    while(*str != '\0')
    {
        if(*str == '=') return str + 1;

        str++;
    }
    
    return str;
}

void ParseCard(Card* card, const char* str)
{
    if(strncmp(str, "NULL", strlen("NULL")) == 0)
    {
        card->value = 0;
        card->cows = 0;
    }
    else
        sscanf(str, "%hhu|%hhu", &card->value, &card->cows);
}

int ParseCards(Card* cards, const char* str, size_t n)
{
    size_t cursor = 0;
    int stop = 0;
    int readCards = 0;
    cursor += strlen("[");

    for(size_t i = 0 ; i < n && !stop ; i++)
    {
        ParseCard(cards + i, str + cursor);

        char cardBuffer[16] = { 0 };

        while(str[cursor] != ',')
        {
            if(str[cursor] == '\0')
            {
                fprintf(stderr, "Erreur dans la fonction ParseHand.\n");
                exit(EXIT_FAILURE);
            }
            if(str[cursor] == ']')
            {
                stop = 1;
                break;
            }

            strncat(cardBuffer, str + cursor, 1);
            cursor++;
        }

        cursor++;

        //sscanf(cardBuffer, "%hhu|%hhu", &cards[i].value, &cards[i].cows);

        readCards++;
    }

    return readCards;
}

void ParseHand(Card* hand, const char* str)
{
    ParseCards(hand, str, MAX_CARDS_IN_HAND);
}

void ParseRows(Row* rows, const char* str)
{
    memset(rows, 0, ROW_NUMBER * sizeof(Row));

    size_t cursor = 0;
    cursor += strlen("[");

    for(size_t i = 0 ; i < ROW_NUMBER ; i++)
    {
        rows[i].size = ParseCards(rows[i].cards, str + cursor, 5);

        while(str[cursor++] != ']');

        if(str[cursor] == ',') str++;
    }
}