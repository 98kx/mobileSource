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
*  OK_Wnd.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  display a OK window and close it
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OK_Wnd.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm_OK *Form_OK;
//---------------------------------------------------------------------------
__fastcall TForm_OK::TForm_OK(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm_OK::FormShow(TObject *Sender)
{
    const AnsiString caption = "Last action was done at " +
                               TDateTime::CurrentTime().FormatString("hh:nn");
                               
    lbLastActionDoneAt->Caption = caption;        
}
//---------------------------------------------------------------------------

