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
*  OK_Wnd.h
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  OK window header file 
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#ifndef OK_WndH
#define OK_WndH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TForm_OK : public TForm
{
__published:	// IDE-managed Components
        TImage *img_OK;
        TLabel *lbLastActionDoneAt;
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm_OK(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm_OK *Form_OK;
//---------------------------------------------------------------------------
#endif
