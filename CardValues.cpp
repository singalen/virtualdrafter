//#include "SysUtils.hpp"
#include "FR_Pars.hpp"
//#include "classes.hpp"
//#include <algorith>
#include <algorithm>
#include <function.h>

#include "CardValues.h"
#include "MyStrUtil.h"

class TMagicParser : public TfrParser
{
public:
  TMagicParser() : TfrParser() {};
  void __fastcall OnMagicFunction(
    const System::AnsiString Name,
    const System::Variant &p1,
    const System::Variant &p2,
    const System::Variant &p3,
    System::AnsiString &Val);
};

class TValueModofier
{
  TCardComparator& const Criteria;
  string Value;
  double ModifyBy;
public:
  TValueModofier(TCardComparator& const aCriteria, string aValue, double aModifyBy)
    : Criteria(aCriteria), Value(aValue), ModifyBy(aModifyBy)
  {};
  TValueModofier& operator = (const TValueModofier& src)
  {
    Criteria = src.Criteria;
    Value = src.Value;
    ModifyBy = src.ModifyBy;
    return *this;
  }
  double operator () (const TCardData* c) const
  {
    if (Criteria(c, Value))
    {
      return ModifyBy;
    }
    else
    {
      return 0;
    }
  }
};

template <class InputIterator, class Size>
void my_count_if (InputIterator first, InputIterator last, TCardComparator& pred,
  string Value, Size& n)
{
    while (first != last)
    if (pred(*first++, Value)) ++n;
}

// типа параметры такие
const TCardPile* aPicked = NULL;
std::vector<TValueModofier> Modifiers;

inline double CountBy(
  const TCardPile* Picked,
  AnsiString What,
  string Value, string Weight)
{
  int Result = 0;
  double nWeight = atof(Weight.c_str());
  bool Success = false;
  TCardComparator& const CardComparator = ComparatorFactory(What.c_str(), Success);
  if (Success)
  {
    //std::binder2nd<TCardComparator> b(CardComparator, Value);
    my_count_if(Picked->begin(), Picked->end(), CardComparator, Value, Result);

    TValueModofier a(CardComparator, Value, nWeight);
    Modifiers.push_back(a);

    return Result * nWeight;
  }

  if (!What.AnsiCompareIC("ColorCost"))
  {
    for (TConstCardBrowser i = aPicked->begin(); i != aPicked->end(); i++)
    {
      const string& s = (*i)->ManaCost;
      for (unsigned int j=0; j<s.size(); j++) Result += (s[j] == Value[0]);
      //Result += std::count((*i)->ManaCost.begin(), (*i)->ManaCost.end(), Value[0]);
    }
  }

  return Result*nWeight;
}

AnsiString ToParam(const Variant& v)
{
  AnsiString s = v;
  if (s.Length() < 2) return s;
  if ((s[1] == s[s.Length()]) && (strchr("\"'`", s[1]) != NULL))
  {
    s = s.SubString(2, s.Length()-2);
  }
  return s;
}

void __fastcall TMagicParser::OnMagicFunction(
  const System::AnsiString Name,
  const System::Variant &p1,
  const System::Variant &p2,
  const System::Variant &p3,
  System::AnsiString &Val)
{
  double Result = 0;
  if (!Name.AnsiCompareIC("CountBy") && (aPicked != NULL))
  {
    Result = CountBy(aPicked, ToParam(p1), ToParam(p2).c_str(), ToParam(p3).c_str());
  }
  Val = Result;
}

TMagicParser* frParser = NULL;

double EvalCardValue(string Formula, const TCardPile& Picked)
{
  if (frParser == NULL)
  {
    frParser = new TMagicParser();
    frParser->OnFunction = frParser->OnMagicFunction;
    //frParser->OnFunction = OnFunction;
  }

  aPicked = &Picked;
  double Result = frParser->Calc(Formula.c_str());
  aPicked = NULL;
  return Result;
}

TCardValues::TCardValues()
{
}

TCardValues::TCardValues(string FileName)
{
  FILE* fp = fopen(FileName.c_str(), "rt");
  if (fp == NULL) return;
  while (!feof(fp))
  {
    char buf[256];
    fgets(buf, 4096, fp);
    string Line = buf;
    if (Line.empty()) continue;
    if (Line[0] == ';') continue;

    int i = Line.find_last_of(' ');
    if (i < 0) continue;
    string Name = Line.substr(0, i);
    Line.erase(0, i+1);
    string Value = Line;
    Value.resize(Value.size()-1);

    // коррекция апострофов
    //int i;
    while ((i = Name.find(char(0x92))) != string::npos) Name[i] = 0x27;
    
    Values[Name] = Value;
  }
  fclose(fp);
}

