//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Client.h"
#include "db.h"
#include "Player.h"
#include "Options.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CurrEdit"
#pragma link "ToolEdit"
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfmClient *fmClient;

void ClientDraft()
{
  std::auto_ptr<TDraftPlayer> p(CreatePlayer(
    fmOptions->edName->Text.c_str(), "Local", fmOptions->cbCompLogs->Checked));
  fmClient = new TfmClient(Application);
  fmClient->Player = p.get();

  extern bool MaglibFmt;
  extern bool DeepAnalysis;

  if (fmClient->ShowModal() == mrOk)
  {
    TDraftPlayer::Archetyper->SaveDeck(
      fmClient->Player->Picked,
      fmClient->Player->Name+".dec",
      MaglibFmt,
      DeepAnalysis);
    //SaveInApprentice(fmClient->Player->Name+".dec", MaglibFmt);
  }
  delete fmClient;
}

//---------------------------------------------------------------------------
__fastcall TfmClient::TfmClient(TComponent* Owner)
        : TForm(Owner)
{
  StatusBar1->SimpleText = "Press Go to start";
}

//---------------------------------------------------------------------------
void __fastcall TfmClient::btGoClick(TObject *Sender)
{
  StatusBar1->SimpleText = "Connecting...";
  ClientSocket1->Host = edHost->Text;
  ClientSocket1->Port = edPort->Value;
  ClientSocket1->Open();
  btGo->Enabled = false;
  edHost->Enabled = false;
  edPort->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmClient::ClientSocket1Connect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  extern AnsiString PlayerName;
  btCancel->Enabled = false;
  Socket->SendText("name " + PlayerName + '\n');
  StatusBar1->SimpleText = "Connected";
}
//---------------------------------------------------------------------------

void __fastcall TfmClient::ClientSocket1Read(TObject *Sender,
      TCustomWinSocket *Socket)
{
  AnsiString ReceivedText = Socket->ReceiveText();
  Buffer += ReceivedText;

  //edLog->Lines->Add(ReceivedText + " - read");
  StatusBar1->SimpleText = "Communicating";

  AnsiString Text;
  while (Buffer.Pos('\n') > 0)
  {
    Text = Buffer.SubString(1, Buffer.Pos('\n')-1);
    Buffer.Delete(1, Buffer.Pos('\n'));

    if (Text == "booster")
    {
      StatusBar1->SimpleText = "Getting booster...";
      Booster.clear();
      continue;
    }
    if (Text == "endbooster")
    {
      //extern int GetUserPick(TCardPile& Booster, const TCardPile& Picked);
      //int i = GetUserPick(Booster, Picked);
      StatusBar1->SimpleText = "Waiting for pick";
      Player->Pick(Booster);
      extern bool BreakDraft;
      if (BreakDraft)
      {
        ModalResult = mrCancel;
      }
      StatusBar1->SimpleText = "Sending pick";
      const TCardData* c = Player->Picked.back();
      Socket->SendText((c->Name + '\n').c_str());

      //IntToStr(i) + '\n');
      continue;
    }
    if (Text == "enddraft")
    {
      StatusBar1->SimpleText = "All, folks!";
      ModalResult = mrOk;
      continue;
    }
    if (Text.SubString(1, 3) == "set")
    {
      CreateSet(Text.SubString(5, 3).c_str());
    }

    StatusBar1->SimpleText = AnsiString("Card ") + Text;
    TCardData* Card = GetCard(Text.c_str());
    Booster.push_back(Card);
  }
}
//---------------------------------------------------------------------------

void __fastcall TfmClient::ClientSocket1Disconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  if (ModalResult == mrNone)
  {
    btCancel->Enabled = true;
    StatusBar1->SimpleText = "Trying to reconnect...";
    ClientSocket1->Open();
  }
}
//---------------------------------------------------------------------------


void __fastcall TfmClient::ClientSocket1Error(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  if ((ErrorEvent == eeDisconnect) && (ModalResult == mrNone))
  {
    btCancel->Enabled = true;
    StatusBar1->SimpleText = "Trying to reconnect...";
    ErrorCode = 0;
    ClientSocket1->Open();
  }
}
//---------------------------------------------------------------------------

