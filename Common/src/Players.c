#include "Players.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void Broadcast(const PlayerList players, const char* format, ...)
{
    for(size_t i = 0 ; i < players.size ; i++)
    {
        va_list params;
        va_start(params, format);

        vClientPrintf(&players.list[i].clientRef, format, params);

        va_end(params);
    }
}

void NotifyAll(const PlayerList players, const char* format, ...)
{
    for(size_t i = 0 ; i < players.size ; i++)
    {
        va_list params;
        va_start(params, format);

        vClientNotifyf(&players.list[i].clientRef, format, params);

        va_end(params);
    }
}

void BroadcastScores(const PlayerList players)
{
    Broadcast(players, "Total des scores :\n");

    for(size_t i = 0 ; i < players.size ; i++)
    {
        Broadcast(players, "\n\t%s : %u points.", players.list[i].name, players.list[i].points);
    }

    Broadcast(players, "\n");
}

void NotifyScores(const PlayerList players)
{
    char buffer[1024];

    strcat(buffer, "SCORES=[");

    for(size_t i = 0 ; i < players.size ; i++)
    {
        sprintf(buffer + strlen(buffer), "%hhu", players.list[i].points);

        if(i < players.size - 1)
            strcat(buffer, ",");
    }

    strcat(buffer, "]");

    NotifyAll(players, buffer);
}

void BroadcastHands(const PlayerList players)
{
    for(size_t i = 0 ; i < players.size ; i++)
    {
        Player* player = &players.list[i];

        ClientPrintf(&player->clientRef, "Voici votre main :");

        for(size_t j = 0 ; j < MAX_CARDS_IN_HAND ; j++)
        {
            if(player->hand[j].value > 0)
            {
                ClientPrintf
                (
                    &player->clientRef,
                    "\n\tCarte %-2lu - \033[%umValeur %+3u | %u vaches\033[0m",
                    j + 1, player->hand[j].consoleColor, player->hand[j].value, player->hand[j].cows
                );
            }
            else
            {
                ClientPrintf
                (
                    &player->clientRef,
                    "\n\tCarte %-2lu - Carte jouee.",
                    j + 1
                );
            }
        }

        ClientPrint(&player->clientRef, "\n");
    }
}

void NotifyHands(const PlayerList players)
{

    for(size_t i = 0 ; i < players.size ; i++)
    {
        char strToNotify[256] = { 0 };
        size_t cursor = 0;

        sprintf(strToNotify + cursor, "HAND=[");
        cursor += strlen(strToNotify + cursor);

        Player* player = &players.list[i];

        for(size_t j = 0 ; j < MAX_CARDS_IN_HAND ; j++)
        {
            if(player->hand[j].value > 0)
                sprintf(strToNotify + cursor, "%u|%u", player->hand[j].value, player->hand[j].cows);
            else
                sprintf(strToNotify + cursor, "NULL");
            
            cursor += strlen(strToNotify + cursor);
            
            if(j < MAX_CARDS_IN_HAND - 1)
                sprintf(strToNotify + cursor, ",");
            else
                sprintf(strToNotify + cursor, "]");

            cursor += strlen(strToNotify + cursor);
        }

        ClientNotifyf(&player->clientRef, strToNotify);
    }
}

int CheckEndGame(const PlayerList players)
{
    for(size_t i = 0 ; i < players.size ; i++)
    {
        if(players.list[i].points >= 66) return 1;
    }

    return 0;
}

Player* FindLowestValuePlayer(const PlayerList players)
{
    Player* out = NULL;
    
    for(size_t i = 0 ; i < players.size ; i++)
    {
        Player* player = &players.list[i];

        // Si le joueur courant n'a choisi aucune carte, alors on passe au joueur suivant.
        if(player->chosenCard.value == 0) continue;

        // Sinon : si "out" n'a pas encore été défini,
        // ou alors, si "out" est défini mais sa carte choisie a une plus grande valeur que la carte choisie du joueur courant,
        // alors "out" devient le joueur courant.
        if(out == NULL || player->chosenCard.value < out->chosenCard.value)
            out = player;
    }

    return out; // Sera NULL si aucun joueur n'a choisi de carte.
}

void PlayerPickAllCardsFromRow(Player* player, Row* row)
{
    for(size_t i = 0 ; i < row->size ; i++)
    {
        player->points += row->cards[i].cows;
        row->cards[i].value = 0; // On supprime la carte de la colonne
    }

    row->size = 0; // La colonne est maintenant vide.
}