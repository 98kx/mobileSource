//---------------------------------------------------------------------------

#ifndef frm_BootROMStartCmdRetryH
#define frm_BootROMStartCmdRetryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class Tfrm_BromStartCmdRetry : public TForm
{
__published:	// IDE-managed Components
	TButton *m_OK_Button;
	TButton *m_Cancel_Button;
	TLabeledEdit *m_RetryCount;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall m_OK_ButtonClick(TObject *Sender);
	void __fastcall m_Cancel_ButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall Tfrm_BromStartCmdRetry(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tfrm_BromStartCmdRetry *frm_BromStartCmdRetry;
//---------------------------------------------------------------------------
#endif
