#include "Game.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "Messages.h"
#include "Clients.h"
#include "Cards.h"
#include "Players.h"

#define BROADCAST(...) Broadcast(players, __VA_ARGS__)
#define NOTIFYALL(...) NotifyAll(players, __VA_ARGS__)

// Variables statiques (= limitées à cette unité de traduction)
static PlayerList players;
static Deck deck = { 0 };
static Row rows[ROW_NUMBER] = { 0 };

static size_t roundNumber = 0, turnNumber = 0;

void StartGame(ClientList* clients)
{
    // Joueurs
    printf("Creation des joueurs.\n");
    players.list = (Player*)malloc(clients->size * sizeof(Player));
    players.size = clients->size;

    for(size_t i = 0 ; i < clients->size ; i++)
    {
        players.list[i].clientRef = clients->list[i];
        players.list[i].chosenCardIndex = 0;
        players.list[i].chosenCard.value = 0; // Aucune carte n'est choisie au départ.
        players.list[i].points = 0;
    }

    // On demande aux joueurs leur nom.
    printf("Demande du nom des joueurs...\n");

    for(size_t i = 0 ; i < players.size ; i++)
    {
        Player* player = &(players.list[i]);
        int nameOK = 0;
        
        while(!nameOK)
        {
            ClientNotifyf(&player->clientRef, "PLAYER_ID=%lu", i);
            ClientPrintf(&player->clientRef, "Joueur %lu/%lu. Entrer votre nom (20 caracteres max) : ", i + 1, players.size);
            char nameBuffer[MAX_MESSAGE_CONTENT_SIZE];

            ClientNotifyf(&player->clientRef, "NEED_PLAYER_NAME");
            ClientScanf(&player->clientRef, "%s", nameBuffer);

            if(strlen(nameBuffer) < 21)
            {
                strcpy(player->name, nameBuffer);
                nameOK = 1;
            }
        }

        printf("Nom du joueur %lu : %s.\n", i, player->name);
    }

    // On affiche la liste des joueurs chez tous les joueurs.
    NOTIFYALL("NUMBER_OF_PLAYERS=%lu", players.size);
    BROADCAST("Liste des joueurs :");

    for(size_t i = 0 ; i < players.size ; i++)
    {
        const Player* player = &(players.list[i]);
        BROADCAST("\n\t%s", player->name);
    }

    BROADCAST("\n\n\n");

    // Boucle de jeu
    while(Round() != ENDGAME);

    BROADCAST("---------- Fin de partie.\n");
    NotifyScores(players);
    BroadcastScores(players);

    // Fin de partie.
    NOTIFYALL("ENDGAME");
    
    free(players.list);
}

int Round()
{
    roundNumber++;

    NOTIFYALL("ROUND=%lu", roundNumber);
    BROADCAST("---------- Debut de la manche %lu\n", roundNumber);

    NotifyScores(players);
    BroadcastScores(players);

    BROADCAST("\n");

    turnNumber = 0;

    // On remet le jeu à zéro.
    BuildDeck(&deck);

    // On remet les colonnes à zéro et on place une première carte dans chacune.
    for(size_t i = 0 ; i < ROW_NUMBER ; i++)
    {
        BuildRow(rows + i);
        PutCardOnRow(rows + i, DrawCardFromDeck(&deck));
    }

    // On distribue 10 cartes à chaque joueur.
    for(size_t i = 0 ; i < players.size ; i++)
    {
        Player* player = &players.list[i];

        for(size_t j = 0 ; j < MAX_CARDS_IN_HAND ; j++)
        {
            player->hand[j] = DrawCardFromDeck(&deck);
        }
    }

    for(size_t i = 0 ; i < 10 ; i++) Turn();

    NotifyRows(rows, &players);
    BroadcastRows(rows, &players);
    BROADCAST("\nFin du round.\n");

    if(CheckEndGame(players)) return ENDGAME;
    else return CONTINUE;
}

