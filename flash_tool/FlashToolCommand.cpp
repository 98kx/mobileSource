/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/
#include "FlashToolCommand.h"

#include <algorithm>
#include <utility>

#include <atlbase.h>
#pragma warn -8004
#include <utilcls.h>
#pragma warn .8004
#include <vcl.h>

#include "com_enum.h"
#include "DOWNLOAD.H"
#include "Logger.h"
#include "SLA_Challenge.h"
#include "Utility.h"

#include "rom_setting.h"


using ConsoleMode::BackupSetting;
using ConsoleMode::DownloadSetting;
using ConsoleMode::FormatOptions;
using ConsoleMode::FormatSetting;
using ConsoleMode::GeneralSetting;
using ConsoleMode::ResetToFactoryDefaultSetting;
using ConsoleMode::RestoreSetting;
using ConsoleMode::Logger;
using ConsoleMode::logger;
using std::find;
using std::mismatch;
using std::pair;
using std::vector;
using std::wstring;


DownloadHandle::DownloadHandle(const DownloadSetting &downloadSetting)
: m_Handle()
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << __FUNC__ << endl;

    /////////////////////////////////////////////////////////////
    // Refer to TMainForm::Parse. Load the scatter file
    /////////////////////////////////////////////////////////////

    const wstring scatterFilePath(downloadSetting.GetScatterFilePath());

    ROMSetting romSetting = { 0 };

    int ret = GetROMSetting(W2CA(downloadSetting.GetScatterFilePath().c_str()), &romSetting);
    if ((ret != S_DONE) || (romSetting.version <= 0))
    {
        logger << Logger::Level_Normal
            << "Invalid scatter/config file" << endl;
        // throw exception
    }

    DL_Create(&m_Handle);

    ret = DL_CreateROMEntries(m_Handle, &romSetting);
    if (ret != S_DONE)
    {
        logger << Logger::Level_Normal
            << "DL_CreateROMEntries failed" << endl;
        // throw exception
    }

    if (romSetting.version == 1)
    {
        // For legacy SW load
        ret = DL_AutoLoadByScatRegionName(
            m_Handle,
            W2CA(scatterFilePath.c_str()),
            NORMAL_ROM | RESOURCE_BIN | JUMPTABLE_BIN,
            _TRUE);
    }
    else
    {
        ret = DL_AutoLoadROMFiles(m_Handle);
    }

    if (ret != S_DONE)
    {
        logger << Logger::Level_Normal
            << "Failed to load ROM files" << endl;
        // throw exception
    }

    DL_SetPacketLength(m_Handle, 4096);

    /////////////////////////////////////////////////////////////
    // Please be aware that in conosle mode, files desired to be downloaded are recorded in downloadList.
    // We should enumerate them and set the enable attribute by calling DL_Rom_SetEnableAttr_Ex accordingly.
    /////////////////////////////////////////////////////////////

    vector<wstring> downloadList(downloadSetting.GetDownloadList());

    if (romSetting.version == 1)
    {
        DL_INFO downloadInfo = { 0 };

        DL_GetInfo(m_Handle, &downloadInfo);

        if (downloadInfo.m_config.m_is_nfb)
        {
            const vector<wstring>::iterator result = find(downloadList.begin(),
                downloadList.end(),
                wstring(L"BOOTLOADER"));
            const bool bootloaderEnabled = (result != downloadList.end());

            ret = DL_BL_SetEnableAttr(m_Handle,
                (bootloaderEnabled ? _TRUE : _FALSE));

            if (ret != S_DONE)
            {
                logger << Logger::Level_Normal
                    << "DL_BL_SetEnableAttr() failed" << endl;

                DL_Rom_UnloadAll(m_Handle);
                DL_Destroy(&m_Handle);
                // throw exception
            }
        }

        unsigned short numRoms;
        if(DL_GetCount(m_Handle, &numRoms) == S_DONE)
        {
            ROM_INFO romInfo[MAX_LOAD_SECTIONS];
            if(DL_Rom_GetInfoAll(m_Handle, romInfo, MAX_LOAD_SECTIONS) == S_DONE)
            {
                for (unsigned short i=0; i<numRoms; ++i)
                {
                    const vector<wstring>::iterator result = find(downloadList.begin(),
                        downloadList.end(),
                        wstring(A2CW(romInfo[i].name)));

                    const bool romEnabled = (result != downloadList.end());

                    ret = DL_Rom_SetEnableAttr(m_Handle, i, (romEnabled ? _TRUE : _FALSE));

                    if (ret != S_DONE)
                    {
                        logger << Logger::Level_Normal
                            << "DL_Rom_SetEnableAttr() failed" << endl;
                        DL_Rom_UnloadAll(m_Handle);
                        DL_Destroy(&m_Handle);
                        // throw exception
                    }
                }
            }

        }
    }
    else
    {
        unsigned short numRoms;
        if(DL_GetCount_Ex(m_Handle, &numRoms) == S_DONE)
        {
            ROM_INFO_EX romInfoEx[MAX_LOAD_SECTIONS];
            if(DL_Rom_GetInfoAll_Ex(m_Handle, romInfoEx, MAX_LOAD_SECTIONS) == S_DONE)
            {
                for (unsigned short i=0; i<numRoms; ++i)
                {
                    const vector<wstring>::iterator result = find(downloadList.begin(),
                        downloadList.end(),
                        wstring(A2CW(romInfoEx[i].name)));

                    const bool romEnabled = (result != downloadList.end());

                    ret = DL_Rom_SetEnableAttr_Ex(m_Handle, i, (romEnabled ? _TRUE : _FALSE));

                    if (ret != S_DONE)
                    {
                        logger << Logger::Level_Normal
                            << "DL_Rom_SetEnableAttr() failed" << endl;
                        DL_Rom_UnloadAll(m_Handle);
                        DL_Destroy(&m_Handle);
                        // throw exception
                    }
                }
            }

        }
    }

    unsigned short numRoms;
    if(DL_GetCount_Ex(m_Handle, &numRoms) == S_DONE)
    {
        ROM_INFO_EX romInfoEx[MAX_LOAD_SECTIONS];
        ret = DL_Rom_GetInfoAll_Ex(m_Handle, romInfoEx, MAX_LOAD_SECTIONS);

        if (ret != S_DONE)
        {
            logger << Logger::Level_Normal
                << "DL_Rom_GetInfoAll() failed" << endl;

            DL_Rom_UnloadAll(m_Handle);
            DL_Destroy(&m_Handle);
            // throw exception
        }

        for (unsigned short i=0; i<numRoms; ++i)
        {
            logger << Logger::Level_Normal
                << "    (" << (romInfoEx[i].enable ? "v" : "x") << ")"
                << romInfoEx[i].name << " " << endl;
        }
    }
}

