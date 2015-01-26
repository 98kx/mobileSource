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
*  OptiWnd.h
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  header file of option window
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/


//---------------------------------------------------------------------------

#ifndef DA_OptionH
#define DA_OptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm_DA_Option : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *bb_OK;
        TBitBtn *bb_DA_Cancel;
        TPageControl *PageControl1;
        TTabSheet *ts_Download;
        TLabel *Label3;
        TMaskEdit *ed_StartAddr;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall bb_OKClick(TObject *Sender);
        void __fastcall bb_DA_CancelClick(TObject *Sender);
        void __fastcall ed_StartAddrKeyPress(TObject *Sender, char &Key);
private:	// User declarations

public:		// User declarations
        __fastcall TForm_DA_Option(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TForm_DA_Option *Form_DA_Option;
//---------------------------------------------------------------------------
#endif
