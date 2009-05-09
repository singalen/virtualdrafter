#ifndef DrawMTGCH
#define DrawMTGCH

#include "CardData.h"

void DrawCard(
  AnsiString Name,
  AnsiString Edition,
  AnsiString No,
  TCardPile& Cards,
  TCanvas* Target); // сюда и нарисуемся

#endif