DownloadHandle::~DownloadHandle()
{
    DL_Rom_UnloadAll(m_Handle);
    DL_Destroy(&m_Handle);
}

static int __stdcall COMPortOpenCallback(HANDLE, void *)
{
    logger << Logger::Level_Normal
        << "COM port is open." << endl
        << "Trying to sync with the target..." << endl;
    return 0;
}

static int __stdcall DownloadBootloaderProgressInit(void *)
{
    logger << Logger::Level_Verbose
        << "Downloading bootloader..." << endl;
    return 0;
}

static int __stdcall DownloadProgressInit(void *)
{
    logger << Logger::Level_Verbose
        << "Downloading images..." << endl;
    return 0;
}

static int __stdcall DownloadProgressCallback(unsigned char currentProgress,
                                              unsigned int, unsigned int,
                                              void *)
{
    static unsigned char lastReportedProgress = 0;

    if ((currentProgress != lastReportedProgress) &&
        (currentProgress % 5 == 0))
    {
        logger << Logger::Level_Verbose
            << (int) currentProgress
            << "% of image data sent to the target." << endl;

        lastReportedProgress = currentProgress;
    }

    return 0;
}

static void FillComPortSetting(COM_PORT_SETTING &comPortSetting,
                               const GeneralSetting &generalSetting)
{
    const wstring comPort = generalSetting.GetComPort();

    comPortSetting.com.number =
        (comPort == L"USB") ? 0 : ::_wtoi(comPort.c_str());

    comPortSetting.baudrate         = UART_BAUD_921600;
    comPortSetting.ms_read_timeout  = COM_DEFAULT_TIMEOUT;
    comPortSetting.ms_write_timeout = COM_DEFAULT_TIMEOUT;
}

static void FillFlashToolArg(FLASHTOOL_ARG &flashToolArg,
                             DA_HANDLE_T &daHandle,
                             AUTH_HANDLE_T &authHandle)
{
    ::memset(&flashToolArg, 0, sizeof(flashToolArg));

    /////////////////////////////////////////////////////////////
    // fill BOOT_FLASHTOOL_ARG
    /////////////////////////////////////////////////////////////
    BOOT_FLASHTOOL_ARG &bootFlashtoolArg = flashToolArg.m_boot_arg;

    bootFlashtoolArg.m_bbchip_type     = AUTO_DETECT_BBCHIP;
    bootFlashtoolArg.m_ext_clock       = AUTO_DETECT_EXT_CLOCK;
    bootFlashtoolArg.m_ms_boot_timeout = 30000;
    bootFlashtoolArg.m_max_start_cmd_retry_count =
        DEFAULT_BROM_START_CMD_RETRY_COUNT;

    bootFlashtoolArg.m_cb_com_init_stage     = COMPortOpenCallback;
    bootFlashtoolArg.m_cb_com_init_stage_arg = NULL;
    bootFlashtoolArg.m_cb_in_brom_stage      = NULL;
    bootFlashtoolArg.m_cb_in_brom_stage_arg  = NULL;

    // speed-up BootROM stage baudrate
    bootFlashtoolArg.m_speedup_brom_baudrate = _TRUE;

    // Application's window handle to send WM_BROM_READY_TO_POWER_ON_TGT message
    bootFlashtoolArg.m_ready_power_on_wnd_handle = NULL;
    bootFlashtoolArg.m_ready_power_on_wparam     = NULL;
    bootFlashtoolArg.m_ready_power_on_lparam     = NULL;

    // serial link authentication arg
    bootFlashtoolArg.m_auth_handle               = authHandle;
    bootFlashtoolArg.m_cb_sla_challenge          = SLA_Challenge;
    bootFlashtoolArg.m_cb_sla_challenge_arg      = NULL;
    bootFlashtoolArg.m_cb_sla_challenge_end      = SLA_Challenge_END;
    bootFlashtoolArg.m_cb_sla_challenge_end_arg  = NULL;

    // security certificate
    // Maggie ToDo: export the parameter to set secure control certificate
    bootFlashtoolArg.m_scert_handle = NULL;

    // EMI Control for bank0 and bank1
    bootFlashtoolArg.m_p_bank0_mem_cfg = NULL;
    bootFlashtoolArg.m_p_bank1_mem_cfg = NULL;

    // Maggie ToDo: in GUI mode, daHandle could be zero, see g_bSkipBootRom variable
    bootFlashtoolArg.m_da_handle = daHandle;

    // da download progress callack
    bootFlashtoolArg.m_cb_download_da_init       = NULL;
    bootFlashtoolArg.m_cb_download_da_init_arg   = NULL;
    bootFlashtoolArg.m_cb_download_da            = NULL;
    bootFlashtoolArg.m_cb_download_da_arg        = NULL;

    // download by USB
    bootFlashtoolArg.m_usb_enable = _FALSE;
    bootFlashtoolArg.m_usb_com_port_control = USB_COM_PORT_CONTROL_NO_OPERATION;

    /////////////////////////////////////////////////////////////
    // fill FLASHTOOL_ARG
    /////////////////////////////////////////////////////////////
    flashToolArg.m_cb_da_report     = NULL;
    flashToolArg.m_cb_da_report_arg = NULL;

    flashToolArg.m_cb_security_pre_process_notify     = NULL;
    flashToolArg.m_cb_security_pre_process_notify_arg = NULL;
    flashToolArg.m_baudrate_full_sync_count           = 1;
    flashToolArg.m_nor_chip_select[0]                 = CS0;
    flashToolArg.m_nor_chip_select[1]                 = CS_WITH_DECODER;
    flashToolArg.m_nand_chip_select                   = CS0;
    flashToolArg.m_p_nand_acccon                      = NULL;
    flashToolArg.m_enable_ui_dram_cfg                 = _FALSE;
}

static void FillFlashToolResult(FLASHTOOL_RESULT &flashToolResult)
{
    ::memset(&flashToolResult, 0, sizeof(flashToolResult));
}

