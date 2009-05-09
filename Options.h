//---------------------------------------------------------------------------
#ifndef OptionsH
#define OptionsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "ToolEdit.hpp"
#include <Mask.hpp>
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
class TfmOptions : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *cbCompLogs;
        TLabel *Label1;
        TEdit *edName;
        TPanel *paButtons;
        TPanel *Panel2;
        TButton *Button1;
        TDirectoryEdit *edPicsPath;
        TDirectoryEdit *edMaglibPath;
        TDirectoryEdit *edDecksPath;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TFormStorage *FormStorage1;
        TComboBox *cbSet1;
        TLabel *Label5;
        TComboBox *cbSet2;
        TComboBox *cbSet3;
        TCheckBox *cbMaglibFmt;
        TButton *btCancel;
        TCheckBox *cbDeepAnalysis;
        TCheckBox *cbStudyMode;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        
        
private:	// User declarations
public:		// User declarations
  __fastcall TfmOptions(TComponent* Owner);
  void SetOptions();
};
//---------------------------------------------------------------------------
extern PACKAGE TfmOptions *fmOptions;
//---------------------------------------------------------------------------
#endif
