#include "CardData.h"

const string TCardData::Rarities[3] = {"Common", "Uncommon", "Rare" };


TCardData::TCardData(string aName,
  string aType,
  string aSubTypes,
  string aPowerToughness,
  string aManaCost,
  TCardData::TRarity aRarity,
  string aText,
  string aEdition)
: Name(aName), Type(aType), SubTypes(aSubTypes), //PowerToughness(aPowerToughness),
  //Power(0), Toughness(0),     
  ManaCost(aManaCost), Text(aText),
  Black(0), Blue(0), White(0), Green(0), Red(0),
  ConvertedManaCost(0), Rarity(aRarity), Edition(aEdition)
{
  unsigned int i;

  i = aPowerToughness.find('/');
  if (i != string::npos)
  {
    Power.assign(aPowerToughness, 0, i);
    Toughness.assign(aPowerToughness.c_str()+i+1);
  }

  aManaCost.append(1, ' ');
  for (i=0; i<aManaCost.size(); i++)
  {
    switch(aManaCost[i])
    {
    case 'U':
      Blue++;
      ConvertedManaCost++;
      break;
    case 'B':
      Black++;
      ConvertedManaCost++;
      break;
    case 'R':
      Red++;
      ConvertedManaCost++;
      break;
    case 'G':
      Green++;
      ConvertedManaCost++;
      break;
    case 'W':
      White++;
      ConvertedManaCost++;
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
      if (isdigit(aManaCost[i+1]))
      {
        ConvertedManaCost += (aManaCost[i]-'0')*10 + aManaCost[i+1] - '0';
        i++;
      }
      else
      {
        ConvertedManaCost += aManaCost[i] - '0';
      }
      break;
    case 'X':
    default:
      break;
    } // case 
  }
}

TCardColor TCardData::GetColor() const
{
  if (!Red && !Green && !White && !Blue && !Black) return cColorless;
  if (Red && !Green && !White && !Blue && !Black) return cRed;
  if (!Red && Green && !White && !Blue && !Black) return cGreen;
  if (!Red && !Green && White && !Blue && !Black) return cWhite;
  if (!Red && !Green && !White && Blue && !Black) return cBlue;
  if (!Red && !Green && !White && !Blue && Black) return cBlack;
  return cColorCount; // многоцветная
}


/*TCardData& TCardData::operator = (const TCardData& l)
{
  Black = l.Black;
  Blue = l.Blue;
  White = l.White;
  Green = l.Green;
  Red = l.Red;

  ConvertedManaCost = l.ConvertedManaCost;

  Name = l.Name;
  Type = l.Type;
  SubTypes = l.SubTypes;
  ManaCost = l.ManaCost;

  return *this;
}*/

TCardData* CardDataFactory(string Name)
{
  return NULL;
}

bool CompareCardValues(const TCardData* a, const TCardData* b)
{
  TCardPile pile;
  //return GetExpertMark(a, pile)
  //return a->Expert .compare(b->Name);
  return false;
}

bool CompareCards(const TCardData* a, const TCardData* b)
{
  return a->Name.compare(b->Name);
}

bool operator == (const TCardData* a, const string b)
{
  return a->Name == b;
}

TCardColor CharToColor(char c)
{
   switch(c)
   {
   case 'U':
     return cBlue;
   case 'B':
     return cBlack;
   case 'R':
     return cRed;
   case 'G':
     return cGreen;
   case 'W':
     return cWhite;
   default:
     return cColorless;
   }
}

bool TCardComparator::Is(const TCardData* c, const string Value) const
{
  return false;
}

bool TCardComparator::operator () (const TCardData* c, const string Value) const
{
  return Is(c, Value); 
}

bool CompareType::Is (const TCardData* c, const string Value) const
//bool (const TCardData* c, const string Value)
{
  return c->Type.find(Value) != string::npos;
}

/*bool TypeEquals::operator == (const TCardData* c)
{
  return c->Type.find(Name) != string::npos;
}*/


bool CompareSubType::Is (const TCardData* c, const string Value) const
{
  return c->SubTypes.find(Value) != string::npos;
}

bool CompareName::Is (const TCardData* c, const string Value) const
{
  return c->Name == Value;
}

bool IncludeText::Is (const TCardData* c, const string Value) const
{
  return c->Text.find(Value) != string::npos;
}

bool ComparePower::Is (const TCardData* c, const string Value) const
{
  return c->Power == Value;
}

bool CompareToughness::Is (const TCardData* c, const string Value) const
{
  return c->Toughness == Value;
}

bool CompareEdition::Is (const TCardData* c, const string Value) const
{
  return c->Edition == Value;
}

bool CompareCMC::Is (const TCardData* c, const string Value) const
{
  return c->GetCost() == atoi(Value.c_str());
}

TCardComparator& const ComparatorFactory(string Kind, bool& Success)
{
  static CompareType aCompareType;
  static CompareSubType aCompareSubType;
  static CompareName aCompareName;
  static IncludeText aIncludeText;
  static CompareEdition aCompareEdition;
  static ComparePower aComparePower;
  static CompareToughness aCompareToughness;
  static CompareCMC aCompareCMC;
  static TCardComparator dumb;
  Success = true;
  if (!stricmp(Kind.c_str(), "Type"))    return aCompareType;
  if (!stricmp(Kind.c_str(), "SubType")) return aCompareSubType;
  if (!stricmp(Kind.c_str(), "Name"))    return aCompareName;
  if (!stricmp(Kind.c_str(), "Text"))    return aIncludeText;
  if (!stricmp(Kind.c_str(), "Edition")) return aCompareEdition;
  if (!stricmp(Kind.c_str(), "Power"))   return aComparePower;
  if (!stricmp(Kind.c_str(), "Toughness")) return aCompareToughness;
  if (!stricmp(Kind.c_str(), "ConvertedManaCost")) return aCompareCMC;
  Success = false;
  return dumb;
}

void ComparatorRecycler(TCardComparator* c)
{
}
