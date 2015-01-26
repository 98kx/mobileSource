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
*  ext_mem1.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  external memory 1 setup window
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ext_mem1.h"
#include "main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

Text_mem1Form *ext_mem1Form;

//---------------------------------------------------------------------------
__fastcall Text_mem1Form::Text_mem1Form(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Text_mem1Form::FormShow(TObject *Sender)
{
	m_Bank0_Option->ItemIndex = g_EMI_Config[0].m_bAuto?1:0;
	m_Bank0_OptionChange(Sender);
	m_Bank0_EMI->Text = "0x" + IntToHex((int)(g_EMI_Config[0].m_Config), 8);

	m_Bank1_Option->ItemIndex = g_EMI_Config[1].m_bAuto?1:0;
	m_Bank1_OptionChange(Sender);
	m_Bank1_EMI->Text = "0x" + IntToHex((int)(g_EMI_Config[1].m_Config), 8);

    m_nfi_Option->ItemIndex = g_EMI_NFI_Setting.m_bAuto?1:0;
    m_nfi_OptionChange(Sender);
    m_nand_acccon->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_nand_acccon), 8);

    m_emi_gen_a->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_a), 8);
    m_emi_gen_b->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_b), 8);
    m_emi_gen_c->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_c), 8);
    m_emi_gen_d->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_d), 8);
    m_emi_con_i->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i), 8);
    m_emi_con_i_ext->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i_ext), 8);
    m_emi_con_j->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_j), 8);
    m_emi_con_k->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_k), 8);
    m_emi_con_l->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_l), 8);
    m_emi_con_m->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_m), 8);
    m_emi_con_n->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_n), 8);
    m_emi_del_a->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_a), 8);
    m_emi_del_b->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_b), 8);
    m_emi_del_c->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_c), 8);
    m_emi_del_d->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_d), 8);
    m_emi_del_e->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_e), 8);
    m_emi_del_f->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_f), 8);
    m_emi_del_g->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_g), 8);
    m_emi_del_h->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_h), 8);
    m_emi_del_i->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_i), 8);
    m_emi_del_j->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_j), 8);
    m_emi_arb_a->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_a), 8);
    m_emi_arb_b->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_b), 8);
    m_emi_arb_c->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_c), 8);
    m_emi_arb_d->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_d), 8);
    m_emi_arb_e->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_e), 8);
    m_emi_arb_f->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_f), 8);
    m_emi_arb_g->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_g), 8);
    m_emi_arb_h->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_h), 8);
    m_emi_arb_i->Text = "0x"+ IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_i), 8);

    EnableDRAMSetting(g_EMI_NFI_Setting.m_dram_cfg.m_valid);

    Visible = true;
}

//---------------------------------------------------------------------------

