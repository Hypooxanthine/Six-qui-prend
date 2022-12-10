#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "Messages.h"
#include "Connect.h"
#include "Helper.h"
#include "Behaviour.h"
#include "Players.h"

int InteractWithServer(Socket server);
int GetBehaviours(int argc, char** argv);

static Card hand[MAX_CARDS_IN_HAND];
static Row rows[ROW_NUMBER];

static CardChoiceBehaviour cardBehaviour = CardChoiceBehaviour_MaxVal;
static RowChoiceBehaviour rowBehaviour = RowChoiceBehaviour_MinCowsSum;

typedef struct ArgBehaviour
{
    const char* arg;
    int behaviour;
} ArgBehaviour;

size_t playerId = 0;

int main(int argc, char** argv)
{
    srand(time(NULL));

    if(GetBehaviours(argc, argv) == EXIT_FAILURE) return EXIT_FAILURE;

    printf("Mode de choix des cartes en main : %d\n", cardBehaviour);
    printf("Mode de choix des colonnes : %d\n", rowBehaviour);

    char hostName[30];
    GetHostName(hostName);
    Socket serverSocket = ConnectToServer(hostName, GetPort());

    // Boucle principale.
    while(InteractWithServer(serverSocket) != ENDGAME);

    close(serverSocket);

    return 0;
}

int InteractWithServer(Socket server)
{
    Message rcv = { 0 };

    ReceiveMessage(server, &rcv);

    // L'IA ne s'intéresse qu'aux notifications
    if(rcv.action != MessageAction_Notify) return CONTINUE;

    printf("Notification du serveur : %s\n", rcv.content);

    if(StartsWith(rcv.content, "PLAYER_ID"))
    {
        sscanf(rcv.content, "PLAYER_ID=%lu", &playerId);
        return CONTINUE;
    }

    if(strcmp(rcv.content, "NEED_PLAYER_NAME") == 0)
    {
        Notifyf(server, "IA_%lu", playerId);
        return CONTINUE;
    }

    if(StartsWith(rcv.content, "ROWS="))
    {
        ParseRows(rows, ExtractVariable(rcv.content));
        return CONTINUE;
    }

    if(StartsWith(rcv.content, "HAND="))
    {
        ParseHand(hand, ExtractVariable(rcv.content));
        return CONTINUE;
    }

    if(strcmp(rcv.content, "NEED_CARD_CHOICE") == 0)
    {
        Notifyf(server, "%hhu", ChooseCard(rows, hand, cardBehaviour));
        return CONTINUE;
    }

    if(strcmp(rcv.content, "NEED_ROW_CHOICE") == 0)
    {
        Notifyf(server, "%hhu", ChooseRow(rows, hand, rowBehaviour));
        return CONTINUE;
    }

    if(strcmp(rcv.content, "ENDGAME") == 0)
    {
        return ENDGAME;
    }
}

int GetBehaviours(int argc, char** argv)
{
    // On fait un genre de map pour associer à un argument sa valeur dans l'énumération
    ArgBehaviour handArgBehaviours[] =
    {
        {"random", CardChoiceBehaviour_Random},
        {"maxval", CardChoiceBehaviour_MaxVal},
        {"minval", CardChoiceBehaviour_MinVal},
    };
    size_t handArgs = 3;

    ArgBehaviour rowArgBehaviours[] =
    {
        {"random", RowChoiceBehaviour_Random},
        {"mincowssum", RowChoiceBehaviour_MinCowsSum}
    };
    size_t rowArgs = 2;

    for(int i = 1 ; i < argc ; i++) // Pour chaque argument
    {
        if(strcmp(argv[i], "-h") == 0) // Pour l'option -h : stratégie de choix de carte dans la main
        {
            if(i + 1 >= argc) // Si le -h n'est suivi d'aucun argument
            {
                fprintf(stderr, "Merci de specifier un parametre pour l'option -h :\n"
                                "\t[random, maxval, minval]\n");
                return EXIT_FAILURE;
            }

            i++;

            int found = 0;
            for(size_t j = 0 ; j < handArgs && !found ; j++) // On regarde si l'option spécifiée existe
            {
                if (strcmp(argv[i], handArgBehaviours[j].arg) == 0)
                {
                    cardBehaviour = handArgBehaviours[j].behaviour;
                    found = 1;
                }
            }

            if(!found)
            {
                fprintf(stderr, "Parametre [%d: %s] inconnu.\n"
                                "Parametres possibles :\n"
                                "\t[random, maxval, minval]\n", i, argv[i]);
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(argv[i], "-r") == 0) // Pour l'option -r : stratégie de choix de la colonne de cartes
        {
            if(i + 1 >= argc) // Si le -r n'est suivi d'aucun argument
            {
                fprintf(stderr, "Merci de specifier un parametre pour l'option -r :\n"
                                "\t[random, mincowssum]\n");
                return EXIT_FAILURE;
            }

            i++;

            int found = 0;
            for(size_t j = 0 ; j < rowArgs && !found ; j++) // On regarde si l'option spécifiée existe
            {
                if (strcmp(argv[i], rowArgBehaviours[j].arg) == 0)
                {
                    rowBehaviour = rowArgBehaviours[j].behaviour;
                    found = 1;
                }
            }

            if(!found)
            {
                fprintf(stderr, "Parametre [%d: %s] inconnu.\n"
                                "Parametres possibles :\n"
                                "\t[random, mincowssum]\n", i, argv[i]);
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "Option [%d: %s] inconnu.\n"
                            "Options possibles :\n"
                            "- -h pour spécifier le comportement de choix des cartes dans la main :\n"
                            "\t[random, maxval]\n"
                            "- -r pour spécifier le comportement de choix des colonnes a recuperer\n"
                            "\t[random, mincowssum\n", i, argv[i]);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}