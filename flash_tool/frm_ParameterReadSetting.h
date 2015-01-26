//---------------------------------------------------------------------------

#ifndef frm_ParameterReadSettingH
#define frm_ParameterReadSettingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "main.h"
#include <ExtCtrls.hpp>

class Tm_ParameterReadSetting : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TEdit *m_addr;
    TLabel *Label2;
    TEdit *m_len;
    TBitBtn *BitBtn1;
    TEdit *m_file;
    TButton *m_OK;
    TButton *m_Cancel;
    TOpenDialog *m_OpenFileDialog;
    TSplitter *Splitter1;
    TSaveDialog *m_SaveFileDialog;
    void __fastcall BitBtn1Click(TObject *Sender);
    void __fastcall m_OKClick(TObject *Sender);
    void __fastcall m_CancelClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
    __fastcall Tm_ParameterReadSetting(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tm_ParameterReadSetting *m_ParameterReadSetting;
//---------------------------------------------------------------------------
#endif
