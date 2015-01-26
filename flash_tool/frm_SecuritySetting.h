//---------------------------------------------------------------------------

#ifndef frm_SecuritySettingH
#define frm_SecuritySettingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "main.h"
#include <Menus.hpp>

class Tm_SecuritySetting : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *m_GroupSecuritySetting;
	TCheckBox *m_LockDown;
	TCheckBox *m_FAT_RB_Disabled;
	TCheckBox *m_CODE_DL_Disabled;
	TCheckBox *m_CODE_RB_Disabled;
	TCheckBox *m_CODE_FMT_Disabled;
	TCheckBox *m_SECRO_DL_Disabled;
	TCheckBox *m_SECRO_RB_Disabled;
	TCheckBox *m_FAT_DL_Disabled;
	TCheckBox *m_FAT_FMT_Disabled;
	TButton *m_OK;
	TButton *m_Cancel;
	TCheckBox *m_RestrictedAreaForbidden;
	TPopupMenu *m_SecuritySettingRightClick;
	TMenuItem *m_UncheckAll;
	TMenuItem *m_CheckAll;
	void __fastcall m_OKClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall m_CancelClick(TObject *Sender);
	void __fastcall m_UncheckAllClick(TObject *Sender);
	void __fastcall m_CheckAllClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall Tm_SecuritySetting(TComponent* Owner);
	void __fastcall Display(void);
};
//---------------------------------------------------------------------------
extern PACKAGE Tm_SecuritySetting *m_SecuritySetting;
//---------------------------------------------------------------------------
#endif
