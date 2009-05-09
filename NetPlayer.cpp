#include "NetPlayer.h"
#include "Pick.h"

// VCL headers
#include <ScktComp.hpp>
#include <time.h>

/*TClientNetPlayer::TClientNetPlayer(string aName, bool aLog)
  : TDraftPlayer(aName, aLog)
{
}

void TClientNetPlayer::Pick(TCardPile& Booster)
{
  if (log != NULL)
  {
    fprintf(log, "\nBooster:\n");
    for (unsigned int i=0; i<Booster.size(); i++)
    {
      fprintf(log, "%s\n", Booster[i]->Name.c_str());
    }
  }

  int i = GetUserPick(Booster, Picked);
  if (log != NULL)
  {
    fprintf(log, "Picked: %s\n", Booster[i]->Name.c_str());
  }

  TCardData* c = *(Booster.begin()+i);
  Picked.push_back(c);
  Booster.erase(Booster.begin()+i);
}*/

TServerNetPlayer::TServerNetPlayer(
  string aName,
  bool aLog,
  Scktcomp::TCustomWinSocket *aSocket)
  : TDraftPlayer(aName, aLog), Socket(aSocket), UsedByFactory(false)
{
}

TServerNetPlayer::~TServerNetPlayer()
{
  if (Socket != NULL)
  {
    Socket->SendText("enddraft\n");
    Socket->Close();
  }
}

void TServerNetPlayer::SetSocket(Scktcomp::TCustomWinSocket* s)
{
  Socket = s;
}

void TServerNetPlayer::OnReceiveText(AnsiString Text)
{
  string Name = Text.c_str();
  std::binder2nd<CompareName> b(CompareName(), Name);
  TCardBrowser i = std::find_if(Booster.begin(), Booster.end(), b);
  if (i != Booster.end())
  {
    PickIndex = i - Booster.begin();
  }
  else
  {
    PickIndex = 0; // ???
  }
  //StrToInt(Text);
  PickReceived = true;
}

void TServerNetPlayer::Pick(TCardPile& aBooster)
{
  Booster = aBooster; 
  extern bool BreakDraft;
  if (BreakDraft) return;

reconnect:
  if (Socket == NULL) // disconnected
  {
    time_t Stamp = time(NULL);
    while (time(NULL) - Stamp < TIMEOUT)
    {
      Application->ProcessMessages();
      if (BreakDraft) return;
      if (Socket != NULL) break;
    }
    if (Socket == NULL) throw Exception("User not reconnected");
  }

  PickReceived = false;
  Socket->SendText("booster\n");
  for (unsigned int i=0; i<Booster.size(); i++)
  {
    Socket->SendText((Booster[i]->Name + "\n").c_str());
  }
  Socket->SendText("endbooster\n");
  while (!PickReceived)
  {
    Application->ProcessMessages();
    if (BreakDraft) return;
    if (Socket == NULL) goto reconnect;
  }

  TCardData* c = *(aBooster.begin()+PickIndex);
  Picked.push_back(c);
  aBooster.erase(aBooster.begin()+PickIndex);
}


