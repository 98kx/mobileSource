//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_ParameterReadSetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tm_ParameterReadSetting *m_ParameterReadSetting;
//---------------------------------------------------------------------------
__fastcall Tm_ParameterReadSetting::Tm_ParameterReadSetting(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tm_ParameterReadSetting::BitBtn1Click(TObject *Sender)
{
	AnsiString	s;

	m_SaveFileDialog->Title = "Save file as...";
//	m_SaveFileDialog->InitialDir = MainForm->ROMBaseDir;

    if(!m_SaveFileDialog->Execute()) {
//        s.printf("Failed to save this file as \"%s\" !", m_SaveFileDialog->FileName.c_str());
//        ShowMessage(s);
        return;
    }

    m_file->Text = m_SaveFileDialog->FileName;
}
//---------------------------------------------------------------------------
void __fastcall Tm_ParameterReadSetting::m_OKClick(TObject *Sender)
{
    ParamReadbackConfig_T   *param;

    if( g_bParameterPage ){
        param = &g_ParamReadbackConfig;
    }
    else{
        param = &g_ReadbackPage_Param;
    }
    int    addr = strtoul(m_addr->Text.c_str(), NULL, 10);
    int    len  = strtoul(m_len->Text.c_str(), NULL, 10);
    AnsiString ss;

    if( PARAM_OTP == g_ParameterType ){
        if( (0 == m_addr->Text.Length()) || (0 > addr) ){
            ShowMessage("Error: Read Address is an invalid value!");
            return;
        }
    }

    if( 0 >= len ){
        ShowMessage("Error: Read Length is an invalid value!");
        return;
    }
    else if( 0 == m_file->Text.Length() ){
        ShowMessage("Error: Saved File Path is an invalid path!");
        return;
    }

    switch(g_ParameterType){
        case PARAM_OTP:
            param->m_otp_enable = (_BOOL) true;
            param->m_otp.m_addr = addr;
            param->m_otp.m_len  = len;
        	if( NULL != param->m_otp.m_data ) {
		        free(param->m_otp.m_data);
        	}
            param->m_otp.m_data = (unsigned char *)malloc(len);
            param->m_otp_filepath = m_file->Text;
            param->m_otp_used = (_BOOL) true;
            if( g_bParameterPage ){
                ss.printf("addr(0x%08X), len(%lu), %s", param->m_otp.m_addr, param->m_otp.m_len, param->m_otp_filepath.c_str());
                MainForm->ed_otp_Read->Text = ss;
            }
            else{
                MainForm->RedrawReadbackList();
            }
            break;
        case PARAM_SEC_RO:
            param->m_sec_ro_enable = (_BOOL) true;
            param->m_sec_ro.m_len = len;
        	if( NULL != param->m_sec_ro.m_data ) {
		        free(param->m_sec_ro.m_data);
        	}
            param->m_sec_ro.m_data = (unsigned char *)malloc(len);
            param->m_sec_ro_filepath = m_file->Text;
            param->m_sec_ro_used = (_BOOL) true;
            if( g_bParameterPage ){
                ss.printf("len(%lu): %s", param->m_sec_ro.m_len, param->m_sec_ro_filepath.c_str());
                MainForm->ed_sec_ro_Read->Text = ss;
            }
            else{
                MainForm->RedrawReadbackList();
            }
            break;
        case PARAM_CUST:
            param->m_cust_para_enable = (_BOOL) true;
            param->m_cust_para.m_len = len;
        	if( NULL != param->m_cust_para.m_data ) {
		        free(param->m_cust_para.m_data);
        	}
            param->m_cust_para.m_data = (unsigned char *)malloc(len);
            param->m_cust_para_filepath = m_file->Text;
            param->m_cust_para_used = (_BOOL) true;
            if( g_bParameterPage ){
                ss.printf("len(%lu): %s", param->m_cust_para.m_len, param->m_cust_para_filepath.c_str());
                MainForm->ed_cust_Read->Text = ss;
            }
            else{
                MainForm->RedrawReadbackList();
            }
            break;
        case PARAM_GPS:
            param->m_gps_enable = (_BOOL) true;
            param->m_gps_rom_filesize = len;
            param->m_gps_rom_filepath = m_file->Text;
            param->m_gps_used = (_BOOL) true;
            if( g_bParameterPage ){
                ss.printf("len(%lu): %s", param->m_gps_rom_filesize, param->m_gps_rom_filepath.c_str());
                MainForm->ed_gps_Read->Text = ss;
            }
            else{
                MainForm->RedrawReadbackList();
            }
            break;
        default:
            break;
    }


    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tm_ParameterReadSetting::m_CancelClick(TObject *Sender)
{
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tm_ParameterReadSetting::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Visible = false;
}
//---------------------------------------------------------------------------
