//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RB_Go.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm_RB_GO *Form_RB_GO;
//---------------------------------------------------------------------------
__fastcall TForm_RB_GO::TForm_RB_GO(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm_RB_GO::bb_RB_GoClick(TObject *Sender)
{
   bRB_GO_Click = true;
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm_RB_GO::bb_RB_CancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm_RB_GO::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    MainForm->start_RB( bRB_GO_Click );
}
//---------------------------------------------------------------------------
void __fastcall TForm_RB_GO::FormShow(TObject *Sender)
{   // This is needed for click [X] system close button at right-up coner
    bRB_GO_Click = false;        
}
//---------------------------------------------------------------------------

