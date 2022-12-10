#include "Behaviour.h"

#include <stdio.h>
#include <stdlib.h>

#include "Players.h"

uint8_t ChooseCard(Row* rows, Card* hand, CardChoiceBehaviour mode)
{
    switch(mode)
    {
    case CardChoiceBehaviour_MaxVal:
        return ChooseCard_MaxVal(rows, hand);
    case CardChoiceBehaviour_MinVal:
        return ChooseCard_MinVal(rows, hand);
    case CardChoiceBehaviour_Random:
        return ChooseCard_Random(rows, hand);
    default:
        fprintf(stderr, "Mode de choix de carte inexistant.\n");
        exit(EXIT_FAILURE);
    }
}

uint8_t ChooseRow(Row* rows, Card* hand, RowChoiceBehaviour mode)
{
    switch(mode)
    {
    case RowChoiceBehaviour_MinCowsSum:
        return ChooseRow_MinCowsSum(rows, hand);
    case RowChoiceBehaviour_Random:
        return ChooseRow_Random(rows, hand);
    default:
        fprintf(stderr, "Mode de choix de colonne inexistant.\n");
        exit(EXIT_FAILURE);
    }
}

uint8_t ChooseCard_MaxVal(Row* rows, Card* hand)
{
    uint8_t selected = 0;

    for(size_t i = 1 ; i < MAX_CARDS_IN_HAND ; i++)
    {
        if(hand[i].value > hand[selected].value)
            selected = i;
    }

    return selected + 1; // +1 car l'IA mime un comportement humain, donc l'IA sélectionne entre 1 et 10, pas entre 0 et 9
}

uint8_t ChooseCard_MinVal(Row* rows, Card* hand)
{
    uint8_t selected = 0;

    for(size_t i = 1 ; i < MAX_CARDS_IN_HAND ; i++)
    {
        if(hand[i].value < hand[selected].value)
            selected = i;
    }

    return selected + 1; // +1 car l'IA mime un comportement humain, donc l'IA sélectionne entre 1 et 10, pas entre 0 et 9
}

uint8_t ChooseCard_Random(Row* rows, Card* hand)
{
    uint8_t selected = 0;
    uint8_t randVal = rand() % MAX_CARDS_IN_HAND + 1;

    // On va itérer entre 1 et 10 fois sur les cartes de la main et sélectionner la carte (un peu comme le jeu "Am stram gram")
    for(size_t i = 1 ; randVal > 0 ; i = (i + 1) % MAX_CARDS_IN_HAND)
    {
        if(hand[i].value > 0)
            randVal++;
        
        if(randVal == 0)
            selected = i;
    }

    return selected + 1; // +1 car l'IA mime un comportement humain, donc l'IA sélectionne entre 1 et 10, pas entre 0 et 9
}

uint8_t ChooseRow_MinCowsSum(Row* rows, Card* hand)
{
    uint8_t out = 0;
    uint8_t outCowsSum = 0;

    for(size_t i = 0 ; i < ROW_NUMBER ; i++)
    {
        uint8_t cowsSum = 0;

        for(size_t j = 0 ; j < rows[i].size ; j++)
        {
            cowsSum += rows[i].cards[j].cows;
        }

        if(cowsSum < outCowsSum)
        {
            out = i;
            outCowsSum = cowsSum;
        }
    }

    return out + 1; // +1 car l'IA mime un comportement humain, donc l'IA sélectionne entre 1 et 4, pas entre 0 et 3
}

uint8_t ChooseRow_Random(Row* rows, Card* hand)
{
    return rand() % ROW_NUMBER + 1; // +1 car l'IA mime un comportement humain, donc l'IA sélectionne entre 1 et 4, pas entre 0 et 3
}