static void FillFlashToolDownloadArg(
                                     FLASHTOOL_DOWNLOAD_ARG &flashToolDownloadArg,
                                     DownloadHandle &downloadHandle,
                                     bool checkBasebandVersion,
                                     const DownloadSetting &downloadSetting)
{
    ::memset(&flashToolDownloadArg, 0, sizeof(flashToolDownloadArg));

    flashToolDownloadArg.m_dl_handle_list = NULL;
    flashToolDownloadArg.m_dl_handle      = downloadHandle;

    // download progress callback
    flashToolDownloadArg.m_download_accuracy = ACCURACY_AUTO;
    flashToolDownloadArg.m_cb_download_flash_init = DownloadProgressInit;
    flashToolDownloadArg.m_cb_download_flash_init_arg = NULL;
    flashToolDownloadArg.m_cb_download_flash = DownloadProgressCallback;
    flashToolDownloadArg.m_cb_download_flash_arg = NULL;
    flashToolDownloadArg.m_cb_security_post_process_notify = NULL;
    flashToolDownloadArg.m_cb_security_post_process_notify_arg = NULL;
    flashToolDownloadArg.m_cb_download_fat_change_notify = NULL;
    flashToolDownloadArg.m_cb_download_fat_change_notify_arg = NULL;

    // format option
    const FormatOptions formatOptions(downloadSetting.GetFormatOptions());

    if (formatOptions.IsAutoFormatEnabled())
    {
        flashToolDownloadArg.m_format_enable                  = _TRUE;
        flashToolDownloadArg.m_format_cfg.m_auto_format_fat   = _TRUE;
        flashToolDownloadArg.m_format_cfg.m_format_begin_addr = 0;
        flashToolDownloadArg.m_format_cfg.m_format_length     = 0;
    }
    else if (formatOptions.IsManualFormatEnabled())
    {
        flashToolDownloadArg.m_format_enable                = _TRUE;
        flashToolDownloadArg.m_format_cfg.m_auto_format_fat = _FALSE;
        flashToolDownloadArg.m_format_cfg.m_format_begin_addr =
            formatOptions.m_ManualFormatBeginAddress;
        flashToolDownloadArg.m_format_cfg.m_format_length =
            formatOptions.m_ManualFormatLength;
    }
    else
    {
        flashToolDownloadArg.m_format_enable                  = _FALSE;
        flashToolDownloadArg.m_format_cfg.m_auto_format_fat   = _FALSE;
        flashToolDownloadArg.m_format_cfg.m_format_begin_addr = 0;
        flashToolDownloadArg.m_format_cfg.m_format_length     = 0;
    }

    flashToolDownloadArg.m_cb_format_report_init       = NULL;
    flashToolDownloadArg.m_cb_format_report_init_arg   = NULL;
    flashToolDownloadArg.m_cb_format_report            = NULL;
    flashToolDownloadArg.m_cb_format_report_arg        = NULL;
    flashToolDownloadArg.m_enable_tgt_res_layout_check = _TRUE;
    flashToolDownloadArg.m_enable_bbchip_ver_check =
        checkBasebandVersion ? _TRUE : _FALSE;
    flashToolDownloadArg.m_downloadstyle_sequential    = _FALSE;
    flashToolDownloadArg.m_cb_format_statistics        = NULL;
    flashToolDownloadArg.m_cb_format_statistics_arg    = NULL;

    // parameter
    flashToolDownloadArg.m_otp_enable                = _FALSE;
    flashToolDownloadArg.m_ft_cfg_enable             = _FALSE;
    flashToolDownloadArg.m_sec_ro_enable             = _FALSE;
    flashToolDownloadArg.m_cust_para_enable          = _FALSE;
    flashToolDownloadArg.m_cb_param_process_init     = NULL;
    flashToolDownloadArg.m_cb_param_process_init_arg = NULL;
    flashToolDownloadArg.m_cb_param_process          = NULL;
    flashToolDownloadArg.m_cb_param_process_arg      = NULL;

    // post-process callback
    flashToolDownloadArg.m_cb_post_process_init     = NULL;
    flashToolDownloadArg.m_cb_post_process_init_arg = NULL;
    flashToolDownloadArg.m_cb_post_process          = NULL;
    flashToolDownloadArg.m_cb_post_process_arg      = NULL;

    // Boot-Loader download progress callback
    flashToolDownloadArg.m_cb_download_bloader_init     = DownloadBootloaderProgressInit;
    flashToolDownloadArg.m_cb_download_bloader_init_arg = NULL;
    flashToolDownloadArg.m_cb_download_bloader          = DownloadProgressCallback;
    flashToolDownloadArg.m_cb_download_bloader_arg      = NULL;

    if (downloadSetting.IsWatchdogTimerEnabled())
    {
        flashToolDownloadArg.m_enable_wdt_timeout      = _TRUE;
        flashToolDownloadArg.m_ms_wdt_timeout_interval = 5000;
    }
    else
    {
        flashToolDownloadArg.m_enable_wdt_timeout      = _FALSE;
        flashToolDownloadArg.m_ms_wdt_timeout_interval = 0;
    }

    flashToolDownloadArg.m_relay_da_enable = _FALSE;
}

static void FillFlashToolDownloadResult(
                                        FLASHTOOL_DOWNLOAD_RESULT &flashToolDownloadResult)
{
    ::memset(&flashToolDownloadResult, 0, sizeof(flashToolDownloadResult));
}

/**
* Take care of DA handle initialization and deinitialization
*/
class DAHandleInitializer : private NonCopyable
{
public:
    DAHandleInitializer(DA_HANDLE_T &daHandle,
        const wstring &daFilePath)
        : m_DAHandle(daHandle)
    {
        USES_CONVERSION;

        /////////////////////////////////////////////////////////////
        // Refer to OpenReadDA_File
        /////////////////////////////////////////////////////////////
        DA_Create(&m_DAHandle);

        int ret = DA_Load(m_DAHandle, W2CA(daFilePath.c_str()), _FALSE);

        if (ret != S_DONE)
        {
            logger << Logger::Level_Normal
                << "DA_Load() failed" << endl;

            DA_Unload(m_DAHandle);
            DA_Destroy(&m_DAHandle);
            // throw exception
        }
    }

    ~DAHandleInitializer()
    {
        DA_Unload(m_DAHandle);
        DA_Destroy(&m_DAHandle);
    }

private:
    DA_HANDLE_T &m_DAHandle;
};

DownloadCommand::DownloadCommand(const GeneralSetting  &generalSetting,
                                 const DownloadSetting &downloadSetting)
                                 : m_GeneralSetting(generalSetting),
                                 m_DownloadSetting(downloadSetting),
                                 m_DownloadHandle(downloadSetting)
{
}

DownloadCommand::~DownloadCommand()
{
}

