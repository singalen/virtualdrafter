//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "db.h"
#include "Pick.h"
#include "Player.h"
#include "Options.h"
#include "About.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmMain *fmMain;

//---------------------------------------------------------------------------
__fastcall TfmMain::TfmMain(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::FormCreate(TObject *Sender)
{
  fmOptions = new TfmOptions(this);
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::btLocalClick(TObject *Sender)
{
  for (;;)
  {
    fmOptions->SetOptions();
    Set1 = CreateSet(fmOptions->cbSet1->Text.c_str());
    Set2 = CreateSet(fmOptions->cbSet2->Text.c_str());
    Set3 = CreateSet(fmOptions->cbSet3->Text.c_str());
    if (Set1.size() * Set2.size() * Set3.size() > 0) break;

    //MessageDlg("Не могу прочесть сет.", mtError,
    //            TMsgDlgButtons() << mbOK, 0);
    if (fmOptions->ShowModal() != mrOk) Close();
  }

  LocalDraft();
}
//---------------------------------------------------------------------------

void TfmMain::LocalDraft()
{
  std::auto_ptr<TDraftPlayer> Players[8];
  BreakDraft = false;
  if (!cbDontPlay->Checked)
  {
    Players[0] = std::auto_ptr<TDraftPlayer> (CreatePlayer(
      fmOptions->edName->Text.c_str(), "Local", fmOptions->cbCompLogs->Checked));
  }
  else
  {
    Players[0] = std::auto_ptr<TDraftPlayer> (CreatePlayer(
      "Scrub0", "Computer", fmOptions->cbCompLogs->Checked));
  }

  for (int i=1; i<8; i++)
  {
    Players[i] = std::auto_ptr<TDraftPlayer> (CreatePlayer(
      string("Scrub") + IntToStr(i).c_str(), "Computer", fmOptions->cbCompLogs->Checked));
  }

  randomize();

  for (int i=0; i<10; i++)
  {
    int a = random(7)+1;
    int b = random(7)+1;
    if (a != b)
    {
      std::auto_ptr<TDraftPlayer> p = Players[a];
      Players[a] = Players[b];
      Players[b] = p;
    }
  }

  TCardPile Boosters[8][3];
  for (int i=0; i<8; i++) Boosters[i][0] = GenerateBooster(Set1);
  for (int i=0; i<8; i++) Boosters[i][1] = GenerateBooster(Set2);
  for (int i=0; i<8; i++) Boosters[i][2] = GenerateBooster(Set3);

  for (int i=0; i<BoosterSize; i++)
  {
    for (int j=0; j<8; j++)
    {
      if (BreakDraft) return;
      Players[j]->Pick(Boosters[(i+j)%8][0]);
    }
  }
  for (int i=0; i<BoosterSize; i++)
  {
    for (int j=0; j<8; j++)
    {
      if (BreakDraft) return;
      Players[j]->Pick(Boosters[(BoosterSize-i+j)%8][1]);
    }
  }
  for (int i=0; i<BoosterSize; i++)
  {
    for (int j=0; j<8; j++)
    {
      if (BreakDraft) return;
      Players[j]->Pick(Boosters[(i+j)%8][2]);
    }
  }

  extern bool MaglibFmt;
  extern bool DeepAnalysis;

  for (int i=0; i<8; i++)
  {
    //Players[i]->SaveInApprentice(Players[i]->Name+".dec", MaglibFmt);
    if (TDraftPlayer::Archetyper != NULL)
    {
      TDraftPlayer::Archetyper->SaveDeck(
        Players[i]->Picked,
        Players[i]->Name+".dec",
        MaglibFmt,
        DeepAnalysis);
    }
  }
}

void __fastcall TfmMain::btOptionsClick(TObject *Sender)
{
  fmOptions->ShowModal();        
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::btTestClick(TObject *Sender)
{
  TCardPile Picked;
  TCardPile Set1 = CreateSet(fmOptions->cbSet1->Text.c_str());
  TCardPile Set2 = CreateSet(fmOptions->cbSet2->Text.c_str());
  TCardPile Set3 = CreateSet(fmOptions->cbSet3->Text.c_str());

  //TDraftPlayer::UnitTest(GetCard("Wirewood Pride"), Picked);
  //TDraftPlayer::UnitTest(GetCard("Wirewood Herald"), Picked);
  Picked.push_back(GetCard("Nantuko Husk"));
  Picked.push_back(GetCard("Timberwatch Elf"));
  Picked.push_back(GetCard("Wirewood Herald"));
  Picked.push_back(GetCard("Wirewood Savage"));
  Picked.push_back(GetCard("Wirewood Savage"));
  Picked.push_back(GetCard("Wirewood Savage"));
  Picked.push_back(GetCard("Wirewood Savage"));
  Picked.push_back(GetCard("Wirewood Savage"));
  TDraftPlayer::UnitTest(GetCard("Wirewood Pride"), Picked);
  //TDraftPlayer::UnitTest(GetCard("Wirewood Herald"), Picked);
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::btNetworkClick(TObject *Sender)
{
  extern bool StudyMode;
  StudyMode = false;
  for (;;)
  {
    fmOptions->SetOptions();
    Set1 = CreateSet(fmOptions->cbSet1->Text.c_str());
    Set2 = CreateSet(fmOptions->cbSet2->Text.c_str());
    Set3 = CreateSet(fmOptions->cbSet3->Text.c_str());
    if (Set1.size() * Set2.size() * Set3.size() > 0) break;

    //MessageDlg("Не могу прочесть сет.", mtError,
    //            TMsgDlgButtons() << mbOK, 0);
    if (fmOptions->ShowModal() != mrOk) Close();
  }

  extern void ClientDraft();
  extern void ConnectDraftServer();
  if (rgNetwork->ItemIndex == 0)
  {
    ConnectDraftServer();
    LocalDraft();
  }
  else
  {
    ClientDraft();
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::spbAboutClick(TObject *Sender)
{
  if (AboutBox == NULL) AboutBox = new TAboutBox(this);
  AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

