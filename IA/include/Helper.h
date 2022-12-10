#ifndef HELPER_H
#define HELPER_H

#include <stdarg.h>

#include "Types.h"
#include "Cards.h"

void Notify(Socket server, const char* content);
void Notifyf(Socket server, const char* format, ...);
void vNotifyf(Socket server, const char* format, va_list args);

// Vérification si str commence par start
int StartsWith(const char* str, const char* start);

// Exemple : "PLAYER_ID=1" retournera "1"
const char* ExtractVariable(const char* str);

void ParseHand(Card* hand, const char* str);
void ParseRows(Row* rows, const char* str);

// Permet de convertir une chaine en carte. Exemple de chaine : "34|5"
void ParseCard(Card* card, const char* str);
// Permet de convertir une chaine en plusieurs cartes. Exemple de chaine : "[34|1,97|1,46|1,103|1]"
int ParseCards(Card* cards, const char* str, size_t n);

#endif