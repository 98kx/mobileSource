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
*  main.h
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  main window header file 
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <SyncObjs.hpp>
#include "CGAUGES.h"
#include <Graphics.hpp>
#include <vector>
#include <list>
#include "Trayicon.h"

#include <stdio.h>
#include <string>

#include "flashtool_handle.h"
#include "flashtool.h"
#include "tboot_1.h"
#include "CGAUGES.h"
#include "param.h"
#include <CheckLst.hpp>

#include "Setting.h"


//---------------------------------------------------------------------------
class DownloadBinFile
{
    public:
	    DownloadBinFile() {
		    m_Filepath = "";
		    m_Enable = false;
	    }

	    ~DownloadBinFile() { }

	    AnsiString		m_Filepath;
	    bool			m_Enable;
};

class DownloadSet
{
    public:
    DownloadSet() {
        m_Scatter = ""; 
        m_BinFiles.clear();
    }
    ~DownloadSet() { }

    bool operator==(const DownloadSet &r_value);

    AnsiString				m_Scatter;
    std::vector<DownloadBinFile>	m_BinFiles;
};

typedef std::list<DownloadSet>		DownloadHistoryList;

typedef std::list<DownloadSet>          MultiLoadHistoryList;

#define MAX_DOWNLOAD_HISTORY	10

class MultiLoadNode
{

    public:

	    MultiLoadNode()	{ reset(); }
	    ~MultiLoadNode() { }

	    void reset() {
		    m_scat_file= NULL;
		    m_dl_handle = NULL;
		    m_param.reset();
	    }

	    AnsiString				m_scat_file;
	    DL_HANDLE_T				m_dl_handle;
	    ParamDownloadConfig_T	m_param;


};

//---------------------------------------------------------------------------
class TMainForm : public TForm
{

