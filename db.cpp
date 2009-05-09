#include <io.h>
#include <string>
#include <dialogs.hpp>
#include "MyStrUtil.h"
#include "db.h"

extern string MaglibPath;
string MaglibDatabase = "data/cardslist.dat";

TCardPile* db = NULL;

TCardPile* GetAllCards()
{
  if (db == NULL)
  {
    db = new TCardPile();
  }
  return db;
}

void FreeCards()
{
  if (db == NULL) return;

  for (TCardBrowser i = db->begin(); i != db->end(); i++)
  {
    delete (*i);
  }
  delete db;
  db = NULL;
}

string ExtractWordDelim(string& s, char Delimiter)
{
  if (s.empty()) return "";

  string Result;
  int i = s.find(Delimiter);
  if (i < 0)
  {
    Result = s;
    s.resize(0);
  }
  else
  {
    Result = s.substr(0, i);
    s.erase(0, i+1);
  }
  return Result;
}

TCardData* ParseMaglibLine(string Line, string SetName)
{
  string Name = ExtractWordDelim(Line, '\t');
  string Set = ExtractWordDelim(Line, '\t');
  if (!SetName.empty() && (Set != SetName)) return NULL;
  string RarityStr = ExtractWordDelim(Line, '\t') + " ";

  // коррекция апострофов
  int i;
  while ((i = Name.find(char(0x92))) != string::npos) Name[i] = 0x27;

  TCardData::TRarity Rarity;

  switch (RarityStr[0])
  {
  default:
  case 'C':
    Rarity = TCardData::ryCommon;
    break;
  case 'U':
    Rarity = TCardData::ryUncommon;
    break;
  case 'R':
    Rarity = TCardData::ryRare;
    break;
  }

  string Manacost = ExtractWordDelim(Line, '\t');

  string Type = ExtractWordDelim(Line, '\t');
  string SubType = "";
  i = Type.find("Creature");
  if (i != string::npos)//((Type == "Creature") || (Type == "Artifact Creature"))
  {
    SubType = Type.substr(i+8);
    Type.erase(i+8);
    //Type = "Creature";
  }
  i = Type.find("Summon");
  if (i != string::npos)
  {
    SubType = Type.substr(i+6);
    Type = "Creature";
  }

  if (!SubType.empty())
  {
    string Dash = ExtractWord(SubType, " ");
  }

  string CardText = ExtractWordDelim(Line, '\t');
  while ((i = CardText.find("#P")) >= 0)
  {
    CardText.replace(i, 2, "\n");
  }

  string Flavor = ExtractWordDelim(Line, '\t');
  string PowerToughness = ExtractWordDelim(Line, '\t');

  TCardData* Result = new TCardData(Name, Type, SubType, PowerToughness,
    Manacost, Rarity, CardText, Set);
  return Result;
}

TCardPile CreateSet(string SetName)
{
  TCardPile Result;

  TCardPile* p = GetAllCards();
  //if (!SetName.empty())
  // если сет уже вычитан
  for (TCardBrowser i = p->begin(); i != p->end(); i++)
  {
    if ((*i)->Edition == SetName) Result.push_back(*i);
  }
  if (Result.size() > 0) return Result;

  FILE* fp = fopen((MaglibPath + MaglibDatabase).c_str(), "rt");

  if (fp == NULL)
  {
    MessageDlg(AnsiString("File ") + MaglibPath.c_str() +
      MaglibDatabase.c_str() + " not found",
      mtError, TMsgDlgButtons() << mbOK, 0);
    return Result;
  }

  char buf[4096];
  while (!feof(fp))
  {
    fgets(buf, 4096, fp);
    TCardData* Card = ParseMaglibLine(buf, SetName);
    if (Card != NULL)
    {
      Result.push_back(Card);
      p->push_back(Card);
    }
  }

  if (Result.empty())
  {
    MessageDlg(AnsiString("Set ") + SetName.c_str() +
      " not found in database file",
      mtError, TMsgDlgButtons() << mbOK, 0);
  }

  fclose(fp);
  return Result;
}

TCardPile GenerateBooster(TCardPile& Set)
{
  TCardPile Result;

  //randomize();
  
  int i;
  int Iterations = 0;
  do
  {
    i = random(Set.size());
    Iterations++;
    if (Iterations > 1000) break;
  }
  while (Set[i]->Rarity != TCardData::ryRare);
  Result.push_back(Set[i]);
  Iterations = 0;

  int UncommonColors[7] = { 0,0,0,0,0 };
  for (int j=0; j<3; j++)
  {
    for(;;)
    {
      i = random(Set.size());
      Iterations++;
      if (Iterations > 4000) break;
      if (Set[i]->Rarity != TCardData::ryUncommon) continue;
      if (UncommonColors[Set[i]->GetColor()] > 0) continue;
      if (std::find(Result.begin(), Result.end(), Set[i]) != Result.end()) continue;
      break;
    }
    UncommonColors[Set[i]->GetColor()]++;
    Result.push_back(Set[i]);
  }
  Iterations = 0;

  int CommonColors[7] = { 0,0,0,0,0 };
  for (int j=0; j<11; j++)
  {
    for(;;)
    {
      i = random(Set.size());
      Iterations++;
      if (Iterations > 8000) break;
      if (IsBasicLand(Set[i])) continue;
      if (Set[i]->Rarity != TCardData::ryCommon) continue;
      if (CommonColors[Set[i]->GetColor()] >= 3) continue;
      if (std::find(Result.begin(), Result.end(), Set[i]) != Result.end()) continue;
      break;
    }
    CommonColors[Set[i]->GetColor()]++;
    Result.push_back(Set[i]);
  }

  return Result;
}

bool IsBasicLand(TCardData* c)
{
  return 
    (c->Name == "Island") ||
    (c->Name == "Forest") ||
    (c->Name == "Swamp") ||
    (c->Name == "Mountain") ||
    (c->Name == "Plains");
  // подкрутить для Snow-covereds
}

TCardData* GetCard(string Name)
{
  TCardPile* p = GetAllCards();
  for (TCardBrowser i = p->begin(); i != p->end(); i++)
  {
    if (!stricmp((*i)->Name.c_str(), Name.c_str())) return *i;
  }
  return NULL;
}
