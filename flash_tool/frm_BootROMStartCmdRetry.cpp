//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_BootROMStartCmdRetry.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tfrm_BromStartCmdRetry *frm_BromStartCmdRetry;
//---------------------------------------------------------------------------
__fastcall Tfrm_BromStartCmdRetry::Tfrm_BromStartCmdRetry(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_BromStartCmdRetry::FormShow(TObject *Sender)
{
	m_RetryCount->Text = IntToStr(g_MaxBromStartCmdRetryCount);
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_BromStartCmdRetry::m_OK_ButtonClick(TObject *Sender)
{
	g_MaxBromStartCmdRetryCount	= StrToInt(m_RetryCount->Text);
    MainForm->m_BRomStartCmdRetryCount->Caption.printf("BootROM start cmd retry (%d) ...", g_MaxBromStartCmdRetryCount);
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tfrm_BromStartCmdRetry::m_Cancel_ButtonClick(
      TObject *Sender)
{
	Visible = false;	
}
//---------------------------------------------------------------------------
