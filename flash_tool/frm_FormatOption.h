/******************************************************************************
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
 *  frm_FormatOption.h
 *
 * Project:
 * --------
 *  Flash tool set
 *
 * Description:
 * ------------
 *  Form of format option
 *
 * Author:
 * -------
 *  Amos Hsu (mtk00539)
 *
 *******************************************************************************/

//---------------------------------------------------------------------------

#ifndef frm_FormatOptionH
#define frm_FormatOptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormatOption : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *FormatStartAddr;
	TLabeledEdit *FormatLength;
	TButton *btn_FormatOK;
	TButton *btn_FormatCancel;
	TLabel *Label1;
	TLabel *Label2;
	TGroupBox *GroupBox1;
	TRadioButton *m_Radio_AutoFormat;
	TPanel *m_Panel_AutoFormat2;
	TPanel *m_Panel_AutoFormat1;
	TPanel *m_Panel_ManualFormat1;
	TRadioButton *m_Radio_ManualFormat;
	TPanel *m_Panel_ManualFormat2;
	TRadioButton *m_Radio_DisableFormat;
	TPanel *m_Panel_DisableFormat2;
	TLabel *Label3;
	TPanel *m_Panel_DisableFormat1;
	TLabel *Label4;
    TPanel *m_Panel_ResetToFactoryDefault1;
    TRadioButton *m_Radio_ResetToFactoryDefault;
    TPanel *m_Panel_ResetToFactoryDefault2;
    TLabel *Label5;
    TLabel *Label8;
    TRadioGroup *m_Group_EraseFlag;
    TCheckBox *m_FormatValidation;
    TLabel *Label9;
    TCheckBox *CheckBox1;
        TPanel *m_Panel_TotalFormat1;
        TPanel *m_Panel_TotalFormat2;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label10;
        TRadioButton *m_Radio_TotalFormat;
	void __fastcall btn_FormatOKClick(TObject *Sender);
	void __fastcall btn_FormatCancelClick(TObject *Sender);
	void __fastcall OnShow(TObject *Sender);
	void __fastcall m_Radio_ManualFormatClick(TObject *Sender);
	void __fastcall m_Radio_AutoFormatClick(TObject *Sender);
	void __fastcall m_Radio_DisableFormatClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Hide();
    void __fastcall m_Radio_ResetToFactoryDefaultClick(TObject *Sender);
    void __fastcall m_FormatValidationClick(TObject *Sender);
        void __fastcall m_Radio_TotalFormatClick(TObject *Sender);

private:	// User declarations
	unsigned int m_Original_GroupBox_Height;
	unsigned int m_Original_Form_Height;
    unsigned int m_Original_Panel_ManualFormat2_Height;
        unsigned int m_Original_Panel_TotalFormat2_Height;
	unsigned int m_Panel_Offset;
public:		// User declarations
    bool	bCallFromFormatButton;
	__fastcall TFormatOption(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormatOption *FormatOption;
//---------------------------------------------------------------------------
#endif