bool TCardValues::GetValue(string Name, string& Value) const 
{
  std::map<string, string>::const_iterator i = Values.find(Name);
  if (i != Values.end())
  {
    Value = (*i).second;
    return true;
  }
  return false;
}

void TCardValues::SetValue(string Name, string Value)
{
  Values[Name] = Value;
}

const double TDraftArchetype::ExpectedPickQuality[BoosterSize] =
  { 0.9, 0.8, 0.8, 0.6, 0.6, 0.5, 0.3, 0.3, 0.2, 0.1, 0.1, 0.0, 0.0, 0.0, 0.0 };
//  { 0.8, 0.7, 0.6, 0.4, 0.4, 0.4, 0.3, 0.2, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

TDraftArchetype::TDraftArchetype(
  string aName,
  std::set<TCardColor> aColors,
  const TCardValues& aDefaultValues)
  : Name(aName), Colors(aColors), DefaultValues(aDefaultValues)
{
}

double TDraftArchetype::GetValue(
  const string Name, const TCardPile& Picked) const
{
  string Value;
  if (!CardValues.GetValue(Name, Value))
  {
    if (!DefaultValues.GetValue(Name, Value)) return 0;
  }
  double Result = EvalCardValue(Value, Picked);
  //if (Result < 0.3) Result = 0;
  return Result;
}

bool TDraftArchetype::ColorFits(TCardColor c) const
{
  return ((c == cColorless) || Colors.find(c) != Colors.end());
}

bool TDraftArchetype::Belongs(const TCardData* Card) const
{
  string Value;
  return ColorFits(Card->GetColor()); //|| CardValues.GetValue(Name, Value);
}

// возвращает: суммарное значение архетипа
// CardValue - значение собственно карты
double TDraftArchetype::PickValue(
  const TCardPile& Picked,
  const TCardData* Card,
  double& CardValue) const
{
  TCardPile ArchetypeCards;
  TCardPile Counted;
  //double MinArchetypeValue = 2.1;
  double Sum = 0;

  // модификаторы оценок внутри архетипа
  Modifiers.clear();

  for (unsigned int i=0; i<Picked.size(); i++)
  {
    double PickValue = 0;
    for (unsigned int j=0; j<Modifiers.size(); j++)
    {
      PickValue += Modifiers[j](Picked[i]);
    }
    PickValue += GetValue(Picked[i]->Name, Counted);

    Counted.push_back(Picked[i]);

    if (PickValue < 0.3) continue;

    if (Belongs(Picked[i]))
    {
      ArchetypeCards.push_back(Picked[i]);
      //MinArchetypeValue = min(MinArchetypeValue, PickValue);
      Sum += GetValue(Picked[i]->Name, ArchetypeCards);
    }
  }
  //if (MinArchetypeValue < 0.3) MinArchetypeValue = 0.3;

  if (Card != NULL)
  {
    CardValue = 0;
    for (unsigned int j=0; j<Modifiers.size(); j++)
    {
      CardValue += Modifiers[j](Card);
    }
    CardValue += GetValue(Card->Name, ArchetypeCards);
    Sum += /*1.5**/CardValue;
  }

  /*for (int i=Picked.size(); i<BoosterSize*BoosterCount; i++)
  {
    int PickNo = i % BoosterSize;
    if (ExpectedPickQuality[PickNo] >= 0.3)
    {
      Sum += ExpectedPickQuality[PickNo];
    }
  }*/

  return Sum;
}

const string MarksBase = "data/values.dat";
const string ArchetypesBase = "data/decks.dat";

TArchetyper::TArchetyper()
  : DefaultValues(MarksBase)
{
  ReadArchetypes();
}

string TArchetyper::PickValue(
  const TCardPile& Picked,
  const TCardData* Card,
  double& CardValue,
  double& DeckValue
  ) const
{
  string ArcheName;

  CardValue = 0;
  DeckValue = 0;
  for (unsigned int j=0; j<Archetypes.size(); j++)
  {
    double IncolorQuality;
    //double Quality =
    double CurrDeckValue = Archetypes[j]->PickValue(Picked, Card, IncolorQuality);

    if (IncolorQuality > CardValue)
    {
      ArcheName = Archetypes[j]->GetName();
      CardValue = IncolorQuality;
      DeckValue = CurrDeckValue;
    }
    //if (CurrDeckValue > DeckValue)
    //{
    //}
  }

  string v;
  if (DefaultValues.GetValue(Card->Name, v))
  {
    double q = EvalCardValue(v, Picked);
    if (q > CardValue)
    {
      CardValue = q;
      ArcheName = "offcolor";
      //MaxOffcolorIndex = i;
    }
  }

  return ArcheName;
}

