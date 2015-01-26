//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_OTPSetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tm_OTPSetting *m_OTPSetting;
//---------------------------------------------------------------------------
__fastcall Tm_OTPSetting::Tm_OTPSetting(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::m_WriteClick(TObject *Sender)
{
    m_GroupOTPPara->Visible = true;
    m_OTPSetting->Height = 250;
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::m_WriteLockClick(TObject *Sender)
{
    m_GroupOTPPara->Visible = true;
    m_OTPSetting->Height = 250;
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::m_LockClick(TObject *Sender)
{
    m_GroupOTPPara->Visible = false;
    m_OTPSetting->Height = 130;
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::m_OKClick(TObject *Sender)
{
    ParamDownloadConfig_T   * param;

    if( g_bParameterPage ){
        param = &g_ParamDownloadConfig;
    }
    else{
    	if( NULL == g_pCurNode ) {
        	return;
        }
        param = &g_pCurNode->m_param;
    }

    param->m_otp_enable = (_BOOL) true;

    if( m_Write->Checked ){
        int addr = strtoul(m_otp_addr->Text.c_str(), NULL, 10);
        int len  = strtoul(m_otp_len->Text.c_str(), NULL, 10);

        if( (0 == m_otp_addr->Text.Length()) || (0 > addr) ){
            ShowMessage("Error: OTP Address is an invalid value!");
            return;
        }
        else if( 0 >= len){
            ShowMessage("Error: OTP Length is an invalid value!");
            return;
        }
        else if( NULL == param->m_otp.m_data ){
            ShowMessage("Error: OTP Data is an invalid value!");
            return;
        }

        param->m_otp_op = OTP_WRITE;
        param->m_otp.m_addr = addr;
        param->m_otp.m_len  = len;
    }
    else if( m_WriteLock->Checked ){
        int addr = strtoul(m_otp_addr->Text.c_str(), NULL, 10);
        int len  = strtoul(m_otp_len->Text.c_str(), NULL, 10);

        if( (0 == m_otp_addr->Text.Length()) || (0 > addr) ){
            ShowMessage("Error: OTP Address is an invalid value!");
            return;
        }
        else if( 0 >= len){
            ShowMessage("Error: OTP Length is an invalid value!");
            return;
        }
        else if( NULL == param->m_otp.m_data ){
            ShowMessage("Error: OTP Data is an invalid value!");
            return;
        }

        param->m_otp_op = OTP_WRITE_AND_LOCK;
        param->m_otp.m_addr = addr;
        param->m_otp.m_len  = len;   //Dec
    }
    else if( m_Lock->Checked ){
        param->m_otp_op = OTP_LOCK;
    }

    param->m_otp_filepath = m_otp_file->Text;
    param->m_otp_used = (_BOOL) true;

    if( g_bParameterPage ){
        AnsiString ss;
        ss.printf("addr(0x%08X), len(%lu), %s, %s", param->m_otp.m_addr, param->m_otp.m_len, OTPOpToString(param->m_otp_op), param->m_otp_filepath.c_str());
		MainForm->ed_otp_Write->Text = ss;
    }
    else{
    	MainForm->RedrawDownloadList(g_pCurNode, true);
    }

    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::m_CancelClick(TObject *Sender)
{
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tm_OTPSetting::BitBtn1Click(TObject *Sender)
{
	AnsiString	s;
    ParamDownloadConfig_T   * param;

    if( g_bParameterPage ){
        param = &g_ParamDownloadConfig;
    }
    else{
        if( NULL == g_pCurNode ) {
        	return;
    	}
        param = &g_pCurNode->m_param;
    }

	m_SelectOTPFile->Title = "Open OTP file ...";
//	m_SelectOTPFile->InitialDir = MainForm->ROMBaseDir;

    if(m_SelectOTPFile->Execute()) {
		if(MainForm->LoadFile(m_SelectOTPFile->FileName.c_str(), &param->m_otp.m_data, &param->m_otp.m_len, NULL)) {
			s.printf("Failed to load this OTP file \"%s\" !", m_SelectOTPFile->FileName.c_str());
			ShowMessage(s);
			return;
		}
	}

    m_otp_file->Text = m_SelectOTPFile->FileName;
    m_otp_len->Text  = AnsiString(param->m_otp.m_len);

}
//---------------------------------------------------------------------------

