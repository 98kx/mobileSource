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
*  Readback option window header file
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#ifndef RB_OptionH
#define RB_OptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TForm_RB_Option : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *bb_ReadBack_OK;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TGroupBox *gb_Readback_Parameter_Type;
        TRadioButton *rb_ReadbackHex;
        TRadioButton *rb_ReadbackDec;
        TLabel *lb_StartAddr;
        TEdit *ed_RB_StartAddr;
        TLabel *lb_Length;
        TEdit *ed_RB_Length;
        TBitBtn *bb_Cancel;
    TGroupBox *m_Group_ReadBackFlag;
    TRadioButton *m_Radio_ReadPageOnly;
    TGroupBox *m_Group_ReadBackRange;
    TRadioButton *m_Radio_ReadPageSpare;
    TRadioButton *m_Radio_ReadPageWithECC;
    TRadioButton *m_Radio_ReadSpareOnly;
        void __fastcall bb_ReadBack_OKClick(TObject *Sender);
        void __fastcall bb_CancelClick(TObject *Sender);
        void __fastcall rb_ReadbackFlagClick(TObject *Sender);
        void __fastcall rb_ReadbackHexClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormHide(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
        void _fastcall KeyPress_Hex(TObject *Sender, char &Key);
        void _fastcall KeyPress_Dec(TObject *Sender, char &Key);
		unsigned int m_Original_Group_ReadBackRange_Top;

public:		// User declarations
        __fastcall TForm_RB_Option(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm_RB_Option *Form_RB_Option;
//---------------------------------------------------------------------------
#endif