void __fastcall Text_mem1Form::btn_EMI_OKClick(TObject *Sender)
{
	g_EMI_Config[0].m_bAuto = (1==m_Bank0_Option->ItemIndex)?true:false;
	g_EMI_Config[0].m_Config = strtoul(m_Bank0_EMI->Text.c_str(), NULL, 16);

	g_EMI_Config[1].m_bAuto = (1==m_Bank1_Option->ItemIndex)?true:false;
	g_EMI_Config[1].m_Config = strtoul(m_Bank1_EMI->Text.c_str(), NULL, 16);

    g_EMI_NFI_Setting.m_bAuto = (1==m_nfi_Option->ItemIndex)?true:false;
	g_EMI_NFI_Setting.m_nand_acccon = strtoul(m_nand_acccon->Text.c_str(), NULL, 16);

	g_EMI_NFI_Setting.m_dram_cfg.m_valid = m_EnableDRAMSetting->Checked?_TRUE:_FALSE;
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_a = strtoul(m_emi_gen_a->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_b = strtoul(m_emi_gen_b->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_c = strtoul(m_emi_gen_c->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_d = strtoul(m_emi_gen_d->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i = strtoul(m_emi_con_i->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i_ext = strtoul(m_emi_con_i_ext->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_j = strtoul(m_emi_con_j->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_k = strtoul(m_emi_con_k->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_l = strtoul(m_emi_con_l->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_m = strtoul(m_emi_con_m->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_n = strtoul(m_emi_con_n->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_a = strtoul(m_emi_del_a->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_b = strtoul(m_emi_del_b->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_c = strtoul(m_emi_del_c->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_d = strtoul(m_emi_del_d->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_e = strtoul(m_emi_del_e->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_f = strtoul(m_emi_del_f->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_g = strtoul(m_emi_del_g->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_h = strtoul(m_emi_del_h->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_i = strtoul(m_emi_del_i->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_j = strtoul(m_emi_del_j->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_a = strtoul(m_emi_arb_a->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_b = strtoul(m_emi_arb_b->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_c = strtoul(m_emi_arb_c->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_d = strtoul(m_emi_arb_d->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_e = strtoul(m_emi_arb_e->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_f = strtoul(m_emi_arb_f->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_g = strtoul(m_emi_arb_g->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_h = strtoul(m_emi_arb_h->Text.c_str(), NULL, 16);
	g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_i = strtoul(m_emi_arb_i->Text.c_str(), NULL, 16);

    Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall Text_mem1Form::btn_EMI_CancelClick(TObject *Sender)
{
    Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall Text_mem1Form::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    Visible = false;
}
void __fastcall Text_mem1Form::m_Bank0_OptionChange(TObject *Sender)
{
	if( 1 == m_Bank0_Option->ItemIndex ) {
		m_Bank0_EMI->Visible = false;
	}
	else {
		m_Bank0_EMI->Visible = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall Text_mem1Form::m_Bank1_OptionChange(TObject *Sender)
{
	if( 1 == m_Bank1_Option->ItemIndex ) {
		m_Bank1_EMI->Visible = false;
	}
	else {
		m_Bank1_EMI->Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall Text_mem1Form::m_EnableDRAMSettingClick(TObject *Sender)
{
    EnableDRAMSetting(m_EnableDRAMSetting->Checked);
}

//---------------------------------------------------------------------------
void __fastcall Text_mem1Form::EnableDRAMSetting(bool bEnable)
{
    if(bEnable) {
        m_EnableDRAMSetting->Checked = true;
        Label7->Enabled = true;
        Label8->Enabled = true;
        Label9->Enabled = true;
        Label10->Enabled = true;
        Label11->Enabled = true;
        Label12->Enabled = true;
        Label13->Enabled = true;
        Label15->Enabled = true;
        Label16->Enabled = true;
        Label17->Enabled = true;
        Label18->Enabled = true;
        Label19->Enabled = true;
        Label20->Enabled = true;
        Label21->Enabled = true;
        Label22->Enabled = true;
        Label23->Enabled = true;
        Label24->Enabled = true;
        Label25->Enabled = true;
        Label26->Enabled = true;
        Label27->Enabled = true;
        Label28->Enabled = true;
        Label29->Enabled = true;
        Label30->Enabled = true;
        Label31->Enabled = true;
        Label32->Enabled = true;
        Label33->Enabled = true;
        Label34->Enabled = true;
        Label35->Enabled = true;
        Label36->Enabled = true;
        Label37->Enabled = true;
        m_emi_gen_a->Enabled = true;
        m_emi_gen_b->Enabled = true;
        m_emi_gen_c->Enabled = true;
        m_emi_gen_d->Enabled = true;
        m_emi_con_i_ext->Enabled = true;
        m_emi_con_i->Enabled = true;
        m_emi_con_j->Enabled = true;
        m_emi_con_k->Enabled = true;
        m_emi_con_l->Enabled = true;
        m_emi_con_m->Enabled = true;
        m_emi_con_n->Enabled = true;
        m_emi_del_a->Enabled = true;
        m_emi_del_b->Enabled = true;
        m_emi_del_c->Enabled = true;
        m_emi_del_d->Enabled = true;
        m_emi_del_e->Enabled = true;
        m_emi_del_f->Enabled = true;
        m_emi_del_g->Enabled = true;
        m_emi_del_h->Enabled = true;
        m_emi_del_i->Enabled = true;
        m_emi_del_j->Enabled = true;
        m_emi_arb_a->Enabled = true;
        m_emi_arb_b->Enabled = true;
        m_emi_arb_c->Enabled = true;
        m_emi_arb_d->Enabled = true;
        m_emi_arb_e->Enabled = true;
        m_emi_arb_f->Enabled = true;
        m_emi_arb_g->Enabled = true;
        m_emi_arb_h->Enabled = true;
        m_emi_arb_i->Enabled = true;
    }
    else {
        m_EnableDRAMSetting->Checked = false;
        Label7->Enabled = false;
        Label8->Enabled = false;
        Label9->Enabled = false;
        Label10->Enabled = false;
        Label11->Enabled = false;
        Label12->Enabled = false;
        Label13->Enabled = false;
        Label15->Enabled = false;
        Label16->Enabled = false;
        Label17->Enabled = false;
        Label18->Enabled = false;
        Label19->Enabled = false;
        Label20->Enabled = false;
        Label21->Enabled = false;
        Label22->Enabled = false;
        Label23->Enabled = false;
        Label24->Enabled = false;
        Label25->Enabled = false;
        Label26->Enabled = false;
        Label27->Enabled = false;
        Label28->Enabled = false;
        Label29->Enabled = false;
        Label30->Enabled = false;
        Label31->Enabled = false;
        Label32->Enabled = false;
        Label33->Enabled = false;
        Label34->Enabled = false;
        Label35->Enabled = false;
        Label36->Enabled = false;
        Label37->Enabled = false;
        m_emi_gen_a->Enabled = false;
        m_emi_gen_b->Enabled = false;
        m_emi_gen_c->Enabled = false;
        m_emi_gen_d->Enabled = false;
        m_emi_con_i_ext->Enabled = false;
        m_emi_con_i->Enabled = false;
        m_emi_con_j->Enabled = false;
        m_emi_con_k->Enabled = false;
        m_emi_con_l->Enabled = false;
        m_emi_con_m->Enabled = false;
        m_emi_con_n->Enabled = false;
        m_emi_del_a->Enabled = false;
        m_emi_del_b->Enabled = false;
        m_emi_del_c->Enabled = false;
        m_emi_del_d->Enabled = false;
        m_emi_del_e->Enabled = false;
        m_emi_del_f->Enabled = false;
        m_emi_del_g->Enabled = false;
        m_emi_del_h->Enabled = false;
        m_emi_del_i->Enabled = false;
        m_emi_del_j->Enabled = false;
        m_emi_arb_a->Enabled = false;
        m_emi_arb_b->Enabled = false;
        m_emi_arb_c->Enabled = false;
        m_emi_arb_d->Enabled = false;
        m_emi_arb_e->Enabled = false;
        m_emi_arb_f->Enabled = false;
        m_emi_arb_g->Enabled = false;
        m_emi_arb_h->Enabled = false;
        m_emi_arb_i->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall Text_mem1Form::m_nfi_OptionChange(TObject *Sender)
{
	if( 1 == m_nfi_Option->ItemIndex ) {
		m_nand_acccon->Visible = false;
	}
	else {
		m_nand_acccon->Visible = true;
	}
}
//---------------------------------------------------------------------------

