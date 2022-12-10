#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "Cards.h"

typedef enum
{
    CardChoiceBehaviour_Random = 0,
    CardChoiceBehaviour_MaxVal,
    CardChoiceBehaviour_MinVal,
} CardChoiceBehaviour;

typedef enum
{
    RowChoiceBehaviour_Random = 0,
    RowChoiceBehaviour_MinCowsSum,
} RowChoiceBehaviour;

uint8_t ChooseCard(Row* rows, Card* hand, CardChoiceBehaviour mode);
uint8_t ChooseRow(Row* rows, Card* hand, RowChoiceBehaviour mode);

// Stratégie : on choisit la carte de plus haute valeur
uint8_t ChooseCard_MaxVal(Row* rows, Card* hand);
// Stratégie : on choisit la carte de plus basse valeur
uint8_t ChooseCard_MinVal(Row* rows, Card* hand);
// Stratégie : on choisit une carte au hasard
uint8_t ChooseCard_Random(Row* rows, Card* hand);
// Stratégie : on choisit la colonne de plus faible valeur de vaches cumulées
uint8_t ChooseRow_MinCowsSum(Row* rows, Card* hand);
// Stratégie : on choisit une colonne au hasard
uint8_t ChooseRow_Random(Row* rows, Card* hand);

#endif