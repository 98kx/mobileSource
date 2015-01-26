//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DL_Go.h"

#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm_DL_GO *Form_DL_GO;
//---------------------------------------------------------------------------
__fastcall TForm_DL_GO::TForm_DL_GO(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm_DL_GO::bb_DL_GoClick(TObject *Sender)
{
   bDL_GO_Click = true;
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm_DL_GO::bb_DL_CancelClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm_DL_GO::FormShow(TObject *Sender)
{   // This is needed for click [X] system close button at right-up coner
    bDL_GO_Click = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm_DL_GO::FormClose(TObject *Sender,
      TCloseAction &Action)
{
     MainForm->start_DL( bDL_GO_Click );
}
//---------------------------------------------------------------------------

