//---------------------------------------------------------------------------
#ifndef ServerH
#define ServerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include "CurrEdit.hpp"
#include "Placemnt.hpp"
#include "ToolEdit.hpp"
#include <Mask.hpp>
#include <ComCtrls.hpp>
#include <vector>
//---------------------------------------------------------------------------

class TServerNetPlayer;

class TfmServer : public TForm
{
__published:	// IDE-managed Components
        TServerSocket *ServerSocket1;
        TButton *btGo;
        TListBox *lbPlayers;
        TCurrencyEdit *edPort;
        TLabel *Label1;
        TFormStorage *FormStorage1;
        TButton *btDraft;
        TStatusBar *StatusBar1;
        void __fastcall ServerSocket1ClientConnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ServerSocket1ClientDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ServerSocket1ClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
        
        void __fastcall btDraftClick(TObject *Sender);
        void __fastcall btGoClick(TObject *Sender);
private:	// User declarations
  struct TConnection
  {
    TCustomWinSocket* Socket;
    //AnsiString Name;
    AnsiString Buffer;
    TConnection(TCustomWinSocket* aSocket)
      : Socket(aSocket)
    {}
  };
  std::vector<TConnection> Connections;

  int FindConnection(TCustomWinSocket*) const;
  int FindPlayer(TCustomWinSocket*) const;

public:		// User declarations
  std::vector<TServerNetPlayer*> NetPlayers;
  bool WaitingForReconnect;

  __fastcall TfmServer(TComponent* Owner);
  void Start();
  void Stop();
};
//---------------------------------------------------------------------------
extern PACKAGE TfmServer *fmServer;

void ConnectDraftServer();

//---------------------------------------------------------------------------
#endif
