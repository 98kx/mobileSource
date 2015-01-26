//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_SecuritySetting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tm_SecuritySetting *m_SecuritySetting;
//---------------------------------------------------------------------------
__fastcall Tm_SecuritySetting::Tm_SecuritySetting(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tm_SecuritySetting::Display(void)
{
    ParamDownloadConfig_T   *param;

    if( g_bParameterPage ){
        param = &g_ParamDownloadConfig;
    }
    else{
    	if( NULL == g_pCurNode ) {
        	return;
        }
        param = &g_pCurNode->m_param;
    }

 	m_LockDown->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_lockdown?true:false;
 	m_CODE_DL_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_download?true:false;
 	m_CODE_RB_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_readback?true:false;
 	m_CODE_FMT_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_format?true:false;
 	m_SECRO_DL_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_download?true:false;
 	m_SECRO_RB_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_readback?true:false;
 	m_FAT_DL_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_download?true:false;
 	m_FAT_RB_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_readback?true:false;
 	m_FAT_FMT_Disabled->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_format?true:false;
 	m_RestrictedAreaForbidden->Checked = param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_restricted_area_access_forbidden;
}
//---------------------------------------------------------------------------
void __fastcall Tm_SecuritySetting::m_OKClick(TObject *Sender)
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

 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_lockdown = m_LockDown->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_download = m_CODE_DL_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_readback = m_CODE_RB_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_format = m_CODE_FMT_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_download = m_SECRO_DL_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_readback = m_SECRO_RB_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_download = m_FAT_DL_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_readback = m_FAT_RB_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_format = m_FAT_FMT_Disabled->Checked?_TRUE:_FALSE;
 	param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_restricted_area_access_forbidden = m_RestrictedAreaForbidden->Checked?_TRUE:_FALSE;

    if( g_bParameterPage ){
        AnsiString  ss;
		AnsiString	str_lock="";
		AnsiString	str_code="";
		AnsiString	str_sec_ro="";
		AnsiString	str_fat="";
		AnsiString	str_restricted_area="";

		if(param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_lockdown) {
			str_lock.printf("SecuritySetting(Lockdown), ");
		}
        else {
//			str_lock.printf("SecuritySetting(Allow-Overwrite), ");
    	}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_download) {
			str_code = "W";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_readback) {
			if( 0 < str_code.Length() ) {
				str_code += "/";
			}
			str_code += "R";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_format) {
			if( 0 < str_code.Length() ) {
				str_code += "/";
			}
			str_code += "F";
		}
		if( 0 == str_code.Length() ) {
			str_code = "Forbidden";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_download) {
			str_sec_ro = "W";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_readback) {
			if( 0 < str_sec_ro.Length() ) {
				str_sec_ro += "/";
			}
			str_sec_ro += "R";
		}
		if( 0 == str_sec_ro.Length() ) {
			str_sec_ro = "Forbidden";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_download) {
			str_fat = "W";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_readback) {
			if( 0 < str_fat.Length() ) {
				str_fat += "/";
			}
			str_fat += "R";
		}
		if(!param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_format) {
			if( 0 < str_fat.Length() ) {
				str_fat += "/";
			}
			str_fat += "F";
		}
		if( 0 == str_fat.Length() ) {
			str_fat = "Forbidden";
		}
		if(param->m_ft_cfg.m_security_cfg.m_bit_ctrl.m_restricted_area_access_forbidden) {
			str_restricted_area.printf(", RestrictedArea(Forbidden)");
		}
        else {
//			str_restricted_area.printf(", RestrictedArea(Allow-Overwrite)");
    	}

		ss.printf("%sCODE(%s), SEC_RO(%s), FAT(%s)%s", str_lock.c_str(), str_code.c_str(), str_sec_ro.c_str(), str_fat.c_str(), str_restricted_area.c_str());
        MainForm->ed_sec_setting_Write->Text = ss;
    }
    else{
    	MainForm->RedrawDownloadList(g_pCurNode, true);
    }

    Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall Tm_SecuritySetting::FormShow(TObject *Sender)
{
	Display();
}
//---------------------------------------------------------------------------

void __fastcall Tm_SecuritySetting::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall Tm_SecuritySetting::m_CancelClick(TObject *Sender)
{
	Visible = false;
}


void __fastcall Tm_SecuritySetting::m_UncheckAllClick(TObject *Sender)
{
 	m_LockDown->Checked = false;
 	m_CODE_DL_Disabled->Checked = false;
 	m_CODE_RB_Disabled->Checked = false;
 	m_CODE_FMT_Disabled->Checked = false;
 	m_SECRO_DL_Disabled->Checked = false;
 	m_SECRO_RB_Disabled->Checked = false;
 	m_FAT_DL_Disabled->Checked = false;
 	m_FAT_RB_Disabled->Checked = false;
 	m_FAT_FMT_Disabled->Checked = false;
 	m_RestrictedAreaForbidden->Checked = false;
}
//---------------------------------------------------------------------------

void __fastcall Tm_SecuritySetting::m_CheckAllClick(TObject *Sender)
{
 	m_LockDown->Checked = true;
 	m_CODE_DL_Disabled->Checked = true;
 	m_CODE_RB_Disabled->Checked = true;
 	m_CODE_FMT_Disabled->Checked = true;
 	m_SECRO_DL_Disabled->Checked = true;
 	m_SECRO_RB_Disabled->Checked = true;
 	m_FAT_DL_Disabled->Checked = true;
 	m_FAT_RB_Disabled->Checked = true;
 	m_FAT_FMT_Disabled->Checked = true;
 	m_RestrictedAreaForbidden->Checked = true;
}
//---------------------------------------------------------------------------