bool DownloadCommand::Execute()
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << __FUNC__ << endl;

    /////////////////////////////////////////////////////////////
    // Load the Download Agent
    /////////////////////////////////////////////////////////////

    DA_HANDLE_T daHandle;
    DAHandleInitializer daHandleInitiailzer(
        daHandle, m_GeneralSetting.GetDownloadAgentFilePath());

    /////////////////////////////////////////////////////////////
    // Load the Auth File
    /////////////////////////////////////////////////////////////
    // FIXME
    const wstring authFilePath(m_GeneralSetting.GetAuthenticationFilePath());
    const bool authEnabled = !authFilePath.empty();
    AUTH_HANDLE_T authHandle = NULL;

    if (authEnabled)
    {
        AUTH_Create(&authHandle);
        AUTH_Load(authHandle, W2CA(authFilePath.c_str()));
    }

    // Maggie ToDo: More should be done for new features, such as SCTRL_CERT

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgComPortSetting
    /////////////////////////////////////////////////////////////

    // fill COM_PORT_SETTING
    COM_PORT_SETTING comPortSetting;
    FillComPortSetting(comPortSetting, m_GeneralSetting);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgFlashTool
    /////////////////////////////////////////////////////////////

    // fill FLASHTOOL_ARG
    FLASHTOOL_ARG flashToolArg;
    FillFlashToolArg(flashToolArg, daHandle, authHandle);

    GetExternalMemorySetting(W2CA(m_DownloadSetting.GetScatterFilePath().c_str()),
        &flashToolArg.m_boot_arg.m_external_memory_setting[0]);

    // fill FLASHTOOL_RESULT
    FLASHTOOL_RESULT flashToolResult;
    FillFlashToolResult(flashToolResult);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgFlashToolDownload
    /////////////////////////////////////////////////////////////

    // fill FLASHTOOL_DOWNLOAD_ARG
    FLASHTOOL_DOWNLOAD_ARG flashToolDownloadArg;
    FillFlashToolDownloadArg(flashToolDownloadArg,
        m_DownloadHandle,
        m_GeneralSetting.IsCheckBasebandVersionEnabled(),
        m_DownloadSetting);

    /////////////////////////////////////////////////////////////
    // Refer to TMainForm::Parse Q_Q"
    /////////////////////////////////////////////////////////////
    vector<wstring> downloadList(m_DownloadSetting.GetDownloadList());
    const vector<wstring>::iterator result = find(downloadList.begin(),
        downloadList.end(),
        L"SECURE_RO");

    if (result != downloadList.end())
    {
        const wstring scatterFilePath = m_DownloadSetting.GetScatterFilePath();
        const AnsiString secROFilePath =
            ExtractFileDir(W2CA(scatterFilePath.c_str())) + "\\SECURE_RO";

        if (FileExists(secROFilePath))
        {
            flashToolDownloadArg.m_sec_ro_enable = _TRUE;
            flashToolDownloadArg.m_sec_ro.m_len =
                GetFileSize(secROFilePath.c_str());

            // FIXME: memory leak
            flashToolDownloadArg.m_sec_ro.m_data =
                GetFileContent(secROFilePath.c_str());
        }
    }

    FLASHTOOL_DOWNLOAD_RESULT flashToolDownloadResult;
    FillFlashToolDownloadResult(flashToolDownloadResult);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::Download_Internal Q_Q"
    /////////////////////////////////////////////////////////////

    if (comPortSetting.com.number == 0)
    {
        //
        // Search for a newly created COM port
        //
        logger << Logger::Level_Normal
            << "Searching for a newly created USB COM port..." << endl;

        unsigned short newCOMPortNum;

        if (!SearchNewCOMPort(newCOMPortNum))
        {
            logger << Logger::Level_Normal
                << "Download failed: no USB COM port detected" << endl;
            return false;
        }

        logger << Logger::Level_Normal
            << "USB COM port detected: COM" << newCOMPortNum << endl;

        comPortSetting.com.number = newCOMPortNum;
        flashToolArg.m_boot_arg.m_usb_enable = _TRUE;
    }

    /////////////////////////////////////////////////////////////
    // Call into BROM DLL to do the download
    /////////////////////////////////////////////////////////////

    int stopFlag;
    const int ret = FlashDownload(&comPortSetting,
        &flashToolArg,
        &flashToolResult,
        &flashToolDownloadArg,
        &flashToolDownloadResult,
        &stopFlag);

    if(flashToolDownloadResult.m_BLNotDownloadWarning)
    {
        logger << Logger::Level_Normal
            << "BootLoader is not updated under the UART Download for EndUser mode" << endl;      
    }

    // FIXME
    if (authEnabled)
    {
        AUTH_Destroy(&authHandle);
    }

    if (ret == S_DONE)
    {
        logger << Logger::Level_Normal
            << "Download succeeded." << endl;
        return true;
    }

    logger << Logger::Level_Normal
        << "Download failed: " << StatusToString(ret) << endl;
    return false;
}

static void FillFlashToolFormatArg_Ex(FLASHTOOL_FORMAT_ARG_EX &flashToolFormatArgEx,
                                      const FormatSetting &formatSetting)
{
    const FormatOptions formatOptions(formatSetting.GetFormatOptions());

    ::memset(&flashToolFormatArgEx, 0, sizeof(flashToolFormatArgEx));

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgFlashToolFormatEx
    /////////////////////////////////////////////////////////////

    // Maggie ToDo: Support more storage type and format action type.

    switch(formatOptions.m_StorageType)
    {
    case HW_STORAGE_NAND:
        flashToolFormatArgEx.m_format_cfg.storageType = HW_STORAGE_NAND;
        flashToolFormatArgEx.m_format_cfg.formatAction.uArg.NAND_manual.formatRange.startAddr = formatOptions.m_ManualFormatBeginAddress;
        flashToolFormatArgEx.m_format_cfg.formatAction.uArg.NAND_manual.formatRange.length = formatOptions.m_ManualFormatLength;
        flashToolFormatArgEx.m_format_cfg.formatAction.uArg.NAND_manual.formatFlag = NUTL_ERASE;
        break;
    case HW_STORAGE_NOR:
    default:
        flashToolFormatArgEx.m_format_cfg.storageType = HW_STORAGE_NOR;
        flashToolFormatArgEx.m_format_cfg.formatAction.uArg.manual.startAddr = formatOptions.m_ManualFormatBeginAddress;
        flashToolFormatArgEx.m_format_cfg.formatAction.uArg.manual.length = formatOptions.m_ManualFormatLength;
        break;
    }

    flashToolFormatArgEx.m_format_cfg.validation   = _FALSE;
    FORMAT_ACTION_TYPE_E formatAction;
    if (formatOptions.IsAutoFormatEnabled())
    {
        formatAction = FAT_FORMAT;
    }
    else if (formatOptions.IsManualFormatEnabled())
    {
        formatAction = NORMAL_MANUAL_FORMAT;
    }
    else
    {
        formatAction = NOT_FORMAT;
    }

    flashToolFormatArgEx.m_format_cfg.formatAction.type = formatAction;

    flashToolFormatArgEx.m_cb_format_report_init     = NULL;
    flashToolFormatArgEx.m_cb_format_report_init_arg = NULL;
    flashToolFormatArgEx.m_cb_format_report          = NULL;
    flashToolFormatArgEx.m_cb_format_report_arg      = NULL;
    flashToolFormatArgEx.m_cb_format_statistics      = NULL;
    flashToolFormatArgEx.m_cb_format_statistics_arg  = NULL;

    if (formatSetting.IsWatchdogTimerEnabled())
    {
        flashToolFormatArgEx.m_enable_wdt_timeout      = _TRUE;
        flashToolFormatArgEx.m_ms_wdt_timeout_interval = 5000;
    }
    else
    {
        flashToolFormatArgEx.m_enable_wdt_timeout      = _FALSE;
        flashToolFormatArgEx.m_ms_wdt_timeout_interval = 0;
    }
}

