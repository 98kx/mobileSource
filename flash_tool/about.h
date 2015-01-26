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
*  about.h
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  about window header
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/


//---------------------------------------------------------------------------

#ifndef aboutH
#define aboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm_About : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TButton *Button1;
	TLabel *VersionInfo;
        TLabel *lb_BuildDate;
        TLabel *Label3;
	TLabel *brom_dll_version;
    TLabel *meta_dll_version;
	TLabel *brom_dll_release_type;
	TLabel *meta_dll_release_type;
	TLabel *brom_dll_build_date;
	TLabel *meta_dll_build_date;
    TLabel *da_version;
    TLabel *da_build_date;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
		int GetVersion( unsigned int *major_ver, unsigned int *minor_ver, unsigned int *release_num, unsigned int *build_num );
public:		// User declarations
        __fastcall TForm_About(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm_About *Form_About;
//---------------------------------------------------------------------------
#endif
