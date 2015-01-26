//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_COM_Option.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCOM_Option *COM_Option;
//---------------------------------------------------------------------------
__fastcall TCOM_Option::TCOM_Option(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCOM_Option::FormShow(TObject *Sender)
{
	m_COM_Read_Timeout->Text = "5000";
	if( COM_DEFAULT_TIMEOUT <= g_COM_Read_Timeout ) {
		m_COM_Read_Timeout_Option->ItemIndex = 1;
	}
	else if( COM_NO_TIMEOUT == g_COM_Read_Timeout ) {
		m_COM_Read_Timeout_Option->ItemIndex = 0;
	}
	else {
		m_COM_Read_Timeout->Text = IntToStr(g_COM_Read_Timeout);
		m_COM_Read_Timeout_Option->ItemIndex = 2;
	}
	m_COM_Read_Timeout_OptionChange(Sender);

	m_COM_Write_Timeout->Text = "1500";
	if( COM_DEFAULT_TIMEOUT <= g_COM_Write_Timeout ) {
		m_COM_Write_Timeout_Option->ItemIndex = 1;
	}
	else if( COM_NO_TIMEOUT == g_COM_Write_Timeout ) {
		m_COM_Write_Timeout_Option->ItemIndex = 0;
	}
	else {
		m_COM_Write_Timeout->Text = IntToStr(g_COM_Write_Timeout);
		m_COM_Write_Timeout_Option->ItemIndex = 2;
	}
	m_COM_Write_Timeout_OptionChange(Sender);

	m_Baudrate_FullSync_Count->Text = IntToStr(g_Baudrate_FullSync_Count);

    Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TCOM_Option::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TCOM_Option::btn_COM_Option_OKClick(TObject *Sender)
{
	if( 0 == m_COM_Read_Timeout_Option->ItemIndex ) {
		g_COM_Read_Timeout = COM_NO_TIMEOUT;
	}
	else if( 1 == m_COM_Read_Timeout_Option->ItemIndex ) {
		g_COM_Read_Timeout = COM_DEFAULT_TIMEOUT;
	}
	else if( 2 == m_COM_Read_Timeout_Option->ItemIndex ) {
		g_COM_Read_Timeout = strtoul(m_COM_Read_Timeout->Text.c_str(), NULL, 10);
		if( COM_DEFAULT_TIMEOUT <= g_COM_Read_Timeout ) {
			g_COM_Read_Timeout = COM_DEFAULT_TIMEOUT;
		}
	}

	if( 0 == m_COM_Write_Timeout_Option->ItemIndex ) {
		g_COM_Write_Timeout = COM_NO_TIMEOUT;
	}
	else if( 1 == m_COM_Write_Timeout_Option->ItemIndex ) {
		g_COM_Write_Timeout = COM_DEFAULT_TIMEOUT;
	}
	else if( 2 == m_COM_Write_Timeout_Option->ItemIndex ) {
		g_COM_Write_Timeout = strtoul(m_COM_Write_Timeout->Text.c_str(), NULL, 10);
		if( COM_DEFAULT_TIMEOUT <= g_COM_Write_Timeout ) {
			g_COM_Write_Timeout = COM_DEFAULT_TIMEOUT;
		}
	}

	g_Baudrate_FullSync_Count = StrToInt(m_Baudrate_FullSync_Count->Text);

    Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TCOM_Option::btn_COM_Option_CancelClick(TObject *Sender)
{
    Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TCOM_Option::m_COM_Read_Timeout_OptionChange(
      TObject *Sender)
{
	if( 2 != m_COM_Read_Timeout_Option->ItemIndex ) {
		m_COM_Read_Timeout->Visible = false;
	}
	else {
		m_COM_Read_Timeout->Visible = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TCOM_Option::m_COM_Write_Timeout_OptionChange(
      TObject *Sender)
{
	if( 2 != m_COM_Write_Timeout_Option->ItemIndex ) {
		m_COM_Write_Timeout->Visible = false;
	}
	else {
		m_COM_Write_Timeout->Visible = true;
	}
}
//---------------------------------------------------------------------------

