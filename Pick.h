//---------------------------------------------------------------------------
#ifndef PickH
#define PickH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "CardData.h"
#include <Buttons.hpp>
//#include "JpgImg.hpp"
#include "Placemnt.hpp"

class TfmPick : public TForm
{
__published:	// IDE-managed Components
    TStatusBar *StatusBar1;
    TTimer *Timer1;
    TPanel *paMain;
    TLabel *la54321;
    TButton *btBreak;
    TSpeedButton *btPause;
    TButton *btTake;
    TCheckBox *cbViewPicked;
    TPanel *paCard;
        TImage *imCard;
        TCheckBox *cbPicture;
        TMemo *edText;
        TFormStorage *FormStorage1;
        TPanel *paCardlist;
        TListBox *liBooster;
        TListBox *liValues;
        TSplitter *Splitter1;
    void __fastcall btTakeClick(TObject *Sender);
    
    void __fastcall liBoosterClick(TObject *Sender);
    void __fastcall liBoosterMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall btBreakClick(TObject *Sender);
    void __fastcall btPauseClick(TObject *Sender);
    
    void __fastcall cbPictureClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
private:	// User declarations
  const TCardPile& Booster;
  const TCardPile& Picked;
  int TimeLeft;
  bool StudyMode;
public:		// User declarations
  //int Choice;
  int GetChoice();
  void PaintCard();
  __fastcall TfmPick(TComponent* Owner, const TCardPile& aBooster,
    const TCardPile& aPicked, bool aStudyMode);
};

//---------------------------------------------------------------------------
extern PACKAGE TfmPick *fmPick;
extern bool BreakDraft;
//---------------------------------------------------------------------------

int GetUserPick(TCardPile& Booster, const TCardPile& Picked,
  bool StudyMode, std::vector<double> Values);
void ViewPile(const TCardPile& Picked);

#endif
