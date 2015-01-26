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
*  RB_Option.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  Readback option window
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RB_Option.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm_RB_Option *Form_RB_Option;

bool bReadbackAddrLengthTypeHex;

//---------------------------------------------------------------------------
__fastcall TForm_RB_Option::TForm_RB_Option(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::bb_ReadBack_OKClick(TObject *Sender)
{
	int ret;
    unsigned long readback_addr;
    unsigned long readback_len;
    TListItem *pli = MainForm->lv_ReadBack->Selected;
    if ( pli != NULL )
    {
		if ( ed_RB_StartAddr->Text.Length() > 0 ) {
			if( bReadbackAddrLengthTypeHex ) {
				readback_addr = strtoul(ed_RB_StartAddr->Text.c_str(), NULL, 16);
			}
			else {
				readback_addr = strtoul(ed_RB_StartAddr->Text.c_str(), NULL, 10);
			}
		}

		if ( ed_RB_Length->Text.Length() > 0 ) {
			if ( bReadbackAddrLengthTypeHex ) {
				readback_len = strtoul(ed_RB_Length->Text.c_str(), NULL, 16);
			}
			else {
				readback_len = strtoul(ed_RB_Length->Text.c_str(), NULL, 10);
			}
		}

		// set read back flag 
		NUTL_ReadFlag_E		read_flag;
		if(m_Radio_ReadPageSpare->Checked) {
			read_flag = NUTL_READ_PAGE_SPARE;
		}
		else if(m_Radio_ReadPageOnly->Checked) {
			read_flag = NUTL_READ_PAGE_ONLY;
		}
		else if(m_Radio_ReadSpareOnly->Checked) {
			read_flag = NUTL_READ_SPARE_ONLY;
		}
		else if(m_Radio_ReadPageWithECC->Checked) {
			read_flag = NUTL_READ_PAGE_WITH_ECC;
		}
		else {
			read_flag = NUTL_READ_PAGE_WITH_ECC;
		}

		if(S_DONE != (ret=RB_SetReadbackFlag(g_RB_HANDLE, MainForm->lv_ReadBack->ItemIndex, read_flag))) {
			if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
				ShowMessage("Program is busy! It can not update settings right now.");
			}
			else {
				ShowMessage("Failed to update the readback settings!");
			}
			goto end;
		}
   	
		if( NOR_OPERATION != g_OperationMethod ) {
			// NAND flash read back must be pagesize alignment 
			if( 0 != (readback_addr%2048) ) {
				readback_addr = ((readback_addr/2048)+1)*2048;
				ShowMessage("[WARNING] This readback address is adjusted to 2048 pagesize alignment!");
			}				
			if( 0 != (readback_len%2048) ) {
				readback_len = ((readback_len/2048)+1)*2048;
				ShowMessage("[WARNING] This readback length is adjusted to 2048 pagesize alignment!");
			}				
		}

		if(S_DONE != (ret=RB_SetReadbackAddr(g_RB_HANDLE, MainForm->lv_ReadBack->ItemIndex, readback_addr))) {
			if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
				ShowMessage("Program is busy! It can not update settings right now.");
			}
			else {
				ShowMessage("Failed to update the readback settings!");
			}
			goto end;
		}

		if(S_DONE != (ret=RB_SetReadbackLength(g_RB_HANDLE, MainForm->lv_ReadBack->ItemIndex, readback_len))) {
			if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
				ShowMessage("Program is busy! It can not update settings right now.");
			}
			else {
				ShowMessage("Failed to update the readback settings!");
			}
			goto end;
		}

		MainForm->RedrawReadbackList();
    } // if

end:
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::bb_CancelClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void _fastcall TForm_RB_Option::KeyPress_Dec(TObject *Sender, char &Key)
{

      if ( Key == '\x1B' )
      {
        dynamic_cast<TEdit *>( Sender)->Undo();
      }
      else if ( Key < '0' || Key > '9' )
      {
         if ( Key !='\b' )
        Key=NULL;
      }

}
//---------------------------------------------------------------------------
void _fastcall TForm_RB_Option::KeyPress_Hex(TObject *Sender, char &Key)
{
      if ( Key == '\x1B' )
      {
        dynamic_cast<TEdit *>( Sender)->Undo();
      }
      else if ( Key < '0' || Key > '9' )
      {
         if ( Key !='\b' &&
              ( Key < 'A' ||  (Key > 'F' && Key < 'a') || Key > 'f'))
        Key=NULL;
      }
}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::rb_ReadbackFlagClick(TObject *Sender)
{
   if ( bReadbackAddrLengthTypeHex )
   {
     ed_RB_StartAddr->Text = IntToStr(strtoul(ed_RB_StartAddr->Text.c_str(), NULL, 16));
     ed_RB_Length->Text = IntToStr(strtoul(ed_RB_Length->Text.c_str(), NULL, 16));

     ed_RB_StartAddr->OnKeyPress = KeyPress_Dec;
     ed_RB_Length->OnKeyPress = KeyPress_Dec;

     bReadbackAddrLengthTypeHex = false;
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::rb_ReadbackHexClick(TObject *Sender)
{
   if ( ! bReadbackAddrLengthTypeHex )
   {
     ed_RB_StartAddr->Text = "0x" + IntToHex((int)strtoul(ed_RB_StartAddr->Text.c_str(), NULL, 10), 8);
     ed_RB_Length->Text = "0x" + IntToHex((int)strtoul(ed_RB_Length->Text.c_str(), NULL, 10), 8);
     ed_RB_StartAddr->OnKeyPress = KeyPress_Hex;
     ed_RB_Length->OnKeyPress = KeyPress_Hex;

     bReadbackAddrLengthTypeHex = true;
   }

}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::FormCreate(TObject *Sender)
{
    bReadbackAddrLengthTypeHex = true;
    rb_ReadbackHex->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::FormHide(TObject *Sender)
{
	if( m_Group_ReadBackRange->Top != (int)m_Original_Group_ReadBackRange_Top ) {
		m_Group_ReadBackRange->Top = (int)m_Original_Group_ReadBackRange_Top;
	}

	if( NOR_OPERATION == g_OperationMethod ) {
		m_Group_ReadBackFlag->Visible = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm_RB_Option::FormShow(TObject *Sender)
{
	m_Original_Group_ReadBackRange_Top = m_Group_ReadBackRange->Top;
	if( NOR_OPERATION == g_OperationMethod ) {
		m_Group_ReadBackRange->Top = m_Group_ReadBackFlag->Top;
		m_Group_ReadBackFlag->Visible = false;
	}
}
//---------------------------------------------------------------------------

