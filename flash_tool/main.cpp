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
*  main.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  Flash tool set main window
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

#include <vcl.h>
#include <FileCtrl.hpp>
#include <inifiles.hpp>
#pragma warn -8027  // Functions containing do are not expanded inline
#include <utilcls.h>
#pragma warn .8027

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include <sstream>
#include <string>
#include <vector>

#pragma hdrstop

#include "meta.h"
#include "main.h"
#include "ext_mem1.h"

#include "tboot_1.h"
#include "about.h"

#include "DA_option.h"
#include "RB_option.h"
#include "frm_FormatOption.h"
#include "flashtool.h"

#include "Config.h"
#include "DL_Go.h"
#include "RB_Go.h"
#include "com_util.h"
#include "com_enum.h"
#include "frm_BootROMStartCmdRetry.h"
#include "frm_COM_Option.h"
#include "frm_SecuritySetting.h"
#include "frm_OTPSetting.h"
#include "frm_ParameterReadSetting.h"
#include "flashtool_handle.h"
#include "rom_setting.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Trayicon"
#pragma link "CGAUGES"
#pragma link "CGAUGES"
#pragma resource "*.dfm"

TMainForm *MainForm;

DownloadHistoryList	g_DownloadHistory;
MultiLoadHistoryList	g_MultiLoadHistory;

UART_BAUDRATE	BAUD_RATE;

EXT_CLOCK	g_EXT_CLOCK   = EXT_26M;

BBCHIP_TYPE	g_BBCHIP_TYPE = AUTO_DETECT_BBCHIP;

FLASHTOOL_FUNCTION	    g_flashtool_function    = FLASHTOOL_DOWNLOAD;

ERROR_CODE_MODE_E       g_error_code_mode       = FLASHTOOL;

FLASHTOOL_FINISH_WARNING_E  g_finish_warning_code  = FLASHTOOL_WARNING_NONE;
BACKUP_RESTORE_STATE_E  g_backup_restore_state  = NOACTION;
UPLOAD_STATE_E          g_upload_state          = NOUPLOAD;
MULTILOAD_STATE_E       g_multiload_state       = NOMULTILOAD;
AnsiString              g_restore_backup_data_path;


int g_stopflag;
int iFlashItemIndex = 0;
bool g_HasOngoingOperation = false;

FormatOption_E	g_FormatAfterDownloadOption;
FormatOption_E	g_FormatOption;

unsigned int COM_PORT = 1;   // default com port 1
unsigned int g_ulFormatStartAddr;
unsigned int g_ulFormatLength;
unsigned int g_ulNandFormatStartAddr;
unsigned int g_ulNandFormatLength;
unsigned int g_COM_Read_Timeout          = COM_DEFAULT_TIMEOUT;
unsigned int g_COM_Write_Timeout         = COM_DEFAULT_TIMEOUT;
unsigned int g_MaxBromStartCmdRetryCount = DEFAULT_BROM_START_CMD_RETRY_COUNT;
unsigned int g_CostTimeSec;
unsigned int g_ProgressTimeSec;
unsigned int g_DisplayFinishedBytes;

unsigned char g_Baudrate_FullSync_Count = 1;

bool    bInitComPort = false;
bool    DEBUG = true;
bool	g_bDebugDA = false;
bool	g_bTrace = false;
bool	g_bSkipBootRom = false;
bool	g_bCommandRecordingMode = false;
bool	g_bCheckECOVer = true;
bool	g_bCheckTgtResourceLayout = true;
bool	g_bAutoLoadResByScat = true;
bool	g_bEnableAdvanceOption = false;
bool	g_bPullDownGPIO17 = true;
bool	g_SpeedUpBootROMBaudrate = true;
bool	g_CompareResProjectId = true;
bool	g_FormatValidation = false;
bool    g_bParameterPage = false;
bool    g_bDownloadStyleSequential = false;
bool	g_bNFI_SelectCS1 = false;
bool    g_bUsbDLRB = false;
bool    g_bMMAAEnable = false;
bool    g_bMultiLoadFlag = false;
bool    g_bMultiMemoryFlag = false;
bool    g_bButtonSentry = false;

OptionMethod_E	    g_OperationMethod;
NUTL_EraseFlag_E	g_EraseFlag;

unsigned short      g_PacketLength = 256;

MultiLoadNode	    g_DL_HANDLE_NODE;
DL_HANDLE_LIST_T    g_DL_HANDLE_LIST = NULL;

std::vector<MultiLoadNode>      g_DL_HANDLE_LIST_nodes;
MultiLoadNode   *g_pCurNode     = &g_DL_HANDLE_NODE;
MultiLoadNode   *g_pLastNode     = NULL;    // Only used in Multi-Load features

DA_HANDLE_T	    g_DA_HANDLE     = NULL;
RB_HANDLE_T	    g_RB_HANDLE     = NULL;

AUTH_HANDLE_T   g_AUTH_HANDLE = NULL;
bool            g_AUTH_HANDLE_Loaded = false;
AnsiString      g_AUTH_File;

SCERT_HANDLE_T  g_SCERT_HANDLE = NULL;
bool            g_SCERT_HANDLE_Loaded = false;
AnsiString      g_SCERT_File;

FLASHTOOL_API_HANDLE_T  g_ft_handle = NULL;

ParamDownloadConfig_T   g_ParamDownloadConfig;
ParamReadbackConfig_T   g_ParamReadbackConfig;
ParamReadbackConfig_T   g_ReadbackPage_Param;
ParameterType_T         g_ParameterType;

// Boot Cert 
bool g_BootCert_Enabled = false;
BootCert_T g_BootCert;

EMI_Config_S		g_EMI_Config[8];
DRAM_NFI_Setting	g_EMI_NFI_Setting = { _TRUE, 0, _FALSE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// apply BootLoader's DRAM setting by default

AnsiString	g_asNormalModeDAPath;
AnsiString	g_asTestModeDAPath;
//AnsiString  g_BackupCalibrationDataFolderPath;
//AnsiString 	g_BackUpDirNumber;

TTreeNode * g_rootLoad;

ListViewItemType_E	g_LV_ItemType[] = {
	 LV_NFB_BOOTLOADER
    ,LV_NFB_EXT_BOOTLOADER
	,LV_NFB_DL_ROM
    ,LV_NOR_BOOTLOADER
	,LV_NOR_DL_ROM
	,LV_OTP
	,LV_FTCFG
	,LV_SEC_RO
	,LV_CUST_PARA
    ,LV_GPS_ROM
};

//---------------------------------------------------------------------------
static bool OpenReadDA_File( AnsiString asFileName)
{
	int ret;

	if( S_DONE != (ret=DA_Load(g_DA_HANDLE, asFileName.c_str(), g_bDebugDA?_FALSE:_TRUE)))
    {
		if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
			ShowMessage("Program is busy! It can not reload DA right now.");
		}
		else if( S_INVALID_DA_FILE == ret ) {
			ShowMessage(" \"" + asFileName + "\" is not a valid DA format!");
		}
		else {
			ShowMessage("Failed to load DA \"" + asFileName + "\"!");
		}
		return false;
	}

    // update DA File path to selected File path
    MainForm->ed_DA_File->Text = asFileName;

	return true;
}
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
}
//----------------------------------------------------------------------
void __fastcall TMainForm::Download1Click(TObject *Sender)
{
    g_flashtool_function = FLASHTOOL_DOWNLOAD;

    MultiLoadNode*  p_node = g_pCurNode;
    if( NULL == g_pCurNode )
    {
        // Multi-Load
        if( MainForm->MultiLoad->Checked || 
            MainForm->MultiLoadWithMMAA->Checked )
        {
            if(NULL == g_pLastNode)
            {
                ShowMessage("Please choose a load first.\nIf no load exists, please click the right button in the Multi-Load window, then use \"Add a Load\" to add a load first!");
                return;
            }
            p_node = g_pLastNode;
        }
        else
        {
            // Single Load
            return;
        }
    }

    // check DA bin before downloading
    if ( asBin.IsEmpty() ) 
    {
        ShowMessage(" You must open a download agent file before downloading. ");
        if ( !FileOpenDownloadAgentExecute() )
        {
        	return;
        }
    }

    if ( !g_bDebugDA && (p_node->m_scat_file.IsEmpty() || p_node->m_scat_file == 0)) 
    {
        ShowMessage(" You must open a scatter file before downloading. ");

        if ( !FileOpenLinkMapExecute(p_node) )
        {
        	return;
        }
    }

    
    DL_HANDLE_T &dl_handle = p_node->m_dl_handle;
    DL_INFO_EX dl_info = {};
    DL_GetInfo_Ex(p_node->m_dl_handle, &dl_info);

    if ( !g_bDebugDA && 
        (MultiLoad->Checked || MultiLoadWithMMAA->Checked) )
    {
        int handleNumber = 0;
        DL_GetHandleNumber(g_DL_HANDLE_LIST, &handleNumber);
        if( 0 == handleNumber ) 
        {
        	ShowMessage(" You must at least add 1 load in Multi-Load mode. ");
        	return ;
        }
    }

    // Check ParamListView Download Items
    if( dl_info.security_supported )
    {
        if( true == p_node->m_param.m_otp_enable )
        {
            if ( p_node->m_param.m_otp_filepath.Length() <= 0 )
            {
                ShowMessage(" You must choose a OTP file!");
                return;
            }
        }

        if( ROM_VERSION_SV3 == dl_info.rom_version &&       // SV3 only
            true == p_node->m_param.m_sec_ro_enable )
        {
            if ( p_node->m_param.m_sec_ro_filepath.Length() <= 0 )
            {
                ShowMessage(" You must choose a Secure RO file!");
                return;
            }
        }

        if( ROM_VERSION_SV3 == dl_info.rom_version &&       // SV3 only
            true == p_node->m_param.m_cust_para_enable )
        {
            if ( p_node->m_param.m_cust_para_filepath.Length() <= 0 )
            {
                ShowMessage(" You must choose a Parameter file!");
                return;
        	}
        }
    }

    // clean Flash device info
    HWStatusBarClear();

    // update Status Bar
    updateStatusBar();

    // update DA to the lastest version
    if( UpdateDA() ) return;

    int ret;
    AnsiString s1, s2;
    ROM_INFO_EX rom_info = {};

    while( S_DONE != (ret=DL_IsReady_Ex(dl_handle, &rom_info, _TRUE)) )
    {
        if( S_FTHND_FILE_IS_UPDATED == ret )
        {
            if( DL_Rom_Load_Ex(dl_handle, rom_info.index, rom_info.filepath) )
            {
                s1 = rom_info.name;
                s2 = rom_info.filepath;

                if( S_DL_LOAD_REGION_IS_OVERLAP == ret )
                {
                	ShowMessage( "\"" + s2 + "\" is too large and causes regions to overlap!");
                	return;
                }
                else 
                {
                	ShowMessage("Failed to load " + s1 + " from file \"" + s2 + "\"!");
                	return;
                }
            }
        }
        else {
        	break;
        }
    }

    if ( g_bDebugDA ) 
    {
        DebugDAWindow->Text = "";
    }

    if( !g_bDebugDA )
    {
        // construct current download set
        DownloadSet		dl_set;
        {
            // get scatter file path
            dl_set.m_Scatter = p_node->m_scat_file.c_str();

            // get bin file info
            unsigned short	count=0;

            if( ( S_DONE == DL_GetCount_Ex(dl_handle, &count) ) && (0<count) )
            {
                ROM_INFO_EX binRomInfoEx[MAX_LOAD_SECTIONS];
                if( S_DONE == 
                        DL_Rom_GetInfoAll_Ex(dl_handle, binRomInfoEx, MAX_LOAD_SECTIONS) )
                {
                    for( int i=0; i < count; i++ )
                    {
                        DownloadBinFile dlBinFiles;                        
                        dlBinFiles.m_Filepath = binRomInfoEx[i].filepath;
                        dlBinFiles.m_Enable   = binRomInfoEx[i].enable?true:false;
                        dl_set.m_BinFiles.push_back( dlBinFiles );                                                
                    }
                }
            }
        }
  
        if( MainForm->MultiLoad->Checked || 
            MainForm->MultiLoadWithMMAA->Checked )
        {
            MultiLoadHistoryList::iterator multiLoaditer =  g_MultiLoadHistory.begin();
            for(multiLoaditer = g_MultiLoadHistory.begin(); 
                 multiLoaditer != g_MultiLoadHistory.end(); 
                 multiLoaditer++)
            {
                if(!strcmp(multiLoaditer->m_Scatter.c_str(), dl_set.m_Scatter.c_str()))
                {
                      g_MultiLoadHistory.insert(multiLoaditer,dl_set);
                      g_MultiLoadHistory.erase(multiLoaditer);
                      break;
                }
            }
        }

        // update download history
        DownloadHistoryList::iterator	iter;
        for( iter=g_DownloadHistory.begin(); 
             iter!=g_DownloadHistory.end(); 
             iter++ )
        {
            if( dl_set == (*iter) )
            {
                //if( iter != g_DownloadHistory.begin() )
                {
                	// if download set already exist, move it to the top of history
                	g_DownloadHistory.erase( iter );
                	g_DownloadHistory.push_front( dl_set );
                }
                goto download;
            }
        }

        // delete the oldest history
        if( MAX_DOWNLOAD_HISTORY == g_DownloadHistory.size() ) {                
            g_DownloadHistory.pop_back ();
        }

        // insert new download set to the top of history
        g_DownloadHistory.push_front( dl_set );
    }

download:
   if( MT6205 != g_BBCHIP_TYPE )
	   start_DL(true);
   else
	   Form_DL_GO->ShowModal();
}
//------------------------------------------------------------------------------
void TMainForm::start_DL( bool b_start_DL )
{
    if (g_bCommandRecordingMode)
    {
        CreateConfigFile();
        return;
    }

	g_CostTimeSec = 0;  // reset download time
	g_ProgressTimeSec = 0;

	sb_Main->Panels->Items[4]->Text = " 0:00 sec ";

	p1 = new tboot_1( true );
	p1->FreeOnTerminate  = true;
	p1->Resume();
}
//---------------------------------------------------------------------------

using ConsoleMode::Config;
using ConsoleMode::GeneralSetting;
using ConsoleMode::DownloadSetting;
using ConsoleMode::FormatOptions;
using ConsoleMode::FormatSetting;
using ConsoleMode::ResetToFactoryDefaultSetting;
using ConsoleMode::BackupSetting;
using ConsoleMode::RestoreSetting;

void TMainForm::CreateConfigFile()
{
    void (TMainForm::*configFileCreator)(const std::wstring &) = NULL;

    if (g_flashtool_function == FLASHTOOL_DOWNLOAD)
    {
        if (g_FormatAfterDownloadOption == DISABLE_FORMAT)
        {
            if (g_backup_restore_state == NOACTION)
            {
                configFileCreator = &TMainForm::Create_DL_ConfigFile;
            }
            else if (g_backup_restore_state == BACKUPRESTORE)
            {
                configFileCreator = &TMainForm::Create_BK_DL_RS_ConfigFile;
            }
        }
        else if (g_FormatAfterDownloadOption == RESET_TO_FACTORY)
        {
            if (g_backup_restore_state == NOACTION)
            {
                configFileCreator = &TMainForm::Create_DL_RtFD_ConfigFile;
            }
            else if (g_backup_restore_state == BACKUPRESTORE)
            {
                configFileCreator =
                        &TMainForm::Create_BK_DL_RtFD_RS_ConfigFile;
            }
        }
        else if ((g_FormatAfterDownloadOption == FAT_FORMAT_OPTION) ||
                 (g_FormatAfterDownloadOption == MANUAL_FORMAT_OPTION))
        {
            if (g_backup_restore_state == NOACTION)
            {
                configFileCreator = &TMainForm::Create_DL_FMT_ConfigFile;
            }
            else if (g_backup_restore_state == BACKUPRESTORE)
            {
                configFileCreator = &TMainForm::Create_BK_DL_FMT_RS_ConfigFile;
            }
        }
    }
    else if (g_flashtool_function == FLASHTOOL_FORMAT)
    {
        if (g_FormatOption == RESET_TO_FACTORY)
        {
            if (g_backup_restore_state == NOACTION)
            {
                configFileCreator = &TMainForm::Create_RtFD_ConfigFile;
            }
            else if (g_backup_restore_state == BACKUPRESTORE)
            {
                configFileCreator = &TMainForm::Create_BK_RtFD_RS_ConfigFile;
            }
        }
        else if ((g_FormatOption == FAT_FORMAT_OPTION) ||
                 (g_FormatOption == MANUAL_FORMAT_OPTION) ||
                 (g_FormatOption == TOTAL_FORMAT_OPTION))
        {
            if (g_backup_restore_state == NOACTION)
            {
                configFileCreator = &TMainForm::Create_FMT_ConfigFile;
            }
            else if (g_backup_restore_state == BACKUPRESTORE)
            {
                configFileCreator = &TMainForm::Create_BK_FMT_RS_ConfigFile;
            }
        }
    }

    if (configFileCreator == NULL)
    {
        MessageDlg("This operation is not supported in Command-Recording Mode",
                   mtError, TMsgDlgButtons() << mbOK, 0);
        return;
    }

    static AnsiString initialDirectory(GetCurrentDir());
    AnsiString filename("config.xml");

    if (!PromptForFileName(filename,
                          "FlashTool Console Mode config files (*.xml)|*.xml",
                          "xml", "", initialDirectory, true))
    {
        return;
    }

    // Record the initial directory
    initialDirectory = ExtractFilePath(filename);

    const TOleString oleStr(AnsiToOLESTR(filename.c_str()));
    (this->*configFileCreator)(static_cast<BSTR>(oleStr));
}

void TMainForm::Create_DL_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateDownloadSetting());
    config.SaveFile(filename);
}

void TMainForm::Create_FMT_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateFormatSetting());
    config.SaveFile(filename);
}

void TMainForm::Create_RtFD_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateRtFDSetting(false));
    config.SaveFile(filename);
}

void TMainForm::Create_DL_FMT_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateDownloadSetting());
    config.SaveFile(filename);
}

void TMainForm::Create_DL_RtFD_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateDownloadSetting());
    config.AddCommandSetting(CreateRtFDSetting(false));
    config.SaveFile(filename);
}

void TMainForm::Create_BK_DL_RS_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateBackupSetting(true));
    config.AddCommandSetting(CreateDownloadSetting());
    config.AddCommandSetting(CreateRestoreSetting(false));
    config.SaveFile(filename);
}

void TMainForm::Create_BK_DL_FMT_RS_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateBackupSetting(true));
    config.AddCommandSetting(CreateDownloadSetting());
    config.AddCommandSetting(CreateRestoreSetting(false));
    config.SaveFile(filename);
}

void TMainForm::Create_BK_DL_RtFD_RS_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateBackupSetting(true));
    config.AddCommandSetting(CreateDownloadSetting());
    config.AddCommandSetting(CreateRtFDSetting(true));
    config.AddCommandSetting(CreateRestoreSetting(false));
    config.SaveFile(filename);
}

void TMainForm::Create_BK_FMT_RS_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateBackupSetting(true));
    config.AddCommandSetting(CreateFormatSetting());
    config.AddCommandSetting(CreateRestoreSetting(false));
    config.SaveFile(filename);
}

void TMainForm::Create_BK_RtFD_RS_ConfigFile(const std::wstring &filename)
{
    Config config;

    config.SetGeneralSetting(CreateGeneralSetting());
    config.AddCommandSetting(CreateBackupSetting(true));
    config.AddCommandSetting(CreateRtFDSetting(true));
    config.AddCommandSetting(CreateRestoreSetting(false));
    config.SaveFile(filename);
}

GeneralSetting TMainForm::CreateGeneralSetting()
{
    std::wostringstream comPort;

    if (g_bUsbDLRB)
    {
        comPort << L"USB";
    }
    else
    {
        comPort << COM_PORT;
    }

    DA_INFO daInfo;
    DA_GetInfo(g_DA_HANDLE, &daInfo);

    const TOleString downloadAgentFilepath(AnsiToOLESTR(daInfo.filepath));

    if (ed_AUTH_File->Text.Length() == 0)
    {
        return GeneralSetting(comPort.str(),
                              g_bCheckECOVer,
                              static_cast<BSTR>(downloadAgentFilepath),
                              std::wstring());
    }

    const TOleString authenticationFilepath(
                            AnsiToOLESTR(ed_AUTH_File->Text.c_str()));

    return GeneralSetting(comPort.str(),
                          g_bCheckECOVer,
                          static_cast<BSTR>(downloadAgentFilepath),
                          static_cast<BSTR>(authenticationFilepath));
}

DownloadSetting TMainForm::CreateDownloadSetting()
{
    std::vector<std::wstring> downloadList;   
    unsigned short numRoms = 0;
    ROM_INFO_EX romInfoEx[MAX_LOAD_SECTIONS];
    
    if( (S_DONE == DL_GetCount_Ex(g_pCurNode->m_dl_handle, &numRoms) ) && (0 < numRoms) )
    {
        DL_Rom_GetInfoAll_Ex(g_pCurNode->m_dl_handle, romInfoEx, MAX_LOAD_SECTIONS);
    }
    
    DL_INFO_EX dlInfoEx = {};
    DL_GetInfo_Ex(g_pCurNode->m_dl_handle, &dlInfoEx);

    if (ROM_VERSION_SV3 == dlInfoEx.rom_version)
    {
        for (unsigned short i=0; i < numRoms; ++i)
        {
            if( ARM_BL_ROM == romInfoEx[i].rom_type)
            {
                const std::vector<std::wstring>::iterator result = std::find(downloadList.begin(),
                    downloadList.end(),
                    std::wstring(L"BOOTLOADER"));
                const bool hasBootloaderEnabled = (result != downloadList.end());
                if(!hasBootloaderEnabled && mi_USBDownloadReadback && romInfoEx[i].enable)
                {
                    downloadList.push_back(L"BOOTLOADER");
                }
            }
            else if (romInfoEx[i].enable)
            {
                const TOleString name(AnsiToOLESTR(romInfoEx[i].name));
                downloadList.push_back(static_cast<BSTR>(name));
            }
        }

        if( g_pCurNode->m_param.m_sec_ro_enable)
        {
            downloadList.push_back(L"SECURE_RO");
        }
    }
    else
    {
        for (unsigned short i=0; i < numRoms; ++i)
        {
            if (romInfoEx[i].enable)
            {
                const TOleString name(AnsiToOLESTR(romInfoEx[i].name));
                downloadList.push_back(static_cast<BSTR>(name));
            }
        }
    }

    const TOleString scatterFilepath(
                        AnsiToOLESTR(g_pCurNode->m_scat_file.c_str()));

    const bool autoFormatEnabled =
                    (g_FormatAfterDownloadOption == FAT_FORMAT_OPTION);

    const HW_StorageType_E storageType = (g_OperationMethod == NOR_OPERATION) ? HW_STORAGE_NOR : HW_STORAGE_NAND;
    switch(g_OperationMethod)
    {
        case NOR_OPERATION:
            storageType == HW_STORAGE_NOR;
            break;
        case NFB_OPERATION:
            storageType == HW_STORAGE_NAND;
            break;
        case EMMC_OPERATION:
            storageType == HW_STORAGE_EMMC;
            break;
    }

    unsigned int manualFormatBeginAddress = 0;
    unsigned int manualFormatLength = 0;

    if (g_FormatAfterDownloadOption == MANUAL_FORMAT_OPTION)
    {
        manualFormatBeginAddress =
            (g_OperationMethod == NOR_OPERATION) ? g_ulFormatStartAddr : g_ulNandFormatStartAddr;

        manualFormatLength =
            (g_OperationMethod == NOR_OPERATION) ? g_ulFormatLength : g_ulNandFormatLength;
    }

    const bool watchdogTimerEnabled = 
        (g_FormatAfterDownloadOption == RESET_TO_FACTORY) ||
        (g_backup_restore_state == BACKUPRESTORE);

    // Maggie ToDo: Should support more format action types
    if(autoFormatEnabled)
    {
        return DownloadSetting(static_cast<BSTR>(scatterFilepath),
            downloadList,
            FormatOptions(FAT_FORMAT,
            storageType,
            manualFormatBeginAddress,
            manualFormatLength),
            watchdogTimerEnabled);
    }
    else if(manualFormatLength != 0)
    {
        return DownloadSetting(static_cast<BSTR>(scatterFilepath),
            downloadList,
            FormatOptions(NORMAL_MANUAL_FORMAT,
            storageType,
            manualFormatBeginAddress,
            manualFormatLength),
            watchdogTimerEnabled);
    }
    else
    {
        return DownloadSetting(static_cast<BSTR>(scatterFilepath),
            downloadList,
            FormatOptions(NOT_FORMAT,
            storageType,
            manualFormatBeginAddress,
            manualFormatLength),
            watchdogTimerEnabled);
    }
}

FormatSetting TMainForm::CreateFormatSetting()
{
    const bool autoFormatEnabled = (g_FormatOption == FAT_FORMAT_OPTION);

    const HW_StorageType_E storageType = (g_OperationMethod == NOR_OPERATION) ?
                                            HW_STORAGE_NOR : HW_STORAGE_NAND;

    const unsigned int manualFormatBeginAddress =
        (g_OperationMethod == NOR_OPERATION) ? g_ulFormatStartAddr : g_ulNandFormatStartAddr;

    const unsigned int manualFormatLength =
        (g_OperationMethod == NOR_OPERATION) ? g_ulFormatLength : g_ulNandFormatLength;

    const bool watchdogTimerEnabled =
                            (g_backup_restore_state == BACKUPRESTORE);

    // Maggie ToDo: Should support more format action types
    if(autoFormatEnabled)
    {
        return FormatSetting(FormatOptions(FAT_FORMAT,
            storageType,
            manualFormatBeginAddress,
            manualFormatLength),
            watchdogTimerEnabled);
    }
    else if(manualFormatLength != 0)
    {
        return FormatSetting(FormatOptions(NORMAL_MANUAL_FORMAT,
            storageType,
            manualFormatBeginAddress,
            manualFormatLength),
            watchdogTimerEnabled);
    }
    else
    {
        return FormatSetting(FormatOptions(NOT_FORMAT,
            storageType,
            manualFormatBeginAddress,
            manualFormatLength),
            watchdogTimerEnabled);
    }
}

ResetToFactoryDefaultSetting TMainForm::CreateRtFDSetting(
                                            bool enableWatchdogTimer)
{
    return ResetToFactoryDefaultSetting(enableWatchdogTimer);
}

BackupSetting TMainForm::CreateBackupSetting(bool enableWatchdogTimer)
{
    AnsiString str(ExtractFilePath(Application->ExeName) + "BACKUP.INI");
    const TOleString iniFilePath(AnsiToOLESTR(str.c_str()));

    str = ExtractFilePath(Application->ExeName) + "backup" + '\\' + IntToStr(COM_PORT);

    const TOleString backupDirectoryPath(AnsiToOLESTR(str.c_str()));

    return BackupSetting(static_cast<BSTR>(iniFilePath),
                         static_cast<BSTR>(backupDirectoryPath),
                         enableWatchdogTimer);
}

RestoreSetting TMainForm::CreateRestoreSetting(bool enableWatchdogTimer)
{
    AnsiString str(ExtractFilePath(Application->ExeName) + "BACKUP.INI");
    const TOleString iniFilePath(AnsiToOLESTR(str.c_str()));

    str = ExtractFilePath(Application->ExeName) + "backup" + '\\' + IntToStr(COM_PORT);

    const TOleString restoreDirectoryPath(AnsiToOLESTR(str.c_str()));

    return RestoreSetting(static_cast<BSTR>(iniFilePath),
                          static_cast<BSTR>(restoreDirectoryPath),
                          enableWatchdogTimer);
}

