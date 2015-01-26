//---------------------------------------------------------------------------

#ifndef DL_GoH
#define DL_GoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm_DL_GO : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *bb_DL_Go;
        TLabel *Label1;
        TBitBtn *bb_DL_Cancel;
        void __fastcall bb_DL_GoClick(TObject *Sender);
        void __fastcall bb_DL_CancelClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
        bool bDL_GO_Click;
public:		// User declarations
        __fastcall TForm_DL_GO(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm_DL_GO *Form_DL_GO;
//---------------------------------------------------------------------------
#endif

