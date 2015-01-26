/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2002
*
*****************************************************************************/

/*******************************************************************************
* Filename:
* ---------
*  about.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  about window, display version, build date support feature.
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "about.h"
#include "flashtool.h"
#include "meta.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm_About *Form_About;
//---------------------------------------------------------------------------
__fastcall TForm_About::TForm_About(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm_About::Button1Click(TObject *Sender)
{
    Visible = false;        
}
//---------------------------------------------------------------------------
int TForm_About::GetVersion( unsigned int *major_ver, unsigned int *minor_ver, unsigned int *release_num, unsigned int *build_num )
{

	static unsigned int s_major_ver=0;
	static unsigned int s_minor_ver=0;
    static unsigned int s_release_num=0;
	static unsigned int s_build_num=0;

    if( NULL == major_ver || NULL == minor_ver || NULL== release_num || NULL == build_num ) {
    	return 1;
	}

    *major_ver = 0;
    *minor_ver = 0;
    *release_num = 0;
    *build_num = 0;

	if( 0==s_major_ver && 0==s_minor_ver && 0==s_release_num && 0==s_build_num ) {

		// get file version indo
		DWORD dwHnd;
	 	DWORD dwVerInfoSize;
	 	char *pBuffer;
		if( 0 >= (dwVerInfoSize=GetFileVersionInfoSize("flash_tool.exe", &dwHnd)) ) {
			// get fileversion size error
			return 2;
		}
		if( NULL == (pBuffer=(char *)malloc(dwVerInfoSize)) ) {
			// not enough memory
			return 3;
		}

		// get file version info
		if(!GetFileVersionInfo("flash_tool.exe", dwHnd, dwVerInfoSize, pBuffer)) {
			// get file version info fail
			return 4;
		}

		// get FileVersion string from resource
		VS_FIXEDFILEINFO *p_version;
		unsigned int version_len=0;
		if(!VerQueryValue(pBuffer, "\\", (void **)&p_version, &version_len)) {
			// get query value fail
			return 5;
		}

        /*                                            */
        /* Look at FILEVERSION in rc file.            */
        /*                                            */
        /* FILEVERSION    1,    2,    3,    4         */
        /*             0001  0002  0003  0004         */
        /*             -----+----  -----+----         */
        /*                  |         dwFileVersionLS */
        /*                  |                         */
        /*           dwFileVersionMS                  */
        /*                                            */
		s_major_ver = HIWORD(p_version->dwFileVersionMS);
		s_minor_ver = LOWORD(p_version->dwFileVersionMS);
        s_release_num = HIWORD(p_version->dwFileVersionLS);
		s_build_num = LOWORD(p_version->dwFileVersionLS);
	}

	// return version number
	*major_ver = s_major_ver;
	*minor_ver = s_minor_ver;
    *release_num = s_release_num;
	*build_num = s_build_num;

	return 0;
}

void __fastcall TForm_About::FormCreate(TObject *Sender)
{
   AnsiString as;
   lb_BuildDate->Caption = "Build date: " + AnsiString( __DATE__ );

   unsigned int major_ver;
   unsigned int minor_ver;
   unsigned int release_num;
   unsigned int build_num;

   // get flash tool (UI) verison
   GetVersion(&major_ver, &minor_ver, &release_num, &build_num);

//   VersionInfo->Caption = "FlashTool v" + IntToStr(major_ver) + "." + IntToStr(minor_ver) + "." + IntToStr(release_num) + "." + IntToStr(build_num);
    as.printf("FlashTool v%u.%04u.%02u.%02u", major_ver, minor_ver, release_num, build_num);
    VersionInfo->Caption = as;

#ifdef _DEBUG
     as = VersionInfo->Caption;
     as = as+" (DEBUG version) ";
     VersionInfo->Caption = as;
#endif /* #ifdef _DEBUG */

   const char *p_version, *p_release_type, *p_build_date;

   // get the brom dll information
   Brom_GetDLLInfo(&p_version, &p_release_type, &p_build_date, NULL);
   as.printf("%s", p_version);
   brom_dll_version->Caption = as;
   as.printf("%s", p_release_type);
   brom_dll_release_type->Caption = as;
   as.printf("(Build-Date: %s)", p_build_date);
   brom_dll_build_date->Caption = as;

   // get the meta dll information
   META_GetDLLInfo(&p_version, &p_release_type, &p_build_date, NULL);
   as.printf("%s", p_version);
   meta_dll_version->Caption = as;
   as.printf("%s", p_release_type);
   meta_dll_release_type->Caption = as;
   as.printf("(Build-Date: %s)", p_build_date);
   meta_dll_build_date->Caption = as;
}
//---------------------------------------------------------------------------