//---------------------------------------------------------------------------
bool TMainForm::FileOpenDownloadAgentExecute ( void )
{
    static AnsiString asDA_Path;

    if ( asDA_Path.Length() > 0 )
    {
       OpenDownloadAgentDialog->InitialDir = asDA_Path;
    }
    else if( 0 < ed_DA_File->Text.Length() )
    {
        const int iLastDel = ed_DA_File->Text.LastDelimiter("\\:");

        if ( iLastDel > 0)
        {
	        OpenDownloadAgentDialog->InitialDir = ed_DA_File->Text.SubString(1, iLastDel-1 );
        }
	}

    if ( OpenDownloadAgentDialog->Execute() )    // *.bin file
    {
         if ( OpenReadDA_File( OpenDownloadAgentDialog->FileName ) )
         {
            asBin = OpenDownloadAgentDialog->FileName;

			if( g_bDebugDA ) {
				// save as TestMode DA path
				g_asTestModeDAPath   = asBin;
			}
			else {
				// save as NormalMode DA path
				g_asNormalModeDAPath = asBin;
			}

            const int iLastDel = asBin.LastDelimiter("\\:");

            if ( iLastDel > 0 ) {
                asDA_Path = asBin.SubString( 1, iLastDel-1 );
            }

            return true;
         }
         else {
            return false;
        }
    }
    else {
        return false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FileOpenDownloadAgentExecute(TObject *Sender)
{
    FileOpenDownloadAgentExecute();
}
//---------------------------------------------------------------------------

bool TMainForm::Parse(MultiLoadNode *p_node, AnsiString asMapFileName)
{
    int ret;
    unsigned short count;

    if (NULL == p_node)
    {
        return false;
    }

    DL_HANDLE_T &dl_handle = p_node->m_dl_handle;

    if (!CreateROMEntries(asMapFileName,
                          true, // Auto load ROM files
                          dl_handle))
    {
        return false;
    }

    p_node->m_scat_file = asMapFileName;

    if(mi_USBDownloadReadback->Checked)
    {
        // Disable bootloader is USB DL/RB is enabled

        if (!BootROMUSBSupported(dl_handle))
        {
            unsigned short count = 0;
            if( ( S_DONE == DL_GetCount_Ex(dl_handle, &count) ) && (0 < count) )
            {       
                ROM_INFO_EX romInfoEx[MAX_LOAD_SECTIONS];
                if( S_DONE == DL_Rom_GetInfoAll_Ex(dl_handle, romInfoEx, MAX_LOAD_SECTIONS))
                {
                    for( int index = 0; index < count; index++)
                    {
                        if(ARM_BL_ROM == romInfoEx[index].rom_type)
                        {
                            DL_Rom_SetEnableAttr_Ex(dl_handle, index, _FALSE);
                        }
                    }
                }    
            }
        }
    }
    //---------------------------------------------------------
    // update Parameter List
    AnsiString tmp_dir;
    AnsiString tmp_file;

    // initialize Check Box of SEC_RO and CUST_PARA
    tmp_dir = ExtractFileDir(asMapFileName);

    // 1. Load SEC_RO
    p_node->m_param.m_sec_ro_enable   = _TRUE; 
    p_node->m_param.m_sec_ro_filepath = "";
    p_node->m_param.m_sec_ro.m_len    = 0;

    tmp_file = tmp_dir + "\\SECURE_RO";
    if( FileExists(tmp_file) )
    {
        if( !LoadFile(tmp_file.c_str(), &p_node->m_param.m_sec_ro.m_data, &p_node->m_param.m_sec_ro.m_len, NULL) )
        {
            p_node->m_param.m_sec_ro_filepath = tmp_file;
        }
    }

    // 2. Load CUST_PARA
    p_node->m_param.m_cust_para_enable   = _TRUE;
    p_node->m_param.m_cust_para_filepath = "";
    p_node->m_param.m_cust_para.m_len    = 0;

    tmp_file = tmp_dir + "\\CUST_PARA";
    if( FileExists(tmp_file) )
    {
        if( !LoadFile(tmp_file.c_str(), &p_node->m_param.m_cust_para.m_data, &p_node->m_param.m_cust_para.m_len, NULL) )
        {
            p_node->m_param.m_cust_para_filepath = tmp_file;
        }
    }

    // 3. Load GPS Project
    // Check if GPS Project exists
    AnsiString gps_dir = tmp_dir + "\\GPS";
    if( DirectoryExists(gps_dir) )
    {
        unsigned int bFindGpsRom = 0;

        p_node->m_param.m_gps_download = (_BOOL) true;

        AnsiString gps_rom = gps_dir + "\\M-core";
        AnsiString gps_ini = gps_dir + "\\gps.ini";

        if( FileExists(gps_rom) ) {
            bFindGpsRom = 1;
        }
        else
        {
            // search *.bin
            TSearchRec sr;
            if ( FindFirst(gps_dir+"\\*.bin", 0, sr) == 0 )
            {
                do
                {
                    gps_rom = gps_dir + "\\" + sr.Name;
                    bFindGpsRom = 1;
                    break;
                } while ( FindNext(sr) == 0 );
                FindClose(sr);
            }
        }

        if( bFindGpsRom && FileExists(gps_ini) )
        {
            p_node->m_param.m_gps_enable        = (_BOOL) true;
            p_node->m_param.m_gps_rom_dirpath   = gps_dir;
            p_node->m_param.m_gps_rom_filepath  = gps_rom;
            p_node->m_param.m_gps_ini_filepath  = gps_ini;

            LoadFileInfo(gps_rom.c_str(), &p_node->m_param.m_gps_rom_filesize, NULL);

            TIniFile *ini = new TIniFile( gps_ini );
            p_node->m_param.m_gps_power_pin     = ini->ReadInteger( "GPIO", "gpio_gps_power_pin",   0xFFFF );
            p_node->m_param.m_gps_reset_pin     = ini->ReadInteger( "GPIO", "gpio_gps_reset_pin",   0xFFFF );
            p_node->m_param.m_gps_standby_pin   = ini->ReadInteger( "GPIO", "gpio_gps_standby_pin", 0xFFFF );
            p_node->m_param.m_gps_32k_pin       = ini->ReadInteger( "GPIO", "gpio_gps_32k_pin",     0xFFFF );
        }
        else{
            p_node->m_param.m_gps_enable   = (_BOOL) false;
        }
    }
    else
    {
        p_node->m_param.m_gps_download = (_BOOL) false;
    }

    // Update DL List
    RedrawReadbackList();
    RedrawDownloadList(p_node, false);

    // show Parameter List View
    DL_INFO_EX dl_info = {};
    if( !DL_GetInfo_Ex(dl_handle, &dl_info) )
    {
        if( dl_info.security_supported || 
            ( ROM_VERSION_SV3 == dl_info.rom_version &&
              p_node->m_param.m_gps_download ) )
        {
            m_ParamListView->Visible = true;
        }
        else
        {
            m_ParamListView->Visible = false;
        }

        MainForm->imgBLSupportUSB->Visible = false;
        MainForm->imgBROMSupportUSB->Visible = false;
        if(dl_info.brom_usb_supported)
        {
            MainForm->imgBROMSupportUSB->Visible = true;
            MainForm->imgBROMSupportUSB->Hint = AnsiString("The chip supports BROM USB Download.\n");
        }
        else if(dl_info.bootloader_usb_supported)
        {
            MainForm->imgBLSupportUSB->Visible = true;
            MainForm->imgBLSupportUSB->Hint = AnsiString("The MAUI load supports USB Download.\n") +
                                            AnsiString("(Please use UART cable to download Bootloader first)");
        }
    }

	return true;
}

//------------------------------------------------------------------------------
bool TMainForm::FileOpenLinkMapExecute(MultiLoadNode *p_node)
{
	if( NULL ==  p_node )
    {
		return false;
	}

    if ( ScatBaseDir.Length() > 0 )
    {
        OpenLdMapDialog->InitialDir = ScatBaseDir;
        OpenLdMapDialog->FileName   = "";
    }

    else if( 0 < ed_LinkMapFile->Text.Length() )
    {
        const int iLastDel = ed_LinkMapFile->Text.LastDelimiter("\\:");

        if ( iLastDel > 0)
        {
	        OpenLdMapDialog->InitialDir = ed_LinkMapFile->Text.SubString(1, iLastDel-1 );
			OpenLdMapDialog->FileName   = "";
        }
    }

    if ( OpenLdMapDialog->Execute() )
    {
    	if( Parse(p_node, OpenLdMapDialog->FileName))
        {
            const int iLastDel = p_node->m_scat_file.LastDelimiter("\\:");

            if ( iLastDel > 0)
            {
               ScatBaseDir  = p_node->m_scat_file.SubString(1, iLastDel-1 );
               ROMBaseDir   = ScatBaseDir;
            }
    	    return true;
    	}
    	else {
        	return false;
    	}
    }
	// do not add "else" !
    return false;
}
//------------------------------------------------------------------------------

void __fastcall TMainForm::FileOpenLinkMapExecute(TObject *Sender)
{
    if ( MultiLoad->Checked || MultiLoadWithMMAA->Checked )
    {
        if( NULL ==  g_pCurNode )
        {
            // Show a promot
            if( MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked )
            {
                ShowMessage("Please choose a load first.\nIf no load exists, please click the right button in the Multi-Load window, then use \"Add a Load\" to add a load first!");
            }
            return;
        }

        // Open Auth for Multi-Load
        OpenAuthMultiLoad();
    }
    else
    {
        // Open Auth for Single Load
        FileOpenLinkMapExecute(g_pCurNode);
    }

    return;
}

void TMainForm::OpenAuthMultiLoad(void)
{
    MultiLoadNode	node;   // for new load in Multi Load

   	// Create DL_HANDLE instance
   	DL_Create(&node.m_dl_handle);

	if( !FileOpenLinkMapExecute(&node) )
    {
        goto error;
    }

    if(OpenAuthMultiLoadMain(&node)){
        goto end;
    } else {
        goto error;
    }

error:
    DL_Destroy(&node.m_dl_handle);
end:
    return;
}

bool TMainForm::OpenAuthMultiLoadMain(MultiLoadNode *p_node)
{
    // Multi-Load ( w/ or w/o Multi-Memory)
    MultiLoadNode	&node = *p_node;

    /* 1. Get BB/Flash Name */
    char custom_name[128];
    char flash_ID[30];
    char flash_name[40];
    char project_flashID[200];

    int retFlashName = OpenAuthGetFlashIdName(&node, custom_name, flash_ID, flash_name, project_flashID);

    /* 2. Update this Handle into DL_HANDLE_List */
    if(!OpenAuthUpdateDLList(&node, AnsiString(custom_name), AnsiString(flash_name), AnsiString(flash_ID), retFlashName)){
        return false;
    }
    //   Destroy all ROM file, and DL_Handle instance
    DL_Rom_UnloadAll(g_pCurNode->m_dl_handle);
	DL_Destroy(&g_pCurNode->m_dl_handle);

    /* 3. Update this Load into UI Node List*/
    OpenAuthUpdateDLNodeList(&node);

    /* 4. Update this Node's BB/Flash Name in UI TreeView Child*/
    TreeView1->Selected->Text = AnsiString(project_flashID);

    /* 5. Update this Load into UI MultiLoad History */
    OpenAuthUpdateMultiHistory(&node);

    return true;
}

int TMainForm::OpenAuthGetFlashIdName(MultiLoadNode *p_node, char *custom_name, char *flash_ID, char *flash_name, char *project_flashID)
{
    MultiLoadNode &node = *p_node;

    /* Get BB/Flash Name */
    int retFlashName = S_DONE;

    // TODO:: what's the meaning here??
    DL_GetCustomName(node.m_dl_handle, custom_name, 128);   //char custom_name[128];
    DL_GetFlashDeviceCode(node.m_dl_handle,flash_ID);

    strcpy(project_flashID,custom_name);
    
    //If MMAA is enabled, display both project name and flash ID, else display only project name
    if(g_bMMAAEnable == true)
    {
        retFlashName = DL_GetFlashName(node.m_dl_handle,flash_name, 40);    //char flash_name[40];
        if(S_DONE == retFlashName)
        {
            strcat(project_flashID, " [");
            strcat(project_flashID,flash_name);
            strcat(project_flashID,"]");
        }
        else
        {
            strcat(project_flashID, " [");
            strcat(project_flashID,flash_ID);
            strcat(project_flashID,"]");
        }
    }

    return retFlashName;
}

bool TMainForm::OpenAuthUpdateDLList(MultiLoadNode *p_node, const AnsiString asCustomName, const AnsiString asFlashName, const AnsiString asFlashID, int retFlashName)
{
    MultiLoadNode	&node = *p_node;

	/* Add this DL_HANDLE into DL_HANDLE_List */
    int ret;
    ret = DL_UpdateHandleToList(g_DL_HANDLE_LIST, g_pCurNode->m_dl_handle, node.m_dl_handle);

	if( S_DONE != ret)
    {
        // ------------------------------
        // Fix @MAUI_02067291 from Deepak
		g_pCurNode = &g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index];

		// re-draw ListView
		RedrawReadbackList();
		RedrawDownloadList(g_pCurNode, false);
        // ------------------------------


        switch(ret)
        {
            case S_DLIST_SAME_BBCHIP_SW_VER:
//                ShowMessage("Load for " + AnsiString(custom_name) + " already exists!");
                ShowMessage("Load for " + asCustomName + " already exists!");
                break;
            case S_DLIST_SAME_FLASHID_AND_BBCHIP_SW_VER:
                if(retFlashName == S_DONE)
                {
//                    ShowMessage("Load for "+  AnsiString(custom_name) + " project and " + AnsiString(flash_name)+" flash already exists!");
                    ShowMessage("Load for "+  asCustomName + " project and " + asFlashName +" flash already exists!");
                }
                else
                {
//                    ShowMessage("Load for " + AnsiString(custom_name) + " project and " + AnsiString(flash_ID) + " flash ID already exists!");
                    ShowMessage("Load for " + asCustomName + " project and " + asFlashID + " flash ID already exists!");
                }
                break;
        }
//        g_rootLoad->Selected = true;
		return false;
	}

    return true;
}

void TMainForm::OpenAuthUpdateDLNodeList(MultiLoadNode *p_node)
{

    int i;
    std::vector<MultiLoadNode>::iterator iter;
    for( i= 0 ,iter=g_DL_HANDLE_LIST_nodes.begin(); iter!= g_DL_HANDLE_LIST_nodes.end(); iter++,i++ )
    {
        if(i == TreeView1->Selected->Index)
        {
            *iter = *p_node;
            break;
        }
    }

}

void TMainForm::OpenAuthUpdateMultiHistory(MultiLoadNode *p_node)
{
    MultiLoadNode	&node = *p_node;

    /* Update this Load into UI MultiLoad History */

    DownloadSet dl_set;
    dl_set.m_Scatter = node.m_scat_file;

    unsigned short count=0;
    if( ( S_DONE == DL_GetCount_Ex(node.m_dl_handle, &count) ) && (0<count) )
    {
        ROM_INFO_EX	rom[MAX_LOAD_SECTIONS];
        if( S_DONE == DL_Rom_GetInfoAll_Ex(node.m_dl_handle, rom, MAX_LOAD_SECTIONS) )
        {
            for( int i=0; i < count; i++ )
            {
                DownloadBinFile		dl_bin;
                dl_bin.m_Filepath = rom[i].filepath;
                dl_bin.m_Enable   = rom[i].enable?true:false;
                dl_set.m_BinFiles.push_back( dl_bin );
            }
        }
    }

    int i;
    MultiLoadHistoryList::iterator	iter;
    for( i= 0 ,iter=g_MultiLoadHistory.begin(); iter!= g_MultiLoadHistory.end(); iter++,i++ )
    {
        if(i == TreeView1->Selected->Index)
        {
            *iter = dl_set;
            break;
        }
    }
}


//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    m_pUpdateListLock = new TCriticalSection();

    // init META_DLL
    META_Init(NULL);
    g_pCurNode = &g_DL_HANDLE_NODE;
    DL_HANDLE_T &dl_handle = g_pCurNode->m_dl_handle;
    DL_Create(&dl_handle);
    DA_Create(&g_DA_HANDLE);
    RB_Create(&g_RB_HANDLE);
    AUTH_Create(&g_AUTH_HANDLE);
    SCERT_Create(&g_SCERT_HANDLE);
    DL_CreateList(&g_DL_HANDLE_LIST);

    Timer_Download->Enabled = false;
    mi_921600Click(Sender);
    imgBLSupportUSB->Visible = false;
    imgBROMSupportUSB->Visible = false;

    memset(&g_ParamDownloadConfig, 0, sizeof(g_ParamDownloadConfig));
    memset(&g_ParamReadbackConfig, 0, sizeof(g_ParamReadbackConfig));
    memset(&g_ReadbackPage_Param, 0, sizeof(g_ReadbackPage_Param));
    memset(&g_ParameterType, 0, sizeof(g_ParameterType));

    // Initialization of TreeView for Multi-Load
    TreeView1->ReadOnly = true; // User are not allowed to edit the caption of each node.
    ::SetWindowLong(TreeView1->Handle, GWL_STYLE,
    ::GetWindowLong(TreeView1->Handle, GWL_STYLE) | TVS_NOTOOLTIPS);
    
    g_rootLoad = TreeView1->Items->AddChild( NULL, "Multi-Load");
    g_rootLoad->Data = NULL;

    // Open the FlashTool.ini and parse the ini file
    ReadFlashToolINI();

    // Compare the project ID
    DL_ResourceProjectIdComparisonSetting( dl_handle, g_CompareResProjectId?_TRUE:_FALSE );

    RedrawReadbackList();
    RedrawDownloadList( g_pCurNode, false );

    // show Parameter List View
    DL_INFO_EX dl_info = {};
    if( !DL_GetInfo_Ex( dl_handle, &dl_info ) )
    {
        if( dl_info.security_supported || 
            ( ROM_VERSION_SV3 == dl_info.rom_version &&
              g_pCurNode->m_param.m_gps_download ) )
        {
            //GPS Project : todo : what kind of rule to show M-core?
            m_ParamListView->Visible = true;
        }
        else{
            m_ParamListView->Visible = false;
        }
    }
    
      // update multiload ietm
    switch( g_multiload_state )
    {
        case MULTILOADWITHMMAA:
            MultiLoadWithMMAA->Checked = false;
            MultiLoadWithMMAAClick(NULL);
            break;
        case MULTILOAD:
            MultiLoad->Checked = false;
            MultiLoadClick(NULL);
            break;

        case NOMULTILOAD :
        default:
            NoMultiLoad->Checked = false;
            NoMultiLoadClick(NULL);
            break;
    }
    
    // show ts_Download tabsheet in first page
    ts_Download->Show();

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::View_toolbarExecute(TObject *Sender)
{
/*
   if ( tb_Main->Visible )
   {
     tb_Main->Visible = false;
     mi_ToolBar->Checked = false;
   }
   else
   {
     tb_Main->Visible = true;
     mi_ToolBar->Checked = true;
   }
*/
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::View_StatusBarExecute(TObject *Sender)
{
/*
   if ( sb_Main->Visible )
   {
      sb_Main->Visible = false;
      mi_StatusBar->Checked = false;
   }
   else
   {
      sb_Main->Visible = true;
      mi_StatusBar->Checked = true;
   }
*/
}
//---------------------------------------------------------------------------

void TMainForm::updateStatusBar()
{
    if( g_bUsbDLRB  ) {
        sb_Main->Panels->Items[2]->Text = "Searching";
    } else {
        sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    switch ( BAUD_RATE )
    {
        case UART_BAUD_921600:
             sb_Main->Panels->Items[3]->Text = "921600 bps";
             break;
        case UART_BAUD_460800:
             sb_Main->Panels->Items[3]->Text = "460800 bps";
             break;
        case UART_BAUD_230400:
             sb_Main->Panels->Items[3]->Text = "230400 bps";
             break;
        case UART_BAUD_115200:
             sb_Main->Panels->Items[3]->Text = "115200 bps";
             break;
        case UART_BAUD_57600:
             sb_Main->Panels->Items[3]->Text = "57600 bps";
             break;
        case UART_BAUD_38400:
             sb_Main->Panels->Items[3]->Text = "38400 bps";
             break;
        case UART_BAUD_19200:
             sb_Main->Panels->Items[3]->Text = "19200 bps";
             break;
        case UART_BAUD_9600:
             sb_Main->Panels->Items[3]->Text = "9600 bps";
             break;
        default :
             if (DEBUG)
             ShowMessage(" main.cpp : undefined Baud Rate " + IntToStr( COM_PORT));
             break;
     }

	switch ( g_OperationMethod )
    {
	    case NOR_OPERATION:
		    sb_Main->Panels->Items[1]->Text = "NOR";
		    break;
       
	    case NFB_OPERATION:
		    sb_Main->Panels->Items[1]->Text = "NFB";
		    break;

	    default:
		    sb_Main->Panels->Items[1]->Text = "??";
		    break;
	}
}

//--------------------------------------------------

void TMainForm::updateMenuItem()
{
    // MCU TYPE
    m_AutoBBCHIP->Checked   = false;
   	BBCHIP_MT6205->Checked  = false;
   	BBCHIP_MT6205B->Checked = false;
   	BBCHIP_MT6218->Checked  = false;
   	BBCHIP_MT6218B->Checked = false;
   	BBCHIP_MT6219->Checked  = false;
    BBCHIP_MT6217->Checked  = false;
    BBCHIP_MT6228->Checked  = false;
    BBCHIP_MT6227->Checked  = false;
    BBCHIP_MT6229->Checked  = false;
    BBCHIP_MT6226->Checked  = false;
    BBCHIP_MT6226M->Checked = false;
    BBCHIP_MT6230->Checked  = false;
    BBCHIP_MT6225->Checked  = false;
    BBCHIP_MT6223->Checked  = false;
    BBCHIP_MT6227D->Checked = false;
    BBCHIP_MT6226D->Checked = false;
    BBCHIP_MT6223P->Checked = false;
    BBCHIP_MT6235->Checked  = false;
    BBCHIP_MT6238->Checked  = false;
    BBCHIP_MT6239->Checked  = false;
    BBCHIP_MT6268A->Checked = false;
    BBCHIP_MT6251T->Checked = false;
    BBCHIP_MT6253T->Checked = false;
    BBCHIP_MT6268B->Checked = false;


    switch( g_BBCHIP_TYPE )
    {
   	    case MT6205:
            BBCHIP_MT6205->Checked = true;
            break;

        case MT6218:
    	    BBCHIP_MT6218->Checked = true;
            break;

        case MT6219:
    	    BBCHIP_MT6219->Checked = true;
            break;

        case MT6205B:
    	    BBCHIP_MT6205B->Checked = true;
            break;

        case MT6217:
	        BBCHIP_MT6217->Checked = true;
            break;

        case MT6228:
	        BBCHIP_MT6228->Checked = true;
            break;

        case MT6227:
	        BBCHIP_MT6227->Checked = true;
            break;

        case MT6229:
	        BBCHIP_MT6229->Checked = true;
            break;

        case MT6226:
	        BBCHIP_MT6226->Checked = true;
            break;

        case MT6226M:
	        BBCHIP_MT6226M->Checked = true;
            break;

        case MT6218B:
    	    BBCHIP_MT6218B->Checked = true;
            break;

        case MT6230:
	        BBCHIP_MT6230->Checked = true;
            break;

        case MT6225:
	        BBCHIP_MT6225->Checked = true;
            break;

        case MT6223:
	        BBCHIP_MT6223->Checked = true;
            break;

        case MT6227D:
	        BBCHIP_MT6227D->Checked = true;
            break;

        case MT6226D:
	        BBCHIP_MT6226D->Checked = true;
            break;

        case MT6223P:
	        BBCHIP_MT6223P->Checked = true;
            break;

        case MT6235:
	        BBCHIP_MT6235->Checked = true;
            break;

        case MT6238:
	        BBCHIP_MT6238->Checked = true;
            break;

        case MT6239:
	        BBCHIP_MT6239->Checked = true;
            break;

        case MT6251T:
	        BBCHIP_MT6251T->Checked = true;
            break;

        case MT6253T:
	        BBCHIP_MT6253T->Checked = true;
            break;

        case MT6268A:
	        BBCHIP_MT6268A->Checked = true;
            break;
         
        case MT6268B:
	        BBCHIP_MT6268B->Checked = true;
            break;

        default:
	        m_AutoBBCHIP->Checked = true;
            break;
	}

    // MCU FREQUENCY
    m_AutoExtClock->Checked = false;

    MCU_13MHz->Enabled = true;
    MCU_26MHz->Enabled = true;
    MCU_52MHz->Enabled = true;

   	MCU_13MHz->Checked = false;
   	MCU_26MHz->Checked = false;
   	MCU_52MHz->Checked = false;

    switch( g_EXT_CLOCK )
    {
        case EXT_13M:
    	    MCU_13MHz->Checked  = true;
            break;

        case EXT_26M:
    	    MCU_26MHz->Checked  = true;
            break;

        case EXT_52M:
    	    MCU_52MHz->Checked  = true;
            break;

        default:
	        m_AutoExtClock->Checked = true;
            break;
	}

    uncheckAllBaudRate();

    mi_921600->Enabled = true;
    mi_460800->Enabled = true;
    mi_230400->Enabled = true;
    mi_57600->Enabled  = true;
    mi_38400->Enabled  = true;
    mi_19200->Enabled  = true;
    mi_9600->Enabled   = true;

    switch ( BAUD_RATE )
    {
        case UART_BAUD_921600:
            mi_921600->Checked = true;
            break;
        case UART_BAUD_460800:
            mi_460800->Checked = true;
            break;
        case UART_BAUD_230400:
            mi_230400->Checked = true;
            break;
        case UART_BAUD_115200:
            mi_115200->Checked = true;
            break;
        case UART_BAUD_57600:
            mi_57600->Checked  = true;
            break;
        case UART_BAUD_38400:
            mi_38400->Checked  = true;
            break;
        case UART_BAUD_19200:
            mi_19200->Checked  = true;
            break;
        case UART_BAUD_9600:
            mi_9600->Checked   = true;
            break;
        default :
            if (DEBUG) ShowMessage(" main.cpp : undefined Baud Rate " + IntToStr( COM_PORT ));
            break;
     }

    switch( g_FormatAfterDownloadOption )
    {
	    case RESET_TO_FACTORY:
		    mi_FormatFDM->Checked = true;
		    mi_FormatFDM->Caption = "Format FAT (Reset to Factory Default) ...";
		    break;

	    case FAT_FORMAT_OPTION:
		    mi_FormatFDM->Checked = true;
		    mi_FormatFDM->Caption = "Format FAT (Auto) ...";
		    break;

	    case MANUAL_FORMAT_OPTION:
		    mi_FormatFDM->Checked = true;
		    mi_FormatFDM->Caption = "Format FAT (Manual) ...";
		    break;

	    case DISABLE_FORMAT:
	    default:
		    mi_FormatFDM->Checked = false;
		    mi_FormatFDM->Caption = "Format FAT (Disabled) ...";
		    break;
	}

    // update backup and restore item
    switch( g_backup_restore_state )
    {

	    case BACKUPRESTORE:
		    BackupandRestore->Caption = "Backup and Restore (Backup -> Download/Format ->Restore) ...";
		    break;

	    case RESTORE:
		    BackupandRestore->Caption = "Backup and Restore (Download/Format -> Restore) ...";
		    break;

	    case RESTOREONLY:
		    BackupandRestore->Caption = "Backup and Restore (Restore Only) ...";
		    break;

	    case NOACTION:

            default:
		    BackupandRestore->Caption = "Backup and Restore (No Action) ...";
		    break;
    }

    // update upload ietm
    switch( g_upload_state )
    {
	    case UPLOAD:
		    UploadSetting->Caption = "Upload (Download/Format -> Upload) ...";
		    break;

	    case UPLOADONLY:
		    UploadSetting->Caption = "Upload (Upload Only) ...";
		    break;

	    case NOUPLOAD :
        default:
		    UploadSetting->Caption = "Upload (No Upload) ...";
		    break;
	}

            // update multiload ietm
    switch( g_multiload_state )
    {
	    case MULTILOADWITHMMAA:
		    MultiLoadSetting->Caption = "Multi-Load (Multi-Memory) ...";
		    break;

	    case MULTILOAD:
		    MultiLoadSetting->Caption = "Multi-Load (Normal) ...";
		    break;

	    case NOMULTILOAD :
            default:
		    MultiLoadSetting->Caption = "Multi-Load (No Action) ...";
		    break;
    }


	if ( g_bCheckECOVer ) {
		m_CheckECOVersion->Checked = true;
	} else {
		m_CheckECOVersion->Checked = false;
	}

    if( g_bCheckTgtResourceLayout ) {
	    m_CheckTgtResourceLayout->Checked = true;
    } else {
	    m_CheckTgtResourceLayout->Checked = false;
    }

    if( g_bAutoLoadResByScat ) {
	    m_AutoLoadResByScat->Checked = true;
    } else {
	    m_AutoLoadResByScat->Checked = false;
    }

    if( g_bPullDownGPIO17 ) {
    	m_PullDownGPIO17->Checked = true;
    } else {
    	m_PullDownGPIO17->Checked = false;
    }

   	if( g_SpeedUpBootROMBaudrate ) {
        m_SpeedUpBootROMBaudrate->Checked = true;
	} else {
        m_SpeedUpBootROMBaudrate->Checked = false;
	}

   	if( g_CompareResProjectId ) {
        m_CompareProjectId->Checked = true;
	} else {
        m_CompareProjectId->Checked = false;
	}

   	if( g_bNFI_SelectCS1 ) {
        m_EnableNFICS1->Checked = true;
	} else {
        m_EnableNFICS1->Checked = false;
	}

    if( g_bDownloadStyleSequential ) {
        DownloadStyle->Checked = true;
    } else {
        DownloadStyle->Checked = false;
    }

	m_Menu_OPM_NOR->Checked  = false;
	m_Menu_OPM_NAND->Checked = false;
	m_Menu_OPM_NFB->Checked  = false;

    switch( g_OperationMethod )
    {
	    case NOR_OPERATION:
		    m_Menu_OPM_NOR->Checked  = true;
		    break;

	    case NFB_OPERATION:
		    m_Menu_OPM_NFB->Checked  = true;
		    break;

	    default:
		    break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::m_AutoExtClockClick(TObject *Sender)
{
	g_EXT_CLOCK = AUTO_DETECT_EXT_CLOCK;
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MCU_13MHzClick(TObject *Sender)
{
	g_EXT_CLOCK = EXT_13M;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MCU_26MHzClick(TObject *Sender)
{
	g_EXT_CLOCK = EXT_26M;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MCU_52MHzClick(TObject *Sender)
{
	g_EXT_CLOCK = EXT_52M;

	updateStatusBar();
	updateMenuItem();
}

//---------------------------------------------------------------------------
void TMainForm::uncheckAllBaudRate()
{
   mi_921600->Checked = false;    // High Speed
   mi_460800->Checked = false;
   mi_230400->Checked = false;
   mi_115200->Checked = false;
   mi_57600->Checked  = false;
   mi_38400->Checked  = false;
   mi_19200->Checked  = false;
   mi_9600->Checked   = false;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::mi_921600Click(TObject *Sender)
{
   BAUD_RATE  = UART_BAUD_921600;
   uncheckAllBaudRate();
   mi_921600->Checked = true;
   updateStatusBar();
   PL_3Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::br_460800Execute(TObject *Sender)
{
   BAUD_RATE  = UART_BAUD_460800;
   uncheckAllBaudRate();
   mi_460800->Checked = true;
   updateStatusBar();
   PL_3Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::br_230400Execute(TObject *Sender)
{
   BAUD_RATE  = UART_BAUD_230400;
   uncheckAllBaudRate();
   mi_230400->Checked = true;
   updateStatusBar();
   PL_2Click(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::br_115200Execute(TObject *Sender)
{
   BAUD_RATE  = UART_BAUD_115200;
   uncheckAllBaudRate();
   mi_115200->Checked = true;

   updateStatusBar();
   PL_2Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::br_19200Execute(TObject *Sender)
{

    BAUD_RATE  = UART_BAUD_19200;
    uncheckAllBaudRate();
    mi_19200->Checked = true;

    updateStatusBar();
    PL_1Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::br_57600Execute(TObject *Sender)
{
    BAUD_RATE  = UART_BAUD_57600;
    uncheckAllBaudRate();
    mi_57600->Checked = true;

    updateStatusBar();
    PL_1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::br_38400Execute(TObject *Sender)
{
    BAUD_RATE  = UART_BAUD_38400;
    uncheckAllBaudRate();
    mi_38400->Checked = true;

    updateStatusBar();
    PL_1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::br_9600Execute(TObject *Sender)
{
    BAUD_RATE  = UART_BAUD_9600;
    uncheckAllBaudRate();
    mi_9600->Checked = true;
    updateStatusBar();
    PL_1Click(Sender);
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::mi_DEBUGClick(TObject *Sender)
{
   if (DEBUG)
   {
      DEBUG = false;
      ShowMessage(" Disable DEBUG. ");
   }
   else
   {
      DEBUG = true;
      ShowMessage(" Enable DEBUG. ");
   }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ExitExecute(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (g_HasOngoingOperation)
    {
        const int response = Application->MessageBox(
            "There is still an ongoing operation. Do you really want to exit?",
            "FlashTool", MB_YESNO | MB_ICONWARNING);

        if (response != IDYES)
        {
            Action = caNone;
            return;
        }
    }

    WriteBackFlashToolINI();

    DL_Destroy(&g_DL_HANDLE_NODE.m_dl_handle);

    if( NULL != g_DL_HANDLE_NODE.m_param.m_sec_ro.m_data ){
        free(g_DL_HANDLE_NODE.m_param.m_sec_ro.m_data);
    }
    if( NULL != g_DL_HANDLE_NODE.m_param.m_cust_para.m_data ){
        free(g_DL_HANDLE_NODE.m_param.m_cust_para.m_data );
    }
    if( NULL != g_DL_HANDLE_NODE.m_param.m_otp.m_data ){
        free(g_DL_HANDLE_NODE.m_param.m_otp.m_data);
    }

    DA_Destroy(&g_DA_HANDLE);
    RB_Destroy(&g_RB_HANDLE);

	AUTH_Destroy(&g_AUTH_HANDLE);
    g_AUTH_HANDLE_Loaded = false;
    g_AUTH_File = "";

    SCERT_Destroy(&g_SCERT_HANDLE);
    g_SCERT_HANDLE_Loaded = false;
    g_SCERT_File = "";

	DL_DestroyList(&g_DL_HANDLE_LIST);

	// destroy META_DLL
	META_Deinit();
}

//---------------------------------------------------------------------------
void TMainForm::ReadFlashToolINI( void )
{
    TIniFile *ini;
    ini = new TIniFile(ChangeFileExt(Application->ExeName, ".INI" ));

/****************************************** [DL_HISTORY_X] Section *************************************************/

    // load download history
    for( int i=0; i < MAX_DOWNLOAD_HISTORY; i++ )
    {
        DownloadSet dl_set;
        dl_set.m_Scatter = ini->ReadString( "DL_HISTORY_"+IntToStr(i), "scatter_file", "");
        if( dl_set.m_Scatter.IsEmpty() ) 
        {
        	break;
        }
        if( dl_set.m_Scatter.Length() == 1 && dl_set.m_Scatter.c_str()[0] == '0')
        {
            continue;
        }        

        const int file_count = ini->ReadInteger( "DL_HISTORY_"+IntToStr(i), "file_count", 1 );

        for ( int j=0; j < file_count; j++ )
        {
        	DownloadBinFile dl_bin;
        	dl_bin.m_Filepath = ini->ReadString ( "DL_HISTORY_"+IntToStr(i), "file"+IntToStr(j), "" );
        	dl_bin.m_Enable   = ini->ReadBool   ( "DL_HISTORY_"+IntToStr(i), "file"+IntToStr(j)+"_enable", true );
        	dl_set.m_BinFiles.push_back( dl_bin );
        }
        g_DownloadHistory.push_back( dl_set );
    }

    // load 1st history
    if( 0 < g_DownloadHistory.size() ) 
    {
        LoadByDownloadSet( *g_DownloadHistory.begin(), g_pCurNode );
    }

/****************************************** [FORM] Section *************************************************/

    g_bPullDownGPIO17  = ini->ReadBool ( "FORM", "pull_down_gpio17", true );
    COM_PORT           = ini->ReadInteger ( "FORM", "com", 1 );
    //g_prev_multiload_state  = (MULTILOAD_STATE_E)(ini->ReadInteger("FORM","PrevMultiLoadOption",1));
    g_multiload_state  = (MULTILOAD_STATE_E)(ini->ReadInteger("FORM","MultiLoadOption",0));
    g_bMultiLoadFlag = ini->ReadBool("FORM", "MultiLoadSetFlag", false);
    g_bMultiMemoryFlag = ini->ReadBool("FORM","MultiMemorySetFlag", false);

    if(g_multiload_state == MULTILOADWITHMMAA)
    {
        g_bMMAAEnable = true;
    }
    else
    {
        g_bMMAAEnable = false;
    }
    
    int multiLoadcount;
    multiLoadcount      = ini->ReadInteger("FORM","MultiLoadCount",0);
    g_MaxBromStartCmdRetryCount = ini->ReadInteger ( "FORM", "brom_start_cmd_retry_count", 
                                                            DEFAULT_BROM_START_CMD_RETRY_COUNT );
    m_BRomStartCmdRetryCount->Caption.printf ( "BootROM Start Command Retry Count (%d) ...", 
                                                            g_MaxBromStartCmdRetryCount );

    int iBaudRate    =  ini->ReadInteger( "FORM", "baudrate", 921600 );
    switch ( iBaudRate )
    {
        case 921600:
        	BAUD_RATE = UART_BAUD_921600;
        	break;
        case 460800:
        	BAUD_RATE = UART_BAUD_460800;
        	break;
        case 230400:
        	BAUD_RATE = UART_BAUD_230400;
        	break;
        case 115200:
        	BAUD_RATE = UART_BAUD_115200;
        	break;
        case 57600:
        	BAUD_RATE = UART_BAUD_57600;
        	break;
        case 38400:
        	BAUD_RATE = UART_BAUD_38400;
        	break;
        case 19200:
        	BAUD_RATE = UART_BAUD_19200;
        	break;
        case 9600:
        	BAUD_RATE = UART_BAUD_9600;
        	break;
        default:
    		BAUD_RATE = UART_BAUD_19200;
    		break;
    }

    AnsiString asNull;
    g_asNormalModeDAPath = asBin = ini->ReadString ( "FORM", "bin", asNull );
    g_asTestModeDAPath   = ini->ReadString ( "FORM", "testmode_da_path", 
                                                                g_asNormalModeDAPath );
    if ( access( asBin.c_str(), 0 ) != 0 ) {
        asBin.SetLength(0);
        if(OpenReadDA_File(GetCurrentDir() + "\\MTK_AllInOne_DA.bin"))
        {
            g_asNormalModeDAPath = asBin = GetCurrentDir() + "\\MTK_AllInOne_DA.bin";
        }
    }
    else 
    {
        // load DA
        OpenReadDA_File( asBin);
    }

    g_OperationMethod  = (OptionMethod_E) ini->ReadInteger ( "FORM", "OperationMethod", NOR_OPERATION );
    g_FormatValidation = ini->ReadInteger ( "FORM", "erase_validation", false );

    // AUTH file
    ed_AUTH_File->Text = ini->ReadString( "FORM", "AUTH_File", asNull );

    int ret = S_DONE;
    if( 0 < ed_AUTH_File->Text.Length() )
    {
        if( S_DONE != ( ret = AUTH_Load( g_AUTH_HANDLE, ed_AUTH_File->Text.c_str())) )
        {
            g_AUTH_HANDLE_Loaded = false;
            g_AUTH_File = "";

            if( S_FTHND_HANDLE_BUSY_NOW == ret )
            {
                ShowMessage( "Program is busy! It can not update settings right now." );
            }
            else 
            {
                ShowMessage( "Failed to load Authentication File \"" + ed_AUTH_File->Text + "\"!" );
            }
        }
        else
        {
            g_AUTH_HANDLE_Loaded = true;
            g_AUTH_File = ed_AUTH_File->Text;
        }
    }

    // SCERT file
    ed_SCERT_File->Text = ini->ReadString( "FORM", "SCERT_File", asNull );

    if( 0 < ed_SCERT_File->Text.Length() )
    {
        if( S_DONE != ( ret = SCERT_Load( g_SCERT_HANDLE, ed_SCERT_File->Text.c_str())) )
        {
            g_SCERT_HANDLE_Loaded = false;
            g_SCERT_File = "";

            if( S_FTHND_HANDLE_BUSY_NOW == ret )
            {
                ShowMessage( "Program is busy! Can not update CERT File right now." );
            }
            else 
            {
                ShowMessage( "Load Certification File \"" + ed_SCERT_File->Text + "\" fail!" );
            }
        }
        else
        {
            g_SCERT_HANDLE_Loaded = true;
            g_SCERT_File = ed_SCERT_File->Text;
        }
    }

    iFlashItemIndex = ini->ReadInteger( "FORM", "flashitemindex", 2 );
    g_ulFormatStartAddr     = strtoul(ini->ReadString( "FORM", "FormatStartAddr", "0x00E00000").c_str(),     NULL, 16);
    g_ulFormatLength        = strtoul(ini->ReadString( "FORM", "FormatLength", "0x00200000").c_str(),        NULL, 16);
    g_ulNandFormatStartAddr = strtoul(ini->ReadString( "FORM", "NandFormatStartAddr", "0x00000000").c_str(), NULL, 16);
    g_ulNandFormatLength    = strtoul(ini->ReadString( "FORM", "NandFormatLength", "0x08000000").c_str(),    NULL, 16);
    g_FormatOption          = (FormatOption_E) ini->ReadInteger( "FORM", "FormatOption", FAT_FORMAT_OPTION);
/****************************************** [MUTLILOAD_HISTORY_X] Section ****************************************************/
    // load download history
    for( int i=0; i < multiLoadcount; i++ )
    {
        DownloadSet		dl_set;
        dl_set.m_Scatter = ini->ReadString( "MULTILOAD_HISTORY_"+IntToStr(i), "scatter_file", "");
        if( dl_set.m_Scatter.IsEmpty() )
        {
            break;
        }
        if( dl_set.m_Scatter.Length() == 1 && dl_set.m_Scatter.c_str()[0] == '0')
        {
            continue;
        }


        const int file_count = ini->ReadInteger( "MULTILOAD_HISTORY_"+IntToStr(i), "file_count", 1 );
        for ( int j=0; j < file_count; j++ )
        {
        	DownloadBinFile dl_bin;
        	dl_bin.m_Filepath = ini->ReadString ( "MULTILOAD_HISTORY_"+IntToStr(i), "file"+IntToStr(j), "" );
        	dl_bin.m_Enable   = ini->ReadBool   ( "MULTILOAD_HISTORY_"+IntToStr(i), "file"+IntToStr(j)+"_enable", true );
        	dl_set.m_BinFiles.push_back( dl_bin );
        }
        g_MultiLoadHistory.push_back(dl_set);
    }
    LoadByMultiLoadSet(&g_MultiLoadHistory);

/****************************************** [DOWNLOAD] Section ****************************************************/
    g_bCheckTgtResourceLayout   = ini->ReadBool( "DOWNLOAD", "chk_tgt_resource_layout", true );
    g_bCheckECOVer              = ini->ReadBool( "DOWNLOAD", "check_bbchip_eco_ver", true );
    g_bAutoLoadResByScat        = ini->ReadBool( "DOWNLOAD", "auto_load_resource_by_scat", true );
    g_FormatAfterDownloadOption = (FormatOption_E) ini->ReadInteger( "DOWNLOAD", "FormatAfterDownloadOption", DISABLE_FORMAT );
    g_CompareResProjectId       = ini->ReadBool ( "DOWNLOAD", "compare_resource_project_id", true);


/****************************************** [READBACK] Section *************************************************/

    int iReadBackCount = ini->ReadInteger( "READBACK", "readback_count", 0 );
    if ( iReadBackCount > 0)
    {
        for ( int i=0; i < iReadBackCount; i++ )
        {
            AnsiString as_rb_file       = ini->ReadString( "READBACK", "file"+IntToStr(i), "ROM_"+ IntToStr( i ));
            bool enable                 = ini->ReadBool  ( "READBACK", "file"+IntToStr(i)+"_enable", true);
            NUTL_ReadFlag_E	read_flag   = (NUTL_ReadFlag_E) ini->ReadInteger( "READBACK", "file"+IntToStr(i)+"_read_flag", NUTL_READ_PAGE_SPARE);

            AnsiString as_readback_addr = ini->ReadString( "READBACK", "startaddr"+IntToStr(i), "0x00000000" );
            AnsiString as_readback_len  = ini->ReadString( "READBACK", "length"+IntToStr(i),    "0x00000100" );

            unsigned long readback_addr = strtoul( as_readback_addr.c_str(), NULL, 16);
            unsigned long readback_len  = strtoul( as_readback_len.c_str() , NULL, 16);

            if ( !RB_Append( g_RB_HANDLE, as_rb_file.c_str(), readback_addr, readback_len ) )
            {
                RB_SetEnableAttr(g_RB_HANDLE, i, enable?_TRUE:_FALSE);
                RB_SetReadbackFlag(g_RB_HANDLE, i, read_flag);
            }
        }
    }

/****************************************** [TRANS_OPTION] Section *************************************************/

    g_PacketLength            = ini->ReadInteger ( "TRANS_OPTION", "packet_length", 4096);
    g_Baudrate_FullSync_Count = ini->ReadInteger ( "TRANS_OPTION", "baudrate_full_sync_count", 1 );
    g_SpeedUpBootROMBaudrate  = ini->ReadBool    ( "TRANS_OPTION", "speedup_brom_baudrate", true);

/****************************************** [EMI] Section *************************************************/
    for ( int i=0 ; i<2 ; i++ )
    {
        g_EMI_Config[i].m_bAuto  = ini->ReadBool ( "EMI", "bank"+IntToStr(i)+"_auto", true);
        g_EMI_Config[i].m_Config = strtoul(ini->ReadString( "EMI", "bank"+IntToStr(i)+"_cfg", "0x00004102" ).c_str(), NULL, 16);
    }

    // NAND NFI Setting
    g_EMI_NFI_Setting.m_bAuto           = ini->ReadBool ( "EMI", "nand_acccon_auto", true);
    g_EMI_NFI_Setting.m_nand_acccon     = strtoul(ini->ReadString ( "EMI", "nand_acccon", "0x000007FF" ).c_str(), NULL, 16);
    // manual DRAM EMI Setting
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_a        = strtoul(ini->ReadString ( "EMI", "emi_gen_a", "0x00008A0A" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_b        = strtoul(ini->ReadString ( "EMI", "emi_gen_b", "0xC0" ).c_str(),       NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_c        = strtoul(ini->ReadString ( "EMI", "emi_gen_c", "0x18C618C6" ).c_str(), NULL, 16);
    	g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_d        = strtoul(ini->ReadString ( "EMI", "emi_gen_d", "0x00020001" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i        = strtoul(ini->ReadString ( "EMI", "emi_con_i", "0x02334000" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i_ext    = strtoul(ini->ReadString ( "EMI", "emi_don_i_ext", "0x0" ).c_str(),    NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_j        = strtoul(ini->ReadString ( "EMI", "emi_con_j", "0x3CD23331" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_k        = strtoul(ini->ReadString ( "EMI", "emi_con_k", "0x03000000" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_l        = strtoul(ini->ReadString ( "EMI", "emi_con_l", "0x1B007004" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_m        = strtoul(ini->ReadString ( "EMI", "emi_con_m", "0x0" ).c_str(),        NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_n        = strtoul(ini->ReadString ( "EMI", "emi_con_n", "0x00400003" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_a        = strtoul(ini->ReadString ( "EMI", "emi_del_a", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_b        = strtoul(ini->ReadString ( "EMI", "emi_del_b", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_c        = strtoul(ini->ReadString ( "EMI", "emi_del_c", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_d        = strtoul(ini->ReadString ( "EMI", "emi_del_d", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_e        = strtoul(ini->ReadString ( "EMI", "emi_del_e", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_f        = strtoul(ini->ReadString ( "EMI", "emi_del_f", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_g        = strtoul(ini->ReadString ( "EMI", "emi_del_g", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_h        = strtoul(ini->ReadString ( "EMI", "emi_del_h", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_i        = strtoul(ini->ReadString ( "EMI", "emi_del_i", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_j        = strtoul(ini->ReadString ( "EMI", "emi_del_j", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_a        = strtoul(ini->ReadString ( "EMI", "emi_arb_a", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_b        = strtoul(ini->ReadString ( "EMI", "emi_arb_b", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_c        = strtoul(ini->ReadString ( "EMI", "emi_arb_c", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_d        = strtoul(ini->ReadString ( "EMI", "emi_arb_d", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_e        = strtoul(ini->ReadString ( "EMI", "emi_arb_e", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_f        = strtoul(ini->ReadString ( "EMI", "emi_arb_f", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_g        = strtoul(ini->ReadString ( "EMI", "emi_arb_g", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_h        = strtoul(ini->ReadString ( "EMI", "emi_arb_h", "0x0" ).c_str(), NULL, 16);
    g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_i        = strtoul(ini->ReadString ( "EMI", "emi_arb_i", "0x0" ).c_str(), NULL, 16);

    // update packet length
    UpdatePacketLength ( g_PacketLength );

    delete ini;
}

//---------------------------------------------------------------------------
void TMainForm::WriteBackFlashToolINI( void )
{
    unsigned int	i;
    DownloadHistoryList::iterator	iter;
    MultiLoadHistoryList::iterator  multiLoaditer;
    TIniFile *ini;

    try 
    {
        ini = new TIniFile(ChangeFileExt(Application->ExeName, ".INI" ));
    } 
    catch (...) 
    {                    
        return;
    }


/****************************************** [EMI] Section *************************************************/
    for ( i=0; i<2; i++ )
    {
        ini->WriteBool  ( "EMI", "bank"+IntToStr(i)+"_auto", g_EMI_Config[i].m_bAuto );
        ini->WriteString( "EMI", "bank"+IntToStr(i)+"_cfg", "0x"+IntToHex((int)(g_EMI_Config[i].m_Config), 8));
    }

//NAND NFI Setting
	ini->WriteBool  ( "EMI", "nand_acccon_auto", g_EMI_NFI_Setting.m_bAuto);
	ini->WriteString( "EMI", "nand_acccon", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_nand_acccon), 8));

//DRAM EMI Setting
	ini->WriteString( "EMI", "emi_gen_a", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_a), 8));
	ini->WriteString( "EMI", "emi_gen_b", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_b), 8));
	ini->WriteString( "EMI", "emi_gen_c", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_c), 8));
	ini->WriteString( "EMI", "emi_gen_d", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_gen_d), 8));
	ini->WriteString( "EMI", "emi_con_i", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i), 8));
	ini->WriteString( "EMI", "emi_con_i_ext", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_i_ext), 8));
	ini->WriteString( "EMI", "emi_con_j", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_j), 8));
	ini->WriteString( "EMI", "emi_con_k", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_k), 8));
	ini->WriteString( "EMI", "emi_con_l", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_l), 8));
	ini->WriteString( "EMI", "emi_con_m", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_m), 8));
	ini->WriteString( "EMI", "emi_con_n", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_con_n), 8));
	ini->WriteString( "EMI", "emi_del_a", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_a), 8));
	ini->WriteString( "EMI", "emi_del_b", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_b), 8));
	ini->WriteString( "EMI", "emi_del_c", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_c), 8));
	ini->WriteString( "EMI", "emi_del_d", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_d), 8));
	ini->WriteString( "EMI", "emi_del_e", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_e), 8));
	ini->WriteString( "EMI", "emi_del_f", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_f), 8));
	ini->WriteString( "EMI", "emi_del_g", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_g), 8));
	ini->WriteString( "EMI", "emi_del_h", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_h), 8));
	ini->WriteString( "EMI", "emi_del_i", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_i), 8));
	ini->WriteString( "EMI", "emi_del_j", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_del_j), 8));
	ini->WriteString( "EMI", "emi_arb_a", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_a), 8));
	ini->WriteString( "EMI", "emi_arb_b", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_b), 8));
	ini->WriteString( "EMI", "emi_arb_c", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_c), 8));
	ini->WriteString( "EMI", "emi_arb_d", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_d), 8));
	ini->WriteString( "EMI", "emi_arb_e", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_e), 8));
	ini->WriteString( "EMI", "emi_arb_f", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_f), 8));
	ini->WriteString( "EMI", "emi_arb_g", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_g), 8));
	ini->WriteString( "EMI", "emi_arb_h", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_h), 8));
	ini->WriteString( "EMI", "emi_arb_i", "0x"+IntToHex((int)(g_EMI_NFI_Setting.m_dram_cfg.m_emi_arb_i), 8));


/****************************************** [FORM] Section *************************************************/
    
    ini->WriteInteger( "FORM", "com", COM_PORT );
    switch ( BAUD_RATE )
    {
        case UART_BAUD_921600:
            ini->WriteInteger( "FORM", "baudrate", 921600);
            break;
        case UART_BAUD_460800:
            ini->WriteInteger( "FORM", "baudrate", 460800);
            break;
        case UART_BAUD_230400:
            ini->WriteInteger( "FORM", "baudrate", 230400);
            break;
        case UART_BAUD_115200:
            ini->WriteInteger( "FORM", "baudrate", 115200);
            break;
        case UART_BAUD_57600:
            ini->WriteInteger( "FORM", "baudrate", 57600);
            break;
        case UART_BAUD_38400:
            ini->WriteInteger( "FORM", "baudrate", 38400);
            break;
        case UART_BAUD_19200:
            ini->WriteInteger( "FORM", "baudrate", 19200);
            break;
        case UART_BAUD_9600:
            ini->WriteInteger( "FORM", "baudrate", 9600);
            break;
        default :
            ini->WriteInteger( "FORM", "baudrate", 19200);
            if (DEBUG) ShowMessage( " main.cpp : undefined Baud Rate " + IntToStr( COM_PORT ));
            break;
    }


    ini->WriteString ( "FORM", "bin",                        g_asNormalModeDAPath );
    ini->WriteString ( "FORM", "AUTH_File",                  ed_AUTH_File->Text );
    ini->WriteBool   ( "FORM", "pull_down_gpio17",           g_bPullDownGPIO17 );
    ini->WriteInteger( "FORM", "brom_start_cmd_retry_count", g_MaxBromStartCmdRetryCount );
    ini->WriteInteger( "FORM", "OperationMethod",            g_OperationMethod );
    ini->WriteInteger( "FORM", "erase_validation",           g_FormatValidation );
    ini->WriteInteger( "FORM", "flashitemindex",             iFlashItemIndex );
    ini->WriteString ( "FORM", "FormatStartAddr",            "0x" + IntToHex((int) g_ulFormatStartAddr, 8) );
    ini->WriteString ( "FORM", "FormatLength",               "0x" + IntToHex((int) g_ulFormatLength, 8) );
    ini->WriteString ( "FORM", "NandFormatStartAddr",        "0x" + IntToHex((int) g_ulNandFormatStartAddr, 8) );
    ini->WriteString ( "FORM", "NandFormatLength",           "0x" + IntToHex((int) g_ulNandFormatLength, 8) );
    ini->WriteInteger( "FORM", "FormatOption",               g_FormatOption );
    //    ini->WriteInteger("FORM",  "PrevMultiLoadOption",            g_prev_multiload_state);
    ini->WriteInteger("FORM",  "MultiLoadOption",            g_multiload_state);
    ini->WriteInteger("FORM","MultiLoadCount",  g_DL_HANDLE_LIST_nodes.size());
    ini->WriteBool("FORM", "MultiLoadSetFlag", g_bMultiLoadFlag);
    ini->WriteBool("FORM", "MultiMemorySetFlag", g_bMultiMemoryFlag);


/****************************************** [DOWNLOAD] Section ****************************************************/

    ini->WriteBool    ( "DOWNLOAD", "compare_resource_project_id", g_CompareResProjectId );
    ini->WriteInteger ( "DOWNLOAD", "FormatAfterDownloadOption",   g_FormatAfterDownloadOption );


/****************************************** [DL_HISTORY_X] Section *************************************************/

    // erase existing section first
    for ( i=0; i<MAX_DOWNLOAD_HISTORY; i++ )
    {
        ini->EraseSection ( "DL_HISTORY_"+IntToStr(i));
    }

    // save download history
    for ( i=0, iter=g_DownloadHistory.begin(); 
           i < MAX_DOWNLOAD_HISTORY && iter!=g_DownloadHistory.end(); 
           i++, iter++)
    {
        // write new section data
        ini->WriteString  ( "DL_HISTORY_"+IntToStr(i), "scatter_file",      iter->m_Scatter);
        ini->WriteInteger ( "DL_HISTORY_"+IntToStr(i), "file_count",        iter->m_BinFiles.size());

        for ( unsigned int j=0; j<iter->m_BinFiles.size(); j++ )
        {
        	ini->WriteString ( "DL_HISTORY_"+IntToStr(i), "file"+IntToStr(j), iter->m_BinFiles[j].m_Filepath);
        	ini->WriteBool   ( "DL_HISTORY_"+IntToStr(i), "file"+IntToStr(j)+"_enable", iter->m_BinFiles[j].m_Enable);
        }
    }


/****************************************** [READBACK] Section *************************************************/

    unsigned short	count;
    if ( ( S_DONE == RB_GetCount(g_RB_HANDLE, &count)) && ( 0<count ) )
    {
        RB_INFO	*rb=new RB_INFO[count];

        if( NULL != rb )
        {
        	ini->WriteInteger( "READBACK", "readback_count", count );

        	if( S_DONE == RB_GetInfoAll(g_RB_HANDLE, rb, count) )
            {
                for ( i=0; i<(unsigned int)count; i++ )
                {
                    // read back file name
                    ini->WriteString ( "READBACK", "file"+IntToStr(i),              rb[i].filepath);
                    // RB_FILE enable flag
                    ini->WriteInteger( "READBACK", "file"+IntToStr(i)+"_enable",    rb[i].enable);
                    ini->WriteInteger( "READBACK", "file"+IntToStr(i)+"_read_flag", rb[i].m_read_flag);
                    // read back addr
                    ini->WriteString ( "READBACK", "startaddr"+IntToStr(i),         "0x"+IntToHex((int)rb[i].readback_addr, 8));
                    // read back length
                    ini->WriteString ( "READBACK", "length"+IntToStr(i),            "0x"+IntToHex((int)rb[i].readback_len, 8));
                }
        	}
        	delete [] rb;
        }
    }


/****************************************** [TRANS_OPTION] Section *************************************************/
	ini->WriteInteger( "TRANS_OPTION", "packet_length",            g_PacketLength );
	ini->WriteInteger( "TRANS_OPTION", "baudrate_full_sync_count", g_Baudrate_FullSync_Count );
	ini->WriteBool   ( "TRANS_OPTION", "speedup_brom_baudrate",    g_SpeedUpBootROMBaudrate);

/****************************************** [MULTILOAD] Section *************************************************/

    for ( i=0; ; i++ )
    {
        if( ini->ReadString( "MULTILOAD_HISTORY_"+IntToStr(i), "scatter_file", "").IsEmpty())
        {
            break;
        }
        ini->EraseSection ( "MULTILOAD_HISTORY_"+IntToStr(i));
    }

    // save download history
    for ( i=0, multiLoaditer= g_MultiLoadHistory.begin(); 
           i < g_DL_HANDLE_LIST_nodes.size() && multiLoaditer!=g_MultiLoadHistory.end(); 
           i++, multiLoaditer++)
    {
        // write new section data
        ini->WriteString  ( "MULTILOAD_HISTORY_"+IntToStr(i), "scatter_file",      multiLoaditer->m_Scatter);
        ini->WriteInteger ( "MULTILOAD_HISTORY_"+IntToStr(i), "file_count",        multiLoaditer->m_BinFiles.size());

        for ( unsigned int j=0; j<multiLoaditer->m_BinFiles.size(); j++ )
        {
        	ini->WriteString ( "MULTILOAD_HISTORY_"+IntToStr(i), "file"+IntToStr(j), multiLoaditer->m_BinFiles[j].m_Filepath);
        	ini->WriteBool   ( "MULTILOAD_HISTORY_"+IntToStr(i), "file"+IntToStr(j)+"_enable", multiLoaditer->m_BinFiles[j].m_Enable);
        }
    }
           
	delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mi_ViewClick(TObject *Sender)
{
   updateMenuItem();
}
//---------------------------------------------------------------------------
// TODO:: remove it because there is not index transformation by using _Ex API
int GetROMIndex(ListViewItemType_E type, DL_HANDLE_T dl_handle, int org_index)
{
    /*
    int itemIndex;

    if(LV_NFB_DL_ROM==type)
    {
        BL_INFO bl_info;

        if( S_DONE == DL_BL_GetInfo(dl_handle, &bl_info))
        {
            if(bl_info.m_ext_bootloader_exist)
            {
                itemIndex = org_index - 2;
            }
            else
            {
                itemIndex = org_index - 1;
            }
        }
    }
    else
    {
        itemIndex = org_index;
    }

    return itemIndex;
    */
    return 1;
}

void __fastcall TMainForm::ListView1_oldClick(TObject *Sender)
{
    // This function is only used for scatter file loading case (rom setting version 1)
    int ret;
    TListView *plv = dynamic_cast<TListView *>(Sender);

    if ( plv->Selected == NULL ) return;
    if ( NULL == g_pCurNode ) return;

    DL_HANDLE_T &dl_handle=g_pCurNode->m_dl_handle;

    if( ROMBaseDir.Length() > 0 )
    {
        OpenSingleRomFile->InitialDir = ROMBaseDir;
        OpenSingleRomFile->FileName = "";
    }
    else if( 4 <= plv->Selected->SubItems->Count )
    {
        const int iLastDel = plv->Selected->SubItems->Strings[3].LastDelimiter("\\:");

        if ( iLastDel > 0)
        {
            OpenSingleRomFile->InitialDir = plv->Selected->SubItems->Strings[3].SubString(1, iLastDel-1 );
            OpenSingleRomFile->FileName = "";
        }
    }

    // open file
    if( OpenSingleRomFile->Execute() ) 
    {
        goto load_file;
    }

    return;

load_file:    
    const unsigned int itemIndex = plv->ItemIndex;

    // load ROM list
    if( S_DONE != ( ret = DL_Rom_Load_Ex(dl_handle, itemIndex, OpenSingleRomFile->FileName.c_str())) )
    {
        if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
        	ShowMessage( "Program is busy! It can not update settings right now." );
        	return;
        }
        else if( S_DL_LOAD_REGION_IS_OVERLAP == ret ) {
        	ShowMessage( "\"" + OpenSingleRomFile->FileName + "\" is too large and causes regions to overlap!");
        	return;
        }
        else {
        	ShowMessage( "Failed to load file \"" + OpenSingleRomFile->FileName + "\"!" );
        	return;
        }
    }

    const int iLastDel = OpenSingleRomFile->FileName.LastDelimiter("\\:");
    if ( iLastDel > 0)
    {
        ROMBaseDir = OpenSingleRomFile->FileName.SubString(1, iLastDel-1 );
    }

    // auto load download bin files for scatter file (rom setting version 1)    
    unsigned short count=0;
    if( (S_DONE == DL_GetCount_Ex( dl_handle, &count )) && (0 < count))
    {
        DL_INFO_EX dl_info = {};

        /*
              if(S_DONE != DL_GetInfo_Ex(dl_handle, &dl_info))
              {
                ShowMessage( "Failed to get DL_INFO_EX by DL_GetInfo_Ex()!" );
                        return;
                }
            */
        if(S_DONE == DL_GetInfo_Ex(dl_handle, &dl_info))
        {
            /*ShowMessage( "Failed to get DL_INFO_EX by DL_GetInfo_Ex()!" );
                        return;*/
            if( g_bAutoLoadResByScat && 
                !strcmp( "ROM", plv->Selected->Caption.c_str()) )
            {
                if(ROM_VERSION_SV3 == dl_info.rom_version)
                {
                    DL_AutoLoadByScatRegionName(dl_handle, OpenSingleRomFile->FileName.c_str(), NORMAL_ROM|RESOURCE_BIN|JUMPTABLE_BIN, _FALSE);
                }
                else if(ROM_VERSION_SV5 == dl_info.rom_version)
                {
                    DL_AutoLoadROMFiles(dl_handle);
                }
                else
                {
                    ShowMessage( "Unknown ROM VERSION to auto load!" );
                    return;
                }
            }    
        }
        
            
    }
    else
    {
        ShowMessage( "DL_GetCount_Ex() fail!" );
        return;
    }
	
redraw:
	RedrawReadbackList();
	RedrawDownloadList(g_pCurNode, false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReadbackExecute(TObject *Sender)
{

    g_flashtool_function = FLASHTOOL_READBACK;

    if ( asBin.IsEmpty() )
    {
       ShowMessage(" You must open a download agent file before readbacking. ");

       if ( !FileOpenDownloadAgentExecute() )
       {
          return;
       }
    }

    if ( lv_ReadBack->Items->Count ==0 )
    {
        ShowMessage(" Please add a readback file.");
        return;
    }

    // clean Flash device info
	HWStatusBarClear();

    // update DA to the latest version
	if( UpdateDA() ) return;

    if( MT6205 != g_BBCHIP_TYPE )
        start_DL(true);
    else
        Form_RB_GO->ShowModal();
}
//------------------------------------------------------------------------------
void TMainForm::start_RB( bool b_start_RB )
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::tb_AddReadBackFileClick(TObject *Sender)
{
	int ret;
	AnsiString s;
	TListItem *pListItem;

    unsigned long default_readback_addr = 0x00000000;
    unsigned long default_readback_len = 0x00000800;

	s = "ROM_" + IntToStr(lv_ReadBack->Items->Count);
	if( S_DONE != ( ret=RB_Append(g_RB_HANDLE, s.c_str(), default_readback_addr, default_readback_len)))
    {
		if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
			ShowMessage("Program is busy! It can not update settings right now.");
		}
		else {
			ShowMessage("Failed to add a readback file! ");
		}
		return;
	}

	RedrawReadbackList();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::tb_DeleteReadBackFileClick(TObject *Sender)
{
	int ret;

	if ( lv_ReadBack->Selected == NULL ) return;

	if( S_DONE != (ret=RB_Delete(g_RB_HANDLE, lv_ReadBack->ItemIndex)))
    {
		if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
			ShowMessage("Program is busy! It can not delete readback entry right now.");
		}
		else {
			ShowMessage("Failed to delete a readback entry!");
		}
		return;
	}
	else {
		lv_ReadBack->Selected->Delete();
	}
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::lv_ReadBack_oldDblClick(TObject *Sender)
{
	int ret;
	TListView *plv = dynamic_cast<TListView *>(Sender);

	if ( plv->Selected == NULL ) return;

	sd_ReadBack->FileName = plv->Selected->SubItems->Strings[2];

	RB_INFO	rb_info;
	if( S_DONE != ( ret=RB_GetInfo(g_RB_HANDLE, plv->ItemIndex, &rb_info)))
    {
		if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
			ShowMessage("Program is busy! It can not update settings right now.");
		}
		else {
			ShowMessage("Failed to update the readback settings!");
		}
		return;
	}

	if ( sd_ReadBack->Execute() )
	{
		if( S_DONE != (ret=RB_SetFilepath(g_RB_HANDLE, plv->ItemIndex, sd_ReadBack->FileName.c_str())))
        {
			if( S_FTHND_HANDLE_BUSY_NOW == ret ) {
				ShowMessage("Program is busy! It can not update settings right now.");
			}
			else {
				ShowMessage("Failed to update the readback settings!");
			}
			return;
		}
		else {
			plv->Selected->SubItems->Strings[2] = sd_ReadBack->FileName ;
		}
	}
	else {
		return;
	}

	Form_RB_Option->rb_ReadbackHex->Checked = true;
	Form_RB_Option->ed_RB_StartAddr->Text   = "0x" + IntToHex((int)rb_info.readback_addr, 8);
	Form_RB_Option->ed_RB_Length->Text      = "0x" + IntToHex((int)rb_info.readback_len, 8);

	switch( rb_info.m_read_flag )
    {
	    case NUTL_READ_PAGE_ONLY:
    	    Form_RB_Option->m_Radio_ReadPageOnly->Checked = true;
		    break;

	    case NUTL_READ_SPARE_ONLY:
        	Form_RB_Option->m_Radio_ReadSpareOnly->Checked = true;
		    break;

	    case NUTL_READ_PAGE_WITH_ECC:
    	    Form_RB_Option->m_Radio_ReadPageWithECC->Checked = true;
		    break;

	    case NUTL_READ_PAGE_SPARE:
	    default:
    	    Form_RB_Option->m_Radio_ReadPageSpare->Checked = true;
		    break;
	}

	Form_RB_Option->Visible = true;

	Form_RB_Option->bb_ReadBack_OK->Visible = true;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::Timer_DownloadTimer(TObject *Sender)
{
	unsigned int Bps;
	float KBps;
	AnsiString rate;
	AnsiString Zero;
	static unsigned int LastDisplayFinishedBytes=0;

	g_CostTimeSec++;
	g_ProgressTimeSec++;

    Zero = (10>(g_CostTimeSec%60)) ? "0" : "";

	sb_Main->Panels->Items[4]->Text = " " + IntToStr(g_CostTimeSec/60) + ":" + Zero + IntToStr(g_CostTimeSec%60) + " sec ";

	if( LastDisplayFinishedBytes != g_DisplayFinishedBytes )
    {

		LastDisplayFinishedBytes = g_DisplayFinishedBytes;

		if( 0 >= g_ProgressTimeSec ) {
			rate = "0 Bps";
		}
		else if( 1024 < (Bps=g_DisplayFinishedBytes/g_ProgressTimeSec) ) {
			KBps = Bps;
			rate = FloatToStrF( KBps/1024, ffFixed, 7, 2) + " KBps";
		}
		else {
			rate = IntToStr(Bps) +  " Bps";
		}

	    MainForm->sb_Main->Panels->Items[0]->Text = IntToStr(g_DisplayFinishedBytes) + " Bytes / " + rate;
	}
}

//---------------------------------------------------------------------------

#ifdef _DEBUG
  #include "ok_wnd.h"
#endif /* #ifdef _DEBUG */

void __fastcall TMainForm::testOKWnd1Click(TObject *Sender)
{
#ifdef _DEBUG

    //Form_OK->ClientHeight = Form_OK->img_OK->Picture->Height+1;
    //Form_OK->ClientWidth  = Form_OK->img_OK->Picture->Width+1;

    //Form_OK->img_OK->Height = Form_OK->img_OK->Picture->Height;
    //Form_OK->img_OK->Width  = Form_OK->img_OK->Picture->Width;
    //Form_OK->ClientHeight   = Form_OK->img_OK->Height+1;
    //Form_OK->ClientWidth    = Form_OK->img_OK->Width+1;

    //Form_OK->Show();

#endif /* #ifdef _DEBUG */
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::About1Click(TObject *Sender)
{
    AnsiString as;
    DA_INFO da_info;
    unsigned int ret;


    // retrieval the DA information
    if( !DA_GetInfo( g_DA_HANDLE, &da_info) )
    {
        if( strlen(da_info.version) != 0 )
        {
            as.printf("DA_v%s",da_info.version);
            Form_About->da_version->Caption = as;

            if( strlen( da_info.last_modified_date ) != 0 )
            {
                as.printf( "(Build-Date: %s)", da_info.last_modified_date );
                Form_About->da_build_date->Caption = as;
            }
            else{
                Form_About->da_build_date->Caption = "";
            }
        }
        else{
            Form_About->da_version->Caption    = "";
            Form_About->da_build_date->Caption = "";
        }
    }
    else{
        Form_About->da_version->Caption    = "";
        Form_About->da_build_date->Caption = "";
    }

    // show FlashTool about Form
    Form_About->Show();

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mi_FormatFDMClick(TObject *Sender)
{
	FormatOption->bCallFromFormatButton = false;
	FormatOption->Visible = true;

/*	if(bFormatFDM) {
        bFormatFDM = false;
	}
    else {
        bFormatFDM = true;
	}

	updateMenuItem();*/
}
//---------------------------------------------------------------------------

void TMainForm::DisableFileMenu()
{
    if (File1->Count == 0)
    {
        return;
    }

    for (int i=0; i<File1->Count; ++i)
    {
        TMenuItem *item = File1->Items[i];
        assert(item != NULL);

        item->Enabled = false;
    }

    // Exit should still work
    Exit1->Enabled = true;
}

//---------------------------------------------------------------------------

void TMainForm::EnableFileMenu()
{
    if (File1->Count == 0)
    {
        return;
    }

    for (int i=0; i<File1->Count; ++i)
    {
        TMenuItem *item = File1->Items[i];
        assert(item != NULL);

        item->Enabled = true;
    }
}

//---------------------------------------------------------------------------
void TMainForm::DisableDownloadButton ( void )
{
	// disable download button
    tb_DL_StartButton->Enabled = false;
    Download1->Enabled = false;
    Download->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableDownloadButton ( void )
{
	// enable download button
    tb_DL_StartButton->Enabled = true;
    Download1->Enabled = true;
    Download->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableReadbackButton ( void )
{
	// disable readback button
    ReadBack1->Enabled = false;
    tb_ReadBack2->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableReadbackButton ( void )
{
	// enable readback button
    ReadBack1->Enabled = true;
    tb_ReadBack2->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableFormatButton ( void )
{
	// disable download button
    tb_FormatButton->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableFormatButton ( void )
{
	// enable download button
    tb_FormatButton->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::EnableDASelectButton ( void )
{
	// enable download button
    tb_OpenDownloadAgent->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableDASelectButton ( void )
{
	// enable download button
    tb_OpenDownloadAgent->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableScatterSelectButton ( void )
{
	// enable download button
    tb_ScatterLoading->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableScatterSelectButton ( void )
{
	// enable download button
    tb_ScatterLoading->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableAuthSelectButton ( void )
{
	// enable download button
    tb_OpenAuthFile->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableAuthSelectButton ( void )
{
	// enable download button
    tb_OpenAuthFile->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableSCertSelectButton ( void )
{
	// enable download button
    tb_OpenSCertFile->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableSCertSelectButton ( void )
{
	// enable download button
    tb_OpenSCertFile->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableRBAddButton ( void )
{
	tb_AddReadBackFile-> Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableRBAddButton ( void )
{
	tb_AddReadBackFile-> Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableRBRemoveButton ( void )
{
	tb_DeleteReadBackFile->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableRBRemoveButton ( void )
{
	tb_DeleteReadBackFile->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableBootCertSelectButton ( void )
{
    bt_boot_cert_file_open->Enabled = true;
}
//---------------------------------------------------------------------------
void TMainForm::DisableBootCertSelectButton ( void )
{
    bt_boot_cert_file_open->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::UpdateCaption ( void )
{
	AnsiString s;
	s = "FlashTool";

	if(g_bDebugDA) {
		s += " (Test Mode)";
	}
	if(g_bTrace) {
		s += " (Runtime Trace)";
	}
	if(g_bSkipBootRom) {
		s += " (Skip BootRom)";
	}
	if(g_bEnableAdvanceOption) {
		s += " (Advanced Mode)";
	}
	if(g_bCommandRecordingMode) {
		s += " (Command-Recording Mode)";
	}

    Caption = s;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::action_ToggleDebugDAExecute(TObject *Sender)
{
	// toggle debug DA flag
	if( g_bDebugDA ) {
		g_bDebugDA                    = false;
        DebugDAWindow->Visible        = false;
        Panel1->Visible               = true;
		ListView1->Visible            = true;
		m_ParamListView->Visible      = true;
        tb_ReadBack->Visible          = true;
        lv_ReadBack->Visible          = true;
        m_SaveTestModeLog->Visible    = false;
        FileSave_TestModeLog->Enabled = false;

		// restore NormalMode DA path
		asBin = g_asNormalModeDAPath;
	}
	else {
		g_bDebugDA                    = true;
        DebugDAWindow->Visible        = true;
        DebugDAWindow->Text           = "";
        Panel1->Visible               = false;
		ListView1->Visible            = false;
		m_ParamListView->Visible      = false;
        tb_ReadBack->Visible          = false;
        lv_ReadBack->Visible          = false;
        m_SaveTestModeLog->Visible    = true;
        FileSave_TestModeLog->Enabled = true;

		// restore TestMode DA path
		asBin = g_asTestModeDAPath;
	}

	// reload DA
	OpenReadDA_File(asBin);

	UpdateCaption();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::action_ToggleTraceExecute(TObject *Sender)
{
	// toggle runtime trace flag  (Ctrl + Alt + T)
	if( g_bTrace ) {
		g_bTrace = false;
        Brom_DebugOff();
        META_DebugOff();
	}
	else {
		g_bTrace = true;
        Brom_DebugOn();
        META_DebugOn();
	}
	UpdateCaption();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::action_RecordCommandExecute(TObject *Sender)
{
    g_bCommandRecordingMode = !g_bCommandRecordingMode;
    UpdateCaption();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::tb_FormatButtonClick(TObject *Sender)
{
	g_flashtool_function = FLASHTOOL_FORMAT;

   	FormatOption->bCallFromFormatButton = true;
    FormatOption->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::action_ForceStopExecute(TObject *Sender)
{
    // Ctrl + Alt + Z
	g_stopflag = BOOT_STOP;
	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::action_ToggleSkipBootRomExecute(TObject *Sender)
{
	// toggle skip bootrom flag, Ctrl + Alt + B
	if( g_bSkipBootRom ) {
		g_bSkipBootRom = false;
	}
	else {
		g_bSkipBootRom = true;
	}
	UpdateCaption();
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::action_PurgeDebugLogExecute(TObject *Sender)
{
    // Ctrl + X
	Brom_DebugClear();
    META_DebugClear();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::PL_1Click(TObject *Sender)
{
    UpdatePacketLength(256);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PL_2Click(TObject *Sender)
{
    UpdatePacketLength(1024);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PL_3Click(TObject *Sender)
{
    UpdatePacketLength(4096);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PL_4Click(TObject *Sender)
{
    UpdatePacketLength(10240);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PL_5Click(TObject *Sender)
{
    UpdatePacketLength(20480);
}
//---------------------------------------------------------------------------

void TMainForm::UpdatePacketLength(unsigned short pkt_length)
{
    PL_1->Checked = false;
    PL_2->Checked = false;
    PL_3->Checked = false;
    PL_4->Checked = false;
    PL_5->Checked = false;

	switch( pkt_length )
    {
    	case 256:
        	g_PacketLength = 256;
		    PL_1->Checked = true;
        	break;

        case 1024:
        	g_PacketLength = 1024;
		    PL_2->Checked = true;
        	break;

        case 4096:
        	g_PacketLength = 4096;
		    PL_3->Checked = true;
        	break;

        case 10240:
        	g_PacketLength = 10240;
		    PL_4->Checked = true;
        	break;

        case 20480:
        	g_PacketLength = 20480;
		    PL_5->Checked = true;
        	break;

    	default:
        	g_PacketLength = 1024;
		    PL_2->Checked = true;
            break;
	}

    if ((g_pCurNode != NULL) &&
        (g_pCurNode->m_dl_handle != NULL))
    {
        DL_SetPacketLength(g_pCurNode->m_dl_handle, g_PacketLength);
    }

    if (g_RB_HANDLE != NULL)
    {
        RB_SetPacketLength(g_RB_HANDLE, g_PacketLength);
    }
}

//---------------------------------------------------------------------------


void __fastcall TMainForm::Options1Click(TObject *Sender)
{
    if (g_bButtonSentry)
    {
        return;
    }

	int ret;
	unsigned short com_port_list[255];
    unsigned short com_port_count = sizeof(com_port_list)/sizeof(unsigned short);

    if( COM_ENUM_OK != (ret=ComPortEnumerate(com_port_list, &com_port_count)) )
    {
    	if( COM_ENUM_NO_COM_PORT_FOUND == ret ) {
            for (int i=0; i<sizeof(m_COMPortList)/sizeof(m_COMPortList[0]); ++i)
            {
                m_COMPortList[i] = 0;
            }

            m_COMPortCount = 0;

            DCOMPort->Clear();
            DCOMPort->Enabled = false;
    	}
    	else {
			ShowMessage("Failed to enumerate a certain COM port! ");
		}
		return;
	}

    if( com_port_count == m_COMPortCount && !memcmp((void *)com_port_list, (void *)m_COMPortList, m_COMPortCount*sizeof(unsigned short)) )
    {
//		ShowMessage(" COM ports are the same! ");
    	return;
    }

    // assign new com port list
    m_COMPortCount = com_port_count;
    memcpy((void *)m_COMPortList, (void *)com_port_list, m_COMPortCount*sizeof(unsigned short));

	DCOMPort->Clear();

    AnsiString s;
    bool DefaultChecked = false;

    for( int i=0;  i < m_COMPortCount ;  i++ )
    {
    	TMenuItem *NewItem = new TMenuItem(DCOMPort);
       	s = "COM" + IntToStr(m_COMPortList[i]);
   	    NewItem->Caption = s;
   	    NewItem->Enabled = true;
        NewItem->Visible = true;
        NewItem->OnClick = COMPortSubMenuClick;

		if( m_COMPortList[i] == (int)COM_PORT )
        {
			NewItem->Checked = true;
			DefaultChecked = true;
        }
        DCOMPort->Add(NewItem);
	}

	if( !DefaultChecked )
    {
        DCOMPort->Items[0]->Checked = true;
		COM_PORT = m_COMPortList[0];
    }

    updateStatusBar();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::COMPortSubMenuClick(TObject *Sender)
{
	for( unsigned short i=0; i<m_COMPortCount; i++ )
    {
        DCOMPort->Items[i]->Checked = false;
    }

	((TMenuItem *)Sender)->Checked = true;
	COM_PORT = m_COMPortList[DCOMPort->IndexOf(((TMenuItem *)Sender))];
	updateStatusBar();
}
//---------------------------------------------------------------------------
static int DL_IndexOfChangedCheckBox = -1;

void __fastcall TMainForm::ListView1Click(TObject *Sender)
{
    TListView *plv = dynamic_cast<TListView *>(Sender);
    if( NULL == g_pCurNode ) 
    {
        return;
    }

    DL_HANDLE_T &dl_handle=g_pCurNode->m_dl_handle;

    if( 0 <= DL_IndexOfChangedCheckBox && 
        DL_IndexOfChangedCheckBox < plv->Items->Count ) 
    {
        TListItem *pItem=plv->Items->Item[DL_IndexOfChangedCheckBox];
        // USB DL: User Forbidden to select Boot Loader
        if( g_bUsbDLRB && !BootROMUSBSupported(dl_handle))
        {
            ROM_INFO_EX romInfoEx = {};
            if(S_DONE == DL_Rom_GetInfo_Ex(dl_handle, pItem->Index/*plv->Selected->Index*/ , &romInfoEx))
            {
                if(ARM_BL_ROM == romInfoEx.rom_type)
                {
                    RedrawDownloadCheckBox();
                    return;
                }
            }
             
            /* TODO:: revmove it ?
            if( (LV_NOR_BOOTLOADER == type ) ||
                (LV_NFB_BOOTLOADER == type) ||
                (LV_NFB_EXT_BOOTLOADER == type) )
            {
                RedrawDownloadCheckBox();
                return;
            }
            */
        }
      
        DL_Rom_SetEnableAttr_Ex(dl_handle, pItem->Index, pItem->Checked?_TRUE:_FALSE);
        RedrawDownloadCheckBox();
    }
    else 
    {
    	ListView1_oldClick(Sender);
    }

    DL_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ListView1DblClick(TObject *Sender)
{
	ListView1Click(Sender);
}
//---------------------------------------------------------------------------

static bool DL_bPreviousCheckBoxValue;

void __fastcall TMainForm::ListView1Change(TObject *Sender, TListItem *Item, TItemChange Change)
{
	// ShowMessage("Changed!");

    if( (ctState == Change) &&
    	(Item->Checked != DL_bPreviousCheckBoxValue) ) {
        DL_IndexOfChangedCheckBox = Item->Index;
        return;
    }

    DL_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ListView1Changing(TObject *Sender, TListItem *Item, TItemChange Change, bool &AllowChange)
{
	// ShowMessage("Changing!");

	DL_bPreviousCheckBoxValue = Item->Checked;

    DL_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void TMainForm::RedrawDownloadList(MultiLoadNode  *p_node, bool bUpdateParamOnly)
{
    CS_Sentry	cs(m_pUpdateListLock);
    AnsiString	    s;
    DL_INFO_EX dl_info;
    unsigned short  count;
    TListItem	    *pListItem;

    if( !bUpdateParamOnly )
    {
        ListView1->Items->Clear();
    }
    m_ParamListView->Items->Clear();

    if( NULL == p_node )
    {
        return;
    }
    
    memset(&dl_info,0,sizeof(DL_INFO_EX));
    DL_HANDLE_T &dl_handle=p_node->m_dl_handle;
    DL_GetInfo_Ex(dl_handle, &dl_info);
    if ( bUpdateParamOnly ) 
    {
        goto update_param;
    }

    MainForm->imgBLSupportUSB->Visible = false;
    MainForm->imgBROMSupportUSB->Visible = false;
    if(dl_info.brom_usb_supported)
    {
        MainForm->imgBROMSupportUSB->Visible = true;
        MainForm->imgBROMSupportUSB->Hint = AnsiString("The chip supports BROM USB Download.\n");
    }
    else if(dl_info.bootloader_usb_supported)
    {
        MainForm->imgBLSupportUSB->Visible = true;
        MainForm->imgBLSupportUSB->Hint = AnsiString("The MAUI load supports USB Download.\n") +
                                            AnsiString("(Please use UART cable to download Bootloader first)");
    }    
    
    // update scatter path
    //if(p_node->m_scat_file.Length() == 0 || p_node->m_scat_file == "")
    //{
    //    ed_LinkMapFile->Text = "";
    //}
    //else
    //{
        ed_LinkMapFile->Text = p_node->m_scat_file;
    //}

    switch(dl_info.boot_type)	
    {
        case NOR_BOOT:
            g_OperationMethod = NOR_OPERATION;
            break;
        case NAND_BOOT:
            g_OperationMethod = NFB_OPERATION;
            break;
        case EMMC_BOOT:
            g_OperationMethod = EMMC_OPERATION;
            break;
        default:
            g_OperationMethod = NOR_OPERATION;
            break;
    }
    
    updateMenuItem();
    updateStatusBar();

    if ( (S_DONE == DL_GetCount_Ex(dl_handle, &count)) && (0 < count) )
    {
        ROM_INFO_EX romInfoEx[MAX_LOAD_SECTIONS] = { 0 };
        if( !DL_Rom_GetInfoAll_Ex(dl_handle, romInfoEx, MAX_LOAD_SECTIONS) )
        {
            for ( int i=0; i < count; i++ )
            {
                AnsiString  region_prefix;
                pListItem = ListView1->Items->Add();
                pListItem->Caption = romInfoEx[i].name;
                pListItem->Checked = _TRUE==romInfoEx[i].enable?true:false;
                if ( OFFSET_VALUE == romInfoEx[i].region_addr_type) 
                {
                    region_prefix = "+";
                }
                else 
                {
                	region_prefix = "";
                }
                romInfoEx[i].rom_type;
                pListItem->SubItems->Add( region_prefix + "0x" + IntToHex((int)romInfoEx[i].region_addr, 8) );
                pListItem->SubItems->Add( "0x" + IntToHex((int)romInfoEx[i].begin_addr, 8) );
                pListItem->SubItems->Add( "0x" + IntToHex((int)romInfoEx[i].end_addr, 8) );
                pListItem->SubItems->Add(romInfoEx[i].filepath);

                // TODO:: how to deal with pListItem->Data in SV5???
                if ( ROM_VERSION_SV3 == dl_info.rom_version) 
                {
                    BOOT_TYPE bootType = NOR_BOOT;
                    if( S_DONE == DL_GetBootType(dl_handle, &bootType) )
                    {
                        if(NAND_BOOT == bootType)
                        {
                            // NFB: ROMs
                            pListItem->Data = &g_LV_ItemType[LV_NFB_DL_ROM];
                        }
                        else 
                        {
                            // NOR BOOT: BOOTLOADER and ROMs
                            if ( pListItem->Caption == AnsiString("BOOTLOADER") ) 
                            {
                                pListItem->Data = &g_LV_ItemType[LV_NOR_BOOTLOADER];
                            }
                            else 
                            {
                                pListItem->Data = &g_LV_ItemType[LV_NOR_DL_ROM];
                            }
                        }
                    }
                }
            }
        }
    }

update_param:
    // update parameter list view
    // add OTP parameter
    {
        pListItem = m_ParamListView->Items->Add();
        pListItem->Caption = "OTP";
        pListItem->Checked = p_node->m_param.m_otp_enable?true:false;
        s.printf("addr(0x%08X), len(%lu), %s, %s", 
                    p_node->m_param.m_otp.m_addr, 
                    p_node->m_param.m_otp.m_len, 
                    OTPOpToString(p_node->m_param.m_otp_op), 
                    p_node->m_param.m_otp_filepath.c_str());
        pListItem->SubItems->Add(s);
        pListItem->Data = &g_LV_ItemType[LV_OTP];
    }

    // add Security Setting
    if( dl_info.security_supported )
    {
        AnsiString	str_lock   ="";
        AnsiString	str_code   ="";
        AnsiString	str_sec_ro ="";
        AnsiString	str_fat    ="";
        AnsiString	str_restricted_area  ="";
        AnsiString	str_secure_cust_name ="";

        if ( p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_lockdown ) {
        	str_lock.printf("SecuritySetting(Lockdown), ");
        }
        else 
        {
            //str_lock.printf("SecuritySetting(Allow-Overwrite), ");
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_download ) 
        {
        	str_code = "W";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_readback )
        {
            if( 0 < str_code.Length() )
            {
                str_code += "/";
            }
            str_code += "R";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_code_format )
        {
            if ( 0 < str_code.Length() ) 
            {
                	str_code += "/";
            }
            str_code += "F";
        }

        if ( 0 == str_code.Length() ) 
        {
        	str_code = "Forbidden";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_download )
        {
            str_sec_ro = "W";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_sec_ro_readback )
        {
            if( 0 < str_sec_ro.Length() ) {
                str_sec_ro += "/";
            }
            str_sec_ro += "R";
        }

        if ( 0 == str_sec_ro.Length() ) {
        	str_sec_ro = "Forbidden";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_download ) {
        	str_fat = "W";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_readback )
        {
        	if( 0 < str_fat.Length() )
            {
                str_fat += "/";
            }
            str_fat += "R";
        }

        if ( !p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_disable_fat_format )
        {
        	if( 0 < str_fat.Length() )
            {
        		str_fat += "/";
        	}
        	str_fat += "F";
        }

        if ( 0 == str_fat.Length() ) {
        	str_fat = "Forbidden";
        }

        if ( p_node->m_param.m_ft_cfg.m_security_cfg.m_bit_ctrl.m_restricted_area_access_forbidden ) {
        	str_restricted_area.printf(", RestrictedArea(Forbidden)");
        }
        else {
        //			str_restricted_area.printf(", RestrictedArea(Allow-Overwrite)");
        }

        pListItem = m_ParamListView->Items->Add();
        pListItem->Caption = "Security Setting";
        pListItem->Checked = p_node->m_param.m_ft_cfg_enable?true:false;
        s.printf("%sCODE(%s), SEC_RO(%s), FAT(%s)%s%s", 
                    str_lock.c_str(), str_code.c_str(), str_sec_ro.c_str(), 
                    str_fat.c_str(), str_restricted_area.c_str(), 
                    str_secure_cust_name.c_str());
        pListItem->SubItems->Add(s);
        pListItem->Data = &g_LV_ItemType[LV_FTCFG];
    }
    else
    {
        p_node->m_param.m_ft_cfg_enable = (_BOOL) false;
    }


    // add SEC_RO m_parameters
    if( dl_info.rom_version == ROM_VERSION_SV3 &&
        dl_info.security_supported && 
        p_node->m_param.m_sec_ro.m_len > 0 )
    {
        pListItem = m_ParamListView->Items->Add();
        pListItem->Caption = "Secure RO Data";
        pListItem->Checked = p_node->m_param.m_sec_ro_enable?true:false;
        s.printf("len(%lu): %s", p_node->m_param.m_sec_ro.m_len, 
                    p_node->m_param.m_sec_ro_filepath.c_str());
        pListItem->SubItems->Add(s);
        pListItem->Data = &g_LV_ItemType[LV_SEC_RO];
    }
    else
    {
        p_node->m_param.m_sec_ro_enable = (_BOOL) false;
    }

    // add CUST_PARA m_parameters
    if( dl_info.rom_version == ROM_VERSION_SV3 &&
        dl_info.cust_para_supported )
    {
        pListItem = m_ParamListView->Items->Add();
        pListItem->Caption = "Custom Parameters";
        pListItem->Checked = p_node->m_param.m_cust_para_enable?true:false;
        s.printf("len(%lu): %s", p_node->m_param.m_cust_para.m_len, 
                   p_node->m_param.m_cust_para_filepath.c_str());
        pListItem->SubItems->Add(s);
        pListItem->Data = &g_LV_ItemType[LV_CUST_PARA];
    }
    else
    {
        p_node->m_param.m_cust_para_enable = (_BOOL) false;
    }

    // add GPS ROM to List View
    if( dl_info.rom_version == ROM_VERSION_SV3 &&
        p_node->m_param.m_gps_download )
    {
        pListItem = m_ParamListView->Items->Add();
        pListItem->Caption = "GPS ROM";
        pListItem->Checked = p_node->m_param.m_gps_enable?true:false;
        s.printf("len(%lu): %s", p_node->m_param.m_gps_rom_filesize, 
                    p_node->m_param.m_gps_rom_filepath.c_str());
        pListItem->SubItems->Add(s);
        pListItem->Data = &g_LV_ItemType[LV_GPS_ROM];
    }
    else
    {
        p_node->m_param.m_gps_enable = (_BOOL) false;
    }
}
//---------------------------------------------------------------------------
void TMainForm::RedrawDownloadCheckBox()
{
    CS_Sentry	cs(m_pUpdateListLock);
    unsigned short	count;

    if ( NULL == g_pCurNode ) return;

    DL_HANDLE_T &dl_handle = g_pCurNode->m_dl_handle;
            
    for ( int itemIndex=0; itemIndex < ListView1->Items->Count; itemIndex++ )
    {            
        ROM_INFO_EX	rom = { 0 };
        if ( S_DONE == DL_Rom_GetInfo_Ex(dl_handle, itemIndex, &rom) ) 
        {
            ListView1->Items->Item[itemIndex]->Checked = (_TRUE==rom.enable)?true:false;
        }
    }
}
//---------------------------------------------------------------------------
void TMainForm::RedrawReadbackList()
{

    unsigned short count;
	AnsiString	s;
	TListItem	*pListItem;

    lv_ReadBack->Items->Clear();

	if( !RB_GetCount(g_RB_HANDLE, &count) && 0<count )
    {

		RB_INFO	 *rb = new RB_INFO[count];

		if( NULL != rb )
        {
			if( !RB_GetInfoAll( g_RB_HANDLE, rb, count ) )
            {
				for( int i=0; i < count; i++ )
                {
					TListItem	*pListItem;
					pListItem = lv_ReadBack->Items->Add();
					pListItem->Checked = (_TRUE==rb[i].enable?true:false);

					if( NOR_OPERATION == g_OperationMethod )
                    {
						pListItem->Caption = "N/A";
					}
					else {

						switch( rb[i].m_read_flag )
                        {
						    case NUTL_READ_PAGE_ONLY:
							    pListItem->Caption = "PageOnly";
							    break;

						    case NUTL_READ_SPARE_ONLY:
							    pListItem->Caption = "SpareOnly";
							    break;

						    case NUTL_READ_PAGE_WITH_ECC:
							    pListItem->Caption = "PageWithECC";
							    break;

						    case NUTL_READ_PAGE_SPARE:
						    default:
							    pListItem->Caption = "PageSpare";
							    break;
						}

					}
					pListItem->SubItems->Add( "0x" + IntToHex((int)rb[i].readback_addr, 8) );
					pListItem->SubItems->Add( "0x" + IntToHex((int)rb[i].readback_len, 8) );
					pListItem->SubItems->Add( rb[i].filepath );
				}
			}
			delete [] rb;
		}
	}


	// update "Read" parameter list view
    m_ReadParamListView->Items->Clear();
	// add OTP parameters
	{
		pListItem = m_ReadParamListView->Items->Add();
		pListItem->Caption = "OTP";
		pListItem->Checked = g_ReadbackPage_Param.m_otp_enable?true:false;
		s.printf("addr(0x%08X), len(%lu), %s", g_ReadbackPage_Param.m_otp.m_addr, g_ReadbackPage_Param.m_otp.m_len, g_ReadbackPage_Param.m_otp_filepath.c_str());
	    pListItem->SubItems->Add(s);
		pListItem->Data = &g_LV_ItemType[LV_OTP];
	}
	// add SEC_RO m_parameters
	{
		pListItem = m_ReadParamListView->Items->Add();
		pListItem->Caption = "Secure RO Data";
		pListItem->Checked = g_ReadbackPage_Param.m_sec_ro_enable?true:false;
		s.printf("len(%lu): %s", g_ReadbackPage_Param.m_sec_ro.m_len, g_ReadbackPage_Param.m_sec_ro_filepath.c_str());
	    pListItem->SubItems->Add(s);
		pListItem->Data = &g_LV_ItemType[LV_SEC_RO];
	}
	// add CUST_PARA m_parameters
	{
		pListItem = m_ReadParamListView->Items->Add();
		pListItem->Caption = "Custom Parameters";
		pListItem->Checked = g_ReadbackPage_Param.m_cust_para_enable?true:false;
		s.printf("len(%lu): %s", g_ReadbackPage_Param.m_cust_para.m_len, g_ReadbackPage_Param.m_cust_para_filepath.c_str());
	    pListItem->SubItems->Add(s);
		pListItem->Data = &g_LV_ItemType[LV_CUST_PARA];
	}
}
//---------------------------------------------------------------------------
static int RB_IndexOfChangedCheckBox = -1;
static bool RB_bPreviousCheckBoxValue;

void __fastcall TMainForm::lv_ReadBackChange(TObject *Sender, TListItem *Item, TItemChange Change)
{
	// ShowMessage("Changed!");

    if( (ctState == Change) && (Item->Checked != RB_bPreviousCheckBoxValue) )
    {
        RB_IndexOfChangedCheckBox = Item->Index;
        return;
    }

    RB_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::lv_ReadBackChanging(TObject *Sender, TListItem *Item, TItemChange Change, bool &AllowChange)
{
	// ShowMessage("Changing!");

	RB_bPreviousCheckBoxValue = Item->Checked;

    RB_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::lv_ReadBackClick(TObject *Sender)
{
	TListView *plv = dynamic_cast<TListView *>(Sender);

	//	ShowMessage("Click!");

    if( 0<=RB_IndexOfChangedCheckBox && RB_IndexOfChangedCheckBox<plv->Items->Count )
    {
	    // update enabled flag according to checkbox
		RB_SetEnableAttr(g_RB_HANDLE, RB_IndexOfChangedCheckBox, plv->Items->Item[RB_IndexOfChangedCheckBox]->Checked?_TRUE:_FALSE);
        /*if(plv->Items->Item[RB_IndexOfChangedCheckBox]->Checked) {
			ShowMessage("Enable!");
        }
        else {
			ShowMessage("Disable!");
    	}*/
    }

    RB_IndexOfChangedCheckBox = -1;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::lv_ReadBackDblClick(TObject *Sender)
{
	TListView *plv = dynamic_cast<TListView *>(Sender);

	//	ShowMessage("DoubleClick!");

    if( 0<=RB_IndexOfChangedCheckBox && RB_IndexOfChangedCheckBox<plv->Items->Count )
    {
	    // update enabled flag according to checkbox
		RB_SetEnableAttr(g_RB_HANDLE, RB_IndexOfChangedCheckBox, plv->Items->Item[RB_IndexOfChangedCheckBox]->Checked?_TRUE:_FALSE);
        /*if(plv->Items->Item[RB_IndexOfChangedCheckBox]->Checked) {
			ShowMessage("Enable!");
        }
        else {
			ShowMessage("Disable!");
    	}*/
    }
	else {
		lv_ReadBack_oldDblClick(Sender);
    }

    RB_IndexOfChangedCheckBox = -1;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::ListView1InfoTip(TObject *Sender, TListItem *Item, AnsiString &InfoTip)
{
    static ROM_TYPE_EX romType= UNKNOWN_ROM;
    static ROM_INFO_EX	romInfoEx = {};
    static MTK_Resource	res_info = {};
    static MTK_JumpTable jmptbl_info = {};
    static char	custom_name[128], bbchip_name[128];

    char *p_filename=NULL;
    char path[512];
    AnsiString  filename;
    AnsiString	res_string;
    AnsiString	filepath="";

    InfoTip = "";

    if( NULL == g_pCurNode ) 
    {
        return;
    }

    DL_HANDLE_T &dl_handle = g_pCurNode->m_dl_handle;
    unsigned int romIndex = Item->Index;

    if(S_DONE != DL_Rom_GetInfo_Ex(dl_handle, romIndex, &romInfoEx) )
    {
        return;
    }

    romType = romInfoEx.rom_type;

    GetFullPathName(romInfoEx.filepath, sizeof(path), path, &p_filename);
    if( NULL!=p_filename && (strlen(romInfoEx.filepath)>(strlen(p_filename)+12)) ) 
    {
        filename.printf("%.12s...\\%s", path, p_filename);
    }
    else 
    {
        filename = romInfoEx.filepath;
    }
    
    if( RESOURCE_ROM == romType ) 
    {
        if( S_DONE == DL_Rom_GetResInfo(dl_handle, romIndex, &res_info) )
        {
            res_string.printf( "[%s]:  [%s]  (0x%08X), (0x%08X), (0x%08X + 0x%08X)=%u, magic(0x%08X) ",
                    res_info.m_prefix,
                    res_info.m_project_id,
                    res_info.m_type_ver,
                    res_info.m_base_addr,
                    res_info.m_table_size,
                    res_info.m_content_size,
                    res_info.m_table_size+res_info.m_content_size,
                    res_info.m_magic);
            InfoTip = " " + filename + "  (" + IntToStr(romInfoEx.filesize) + " bytes)\n\n" + res_string;    
        }                
    }
    else if( JUMP_TABLE_ROM == romType )
    {
        if( S_DONE==DL_GetJumpTableInfo(dl_handle, &jmptbl_info) )
        {
            InfoTip = " " + filename + "  (" + IntToStr(romInfoEx.filesize) + " bytes)\n";
            for ( int i=0; i < jmptbl_info.m_res_count; i++ )
            {
                res_string.printf( "\n[%d][%s]:  [%s]  (0x%08X), (0x%08X), (0x%08X + 0x%08X)=%u, magic(0x%08X) ",
                       i,
                       jmptbl_info.m_res[i].m_prefix,
                       jmptbl_info.m_res[i].m_project_id,
                       jmptbl_info.m_res[i].m_type_ver,
                       jmptbl_info.m_res[i].m_base_addr,
                       jmptbl_info.m_res[i].m_table_size,
                       jmptbl_info.m_res[i].m_content_size,
                       jmptbl_info.m_res[i].m_table_size + jmptbl_info.m_res[i].m_content_size,
                       jmptbl_info.m_res[i].m_magic);
                InfoTip += res_string;
            }
        }            
    }
    else if( V_BL_ROM <= romType && 
               V_MAUI_ROM_END > romType &&
               !strcmp(romInfoEx.name, "ROM"))
    {
        if( S_DONE != DL_GetCustomName(dl_handle, custom_name, sizeof(custom_name)) ||
            S_DONE != DL_GetBBChipName(dl_handle, bbchip_name, sizeof(bbchip_name)) )
        {
            custom_name[0] = '\0';
            bbchip_name[0] = '\0';
        }
        else
        {
            InfoTip.printf(" %s  (%u bytes), %s, %s ", 
                                filename.c_str(), romInfoEx.filesize, 
                                custom_name, bbchip_name);
        }
    }
    else 
    {
        InfoTip = " " + filename + "  (" + IntToStr(romInfoEx.filesize) + " bytes) ";
    }
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::m_CheckECOVersionClick(TObject *Sender)
{
	if ( g_bCheckECOVer ) {
        g_bCheckECOVer = false;
        NoMultiLoadClick(NULL);
	}
    else {
        g_bCheckECOVer = true;
	}

	updateMenuItem();
}
//---------------------------------------------------------------------------

int TMainForm::UpdateDA()
{
	int ret;
	AnsiString s;
	DA_INFO	da_info;

    memset(&da_info, 0x0, sizeof(da_info));

	if( S_DONE != (ret=DA_IsReady(g_DA_HANDLE, &da_info, _TRUE)) )
    {
		if( S_FTHND_FILE_IS_NOT_LOADED_YET == ret || S_FTHND_FILE_IS_UPDATED == ret )
        {
			if( S_DONE != ( ret = DA_Load(g_DA_HANDLE, da_info.filepath, g_bDebugDA?_FALSE:_TRUE)) )
            {
				if( S_INVALID_DA_FILE == ret ) {
					s = da_info.filepath;
					ShowMessage(" \"" + s + "\" is not a valid DA format!");
					return 1;
				}
				else {
					s = da_info.filepath;
					ShowMessage("Failed to load DA \"" + s + "\"!");
					return 2;
				}
			}
		}
		else {
			s = da_info.filepath;
			ShowMessage("Failed to check DA \"" + s + "\"!");
			return 3;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::action_AdvanceOptionExecute(TObject *Sender)
{
    if( g_bEnableAdvanceOption ) {
        BaseBandChip1->Visible = false;
        MCUClock1->Visible = false;
        m_Transmission_Option->Visible = false;
        m_COM_Port_Timeout->Visible = false;
        mi_EMI_Config->Visible = false;
		PacketLength1->Visible = false;
        m_CheckTgtResourceLayout->Visible = false;
        m_AutoLoadResByScat->Visible = false;
        m_DA_Option->Visible = false;
        m_ResourceOption->Visible = false;
        m_BRomStartCmdRetryCount->Visible = false;
        m_PullDownGPIO17->Visible = false;

        g_bEnableAdvanceOption = false;
	}
    else {
		BaseBandChip1->Visible = true;
        MCUClock1->Visible = true;
        m_Transmission_Option->Visible = true;
        m_COM_Port_Timeout->Visible = true;
        mi_EMI_Config->Visible = true;
		PacketLength1->Visible = true;
        m_CheckTgtResourceLayout->Visible = true;
        m_AutoLoadResByScat->Visible = true;
        m_DA_Option->Visible = true;
        m_ResourceOption->Visible = true;
        m_BRomStartCmdRetryCount->Visible = true;
        m_PullDownGPIO17->Visible = true;

        g_bEnableAdvanceOption = true;
	}

   	UpdateCaption();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::tb_DL_StopButtonClick(TObject *Sender)
{
    // reset stop flag
	g_stopflag = BOOT_STOP;

	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------

void TMainForm::DisableStopButton()
{
	// disable stop button
    tb_DL_StopButton->Enabled = false;
    RB_StopButton->Enabled = false;
}
//---------------------------------------------------------------------------
void TMainForm::EnableStopButton()
{
	// enable stop button
    tb_DL_StopButton->Enabled = true;
    RB_StopButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RB_StopButtonClick(TObject *Sender)
{
	g_stopflag = BOOT_STOP;
	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_CheckTgtResourceLayoutClick(TObject *Sender)
{
	if( g_bCheckTgtResourceLayout ) {
        g_bCheckTgtResourceLayout = false;
	}
    else {
        g_bCheckTgtResourceLayout = true;
	}

	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_AutoLoadResByScatClick(TObject *Sender)
{
	if( g_bAutoLoadResByScat ) {
        g_bAutoLoadResByScat = false;
	}
    else {
        g_bAutoLoadResByScat = true;
	}

	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_BRomStartCmdRetryCountClick(TObject *Sender)
{
	frm_BromStartCmdRetry->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_PullDownGPIO17Click(TObject *Sender)
{
	if ( g_bPullDownGPIO17 ) {
        g_bPullDownGPIO17 = false;
	}
    else {
        g_bPullDownGPIO17 = true;
	}

	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mi_EMI_ConfigClick(TObject *Sender)
{
	ext_mem1Form->Visible = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_COM_Port_TimeoutClick(TObject *Sender)
{
	COM_Option->Visible = true;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::m_SpeedUpBootROMBaudrateClick(TObject *Sender)
{
	if( g_SpeedUpBootROMBaudrate ) {
        g_SpeedUpBootROMBaudrate = false;
	}
    else {
        g_SpeedUpBootROMBaudrate = true;
	}
	updateMenuItem();
}
//---------------------------------------------------------------------------

bool TMainForm::FileSaveTestModeLogExecute()
{
    int iLastDel;
    AnsiString file_ext;

    Dialog_SaveTestModeLog->Filter = "Text files (*.txt)|*.TXT";
    if ( Dialog_SaveTestModeLog->Execute() )
    {
        TestModeLogFile = Dialog_SaveTestModeLog->FileName;
        iLastDel = TestModeLogFile.LastDelimiter(".");

        if ( 0 < iLastDel )
        {
	        file_ext = TestModeLogFile.SubString(iLastDel+1, 1000);
            if ( stricmp(file_ext.c_str(), "txt")) {
                TestModeLogFile += ".txt";
            }
        }
        else {
            TestModeLogFile += ".txt";
        }

        return true;
    }
    else {
        return false;
    }
   
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::FileSave_TestModeLogExecute(TObject *Sender)
{
    if( g_bDebugDA && FileSaveTestModeLogExecute() )
    {
        FILE    *fp;
        if( NULL != ( fp=fopen( TestModeLogFile.c_str(), "w+b" ) ) )
        {
            fwrite((void *)DebugDAWindow->Text.c_str(), 1, DebugDAWindow->Text.Length(), fp);
            fclose(fp);
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::m_CompareProjectIdClick(TObject *Sender)
{
	if( g_CompareResProjectId ) {
        g_CompareResProjectId = false;
	}
    else {
        g_CompareResProjectId = true;
	}
    DL_ResourceProjectIdComparisonSetting(g_pCurNode->m_dl_handle, g_CompareResProjectId?_TRUE:_FALSE);
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_FakeProgressTimerTimer(TObject *Sender)
{
	MainForm->CGauge1->Progress += 2;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_EnableNFICS1Click(TObject *Sender)
{
	if( g_bNFI_SelectCS1 ) {
        g_bNFI_SelectCS1 = false;
	}
    else {
        g_bNFI_SelectCS1 = true;
	}
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_Menu_OPM_NORClick(TObject *Sender)
{
    if( NOR_OPERATION == g_OperationMethod ) {
        return;
    }

    g_OperationMethod = NOR_OPERATION;
	updateMenuItem();
    updateStatusBar();
    RedrawReadbackList();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_Menu_OPM_NANDClick(TObject *Sender)
{
    // TODO:: useless code but having UI invisible menu. It should be removed
    /*
    if( NAND_OPERATION == g_OperationMethod ) {
        return;
    }

    g_OperationMethod = NAND_OPERATION;
	updateMenuItem();
    updateStatusBar();
    RedrawReadbackList();
    */
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_Menu_OPM_NFBClick(TObject *Sender)
{
    if( NFB_OPERATION == g_OperationMethod ) {
        return;
    }

    g_OperationMethod = NFB_OPERATION;
	updateMenuItem();
    updateStatusBar();
    RedrawReadbackList();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
   	delete m_pUpdateListLock;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::sb_MainDrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect)
{
    AnsiString  s;

    if( 1 != Panel->Index ) {
        return;
    }

    StatusBar->Canvas->Font->Color = (TColor) 0x00000000;

	switch( g_OperationMethod )
    {
	    case NOR_OPERATION:
		    s = " NOR";
		    break;

	    case NFB_OPERATION:
            StatusBar->Canvas->Font->Color = (TColor) 0x00A00070;
		    s = " NFB";
		    break;

	    default:
		    s = "??";
		    break;
	}

    StatusBar->Canvas->Font->Style = TFontStyles() << fsBold;
    StatusBar->Canvas->Font->Name = "Arial";
    StatusBar->Canvas->Font->Size = 9;
    StatusBar->Canvas->TextOutA(Rect.Left+2, Rect.Top+2, s);
}
//---------------------------------------------------------------------------
void TMainForm::HWStatusBarClear()
{
    MainForm->sb_Main->Panels->Items[5]->Text = "";
    MainForm->sb_Main->Hint = "";
    MainForm->m_HWStatus->Panels->Items[0]->Text = "";
    MainForm->m_HWStatus->Panels->Items[1]->Text = "";
    MainForm->m_HWStatus->Panels->Items[2]->Text = "";
    MainForm->m_HWStatus->Panels->Items[3]->Text = "";
    MainForm->m_HWStatus->Hint = "";

    // reset download time
    sb_Main->Panels->Items[4]->Text = " 0:00 sec ";
    g_CostTimeSec = 0;  // reset download time
    g_ProgressTimeSec = 0;
}
//---------------------------------------------------------------------------
void TMainForm::HWStatusBarUpdate( const DA_REPORT_T *p_da_report,
                                   const char *p_nor_flash_hint,
                                   const char *p_nand_flash_hint,
                                   const char *m_hw_detection_report)
{

    if( NULL == p_da_report ) {
        return;
    }

    AnsiString  s;

    // selected DL_HANDLE name
    if( NULL != p_da_report->m_dl_handle )
    {
        char cust_name[1024]={0};
        if( S_DONE == DL_GetCustomName( p_da_report->m_dl_handle, cust_name, sizeof(cust_name) ) )
        {
            s.printf( "Project: %s", cust_name);
            MainForm->m_HWStatus->Panels->Items[2]->Text = s;
        }
    }

    // bbchip info
    MainForm->m_HWStatus->Panels->Items[0]->Text = p_da_report->m_bbchip_name;

    // ram
    AnsiString	str_ext_ram_size;
    AnsiString	str_ext_ram_type;
    double		size_in_double;

    size_in_double = p_da_report->m_ext_ram_size*8;

    if ( 0x100000 <= size_in_double )
    {
        str_ext_ram_size.printf("%dMb", (unsigned int)(size_in_double/0x100000));
    }
    else 
    {
        str_ext_ram_size.printf("%dKb", (unsigned int)(size_in_double/0x400));
    }

	switch ( p_da_report->m_ext_ram_type )
    {
	    case HW_RAM_SRAM:
		    str_ext_ram_type = "SRAM";
		    break;

	    case HW_RAM_DRAM:
		    str_ext_ram_type = "DRAM";
		    break;

	    case HW_RAM_UNKNOWN:
	    default:
		    str_ext_ram_type = "RAM: N/A";
		    break;
	}

	if( S_DONE == p_da_report->m_ext_ram_ret ) {
		s.printf( "%s %s", str_ext_ram_size.c_str(), str_ext_ram_type.c_str() );
	}
	else {
		s = str_ext_ram_type;
	}

	MainForm->m_HWStatus->Panels->Items[1]->Text = s;

	// NOR flash
	s.printf("NOR: %s", p_nor_flash_hint);
	MainForm->sb_Main->Panels->Items[5]->Text    = s;

	// NAND flash
	s.printf("NAND: %s", p_nand_flash_hint);
	MainForm->m_HWStatus->Panels->Items[3]->Text = s;

	// set hint
	MainForm->m_HWStatus->Hint = m_hw_detection_report;

	// Multi-Load
	if ( MultiLoad->Checked || MultiLoadWithMMAA->Checked )
    {
		for ( unsigned int index = 0; index < g_DL_HANDLE_LIST_nodes.size(); index++ )
        {
			if( g_DL_HANDLE_LIST_nodes[index].m_dl_handle == p_da_report->m_dl_handle )
            {
                        TTreeNode * node    = g_rootLoad->Item[index];
                        node->ImageIndex    = 2;
                        node->SelectedIndex = 2;
                        node->Selected = true;
                        MultiLoadNode *downloadNode = new MultiLoadNode();
                        downloadNode->m_dl_handle =  p_da_report->m_dl_handle;
                        downloadNode->m_scat_file =  g_DL_HANDLE_LIST_nodes[index].m_scat_file;
                        downloadNode->m_param     =  g_DL_HANDLE_LIST_nodes[index].m_param;
                        RedrawDownloadList(downloadNode, false);
                        delete(downloadNode);
                        break;
			}
		}
	}
}


//---------------------------------------------------------------------------

bool TMainForm::LoadByDownloadSet(const DownloadSet &dl_set, MultiLoadNode  *p_node)
{
    unsigned int i;
    AnsiString	err;
    
    if( NULL ==  p_node ) {
    	return false;
    }

    DL_HANDLE_T &dl_handle = p_node->m_dl_handle;

    // check if scatter file exists
    if( INVALID_FILE_ATTRIBUTES == GetFileAttributes(dl_set.m_Scatter.c_str()) ) {
        // pop error message
        err.printf("Scatter doesn't exit!\n\n%s", dl_set.m_Scatter.c_str());
        ShowMessage(err);
        return false;
    }

    // check if all the selected bin files exist
    for ( i=0; i<dl_set.m_BinFiles.size(); i++ )
    {
        if ( dl_set.m_BinFiles[i].m_Enable &&
        	(0 < dl_set.m_BinFiles[i].m_Filepath.Length()) &&
              INVALID_FILE_ATTRIBUTES==GetFileAttributes(dl_set.m_BinFiles[i].m_Filepath.c_str()) ) 
        {
        	// pop error message
        	err.printf("File[%d] doesn't exit!\n\n%s", i, dl_set.m_BinFiles[i].m_Filepath.c_str());
        	ShowMessage(err);
        	return false;
        }
    }

    if (!CreateROMEntries(dl_set.m_Scatter,
                          false, // Do not auto load ROM files
                          dl_handle))
    {
        return false;
    }

    p_node->m_scat_file = dl_set.m_Scatter;

    // update scatter file's base directory
    if ( 0 < p_node->m_scat_file.Length() )
    {
        int iLastDel = p_node->m_scat_file.LastDelimiter("\\:");
        if ( 0 < iLastDel )
        {
        	ScatBaseDir = p_node->m_scat_file.SubString(1, iLastDel-1 );
        }
    }

    // load bin files
    for ( unsigned int i=0; i < dl_set.m_BinFiles.size(); i++ )
    {
        if( dl_set.m_BinFiles[i].m_Enable &&
           (0<dl_set.m_BinFiles[i].m_Filepath.Length()) &&
            S_DONE != DL_Rom_Load_Ex(dl_handle, i, dl_set.m_BinFiles[i].m_Filepath.c_str()) ) 
        {
            // pop error message
            err.printf("Failed to load File[%d]!\n\n%s", i, dl_set.m_BinFiles[i].m_Filepath.c_str());
            ShowMessage(err);
            return false;
        }
        
        if(mi_USBDownloadReadback->Checked && !BootROMUSBSupported(dl_handle))
        {
            ROM_INFO_EX romInfoEx = {};
            if( S_DONE == DL_Rom_GetInfo_Ex(dl_handle, i, &romInfoEx))
            {
                if(ARM_BL_ROM == romInfoEx.rom_type)
                {
                    DL_Rom_SetEnableAttr_Ex(dl_handle, i, _FALSE);
                }
            }    
        }
        else
        {
            DL_Rom_SetEnableAttr_Ex(dl_handle, i, dl_set.m_BinFiles[i].m_Enable?_TRUE:_FALSE);
        }
        // update ROM file's base directory
        if( 0==i && 0<dl_set.m_BinFiles[i].m_Filepath.Length() )
        {
            int iLastDel = dl_set.m_BinFiles[i].m_Filepath.LastDelimiter("\\:");
            if( 0 < iLastDel ) 
            {
            	    ROMBaseDir = dl_set.m_BinFiles[i].m_Filepath.SubString(1, iLastDel-1 );
            }
        }
    }

    DL_INFO_EX dl_info = {};
    /*if(S_DONE !=  DL_GetInfo_Ex(dl_handle, &dl_info))
    {
        ShowMessage("Failed to get DL_INFO_EX!");
        return false;
    }
    else
    {     
        MainForm->imgBLSupportUSB->Visible = false;
        MainForm->imgBROMSupportUSB->Visible = false;
        if(dl_info.brom_usb_supported)
        {
           MainForm->imgBROMSupportUSB->Visible = true;
           MainForm->imgBROMSupportUSB->Hint = AnsiString("The chip supports BROM USB Download.\n");
        }
        else if(dl_info.bootloader_usb_supported)
        {
            MainForm->imgBLSupportUSB->Visible = true;
            MainForm->imgBLSupportUSB->Hint = AnsiString("The MAUI load supports USB Download.\n") +
                                            AnsiString("(Please use UART cable to download Bootloader first)");
        }
    }*/
    //FIXME
    if(S_DONE ==  DL_GetInfo_Ex(dl_handle, &dl_info))
    {
        MainForm->imgBLSupportUSB->Visible = false;
        MainForm->imgBROMSupportUSB->Visible = false;
        if(dl_info.brom_usb_supported)
        {
           MainForm->imgBROMSupportUSB->Visible = true;
           MainForm->imgBROMSupportUSB->Hint = AnsiString("The chip supports BROM USB Download.\n");
        }
        else if(dl_info.bootloader_usb_supported)
        {
            MainForm->imgBLSupportUSB->Visible = true;
            MainForm->imgBLSupportUSB->Hint = AnsiString("The MAUI load supports USB Download.\n") +
                                            AnsiString("(Please use UART cable to download Bootloader first)");
        }
    }
    // update Parameter List    
    if( ROM_VERSION_SV3 == dl_info.rom_version)
    {
        //---------------------------------------------------------
        // update Parameter List
        AnsiString tmp_dir;
        AnsiString tmp_file;

        // initialize Check Box of SEC_RO and CUST_PARA
        tmp_dir = ExtractFileDir(p_node->m_scat_file);

      
        // Load SEC_RO
        
        p_node->m_param.m_sec_ro_enable     = _TRUE;
        p_node->m_param.m_sec_ro_filepath   = "";

        tmp_file = tmp_dir + "\\SECURE_RO";
        if ( FileExists(tmp_file) )
        {
     		if ( !LoadFile(tmp_file.c_str(), &p_node->m_param.m_sec_ro.m_data, &p_node->m_param.m_sec_ro.m_len, NULL) )
            {
                p_node->m_param.m_sec_ro_filepath = tmp_file;
            }
        }

        // Load CUST_PARA
        p_node->m_param.m_cust_para_enable = _TRUE;
        p_node->m_param.m_cust_para_filepath = "";

        tmp_file = tmp_dir + "\\CUST_PARA";
        if ( FileExists(tmp_file) )
        {
     		if( !LoadFile(tmp_file.c_str(), &p_node->m_param.m_cust_para.m_data, &p_node->m_param.m_cust_para.m_len, NULL) )
            {
                p_node->m_param.m_cust_para_filepath = tmp_file;
            }
        }

        // Check if GPS Project exists
        AnsiString gps_dir = tmp_dir + "\\GPS";
        if( DirectoryExists(gps_dir) )
        {
            unsigned int bFindGpsRom = 0;

            p_node->m_param.m_gps_download = (_BOOL) true;

            AnsiString gps_rom = gps_dir + "\\M-core";
            AnsiString gps_ini = gps_dir + "\\gps.ini";

            if ( FileExists(gps_rom) ) {
                bFindGpsRom = 1;
            }
            else
            {
                TSearchRec sr;
                if ( FindFirst(gps_dir+"\\*.bin", 0, sr) == 0 )
                {
                    do
                    {
                        gps_rom = gps_dir + "\\" + sr.Name;
                        bFindGpsRom = 1;
                        break;
                    } while( FindNext(sr) == 0 );

                    FindClose(sr);
                }
            }

            if ( bFindGpsRom && FileExists(gps_ini) )
            {
                p_node->m_param.m_gps_enable        = (_BOOL) true;
                p_node->m_param.m_gps_rom_dirpath   = gps_dir;
                p_node->m_param.m_gps_rom_filepath  = gps_rom;
                p_node->m_param.m_gps_ini_filepath  = gps_ini;

                LoadFileInfo(gps_rom.c_str(), &p_node->m_param.m_gps_rom_filesize, NULL);

                TIniFile *ini = new TIniFile( gps_ini );
            	p_node->m_param.m_gps_power_pin     = ini->ReadInteger( "GPIO", "gpio_gps_power_pin", 0xFFFF );
                p_node->m_param.m_gps_reset_pin     = ini->ReadInteger( "GPIO", "gpio_gps_reset_pin", 0xFFFF );
                p_node->m_param.m_gps_standby_pin   = ini->ReadInteger( "GPIO", "gpio_gps_standby_pin", 0xFFFF );
                p_node->m_param.m_gps_32k_pin       = ini->ReadInteger( "GPIO", "gpio_gps_32k_pin", 0xFFFF );
            }
            else{
                p_node->m_param.m_gps_enable        = (_BOOL) false;
            }
        }
        else{
            p_node->m_param.m_gps_download = (_BOOL) false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::File1Click(TObject *Sender)
{
	int i;

    TMenuItem * DL_History[MAX_DOWNLOAD_HISTORY] = {
    	 m_DL_History_0
    	,m_DL_History_1
    	,m_DL_History_2
    	,m_DL_History_3
    	,m_DL_History_4
    	,m_DL_History_5
    	,m_DL_History_6
    	,m_DL_History_7
    	,m_DL_History_8
    	,m_DL_History_9
	};

	// invisible all history
	for(i=0; i<MAX_DOWNLOAD_HISTORY; i++) {
		DL_History[i]->Visible = false;
	}

	// update
	if(!g_bDebugDA) {
		DownloadHistoryList::iterator	iter;
		for(i=0, iter=g_DownloadHistory.begin(); iter!=g_DownloadHistory.end(); i++, iter++) {
	   	    DL_History[i]->Caption.printf("%d: %s", i+1, iter->m_BinFiles[0].m_Filepath.c_str());
	        DL_History[i]->Visible = true;
	        DL_History[i]->OnClick = OnDownloadHistoryClick;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnDownloadHistoryClick(TObject *Sender)
{
    int index;
    int i;

    // find download history index
	TMenuItem * cur_item = (TMenuItem *)Sender;
    AnsiString	title=cur_item->Caption.c_str();

    // Caption = "X&XXXXXXXXXX", delete '&' char
    char *s=title.c_str();
    char *p=strchr(s, '&');

    if( NULL != p ) {
    	title.Delete(p-s+1, 1);
    }
    index = (atoi(title.c_str())-1);

    // move current index to the top of history
    DownloadHistoryList::iterator	iter;

    for ( i=0, iter=g_DownloadHistory.begin(); 
           0!=index && iter!=g_DownloadHistory.end(); 
           i++, iter++)
    {
        if( i == index ) 
        {
            DownloadSet dl_set = (*iter);
            g_DownloadHistory.erase(iter);
            g_DownloadHistory.push_front(dl_set);
            break;
        }
    }

    // FIX CR: MAUI_02143992
    if ( MultiLoad->Checked || MultiLoadWithMMAA->Checked )
    {
         // while in MutiLoad or MMAA, add a load to tree view
        MultiLoadNode	node ;   
        // Create DL_HANDLE instance
        DL_Create(&node.m_dl_handle);
        DownloadSet	dl_set = *(g_DownloadHistory.begin());
        if( !Parse(&node, dl_set.m_Scatter))
        {
            AnsiString err;
            err.printf("Fail to parse scatter file.\n");
            ShowMessage(err);
            return;
        }
        // Get BB/Flash Name, and add this Load into List
        if(!AddaLoadInternalMain(&node))
        {
            return;
        }		
    }                 
    else
    {
        // load by download history
        if(!LoadByDownloadSet(g_DownloadHistory.front(), g_pCurNode)) 
        {
            // delete history that doesn't exist anymore
            g_DownloadHistory.pop_front();
            return;
        }

        // update list view
        RedrawReadbackList();
        RedrawDownloadList(g_pCurNode, false);

        DL_HANDLE_T &dl_handle=g_pCurNode->m_dl_handle;

        // show Parameter List View
        DL_INFO_EX dl_info = {};
        if( S_DONE != DL_GetInfo_Ex(dl_handle, &dl_info) )
        {
            if( dl_info.security_supported || 
                ( ROM_VERSION_SV3 == dl_info.rom_version &&
                g_pCurNode->m_param.m_gps_download ))
            {
                m_ParamListView->Visible = true;
            }
            else
            {
                m_ParamListView->Visible = false;
            } 
        }
    }
    
    return;    
}

//---------------------------------------------------------------------------
/*void __fastcall TMainForm::mi_MultiLoadClick(TObject *Sender)
{
	if( mi_MultiLoad->Checked ){
		mi_MultiLoad->Checked = false;
		TreeView1->Visible = false;
		g_pCurNode = &g_DL_HANDLE_NODE;
	}
	else{
		mi_MultiLoad->Checked = true;
		TreeView1->Visible = true;
        g_rootLoad->Selected = true;
		g_pCurNode = NULL;
	}

	RedrawReadbackList();
	RedrawDownloadList(g_pCurNode, false);
}    */
//---------------------------------------------------------------------------

void __fastcall TMainForm::AddaLoad1Click(TObject *Sender)
{
    // Update MMAA Enable
    UpdateMMAAEnable(g_bMMAAEnable);

    MultiLoadNode	node;
    //DL_HANDLE_T dl_handle;
    //DL_INFO_EX dl_info = {};
    // Create DL_HANDLE instance
    DL_Create(&node.m_dl_handle);

	// load by scatter file
    if(!FileOpenLinkMapExecute(&node)) 
    {
        goto error;
    }

    // Get BB/Flash Name, and add this Load into List
    if(AddaLoadInternalMain(&node)){
        goto end;
    } else {
        goto error;
    }

error:
	DL_Destroy(&node.m_dl_handle);
end:

}

bool __fastcall TMainForm::AddaLoadInternalMain(MultiLoadNode *p_node)
{
    MultiLoadNode	&node = *p_node;

    //-----------------------------------------
    // Multi-Load ( w/ or w/o Multi-Memory)

    /* 1. Get BB/Flash Name */
	char custom_name[128];
    char flash_ID[30];
    char flash_name[40];
    char project_flashID[200];

    int retFlashName = OpenAuthGetFlashIdName(&node, custom_name, flash_ID, flash_name, project_flashID);

	/* 2. Add this DL_HANDLE into DL_HANDLE_List */
    if(!OpenAuthAddIntoDLList(&node, AnsiString(custom_name), AnsiString(flash_name), AnsiString(flash_ID), retFlashName)){
        return false;
    }

    /* 3. Add this Node into UI Node List */
    g_DL_HANDLE_LIST_nodes.push_back(node);    //binding

    /* 4. Add this Node into UI TreeView Child */
    OpenAuthAddIntoTreeView(&node,AnsiString(project_flashID));

    /* 5. Add this Node into UI MultiLoad History */
    OpenAuthAddIntoMultiHistory(&node);

    return true;
}

bool __fastcall TMainForm::OpenAuthAddIntoDLList(MultiLoadNode *p_node, const AnsiString asCustomName, const AnsiString asFlashName, const AnsiString asFlashID, int retFlashName)
{
    MultiLoadNode	&node = *p_node;

	/* Add this DL_HANDLE into DL_HANDLE_List */
    int ret;
    ret = DL_AddHandleToList(g_DL_HANDLE_LIST, node.m_dl_handle);

	if( S_DONE != ret)
    {
        // ------------------------------
        // Fix @MAUI_02067291 from Deepak
        if( NULL == g_pLastNode )
        {
            g_pLastNode = &g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index];
        }

		// re-draw ListView
		RedrawReadbackList();
		RedrawDownloadList(g_pLastNode, false);
        // ------------------------------


        switch(ret)
        {
            case S_DLIST_SAME_BBCHIP_SW_VER:
//                ShowMessage("Load for " + AnsiString(custom_name) + " already exists!");
                ShowMessage("Load for " + asCustomName + " already exists!");
                break;
            case S_DLIST_SAME_FLASHID_AND_BBCHIP_SW_VER:
                if(retFlashName == S_DONE)
                {
//                    ShowMessage("Load for "+  AnsiString(custom_name) + " project and " + AnsiString(flash_name)+" flash already exists!");
                    ShowMessage("Load for "+  asCustomName + " project and " + asFlashName +" flash already exists!");
                }
                else
                {
//                    ShowMessage("Load for " + AnsiString(custom_name) + " project and " + AnsiString(flash_ID) + " flash ID already exists!");
                    ShowMessage("Load for " + asCustomName + " project and " + asFlashID + " flash ID already exists!");
                }
                break;
        }
//        g_rootLoad->Selected = true;
		return false;
	}

    return true;
}

void __fastcall TMainForm::OpenAuthAddIntoTreeView(MultiLoadNode *p_node, const AnsiString asProjectFlashID)
{

	/* Add this Load into UI TreeView Child */
    TTreeNode * nLoad;
    AnsiString projectFlashID = asProjectFlashID;

    if(DL_IsEnctrypedBin(p_node->m_dl_handle))
    {
        // EndUser download load
        projectFlashID = " [Encrypted Bins]";
    }
    else if( asProjectFlashID == " []")
    {
        projectFlashID = " [Unknown Bins]";
    }

    
      nLoad = TreeView1->Items->AddChild(g_rootLoad, projectFlashID);    
	nLoad->ImageIndex = 1;
	nLoad->SelectedIndex = 1;
	nLoad->Data = &g_DL_HANDLE_LIST_nodes[g_DL_HANDLE_LIST_nodes.size()-1];
	g_rootLoad->Expand(true);

	nLoad->Selected = true;
	g_pCurNode = (MultiLoadNode *)nLoad->Data;
 	RedrawReadbackList();
	RedrawDownloadList(g_pCurNode, false);

}

void __fastcall TMainForm::OpenAuthAddIntoMultiHistory(MultiLoadNode *p_node)
{
    //
    //  Add this Load into UI MultiLoad History
    // 
    MultiLoadNode	&node = *p_node;    
    DownloadSet dl_set;
    dl_set.m_Scatter = node.m_scat_file;

    unsigned short count=0;
    if( ( S_DONE == DL_GetCount_Ex(node.m_dl_handle, &count) ) && (0<count) )
    {
        ROM_INFO_EX	rom[MAX_LOAD_SECTIONS];
        if( S_DONE == DL_Rom_GetInfoAll_Ex(node.m_dl_handle, rom, MAX_LOAD_SECTIONS) )
        {
            for( int i=0; i < count; i++ )
            {
                DownloadBinFile		dl_bin;
                dl_bin.m_Filepath = rom[i].filepath;
                dl_bin.m_Enable   = rom[i].enable?true:false;
                dl_set.m_BinFiles.push_back( dl_bin );
            }
        }
    }
    g_MultiLoadHistory.push_back(dl_set);
}

//---------------------------------------------------------------------------
bool TMainForm::LoadByMultiLoadSet(MultiLoadHistoryList *multiLoadList)
{
    MultiLoadHistoryList::iterator multiLoaditer;
    for ( multiLoaditer= g_MultiLoadHistory.begin(); 
           multiLoaditer!=g_MultiLoadHistory.end();
           multiLoaditer++)
    {
        TTreeNode * nLoad;
        MultiLoadNode node;
        char   custom_name[128];
        char   project_flashID[170];
        char   flash_ID[30], flash_name[40];
        int ret,flashNameret ;

        // check if scatter file exists
        if( INVALID_FILE_ATTRIBUTES == GetFileAttributes(multiLoaditer->m_Scatter.c_str()))
        {
            // pop error message
            ShowMessage("Scatter doesn't exit!\n\n%s" + multiLoaditer->m_Scatter);
            return false;
        }
        
        // check if all the bin files exist
        for ( unsigned int i=0; i< multiLoaditer->m_BinFiles.size(); i++ )
        {
            if ( INVALID_FILE_ATTRIBUTES == GetFileAttributes(multiLoaditer->m_BinFiles[i].m_Filepath.c_str()) &&
                 multiLoaditer->m_BinFiles[i].m_Enable &&
            	   (0 < multiLoaditer->m_BinFiles[i].m_Filepath.Length()))
             {
                // pop error message
                ShowMessage("File[%d] doesn't exit!\n\n%s" + multiLoaditer->m_BinFiles[i].m_Filepath);
                return false;
             }
        }

        UpdateMMAAEnable(g_bMMAAEnable);

         // Create DL_HANDLE instance
        DL_Create(&node.m_dl_handle);
        DL_HANDLE_T &dl_handle = node.m_dl_handle;

        if (!CreateROMEntries(multiLoaditer->m_Scatter,
                              false, // Do not auto load ROM files
                              dl_handle))
        {
            return false;
        }

        node.m_scat_file = multiLoaditer->m_Scatter;

        // load bin files
        for ( unsigned int i=0; i < multiLoaditer->m_BinFiles.size(); i++ )
        {
            if( multiLoaditer->m_BinFiles[i].m_Enable &&
               (0 < multiLoaditer->m_BinFiles[i].m_Filepath.Length()) &&
                S_DONE != DL_Rom_Load_Ex(dl_handle, i, multiLoaditer->m_BinFiles[i].m_Filepath.c_str()) ) 
            {
                // pop error message
                AnsiString	err;
                err.printf("Failed to load File[%d]!\n\n%s", i, multiLoaditer->m_BinFiles[i].m_Filepath.c_str());
                ShowMessage(err);
                return false;
            }
            
            if(mi_USBDownloadReadback->Checked && !BootROMUSBSupported(dl_handle))
            {
                ROM_INFO_EX romInfoEx = {};
                if( S_DONE == DL_Rom_GetInfo_Ex(dl_handle, i, &romInfoEx))
                {
                    if(ARM_BL_ROM == romInfoEx.rom_type)
                    {
                        DL_Rom_SetEnableAttr_Ex(dl_handle, i, _FALSE);
                    }
                }    
            }
            else
            {
                DL_Rom_SetEnableAttr_Ex(dl_handle, i, multiLoaditer->m_BinFiles[i].m_Enable?_TRUE:_FALSE);
            }
            
            // update ROM file's base directory
            if( 0==i && 0 < multiLoaditer->m_BinFiles[i].m_Filepath.Length() )
            {
                int iLastDel = multiLoaditer->m_BinFiles[i].m_Filepath.LastDelimiter("\\:");
                if( 0 < iLastDel ) 
                {
                	    ROMBaseDir = multiLoaditer->m_BinFiles[i].m_Filepath.SubString(1, iLastDel-1 );
                }
            }
        }

        /*
        for ( unsigned int j=0; j < multiLoaditer->m_BinFiles.size(); j++ )
        {
             DL_Rom_SetEnableAttr_Ex(dl_handle, j, 
                            multiLoaditer->m_BinFiles[j].m_Enable?_TRUE:_FALSE);
        }
        
        if(mi_USBDownloadReadback->Checked && !BootROMUSBSupported(dl_handle))
        {
            // Disable bootloader is USB DL/RB is enabled
            unsigned short count = 0;
            if( ( S_DONE == DL_GetCount_Ex(dl_handle, &count) ) && (0 < count) )
            {       
                ROM_INFO_EX romInfoEx[MAX_LOAD_SECTIONS];
                if( S_DONE == DL_Rom_GetInfoAll_Ex(dl_handle, romInfoEx, MAX_LOAD_SECTIONS))
                {
                    for( int index = 0; index < count; index++)
                    {
                        if(ARM_BL_ROM == romInfoEx[index].rom_type)
                        {
                            DL_Rom_SetEnableAttr_Ex(dl_handle, index, _FALSE);
                        }
                    }
                }    
            }                       
        }
        */
        //---------------------------------------------------------

        DL_INFO_EX dl_info = {};
        if(S_DONE != DL_GetInfo_Ex(dl_handle, &dl_info))
        {
            ShowMessage("DL_GetInfo_Ex() failed!");
            return false;
        }
        
        // update Parameter List
        AnsiString tmp_dir = ExtractFileDir(multiLoaditer->m_Scatter);;
        AnsiString tmp_file;

        // 1. Load SEC_RO
        node.m_param.m_sec_ro_enable   = _TRUE;
        node.m_param.m_sec_ro_filepath = "";
        node.m_param.m_sec_ro.m_len    = 0;

        tmp_file = tmp_dir + "\\SECURE_RO";
        if( ROM_VERSION_SV3 == dl_info.rom_version &&
            FileExists(tmp_file) )
        {
            if( !LoadFile(tmp_file.c_str(), &node.m_param.m_sec_ro.m_data, 
                              &node.m_param.m_sec_ro.m_len, NULL) )
            {
                node.m_param.m_sec_ro_filepath = tmp_file;
            }
        }

        // 2. Load CUST_PARA
        node.m_param.m_cust_para_enable   = _TRUE;
        node.m_param.m_cust_para_filepath = "";
        node.m_param.m_cust_para.m_len    = 0;

        tmp_file = tmp_dir + "\\CUST_PARA";
        if( ROM_VERSION_SV3 == dl_info.rom_version &&
            FileExists(tmp_file) )
        {
            if( !LoadFile(tmp_file.c_str(), &node.m_param.m_cust_para.m_data, 
                               &node.m_param.m_cust_para.m_len, NULL) )
            {
                node.m_param.m_cust_para_filepath = tmp_file;
            }
        }

        // 3. Load GPS Project
        // Check if GPS Project exists
        AnsiString gps_dir = tmp_dir + "\\GPS";
        if( ROM_VERSION_SV3 == dl_info.rom_version &&
            DirectoryExists(gps_dir) )
        {
            unsigned int bFindGpsRom = 0;

            node.m_param.m_gps_download = (_BOOL) true;

            AnsiString gps_rom = gps_dir + "\\M-core";
            AnsiString gps_ini = gps_dir + "\\gps.ini";

            if( FileExists(gps_rom) ) {
                bFindGpsRom = 1;
            }
            else
            {
                // search *.bin
                TSearchRec sr;
                if ( FindFirst(gps_dir+"\\*.bin", 0, sr) == 0 )
                {
                    do
                    {
                        gps_rom = gps_dir + "\\" + sr.Name;
                        bFindGpsRom = 1;
                        break;
                    } while ( FindNext(sr) == 0 );
                    FindClose(sr);
                }
            }

            if( bFindGpsRom && FileExists(gps_ini) )
            {
                node.m_param.m_gps_enable        = (_BOOL) true;
                node.m_param.m_gps_rom_dirpath   = gps_dir;
                node.m_param.m_gps_rom_filepath  = gps_rom;
                node.m_param.m_gps_ini_filepath  = gps_ini;

                LoadFileInfo(gps_rom.c_str(), &node.m_param.m_gps_rom_filesize, NULL);

                TIniFile *ini = new TIniFile( gps_ini );
            	node.m_param.m_gps_power_pin     = ini->ReadInteger( "GPIO", "gpio_gps_power_pin",   0xFFFF );
                node.m_param.m_gps_reset_pin     = ini->ReadInteger( "GPIO", "gpio_gps_reset_pin",   0xFFFF );
                node.m_param.m_gps_standby_pin   = ini->ReadInteger( "GPIO", "gpio_gps_standby_pin", 0xFFFF );
                node.m_param.m_gps_32k_pin       = ini->ReadInteger( "GPIO", "gpio_gps_32k_pin",     0xFFFF );
            }
            else
            {
                node.m_param.m_gps_enable   = (_BOOL) false;
            }
        }
        else
        {
            node.m_param.m_gps_download = (_BOOL) false;
        }

        RedrawReadbackList();
        RedrawDownloadList(&node, false);

    // show Parameter List View
    if( dl_info.security_supported || 
        ( ROM_VERSION_SV3 == dl_info.rom_version &&
          node.m_param.m_gps_download ))
    {
        m_ParamListView->Visible = true;
    }
    else
    {
        m_ParamListView->Visible = false;
    }
    
    DL_GetCustomName(node.m_dl_handle, custom_name, sizeof(custom_name));
    DL_GetFlashDeviceCode(node.m_dl_handle,flash_ID);

    strcpy(project_flashID,custom_name);

    //If MMAA is enabled, display both project name and flash ID, else display only project name
    if(g_bMMAAEnable == true || g_bMultiMemoryFlag == true)
    {
        if(S_DONE == (flashNameret = DL_GetFlashName(node.m_dl_handle,flash_name, sizeof(flash_name))))
        {
            strcat(project_flashID, " [");
            strcat(project_flashID,flash_name);
            strcat(project_flashID,"]");
        }
        else
        {
            strcat(project_flashID, " [");
            strcat(project_flashID,flash_ID);
            strcat(project_flashID,"]");
        }
    }

    // if the bin file can't parse the BBChip or, UI will display it and notify the user
    char bbchip_name[128]={0};
    DL_GetBBChipName(dl_handle, bbchip_name, sizeof(bbchip_name));
    if(bbchip_name[0]=='\0')
    {
        // EndUser download load
        strcpy(project_flashID, " [Encrypted Bins]");
    }
    else if(!strcmp(project_flashID," []"))
    {
        strcpy(project_flashID, " [Unknown Bins]");
    }
    

    /* 2. Add this DL_HANDLE into List */
    if( S_DONE == (ret = DL_AddHandleToList(g_DL_HANDLE_LIST, node.m_dl_handle)))
    {
       g_DL_HANDLE_LIST_nodes.push_back(node);    //binding
    }
    else
    {
        switch(ret)
        {
            case S_DLIST_SAME_BBCHIP_SW_VER:
            {
                ShowMessage("Load for " + AnsiString(custom_name) + " already exists!");                
                break;
            }
            case S_DLIST_SAME_FLASHID_AND_BBCHIP_SW_VER:
            {
                if(flashNameret == S_DONE)
                {
                    ShowMessage("Load for "+  AnsiString(custom_name) + " and " + AnsiString(flash_name)+" already exists!");
                }
                else
                {
                    ShowMessage("Load for " + AnsiString(custom_name) + " already exists!");
                }
                break;
            }            
            default:
                break;
        }        
        //g_rootLoad->Selected = true;  //FIX CR: MAUI_02144491
        goto error;
    }
    
    // Tree View
    nLoad = TreeView1->Items->AddChild(g_rootLoad, AnsiString(project_flashID));
    nLoad->ImageIndex = 1;
    nLoad->SelectedIndex = 1;
    nLoad->Data = &g_DL_HANDLE_LIST_nodes[g_DL_HANDLE_LIST_nodes.size()-1];
    g_rootLoad->Expand(true);
    nLoad->Selected = true;
    g_pCurNode = (MultiLoadNode *)nLoad->Data;
    RedrawReadbackList();
    RedrawDownloadList(g_pCurNode, false);

    goto end;

error:
    DL_Destroy(&node.m_dl_handle);
    return false;
end:

    }
    return true;
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::DeleteaLoad1Click(TObject *Sender)
{
	if( TreeView1->Selected != g_rootLoad )
    {
		//1. Destroy the linke of DL_HANDLE instance from DL_HANDLE_LIST
		DL_DeleteHandleFromList(g_DL_HANDLE_LIST,g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index].m_dl_handle);

        //   Destroy all ROM file, and DL_Handle instance
		DL_Rom_UnloadAll(g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index].m_dl_handle);
		DL_Destroy(&g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index].m_dl_handle);


        //2. Destroy this Node in UI Node List
		g_DL_HANDLE_LIST_nodes.erase(&g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index]);

        //3. Destroy this Node in UI MultiLoad History
        int i;
        MultiLoadHistoryList::iterator	iter;
		for( i= 0 ,iter=g_MultiLoadHistory.begin(); iter!= g_MultiLoadHistory.end(); iter++,i++ )
        {
            if(i == TreeView1->Selected->Index)
            {
                g_MultiLoadHistory.erase(iter);
                break;
            }
        }

		//4. Destroy this Node in UI TreeView, and set Selected to root
		TreeView1->Selected->Delete();

        g_rootLoad->Selected = true;
		g_pCurNode = NULL;
		g_pLastNode = NULL;
 		RedrawReadbackList();
		RedrawDownloadList(g_pCurNode, false);

        //5. Reset g_pLastNode when TreeView is empty
        if(TreeView1->Items->Count <= 1){
    		g_pLastNode = NULL;
        }
	}
}
//---------------------------------------------------------------------------
// Handle Right button
void __fastcall TMainForm::TreeView1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPoint p;
    GetCursorPos(&p); // To get real coordination in screen
    if (Button == mbRight)
    {
        TTreeNode * onRightButtonNode = TreeView1->GetNodeAt(X, Y);

        if(onRightButtonNode != NULL){
            onRightButtonNode->Selected = true;    // set focus on this Node (Load)

            TreeView1->Selected = onRightButtonNode;    // re-assign the selected Node (Load)
        }

        // Then, Below flow is the same with Right button
        //TreeView1Click(Sender);
        TreeView1Change(Sender,TreeView1->Selected);

        // Show Pop-up menu
        PopupMenu1->Popup(p.x, p.y);
    }
}
//---------------------------------------------------------------------------
bool TMainForm::OpenAuthFile()
{
	static AnsiString asAUTH_Path;

	if ( 0 < asAUTH_Path.Length() )
    {
		m_OpenAuthFileDialog->InitialDir = asAUTH_Path;
	}
	else if ( 0 < ed_AUTH_File->Text.Length() )
    {
		const int iLastDel = ed_AUTH_File->Text.LastDelimiter("\\:");

		if ( 0 < iLastDel ) {
			m_OpenAuthFileDialog->InitialDir = ed_AUTH_File->Text.SubString(1, iLastDel-1 );
		}
	}

    if (m_OpenAuthFileDialog->Execute())
    {
        ed_AUTH_File->Text = m_OpenAuthFileDialog->FileName;

        const int iLastDel = ed_AUTH_File->Text.LastDelimiter("\\:");

        if (0 < iLastDel)
        {
            asAUTH_Path = ed_AUTH_File->Text.SubString(1, iLastDel-1 );
        }
    }
    else
    {
        ed_AUTH_File->Text = "";
    }

    AUTH_Unload(g_AUTH_HANDLE);
    g_AUTH_HANDLE_Loaded = false;
    g_AUTH_File = "";

    if (ed_AUTH_File->Text.Length() == 0)
    {
        return true;
    }

    const int ret = AUTH_Load(g_AUTH_HANDLE, ed_AUTH_File->Text.c_str());

    if (ret != S_DONE)
    {
        ed_AUTH_File->Text = "";
        g_AUTH_HANDLE_Loaded = false;
        g_AUTH_File = ed_AUTH_File->Text;

        if (ret == S_FTHND_HANDLE_BUSY_NOW)
        {
            ShowMessage(" Program is busy! Can not reload AUTH file right now.");
        }
        else
        {
            ShowMessage(" Load AUTH file \"" + m_OpenAuthFileDialog->FileName + "\" fail!");
        }
    }
    else
    {
        g_AUTH_HANDLE_Loaded = true;
        g_AUTH_File = ed_AUTH_File->Text;
    }

    return (ret == S_DONE);
}

void __fastcall TMainForm::tb_OpenAuthFileClick(TObject *Sender)
{
    OpenAuthFile();
}
//---------------------------------------------------------------------------
bool TMainForm::OpenSCertFile()
{
    static AnsiString asSCERT_Path;

    if (0 < asSCERT_Path.Length())
    {
        m_OpenSCertFileDialog->InitialDir = asSCERT_Path;
    }
    else if (0 < ed_SCERT_File->Text.Length())
    {
        const int iLastDel = ed_SCERT_File->Text.LastDelimiter("\\:");

        if (0 < iLastDel)
        {
            m_OpenSCertFileDialog->InitialDir = ed_SCERT_File->Text.SubString(1, iLastDel-1 );
        }
    }

    if (m_OpenSCertFileDialog->Execute())
    {
        ed_SCERT_File->Text = m_OpenSCertFileDialog->FileName;

        const int iLastDel = ed_SCERT_File->Text.LastDelimiter("\\:");

        if (0 < iLastDel)
        {
            asSCERT_Path = ed_SCERT_File->Text.SubString(1, iLastDel-1 );
        }
    }
    else
    {
        ed_SCERT_File->Text = "";
    }

    SCERT_Unload(g_SCERT_HANDLE);
    g_SCERT_HANDLE_Loaded = false;
    g_SCERT_File = "";

    if (ed_SCERT_File->Text.Length() == 0)
    {
        return true;
    }

    const int ret = SCERT_Load(g_SCERT_HANDLE, ed_SCERT_File->Text.c_str());

    if (ret != S_DONE)
    {
        ed_SCERT_File->Text = "";
        g_SCERT_HANDLE_Loaded = false;
        g_SCERT_File = ed_SCERT_File->Text;

        if (ret == S_FTHND_HANDLE_BUSY_NOW)
        {
            ShowMessage(" Program is busy! Can not reload CERT file right now.");
        }
        else
        {
            ShowMessage(" Load CERT file \"" + m_OpenSCertFileDialog->FileName + "\" fail!");
        }
    }
    else
    {
        g_SCERT_HANDLE_Loaded = true;
        g_SCERT_File = ed_SCERT_File->Text;
    }

    return (ret == S_DONE);
}

void __fastcall TMainForm::tb_OpenSCertFileClick(TObject *Sender)
{
    OpenSCertFile();
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::m_AutoBBCHIPClick(TObject *Sender)
{
	g_BBCHIP_TYPE = AUTO_DETECT_BBCHIP;
	updateMenuItem();
}

//---------------------------------------------------------------------------

static int Param_IndexOfChangedCheckBox = -1;

void __fastcall TMainForm::m_ParamListViewClick(TObject *Sender)
{
	TListView *plv = dynamic_cast<TListView *>(Sender);

    if( NULL == g_pCurNode ) {
    	return;
	}

	ParamDownloadConfig_T	&param = g_pCurNode->m_param;

	if( 0<=Param_IndexOfChangedCheckBox && Param_IndexOfChangedCheckBox<plv->Items->Count )
    {
		TListItem *pItem = plv->Items->Item[Param_IndexOfChangedCheckBox];
		ListViewItemType_E	type =*((ListViewItemType_E *)pItem->Data);

		switch( type )
        {
		    case LV_OTP:
			    param.m_otp_enable       = pItem->Checked?_TRUE:_FALSE;
			    break;

		    case LV_FTCFG:
			    param.m_ft_cfg_enable    = pItem->Checked?_TRUE:_FALSE;
			    break;

		    case LV_SEC_RO:
			    param.m_sec_ro_enable    = pItem->Checked?_TRUE:_FALSE;
			    break;

		    case LV_CUST_PARA:
			    param.m_cust_para_enable = pItem->Checked?_TRUE:_FALSE;
			    break;

            case LV_GPS_ROM:
                param.m_gps_enable       = pItem->Checked?_TRUE:_FALSE;
                break;

		    default:
			    break;
		}
	}
	else if( NULL != plv->Selected )
    {
		TListItem *pItem=plv->Selected;
		ListViewItemType_E	type=*((ListViewItemType_E *)pItem->Data);

		switch( type )
        {
		    case LV_OTP:
                g_bParameterPage = false;
			    m_OTPSetting->Visible = true;

                if( param.m_otp_op == OTP_WRITE )
                {
                    m_OTPSetting->m_Write->Checked     = true;
                    m_OTPSetting->m_WriteLock->Checked = false;
                    m_OTPSetting->m_Lock->Checked      = false;
                }
                else if( param.m_otp_op == OTP_WRITE_AND_LOCK )
                {
                    m_OTPSetting->m_Write->Checked     = false;
                    m_OTPSetting->m_WriteLock->Checked = true;
                    m_OTPSetting->m_Lock->Checked      = false;
                }
                else if( param.m_otp_op == OTP_LOCK )
                {
                    m_OTPSetting->m_Write->Checked     = false;
                    m_OTPSetting->m_WriteLock->Checked = false;
                    m_OTPSetting->m_Lock->Checked      = true;
                }

       		    m_OTPSetting->m_otp_addr->Text  = param.m_otp.m_addr;
   	    	    m_OTPSetting->m_otp_len->Text   = param.m_otp.m_len;
                m_OTPSetting->m_otp_file->Text  = param.m_otp_filepath;
			    break;

		    case LV_FTCFG:
                g_bParameterPage = false;
                m_SecuritySetting->Visible = true;
			    break;

		    case LV_SEC_RO:
                g_bParameterPage = false;
			    LoadSecRO();
			    break;

		    case LV_CUST_PARA:
                g_bParameterPage = false;
			    LoadCustPara();
			    break;

            case LV_GPS_ROM:
                g_bParameterPage = false;
			    LoadGPS();
			    break;

		    default:
			    break;
		}
	}

    Param_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

static bool Param_bPreviousCheckBoxValue;

void __fastcall TMainForm::m_ParamListViewChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
	// ShowMessage("Changed!");

    if( (ctState == Change) &&
    	(Item->Checked != Param_bPreviousCheckBoxValue) ) {
        Param_IndexOfChangedCheckBox = Item->Index;
        return;
    }

    Param_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_ParamListViewChanging(TObject *Sender,
      TListItem *Item, TItemChange Change, bool &AllowChange)
{
	Param_bPreviousCheckBoxValue = Item->Checked;

    Param_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_ParamListViewDblClick(TObject *Sender)
{
	m_ParamListViewClick(Sender);
}
//---------------------------------------------------------------------------
int TMainForm::LoadFileInfo(const char *pathname, unsigned int *p_nFileLength, long *p_last_modify_time)
{
	FILE *fp;
	struct _stat  file_stat={0};

	if( NULL == pathname || '\0' == pathname[0] || NULL == p_nFileLength ) return 1;


	file_open_sentry	file_sentry(pathname, "rb", fp);

    if( NULL == fp ) {
        return 2;
    }

	if( _fstat(fileno(fp), &file_stat) ) {
		return 3;
	}

	unsigned int file_len;
	unsigned int buf_len;

	if( 0 >= (file_len=filelength(fileno(fp))) ) {
		return 4;
	}

	if( 0 != (file_len%2) ) {
		buf_len = file_len+1;
	}
	else {
		buf_len = file_len;
	}

	*p_nFileLength = buf_len;
	if( NULL != p_last_modify_time ) {
		*p_last_modify_time = file_stat.st_mtime;
	}

    return 0;
}
//---------------------------------------------------------------------------
int TMainForm::LoadFile(const char *pathname, unsigned char **pp_buf, unsigned int *p_nFileLength, long *p_last_modify_time)
{

	FILE *fp;
    unsigned int    ret;

	if( NULL == pp_buf ) {
		return 1;
	}

    ret = LoadFileInfo(pathname, p_nFileLength, p_last_modify_time);
    if( ret ){
        return ret;
    }

	file_open_sentry	file_sentry(pathname, "rb", fp);

    unsigned int buf_len = *p_nFileLength;
	unsigned char *buf;

	if( NULL == (buf=(unsigned char *)malloc(buf_len)) ) {
		return 5;
	}

	if( buf_len > fread(buf, 1, buf_len, fp) ) {
		free(buf);
		return 6;
	}

	if( NULL != *pp_buf ) {
		free(*pp_buf);
	}

	*pp_buf = buf;

	return 0;
}
//---------------------------------------------------------------------------
void TMainForm::LoadSecRO(void)
{
	AnsiString	s;
    ParamDownloadConfig_T   * param;

    if( g_bParameterPage ){
        param = &g_ParamDownloadConfig;
    }
    else{
        if( NULL == g_pCurNode ) {
        	return;
    	}
        param = &g_pCurNode->m_param;
    }
	m_OpenFileDialog->Title = "Open SEC_RO file ...";
	m_OpenFileDialog->InitialDir = ROMBaseDir;

    if( m_OpenFileDialog->Execute() )
    {
		if(LoadFile(m_OpenFileDialog->FileName.c_str(), &param->m_sec_ro.m_data, &param->m_sec_ro.m_len, NULL)) {
			s.printf("Failed to load SEC_RO file \"%s\" !", m_OpenFileDialog->FileName.c_str());
			ShowMessage(s);
			return;
		}
	}

	param->m_sec_ro_filepath = m_OpenFileDialog->FileName;

    if( g_bParameterPage ){
        AnsiString ss;
        ss.printf("len(%lu): %s", param->m_sec_ro.m_len, param->m_sec_ro_filepath.c_str());
        ed_sec_ro_Write->Text = ss;
    }
    else{
    	RedrawDownloadList(g_pCurNode, true);
    }
}
//---------------------------------------------------------------------------
void TMainForm::LoadCustPara( void )
{
	AnsiString	s;
    ParamDownloadConfig_T   *param;

    if( g_bParameterPage ) {
        param = &g_ParamDownloadConfig;
    }
    else{
        if( NULL == g_pCurNode ) return;
        param = &g_pCurNode->m_param;
    }
    
	m_OpenFileDialog->Title = "Open Custom Defined Parameters file ...";
	m_OpenFileDialog->InitialDir = ROMBaseDir;

    if( m_OpenFileDialog->Execute() )
    {
		if( LoadFile( m_OpenFileDialog->FileName.c_str(), &param->m_cust_para.m_data, &param->m_cust_para.m_len, NULL) )
        {
			s.printf("Failed to load CUSTOM PARAMETER file \"%s\" !", m_OpenFileDialog->FileName.c_str());
			ShowMessage(s);
			return;
		}
	}

	param->m_cust_para_filepath = m_OpenFileDialog->FileName;

    if( g_bParameterPage )
    {
        AnsiString ss;
        ss.printf("len(%lu): %s", param->m_cust_para.m_len, param->m_cust_para_filepath.c_str());
        ed_cust_Write->Text = ss;
    }
    else{
    	RedrawDownloadList(g_pCurNode, true);
    }
}
//---------------------------------------------------------------------------
void TMainForm::LoadGPS( void )
{
	AnsiString	s;
    ParamDownloadConfig_T   *param;

    if( g_bParameterPage ) {
        param = &g_ParamDownloadConfig;
    }
    else{
        if( NULL == g_pCurNode ) {
        	return;
    	}
        param = &g_pCurNode->m_param;
    }
    
	m_OpenFileDialog->Title      = "Open GPS ROM file ...";
	m_OpenFileDialog->InitialDir = ROMBaseDir + "\\GPS";
    if( m_OpenFileDialog->Execute() )
    {
        AnsiString  selectDir;
//        AnsiString  selectSca;
        AnsiString  selectIni;
		if(LoadFileInfo(m_OpenFileDialog->FileName.c_str(), &param->m_gps_rom_filesize, NULL)) {
			s.printf("Failed to load GPS ROM \"%s\" !", m_OpenFileDialog->FileName.c_str());
			ShowMessage(s);
			return;
		}

        selectDir = ExtractFilePath(m_OpenFileDialog->FileName);
//        selectSca = selectDir + "scat.txt";
        selectIni = selectDir + "gps.ini";

//        if( !FileExists(selectSca) ){
//			s.printf("Failed to load GPS FW's Scatter file  \"%s\" !", selectSca.c_str());
//			ShowMessage(s);
//			return;
//        }

        if( !FileExists(selectIni) ){
			s.printf("Failed to load GPS FW's INI file      \"%s\" !", selectIni.c_str());
			ShowMessage(s);
			return;
        }
	}
    else{
        return;
    }

	param->m_gps_rom_filepath = m_OpenFileDialog->FileName;
    param->m_gps_rom_dirpath  = ExtractFilePath(param->m_gps_rom_filepath);
//    param->m_gps_sca_filepath = param->m_gps_rom_dirpath + "scat.txt";
    param->m_gps_ini_filepath = param->m_gps_rom_dirpath + "gps.ini";

    TIniFile *ini = new TIniFile( param->m_gps_ini_filepath );
    param->m_gps_power_pin   = ini->ReadInteger( "GPIO", "gpio_gps_power_pin", 1 );
    param->m_gps_reset_pin   = ini->ReadInteger( "GPIO", "gpio_gps_reset_pin", 25 );
    param->m_gps_standby_pin = ini->ReadInteger( "GPIO", "gpio_gps_standby_pin", 42 );
    param->m_gps_32k_pin     = ini->ReadInteger( "GPIO", "gpio_gps_32k_pin", 36 );

    if( g_bParameterPage )
    {
        AnsiString ss;
        ss.printf("len(%lu): %s", param->m_gps_rom_filesize, param->m_gps_rom_filepath.c_str());
        ed_gps_Write->Text = ss;
    }
    else{
    	RedrawDownloadList(g_pCurNode, true);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::tb_ParameterButtonClick(TObject *Sender)
{
	g_flashtool_function = FLASHTOOL_PARAMETER;

	if ( asBin.IsEmpty() )
    {
		if ( !FileOpenDownloadAgentExecute() )
		{
			ShowMessage(" You must open a download agent file before downloading. ");
			return;
		}
	}

	// clean Flash device info
	HWStatusBarClear();

	if( UpdateDA() ) {
		return;
	}

    start_DL( true );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::cb_otpClick(TObject *Sender)
{
    if( cb_otp->Checked )
    {
        g_ParamDownloadConfig.m_otp_enable = (_BOOL) true;
        g_ParamReadbackConfig.m_otp_enable = (_BOOL) true;
        
        gb_otp->Enabled = true;
        bt_otp_setting->Enabled = true;
        rb_otp_Write->Enabled   = true;
        rb_otp_Read->Enabled    = true;
        lb_otp_Write->Enabled   = true;
        lb_otp_Read->Enabled    = true;
        ed_otp_Write->Enabled   = true;
        ed_otp_Read->Enabled    = true;
    }
    else{
        g_ParamDownloadConfig.m_otp_enable = (_BOOL) false;
        g_ParamReadbackConfig.m_otp_enable = (_BOOL) false;

        gb_otp->Enabled         = false;
		bt_otp_setting->Enabled = false;
		rb_otp_Write->Enabled   = false;
		rb_otp_Read->Enabled    = false;
		lb_otp_Write->Enabled   = false;
		lb_otp_Read->Enabled    = false;
		ed_otp_Write->Enabled   = false;
		ed_otp_Read->Enabled    = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_sec_settingClick(TObject *Sender)
{
    if( cb_sec_setting->Checked )
    {
        g_ParamDownloadConfig.m_ft_cfg_enable = (_BOOL) true;

        gb_sec_setting->Enabled         = true;
		bt_sec_setting->Enabled         = true;
		rb_sec_setting_Write->Enabled   = true;
		lb_sec_setting_Write->Enabled   = true;
		ed_sec_setting_Write->Enabled   = true;
    }
    else{
        g_ParamDownloadConfig.m_ft_cfg_enable = (_BOOL) false;

        gb_sec_setting->Enabled         = false;
		bt_sec_setting->Enabled         = false;
		rb_sec_setting_Write->Enabled   = false;
		lb_sec_setting_Write->Enabled   = false;
		ed_sec_setting_Write->Enabled   = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_sec_roClick(TObject *Sender)
{
    if( cb_sec_ro->Checked ){
        g_ParamDownloadConfig.m_sec_ro_enable = (_BOOL) true;
        g_ParamReadbackConfig.m_sec_ro_enable = (_BOOL) true;

        gb_sec_ro->Enabled          = true;
		bt_sec_ro_setting->Enabled  = true;
		rb_sec_ro_Write->Enabled    = true;
		rb_sec_ro_Read->Enabled     = true;
		lb_sec_ro_Write->Enabled    = true;
		lb_sec_ro_Read->Enabled     = true;
		ed_sec_ro_Write->Enabled    = true;
		ed_sec_ro_Read->Enabled     = true;
    }
    else{
        g_ParamDownloadConfig.m_sec_ro_enable = (_BOOL) false;
        g_ParamReadbackConfig.m_sec_ro_enable = (_BOOL) false;

        gb_sec_ro->Enabled          = false;
		bt_sec_ro_setting->Enabled  = false;
		rb_sec_ro_Write->Enabled    = false;
		rb_sec_ro_Read->Enabled     = false;
		lb_sec_ro_Write->Enabled    = false;
		lb_sec_ro_Read->Enabled     = false;
		ed_sec_ro_Write->Enabled    = false;
		ed_sec_ro_Read->Enabled     = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_custClick(TObject *Sender)
{
    if( cb_cust->Checked ){
        g_ParamDownloadConfig.m_cust_para_enable = (_BOOL) true;
        g_ParamReadbackConfig.m_cust_para_enable = (_BOOL) true;

        gb_cust->Enabled          = true;
		bt_cust_setting->Enabled  = true;
		rb_cust_Write->Enabled    = true;
		rb_cust_Read->Enabled     = true;
		lb_cust_Write->Enabled    = true;
		lb_cust_Read->Enabled     = true;
		ed_cust_Write->Enabled    = true;
		ed_cust_Read->Enabled     = true;
    }
    else{
        g_ParamDownloadConfig.m_cust_para_enable = (_BOOL) false;
        g_ParamReadbackConfig.m_cust_para_enable = (_BOOL) false;

        gb_cust->Enabled = false;
		bt_cust_setting->Enabled  = false;
		rb_cust_Write->Enabled    = false;
		rb_cust_Read->Enabled     = false;
		lb_cust_Write->Enabled    = false;
		lb_cust_Read->Enabled     = false;
		ed_cust_Write->Enabled    = false;
		ed_cust_Read->Enabled     = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_otp_settingClick(TObject *Sender)
{
    g_bParameterPage = true;

    if( rb_otp_Write->Checked ) {
        m_OTPSetting->Visible = true;

        if( g_ParamDownloadConfig.m_otp_op == OTP_WRITE ){
            m_OTPSetting->m_Write->Checked     = true;
            m_OTPSetting->m_WriteLock->Checked = false;
            m_OTPSetting->m_Lock->Checked      = false;
        }
        else if( g_ParamDownloadConfig.m_otp_op == OTP_WRITE_AND_LOCK ){
            m_OTPSetting->m_Write->Checked     = false;
            m_OTPSetting->m_WriteLock->Checked = true;
            m_OTPSetting->m_Lock->Checked      = false;
        }
        else if( g_ParamDownloadConfig.m_otp_op == OTP_LOCK ){
            m_OTPSetting->m_Write->Checked     = false;
            m_OTPSetting->m_WriteLock->Checked = false;
            m_OTPSetting->m_Lock->Checked      = true;
        }
        if(g_ParamDownloadConfig.m_otp_used)
        {
            m_OTPSetting->m_otp_addr->Text = g_ParamDownloadConfig.m_otp.m_addr;
            m_OTPSetting->m_otp_len->Text  = g_ParamDownloadConfig.m_otp.m_len;
        }
        else
        {
            m_OTPSetting->m_otp_addr->Text = "";
            m_OTPSetting->m_otp_len->Text  = "";
        }

        m_OTPSetting->m_otp_file->Text = g_ParamDownloadConfig.m_otp_filepath;
    }
    else if( rb_otp_Read->Checked ) {
        g_ParameterType = PARAM_OTP;
        m_ParameterReadSetting->Visible = true;

        m_ParameterReadSetting->Label1->Visible = true;
        m_ParameterReadSetting->m_addr->Visible = true;

        if(g_ParamReadbackConfig.m_otp_used)
        {
            m_ParameterReadSetting->m_addr->Text = g_ParamReadbackConfig.m_otp.m_addr;
            m_ParameterReadSetting->m_len->Text  = g_ParamReadbackConfig.m_otp.m_len;
        }
        else
        {
            m_ParameterReadSetting->m_addr->Text = "";
            m_ParameterReadSetting->m_len->Text  = "";
        }

        m_ParameterReadSetting->m_file->Text = g_ParamReadbackConfig.m_otp_filepath ;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_sec_settingClick(TObject *Sender)
{
    g_bParameterPage = true;

    if( rb_sec_setting_Write->Checked ){
        m_SecuritySetting->Visible = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_sec_ro_settingClick(TObject *Sender)
{
    g_bParameterPage = true;

    if( rb_sec_ro_Write->Checked ) {
        LoadSecRO();
    }
    else if( rb_sec_ro_Read->Checked ) {
        g_ParameterType = PARAM_SEC_RO;
        m_ParameterReadSetting->Visible = true;

        m_ParameterReadSetting->Label1->Visible = false;
        m_ParameterReadSetting->m_addr->Visible = false;

        if(g_ParamReadbackConfig.m_sec_ro_used)
        {
            m_ParameterReadSetting->m_len->Text  = g_ParamReadbackConfig.m_sec_ro.m_len;
            m_ParameterReadSetting->m_file->Text = g_ParamReadbackConfig.m_sec_ro_filepath;
        }
        else
        {
            m_ParameterReadSetting->m_len->Text  = "";
            m_ParameterReadSetting->m_file->Text = "";
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_cust_settingClick(TObject *Sender)
{
    g_bParameterPage = true;

    if( rb_cust_Write->Checked ) {
        LoadCustPara();
    }
    else if( rb_cust_Read->Checked ) {
        g_ParameterType = PARAM_CUST;
        m_ParameterReadSetting->Visible = true;

        m_ParameterReadSetting->Label1->Visible = false;
        m_ParameterReadSetting->m_addr->Visible = false;

        if(g_ParamReadbackConfig.m_cust_para_used)
        {
            m_ParameterReadSetting->m_len->Text  = g_ParamReadbackConfig.m_cust_para.m_len;
            m_ParameterReadSetting->m_file->Text = g_ParamReadbackConfig.m_cust_para_filepath;
        }
        else
        {
            m_ParameterReadSetting->m_len->Text  = "";
            m_ParameterReadSetting->m_file->Text = "";
        }
    }
}
//---------------------------------------------------------------------------
int TMainForm::SaveFile(const char *pathname, unsigned char *p_buf, unsigned int nFileLength) {

	FILE *fp;

	if( NULL == pathname || '\0' == pathname[0] || NULL == p_buf || 0 == nFileLength ) {
		return 1;
	}

	file_open_sentry	file_sentry(pathname, "wb", fp);

	if( NULL == fp ) {
		return 2;
	}

	if( nFileLength > fwrite(p_buf, 1, nFileLength, fp) ) {
		return 3;
	}

	return 0;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::bb_ParameterButtonClick(TObject *Sender)
{
	g_flashtool_function = FLASHTOOL_PARAMETER;

	if ( asBin.IsEmpty() )
    {
        ShowMessage(" You must open a download agent file first. ");

		if ( !FileOpenDownloadAgentExecute() )
		{
			return;
		}
	}

	// clean flash device info
	HWStatusBarClear();

    // update DA to the latest version
	if( UpdateDA() ) return;

    start_DL(true);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bb_StopButtonClick(TObject *Sender)
{
	g_stopflag = BOOT_STOP;
	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------
static bool ParamRead_bPreviousCheckBoxValue;

void __fastcall TMainForm::m_ReadParamListViewChange( TObject *Sender, TListItem *Item, TItemChange Change)
{
	// ShowMessage("Changed!");

    if( (ctState == Change) && (Item->Checked != ParamRead_bPreviousCheckBoxValue) )
    {
        Param_IndexOfChangedCheckBox = Item->Index;
        return;
    }

    Param_IndexOfChangedCheckBox = -1;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_ReadParamListViewChanging(TObject *Sender,
      TListItem *Item, TItemChange Change, bool &AllowChange)
{
	ParamRead_bPreviousCheckBoxValue = Item->Checked;

    Param_IndexOfChangedCheckBox = -1;

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_ReadParamListViewClick(TObject *Sender)
{
	TListView *plv = dynamic_cast<TListView *>(Sender);

    if( NULL == g_pCurNode ) return;

	ParamReadbackConfig_T	&param=g_ReadbackPage_Param;

	if( 0 <= Param_IndexOfChangedCheckBox && Param_IndexOfChangedCheckBox<plv->Items->Count )
    {
		TListItem *pItem=plv->Items->Item[Param_IndexOfChangedCheckBox];
		ListViewItemType_E	type=*((ListViewItemType_E *)pItem->Data);

		switch ( type )
        {
		    case LV_OTP:
			    param.m_otp_enable       = pItem->Checked?_TRUE:_FALSE;
			    break;

		    case LV_SEC_RO:
			    param.m_sec_ro_enable    = pItem->Checked?_TRUE:_FALSE;
			    break;

		    case LV_CUST_PARA:
			    param.m_cust_para_enable = pItem->Checked?_TRUE:_FALSE;
			    break;

		    default:
			    break;
		}
	}
	else if( NULL != plv->Selected )
    {
		TListItem *pItem=plv->Selected;
		ListViewItemType_E	type=*((ListViewItemType_E *)pItem->Data);

		switch( type )
        {
		    case LV_OTP:
                g_ParameterType  = PARAM_OTP;
                g_bParameterPage = false;
                m_ParameterReadSetting->Visible = true;
                m_ParameterReadSetting->Label1->Visible = true;
                m_ParameterReadSetting->m_addr->Visible = true;
                m_ParameterReadSetting->m_addr->Text = param.m_otp.m_addr;
                m_ParameterReadSetting->m_len->Text  = param.m_otp.m_len;
                m_ParameterReadSetting->m_file->Text = param.m_otp_filepath ;
			    break;

		    case LV_SEC_RO:
                g_ParameterType  = PARAM_SEC_RO;
                g_bParameterPage = false;
                m_ParameterReadSetting->Visible = true;
                m_ParameterReadSetting->Label1->Visible = false;
                m_ParameterReadSetting->m_addr->Visible = false;
                m_ParameterReadSetting->m_len->Text  = param.m_sec_ro.m_len;
                m_ParameterReadSetting->m_file->Text = param.m_sec_ro_filepath ;
			    break;

		    case LV_CUST_PARA:
                g_ParameterType  = PARAM_CUST;
                g_bParameterPage = false;
                m_ParameterReadSetting->Visible = true;
                m_ParameterReadSetting->Label1->Visible = false;
                m_ParameterReadSetting->m_addr->Visible = false;
                m_ParameterReadSetting->m_len->Text  = param.m_cust_para.m_len;
                m_ParameterReadSetting->m_file->Text = param.m_cust_para_filepath ;
			    break;

		    default:
			    break;
		}
	}

    Param_IndexOfChangedCheckBox = -1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_ReadParamListViewDblClick(TObject *Sender)
{
	m_ReadParamListViewClick( Sender );
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::m_FS_StartButtonClick(TObject *Sender)
{

	g_flashtool_function = FLASHTOOL_API;

    if( ed_ExceptionLog->Text.Length() == 0 )
    {
        ShowMessage( "You must input your file name of Exception Log!" );
        return;
    }

    if( ed_SaveExceptionLog->Text.Length() == 0 )
    {
        ShowMessage( "You must input your saving path of Exception Log!" );
        return;
    }

	if ( asBin.IsEmpty() )
    {
        ShowMessage(" You must open a download agent file first. ");

		if ( !FileOpenDownloadAgentExecute() )
		{
			return;
		}
	}

	// clean Flash device info
	HWStatusBarClear();

	if( UpdateDA() ) return;

    start_DL( true );

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_FS_StopButtonClick(TObject *Sender)
{
	g_stopflag = BOOT_STOP;
	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_SaveExceptionLogClick(TObject *Sender)
{
	AnsiString	s;

	m_SaveFileDialog->Title = "Save Exception Log file as...";
//	m_SaveFileDialog->InitialDir = MainForm->ROMBaseDir;

    if( m_SaveFileDialog->Execute() )
    {
        ed_SaveExceptionLog->Text = m_SaveFileDialog->FileName.c_str();
	}
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6205BClick(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6205B;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6218BClick(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6218B;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6219Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6219;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6217Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6217;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6228Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6228;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6227Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6227;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6229Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6229;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6226Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6226;

	updateStatusBar();
	updateMenuItem();
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6226MClick(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6226;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6230Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6230;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6225Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6225;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6223Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6223;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6227DClick(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6227D;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6226DClick(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6226D;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6223PClick(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6223P;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6235Click(TObject *Sender)
{
	g_BBCHIP_TYPE = MT6235;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BBCHIP_MT6238Click(TObject *Sender)
{
  	g_BBCHIP_TYPE = MT6238;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6239Click(TObject *Sender)
{
    g_BBCHIP_TYPE = MT6239;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6268AClick(TObject *Sender)
{
  	g_BBCHIP_TYPE = MT6268A;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6251TClick(TObject *Sender)
{
    g_BBCHIP_TYPE = MT6251T;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6253TClick(TObject *Sender)
{
    g_BBCHIP_TYPE = MT6253T;

	updateStatusBar();
	updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BBCHIP_MT6268BClick(TObject *Sender)
{
    g_BBCHIP_TYPE = MT6268B;

	updateStatusBar();
	updateMenuItem();
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_SaveExtractNFBClick(TObject *Sender)
{
	AnsiString	s;

	m_SaveFileDialog->Title = "Save NFB intermediate file as...";
//	m_SaveFileDialog->InitialDir = MainForm->ROMBaseDir;

    if( m_SaveFileDialog->Execute() )
    {
        ed_SaveExtractNFB->Text = m_SaveFileDialog->FileName.c_str();
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_RW_StartButtonClick(TObject *Sender)
{
  /*
	g_flashtool_function = FLASHTOOL_ROMWRITER;

    if( ed_SaveExtractNFB->Text.Length() == 0 )
    {
        ShowMessage(" You must input your Saving Path of NFB intermediate file!");
        return;
    }

	if ( asBin.IsEmpty() )
    {
        ShowMessage(" You must open a download agent file first. ");

		if ( !FileOpenDownloadAgentExecute() )
		{
			return;
		}
	}

	// clean Flash device info
	HWStatusBarClear();

	if( UpdateDA() ) return;

    start_DL(true);
    */

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::m_RW_StopButtonClick(TObject *Sender)
{
	g_stopflag = BOOT_STOP;
	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_Menu_Window_CheckParameterPageClick( TObject *Sender)
{
    if( m_Menu_Window_CheckParameterPage->Checked ) {
        m_Menu_Window_CheckParameterPage->Checked = false;
        ts_Paremeter->TabVisible = false;
    }
    else{
        m_Menu_Window_CheckParameterPage->Checked = true;
        ts_Paremeter->TabVisible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::m_Menu_Window_CheckFileManagementClick( TObject *Sender)
{
     if( m_Menu_Window_CheckFileManagement->Checked ) {
        m_Menu_Window_CheckFileManagement->Checked = false;
        ts_FileManagement->TabVisible = false;
     }
     else{
        m_Menu_Window_CheckFileManagement->Checked = true;
        ts_FileManagement->TabVisible = true;
     }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::m_Menu_Window_CheckROMWriterClick( TObject *Sender)
{
    /*
    if( m_Menu_Window_CheckROMWriter->Checked ) {
        m_Menu_Window_CheckROMWriter->Checked = false;
        ts_RomWriter->TabVisible = false;
    }
    else{
        m_Menu_Window_CheckROMWriter->Checked = true;
        ts_RomWriter->TabVisible = true;
    }
    */
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::m_Menu_Window_CheckEnableBootClick(TObject *Sender)
{
    if (m_Menu_Window_CheckEnableBoot->Checked)
    {
        m_Menu_Window_CheckEnableBoot->Checked = false;
        ts_EnableBoot->TabVisible = false;
    }
    else
    {
        m_Menu_Window_CheckEnableBoot->Checked = true;
        ts_EnableBoot->TabVisible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::bt_MT_SelectAllClick(TObject *Sender)
{
    // enable all RAM testing method
    RAMCheckBox->Checked = true;
    cb_MT_RAM_Data_Bus_Test->Checked = true;
    cb_MT_RAM_Addr_Bus_Test->Checked = true;
    cb_MT_RAM_Pattern_Test->Checked  = true;
    cb_MT_RAM_Inc_Dec_Test->Checked  = true;

    // enable all NOR flash testing method
    NORCheckBox->Checked = true;
    cb_MT_NOR_Addr_Bus_Test->Checked = true;
    cb_MT_NOR_Pattern_Test->Checked  = true;


    // enable all NAND flash testing method
    NANDCheckBox->Checked = true;
    cb_MT_NAND_Pattern_Test->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_MT_StartButtonClick(TObject *Sender)
{
	g_flashtool_function = FLASHTOOL_MEMORYTEST;

    MultiLoadNode*  p_node = g_pCurNode;

	if( NULL == g_pCurNode ) {
        // Multi-Load
        if( MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked )
        {
            if(NULL == g_pLastNode)
            {
                return;
            }

            p_node = g_pLastNode;
        }
        // Single Load
        else
        {
		    return;
        }
	}

    // load DA
   	if ( asBin.IsEmpty() )
    {
		ShowMessage(" You must open a download agent file first. ");

		if ( !FileOpenDownloadAgentExecute() )
		{
			return;
		}
	}

    // parse scatter file and load images
	if ( !g_bDebugDA && p_node->m_scat_file.IsEmpty() )
    {
		ShowMessage( " Open a scatter file accroding to your project. " );
        
		if ( !FileOpenLinkMapExecute(p_node) )
		{
			return;
		}
	}


	// clean Flash device info
	HWStatusBarClear();

    // update DA to the leatest version
	if( UpdateDA() ) return;

    start_DL( true );
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::m_MT_StopButtonClick(TObject *Sender)
{
	g_stopflag = BOOT_STOP;
	META_CancelAllBlockingCall();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::mi_USBDownloadReadbackClick(TObject *Sender)
{

    if( mi_USBDownloadReadback->Checked == false )
    {
        g_bUsbDLRB                      = true;

        mi_USBDownloadReadback->Checked = true;
        BaudRate1->Enabled              = false;
        DCOMPort->Enabled               = false;

        mi_DownloadWithoutBattery->Enabled = true;
    }
    else
    {
        g_bUsbDLRB                      = false;

        mi_USBDownloadReadback->Checked = false;
        BaudRate1->Enabled              = true;
        DCOMPort->Enabled               = true;

        mi_DownloadWithoutBattery->Enabled = false;
        mi_DownloadWithoutBattery->Checked = false;
        
        for( unsigned short i=0; i<m_COMPortCount; i++ )
        {
            if (DCOMPort->Items[i]->Checked == true)
            {
                COM_PORT = m_COMPortList[i];
            }
        }
    }

    // Update Enable attribute of DL ROM
    mi_USBDownloadReadbackClickInternal(mi_USBDownloadReadback->Checked);

    // Update Caption and StatusBar
    UpdateCaption();
    updateStatusBar();
}

void __fastcall TMainForm::mi_USBDownloadReadbackClickInternal(bool bUSBChecked)
{
    if ( MultiLoad->Checked || MultiLoadWithMMAA->Checked )
    {
        // Update all Load in DL list
        const max_index = g_DL_HANDLE_LIST_nodes.size();
        int index;

        for(index=0; index < max_index; index++)
        {
            DL_HANDLE_T &dl_handle=(&g_DL_HANDLE_LIST_nodes[index])->m_dl_handle;
            unsigned short romNum = 0;

            if (BootROMUSBSupported(dl_handle))
            {
                continue;
            }

            if( (S_DONE == DL_GetCount_Ex(dl_handle, &romNum) ) && (0 < romNum) )
            {

                for(int i = 0; i < romNum; i++)                
                {
                    ROM_INFO_EX romInfo = {};     
                    if(S_DONE == DL_Rom_GetInfo_Ex(dl_handle, i, &romInfo) )
                    {
                        if(ARM_BL_ROM == romInfo.rom_type)
                        {
                            DL_Rom_SetEnableAttr_Ex(dl_handle, i, (bUSBChecked)?_FALSE:_TRUE);                   
                        }                    
                    }
                }
            }
            else
            {
                ShowMessage(" DL_GetCount() failed! ");
                return;
            }
            
        }
    
        // re-draw ListView
        RedrawReadbackList();
        RedrawDownloadList(g_pLastNode, false);
    }
    else    // Single Load
    {
        if( NULL == g_pCurNode ) return;

        if( g_pCurNode->m_scat_file == 0 ) return;

        // USB DL: User Forbidden to select Boot Loader
        DL_HANDLE_T &dl_handle=g_pCurNode->m_dl_handle;

        if (BootROMUSBSupported(dl_handle))
        {
            return;
        }

        unsigned short romNum = 0;
        if( (S_DONE == DL_GetCount_Ex(dl_handle, &romNum) ) && (0 < romNum) )
        {
            for(int i = 0; i < romNum; i++)                
            {
                ROM_INFO_EX romInfo = {};       
                if(S_DONE == DL_Rom_GetInfo_Ex(dl_handle, i, &romInfo) )
                {
                    if(ARM_BL_ROM == romInfo.rom_type)
                    {
                        DL_Rom_SetEnableAttr_Ex(dl_handle, i, (bUSBChecked)?_FALSE:_TRUE);                   
                    }                    
                }
            }
        }
        else
        {
            ShowMessage(" DL_GetCount() failed! ");
            return;
        }
        
        RedrawDownloadCheckBox();        
    }
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::cb_gpsClick(TObject *Sender)
{
    if( cb_gps->Checked )
    {
        g_ParamDownloadConfig.m_gps_enable = (_BOOL) true;
        g_ParamReadbackConfig.m_gps_enable = (_BOOL) true;

        gb_gps->Enabled = true;
		bt_gps_setting->Enabled = true;
		rb_gps_Write->Enabled   = true;
		rb_gps_Read->Enabled    = true;
		lb_gps_Write->Enabled   = true;
		lb_gps_Read->Enabled    = true;
		ed_gps_Write->Enabled   = true;
		ed_gps_Read->Enabled    = true;
    }
    else{
        g_ParamDownloadConfig.m_gps_enable = (_BOOL) false;
        g_ParamReadbackConfig.m_gps_enable = (_BOOL) false;

        gb_gps->Enabled = false;
		bt_gps_setting->Enabled = false;
		rb_gps_Write->Enabled   = false;
		rb_gps_Read->Enabled    = false;
		lb_gps_Write->Enabled   = false;
		lb_gps_Read->Enabled    = false;
		ed_gps_Write->Enabled   = false;
		ed_gps_Read->Enabled    = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::bt_gps_settingClick(TObject *Sender)
{
    g_bParameterPage = true;

    if( rb_gps_Write->Checked ) {
        LoadGPS();
    }
    else if( rb_gps_Read->Checked )
    {
        g_ParameterType = PARAM_GPS;
        m_ParameterReadSetting->Visible = true;

        m_ParameterReadSetting->Label1->Visible = false;
        m_ParameterReadSetting->m_addr->Visible = false;

        if(g_ParamReadbackConfig.m_gps_used)
        {
            m_ParameterReadSetting->m_len->Text  = g_ParamReadbackConfig.m_gps_rom_filesize;
            m_ParameterReadSetting->m_file->Text = g_ParamReadbackConfig.m_gps_rom_filepath;
        }
        else
        {
            m_ParameterReadSetting->m_len->Text  = "";
            m_ParameterReadSetting->m_file->Text = "";
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DownloadStyleClick(TObject *Sender)
{
	if(g_bDownloadStyleSequential) {
        g_bDownloadStyleSequential = false;
	}
    else {
        g_bDownloadStyleSequential = true;
	}

	updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RAMCheckBoxClick(TObject *Sender)
{
    if ( RAMCheckBox->Checked == true ) {
        cb_MT_RAM_Data_Bus_Test->Enabled = true;
        cb_MT_RAM_Addr_Bus_Test->Enabled = true;
        cb_MT_RAM_Pattern_Test->Enabled = true;
        cb_MT_RAM_Inc_Dec_Test->Enabled = true;
   } else {
        cb_MT_RAM_Data_Bus_Test->Enabled = false;
        cb_MT_RAM_Addr_Bus_Test->Enabled = false;
        cb_MT_RAM_Pattern_Test->Enabled = false;
        cb_MT_RAM_Inc_Dec_Test->Enabled = false;
    }    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::NORCheckBoxClick(TObject *Sender)
{
    if ( NORCheckBox->Checked == true ) {
        cb_MT_NOR_Addr_Bus_Test->Enabled = true;
        cb_MT_NOR_Pattern_Test->Enabled = true;
    } else {
        cb_MT_NOR_Addr_Bus_Test->Enabled = false;
        cb_MT_NOR_Pattern_Test->Enabled = false;
    }    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::NANDCheckBoxClick(TObject *Sender)
{
    if ( NANDCheckBox->Checked == true) {
        cb_MT_NAND_Pattern_Test->Enabled = true;
    } else {
        cb_MT_NAND_Pattern_Test->Enabled = false;
    }    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_RAM_Data_Bus_TestClick(TObject *Sender)
{
    if ( (cb_MT_RAM_Data_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Addr_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Pattern_Test->Checked  == false) &&
         (cb_MT_RAM_Inc_Dec_Test->Checked  == false) )
    {
        RAMCheckBox->Checked = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_RAM_Addr_Bus_TestClick(TObject *Sender)
{
    if ( (cb_MT_RAM_Data_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Addr_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Pattern_Test->Checked  == false) &&
         (cb_MT_RAM_Inc_Dec_Test->Checked  == false) )
    {
        RAMCheckBox->Checked = false;
    }    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_RAM_Pattern_TestClick(TObject *Sender)
{
    if ( (cb_MT_RAM_Data_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Addr_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Pattern_Test->Checked  == false) &&
         (cb_MT_RAM_Inc_Dec_Test->Checked  == false) )
    {
        RAMCheckBox->Checked = false;
    }    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_RAM_Inc_Dec_TestClick(TObject *Sender)
{
    if ( (cb_MT_RAM_Data_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Addr_Bus_Test->Checked == false) &&
         (cb_MT_RAM_Pattern_Test->Checked  == false) &&
         (cb_MT_RAM_Inc_Dec_Test->Checked  == false) )
    {
        RAMCheckBox->Checked = false;
    }    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_NOR_Addr_Bus_TestClick(TObject *Sender)
{
    if ( (cb_MT_NOR_Addr_Bus_Test->Checked == false) &&
         (cb_MT_NOR_Pattern_Test->Checked  == false) )
    {
        NORCheckBox->Checked = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_NOR_Pattern_TestClick(TObject *Sender)
{
    if ( (cb_MT_NOR_Addr_Bus_Test->Checked == false) &&
         (cb_MT_NOR_Pattern_Test->Checked  == false) )
    {
        NORCheckBox->Checked = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::cb_MT_NAND_Pattern_TestClick(TObject *Sender)
{
    if ( (cb_MT_NAND_Pattern_Test->Checked == false) )
    {
        NANDCheckBox->Checked = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::lv_ReadBackKeyPress(TObject *Sender, char &Key)
{
    if ( Key =='\b')
    {
        tb_DeleteReadBackFileClick(Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::lv_ReadBackKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if ( Key == 46 || Key == 109 )
    {
        tb_DeleteReadBackFileClick(Sender);
    }
    else if ( Key == 107 || Key == 13 )
    {
        tb_AddReadBackFileClick( Sender);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NoActionClick(TObject *Sender)
{
    ResetBackupRestoreItem();
    updateMenuItem();
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::BackupRestoreClick(TObject *Sender)
{
    if ( BackupRestore->Checked == false )
    {
        NoAction->Checked      = false;
        BackupRestore->Checked = true;
        Restore->Checked       = false;
        RestoreOnly->Checked   = false;

        g_backup_restore_state = BACKUPRESTORE;

        // reset upload item to avoid operation conflict with  backup and restore
        ResetUploadItem();

    } else {
        ResetBackupRestoreItem();
    }
    updateMenuItem();

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::RestoreClick(TObject *Sender)
{
    AnsiString RestorePath;
    AnsiString BackupCalibrationDataFolderPath = \
        ExtractFilePath( Application->ExeName );
    if(SelectDirectory("Choose the directory which you want to restore", "", RestorePath))
    {
        g_restore_backup_data_path = RestorePath;
        AnsiString tmp("Download/Format -> Restore");
        AnsiString r_parenthesis(")");
        AnsiString l_parenthesis("(");

        if(RestorePath.Length()>=64)
        {
            int hardisk_pos = RestorePath.LastDelimiter(":");
            int dir_pos = RestorePath.LastDelimiter("\\");
            Restore->Caption =  tmp + " " + l_parenthesis +
                                    RestorePath.SubString(1,hardisk_pos) +
                                    "\\..." +
                                    RestorePath.SubString(dir_pos, RestorePath.Length()-dir_pos + 1) +
                                    r_parenthesis;
        }
        else
        {

            Restore->Caption =  tmp + " " + l_parenthesis +
                                    RestorePath + r_parenthesis;
        }

        if ( Restore->Checked == false )
        {
            NoAction->Checked       = false;
            BackupRestore->Checked  = false;
            Restore->Checked        = true;
            RestoreOnly->Checked    = false;

            g_backup_restore_state  = RESTORE;

            // reset upload item to avoid operation conflict with  backup and restore
            ResetUploadItem();
        } else {
            ResetBackupRestoreItem();
        }
        updateMenuItem();
    }



}





//---------------------------------------------------------------------------
void __fastcall TMainForm::RestoreOnlyClick(TObject *Sender)
{
    AnsiString RestorePath;
    AnsiString BackupCalibrationDataFolderPath = \
        ExtractFilePath( Application->ExeName );
    if(SelectDirectory("Choose the directory which you want to restore", "", RestorePath))
    {
        g_restore_backup_data_path = RestorePath;
        AnsiString tmp("Restore Only");
        AnsiString r_parenthesis(")");
        AnsiString l_parenthesis("(");
        if(RestorePath.Length()>=64)
        {
            int hardisk_pos = RestorePath.LastDelimiter(":");
            int dir_pos = RestorePath.LastDelimiter("\\");
            RestoreOnly->Caption =  tmp + " " + l_parenthesis +
                                    RestorePath.SubString(1,hardisk_pos) +
                                    "\\..." +
                                    RestorePath.SubString(dir_pos, RestorePath.Length()-dir_pos + 1) +
                                    r_parenthesis;
        }
        else
        {

            RestoreOnly->Caption =  tmp + " " + l_parenthesis +
                                    RestorePath + r_parenthesis;
        }

        
        if ( RestoreOnly->Checked == false )
        {
            NoAction->Checked      = false;
            BackupRestore->Checked = false;
            Restore->Checked       = false;
            RestoreOnly->Checked   = true;

            g_backup_restore_state = RESTOREONLY;

            // reset upload item to avoid operation conflict with  backup and restore
            ResetUploadItem();

        } else {
            ResetBackupRestoreItem();
        }
        updateMenuItem();
    }
}

//---------------------------------------------------------------------------
void TMainForm::ResetBackupRestoreItem( void )
{
    NoAction->Checked      = true;
    BackupRestore->Checked = false;
    Restore->Checked       = false;
    RestoreOnly->Checked   = false;

    g_backup_restore_state =  NOACTION;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::NoUploadClick(TObject *Sender)
{
    ResetUploadItem();
    updateMenuItem();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UploadClick(TObject *Sender)
{
    if ( Upload->Checked == false )
    {
        NoUpload->Checked   = false;
        Upload->Checked     = true;
        UploadOnly->Checked = false;

        g_upload_state      = UPLOAD;

        // reset backup and restore item to avoid operation conflict with upload
        ResetBackupRestoreItem();

    } else {
        ResetUploadItem();
    }
    updateMenuItem();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UploadOnlyClick(TObject *Sender)
{
    if ( UploadOnly->Checked == false )
    {
        NoUpload->Checked   = false;
        Upload->Checked     = false;
        UploadOnly->Checked = true;

        g_upload_state              = UPLOADONLY;

        // reset backup and restore item to avoid operation conflict with upload
        ResetBackupRestoreItem();

    } else {
        ResetUploadItem();
    }
    updateMenuItem();
}

//---------------------------------------------------------------------------
void TMainForm::ResetUploadItem( void )
{
    NoUpload->Checked   = true;
    Upload->Checked     = false;
    UploadOnly->Checked = false;

    g_upload_state      = NOUPLOAD;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DAStartAddressClick(TObject *Sender)
{
    Form_DA_Option->Visible = true;
}
//---------------------------------------------------------------------------

bool DownloadSet::operator==(const DownloadSet &r_value)
{
    if( 0==m_Scatter.AnsiCompareIC(r_value.m_Scatter) &&
        m_BinFiles.size()==r_value.m_BinFiles.size()
    ) {
        unsigned int i;
        for(i=0; i < m_BinFiles.size(); i++) {
            if( 0 != m_BinFiles[i].m_Filepath.AnsiCompareIC(r_value.m_BinFiles[i].m_Filepath) ) {
                return false;
            }
        }
        return true;
    }
    return false;
}

button_sentry::button_sentry()
{
    MainForm->DisableFileMenu();

    // disable download button
    MainForm->DisableDownloadButton();
    // disable readback button
    MainForm->DisableReadbackButton();
    // disable format button
    MainForm->DisableFormatButton();
    // disable DA select button
    MainForm->DisableDASelectButton();
    // disable scatter select button
    MainForm->DisableScatterSelectButton();
    // disable Auth select button
    MainForm->DisableAuthSelectButton();
    // disable SCert select button
    MainForm->DisableSCertSelectButton();
    // disable readback add button
    MainForm->DisableRBAddButton();
    // disable readback remove button
    MainForm->DisableRBRemoveButton();
    // disable Boot Cert select button
    MainForm->DisableBootCertSelectButton();

    // enable stop button
    MainForm->EnableStopButton();

    MainForm->bb_ParameterButton->Enabled = false;
    MainForm->bb_StopButton->Enabled      = true;

    MainForm->m_FS_StartButton->Enabled   = false;
    MainForm->m_FS_StopButton->Enabled    = true;

    MainForm->m_RW_StartButton->Enabled   = false;
    MainForm->m_RW_StopButton->Enabled    = true;

    MainForm->m_MT_StartButton->Enabled   = false;
    MainForm->m_MT_StopButton->Enabled    = true;

    MainForm->bt_start_enable_boot->Enabled = false;
    MainForm->bt_stop_enable_boot->Enabled = true;

    MainForm->m_ParamListView->Enabled    = false;

    // disable options
    MainForm->BaudRate1->Enabled = false;
    MainForm->DCOMPort->Enabled = false;
    MainForm->mi_USBDownloadReadback->Enabled = false;
    MainForm->BaseBandChip1->Enabled = false;
    MainForm->MCUClock1->Enabled = false;
    MainForm->mi_DownloadWithoutBattery->Enabled = false;


    MainForm->m_Menu_OperationMethod->Enabled = false;
    MainForm->m_Transmission_Option->Enabled = false;
    MainForm->m_DA_Option->Enabled = false;
    MainForm->m_BBChipOption->Enabled = false;
    MainForm->mi_FormatFDM->Enabled = false;
    MainForm->BackupandRestore->Enabled = false;
    MainForm->UploadSetting->Enabled = false;
    MainForm->m_ResourceOption->Enabled = false;
    MainForm->MultiLoadSetting->Enabled = false;

    // disable Popup Menu in Multi-Load TreeView
    if( MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked ){
        MainForm->AddaLoad1->Enabled      = false;
        MainForm->DeleteaLoad1->Enabled   = false;
    }

    MainForm->ListView1->Enabled = false;
    
    g_bButtonSentry = true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
button_sentry::~button_sentry()
{
    MainForm->EnableFileMenu();

    // enable download button
    MainForm->EnableDownloadButton();
    // enable readback button
    MainForm->EnableReadbackButton();
    // enable format button
    MainForm->EnableFormatButton();
    // enable DA select button
    MainForm->EnableDASelectButton();
    // enable scatter select button
    MainForm->EnableScatterSelectButton();
    // enable Auth select button
    MainForm->EnableAuthSelectButton();
    // enable SCert select button
    MainForm->EnableSCertSelectButton();
    // enable readback add button
    MainForm->EnableRBAddButton();
    // enable readback remove button
    MainForm->EnableRBRemoveButton();
    // enable Boot Cert select button
    MainForm->EnableBootCertSelectButton();
    
    // disable stop button
    MainForm->DisableStopButton();

    MainForm->bb_ParameterButton->Enabled = true;
    MainForm->bb_StopButton->Enabled      = false;

    MainForm->m_FS_StartButton->Enabled   = true;
    MainForm->m_FS_StopButton->Enabled    = false;

    MainForm->m_RW_StartButton->Enabled   = true;
    MainForm->m_RW_StopButton->Enabled    = false;

    MainForm->m_MT_StartButton->Enabled   = true;
    MainForm->m_MT_StopButton->Enabled    = false;

    MainForm->bt_start_enable_boot->Enabled = true;
    MainForm->bt_stop_enable_boot->Enabled = false;

    MainForm->m_ParamListView->Enabled    = true;


    // enable options
    MainForm->BaudRate1->Enabled = !g_bUsbDLRB;
    MainForm->DCOMPort->Enabled = !g_bUsbDLRB;
    MainForm->mi_USBDownloadReadback->Enabled = true;
    MainForm->BaseBandChip1->Enabled = true;
    MainForm->MCUClock1->Enabled = true;

    MainForm->m_Menu_OperationMethod->Enabled = true;
    MainForm->m_Transmission_Option->Enabled = true;
    MainForm->m_DA_Option->Enabled = true;
    MainForm->m_BBChipOption->Enabled = true;
    MainForm->mi_FormatFDM->Enabled = true;
    MainForm->BackupandRestore->Enabled = true;
    MainForm->UploadSetting->Enabled = true;
    MainForm->m_ResourceOption->Enabled = true;
    MainForm->MultiLoadSetting->Enabled = true;
    MainForm->mi_DownloadWithoutBattery->Enabled = true;

    // enable Popup Menu in Multi-Load TreeView
    if( MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked)
    {
        for( int index = 0; index < g_rootLoad->Count; index++ )
        {
            if( ( g_rootLoad->Item[index]->ImageIndex    == 2 ) ||
                ( g_rootLoad->Item[index]->SelectedIndex == 2) )
            {
                g_rootLoad->Item[index]->ImageIndex    = 1;
                g_rootLoad->Item[index]->SelectedIndex = 1;
            }
        }

        MainForm->AddaLoad1->Enabled    = true;
        MainForm->DeleteaLoad1->Enabled = true;
    }

    MainForm->ListView1->Enabled = true;

    g_bButtonSentry = false;
}
//---------------------------------------------------------------------------
void TMainForm::ResetMultiLoadItem( void )
{
    NoMultiLoad->Checked = true;
	MultiLoad->Checked = false;
	MultiLoadWithMMAA->Checked = false;
	TreeView1->Visible  = false;
    g_pCurNode = &g_DL_HANDLE_NODE;
    g_bMMAAEnable = false;
    g_multiload_state = NOMULTILOAD;
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::MultiLoadClick(TObject *Sender)
{
        if(MultiLoad->Checked == false)
        {
              // g_prev_multiload_state = g_multiload_state;

                if(g_bCheckECOVer == false)
                {
                     ShowMessage("Enable the Check Baseband Chip HW Version field (in Baseband Chip Options) to use this feature.");
                }
                else
                {

                        if(g_bMultiMemoryFlag == true)
                        {
                                if(TreeView1->Items->Count > 1)
                                {
                                        if(Application->MessageBox("This action will clear the load lists in MultiLoad With Multi-Memory.","Warning",MB_OKCANCEL | MB_ICONWARNING) == ID_OK)
                                        {
                                                g_DL_HANDLE_LIST = NULL;
                                                DL_CreateList(&g_DL_HANDLE_LIST);
                                                g_DL_HANDLE_LIST_nodes.clear();
                                                TTreeNode *node = TreeView1->Items->GetFirstNode();
                                                node->DeleteChildren();
                                        }
                                        else
                                        {
                                                goto end;
                                        }
                                }
                        }
                        g_bMultiMemoryFlag = false;
                        g_bMultiLoadFlag = true;
                        MultiLoad->Checked = true;
                        NoMultiLoad->Checked = false;
                        MultiLoadWithMMAA->Checked = false;
                        TreeView1->Visible = true;
                        g_rootLoad->Text = "Multi-Load";
                        g_rootLoad->Selected = true;
		                g_pCurNode = NULL;
                        g_pLastNode = NULL;
                        g_multiload_state = MULTILOAD;
                        g_bMMAAEnable = false;
                }
        }
        else
        {
                ResetMultiLoadItem();
        }
end:
        RedrawReadbackList();
	    RedrawDownloadList(g_pCurNode, false);
        updateMenuItem();

        UpdateMMAAEnable(g_bMMAAEnable);
}


//---------------------------------------------------------------------------

void __fastcall TMainForm::NoMultiLoadClick(TObject *Sender)
{
        //g_prev_multiload_state = g_multiload_state;

        ResetMultiLoadItem();
        RedrawReadbackList();
	    RedrawDownloadList(g_pCurNode, false);
        updateMenuItem();

        UpdateMMAAEnable(g_bMMAAEnable);
}

//---------------------------------------------------------------------------


void __fastcall TMainForm::MultiLoadWithMMAAClick(TObject *Sender)
{
       if(MultiLoadWithMMAA->Checked == false)
        {
                //g_prev_multiload_state = g_multiload_state;
                if(g_bCheckECOVer == false)
                {
                     ShowMessage("Enable the Check baseband chip HW version field (in Baseband Chip Options) to use this feature.");
                }
                else
                {

                        if(g_bMultiLoadFlag == true)
                        {

                                if(TreeView1->Items->Count > 1)
                                {
                                        if(Application->MessageBox("This action will clear the load lists in Normal MultiLoad.","Warning",MB_OKCANCEL | MB_ICONWARNING) == ID_OK)
                                        {
                                                g_DL_HANDLE_LIST = NULL;
                                                DL_CreateList(&g_DL_HANDLE_LIST);
                                                g_DL_HANDLE_LIST_nodes.clear();
                                                TTreeNode *node = TreeView1->Items->GetFirstNode();
                                                node->DeleteChildren();
                                        }
                                        else
                                        {
                                                goto end;
                                        }
                                }
                        }
                        g_bMultiLoadFlag = false;
                        g_bMultiMemoryFlag = true;
                        MultiLoadWithMMAA->Checked = true;
                        NoMultiLoad->Checked = false;
                        MultiLoad->Checked = false;
                        TreeView1->Visible = true;
                        g_rootLoad->Text = "Multi-Load (Multi-Memory)";
                        g_rootLoad->Selected = true;
	        	        g_pCurNode = NULL;
                        g_pLastNode = NULL;

                        g_multiload_state = MULTILOADWITHMMAA;
                        g_bMMAAEnable = true;
                }

        }
        else
        {
                ResetMultiLoadItem();
        }


end:    RedrawReadbackList();
	    RedrawDownloadList(g_pCurNode, false);
        updateMenuItem();

        UpdateMMAAEnable(g_bMMAAEnable);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::TreeView1Change(TObject *Sender,
      TTreeNode *Node)
{

// Handle the event from  Left button (OnClick Event), Key (OnKeyDown)

	if( TreeView1->Selected == g_rootLoad )
    {
        // Root of Multi-Load
        if(!g_bButtonSentry){
            DeleteaLoad1->Enabled = false;  // Root is not allowed to delete any load.
        }
        g_pCurNode = NULL;

    }
    else
    {
        // specific Load
        if(!g_bButtonSentry){
            DeleteaLoad1->Enabled = true;
        }
		g_pCurNode = &g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index];
        g_pLastNode= &g_DL_HANDLE_LIST_nodes[TreeView1->Selected->Index];

		// re-draw ListView
		RedrawReadbackList();
		RedrawDownloadList(g_pCurNode, false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ed_LinkMapFileEnter(TObject *Sender)
{
    ed_LinkMapFile->Hint = ed_LinkMapFile->Text;
    ed_LinkMapFile->ShowHint = true;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::ed_DA_FileEnter(TObject *Sender)
{
    ed_DA_File->Hint = ed_DA_File->Text;
    ed_DA_File->ShowHint = true;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::ed_AUTH_FileChange(TObject *Sender)
{
    ed_AUTH_File->Hint = ed_AUTH_File->Text;
    ed_AUTH_File->ShowHint = true;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_boot_cert_file_openClick(TObject *Sender)
{
    m_OpenFileDialog->Title = "Open Boot Cert";
    m_OpenFileDialog->InitialDir = ROMBaseDir;

    if (!m_OpenFileDialog->Execute())
    {
        return;
    }

    if (LoadFile(m_OpenFileDialog->FileName.c_str(),
                 &g_BootCert.m_data, &g_BootCert.m_len, NULL))
    {
        AnsiString msg;

        msg.printf("Failed to load Boot Cert file \"%s\"",
                   m_OpenFileDialog->FileName.c_str());
        ShowMessage(msg);
        return;
    }

    g_BootCert_Enabled = true;
    ed_boot_cert_file_path->Text = m_OpenFileDialog->FileName;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_start_enable_bootClick(TObject *Sender)
{
    if (!g_BootCert_Enabled)
    {
        ShowMessage("Please specify a valid Boot Cert file.");
        return;
    }

    g_flashtool_function = FLASHTOOL_ENABLE_BOOT;

    if (asBin.IsEmpty())
    {
        ShowMessage("You must open a download agent file first.");

        if (!FileOpenDownloadAgentExecute())
        {
            return;
        }
    }

    // clean flash device info
    HWStatusBarClear();

    // update DA to the latest version
    if (UpdateDA()) return;

    start_DL(true);
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::bt_stop_enable_bootClick(TObject *Sender)
{
    g_stopflag = BOOT_STOP;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::ed_SCERT_FileChange(TObject *Sender)
{
    ed_SCERT_File->Hint = ed_SCERT_File->Text;
    ed_SCERT_File->ShowHint = true;
}

//---------------------------------------------------------------------------

void __fastcall TMainForm::PreservebackupdataClick(TObject *Sender)
{
    if(Preservebackupdata->Checked == false)
    {
        Preservebackupdata->Checked = true;
        //g_preserve_backup_data = true;

        UseCOMPortNumberToStoreBackupData->Checked = false;
        //g_use_com_number_store_data = false;
    }
    else
    {
        Preservebackupdata->Checked = false;
        //g_preserve_backup_data = false;
    }
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::UseCOMPortNumberToStoreBackupDataClick(
      TObject *Sender)
{
    
    if(UseCOMPortNumberToStoreBackupData->Checked == false)
    {
        UseCOMPortNumberToStoreBackupData->Checked = true;

        Preservebackupdata->Checked = false;
    }
    else
    {
        UseCOMPortNumberToStoreBackupData->Checked = false;
    }
}
//---------------------------------------------------------------------------
void TMainForm::GetBackupRestoreStatus(bool* isPreserveBackupData, 
                                       bool* isUseComPortStoreData,
                                       AnsiString* restore_backup_path)
{
    if(isPreserveBackupData != NULL)
    {
        if(Preservebackupdata->Checked)
        {
            *isPreserveBackupData = true;
        }
        else
        {
            *isPreserveBackupData = false;
        }
    }

    if(isUseComPortStoreData != NULL)
    {
        if(UseCOMPortNumberToStoreBackupData->Checked)
        {
            *isUseComPortStoreData = true;
        }
        else
        {
            *isUseComPortStoreData = false;
        }
    }

    if(restore_backup_path != NULL)
    {
        if(Restore->Checked == false && RestoreOnly->Checked == false)
        {
            g_restore_backup_data_path = "";
            *restore_backup_path = ""; 
        }
        else
        {
            *restore_backup_path = g_restore_backup_data_path;
        }
    }
    
}
//---------------------------------------------------------------------------
bool TMainForm::BootROMUSBSupported(DL_HANDLE_T &dl_handle)
{
    if (dl_handle == NULL)
    {
        return false;
    }

    DL_INFO_EX dl_info = {};
    DL_GetInfo_Ex(dl_handle, &dl_info);

    return dl_info.brom_usb_supported;
}
//---------------------------------------------------------------------------
bool TMainForm::CreateROMEntries(const AnsiString &configFilePath,
                                 bool autoLoadROMFiles,
                                 DL_HANDLE_T downloadHandle)
{
    ROMSetting romSetting = { 0 };

    int ret = GetROMSetting(configFilePath.c_str(), &romSetting);

    if ((ret != S_DONE) || (romSetting.version <= 0))
    {
        ShowMessage("Invalid scatter/config file");
        return false;
    }

    ret = DL_CreateROMEntries(downloadHandle, &romSetting);

    if (ret != S_DONE)
    {
        switch (ret)
        {
            case S_FTHND_HANDLE_BUSY_NOW:
                ShowMessage("Program is busy. "
                            "Can not reload a scatter/config file right now.");
                break;

            case S_DL_SCAT_OFFSET_IS_NOT_WORD_ALIGN:
                ShowMessage("Failed to load this scatter file \"" +
                            configFilePath + "\".\n\n"
                            "The address offset of one load region "
                            "is not word-aligned.");
                break;

            case S_DL_SCAT_ADDR_IS_NOT_WORD_ALIGN:
                ShowMessage("Failed to load this scatter file \"" +
                            configFilePath + "\".\n\n"
                            "The start address of one load region "
                            "is not word-aligned.");
                break;

            case S_DL_SCAT_ADDR_IS_NOT_ASCENDING_ORDER:
                ShowMessage("Failed to load this scatter file \"" +
                            configFilePath + "\".\n\n"
                            "The region start addresses "
                            "is not in ascending order.");
                break;

            default:
                ShowMessage("Failed to create ROM entries");
                break;
        }

        return false;
    }

    if (autoLoadROMFiles)
    {
        assert (romSetting.version > 0);

        if (romSetting.version == 1)
        {
            // For legacy SW load
            ret = DL_AutoLoadByScatRegionName(
                            downloadHandle,
                            configFilePath.c_str(),
                            NORMAL_ROM | RESOURCE_BIN | JUMPTABLE_BIN,
                            _TRUE);
        }
        else
        {
            ret = DL_AutoLoadROMFiles(downloadHandle);
        }

        if (ret != S_DONE)
        {
            ShowMessage("Failed to load ROM files");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mi_DownloadWithoutBatteryClick(TObject *Sender)
{
    mi_DownloadWithoutBattery->Checked = !(mi_DownloadWithoutBattery->Checked);
}
//---------------------------------------------------------------------------

