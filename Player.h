#ifndef PlayerH
#define PlayerH

#include "CardData.h"
#include "CardValues.h"

class TDraftPlayer
{
public:
  TCardPile Picked;
  //string DeckName;
  static TArchetyper* Archetyper; 

protected:
  //double Signals[cColorCount];
  //int PickedCount[cColorCount];
  //double PickedQuality[cColorCount];
  static int InstanceCount;
  //static std::vector<TDraftArchetype*> Archetypes;
  //static TCardValues DefaultValues;
  //static void ReadArchetypes();
  //static void FreeArchetypes();
  FILE* log;

public:
  bool Log;
  TDraftPlayer(string aName, bool aLog);
  virtual ~TDraftPlayer();
  string Name;
  virtual void Pick(TCardPile& Booster) = 0;
  virtual const TCardPile& GetDeck() const { return Picked; }
  //virtual void SaveInApprentice(string Filename, bool MaglibFmt);
  static void UnitTest(const TCardData* c, TCardPile& Picked);
};

TDraftPlayer* CreatePlayer(string Name, string Type, bool Log);


// "сервер" драфта - локальный или удаленный.
class TDraft
{
public:
  TDraft();
};

#endif
