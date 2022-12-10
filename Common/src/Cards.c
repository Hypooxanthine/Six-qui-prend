#include "Cards.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Players.h"

void BuildDeck(Deck* deck)
{
    for(size_t i = 0 ; i < CARD_NUMBER ; i++)
    {
        size_t nb = i + 1;
        
        deck->cards[i].value = nb;

        deck->cards[i].cows = 0;

        if(nb % 5 == 0) deck->cards[i].cows += 2;
        if(nb % 10 == 0) deck->cards[i].cows += 1;
        if(nb % 11 == 0) deck->cards[i].cows += 5;

        if(deck->cards[i].cows == 0) deck->cards[i].cows = 1;

        switch(deck->cards[i].cows)
        {
        case 1:
            deck->cards[i].consoleColor = 35; // Dark magenta
            break;
        case 2:
            deck->cards[i].consoleColor = 96; // Cyan
            break;
        case 3:
            deck->cards[i].consoleColor = 93; // Orange
            break;
        case 5:
            deck->cards[i].consoleColor = 91; // Red
            break;
        case 7:
            deck->cards[i].consoleColor = 31; // Dark red
            break;
        default:
            fprintf(stderr, "Une carte a une valeur interdite de nombre de vaches : %u.\n", deck->cards[i].cows);
            exit(EXIT_FAILURE);
        }
    }

    deck->topCursor = CARD_NUMBER;

    ShuffleDeck(deck);
}

void ShuffleDeck(Deck* deck)
{
    srand(time(NULL));
    int r = rand() % CARD_NUMBER;

    // Méthode de mélange choisie : on parcourt chaque carte une à une et on l'inverse avec une carte aléatoire du deck.
    for(size_t i = 0 ; i < deck->topCursor ; i++)
    {
        int swap = rand() % (deck->topCursor);

        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[swap];
        deck->cards[swap] = temp;
    }
}

void PrintDeck(const Deck* deck)
{
    printf("Deck position | Value | Cows\n");

    for(size_t i = 0 ; i < CARD_NUMBER ; i++)
    {
        printf("%7lu         %3u     %2u\n", i, deck->cards[i].value, deck->cards[i].cows);
    }
}

Card DrawCardFromDeck(Deck* deck)
{    
    return deck->cards[--deck->topCursor];
}

void BuildRow(Row* row)
{
    ClearRow(row);
}

void PutCardOnRow(Row* row, Card card)
{
    if(row->size < 5)
    {
        row->cards[row->size] = card;
        row->size++;
    }
    else
    {
        fprintf(stderr, "Impossible d'ajouter une sixieme carte dans la rangee.\n");
        exit(EXIT_FAILURE);
    }
}

Row* GetClosestValueRow(Row* rows, Card card)
{
    Row* closest = NULL;

    for(size_t i = 0 ; i < ROW_NUMBER ; i++)
    {
        Row* row = rows + i;
        uint8_t lastCardValue = row->cards[row->size - 1].value;
        
        if(card.value < lastCardValue) continue;

        if(closest == NULL || lastCardValue > closest->cards[closest->size - 1].value)
            closest = row;
    }

    return closest;
}

void ClearRow(Row* row)
{
    row->size = 0;
}

void BroadcastRows(Row* rows, const PlayerList* players)
{
    Broadcast(*players, 
            " Colonne 1 | Colonne 2 | Colonne 3 | Colonne 4 \n");
    
    int stop = 0;

    for(size_t i = 0 ; i < 5 && !stop ; i++) // On a max 5 cartes par colonne (donc 5 lignes max)
    {
        stop = 1;

        for(size_t j = 0 ; j < 4 ; j++) // On regarde dans les 4 colonnes
        {
            if(rows[j].size > i)
            {
                stop = 0;

                Broadcast(*players, "   \033[%um%+3u|%-1u\033[0m    ", rows[j].cards[i].consoleColor, rows[j].cards[i].value, rows[j].cards[i].cows);
            }
            else
            {
                Broadcast(*players, "            ");
            }
        }

        Broadcast(*players, "\n");
    }
}

void NotifyRows(Row* rows, const PlayerList* players)
{
    char strToNotify[256] = { 0 };
    size_t cursor = 0;

    sprintf(strToNotify + cursor, "ROWS=[");
    cursor += strlen(strToNotify + cursor);

    for(size_t j = 0 ; j < 4 ; j++) // On regarde dans les 4 colonnes
    {
        sprintf(strToNotify + cursor, "[");
        cursor += strlen(strToNotify + cursor);
        
        for(size_t i = 0 ; i < rows[j].size ; i++) // On a max 5 cartes par colonne (donc 5 lignes max)
        {
            sprintf(strToNotify + cursor, "%u|%u", rows[j].cards[i].value, rows[j].cards[i].cows);

            cursor += strlen(strToNotify + cursor);

            if(i < rows[j].size - 1)
                sprintf(strToNotify + cursor, ",");
            else
                sprintf(strToNotify + cursor, "]");
        
            cursor += strlen(strToNotify + cursor);
        }

        if(j < 3)
            sprintf(strToNotify + cursor, ",");
        else
            sprintf(strToNotify + cursor, "]");

        cursor += strlen(strToNotify + cursor);
    }

    NotifyAll(*players, strToNotify);
}