     friend class tboot_1;

__published:	// IDE-managed Components
    TMainMenu *m_Main;
    TMenuItem *File1;
    TMenuItem *Options1;
    TMenuItem *Run1;
    TMenuItem *Download1;
    TActionList *ActionList1;
    TMenuItem *Open1;
    TMenuItem *N1;
    TMenuItem *Exit1;
    TMenuItem *BaudRate1;
    TMenuItem *mi_115200;
    TMenuItem *mi_38400;
    TMenuItem *mi_19200;
    TMenuItem *mi_9600;
    TMenuItem *mi_57600;
    TOpenDialog *OpenLdMapDialog;
    TSaveDialog *SaveOptionDialog;
    TImageList *ImageList1;
    TAction *FileOpenDownloadAgent;
    TAction *FileSave_TestModeLog;
    TAction *Download;
    TMenuItem *N2;
	TMenuItem *mi_EMI_Config;
    TImageList *ImageList2;
    TOpenDialog *OpenDownloadAgentDialog;
    TMenuItem *Openlinkmapfile1;
    TAction *FileOpenLinkMap;
    TAction *View_toolbar;
    TAction *View_StatusBar;
    TAction *Exit;
    TOpenDialog *OpenSingleRomFile;
    TStatusBar *sb_Main;
    TAction *Readback;
    TMenuItem *ReadBack1;
    TPageControl *pc_mainframe;
    TTabSheet *ts_Download;
    TTabSheet *ts_Readback;
    TListView *ListView1;
    TToolBar *tb_ReadBack;
    TToolButton *tb_AddReadBackFile;
    TImageList *ImageList3;
    TToolButton *tb_DeleteReadBackFile;
    TListView *lv_ReadBack;
    TSaveDialog *sd_ReadBack;
    TMenuItem *mi_460800;
    TMenuItem *mi_230400;
    TTimer *Timer_Download;
    TCGauge *CGauge1;
    TToolBar *tb_Download;
    TToolButton *tb_OpenDownloadAgent;
    TToolButton *tb_ScatterLoading;
    TToolButton *ToolButton10;
    TToolButton *tb_ReadBack2;
    TMenuItem *Help1;
    TMenuItem *About1;
    TEdit *ed_DA_File;
    TEdit *ed_LinkMapFile;
    TMenuItem *mi_FormatFDM;
    TLabel *lb_DA_File;
    TLabel *lb_LinkMapFile;
	TMenuItem *MCUClock1;
	TMenuItem *MCU_13MHz;
	TMenuItem *MCU_26MHz;
	TMenuItem *MCU_52MHz;
	TMenuItem *BaseBandChip1;
	TMenuItem *BBCHIP_MT6205;
	TMenuItem *BBCHIP_MT6205B;
	TAction *action_ToggleDebugDA;
	TRichEdit *DebugDAWindow;
	TMenuItem *BBCHIP_MT6218;
	TAction *action_ToggleTrace;
    TToolButton *tb_FormatButton;
	TAction *action_ForceStop;
	TAction *action_ToggleSkipBootRom;
	TMenuItem *mi_921600;
	TMenuItem *BBCHIP_MT6218B;
	TAction *action_PurgeDebugLog;
	TMenuItem *PacketLength1;
	TMenuItem *PL_1;
	TMenuItem *PL_2;
	TMenuItem *PL_3;
	TMenuItem *PL_4;
	TMenuItem *PL_5;
	TMenuItem *DCOMPort;
	TMenuItem *BBCHIP_MT6219;
	TMenuItem *m_CheckECOVersion;
	TAction *action_AdvanceOption;
	TToolButton *RB_StopButton;
	TMenuItem *m_CheckTgtResourceLayout;
	TMenuItem *m_AutoLoadResByScat;
	TMenuItem *m_DA_Option;
	TMenuItem *m_ResourceOption;
	TMenuItem *m_BBChipOption;
	TMenuItem *m_BRomStartCmdRetryCount;
	TMenuItem *BBCHIP_MT6217;
	TMenuItem *m_PullDownGPIO17;
	TMenuItem *m_COM_Port_Timeout;
	TMenuItem *m_Transmission_Option;
	TMenuItem *m_SpeedUpBootROMBaudrate;
    TSaveDialog *Dialog_SaveTestModeLog;
    TMenuItem *m_SaveTestModeLog;
    TMenuItem *BBCHIP_MT6228;
    TMenuItem *m_CompareProjectId;
    TTimer *m_FakeProgressTimer;
    TMenuItem *m_EnableNFICS1;
    TMenuItem *m_Menu_OperationMethod;
    TMenuItem *m_Menu_OPM_NOR;
    TMenuItem *m_Menu_OPM_NAND;
    TMenuItem *m_Menu_OPM_NFB;
    TStatusBar *m_HWStatus;
    TMenuItem *BBCHIP_MT6227;
    TMenuItem *BBCHIP_MT6229;
    TMenuItem *BBCHIP_MT6226;
    TMenuItem *N3;
    TMenuItem *m_DL_History_0;
    TMenuItem *m_DL_History_1;
    TMenuItem *m_DL_History_2;
    TMenuItem *m_DL_History_3;
    TMenuItem *m_DL_History_4;
    TMenuItem *m_DL_History_5;
    TMenuItem *m_DL_History_6;
    TMenuItem *m_DL_History_7;
    TMenuItem *m_DL_History_8;
    TMenuItem *m_DL_History_9;
    TMenuItem *BBCHIP_MT6226M;
    TTreeView *TreeView1;
    TImageList *ImageList4;
    TPopupMenu *PopupMenu1;
    TMenuItem *AddaLoad1;
    TMenuItem *DeleteaLoad1;
    TEdit *ed_AUTH_File;
    TLabel *lb_AUTH_File;
    TToolButton *tb_OpenAuthFile;
    TToolButton *tb_DL_StopButton;
    TToolButton *tb_DL_StartButton;
    TOpenDialog *m_OpenAuthFileDialog;
	TMenuItem *m_AutoBBCHIP;
	TMenuItem *m_AutoExtClock;
    TTabSheet *ts_Paremeter;
    TTabSheet *ts_FileManagement;
    TGroupBox *GroupBox1;
    TGroupBox *gb_sec_setting;
    TGroupBox *gb_cust;
    TGroupBox *gb_otp;
    TCheckBox *cb_sec_setting;
    TSplitter *Splitter1;
    TCheckBox *cb_cust;
    TCheckBox *cb_sec_ro;
    TCheckBox *cb_otp;
    TBitBtn *bb_ParameterButton;
    TBitBtn *bb_StopButton;
    TGroupBox *gb_sec_ro;
	TListView *m_ParamListView;
    TOpenDialog *m_OpenFileDialog;
    TPanel *Panel1;
    TRadioButton *rb_sec_setting_Write;
    TRadioButton *rb_cust_Write;
    TRadioButton *rb_cust_Read;
    TRadioButton *rb_sec_ro_Write;
    TRadioButton *rb_sec_ro_Read;
    TRadioButton *rb_otp_Write;
    TRadioButton *rb_otp_Read;
    TButton *bt_otp_setting;
    TEdit *ed_otp_Write;
    TEdit *ed_otp_Read;
    TLabel *lb_otp_Write;
    TLabel *lb_otp_Read;
    TButton *bt_sec_setting;
    TLabel *lb_sec_setting_Write;
    TEdit *ed_sec_setting_Write;
    TLabel *lb_sec_ro_Write;
    TLabel *lb_sec_ro_Read;
    TEdit *ed_sec_ro_Write;
    TEdit *ed_sec_ro_Read;
    TLabel *lb_cust_Write;
    TLabel *lb_cust_Read;
    TEdit *ed_cust_Write;
    TEdit *ed_cust_Read;
    TButton *bt_sec_ro_setting;
    TButton *bt_cust_setting;
    TListView *m_ReadParamListView;
	TButton *m_FS_StartButton;
	TButton *m_FS_StopButton;
    TGroupBox *GroupBox2;
    TEdit *ed_ExceptionLog;
    TGroupBox *GroupBox3;
    TButton *bt_SaveExceptionLog;
    TEdit *ed_SaveExceptionLog;
	TMenuItem *BBCHIP_MT6230;
	TMenuItem *BBCHIP_MT6225;
    TMenuItem *BBCHIP_MT6223;
    TMenuItem *BBCHIP_MT6227D;
    TTabSheet *ts_RomWriter;
    TButton *m_RW_StopButton;
    TGroupBox *GroupBox4;
    TEdit *ed_SaveExtractNFB;
    TButton *m_RW_StartButton;
    TButton *bt_SaveExtractNFB;
    TMenuItem *Window1;
    TMenuItem *m_Menu_Window_CheckParameterPage;
    TMenuItem *m_Menu_Window_CheckFileManagement;
    TMenuItem *m_Menu_Window_CheckROMWriter;
    TMenuItem *m_Menu_Window_CheckEnableBoot;
    TTabSheet *ts_MemoryTest;
    TGroupBox *GroupBox5;
    TGroupBox *GroupBox6;
    TCheckBox *cb_MT_RAM_Data_Bus_Test;
    TCheckBox *cb_MT_RAM_Addr_Bus_Test;
    TCheckBox *cb_MT_RAM_Pattern_Test;
    TCheckBox *cb_MT_RAM_Inc_Dec_Test;
    TGroupBox *GroupBox7;
    TCheckBox *cb_MT_NOR_Addr_Bus_Test;
    TCheckBox *cb_MT_NOR_Pattern_Test;
    TButton *bt_MT_SelectAll;
    TButton *m_MT_StartButton;
    TButton *m_MT_StopButton;
    TRichEdit *m_MT_ResultRichEditor;
    TPanel *Panel2;
    TPanel *Panel3;
    TGroupBox *GroupBox8;
    TCheckBox *cb_MT_NAND_Pattern_Test;
    TMenuItem *mi_USBDownloadReadback;
    TMenuItem *BBCHIP_MT6226D;
    TMenuItem *BBCHIP_MT6223P;
    TLabel *Label3;
    TGroupBox *gb_gps;
    TRadioButton *rb_gps_Write;
    TRadioButton *rb_gps_Read;
    TButton *bt_gps_setting;
    TLabel *lb_gps_Write;
    TEdit *ed_gps_Write;
    TEdit *ed_gps_Read;
    TLabel *lb_gps_Read;
    TCheckBox *cb_gps;
    TMenuItem *DownloadStyle;
    TCheckBox *NORCheckBox;
    TCheckBox *NANDCheckBox;
    TCheckBox *RAMCheckBox;
    TMenuItem *BBCHIP_MT6235;
    TMenuItem *BBCHIP_MT6238;
    TMenuItem *BBCHIP_MT6239;
    TMenuItem *BBCHIP_MT6268A;
    TMenuItem *BBCHIP_MT6251T;
    TMenuItem *BBCHIP_MT6253T;
    TMenuItem *BackupandRestore;
        TMenuItem *BackupRestore;
        TMenuItem *Restore;
    TMenuItem *RestoreOnly;
    TMenuItem *NoAction;
        TMenuItem *UploadSetting;
    TMenuItem *NoUpload;
    TMenuItem *UploadOnly;
        TMenuItem *Upload;
    TMenuItem *BBCHIP_MT6268B;
    TMenuItem *DAStartAddress;
        TAction *action_RecordCommand;
        TMenuItem *MultiLoadSetting;
        TMenuItem *NoMultiLoad;
        TMenuItem *MultiLoad;
        TMenuItem *MultiLoadWithMMAA;
    TSaveDialog *m_SaveFileDialog;
    TMenuItem *N4;
    TMenuItem *Preservebackupdata;
    TMenuItem *UseCOMPortNumberToStoreBackupData;
    TImage *imgBLSupportUSB;
    TImage *imgBROMSupportUSB;
        TTabSheet *ts_EnableBoot;
        TGroupBox *GroupBox9;
        TButton *bt_boot_cert_file_open;
        TEdit *ed_boot_cert_file_path;
        TLabel *Label1;
        TButton *bt_start_enable_boot;
        TButton *bt_stop_enable_boot;
        TLabel *lb_SCERT_File;
        TEdit *ed_SCERT_File;
        TToolButton *tb_OpenSCertFile;
        TOpenDialog *m_OpenSCertFileDialog;
        TMenuItem *mi_DownloadWithoutBattery;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall Download1Click(TObject *Sender);
    void __fastcall FileOpenDownloadAgentExecute(TObject *Sender);
    void __fastcall FileOpenLinkMapExecute(TObject *Sender);
    void __fastcall View_toolbarExecute(TObject *Sender);
    void __fastcall View_StatusBarExecute(TObject *Sender);
    void __fastcall mi_921600Click(TObject *Sender);
    void __fastcall br_460800Execute(TObject *Sender);
    void __fastcall br_230400Execute(TObject *Sender);
    void __fastcall br_115200Execute(TObject *Sender);
    void __fastcall br_57600Execute(TObject *Sender);
    void __fastcall br_38400Execute(TObject *Sender);
    void __fastcall br_19200Execute(TObject *Sender);
    void __fastcall br_9600Execute(TObject *Sender);
    void __fastcall mi_DEBUGClick(TObject *Sender);
    void __fastcall ExitExecute(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall mi_ViewClick(TObject *Sender);
    void __fastcall ListView1_oldClick(TObject *Sender);
    void __fastcall ReadbackExecute(TObject *Sender);
    void __fastcall tb_AddReadBackFileClick(TObject *Sender);
    void __fastcall tb_DeleteReadBackFileClick(TObject *Sender);
    void __fastcall Timer_DownloadTimer(TObject *Sender);
    void __fastcall testOKWnd1Click(TObject *Sender);
    void __fastcall About1Click(TObject *Sender);
    void __fastcall mi_FormatFDMClick(TObject *Sender);
	void __fastcall action_ToggleDebugDAExecute(TObject *Sender);
	void __fastcall action_ToggleTraceExecute(TObject *Sender);
	void __fastcall action_ForceStopExecute(TObject *Sender);
	void __fastcall action_ToggleSkipBootRomExecute(TObject *Sender);
    void __fastcall action_PurgeDebugLogExecute(TObject *Sender);
    void __fastcall action_AdvanceOptionExecute(TObject *Sender);
	void __fastcall tb_FormatButtonClick(TObject *Sender);
	void __fastcall PL_1Click(TObject *Sender);
	void __fastcall PL_2Click(TObject *Sender);
	void __fastcall PL_3Click(TObject *Sender);
	void __fastcall PL_4Click(TObject *Sender);
	void __fastcall PL_5Click(TObject *Sender);
	void __fastcall Options1Click(TObject *Sender);
	void __fastcall COMPortSubMenuClick(TObject *Sender);
	void __fastcall ListView1Click(TObject *Sender);
	void __fastcall ListView1Change(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall ListView1Changing(TObject *Sender, TListItem *Item, TItemChange Change, bool &AllowChange);
	void __fastcall ListView1DblClick(TObject *Sender);
	void __fastcall lv_ReadBackChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall lv_ReadBackChanging(TObject *Sender, TListItem *Item, TItemChange Change, bool &AllowChange);
	void __fastcall lv_ReadBackClick(TObject *Sender);
	void __fastcall lv_ReadBack_oldDblClick(TObject *Sender);
    void __fastcall lv_ReadBackDblClick(TObject *Sender);
    void __fastcall lv_ReadBackKeyPress(TObject *Sender, char &Key);
    void __fastcall lv_ReadBackKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ListView1InfoTip(TObject *Sender, TListItem *Item, AnsiString &InfoTip);
	void __fastcall m_CheckECOVersionClick(TObject *Sender);
	void __fastcall tb_DL_StopButtonClick(TObject *Sender);
	void __fastcall RB_StopButtonClick(TObject *Sender);
	void __fastcall m_CheckTgtResourceLayoutClick(TObject *Sender);
	void __fastcall m_AutoLoadResByScatClick(TObject *Sender);
	void __fastcall m_BRomStartCmdRetryCountClick(TObject *Sender);
	void __fastcall m_PullDownGPIO17Click(TObject *Sender);
	void __fastcall mi_EMI_ConfigClick(TObject *Sender);
	void __fastcall m_COM_Port_TimeoutClick(TObject *Sender);
	void __fastcall m_SpeedUpBootROMBaudrateClick(TObject *Sender);
    void __fastcall FileSave_TestModeLogExecute(TObject *Sender);
    void __fastcall m_CompareProjectIdClick(TObject *Sender);
    void __fastcall m_FakeProgressTimerTimer(TObject *Sender);
    void __fastcall m_EnableNFICS1Click(TObject *Sender);
    void __fastcall m_Menu_OPM_NORClick(TObject *Sender);
    void __fastcall m_Menu_OPM_NANDClick(TObject *Sender);
    void __fastcall m_Menu_OPM_NFBClick(TObject *Sender);
    void __fastcall BBCHIP_MT6205BClick(TObject *Sender);
    void __fastcall BBCHIP_MT6217Click(TObject *Sender);
	void __fastcall BBCHIP_MT6218BClick(TObject *Sender);
	void __fastcall BBCHIP_MT6219Click(TObject *Sender);
    void __fastcall BBCHIP_MT6228Click(TObject *Sender);
    void __fastcall BBCHIP_MT6227Click(TObject *Sender);
    void __fastcall BBCHIP_MT6229Click(TObject *Sender);
    void __fastcall BBCHIP_MT6226Click(TObject *Sender);
    void __fastcall BBCHIP_MT6230Click(TObject *Sender);
	void __fastcall BBCHIP_MT6225Click(TObject *Sender);
    void __fastcall BBCHIP_MT6223Click(TObject *Sender);
    void __fastcall BBCHIP_MT6227DClick(TObject *Sender);
    void __fastcall BBCHIP_MT6226DClick(TObject *Sender);
    void __fastcall BBCHIP_MT6226MClick(TObject *Sender);
    void __fastcall BBCHIP_MT6223PClick(TObject *Sender);
    void __fastcall BBCHIP_MT6235Click(TObject *Sender);
    void __fastcall BBCHIP_MT6238Click(TObject *Sender);
    void __fastcall BBCHIP_MT6239Click(TObject *Sender);
    void __fastcall BBCHIP_MT6268AClick(TObject *Sender);
    void __fastcall BBCHIP_MT6268BClick(TObject *Sender);
    void __fastcall BBCHIP_MT6251TClick(TObject *Sender);
    void __fastcall BBCHIP_MT6253TClick(TObject *Sender);
    void __fastcall File1Click(TObject *Sender);
	void __fastcall OnDownloadHistoryClick(TObject *Sender);

    void __fastcall AddaLoad1Click(TObject *Sender);
    bool __fastcall AddaLoadInternalMain(MultiLoadNode *p_node);
    bool __fastcall OpenAuthAddIntoDLList(MultiLoadNode *p_node, const AnsiString asCustomName, const AnsiString asFlashName, const AnsiString asFlashID, int retFlashName);
    void __fastcall OpenAuthAddIntoTreeView(MultiLoadNode *p_node, const AnsiString asProjectFlashID);
    void __fastcall OpenAuthAddIntoMultiHistory(MultiLoadNode *p_node);

    void __fastcall DeleteaLoad1Click(TObject *Sender);
    void __fastcall tb_OpenAuthFileClick(TObject *Sender);
	void __fastcall m_AutoBBCHIPClick(TObject *Sender);
	void __fastcall m_AutoExtClockClick(TObject *Sender);
	void __fastcall m_ParamListViewClick(TObject *Sender);
	void __fastcall m_ParamListViewChange(TObject *Sender, TListItem *Item, TItemChange Change);
	void __fastcall m_ParamListViewChanging(TObject *Sender, TListItem *Item, TItemChange Change, bool &AllowChange);
	void __fastcall m_ParamListViewDblClick(TObject *Sender);
    void __fastcall tb_ParameterButtonClick(TObject *Sender);
    void __fastcall cb_otpClick(TObject *Sender);
    void __fastcall cb_sec_settingClick(TObject *Sender);
    void __fastcall cb_sec_roClick(TObject *Sender);
    void __fastcall cb_custClick(TObject *Sender);
    void __fastcall bt_otp_settingClick(TObject *Sender);
    void __fastcall bt_sec_settingClick(TObject *Sender);
    void __fastcall bt_sec_ro_settingClick(TObject *Sender);
    void __fastcall bt_cust_settingClick(TObject *Sender);
    void __fastcall bb_ParameterButtonClick(TObject *Sender);
    void __fastcall bb_StopButtonClick(TObject *Sender);
    void __fastcall m_ReadParamListViewChange(TObject *Sender, TListItem *Item, TItemChange Change);
    void __fastcall m_ReadParamListViewChanging(TObject *Sender, TListItem *Item, TItemChange Change, bool &AllowChange);
    void __fastcall m_ReadParamListViewClick(TObject *Sender);
    void __fastcall m_ReadParamListViewDblClick(TObject *Sender);
    void __fastcall m_FS_StartButtonClick(TObject *Sender);
    void __fastcall m_FS_StopButtonClick(TObject *Sender);
    void __fastcall bt_SaveExceptionLogClick(TObject *Sender);
    void __fastcall bt_SaveExtractNFBClick(TObject *Sender);
    void __fastcall m_RW_StartButtonClick(TObject *Sender);
    void __fastcall m_RW_StopButtonClick(TObject *Sender);
    void __fastcall m_Menu_Window_CheckParameterPageClick(TObject *Sender);
    void __fastcall m_Menu_Window_CheckFileManagementClick(TObject *Sender);
    void __fastcall m_Menu_Window_CheckROMWriterClick(TObject *Sender);
    void __fastcall m_Menu_Window_CheckEnableBootClick(TObject *Sender);
    void __fastcall bt_MT_SelectAllClick(TObject *Sender);
    void __fastcall m_MT_StartButtonClick(TObject *Sender);
    void __fastcall m_MT_StopButtonClick(TObject *Sender);
    void __fastcall mi_USBDownloadReadbackClick(TObject *Sender);
    void __fastcall mi_USBDownloadReadbackClickInternal(bool bUSBChecked);
    void __fastcall cb_gpsClick(TObject *Sender);
    void __fastcall bt_gps_settingClick(TObject *Sender);
    void __fastcall DownloadStyleClick(TObject *Sender);
    void __fastcall RAMCheckBoxClick(TObject *Sender);
    void __fastcall NORCheckBoxClick(TObject *Sender);
    void __fastcall NANDCheckBoxClick(TObject *Sender);
    void __fastcall MCU_13MHzClick(TObject *Sender);
    void __fastcall MCU_26MHzClick(TObject *Sender);
    void __fastcall MCU_52MHzClick(TObject *Sender);
    void __fastcall DAStartAddressClick(TObject *Sender);
    void __fastcall sb_MainDrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect);
    void __fastcall cb_MT_RAM_Data_Bus_TestClick(TObject *Sender);
    void __fastcall cb_MT_RAM_Addr_Bus_TestClick(TObject *Sender);
    void __fastcall cb_MT_RAM_Pattern_TestClick(TObject *Sender);
    void __fastcall cb_MT_RAM_Inc_Dec_TestClick(TObject *Sender);
    void __fastcall cb_MT_NOR_Addr_Bus_TestClick(TObject *Sender);
    void __fastcall cb_MT_NOR_Pattern_TestClick(TObject *Sender);
    void __fastcall cb_MT_NAND_Pattern_TestClick(TObject *Sender);
    void __fastcall NoActionClick(TObject *Sender);
    void __fastcall BackupRestoreClick(TObject *Sender);
    void __fastcall RestoreClick(TObject *Sender);
    void __fastcall RestoreOnlyClick(TObject *Sender);
    void __fastcall NoUploadClick(TObject *Sender);
    void __fastcall UploadClick(TObject *Sender);
    void __fastcall UploadOnlyClick(TObject *Sender);
    void __fastcall action_RecordCommandExecute(TObject *Sender);
        void __fastcall MultiLoadClick(TObject *Sender);
        void __fastcall NoMultiLoadClick(TObject *Sender);
        void __fastcall MultiLoadWithMMAAClick(TObject *Sender);
    void __fastcall TreeView1MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall TreeView1Change(TObject *Sender, TTreeNode *Node);
        void __fastcall ed_LinkMapFileEnter(TObject *Sender);
        void __fastcall ed_DA_FileEnter(TObject *Sender);
        void __fastcall ed_AUTH_FileChange(TObject *Sender);
    void __fastcall PreservebackupdataClick(TObject *Sender);
    void __fastcall UseCOMPortNumberToStoreBackupDataClick(
          TObject *Sender);
        void __fastcall bt_boot_cert_file_openClick(TObject *Sender);
        void __fastcall bt_start_enable_bootClick(TObject *Sender);
        void __fastcall bt_stop_enable_bootClick(TObject *Sender);
        void __fastcall ed_SCERT_FileChange(TObject *Sender);
        void __fastcall tb_OpenSCertFileClick(TObject *Sender);
        void __fastcall mi_DownloadWithoutBatteryClick(TObject *Sender);



private:	// User declarations

   int uiMultiBootConfigAddr;
   unsigned short m_COMPortList[255];
   unsigned short m_COMPortCount;
   TCriticalSection  *m_pUpdateListLock;

protected:

   AnsiString asBin;
   AnsiString ScatBaseDir;
   AnsiString ROMBaseDir;
   AnsiString TestModeLogFile;

public:		// User declarations

    __fastcall TMainForm(TComponent* Owner);


    tboot_1 *p1;

    void uncheckAllBaudRate ( void );
    void updateStatusBar ( void );
    void updateMenuItem ( void );

    bool Parse(MultiLoadNode *p_node, AnsiString asMapFileName);

    bool FileOpenDownloadAgentExecute ( void );
    bool FileOpenLinkMapExecute ( MultiLoadNode  *p_node );
    bool FileSaveTestModeLogExecute ( void );
    bool OpenAuthFile ( void );
    bool OpenSCertFile ( void );

    void OpenAuthMultiLoad(void);
    bool OpenAuthMultiLoadMain(MultiLoadNode *p_node);
    int  OpenAuthGetFlashIdName(MultiLoadNode *p_node, char *custom_name, char *flash_ID, char *flash_name, char *project_flashID);
    bool OpenAuthUpdateDLList(MultiLoadNode *p_node, const AnsiString asCustomName, const AnsiString asFlashName, const AnsiString asFlashID, int retFlashName);
    void OpenAuthUpdateDLNodeList(MultiLoadNode *p_node);
    void OpenAuthUpdateMultiHistory(MultiLoadNode *p_node);

    int  UpdateDA();

    void DisableFileMenu();
    void EnableFileMenu();
    void DisableDownloadButton( void );
    void EnableDownloadButton( void );
	void DisableReadbackButton( void );
	void EnableReadbackButton( void );
	void DisableFormatButton( void );
	void EnableFormatButton( void );
	void DisableStopButton( void );
	void EnableStopButton( void );
	//---------------------------------------------------------------------------
    void EnableDASelectButton ( void );
    void DisableDASelectButton ( void );
    void EnableScatterSelectButton ( void );
    void DisableScatterSelectButton ( void );
    void EnableAuthSelectButton ( void );
    void DisableAuthSelectButton ( void );
    void EnableSCertSelectButton ( void );
    void DisableSCertSelectButton ( void );
    void EnableRBAddButton ( void );
    void DisableRBAddButton ( void );
    void EnableRBRemoveButton ( void );
    void DisableRBRemoveButton ( void );
    void EnableBootCertSelectButton ( void );
    void DisableBootCertSelectButton ( void );
	void UpdateCaption( void );
    void ResetBackupRestoreItem( void );
    void ResetUploadItem( void );

    void ResetMultiLoadItem(void);

    void UpdatePacketLength(unsigned short pkt_length);

	void RedrawDownloadList (MultiLoadNode  *p_node, bool bUpdateParamOnly);
	void RedrawDownloadCheckBox ( void );
	void RedrawReadbackList ( void );

	bool LoadByDownloadSet(const DownloadSet &dl_set, MultiLoadNode  *p_node);
        bool LoadByMultiLoadSet(MultiLoadHistoryList *multiLoadList);

	void HWStatusBarClear ( void );
	void HWStatusBarUpdate(const DA_REPORT_T *p_da_report, const char *p_nor_flash_hint, const char *p_nand_flash_hint, const char *m_hw_detection_report);
	static int LoadFileInfo(const char *pathname, unsigned int *p_nFileLength, long *p_last_modify_time);
	static int LoadFile(const char *pathname, unsigned char **pp_buf, unsigned int *p_nFileLength, long *p_last_modify_time);
    int SaveFile(const char *pathname, unsigned char *p_buf, unsigned int nFileLength );

	void LoadSecRO( void );
	void LoadCustPara( void );
	void LoadGPS( void );

    void WriteBackFlashToolINI( void );
    void ReadFlashToolINI( void );

    void start_DL( bool bStart_DL );
    void start_RB( bool b_start_RB );

    bool BootROMUSBSupported(DL_HANDLE_T &dl_handle);

    void CreateConfigFile();

    void Create_DL_ConfigFile(const std::wstring &filename);
    void Create_FMT_ConfigFile(const std::wstring &filename);
    void Create_RtFD_ConfigFile(const std::wstring &filename);
    void Create_DL_FMT_ConfigFile(const std::wstring &filename);
    void Create_DL_RtFD_ConfigFile(const std::wstring &filename);
    void Create_BK_DL_RS_ConfigFile(const std::wstring &filename);
    void Create_BK_DL_FMT_RS_ConfigFile(const std::wstring &filename);
    void Create_BK_DL_RtFD_RS_ConfigFile(const std::wstring &filename);
    void Create_BK_FMT_RS_ConfigFile(const std::wstring &filename);
    void Create_BK_RtFD_RS_ConfigFile(const std::wstring &filename);

    ConsoleMode::GeneralSetting CreateGeneralSetting();
    ConsoleMode::DownloadSetting CreateDownloadSetting();
    ConsoleMode::FormatSetting CreateFormatSetting();
    ConsoleMode::ResetToFactoryDefaultSetting CreateRtFDSetting(
                                                    bool enableWatchdogTimer);
    ConsoleMode::BackupSetting CreateBackupSetting(bool enableWatchdogTimer);
    ConsoleMode::RestoreSetting CreateRestoreSetting(bool enableWatchdogTimer);

    void GetBackupRestoreStatus(bool* isPreserveBackupData, 
                                bool* isUseComPortStoreData,
                                AnsiString* restore_backup_path);

    bool CreateROMEntries(const AnsiString &configFilePath,
                          bool autoLoadROMFiles,
                          DL_HANDLE_T downloadHandle);
};
//---------------------------------------------------------------------------
typedef enum {
    DISABLE_FORMAT = 0,
    RESET_TO_FACTORY,
    FAT_FORMAT_OPTION,
    MANUAL_FORMAT_OPTION,
    EMMC_MANUAL_FORMAT_OPTION,
    REGION_FROMAT_OPTION,
    TOTAL_FORMAT_OPTION
} FormatOption_E;

typedef enum {
	NOR_OPERATION = 0,	
	NFB_OPERATION,
	EMMC_OPERATION      // TODO:: review code for this EMMC extend
} OptionMethod_E;

typedef enum {
	FLASHTOOL_DOWNLOAD = 0,
    FLASHTOOL_READBACK,
    FLASHTOOL_FORMAT,
    FLASHTOOL_PARAMETER,
    FLASHTOOL_API,
    FLASHTOOL_ROMWRITER,
    FLASHTOOL_MEMORYTEST,
    FLASHTOOL_ENABLE_BOOT
} FLASHTOOL_FUNCTION;

typedef enum {
    LV_NFB_BOOTLOADER = 0,
    LV_NFB_EXT_BOOTLOADER,
    LV_NFB_DL_ROM,
    LV_NOR_BOOTLOADER,
    LV_NOR_DL_ROM,
    LV_OTP,
    LV_FTCFG,
    LV_SEC_RO,
    LV_CUST_PARA,
    LV_GPS_ROM
} ListViewItemType_E;

typedef enum {
	FLASHTOOL = 1,
	META      = 2,
	GPS       = 3,
} ERROR_CODE_MODE_E;

typedef enum {
    NOACTION      = 0,
    BACKUPRESTORE = 1,
    RESTORE       = 2,
    RESTOREONLY   = 3,
} BACKUP_RESTORE_STATE_E;

typedef enum {
    NOUPLOAD   = 0,
    UPLOAD     = 1,
    UPLOADONLY = 2,
} UPLOAD_STATE_E;

typedef enum{
        NOMULTILOAD = 0,
        MULTILOAD   = 1,
        MULTILOADWITHMMAA = 2,
}MULTILOAD_STATE_E;

typedef enum{
        FLASHTOOL_WARNING_NONE = 0,
        FLASHTOOL_WARNING_BL_NOT_UPDATED   = 1,        
}FLASHTOOL_FINISH_WARNING_E;



//---------------------------------------------------------------------------

typedef struct {
	bool	        m_bAuto;
	unsigned int    m_Config;
} EMI_Config_S;

typedef struct {
    // NAND NFI Setting
	bool	        m_bAuto;
	unsigned int	m_nand_acccon;

    // DRAM EMI Setting
    DRAM_SETTING    m_dram_cfg;
} DRAM_NFI_Setting;

//---------------------------------------------------------------------------
extern int g_stopflag;
extern bool g_HasOngoingOperation;

extern PACKAGE TMainForm *MainForm;
extern UART_BAUDRATE  BAUD_RATE;

extern EXT_CLOCK	g_EXT_CLOCK;
extern BBCHIP_TYPE	g_BBCHIP_TYPE;

extern FormatOption_E   	    g_FormatAfterDownloadOption;
extern FormatOption_E	        g_FormatOption;
extern OptionMethod_E	        g_OperationMethod;
extern NUTL_EraseFlag_E	        g_EraseFlag;
extern ERROR_CODE_MODE_E        g_error_code_mode;
extern FLASHTOOL_FINISH_WARNING_E    g_finish_warning_code;
extern BACKUP_RESTORE_STATE_E   g_backup_restore_state;
extern UPLOAD_STATE_E           g_upload_state;
extern MULTILOAD_STATE_E            g_multiload_state;

extern bool g_bSkipBootRom;
extern bool g_bCheckECOVer;
extern bool g_bCheckTgtResourceLayout;
extern bool g_bPullDownGPIO17;
extern bool g_SpeedUpBootROMBaudrate;
extern bool	g_FormatValidation;
extern bool g_bEnableAdvanceOption;
extern bool g_bParameterPage;
extern bool g_bDownloadStyleSequential;
extern bool	g_bNFI_SelectCS1;
extern bool g_bUsbDLRB;
extern bool	g_bDebugDA;
extern AnsiString g_asNormalModeDAPath;

extern unsigned short g_PacketLength;

extern unsigned int COM_PORT;
extern unsigned int g_ulFormatStartAddr;
extern unsigned int g_ulFormatLength;
extern unsigned int g_ulNandFormatStartAddr;
extern unsigned int g_ulNandFormatLength;
extern unsigned int g_MaxBromStartCmdRetryCount;
extern unsigned int g_COM_Read_Timeout;
extern unsigned int g_COM_Write_Timeout;
extern unsigned int g_CostTimeSec;
extern unsigned int g_ProgressTimeSec;
extern unsigned int g_DisplayFinishedBytes;

extern bool     g_bMMAAEnable;
extern bool     g_bButtonSentry;


extern unsigned char g_Baudrate_FullSync_Count;

//extern AnsiString           g_BackupCalibrationDataFolderPath;
//extern AnsiString			g_BackUpDirNumber;
extern EMI_Config_S         g_EMI_Config[8];
extern DRAM_NFI_Setting     g_EMI_NFI_Setting;
extern MultiLoadNode	    g_DL_HANDLE_NODE;
extern std::vector<MultiLoadNode> g_DL_HANDLE_LIST_nodes;
extern MultiLoadNode *	    g_pCurNode;
extern MultiLoadNode *	    g_pLastNode;
extern DL_HANDLE_LIST_T	    g_DL_HANDLE_LIST;
extern DA_HANDLE_T	        g_DA_HANDLE;
extern RB_HANDLE_T	        g_RB_HANDLE;
extern TTreeNode            *g_rootLoad;
extern AUTH_HANDLE_T        g_AUTH_HANDLE;
extern bool                 g_AUTH_HANDLE_Loaded;
extern SCERT_HANDLE_T       g_SCERT_HANDLE;
extern bool                 g_SCERT_HANDLE_Loaded;
extern FLASHTOOL_API_HANDLE_T   g_ft_handle;
extern ParamDownloadConfig_T    g_ParamDownloadConfig;
extern ParamReadbackConfig_T    g_ParamReadbackConfig;
extern ParamReadbackConfig_T    g_ReadbackPage_Param;
extern ParameterType_T          g_ParameterType;
extern bool g_BootCert_Enabled;
extern BootCert_T g_BootCert;
extern FLASHTOOL_FUNCTION	    g_flashtool_function;
extern BACKUP_RESTORE_STATE_E   g_backup_restore_state;

class button_sentry
{
    public:

	   	button_sentry();
        ~button_sentry();
};

class CS_Sentry
{
    public:
	    CS_Sentry(TCriticalSection  *cs):m_cs(cs) { m_cs->Acquire(); }
        ~CS_Sentry() { m_cs->Release(); }

    private:
	    TCriticalSection	*m_cs;
};

class file_open_sentry
{

    public:
	    file_open_sentry ( const char *filepath, const char *mode, FILE *&fp, bool bIsDebugOn=false)
        :m_fp(fp),
         m_bIsDebugOn(bIsDebugOn)
        {
            m_fp = NULL;

		    if ( NULL==filepath || '\0'==filepath[0] || NULL==mode || '\0'==mode[0] ) return;

		    if ( NULL == ( m_fp = fopen(filepath, mode) ) ) {}
		    else {
			    if(m_bIsDebugOn) {}
		    }
    	}

    	~file_open_sentry()
        {
		    if( NULL != m_fp )
            {
			    fclose( m_fp );
			    if( m_bIsDebugOn ) { }
			    m_fp = NULL;
		    }
	    }

    private:
	    FILE	*&m_fp;
	    bool	m_bIsDebugOn;
};

#endif