class ArchetypedValueComparator
{
  const TCardPile Picked;
  const TDraftArchetype* Archetype;
public:
  ArchetypedValueComparator(const TDraftArchetype* aArchetype, const TCardPile& aPicked)
    : Archetype(aArchetype), Picked(aPicked) {};
  bool operator () (TCardData* a, TCardData* b)
  {
    double QualityA;
    double QualityB;
    Archetype->PickValue(Picked, a, QualityA);
    if (!Archetype->Belongs(a)) QualityA = 0;
    Archetype->PickValue(Picked, b, QualityB);
    if (!Archetype->Belongs(b)) QualityB = 0;

    if (QualityA == QualityB) return a->Name >= b->Name;
    return QualityA >= QualityB;
  };
};

void TArchetyper::SaveDeck(
  TCardPile& Picked,
  string Filename,
  bool MaglibFmt,
  bool DeepAnalysis)
{
  double MaxQuality = 0;
  int ArchetypeIndex = 0;
  for (unsigned int j=0; j<Archetypes.size(); j++)
  {
    double Quality;
    double IncolorQuality;
    Quality = Archetypes[j]->PickValue(Picked, NULL, IncolorQuality);

    if (Quality > MaxQuality)
    {
      MaxQuality = Quality;
      ArchetypeIndex = j;
    }
  }

  TCardPile Deck = Picked;
  TCardPile NextDeck;

  std::vector<bool> SideboardFlag;
  std::vector<double> Qualities;
  //SideboardFlag.insert(SideboardFlag.end(), Picked.size()-SideboardFlag.size()+1, false);
  int Nonlands = 0;
  int Lands = 0;
  int DeckColors[7] = {0,0,0,0,0,0,0};

  for (int BubblePass = 0; BubblePass<2; BubblePass++)
  {
    MaxQuality = 0;
    Nonlands = 0;
    Lands = 0;
    memset(DeckColors, 0, sizeof(DeckColors));

    ArchetypedValueComparator cmp(Archetypes[ArchetypeIndex], Deck);
    std::stable_sort(Picked.begin(), Picked.end(), cmp);
    SideboardFlag.clear();

    for (unsigned int i=0; i<Picked.size(); i++)
    {
      SideboardFlag.push_back(false);
      if (DeepAnalysis)
      {
        double Quality = 0;
        Archetypes[ArchetypeIndex]->PickValue(Deck, Picked[i], Quality);
        Qualities.push_back(Quality);
      }
      if (Picked[i]->Type != "Land")
      {
        Nonlands++;
        SideboardFlag[i] =
          !Archetypes[ArchetypeIndex]->Belongs(Picked[i])
          || (Nonlands > DeckSize-MinDeckLands);
        if (!SideboardFlag[i]) DeckColors[Picked[i]->GetColor()]++;
      }
      else
      {
        // здесь будет искажение из-за того, что мелкие карты еще не включены.
        SideboardFlag[i] = Archetypes[ArchetypeIndex]->GetValue(Picked[i]->Name, Deck) < 0.3;
        if (!SideboardFlag[i]) Lands++;
      }

      if (!SideboardFlag[i])
      {
        NextDeck.push_back(Picked[i]);
        MaxQuality += Archetypes[ArchetypeIndex]->GetValue(Picked[i]->Name, Deck);
      }
    }

    Deck = NextDeck;
  }

  int BasicLandCount = 0;
  for (int i=0; i<5; i++)
  {
    if (DeckColors[i] == 0) continue;
    DeckColors[i] = DeckColors[i]*MinDeckLands/Nonlands;
    // сглаживаем ману
    if (DeckColors[i] < (MinDeckLands-Lands-1)/2)
      DeckColors[i] += ((MinDeckLands-Lands)/2 - DeckColors[i])/2;
    if (DeckColors[i] > (MinDeckLands-Lands+1)/2)
      DeckColors[i] += ((MinDeckLands-Lands)/2 - DeckColors[i])/2;
    BasicLandCount += DeckColors[i];
  }

  while (BasicLandCount + Lands - MinDeckLands != 0)
  {
    int LandDiff = MinDeckLands - (BasicLandCount+Lands);
    LandDiff /= abs(LandDiff);
    for (int i=0; i<5; i++)
    {
      if (DeckColors[i] == 0) continue;
      DeckColors[i] += LandDiff;
      BasicLandCount += LandDiff;
      if (BasicLandCount + Lands - MinDeckLands == 0) break;
    }
  }

  extern string DecksPath;
  AnsiString Decks = DecksPath.c_str();
  FILE* fp = fopen((DecksPath + Filename).c_str(), "wt");
  /*AnsiString FileN = ExtractFileName(Filename.c_str());
  AnsiString FileExt = ExtractFileExt(Filename.c_str());
  FileN.SetLength(FileN.Length() - FileExt.Length());
  int i = 0;
  while ((fp == NULL) && FileExists(Decks + FileN + FileExt))
  {
    FileN = ExtractFileName(Filename.c_str()) + IntToStr(i);
    fp = fopen((Decks + FileN).c_str(), "wt");
    i++;
  }*/
  if (fp == NULL) return;

  string DeckName = Archetypes[ArchetypeIndex]->GetName();
  fprintf(fp, "//NAME: %s\n", DeckName.c_str());
  fprintf(fp, "//COM:^ assembled in VirtualDrafter(http://mtgclub.dp.ua)\n");
  fprintf(fp, "// Deck quality: %.2f\n", MaxQuality);

  static const string BasicLands[5] = { "Forest", "Mountain", "Swamp", "Island", "Plains" };
  for (int i=0; i<5; i++)
  {
    if (DeckColors[i] > 0)
    {
      if (MaglibFmt)
        fprintf(fp, "    %u [4E] - %s\n", DeckColors[i], BasicLands[i].c_str());
      else
        fprintf(fp, "    %u  %s\n", DeckColors[i], BasicLands[i].c_str());
    }
  }

  for (TConstCardBrowser i = Picked.begin(); i != Picked.end(); i++)
  {
    int Count = 1;
    int index = i-Picked.begin();
    while ((i+1 != Picked.end()) && ((*i)->Name == (*(i+1))->Name)
           && (SideboardFlag[index] == SideboardFlag[index+1]))
    {
      Count++;
      i++;
    }
    string SideString = SideboardFlag[index] ? "SB: " : "    ";

    if (MaglibFmt)
      fprintf(fp, "%s%u [%s] - %s\n",
        SideString.c_str(), Count, (*i)->Edition.c_str(), (*i)->Name.c_str());
    else
      fprintf(fp, "%s%u  %s\n", SideString.c_str(), Count, (*i)->Name.c_str());

    if (DeepAnalysis)
    {
      fprintf(fp, "// %.2f\n", Qualities[i - Picked.begin()]);
    }
  }
  fclose(fp);
}

