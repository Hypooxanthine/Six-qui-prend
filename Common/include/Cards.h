#ifndef CARDS_H
#define CARDS_H

#include "Types.h"

#define CARD_NUMBER 104
#define ROW_NUMBER 4

typedef struct Card
{
    uint8_t value; // De 1 à 104
    uint8_t cows; // De 1 à 7 (têtes de vache)
    uint8_t consoleColor; // Couleur de la carte dans la console
} Card;

typedef struct Deck
{
    Card cards[CARD_NUMBER];
    // Le topCursor correspond à la position de la partie "distribuée" du deck. Avant le topCursor se trouvent les cartes qui sont encore dans le deck.
    uint8_t topCursor;
} Deck;

typedef struct Row
{
    Card cards[5];
    uint8_t size;
} Row;

/* Fonctions relatives au deck */

// Construit le deck entièrement et le mélange.
void BuildDeck(Deck* deck);
// Mélange un deck. (Le mélange n'a lieu que sur la partie non distribuée du deck)
void ShuffleDeck(Deck* deck);
// Affiche le deck dans la sortie standard.
void PrintDeck(const Deck* deck);

// Retourne la carte du dessus du packet. Au niveau du deck, la carte retournée passe dans la partie "distribuée" du deck.
Card DrawCardFromDeck(Deck* deck);

/* Fonctions relatives aux rangéesde cartes */

// Construit une colonne vide.
void BuildRow(Row* row);
// Place une carte en haut d'une colonne.
void PutCardOnRow(Row* row, Card card);
// Retourne la colonne où la valeur de la dernière carte est la plus proche de la valeur de "card". Retourne NULL si la valeur de "card" est inférieure à toutes les colonnes.
Row* GetClosestValueRow(Row* rows, Card card);
// Vide entièrement la colonne.
void ClearRow(Row* row);

typedef struct PlayerList PlayerList; // "Forward declaration"
// Affiche toutes les colonnes
void BroadcastRows(Row* rows, const PlayerList* players);
// Notifie toutes les colonnes
void NotifyRows(Row* rows, const PlayerList* players);

#endif