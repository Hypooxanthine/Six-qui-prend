#ifndef PLAYERS_H
#define PLAYERS_H

#include "Types.h"
#include "Clients.h"
#include "Cards.h"

#define MAX_CARDS_IN_HAND 10

typedef struct Player
{
    Client clientRef;
    char name[21];
    Card hand[MAX_CARDS_IN_HAND];
    uint8_t chosenCardIndex;
    Card chosenCard;
    uint8_t points; // La partie s'arrête quand un joueur atteint 66 points.
} Player;

typedef struct PlayerList
{
    Player* list;
    size_t size;
} PlayerList;

// Affiche un message formatté chez tous les joueurs.
void Broadcast(const PlayerList players, const char* format, ...);
// Notifie tous les joueurs.
void NotifyAll(const PlayerList players, const char* format, ...);
// Affiche les scores
void BroadcastScores(const PlayerList players);
// Notifie les scores
void NotifyScores(const PlayerList players);
// Affiche sa main à chaque joueur.
void BroadcastHands(const PlayerList players);
// Notifie sa main à chaque joueur
void NotifyHands(const PlayerList players);

// Retourne vrai si un joueur a 66 points ou plus. Retourne faux sinon.
int CheckEndGame(const PlayerList players);

// Retourne un pointeur vers le joueur ayant choisi la carte de plus faible valeur. Retourne NULL si aucun joueur n'a sélectionné de carte.
Player* FindLowestValuePlayer(const PlayerList players);

void PlayerPickAllCardsFromRow(Player* player, Row* row);

#endif