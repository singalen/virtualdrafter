//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Server.h"
#include "NetPlayer.h"
#include "Options.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CurrEdit"
#pragma link "Placemnt"
#pragma link "ToolEdit"
#pragma resource "*.dfm"
TfmServer *fmServer = NULL;
//---------------------------------------------------------------------------

void ConnectDraftServer()
{
  if (fmServer != NULL) delete fmServer;

  fmServer = new TfmServer(Application);
  fmServer->ShowModal();
}

__fastcall TfmServer::TfmServer(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TfmServer::Start()
{
}

void TfmServer::Stop()
{
}

int TfmServer::FindConnection(TCustomWinSocket* Socket) const
{
  for (std::vector<TConnection>::const_iterator i = Connections.begin();
       i != Connections.end(); i++)
  {
    if ((*i).Socket == Socket)
    {
      return i - Connections.begin();
    }
  }
  return -1;
}

int TfmServer::FindPlayer(TCustomWinSocket* Socket) const
{
  for (std::vector<TServerNetPlayer*>::const_iterator i = NetPlayers.begin();
       i != NetPlayers.end(); i++)
  {
    if ((*i)->GetSocket() == Socket)
    {
      return i - NetPlayers.begin();
    }
  }
  return -1;
}

void __fastcall TfmServer::ServerSocket1ClientConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  //if (NetPlayers.size() == 8) Go();
  Connections.push_back(TConnection(Socket));
  StatusBar1->SimpleText = Socket->RemoteHost + " connected...";
}
//---------------------------------------------------------------------------

void __fastcall TfmServer::ServerSocket1ClientDisconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  int i = FindPlayer(Socket);
  if (i >= 0) NetPlayers[i]->SetSocket(NULL);
  i = FindConnection(Socket);
  if (i >= 0) Connections.erase(Connections.begin()+i);
  StatusBar1->SimpleText = Socket->RemoteHost + " disconnected";
}
//---------------------------------------------------------------------------


void __fastcall TfmServer::ServerSocket1ClientRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
  StatusBar1->SimpleText = Socket->RemoteHost + "-incoming";

  AnsiString ReceivedText = Socket->ReceiveText();

  int Player = FindPlayer(Socket);
  int Connection = FindConnection(Socket);

  AnsiString Buffer;
  if (Player >= 0)
  {
    Buffer = NetPlayers[Player]->Buffer;
  }
  else if (Connection >= 0)
  {
    Buffer = Connections[Connection].Buffer;
  }
  else return; // херня какая-то.

  Buffer += ReceivedText;
  while (Buffer.Pos('\n') > 0)
  {
    Text = Buffer.SubString(1, Buffer.Pos('\n')-1);
    Buffer.Delete(1, Buffer.Pos('\n'));
    //mChat.Lines.Add('Server: ''' + Text + '''');
    //ServerTalk.ProcessServerResponce(Text);
    if (Player >= 0)
    {
      NetPlayers[Player]->OnReceiveText(Text);
      StatusBar1->SimpleText = AnsiString("Serving ") + Socket->RemoteHost; 
    }
    else if (Text.SubString(1, 4) == "name")
    {
      TServerNetPlayer* p =
        new TServerNetPlayer(Text.SubString(6, 16).c_str(), false, Socket);
      NetPlayers.push_back(p);
      Player = NetPlayers.size()-1;

      Socket->SendText(AnsiString("set ") + fmOptions->cbSet1->Text.c_str() + '\n');
      Socket->SendText(AnsiString("set ") + fmOptions->cbSet2->Text.c_str() + '\n');
      Socket->SendText(AnsiString("set ") + fmOptions->cbSet3->Text.c_str() + '\n');

      lbPlayers->Items->Add(p->Name.c_str());

      if (Connection >= 0)
      {
        Connections.erase(Connections.begin()+Connection);
        Connection = -1;
      }
    }

    if (Player >= 0)
      NetPlayers[Player]->Buffer = Buffer;
    else if (Connection >= 0)
      Connections[Connection].Buffer = Buffer;
  }
}
//---------------------------------------------------------------------------


void __fastcall TfmServer::btDraftClick(TObject *Sender)
{
  btGo->Enabled = false;
  ModalResult = mrOk;      
}
//---------------------------------------------------------------------------

void __fastcall TfmServer::btGoClick(TObject *Sender)
{
  ServerSocket1->Port = edPort->Value;
  ServerSocket1->Open();
  btGo->Enabled = false;
  edPort->Enabled = false;
  StatusBar1->SimpleText = "Waiting for connections...";
}
//---------------------------------------------------------------------------

