/*******************************************************************************
 * Filename:
 * ---------
 *  frm_FormatOption.cpp
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

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "frm_FormatOption.h"
#include "DL_Go.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormatOption *FormatOption;


//---------------------------------------------------------------------------
__fastcall TFormatOption::TFormatOption(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormatOption::OnShow(TObject *Sender) {

    if(g_FormatValidation) {
        m_FormatValidation->Checked = true;
    }
    else {
        m_FormatValidation->Checked = false;
    }

    if( NOR_OPERATION == g_OperationMethod ) {
		FormatStartAddr->Text = "0x" + IntToHex((int)g_ulFormatStartAddr, 8);
    	FormatLength->Text = "0x" + IntToHex((int)g_ulFormatLength, 8);
    }
    else {
		FormatStartAddr->Text = "0x" + IntToHex((int)g_ulNandFormatStartAddr, 8);
    	FormatLength->Text = "0x" + IntToHex((int)g_ulNandFormatLength, 8);
	}

  	// save the original groupbox height
  	m_Original_GroupBox_Height = GroupBox1->Height;
   	m_Original_Form_Height = Height;
    m_Original_Panel_ManualFormat2_Height = m_Panel_ManualFormat2->Height;
    m_Original_Panel_TotalFormat2_Height = m_Panel_TotalFormat2->Height;
  	m_Panel_Offset = m_Panel_ResetToFactoryDefault1->Top-m_Panel_DisableFormat1->Top;

	if(bCallFromFormatButton) {

        GroupBox1->Height -= m_Panel_Offset;
       	Height -= m_Panel_Offset;

		m_Panel_ResetToFactoryDefault1->Top -= m_Panel_Offset;
		m_Panel_ResetToFactoryDefault2->Top -= m_Panel_Offset;
    	m_Panel_AutoFormat1->Top -= m_Panel_Offset;
    	m_Panel_AutoFormat2->Top -= m_Panel_Offset;
		m_Panel_ManualFormat1->Top -= m_Panel_Offset;
		m_Panel_ManualFormat2->Top -= m_Panel_Offset;
       	m_Panel_TotalFormat1->Top -= m_Panel_Offset;
    	m_Panel_TotalFormat2->Top -= m_Panel_Offset;

    	m_Panel_DisableFormat1->Visible = false;
    	m_Panel_DisableFormat2->Visible = false;
    	m_Panel_ResetToFactoryDefault1->Visible = true;
    	m_Panel_ResetToFactoryDefault2->Visible = true;
    	m_Panel_AutoFormat1->Visible = true;
    	m_Panel_AutoFormat2->Visible = true;
    	m_Panel_ManualFormat1->Visible = false;
    	m_Panel_ManualFormat2->Visible = false;

   

		switch(g_FormatOption) {
		case MANUAL_FORMAT_OPTION:
			m_Radio_ManualFormatClick(Sender);
			break;
		case FAT_FORMAT_OPTION:
			m_Radio_AutoFormatClick(Sender);
			break;
                case TOTAL_FORMAT_OPTION:
                        m_Radio_TotalFormatClick(Sender);
                        break;
		case RESET_TO_FACTORY:
		default:
			m_Radio_ResetToFactoryDefaultClick(Sender);
			break;
		}

        m_Group_EraseFlag->ItemIndex = 0;
	}
    else {

    	m_Panel_DisableFormat1->Visible = true;
    	m_Panel_DisableFormat2->Visible = true;
    	m_Panel_ResetToFactoryDefault1->Visible = true;
    	m_Panel_ResetToFactoryDefault2->Visible = true;
    	m_Panel_AutoFormat1->Visible = true;
    	m_Panel_AutoFormat2->Visible = true;
    	m_Panel_ManualFormat1->Visible = false;
    	m_Panel_ManualFormat2->Visible = false;

		switch(g_FormatAfterDownloadOption) {
		case RESET_TO_FACTORY:
			m_Radio_ResetToFactoryDefaultClick(Sender);
			break;
		case FAT_FORMAT_OPTION:
			m_Radio_AutoFormatClick(Sender);
			break;
		case MANUAL_FORMAT_OPTION:
			m_Radio_ManualFormatClick(Sender);
			break;
		case DISABLE_FORMAT:
		default:
			m_Radio_DisableFormatClick(Sender);
			break;
		}

    }

    m_Panel_TotalFormat1->Visible = true;
    m_Panel_TotalFormat2->Visible = true;    

    if(g_bEnableAdvanceOption)
    {
        m_Panel_ManualFormat1->Visible = true;
        m_Panel_ManualFormat2->Visible = true;
    }

    if( bCallFromFormatButton && !(NOR_OPERATION==g_OperationMethod) && g_bEnableAdvanceOption ) {
        m_Group_EraseFlag->Visible = true;
    }
    else {
       	m_Panel_ManualFormat2->Height -= (m_Group_EraseFlag->Height+10);
        GroupBox1->Height -= (m_Group_EraseFlag->Height+10);
       	Height -= (m_Group_EraseFlag->Height+10);
        m_Group_EraseFlag->Visible = false;
	}

    Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormatOption::btn_FormatOKClick(TObject *Sender)
{
    // Fomat OK button is clicked

    if( NOR_OPERATION == g_OperationMethod ) {
		g_ulFormatStartAddr = strtoul(FormatStartAddr->Text.c_str(), NULL, 16);
		g_ulFormatLength    = strtoul(FormatLength->Text.c_str(), NULL, 16);
    }
    else {
		g_ulNandFormatStartAddr = strtoul(FormatStartAddr->Text.c_str(), NULL, 16);
		g_ulNandFormatLength    = strtoul(FormatLength->Text.c_str(), NULL, 16);
	}

    if( !bCallFromFormatButton ) {

		if( m_Radio_ResetToFactoryDefault->Checked )
        {
			g_FormatAfterDownloadOption = RESET_TO_FACTORY;

		}
		else if(m_Radio_AutoFormat->Checked) {
			g_FormatAfterDownloadOption = FAT_FORMAT_OPTION;

	    }
	    else if(m_Radio_ManualFormat->Checked) {
			g_FormatAfterDownloadOption = MANUAL_FORMAT_OPTION;
	    }
        else {
			g_FormatAfterDownloadOption = DISABLE_FORMAT;
        }

		MainForm->updateMenuItem();
    }
	else {

		if( m_Radio_ResetToFactoryDefault->Checked ) {
			g_FormatOption = RESET_TO_FACTORY;
		}
		else if ( m_Radio_AutoFormat->Checked ) {
			g_FormatOption = FAT_FORMAT_OPTION;
	    }
        else if( m_Radio_TotalFormat->Checked )
        {
            g_FormatOption = TOTAL_FORMAT_OPTION;
        }
        else
        {
			g_FormatOption = MANUAL_FORMAT_OPTION;
        }

        switch ( m_Group_EraseFlag->ItemIndex )
        {
            case 1:
                g_EraseFlag = NUTL_FORCE_ERASE;
                break;
            case 2:
                g_EraseFlag = NUTL_MARK_BAD_BLOCK;
                break;
            case 0:
            default:
                g_EraseFlag = NUTL_ERASE;
                break;
        }

		if( MT6205 != g_BBCHIP_TYPE ) {

            if ((g_FormatOption != RESET_TO_FACTORY) &&
                g_asNormalModeDAPath.IsEmpty())
            {
                ShowMessage(" You must open a download agent file before formatting. ");

                if (!MainForm->FileOpenDownloadAgentExecute())
                {
                    Hide();
                    return;
                }
            }

            if (!g_asNormalModeDAPath.IsEmpty())
            {
                // update DA to the lastest version
                if (MainForm->UpdateDA())
                {
                    Hide();
                    return;
                }
            }

            // clean Flash device info
            MainForm->HWStatusBarClear();

			MainForm->start_DL( true );
		}
        else {
			Form_DL_GO->ShowModal();
        }
	}

	Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormatOption::btn_FormatCancelClick(TObject *Sender)
{
	Hide();
}

//---------------------------------------------------------------------------

void __fastcall TFormatOption::m_Radio_ManualFormatClick(TObject *Sender)
{
    m_Radio_DisableFormat->Checked              = false;
    m_Radio_ResetToFactoryDefault->Checked  = false;
    m_Radio_AutoFormat->Checked                 = false;
    m_Radio_ManualFormat->Checked              = true;
    m_Radio_TotalFormat->Checked                = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormatOption::m_Radio_AutoFormatClick(TObject *Sender)
{
    m_Radio_DisableFormat->Checked              = false;
    m_Radio_ResetToFactoryDefault->Checked  = false;
    m_Radio_AutoFormat->Checked                 = true;
    m_Radio_ManualFormat->Checked              = false;
    m_Radio_TotalFormat->Checked                = false;
}
//---------------------------------------------------------------------------

void __fastcall TFormatOption::m_Radio_DisableFormatClick(TObject *Sender)
{
    m_Radio_DisableFormat->Checked              = true;
    m_Radio_ResetToFactoryDefault->Checked  = false;
    m_Radio_AutoFormat->Checked                 = false;
    m_Radio_ManualFormat->Checked              = false;
    m_Radio_TotalFormat->Checked                = false;
}

//---------------------------------------------------------------------------

void __fastcall TFormatOption::m_Radio_ResetToFactoryDefaultClick(TObject *Sender)
{
    m_Radio_DisableFormat->Checked              = false;
    m_Radio_ResetToFactoryDefault->Checked  = true;
    m_Radio_AutoFormat->Checked                 = false;
    m_Radio_ManualFormat->Checked              = false;
    m_Radio_TotalFormat->Checked                = false;
}

//---------------------------------------------------------------------------
void __fastcall TFormatOption::FormClose(TObject *Sender, TCloseAction &Action)
{
	Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormatOption::Hide()
{

    Visible = false;

   	// restore the original groupbox height
    GroupBox1->Height = m_Original_GroupBox_Height;
    Height = m_Original_Form_Height;
 	m_Panel_ManualFormat2->Height = m_Original_Panel_ManualFormat2_Height;
        m_Panel_TotalFormat2->Height = m_Original_Panel_TotalFormat2_Height;

	if(bCallFromFormatButton) {
		m_Panel_ResetToFactoryDefault1->Top += m_Panel_Offset;
		m_Panel_ResetToFactoryDefault2->Top += m_Panel_Offset;
		m_Panel_AutoFormat1->Top += m_Panel_Offset;
		m_Panel_AutoFormat2->Top += m_Panel_Offset;
		m_Panel_ManualFormat1->Top += m_Panel_Offset;
		m_Panel_ManualFormat2->Top += m_Panel_Offset;
                m_Panel_TotalFormat1->Top += m_Panel_Offset;
		m_Panel_TotalFormat2->Top += m_Panel_Offset;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormatOption::m_FormatValidationClick( TObject *Sender)
{
    if(m_FormatValidation->Checked) {
        g_FormatValidation = true;
    }
    else {
        g_FormatValidation = false;
    }
}
void __fastcall TFormatOption::m_Radio_TotalFormatClick(TObject *Sender)
{
    m_Radio_DisableFormat->Checked              = false;
    m_Radio_ResetToFactoryDefault->Checked  = false;
    m_Radio_AutoFormat->Checked                 = false;
    m_Radio_ManualFormat->Checked              = false;
    m_Radio_TotalFormat->Checked                = true;
}
//---------------------------------------------------------------------------