static void FillFlashToolFormatResult(
                                      FLASHTOOL_FORMAT_RESULT &flashToolFormatResult)
{
    ::memset(&flashToolFormatResult, 0, sizeof(flashToolFormatResult));
}

FormatCommand::FormatCommand(const GeneralSetting &generalSetting,
                             const FormatSetting  &formatSetting)
                             : m_GeneralSetting(generalSetting),
                             m_FormatSetting(formatSetting)
{
}

FormatCommand::~FormatCommand()
{
}

bool FormatCommand::Execute()
{
    logger << Logger::Level_Normal
        << __FUNC__ << endl;

    /////////////////////////////////////////////////////////////
    // Load the Download Agent
    /////////////////////////////////////////////////////////////
    DA_HANDLE_T daHandle;
    DAHandleInitializer daHandleInitiailzer(
        daHandle, m_GeneralSetting.GetDownloadAgentFilePath());

    /////////////////////////////////////////////////////////////
    // Load the Auth File
    /////////////////////////////////////////////////////////////

    // FIXME
    const wstring authFilePath(m_GeneralSetting.GetAuthenticationFilePath());
    const bool authEnabled = !authFilePath.empty();
    AUTH_HANDLE_T authHandle = NULL;

    if (authEnabled)
    {
        USES_CONVERSION;
        AUTH_Create(&authHandle);
        AUTH_Load(authHandle, W2CA(authFilePath.c_str()));
    }

    // Maggie ToDo: More should be done for new features, such as SCTRL_CERT

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgComPortSetting
    /////////////////////////////////////////////////////////////

    // fill COM_PORT_SETTING
    COM_PORT_SETTING comPortSetting;
    FillComPortSetting(comPortSetting, m_GeneralSetting);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgFlashTool
    /////////////////////////////////////////////////////////////

    // fill FLASHTOOL_ARG
    FLASHTOOL_ARG flashToolArg;
    FillFlashToolArg(flashToolArg, daHandle, authHandle);

    FLASHTOOL_RESULT flashToolResult;
    FillFlashToolResult(flashToolResult);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::ArgFlashToolFormatEx
    /////////////////////////////////////////////////////////////

    // fill FLASHTOOL_FORMAT_ARG
    FLASHTOOL_FORMAT_ARG_EX flashToolFormatArgEx;
    FillFlashToolFormatArg_Ex(flashToolFormatArgEx, m_FormatSetting);

    FLASHTOOL_FORMAT_RESULT flashToolFormatResult;
    FillFlashToolFormatResult(flashToolFormatResult);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::Download_Internal Q_Q"
    /////////////////////////////////////////////////////////////

    if (comPortSetting.com.number == 0)
    {
        //
        // Search for a newly created COM port
        //
        logger << Logger::Level_Normal
            << "Searching for newly created USB COM port" << endl;

        unsigned short newCOMPortNum;

        if (!SearchNewCOMPort(newCOMPortNum))
        {
            logger << Logger::Level_Normal
                << "Download failed: no USB COM port detected" << endl;
            return false;
        }

        logger << Logger::Level_Normal
            << "USB COM port detected: COM" << newCOMPortNum << endl;

        comPortSetting.com.number = newCOMPortNum;
        flashToolArg.m_boot_arg.m_usb_enable = _TRUE;
    }

    /////////////////////////////////////////////////////////////
    // Call into BROM DLL to do the format
    /////////////////////////////////////////////////////////////

    int stopFlag;
    const int ret = FlashFormat_Ex(&comPortSetting,
        &flashToolArg,
        &flashToolResult,
        &flashToolFormatArgEx,
        &flashToolFormatResult,
        &stopFlag);

    // FIXME
    if (authEnabled)
    {
        AUTH_Destroy(&authHandle);
    }

    if (ret == S_DONE)
    {
        logger << Logger::Level_Normal
            << "Format succeeded." << endl;
        return true;
    }

    logger << Logger::Level_Normal
        << "Format failed: " << StatusToString(ret) << endl;
    return false;
}

static void FillMETAConnectReq(META_Connect_Req &connectReq,
                               const GeneralSetting &generalSetting,
                               AUTH_HANDLE_T &authHandle)
{
    static META_COMM_BAUDRATE allow_baudrate[11] =
    {
        META_BAUD115200,
        META_BAUD230400,
        META_BAUD460800,
        META_BAUD921600,
        META_BAUD1500000,
        META_BAUD_END,
        META_BAUD_END,
        META_BAUD_END,
        META_BAUD_END,
        META_BAUD_END,
        META_BAUD_END
    };

    ::memset(&connectReq, 0, sizeof(connectReq));
    ::memcpy(connectReq.baudrate, allow_baudrate, sizeof(connectReq.baudrate));

    const wstring comPort = generalSetting.GetComPort();

    connectReq.com_port =
        (comPort == L"USB") ? 0 : ::_wtoi(comPort.c_str());

    connectReq.flowctrl           = META_SW_FLOWCTRL;
    connectReq.ms_connect_timeout = 40000;

    connectReq.boot_meta_arg.m_bbchip_type = AUTO_DETECT_BBCHIP;
    connectReq.boot_meta_arg.m_ext_clock   = AUTO_DETECT_EXT_CLOCK;

    connectReq.boot_meta_arg.m_ms_boot_timeout = 30000;
    connectReq.boot_meta_arg.m_max_start_cmd_retry_count =
        DEFAULT_BROM_START_CMD_RETRY_COUNT;

    // This callback function will be invoke after COM port is opened
    // You can do some initialization by using this callback function.
    connectReq.boot_meta_arg.m_cb_com_init_stage     = COMPortOpenCallback;
    connectReq.boot_meta_arg.m_cb_com_init_stage_arg = NULL;

    // This callback function will be invoke after BootROM start cmd is passed.
    // You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
    // or do whatever you want otherwise.
    connectReq.boot_meta_arg.m_cb_in_brom_stage      = NULL;
    connectReq.boot_meta_arg.m_cb_in_brom_stage_arg  = NULL;

    // speed-up BootROM stage baudrate
    connectReq.boot_meta_arg.m_speedup_brom_baudrate = _TRUE;

    // WM_BROM_READY_TO_POWER_ON_TGT
    connectReq.boot_meta_arg.m_ready_power_on_wnd_handle = NULL;
    connectReq.boot_meta_arg.m_ready_power_on_wparam     = NULL;
    connectReq.boot_meta_arg.m_ready_power_on_lparam     = NULL;

    // serial link authentication arg
    connectReq.boot_meta_arg.m_auth_handle              = authHandle;
    connectReq.boot_meta_arg.m_cb_sla_challenge         = SLA_Challenge;
    connectReq.boot_meta_arg.m_cb_sla_challenge_arg     = NULL;
    connectReq.boot_meta_arg.m_cb_sla_challenge_end     = SLA_Challenge_END;
    connectReq.boot_meta_arg.m_cb_sla_challenge_end_arg = NULL;

    // security certificate
    // Maggie ToDo: export the parameter to set secure control certificate
    connectReq.boot_meta_arg.m_scert_handle = NULL;

    // Boot META, usb flag, msut to set false, since call the META_ConnectWithTarget() API
    connectReq.boot_meta_arg.m_usb_enable = _FALSE;
}

