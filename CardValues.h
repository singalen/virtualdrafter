#ifndef CardValuesH
#define CardValuesH

#include "CardData.h"
#include <string>
#include <map>
#include <set>

class TCardValues
{
  std::map<string, string> Values;
public:
  TCardValues();
  TCardValues(string FileName);
  bool GetValue(string Name, string& Value) const;
  void SetValue(string Name, string Value);
};

class TDraftArchetype
{
  TCardValues CardValues;
  const TCardValues& DefaultValues;
  string Name;
  std::set<TCardColor> Colors;

  static const double ExpectedPickQuality[BoosterSize];
public:
  TDraftArchetype(
    string aName,
    std::set<TCardColor> aColors,
    const TCardValues& aDefaultValues);

  TCardValues& GetCardValues() { return CardValues; };
  const string GetName() const { return Name; };
  const std::set<TCardColor> GetColors() const { return Colors; };

  double GetValue(const string Name, const TCardPile& Picked) const;
  bool ColorFits(TCardColor c) const;
  bool Belongs(const TCardData* Card) const;

  double PickValue(const TCardPile& Picked, const TCardData* Card, double& CardValue) const;
};

double EvalCardValue(string Formula, const TCardPile& Picked);

class TArchetyper
{
public:
  TArchetyper();
  // возвращает общий вес деки.
  string TArchetyper::PickValue(
    const TCardPile& Picked,
    const TCardData* Card,
    double& CardValue,
    double& DeckValue) const;

  void SaveDeck(
    TCardPile& Picked,
    string Filename,
    bool MaglibFmt,
    bool DeepAnalysis);

  const std::vector<TDraftArchetype*>& GetArchetypes() const { return Archetypes; };

private:
  std::vector<TDraftArchetype*> Archetypes;
  TCardValues DefaultValues;
  void ReadArchetypes();
  void FreeArchetypes();
};

#endif
