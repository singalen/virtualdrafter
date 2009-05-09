//---------------------------------------------------------------------------
#include <vcl.h>
#include <string>
#include <FileUtil.hpp>
#pragma hdrstop

#include "Options.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ToolEdit"
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfmOptions *fmOptions;

string MaglibPath;
string DecksPath;
string PicturesPath;

AnsiString PlayerName;

bool MaglibFmt = false;
bool DeepAnalysis = false;
bool StudyMode = false;

//int NetworkKind = 0;

//---------------------------------------------------------------------------
__fastcall TfmOptions::TfmOptions(TComponent* Owner)
        : TForm(Owner)
{
}

void TfmOptions::SetOptions()
{
  MaglibPath = NormalDir(edMaglibPath->Text).c_str();
  DecksPath = NormalDir(edDecksPath->Text).c_str();
  PicturesPath = NormalDir(edPicsPath->Text).c_str();

  MaglibPath = LongToShortPath(MaglibPath.c_str()).c_str();
  DecksPath = LongToShortPath(DecksPath.c_str()).c_str();
  PicturesPath = LongToShortPath(PicturesPath.c_str()).c_str();

  PlayerName = edName->Text;

  MaglibFmt = cbMaglibFmt->Checked;
  DeepAnalysis = cbDeepAnalysis->Checked;
  StudyMode = cbStudyMode->Checked;
  //NetworkKind = rgNetwork->ItemIndex;
}

//---------------------------------------------------------------------------

void __fastcall TfmOptions::Button1Click(TObject *Sender)
{
  SetOptions();        
}
//---------------------------------------------------------------------------

void __fastcall TfmOptions::FormCreate(TObject *Sender)
{
  FormStorage1->RestoreProperties();
}
//---------------------------------------------------------------------------



