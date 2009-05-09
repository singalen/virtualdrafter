#include "Player.h"
#include "Pick.h"
//#include <math.h>
//#include "MyStrUtil.h"

#include "NetPlayer.h"
#include "Server.h"

//int TDraftPlayer::InstanceCount = 0;
//std::vector<TDraftArchetype*> TDraftPlayer::Archetypes;
//TCardValues TDraftPlayer::DefaultValues(MarksBase);
TArchetyper* TDraftPlayer::Archetyper = NULL;

void TDraftPlayer::UnitTest(const TCardData* c, TCardPile& Picked)
{
  double CardQuality, DeckQuality;
  Archetyper->PickValue(Picked, c, CardQuality, DeckQuality);

  char buf[256];
  sprintf(buf, "карта %s стоит %.2f", c->Name.c_str(), CardQuality);
  MessageDlg(//Format("карта %s на %d месте стоит %.2f",
    //ARRAYOFCONST((Picked[i]->Name.c_str(), i, MaxQuality)) ),
    buf,
    mtInformation, TMsgDlgButtons() << mbOK, 0);
}

class TLocalDraftPlayer : public TDraftPlayer
{
public:
  TLocalDraftPlayer(string aName, bool aLog) : TDraftPlayer(aName, aLog) {};

  virtual void Pick(TCardPile& Booster)
  {
    if (log != NULL)
    {
      fprintf(log, "\nBooster:\n");
      for (unsigned int i=0; i<Booster.size(); i++)
      {
        fprintf(log, "%s\n", Booster[i]->Name.c_str());
      }
    }                    

    double CardValue, DeckValue;
    std::vector<double> Values;
    extern bool StudyMode;
    if (StudyMode)
    {
      for (unsigned int i=0; i<Booster.size(); i++)
      {
        //string ArcheName =
        Archetyper->PickValue(Picked, Booster[i], CardValue, DeckValue);
        Values.push_back(CardValue);
      }
    }
    int i = GetUserPick(Booster, Picked, StudyMode, Values);
    if (log != NULL)
    {
      fprintf(log, "Picked: %s\n", Booster[i]->Name.c_str());
    }

    TCardData* c = *(Booster.begin()+i);
    Picked.push_back(c);
    Booster.erase(Booster.begin()+i);
  };
};

class TComputerFixedDraftPlayer : public TDraftPlayer
{
public:
  TComputerFixedDraftPlayer(string aName, bool aLog);
  virtual ~TComputerFixedDraftPlayer();

  virtual void Pick(TCardPile& Booster);
};

TDraftPlayer::TDraftPlayer(string aName, bool aLog)
  : Name(aName), Log(aLog), log(NULL)
{
  if (Archetyper == NULL)
  {
    Archetyper = new TArchetyper();
  }
  //InstanceCount++;

  if (Log)
  {
    extern string DecksPath;
    log = fopen((DecksPath + Name + "_pick.log").c_str(), "wt");
  }
  if (log != NULL)
  {
    fprintf(log, "Draft!\n");
  }
}

TDraftPlayer::~TDraftPlayer()
{
  //InstanceCount--;
  //if (InstanceCount == 0)
  //{
  //  FreeArchetypes();
  //}
  if (log != NULL) fclose(log);
}

TComputerFixedDraftPlayer::TComputerFixedDraftPlayer(string aName, bool aLog)
  : TDraftPlayer(aName, aLog)
{
}

void TComputerFixedDraftPlayer::Pick(TCardPile& Booster)
{
  if (log != NULL) fprintf(log, "\nBooster:\n");

  //double MaxQuality = 0;
  int PickIndex = 0;
  //int PickArchetype = 0;
  //double MaxOffcolorQuality = 0;
  //double MaxOffcolorIndex = 0;
  //double IncolorQuality = 0;
  //double MaxIncolorQuality = 0;
  string Archetype;
  double CardValue;
  double DeckValue;
  double MaxCardValue = 0;
  double MaxDeckValue = 0;

  for (unsigned int i=0; i<Booster.size(); i++)
  {

    string ArcheName = Archetyper->PickValue(Picked, Booster[i], CardValue, DeckValue);
    /*
    for (unsigned int j=0; j<Archetypes.size(); j++)
    {
      if ((Booster[i]->GetColor() != cColorless)
          && !Archetypes[j]->Belongs(Booster[i]))
      {
        string v;
        if (DefaultValues.GetValue(Booster[i]->Name, v))
        {
          double q = EvalCardValue(v, Picked);
          if (q > MaxOffcolorQuality)
          {
            MaxOffcolorQuality = q;
            MaxOffcolorIndex = i;
          }
        }
        continue;
      }

      double Quality = Archetypes[j]->PickValue(Picked, Booster[i], IncolorQuality);
      Quality += IncolorQuality*3;

      if (Quality > MaxQuality)
      {
        MaxQuality = Quality;
        MaxIncolorQuality = IncolorQuality;
        PickIndex = i;
        PickArchetype = j;
      }
    }
    */
    if (CardValue > MaxCardValue)
    {
      MaxCardValue = CardValue;
      MaxDeckValue = DeckValue;
      PickIndex = i;
      Archetype = ArcheName;
    }

    if (log != NULL)
    {
      fprintf(log, "%s %.2f\n", Booster[i]->Name.c_str(), CardValue);
    }
  }

  if (log != NULL)
  {
    //fprintf(log, "\nBooster:\n");
    //for (unsigned int i=0; i<Booster.size(); i++)
    //{
    //  fprintf(log, "%s\n", Booster[i]->Name.c_str());
    //}
    fprintf(log, "Archetype chosen: %s (%.2f)\nCard: %s, value: %.2f\n",
      Archetype.c_str(), MaxDeckValue,
      Booster[PickIndex]->Name.c_str(), MaxCardValue);

    //if ((MaxOffcolorQuality - MaxIncolorQuality) > 0.1999)
    //{
    //  PickIndex = MaxOffcolorIndex;
    //  fprintf(log, "Off-color card taken: %s, value:%.2f\n",
    //      Booster[PickIndex]->Name.c_str(),
    //      MaxOffcolorQuality);
    //}
  }

  TCardData* c = *(Booster.begin()+PickIndex);
  Picked.push_back(c);
  Booster.erase(Booster.begin()+PickIndex);
}

TComputerFixedDraftPlayer::~TComputerFixedDraftPlayer()
{
}

TDraftPlayer* CreatePlayer(string Name, string Type, bool Log)
{
  if (Type == "Local") return new TLocalDraftPlayer(Name, Log);

  if (fmServer != NULL)
  {
    for (unsigned int i=0; i<fmServer->NetPlayers.size(); i++)
    {
      if (!fmServer->NetPlayers[i]->UsedByFactory)
      {
        fmServer->NetPlayers[i]->UsedByFactory = true;
        return fmServer->NetPlayers[i];
      }
    }
  }

  return new TComputerFixedDraftPlayer(Name, Log);
}
//---------------------------------------------------------------------------