static void FillMETAConnectReport(META_Connect_Report &metaConnectReport)
{
    ::memset(&metaConnectReport, 0, sizeof(metaConnectReport));
}

static void FillMETAConnectByUSBReq(META_ConnectByUSB_Req &connectByUSBReq,
                                    const GeneralSetting &generalSetting,
                                    AUTH_HANDLE_T &authHandle)
{
    ::memset(&connectByUSBReq, 0, sizeof(connectByUSBReq));

    const wstring comPort = generalSetting.GetComPort();

    connectByUSBReq.com_port =
        (comPort == L"USB") ? 0 : ::_wtoi(comPort.c_str());

    connectByUSBReq.ms_connect_timeout = 40000;

    connectByUSBReq.boot_meta_arg.m_bbchip_type = AUTO_DETECT_BBCHIP;
    connectByUSBReq.boot_meta_arg.m_ext_clock   = AUTO_DETECT_EXT_CLOCK;

    connectByUSBReq.boot_meta_arg.m_ms_boot_timeout = 30000;
    connectByUSBReq.boot_meta_arg.m_max_start_cmd_retry_count =
        DEFAULT_BROM_START_CMD_RETRY_COUNT;

    // This callback function will be invoke after COM port is opened
    // You can do some initialization by using this callback function.
    connectByUSBReq.boot_meta_arg.m_cb_com_init_stage     = COMPortOpenCallback;
    connectByUSBReq.boot_meta_arg.m_cb_com_init_stage_arg = NULL;

    // This callback function will be invoke after BootROM start cmd is passed.
    // You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
    // or do whatever you want otherwise.
    connectByUSBReq.boot_meta_arg.m_cb_in_brom_stage      = NULL;
    connectByUSBReq.boot_meta_arg.m_cb_in_brom_stage_arg  = NULL;

    // speed-up BootROM stage baudrate
    connectByUSBReq.boot_meta_arg.m_speedup_brom_baudrate = _TRUE;

    // WM_BROM_READY_TO_POWER_ON_TGT
    connectByUSBReq.boot_meta_arg.m_ready_power_on_wnd_handle = NULL;
    connectByUSBReq.boot_meta_arg.m_ready_power_on_wparam     = NULL;
    connectByUSBReq.boot_meta_arg.m_ready_power_on_lparam     = NULL;

    // serial link authentication arg
    connectByUSBReq.boot_meta_arg.m_auth_handle              = authHandle;
    connectByUSBReq.boot_meta_arg.m_cb_sla_challenge         = SLA_Challenge;
    connectByUSBReq.boot_meta_arg.m_cb_sla_challenge_arg     = NULL;
    connectByUSBReq.boot_meta_arg.m_cb_sla_challenge_end     = SLA_Challenge_END;
    connectByUSBReq.boot_meta_arg.m_cb_sla_challenge_end_arg = NULL;

    // security certificate
    // Maggie ToDo: export the parameter to set secure control certificate
    connectByUSBReq.boot_meta_arg.m_scert_handle = NULL;

    // Boot META, usb flag, must to set false, since call the META_ConnectWithTarget() API
    connectByUSBReq.boot_meta_arg.m_usb_enable = _TRUE;
}

static void FillMETAConnectByUSBReport(META_ConnectByUSB_Report &metaConnectReport)
{
    ::memset(&metaConnectReport, 0, sizeof(metaConnectReport));
}

static META_RESULT ConnectToTargetViaUART(const GeneralSetting &generalSetting,
                                          AUTH_HANDLE_T &authHandle)
{
    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::EnterMETAMode
    /////////////////////////////////////////////////////////////

    META_Connect_Req connectReq;
    FillMETAConnectReq(connectReq, generalSetting, authHandle);

    META_Connect_Report connectReport;
    FillMETAConnectReport(connectReport);

    int stopFlag;

    return META_ConnectWithTarget(&connectReq, &stopFlag, &connectReport);
}

static META_RESULT ConnectToTargetViaUSB(const GeneralSetting &generalSetting,
                                         AUTH_HANDLE_T &authHandle)
{
    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::EnterMETAMode
    /////////////////////////////////////////////////////////////

    META_ConnectByUSB_Req connectReq;
    FillMETAConnectByUSBReq(connectReq, generalSetting, authHandle);

    META_ConnectByUSB_Report connectReport;
    FillMETAConnectByUSBReport(connectReport);

    /////////////////////////////////////////////////////////////
    // Refer to tboot_1::EnterMETAResetFactoryDefault @_@
    /////////////////////////////////////////////////////////////
    logger << Logger::Level_Normal
        << "Searching for newly created USB COM port" << endl;

    unsigned short newCOMPortNum;

    if (!SearchNewCOMPort(newCOMPortNum))
    {
        logger << Logger::Level_Normal
            << "No USB COM port detected" << endl;
        return META_FAILED;
    }

    logger << Logger::Level_Normal
        << "USB COM port detected: COM" << newCOMPortNum
        << endl;

    connectReq.com_port = newCOMPortNum;

    int stopFlag;

    return META_ConnectWithTargetByUSB(&connectReq, &stopFlag, &connectReport);
}

static bool ConnectToTarget(const GeneralSetting &generalSetting,
                            AUTH_HANDLE_T &authHandle)
{
    META_RESULT result;

    if (generalSetting.GetComPort() == L"USB")
    {
        result = ConnectToTargetViaUSB(generalSetting, authHandle);
    }
    else
    {
        result = ConnectToTargetViaUART(generalSetting, authHandle);
    }

    if (result != META_SUCCESS)
    {
        logger << Logger::Level_Normal
            << "[META] connect with target failed: "
            << META_GetErrorString(result) << endl;
        return false;
    }

    return true;
}

static bool EnableWatchdogTimer()
{
    FtWatchDog watchDogReq;

    ::memset(&watchDogReq, 0, sizeof(watchDogReq));
    watchDogReq.ms_timeout_interval = 5000;

    return (META_EnableWatchDogTimer(5000, &watchDogReq) == META_SUCCESS);
}

