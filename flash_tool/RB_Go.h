//---------------------------------------------------------------------------

#ifndef RB_GoH
#define RB_GoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm_RB_GO : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TBitBtn *bb_RB_Go;
        TBitBtn *bb_RB_Cancel;
        void __fastcall bb_RB_GoClick(TObject *Sender);
        void __fastcall bb_RB_CancelClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
        bool bRB_GO_Click;
public:		// User declarations
        __fastcall TForm_RB_GO(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm_RB_GO *Form_RB_GO;
//---------------------------------------------------------------------------
#endif