void TArchetyper::ReadArchetypes()
{
  FILE* fp = fopen(ArchetypesBase.c_str(), "rt");
  if (fp == NULL) return;

  TDraftArchetype* at = NULL;
  FreeArchetypes();

  while (!feof(fp))
  {
    char buf[256];
    fgets(buf, 4096, fp);
    string Line = buf;
    if (Line.empty()) continue;
    if (Line[0] == ';') continue;

    string Word = ExtractWord(Line, " \n");
    if (Word == "Format") continue; // заглушка
    if (Word == "Deck")
    {
      string DeckName = ExtractWord(Line, " \n");
      string DeckColors = ExtractWord(Line, " \n");
      std::set<TCardColor> Colors;

      for (unsigned int i=0; i<DeckColors.size(); i++)
      {
        TCardColor cc = CharToColor(DeckColors[i]);
        if (cc != cColorless)
        {
          Colors.insert(cc);
        }
      }

      at = new TDraftArchetype(DeckName, Colors, DefaultValues);
      Archetypes.push_back(at);
      continue;
    }

    if (Word == "Deckend")
    {
      at = NULL;
      continue;
    }

    if (/*!Line.empty() && */(at != NULL)) // читаем деку
    {
      Line = Word + " " + Line;
      int i = Line.find_last_of(' ');
      if (i < 0) continue;
      string Name = Line.substr(0, i);
      Line.erase(0, i+1);
      string Value = Line;
      if (!Value.empty()) Value.resize(Value.size()-1);

      at->GetCardValues().SetValue(Name, Value);
      continue;
    }
  }
  fclose(fp);
}

void TArchetyper::FreeArchetypes()
{
  for (unsigned int i=0; i<Archetypes.size(); i++)
  {
    delete Archetypes[i];
  }
  Archetypes.clear();
}

