//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include <ComCtrls.hpp>

#include "CardData.h"
#include "CardValues.h"
#include <Buttons.hpp>

class TfmMain : public TForm
{
__published:	// IDE-managed Components
        TButton *btLocal;
        TButton *btOptions;
        TButton *btTest;
        TCheckBox *cbDontPlay;
        TButton *btNetwork;
        TRadioGroup *rgNetwork;
        TSpeedButton *spbAbout;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall btLocalClick(TObject *Sender);
        void __fastcall btOptionsClick(TObject *Sender);
        void __fastcall btTestClick(TObject *Sender);

        void __fastcall btNetworkClick(TObject *Sender);
        void __fastcall spbAboutClick(TObject *Sender);
private:	// User declarations
  TCardPile Set1;
  TCardPile Set2;
  TCardPile Set3;

public:		// User declarations
  __fastcall TfmMain(TComponent* Owner);
  void LocalDraft();
};
//---------------------------------------------------------------------------
extern PACKAGE TfmMain *fmMain;
//---------------------------------------------------------------------------
#endif
