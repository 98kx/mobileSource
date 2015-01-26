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
*  OptiWnd.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  Option config window
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DA_Option.h"
#include "main.h"
#include "ext_mem1.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm_DA_Option *Form_DA_Option;

extern bool DEBUG;
extern int iFlashItemIndex;

//---------------------------------------------------------------------------
__fastcall TForm_DA_Option::TForm_DA_Option(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm_DA_Option::FormShow(TObject *Sender)
{
	DA_INFO	da_info;
	DA_GetInfo(g_DA_HANDLE, &da_info);
	ed_StartAddr->Text = "0x"+IntToHex((int)da_info.start_addr, 8);
}
//---------------------------------------------------------------------------
void __fastcall TForm_DA_Option::bb_OKClick(TObject *Sender)
{
	DA_SetDefaultStartAddr(g_DA_HANDLE, strtoul(ed_StartAddr->Text.c_str(), NULL, 16));
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm_DA_Option::bb_DA_CancelClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm_DA_Option::ed_StartAddrKeyPress(TObject *Sender, char &Key)
{

      if ( Key == '\x1B' )
      {
         ed_StartAddr->Undo();
      }
      else if ( Key < '0' || Key > '9' )
      {
         if ( Key !='\b' &&
              ( Key < 'A' ||  (Key > 'F' && Key < 'a') || Key > 'f'))
        Key=NULL;
      }


   return;  // OLD
}
//---------------------------------------------------------------------------