void Turn()
{
    turnNumber++;
    BROADCAST("----- Debut du tour %lu/10\n\n", turnNumber);

    NotifyRows(rows, &players);
    BroadcastRows(rows, &players);

    NotifyHands(players);
    BroadcastHands(players);

    BROADCAST("\n");

    // On demande à chaque joueur quelle carte il souhaite jouer ce tour.
    for(size_t i = 0 ; i < players.size ; i++)
    {
        Player* player = &players.list[i];

        do
        {
            ClientNotifyf(&players.list[i].clientRef, "NEED_CARD_CHOICE");
            ClientPrintf(&players.list[i].clientRef, "Selectionnez la carte que vous souhaitez jouer : ");
            ClientScanf(&players.list[i].clientRef, "%u", &player->chosenCardIndex);
        }
        // On veut que la carte soit entre 1 et 10 et que la carte associée ne soit pas déjà jouée.
        while (!(player->chosenCardIndex >= 1 && player->chosenCardIndex <= 10 && players.list[i].hand[player->chosenCardIndex - 1].value > 0));

        player->chosenCardIndex--; // L'utilisateur choisit entre 1 et 10 mais les valeurs pour le programme sont entre 0 et 9.
        player->chosenCard = player->hand[player->chosenCardIndex];
        player->hand[player->chosenCardIndex].value = 0; // On retire la carte choisie de la main de l'utilisateur
    }

    // On affiche pour tout le monde la carte choisie par chaque joueur.
    BROADCAST("Tous les joueurs ont choisi leur carte :");
    for(size_t i = 0 ; i < players.size ; i++)
    {
        BROADCAST
        (
            "\n\t%s a choisi \033[%um%+3u|%-1u\033[0m.",
            players.list[i].name, players.list[i].chosenCard.consoleColor, players.list[i].chosenCard.value, players.list[i].chosenCard.cows
        );
    }

    BROADCAST("\n");

    // On doit maintenant ajouter les cartes choisies dans les colonnes correspondantes.
    for(size_t i = 0 ; i < players.size ; i++)
    {
        // On récupère le joueur ayant la carte de plus faible valeur.
        // Cette valeur ne devrait jamais être NULL si le nombre d'itérations est correct.
        Player* lowerValuePlayer = FindLowestValuePlayer(players);

        // On récupère la colonne la plus proche de la carte du joueur mais de valeur inférieure à la carte du joueur.
        Row* closestRow = GetClosestValueRow(rows, lowerValuePlayer->chosenCard);

        if(closestRow != NULL) // Si on a trouvé une colonne où le joueur peur poser sa carte
        {
            if(closestRow->size < 5) // Si la colonne n'est pas complète
                PutCardOnRow(closestRow, lowerValuePlayer->chosenCard);
            else
            {
                PlayerPickAllCardsFromRow(lowerValuePlayer, closestRow);
                PutCardOnRow(closestRow, lowerValuePlayer->chosenCard);
            }
        }
        else // Aucune colonne ne peut accueillir la carte du joueur
        {
            // On doit demander au jouer quelle colonne il souhaite récupérer pour lui.

            uint8_t selectedRow = 0;

            do
            {
                ClientNotifyf(&lowerValuePlayer->clientRef, "NEED_ROW_CHOICE");
                ClientPrintf(&lowerValuePlayer->clientRef, "Veuillez choisir une colonne a recuperer [1-4] : ");
                ClientScanf(&lowerValuePlayer->clientRef, "%u", &selectedRow);
            } while (!(selectedRow >= 1 && selectedRow <= 4)); // Pour l'utilisateur, le choix est à faire entre 1 et 4.
            
            // Pour la machine, les colonnes sont numérotées de 0 à 3.
            selectedRow--;

            PlayerPickAllCardsFromRow(lowerValuePlayer, rows + selectedRow);
            PutCardOnRow(rows + selectedRow, lowerValuePlayer->chosenCard);
        }

        lowerValuePlayer->chosenCard.value = 0;
    }

    BROADCAST("\n");
}
