#ifndef _pick_dbH_
#define _pick_dbH_

#include <string>
#include "CardData.h"

TCardPile CreateSet(string SetName);

TCardPile GenerateBooster(TCardPile& Set);

TCardData* GetCard(string Name); 

bool IsBasicLand(TCardData* c);

TCardPile* GetAllCards();
void FreeAllCards(); 

#endif