ResetToFactoryDefaultCommand::ResetToFactoryDefaultCommand(
    const GeneralSetting &generalSetting,
    const ResetToFactoryDefaultSetting &resetToFactoryDefaultSetting)
    : m_GeneralSetting(generalSetting),
    m_ResetToFactoryDefaultSetting(resetToFactoryDefaultSetting)
{
}

ResetToFactoryDefaultCommand::~ResetToFactoryDefaultCommand()
{
}

bool ResetToFactoryDefaultCommand::Execute()
{
    logger << Logger::Level_Normal
        << __FUNC__ << endl;

    /////////////////////////////////////////////////////////////
    // Load the Auth File
    /////////////////////////////////////////////////////////////
    // FIXME
    const wstring authFilePath(m_GeneralSetting.GetAuthenticationFilePath());
    const bool authEnabled = !authFilePath.empty();
    AUTH_HANDLE_T authHandle = NULL;

    if (authEnabled)
    {
        USES_CONVERSION;
        AUTH_Create(&authHandle);
        AUTH_Load(authHandle, W2CA(authFilePath.c_str()));
    }

    /////////////////////////////////////////////////////////////
    // Call into BROM DLL to connect to the target
    /////////////////////////////////////////////////////////////

    if (!ConnectToTarget(m_GeneralSetting, authHandle))
    {
        return false;
    }

    // FIXME
    if (authEnabled)
    {
        AUTH_Destroy(&authHandle);
    }

    /////////////////////////////////////////////////////////////
    // Call into BROM DLL to reset to factory default
    /////////////////////////////////////////////////////////////
    const META_RESULT result = META_NVRAM_ResetToFactoryDefault(40000);

    if (m_ResetToFactoryDefaultSetting.IsWatchdogTimerEnabled())
    {
        if (!EnableWatchdogTimer())
        {
            META_DisconnectWithTarget();
            return false;
        }
    }

    /////////////////////////////////////////////////////////////
    // Call into BROM DLL to disconnect
    /////////////////////////////////////////////////////////////
    META_DisconnectWithTarget();

    if (result == META_SUCCESS)
    {
        logger << Logger::Level_Normal
            << "Reset-to-factory-default succeeded." << endl;
        return true;
    }

    logger << Logger::Level_Normal
        << "Reset-to-factory-default failed: "
        << META_GetErrorString(result) << endl;
    return false;
}

static void FillBackupReq(MISC_BACKUP_REQ_T &backupReq,
                          const char *iniFilePath,
                          const char *backupDirectoryPath)
{
    memset(&backupReq, 0, sizeof(backupReq));

    backupReq.m_pIniFilePath      = const_cast<char *>(iniFilePath);
    backupReq.m_pBackupFolderPath = const_cast<char *>(backupDirectoryPath);
    backupReq.cb_progress         = NULL;
    backupReq.cb_progress_arg     = NULL;
}

static void FillBackupResult(BACKUP_RESULT_T &backupResult)
{
    ::memset(&backupResult, 0, sizeof(backupResult));
}

static bool SupportRID(AnsiString &str_BackupDirNumber)
{
    int ret;    
    ret = META_QueryIfFunctionSupportedByTarget(
        500, "META_MISC_GetRID");
    if (ret != META_SUCCESS)
    {
        // using COM port number
        return false;
    }  

    // using Meta API query RID 
    // Get RID using meta API
    char c_RID[50]={NULL}; // 
    if(META_SUCCESS != (ret = META_MISC_GetRID(1000, c_RID, 16)) )
    {
        // using COM port number
        return false;
    }

    if(c_RID[0] == NULL)
    {
        // Platform doesn't support RID
        // using COM port number
        return false;
    }

    for(int i=0;i<50;i++)
    {
        if(c_RID[i] ==  NULL)
            break;
        else
            str_BackupDirNumber += c_RID[i] ;
    }

    return true;
}

BackupCommand::BackupCommand(const GeneralSetting &generalSetting,
                             const BackupSetting &backupSetting)
                             : m_GeneralSetting(generalSetting),
                             m_BackupSetting(backupSetting)
{
}

BackupCommand::~BackupCommand()
{
}

bool BackupCommand::Execute()
{
    logger << Logger::Level_Normal
        << __FUNC__ << endl;

    const wstring authFilePath(m_GeneralSetting.GetAuthenticationFilePath());
    const bool authEnabled = !authFilePath.empty();
    AUTH_HANDLE_T authHandle = NULL;

    if (authEnabled)
    {
        USES_CONVERSION;
        AUTH_Create(&authHandle);
        AUTH_Load(authHandle, W2CA(authFilePath.c_str()));
    }

    if (!ConnectToTarget(m_GeneralSetting, authHandle))
    {
        return false;
    }

    // FIXME
    if (authEnabled)
    {
        AUTH_Destroy(&authHandle);
    }

    AnsiString str_BackupDirNumber;
    wstring wstr_backupDirectoryPath;
    if(!SupportRID(str_BackupDirNumber))
    {
        // COM PORT
        wstr_backupDirectoryPath = m_BackupSetting.GetBackupDirectoryPath();
    }
    else
    {
        // RID
        AnsiString str_backupDirectoryPath = ExtractFilePath(Application->ExeName) + "backup" + '\\' + str_BackupDirNumber;
        TOleString OlebackupDirectoryPath(AnsiToOLESTR(str_backupDirectoryPath.c_str()));
        wstr_backupDirectoryPath = static_cast<BSTR>(OlebackupDirectoryPath);
    }
    const wstring backupDirectoryPath = wstr_backupDirectoryPath;
    if (!CreateDirectory(backupDirectoryPath))
    {
        logger << Logger::Level_Normal
            << "Failed to create backup directory" << endl;
        META_DisconnectWithTarget();
        return false;
    }

    USES_CONVERSION;
    META_RESULT result = META_DeleteAllFilesInBackupFolder(
        W2CA(backupDirectoryPath.c_str()));

    if (result != META_SUCCESS)
    {
        logger << Logger::Level_Normal
            << "Failed to clear backup directory" << endl;
        META_DisconnectWithTarget();
        return false;
    }

    //
    // Backup calibration data
    //
    result = META_QueryIfFunctionSupportedByTarget(
        5000, "META_MISC_GetIMEILocation");

    if (result != META_SUCCESS)
    {
        if (result != META_FUNC_NOT_IMPLEMENT_YET)
        {
            META_DisconnectWithTarget();
            return false;
        }

        MISC_BACKUP_REQ_T backupReq;
        FillBackupReq(backupReq,
            W2CA(m_BackupSetting.GetIniFilePath().c_str()),
            W2CA(backupDirectoryPath.c_str()));

        int stopFlag;

        result = META_BasicBackupCalibrationData(&backupReq, &stopFlag);

        if (result != META_SUCCESS)
        {
            logger << Logger::Level_Normal
                << "Failed to backup calibration data" << endl;
            META_DisconnectWithTarget();
            return false;
        }
    }
    else
    {
        MISC_BACKUP_REQ_T backupReq;
        FillBackupReq(backupReq,
            W2CA(m_BackupSetting.GetIniFilePath().c_str()),
            W2CA(backupDirectoryPath.c_str()));

        int stopFlag;

        result = META_BackupCalibrationData(&backupReq, &stopFlag);

        if (result != META_SUCCESS)
        {
            logger << Logger::Level_Normal
                << "Failed to backup calibration data" << endl;
            META_DisconnectWithTarget();
            return false;
        }
    }

    logger << Logger::Level_Normal
        << "Backup succeeded." << endl;

    BACKUP_RESULT_T backupResult;
    FillBackupResult(backupResult);
    wstring backDirPath = backupDirectoryPath + L"\\";
    result = META_GetBackupResultInfo(W2CA(backDirPath.c_str()),
        &backupResult);

    if (result != META_SUCCESS)
    {
        logger << Logger::Level_Normal
            << "Failed to get backup result" << endl;
        logger <<  "Path" << W2CA(backupDirectoryPath.c_str()) << endl;
        META_DisconnectWithTarget();
        return false;
    }

    if (m_BackupSetting.IsWatchdogTimerEnabled())
    {
        if (!EnableWatchdogTimer())
        {
            META_DisconnectWithTarget();
            return false;
        }
    }

    META_DisconnectWithTarget();

    return true;
}

