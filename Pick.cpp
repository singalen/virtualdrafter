//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Pick.h"
#include "MyStrUtil.h"
#include "DrawMTGC.h"
#include "db.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//#pragma link "JpgImg"
#pragma link "Placemnt"
#pragma resource "*.dfm"


//TfmPick *fmPick;
bool ShowPictures = false;

bool BreakDraft;
//---------------------------------------------------------------------------
__fastcall TfmPick::TfmPick(TComponent* Owner,
  const TCardPile& aBooster, const TCardPile& aPicked, bool aStudyMode)
    : TForm(Owner), Booster(aBooster), Picked(aPicked), StudyMode(aStudyMode)
{
  TimeLeft = 40 - (7-Booster.size()/2)*5;
  if (TimeLeft < 5) TimeLeft = 5; 
}
//---------------------------------------------------------------------------

int GetUserPick(TCardPile& Booster, const TCardPile& Picked,
  bool StudyMode, std::vector<double> Values)
{
  TfmPick *fmPick = new TfmPick(Application, Booster, Picked, StudyMode);
  fmPick->FormStorage1->RestoreProperties();

  if (fmPick->cbViewPicked->Checked
      && (Picked.size() > 0)
      && (Picked.size() % 15 == 0))
  {
    fmPick->Timer1->Enabled = false;
    ViewPile(Picked);
    fmPick->Timer1->Enabled = true;
  }

  if (StudyMode)
  {
    fmPick->liValues->Visible = true;
  }

  for (unsigned int i=0; i<Booster.size(); i++)
  {
    fmPick->liBooster->Items->Add(Booster[i]->Name.c_str());
  }
  for (unsigned int i=0; i<Values.size(); i++)
  {
    fmPick->liValues->Items->Add(Values[i]);
  }
  //liBooster->Selected[random(liBooster->Items->Count)] = true;
  fmPick->ShowModal();
  int Result = fmPick->GetChoice();

  delete fmPick;
  return Result;
}

int TfmPick::GetChoice()
{
  for (int i=0; i<liBooster->Items->Count; i++)
  {
    if (liBooster->Selected[i])
    {
      return i;
    }
  }
  return random(liBooster->Items->Count);
}

void __fastcall TfmPick::btTakeClick(TObject *Sender)
{
  ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void TfmPick::PaintCard()
{
  const TCardData* c = Booster[GetChoice()];

  edText->Lines->Clear();
  edText->Lines->Add(c->Name.c_str());
  edText->Lines->Add(c->ManaCost.c_str());
  edText->Lines->Add(TCardData::Rarities[c->Rarity].c_str());
  if (c->SubTypes.empty())
  {
    edText->Lines->Add(c->Type.c_str());
  }
  else
  {
    edText->Lines->Add((c->Type + " - " + c->SubTypes).c_str());
    edText->Lines->Add((c->Power + "/" + c->Toughness).c_str());
  }

  string Text = c->Text;
  while (!Text.empty())
  {
    string Line = ExtractWord(Text, "\n");
    edText->Lines->Add(Trim(Line.c_str()));
  }

  if (cbPicture->Checked)
  {
    imCard->Invalidate();
    DrawCard(c->Name.c_str(), c->Edition.c_str(), "no", *GetAllCards(),
      imCard->Canvas);
  }
}
//---------------------------------------------------------------------------


void __fastcall TfmPick::liBoosterClick(TObject *Sender)
{
  PaintCard();    
}
//---------------------------------------------------------------------------

void __fastcall TfmPick::liBoosterMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  PaintCard();
}
//---------------------------------------------------------------------------


void __fastcall TfmPick::Timer1Timer(TObject *Sender)
{
  if (TimeLeft > 0) TimeLeft--;
  StatusBar1->Panels->Items[0]->Text = "Время: " + IntToStr(TimeLeft);
  if (TimeLeft <= 5)
  {
    la54321->Visible = true;
    la54321->Caption = IntToStr(TimeLeft);
  }
  if (!StudyMode && (TimeLeft == 0))
  {
    ModalResult = mrOk;
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmPick::btBreakClick(TObject *Sender)
{
  BreakDraft = true;
  ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfmPick::btPauseClick(TObject *Sender)
{
  btTake->Enabled = !btPause->Down;
  Timer1->Enabled = !btPause->Down;
  liBooster->Enabled = !btPause->Down;
}
//---------------------------------------------------------------------------

void __fastcall TfmPick::cbPictureClick(TObject *Sender)
{
  ShowPictures = cbPicture->Checked;
  imCard->Visible = ShowPictures;
  edText->Visible = !ShowPictures;
}
//---------------------------------------------------------------------------

void ViewPile(const TCardPile& Picked)
{                                                                 
  TfmPick *fmPick = new TfmPick(Application, Picked, Picked, false);
  fmPick->Caption = "Смотрим награбленное";
  fmPick->btTake->Caption = "Close";
  fmPick->btPause->Visible = false;
  fmPick->cbViewPicked->Visible = false;
  fmPick->Timer1->Enabled = false;
  fmPick->btBreak->Visible = false;

  for (unsigned int i=0; i<Picked.size(); i++)
  {
    fmPick->liBooster->Items->Add(Picked[i]->Name.c_str());
  }

  fmPick->ShowModal();
  delete fmPick;
}
//---------------------------------------------------------------------------

void __fastcall TfmPick::FormShow(TObject *Sender)
{
  cbPicture->Checked = ShowPictures;
  cbPictureClick(this);
}
//---------------------------------------------------------------------------


void __fastcall TfmPick::FormResize(TObject *Sender)
{
  //imCard->Canvas->ClipRect = imCard->ClientRect;         
}
//---------------------------------------------------------------------------
