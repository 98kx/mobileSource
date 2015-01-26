//---------------------------------------------------------------------------

#ifndef frm_COM_OptionH
#define frm_COM_OptionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TCOM_Option : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TPanel *m_Panel_COM_Option;
	TPanel *Panel1;
	TButton *btn_COM_Option_OK;
	TButton *btn_COM_Option_Cancel;
	TEdit *m_COM_Read_Timeout;
	TComboBox *m_COM_Read_Timeout_Option;
	TLabel *Label5;
	TLabel *Label6;
	TComboBox *m_COM_Write_Timeout_Option;
	TEdit *m_COM_Write_Timeout;
	TPanel *Panel2;
	TPanel *Panel3;
	TLabel *Label1;
	TEdit *m_Baudrate_FullSync_Count;
	TLabel *Label2;
	TLabel *Label3;
	void __fastcall btn_COM_Option_OKClick(TObject *Sender);
	void __fastcall btn_COM_Option_CancelClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall m_COM_Read_Timeout_OptionChange(TObject *Sender);
	void __fastcall m_COM_Write_Timeout_OptionChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TCOM_Option(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TCOM_Option *COM_Option;
//---------------------------------------------------------------------------
#endif
