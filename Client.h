//---------------------------------------------------------------------------
#ifndef ClientH
#define ClientH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CurrEdit.hpp"
#include "ToolEdit.hpp"
#include <Mask.hpp>
#include <ScktComp.hpp>
#include "Placemnt.hpp"
#include "CardData.h"
#include <ComCtrls.hpp>

class TDraftPlayer;

//---------------------------------------------------------------------------
class TfmClient : public TForm
{
__published:	// IDE-managed Components
        TClientSocket *ClientSocket1;
        TCurrencyEdit *edPort;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *edHost;
        TButton *btGo;
        TFormStorage *FormStorage1;
        TStatusBar *StatusBar1;
        TButton *btCancel;
        void __fastcall btGoClick(TObject *Sender);
        void __fastcall ClientSocket1Connect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ClientSocket1Read(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ClientSocket1Disconnect(TObject *Sender,
          TCustomWinSocket *Socket);
        
        void __fastcall ClientSocket1Error(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
private:	// User declarations
  AnsiString Buffer;
  TCardPile Booster;
  //TCardPile Picked;
public:		// User declarations
  TDraftPlayer* Player;
  __fastcall TfmClient(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmClient *fmClient;

void ClientDraft();

//---------------------------------------------------------------------------
#endif
