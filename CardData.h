#ifndef __CardData_H__
#define __CardData_H__

#include <string>
#include <vector>

#define BoosterSize 15
#define BoosterCount 3
#define DeckSize 40
#define MinDeckLands 18

enum TCardColor
{
  cGreen, cRed, cBlack, cBlue, cWhite, cColorless, cColorCount
};

TCardColor CharToColor(char c);

class TCardData
{
public:
  enum TRarity { ryCommon, ryUncommon, ryRare };
private:
  int Black;
  int Blue;
  int White;
  int Green;
  int Red;

  int ConvertedManaCost;
public:
  TCardData(string aName,
    string aType,
    string aSubTypes,
    string aPowerToughness,
    string aManaCost,
    TRarity aRarity,
    string aText,
    string aEdition);

  const string Name;
  const string Type;
  const string SubTypes;
  const string ManaCost;
  const string Text;
  //const string PowerToughness;
  string Power;
  string Toughness;
  const string Edition;

  TRarity Rarity;
  static const string Rarities[3];

  int GetBlack() const { return Black; };
  int GetBlue()  const { return Blue; };
  int GetWhite() const { return White; };
  int GetGreen() const { return Green; };
  int GetRed()   const { return Red; };

  int GetCost() const { return ConvertedManaCost; };
  TCardColor GetColor() const;
};

typedef std::vector<TCardData*> TCardPile;
typedef std::vector<TCardData*>::iterator TCardBrowser;
typedef std::vector<TCardData*>::const_iterator TConstCardBrowser;

bool CompareCards(const TCardData* a, const TCardData* b);
bool operator == (const TCardData* a, const string b);

struct TCardComparator
{
  bool operator () (const TCardData* c, const string Value) const;
  typedef const TCardData* first_argument_type;
  typedef const string second_argument_type;
  typedef bool result_type;
private:
  // латаем дыры темплейтов CB
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct CompareType : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct CompareSubType : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct CompareName : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct IncludeText : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct ComparePower : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct CompareToughness : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct CompareEdition : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

struct CompareCMC : public TCardComparator
{
  virtual bool Is(const TCardData* c, const string Value) const;
};

TCardComparator& const ComparatorFactory(string Kind, bool& Success);
//void ComparatorRecycler(TCardComparator* c);

#endif