bool BackupCommand::CreateDirectory(const wstring &directoryPath)
{
    if (::CreateDirectoryW(directoryPath.c_str(), NULL) != 0)
    {
        return true;
    }

    return (::GetLastError() == ERROR_ALREADY_EXISTS);
}

static void FillRestoreReq(MISC_RESTORE_REQ_T &restoreReq,
                           const char *iniFilePath,
                           const char *restoreDirectoryPath)
{
    ::memset(&restoreReq, 0, sizeof(restoreReq));

    restoreReq.m_pIniFilePath      = const_cast<char *>(iniFilePath);
    restoreReq.m_pBackupFolderPath = const_cast<char *>(restoreDirectoryPath);
    restoreReq.cb_progress         = NULL;
    restoreReq.cb_progress_arg     = NULL;
}

static void FillRestoreResult(RESTORE_RESULT_T &restoreResult)
{
    ::memset(&restoreResult, 0, sizeof(restoreResult));
}

RestoreCommand::RestoreCommand(const GeneralSetting &generalSetting,
                               const RestoreSetting &restoreSetting)
                               : m_GeneralSetting(generalSetting),
                               m_RestoreSetting(restoreSetting)
{
}

RestoreCommand::~RestoreCommand()
{
}

bool RestoreCommand::Execute()
{
    USES_CONVERSION;
    logger << Logger::Level_Normal
        << __FUNC__ << endl;

    // FIXME
    const wstring authFilePath(m_GeneralSetting.GetAuthenticationFilePath());
    const bool authEnabled = !authFilePath.empty();
    AUTH_HANDLE_T authHandle = NULL;

    if (authEnabled)
    {
        USES_CONVERSION;
        AUTH_Create(&authHandle);
        AUTH_Load(authHandle, W2CA(authFilePath.c_str()));
    }

    if (!ConnectToTarget(m_GeneralSetting, authHandle))
    {
        return false;
    }

    // FIXME
    if (authEnabled)
    {
        AUTH_Destroy(&authHandle);
    }

    META_RESULT result = META_QueryIfFunctionSupportedByTarget(
        5000, "META_MISC_GetIMEILocation");

    //const wstring restoreDirectoryPath(
    //                    m_RestoreSetting.GetRestoreDirectoryPath());

    AnsiString str_RestoreDirNumber="";
    wstring wstr_restoreDir;
    if(SupportRID(str_RestoreDirNumber)==false)
    {
        // COM PORT
        wstr_restoreDir = m_RestoreSetting.GetRestoreDirectoryPath();
    }
    else
    {
        // RID
        str_RestoreDirNumber = ExtractFilePath(Application->ExeName) + "backup" + '\\' + str_RestoreDirNumber;
        TOleString OleRestoreDirectoryPath(AnsiToOLESTR(str_RestoreDirNumber.c_str()));
        wstr_restoreDir = static_cast<BSTR>(OleRestoreDirectoryPath);
    }
    const wstring restoreDirectoryPath = wstr_restoreDir;


    if (result != META_SUCCESS)
    {
        if (result != META_FUNC_NOT_IMPLEMENT_YET)
        {
            META_DisconnectWithTarget();
            return false;
        }

        MISC_RESTORE_REQ_T restoreReq;

        FillRestoreReq(restoreReq,
            W2CA(m_RestoreSetting.GetIniFilePath().c_str()),
            W2CA(restoreDirectoryPath.c_str()));

        int stopFlag;

        result = META_BasicRestoreCalibrationData(&restoreReq, &stopFlag);

        if (result != META_SUCCESS)
        {
            logger << Logger::Level_Normal
                << "Failed to restore calibration data" << endl;
            META_DisconnectWithTarget();
            return false;
        }
    }
    else
    {
        MISC_RESTORE_REQ_T restoreReq;

        FillRestoreReq(restoreReq,
            W2CA(m_RestoreSetting.GetIniFilePath().c_str()),
            W2CA(restoreDirectoryPath.c_str()));

        int stopFlag;

        result = META_RestoreCalibrationData(&restoreReq, &stopFlag);

        if (result != META_SUCCESS)
        {
            logger << Logger::Level_Normal
                << "Failed to restore calibration data" << endl;
            META_DisconnectWithTarget();
            return false;
        }
    }

    logger << Logger::Level_Normal
        << "Restore succeeded." << endl;

    RESTORE_RESULT_T restoreResult;
    FillRestoreResult(restoreResult);

    result = META_GetRestoreResultInfo(W2CA(restoreDirectoryPath.c_str()),
        &restoreResult);

    if (result != META_SUCCESS)
    {
        logger << Logger::Level_Normal
            << "Failed to get restore result" << endl;
        META_DisconnectWithTarget();
        return false;
    }

    result = META_DeleteAllFilesInBackupFolder(
        W2CA(restoreDirectoryPath.c_str()));

    if (result != META_SUCCESS)
    {
        logger << Logger::Level_Normal
            << "Failed to delete all files in the backup directory" << endl;
        META_DisconnectWithTarget();
        return false;
    }

    if (m_RestoreSetting.IsWatchdogTimerEnabled())
    {
        if (!EnableWatchdogTimer())
        {
            META_DisconnectWithTarget();
            return false;
        }
    }

    META_DisconnectWithTarget();

    return true;
}
