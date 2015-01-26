//---------------------------------------------------------------------------

#ifndef frm_OTPSettingH
#define frm_OTPSettingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "main.h"

class Tm_OTPSetting : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *m_GroupOTPPara;
    TButton *m_OK;
    TButton *m_Cancel;
    TRadioGroup *m_GroupOTPOperation;
    TRadioButton *m_Lock;
    TRadioButton *m_Write;
    TRadioButton *m_WriteLock;
    TLabel *Label1;
    TEdit *m_otp_addr;
    TLabel *Label2;
    TEdit *m_otp_len;
    TBitBtn *BitBtn1;
    TOpenDialog *m_SelectOTPFile;
    TEdit *m_otp_file;
    void __fastcall m_WriteClick(TObject *Sender);
    void __fastcall m_WriteLockClick(TObject *Sender);
    void __fastcall m_LockClick(TObject *Sender);
    void __fastcall m_OKClick(TObject *Sender);
    void __fastcall m_CancelClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall Tm_OTPSetting(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tm_OTPSetting *m_OTPSetting;
//---------------------------------------------------------------------------
#endif
