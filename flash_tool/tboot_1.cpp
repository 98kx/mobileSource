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
*  tboot_1.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  a thread
*  1.write command to Boot ROM
*  2.then jump to execute download agent
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tboot_1.h"
#include "ext_mem1.h"
#include "main.h"
#include "ok_wnd.h"
#include "com_util.h"
#include "flashtool.h"
#include "flashtool_api.h"
#include "DA_Option.h"
#include "meta.h"
#include "SLA_Challenge.h"



#pragma package(smart_init)

//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit4::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall tboot_1::tboot_1(bool CreateSuspended )
        : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall tboot_1::ClearStatusBar()
{
	MainForm->HWStatusBarClear();
}
//---------------------------------------------------------------------------
void tboot_1::SyncInitProgress()
{
	Synchronize(InitProgress);
}

void __fastcall tboot_1::InitProgress()
{
	MainForm->CGauge1->Hint      = progress_hint;
	MainForm->CGauge1->ForeColor = progress_forecolor;
	MainForm->CGauge1->Progress  = progress_percentage;
	if ( bUpdateFinishBytes ) {
		g_ProgressTimeSec = 0;
	    MainForm->sb_Main->Panels->Items[0]->Text = IntToStr(progress_finished_bytes) + " Bytes / 0 Bps";
	}
}
//---------------------------------------------------------------------------
void tboot_1::SyncUpdateProgress()
{
	Synchronize( UpdateProgress );
}
void __fastcall tboot_1::UpdateProgress()
{
	unsigned int    Bps;
	float           KBps;
	AnsiString      rate;

	MainForm->CGauge1->Progress = progress_percentage;

	if( bUpdateFinishBytes ) {

		g_DisplayFinishedBytes = progress_finished_bytes;

		if( 0 >= g_ProgressTimeSec ) {
			rate = "0 Bps";
		}
		else if( 1024 < (Bps=progress_finished_bytes/g_ProgressTimeSec) ) {
			KBps = Bps;
			rate = FloatToStrF( KBps/1024, ffFixed, 7, 2) + " KBps";
		}
		else {
			rate = IntToStr(Bps) +  " Bps";
		}
	    MainForm->sb_Main->Panels->Items[0]->Text = IntToStr(progress_finished_bytes) + " Bytes / " + rate;
	}
}
//---------------------------------------------------------------------------
void tboot_1::SyncShowMsg()
{
    Synchronize( ShowMsg );
}
void __fastcall tboot_1::ShowMsg()
{
    ShowMessage( asMsg );

    // reset message string
    asMsg = "";
}
//---------------------------------------------------------------------------
void __fastcall tboot_1::ShowOK_Form()
{
	Form_OK->Visible = true;
	StopTimer();
}
//---------------------------------------------------------------------------
void __fastcall tboot_1::StartTimer()
{
	MainForm->Timer_Download->Enabled = true;
}
void __fastcall tboot_1::StopTimer()
{
	MainForm->Timer_Download->Enabled = false;
}
//---------------------------------------------------------------------------
bool tboot_1::IsUSBCOMPortReady(unsigned short comPortNum)
{
    for (int i=0; i<10; ++i)
    {
        // Wait for 200ms
        Sleep(200);

        // Try to open the COM port to see if it's ready to be used
        HANDLE hCOM = INVALID_HANDLE_VALUE;

        if (open_com_port(hCOM, comPortNum, 115200))
        {
            if (hCOM != NULL)
            {
                close_com_port(hCOM);
            }

            return true;
        }

        if (hCOM != NULL)
        {
            close_com_port(hCOM);
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool tboot_1::GetDynamicComPort(unsigned short *com_port)
{
    int i, ret;
    unsigned short com_port_list_org[255];
    unsigned short com_port_count_org = sizeof(com_port_list_org)/sizeof(unsigned short);
    unsigned short com_port_list[255];
    unsigned short com_port_count = sizeof(com_port_list)/sizeof(unsigned short);
    bool bFindComPort = false;
    bool b_com_port_list[255];


    if( COM_ENUM_OK != ( ret = ComPortEnumerate(com_port_list_org, &com_port_count_org)) )
    {
      	if( COM_ENUM_NO_COM_PORT_FOUND == ret )
        {
            for (int i=0; i<sizeof(com_port_list_org)/sizeof(com_port_list_org[0]); ++i)
            {
                com_port_list_org[i] = 0;
            }

	        com_port_count_org =0;
        }
        else {
    		asMsg = "Failed to enumerate a certain COM port! ";
	    	Synchronize(ShowMsg);
            return false;
        }
    }

    while(1)
    {
        // check if the stop button is pressed
        if( g_stopflag == BOOT_STOP ) {
            return false;
        }

        com_port_count = sizeof(com_port_list)/sizeof(com_port_list[0]);

        if( COM_ENUM_OK != ( ret = ComPortEnumerate(com_port_list, &com_port_count) ) )
        {
      	    if( COM_ENUM_NO_COM_PORT_FOUND == ret )
            {
            	for (int i=0; i<sizeof(com_port_list)/sizeof(com_port_list[0]); ++i)
            	{
                    com_port_list[i] = 0;
            	}

                com_port_count=0;
                continue;
           	}
           	else
            {
                continue;
	        }
       	}

        if( com_port_count == com_port_count_org && !memcmp((void *)com_port_list, (void *)com_port_list_org, com_port_count_org*sizeof(unsigned short)) )
        {
            // COM ports are the same!
           	continue;
        }

        for( i = 0; i < 255; i++ ) b_com_port_list[i] = false;
        for( i = 0; i < com_port_count_org; i++ ) b_com_port_list[com_port_list_org[i]] = true;

        for( i = 0; i < com_port_count; i++ )
        {
            if( b_com_port_list[com_port_list[i]] == false )
            {
                // Find a new com port : COM%d",com_port_list[i]);
                *com_port = com_port_list[i];
                bFindComPort = true;
                break;
            }
        }

        if( bFindComPort ) break;

        com_port_count_org = com_port_count;
        memcpy((void*)com_port_list_org, (void*)com_port_list, sizeof(com_port_list)/ sizeof(unsigned short));
    }

    return IsUSBCOMPortReady(*com_port);
}
//---------------------------------------------------------------------------
void __fastcall tboot_1::Execute()
{
    button_sentry	s;

    if(Form_OK->Visible == true)
    {
        Form_OK->Visible = false;   // force to close the Form_OK
    }
    if( g_stopflag == BOOT_STOP ) return;

    g_HasOngoingOperation = true;

    Synchronize(ClearStatusBar);

    progress_hint           = "";
    progress_forecolor      = clBlue;
    progress_percentage     = 0;
    progress_finished_bytes = 0;

    bUpdateFinishBytes      = true;
    SyncInitProgress();


    // reset error code mode
    g_error_code_mode      = FLASHTOOL;
    g_finish_warning_code           = FLASHTOOL_WARNING_NONE;

    if( g_bDebugDA ) {
        DebugDA();
        // Memory Test cannot be operated automatically.
    }
    else {
        // flash download/ read back / format / memory test/ ...
        FlashTool();
    }


    Terminate();
    g_stopflag = 0;
    g_HasOngoingOperation = false;
}
/******************************************************************************/
/* FlashTool Modules                                                          */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
static int __stdcall cb_download_da_init(void *usr_arg);
static int __stdcall cb_download_da(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);
static int __stdcall cb_download_flash_init(void *usr_arg);
static int __stdcall cb_download_flash(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);
static int __stdcall cb_download_bloader_init(void *usr_arg);
static int __stdcall cb_download_bloader(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);
static int __stdcall cb_format_report_init(HW_StorageType_E storage_type, unsigned int begin_addr, unsigned int length, void *usr_arg);
static int __stdcall cb_format_report(unsigned char percent, void *usr_arg);
static int __stdcall cb_readback_flash_init(HW_StorageType_E  storage_type, unsigned int rb_addr, unsigned int rb_length, const char *rb_filepath, void *usr_arg);
static int __stdcall cb_readback_flash(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);
static int __stdcall cb_da_report(const DA_REPORT_T  *p_da_report, void *usr_arg);
static int __stdcall cb_com_init_stage(HANDLE hCOM, void *usr_arg);
static int __stdcall cb_in_brom_stage(unsigned int brom_handle, HANDLE hCOM, void *usr_arg);
static int __stdcall cb_format_statistics(const FormatStatisticsReport_T  *p_report, void *usr_arg);
static int __stdcall cb_post_process_init(void *usr_arg);
static int __stdcall cb_post_process(unsigned char percent, void *usr_arg);
static int __stdcall cb_progress_init(void *usr_arg);
static int __stdcall cb_progress(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);
static int __stdcall cb_security_pre_process_notify(void *usr_arg);
static int __stdcall cb_security_post_process_notify(void *usr_arg);
static int __stdcall cb_update_param_init(void *usr_arg);
static int __stdcall cb_update_param(unsigned char percent, void *usr_arg);
static int __stdcall cb_enable_boot_init(void *usr_arg);
static int __stdcall cb_enable_boot(unsigned char percent, void *usr_arg);
static int __stdcall cb_download_fat_change_notify(void *usr_arg);
//---------------------------------------------------------------------------
void tboot_1::FlashTool()
{
    int ret = S_DONE;

//*** DOWNLOAD ===========================================================
    if( FLASHTOOL_DOWNLOAD == g_flashtool_function )
    {
        // backup -> download -> restore
        if ( g_backup_restore_state == BACKUPRESTORE  )
        {
            ret = BackupDownloadRestore();
            if( S_DONE != ret ) goto tboot_end;
        }
        // download -> restore
        else if ( g_backup_restore_state == RESTORE )
        {
            ret = DownloadRestore();
            if( S_DONE != ret ) goto tboot_end;
        }
        // retore only
        else if ( g_backup_restore_state == RESTOREONLY )
        {
            ret = RestoreOnly();
            if( S_DONE != ret ) goto tboot_end;
        }
        // donwload -> upload
        else if ( g_upload_state == UPLOAD )
        {
            ret = DownloadUpload();
            if( S_DONE != ret ) goto tboot_end;
        }
        // upload only
        else if ( g_upload_state == UPLOADONLY )
        {
            ret = UploadOnly();
            if( S_DONE != ret ) goto tboot_end;
        }
        // basic download
        else
        {
            ret = Download();
            if( S_DONE != ret ) goto tboot_end;
        }

    }
//*** READBACK ===========================================================
    else if( FLASHTOOL_READBACK == g_flashtool_function )
    {
        if (g_bUsbDLRB)
        {
            // Search for a new USB COM port
            if (!GetDynamicComPort((unsigned short *)&COM_PORT))
            {
                goto tboot_end;
            }

            MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
        }

        // fill the flash readback argurment
        ArgFlashReadback();
        // call flash read bcak api
		ret = FlashReadback(&com, &arg, &result, &rb_arg, &rb_result, &g_stopflag);
        if( S_DONE != ret ) goto tboot_end;
    }

//*** FORMAT =============================================================
    else if( FLASHTOOL_FORMAT == g_flashtool_function )
    {
        // backup -> format -> restore
        if ( g_backup_restore_state == BACKUPRESTORE  )
        {
            ret = BackupFormatRestore();
            if( S_DONE != ret ) goto tboot_end;
        }
        // format -> restore
        else if ( g_backup_restore_state == RESTORE  )
        {
            ret = FormatRestore();
            if( S_DONE != ret ) goto tboot_end;
        }
        // retore only
        else if ( g_backup_restore_state == RESTOREONLY )
        {
            ret = RestoreOnly();
            if( S_DONE != ret ) goto tboot_end;
        }
        // format -> upload
        else if ( g_upload_state == UPLOAD )
        {
            ret = FormatUpload();
            if( S_DONE != ret ) goto tboot_end;
        }
        // upload only
        else if ( g_upload_state == UPLOADONLY )
        {
            ret = UploadOnly();
            if( S_DONE != ret ) goto tboot_end;
        }
        // basic format
        else {
            ret = Format();
            if( S_DONE != ret ) goto tboot_end;
        }

    }

//*** MEMORYTEST============================================================
    else if( FLASHTOOL_MEMORYTEST == g_flashtool_function )
    {
        ret = MemoryTest();
        if( S_DONE != ret ) goto tboot_end;
    }

//*** PARAMETER============================================================
    else if( FLASHTOOL_PARAMETER == g_flashtool_function )
    {
        ret = Parameter();
        if( S_DONE != ret ) goto tboot_end;
    }

//*** ENABLE_BOOT============================================================
    else if( FLASHTOOL_ENABLE_BOOT == g_flashtool_function )
    {
        ret = EnableBoot();
        if( S_DONE != ret ) goto tboot_end;
    }

//*** FLASHTOOL API TEST ==================================================
    else if( FLASHTOOL_API == g_flashtool_function )
    {
        ret = FlashToolAPITest ();
        if( S_DONE != ret ) goto tboot_end;
    }

//***FLASHTOOL_DONE========================================================
tboot_end:

    // stop timer
    Synchronize( StopTimer );

    // check the stop flag and do not go show the final message
	if( BOOT_STOP == g_stopflag ) return;

    // show flash tool final Result
    ShowFlashToolFinalResult ( ret );

}
//---------------------------------------------------------------------------
void tboot_1::ShowFlashToolFinalResult( int ret )
{

    if ( S_DONE == ret ) {  // ok case

        if(g_finish_warning_code != FLASHTOOL_WARNING_NONE)
        {
            switch(g_finish_warning_code)
            {
                 case FLASHTOOL_WARNING_BL_NOT_UPDATED:
                     asMsg = "WARNING: Under the UART Download for EndUser mode, BootLoader is not updated. Please Notice it!!";
                     break;
                 default:
                     break;
            }
            // pop out the error message
            Synchronize( ShowMsg );
        }

        // show OK form
        Synchronize( ShowOK_Form );

    } else {

        switch ( g_error_code_mode )
        {
            case GPS:
                // get GPS download error message
                GetGPSErrorMessag( (WRAPPER_RETURN)ret );
                // pop out the error message
                Synchronize( ShowMsg );

                break;


            case META:  // get META error message
                if ( ret > 0 ) {
                    asMsg = "META ERROR: " + AnsiString(META_GetErrorString( (META_RESULT)ret )) + "\r\n" + asMsg;
                }
                // pop out the error message
                Synchronize( ShowMsg );
                break;

            case FLASHTOOL: // flash tool error message
            default:
                GetErrorMessag( (STATUS_E) ret, result.m_da_report.m_msp_err_code );
                // pop out the error message
                Synchronize( ShowMsg );
                break;
        }
    }
}
//---------------------------------------------------------------------------
int tboot_1::Download( void )
{
    int ret = Download_Internal();

    if (ret != S_DONE)
    {
        return ret;
    }

    // GPS download relay mode
    if(  g_DL_HANDLE_NODE.m_param.m_gps_enable )
    {
        if ( g_bUsbDLRB )
        {
            asMsg = " GPS Download was forbidden by USB! \n Please use the UART cable to download 'GPS ROM'. ";
            return -1;
        }

        g_error_code_mode = GPS;
        ret = GPSDownload();
        if ( S_DONE != ret ) return ret;
    }

    // enter meta mode to reset the FAT area to factory default
    if( RESET_TO_FACTORY == g_FormatAfterDownloadOption )
    {
        // when download the GPS ROM, since DA will entert the relay mode, and lose the control
        // use the fomat option to reset to factory default
        if( g_DL_HANDLE_NODE.m_param.m_gps_enable )
        {
            asMsg = " Please press Format and Select 'Reset to Factroy Deafault'. ";
            return -1;
        }

        g_error_code_mode = META;
        ret = EnterMETAResetFactoryDefault();
        if( META_SUCCESS != ret ) return ret;
    }

    return S_DONE;
}
//---------------------------------------------------------------------------
int tboot_1::Download_Internal()
{
    g_error_code_mode = FLASHTOOL;

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    // fill flash download argument
    ArgFlashDownload();

    const int ret = FlashDownload( &com, &arg, &result, &dl_arg, &dl_result, &g_stopflag );

    if (ret != S_DONE)
    {
        return ret;
    }

    if(dl_result.m_BLNotDownloadWarning)
    {
        g_finish_warning_code = FLASHTOOL_WARNING_BL_NOT_UPDATED;
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::BackupDownloadRestore()
{
    // start timer
    Synchronize( StartTimer );

    bool isPreserveBackupData;
    bool isUseComPortStoreData;
    MainForm->GetBackupRestoreStatus(&isPreserveBackupData,
                                    &isUseComPortStoreData,
                                    NULL);
    // enter meta to backup
    AnsiString BackupCalibrationDataFolderPath;
    int ret = EnterMETABackup (isPreserveBackupData,
                               isUseComPortStoreData,
                               BackupCalibrationDataFolderPath);

    if ( META_SUCCESS != ret ) {
        return ret;
    }

    // download
    ret = Download_Internal();

    if ( S_DONE != ret ) {
        // Display backup succeed message
        asMsg =  "Already Backup! Watch dog reset might not work. Please try the\n\
                  'Donwload/Format ->Restore' option . If the condition is the same, \n\
                  please call for help.\n";
        asMsg += " The backup folder path is: " + BackupCalibrationDataFolderPath;
        return ret;
    }

    // download gps firmware, when enter the gps relay mode, DA will lose the contol,
    // watch dog reset will be invalid
    if( g_DL_HANDLE_NODE.m_param.m_gps_enable )
    {
        g_error_code_mode = GPS;
        ret = GPSDownload();
        if ( S_DONE != ret ) {
            asMsg = "Already Backup! If the condition is the same, please call for help\n";
            return ret;
        }
        // Display backup succeed message
        asMsg = "Download Succeed! Please use 'Restore Only' option to restore the backup files.";
        return -1;
    }


    // enter meta to restore
    ret = EnterMETARestore( isPreserveBackupData,
                            isUseComPortStoreData,
                            BackupCalibrationDataFolderPath);
    if ( META_SUCCESS != ret ) {
         // Display backup succeed message
        asMsg += "Download Succeed! Please try 'Restore Only' option. If the condition is the same, please call for help.\n";
        asMsg += "The backup folder path is: " + BackupCalibrationDataFolderPath;
        return ret;
    }


    return S_DONE;
}
//---------------------------------------------------------------------------
int  tboot_1::DownloadRestore( void )
{
    // mark it, because of EnterMETARestore has already check it
    /*
    // check if the backup folder exist
    if (!g_bUsbDLRB)
    {
        if ( ! CheckBackupDirExist() ) {
            asMsg = "Backup folder does not exist. Please use the option 'Backup->Download->Restore' to backup first.";
            return -1;
        }
    }
    */

    // download
    int ret = Download_Internal();

    if ( S_DONE != ret ) {
        asMsg = "Already Backup! Please try again. If the condition is the same, please call for help\n";
        return ret;
    }

    // download gps firmware, when enter the gps relay mode, DA will lose the contol,
    // watch dog reset will be invalid
    if( g_DL_HANDLE_NODE.m_param.m_gps_enable )
    {
        g_error_code_mode = GPS;
        ret = GPSDownload();
        if ( S_DONE != ret ) {
            asMsg = "Already Backup! If the condition is the same, please call for help\n";
            return ret;
        }
        // Display backup succeed message, and let user to use Restore Only
        asMsg = "Download Succeed! Please use 'Restore Only' option.";
        return -1;
    }


    // enter meta to restore
    AnsiString BackupCalibrationDataFolderPath;
    bool       isPreserveBackupData;
    bool       isUseComPortStoreData;
    MainForm->GetBackupRestoreStatus(&isPreserveBackupData,
                                    &isUseComPortStoreData,
                                    &BackupCalibrationDataFolderPath);
    ret = EnterMETARestore(isPreserveBackupData,
                           isUseComPortStoreData,
                           BackupCalibrationDataFolderPath);

    if ( META_SUCCESS != ret ) {
         // Display backup succeed message
        asMsg = "Download Succeed! Please try 'Restore Only' option. If the condition is the same, please call for help.\n";
        return ret;
    }


    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::Format( void )
{
    if( RESET_TO_FACTORY == g_FormatOption )
    {
        // set error code mode to META
        g_error_code_mode = META;

        // go to META mode to reset factory default
        return EnterMETAResetFactoryDefault( );
    }
    else {
        return Format_Internal();
    }
}
//---------------------------------------------------------------------------
int tboot_1::Format_Internal()
{
    g_error_code_mode = FLASHTOOL;

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    // fill the flash format argurment
    ArgFlashFormatEx();

    return FlashFormat_Ex(&com, &arg, &result, &fmt_arg_ex, &fmt_result, &g_stopflag);
}
//---------------------------------------------------------------------------
int tboot_1::BackupFormatRestore( void )
{
    //-1. start timer
  	Synchronize( StartTimer );

    //-2. enter meta to backup
    AnsiString BackupCalibrationDataFolderPath;
    bool isPreserveBackupData;
    bool isUseComPortStoreData;
    MainForm->GetBackupRestoreStatus(&isPreserveBackupData,
                                     &isUseComPortStoreData,
                                     NULL);
    int ret = EnterMETABackup (isPreserveBackupData,
                               isUseComPortStoreData,
                               BackupCalibrationDataFolderPath);
    if ( META_SUCCESS != ret ) {
        return ret;
    }

    //-3. format and watch dog reset
    if( RESET_TO_FACTORY != g_FormatOption )
    {
        ret = Format_Internal();
        if ( S_DONE != ret ) {
            asMsg = "Already Backuped! Please try 'Download/Format->Restore' option. If the condition is the same, please call for help.\n";
            asMsg += "The backup folder path is: " + BackupCalibrationDataFolderPath;
            return ret;
        }
    }

    //-4. enter meta to restore (reset to factory default (if required))
    ret = EnterMETARestore(isPreserveBackupData,
                           isUseComPortStoreData,
                           BackupCalibrationDataFolderPath);
    if ( META_SUCCESS != ret ) {
         // Display backup succeed message
        asMsg += "Format Succeed! Please try 'Restore Only' option. If the condition is the same, please call for help.\n";
        asMsg += "The backup folder path is: " + BackupCalibrationDataFolderPath;
        return ret;
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::FormatRestore( void )
{
    //-1. start timer
    Synchronize( StartTimer );

    // mark it, because of EnterMETARestore has already checked it
    /*
    //-2. check if the backup folder exist
    if (!g_bUsbDLRB)
    {
        if ( ! CheckBackupDirExist() ) {
            asMsg = "Backup folder does not exist. Please use the option 'Backup->Format->Restore' to backup first.";
            Synchronize( StopTimer );
            return -1;
        }
    }
    */

    int ret;

    //-3. format and watch dog reset
    if( RESET_TO_FACTORY != g_FormatOption )
    {
        ret = Format_Internal();
        if ( S_DONE != ret ) {
            asMsg = "Format failed. If the condition is the same, please call for help.\n";
            return ret;
        }
    }

    //-4. enter meta to restore
    bool isPreserveBackupData;
    bool isUseComPortStoreData;
    AnsiString BackupCalibrationDataFolderPath;
    MainForm->GetBackupRestoreStatus(&isPreserveBackupData,
                                     &isUseComPortStoreData,
                                     &BackupCalibrationDataFolderPath);
    ret = EnterMETARestore(isPreserveBackupData,
                           isUseComPortStoreData,
                           BackupCalibrationDataFolderPath);
    if ( META_SUCCESS != ret ) {
         // Display backup succeed message
        asMsg = "Format Succeed! Please try 'Restore Only' option. If the condition is the same, please call for help.\n";
        return ret;
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::RestoreOnly( void )
{
    //-1. start timer
  	Synchronize( StartTimer );

    // mark it, because EnterMETARestore has already checked it
    /*
    //-2. check if the backup folder exist
    if (!g_bUsbDLRB)
    {
        if ( ! CheckBackupDirExist() ) {
            asMsg = "Backup folder does not exist. Please use the option 'Backup->Download/Format->Restore' to backup first.";
            return -1;
        }
    }
    */

    //-3. enter meta to restore
    bool isPreserveBackupData;
    bool isUseComPortStoreData;
    AnsiString BackupCalibrationDataFolderPath;
    MainForm->GetBackupRestoreStatus(&isPreserveBackupData,
                                     &isUseComPortStoreData,
                                     &BackupCalibrationDataFolderPath);
    int ret = EnterMETARestore(isPreserveBackupData,
                               isUseComPortStoreData,
                               BackupCalibrationDataFolderPath);
    if ( META_SUCCESS != ret ) {
        asMsg = "Already Backuped! Please try again. If the condition is the same, please call for help.\n";
        return ret;
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::DownloadUpload ( void )
{
    int ret = Download_Internal();
    if ( S_DONE != ret ) return ret;

    //-2. download GPS Firmware
    // download gps firmware, when enter the gps relay mode, DA will lose the contol,
    // watch dog reset will be invalid
    if( g_DL_HANDLE_NODE.m_param.m_gps_enable )
    {
        g_error_code_mode = GPS;
        ret = GPSDownload();
        if ( S_DONE != ret ) {
            return ret;
        }
        // Display downlaod succeed message, and let user to use the 'Upload Only' option
        asMsg = "Download Succeed! Please use 'Upload Only' option.";
        return -1;
    }


    //-3. enter meta to upload
    ret = EnterMETAUpload();
    if ( META_SUCCESS != ret ) {
         // Display backup succeed message
        asMsg = "Download Succeed! Please try the 'Upload Only' option. If the condition is the same, please call for help.\n";
        return ret;
    }


    return ret;

}
//---------------------------------------------------------------------------
int tboot_1::FormatUpload( void )
{
    //-1. start timer
  	Synchronize( StartTimer );

    int ret;

    //-2. format and watch dog reset
    if( RESET_TO_FACTORY != g_FormatOption )
    {
        ret = Format_Internal();
        if ( S_DONE != ret ) return ret;
    }

    //-3. enter meta to upload
    ret = EnterMETAUpload();
    if ( META_SUCCESS != ret ) {
        asMsg = "Format Succeed! Please try the 'Upload Only' option and press 'Download Key'. If the condition is the same, please call for help.\n";
        return ret;
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::UploadOnly ( void )
{
    //-1. enter meta to upload
    int ret = EnterMETAUpload();
    if ( META_SUCCESS != ret ) {
        return ret;
    }

    return ret;

}
//---------------------------------------------------------------------------
int tboot_1::EnableBoot()
{
    assert(g_BootCert_Enabled);

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    FLASHTOOL_ENABLE_BOOT_ARG enable_boot_arg;
    FLASHTOOL_ENABLE_BOOT_RESULT enable_boot_result;

    ArgComPortSetting(com);
    ArgFlashTool(arg, result);
    ArgFlashToolEnableBoot(enable_boot_arg, enable_boot_result);

    return FlashEnableBoot(&com, &arg, &result, &enable_boot_arg,
                           &enable_boot_result, &g_stopflag);
}
//---------------------------------------------------------------------------
int tboot_1::FlashToolAPITest ( void )
{
    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    //- 1. connect
    ArgFlashToolConnect( cn_arg, cn_result );
    int ret = FlashTool_Connect( COM_PORT, &cn_arg, &cn_result, &g_stopflag, &g_ft_handle);
    if( S_DONE != ret ) {
        return ret;
    }

    //- 2. call RepairTool_GetExceptionRecord
    ret = RepairTool_GetExceptionRecord( g_ft_handle, MainForm->ed_ExceptionLog->Text.c_str(), MainForm->ed_SaveExceptionLog->Text.c_str());
    if( S_DONE != ret ) {
        FlashTool_Disconnect( &g_ft_handle );
        return ret;
    }

    //- N. disconnect
    FlashTool_Disconnect( &g_ft_handle );

    return ret;
}
//---------------------------------------------------------------------------
#pragma warn -8066  // Unreachable code
#pragma warn -8070  // Function should return a value
int tboot_1::EnterMETAMode()
{
    if (g_bUsbDLRB)
    {
        META_ConnectByUSB_Req meta_con_req;
        META_ConnectByUSB_Report meta_con_report;

        ArgMETAConnectWithTargetViaUSB(meta_con_req, meta_con_report);

        return META_ConnectWithTargetByUSB(&meta_con_req, &g_stopflag,
                                           &meta_con_report);
    }
    else
    {
        META_Connect_Req meta_con_req;
        META_Connect_Report meta_con_report;

        ArgMETAConnectWithTarget(meta_con_req, meta_con_report);

        return META_ConnectWithTarget(&meta_con_req, &g_stopflag,
                                      &meta_con_report);
    }

    assert(0);
}
#pragma warn .8066
#pragma warn .8070
//---------------------------------------------------------------------------
bool tboot_1::PrepareBackUpDirNumber(AnsiString& BackUpDirNumber,
                                        bool& isPreserveBackupData,
                                        bool& isUseComPortStoreData)
{
    int ret;

    if(isUseComPortStoreData == true)
    {
        // using COM port number
        BackUpDirNumber = IntToStr(COM_PORT);
        return true;
    }

    ret = META_QueryIfFunctionSupportedByTarget(
                                        500, "META_MISC_GetRID");
    BackUpDirNumber = "";
    if (ret != META_SUCCESS)
    {
        // using COM port number
        BackUpDirNumber = IntToStr(COM_PORT);
        isPreserveBackupData = false;
    }
    else
    {
        // using Meta API query RID
        // Get RID using meta API
        char c_RID[50]={NULL}; //
        if(META_SUCCESS != (ret = META_MISC_GetRID(2000, c_RID, 16)) )
        {
            // Target may time out, pop warning messages
            return false;
        }

        if(c_RID[0] == NULL)
        {
            // Platform doesn't support RID
            // using COM port number
            BackUpDirNumber = IntToStr(COM_PORT);
            isPreserveBackupData = false;

            return true;
        }
        for(int i=0;i<50;i++)
        {
            if(c_RID[i] ==  NULL)
                break;
            else
                BackUpDirNumber += c_RID[i] ;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

int tboot_1::EnterMETABackup (bool& isPreserveBackupData,
                              bool& isUseComPortStoreData,
                              AnsiString& BackupCalibrationDataFolderPath)
{
    g_error_code_mode = META;

	// update ResetFakeProgress
	progress_hint           = "Enter Meta to Backup Calibration Data ...";
	progress_forecolor      = (TColor) (0x00DBB0CC);
	Synchronize( ResetFakeProgress );

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    int ret;

	if( META_SUCCESS != ( ret=EnterMETAMode()) ) {
        META_DisconnectWithTarget();
		return ret;
	}

	SyncStartFakeProgress();

    AnsiString BackUpDirNumber;
    bool result = PrepareBackUpDirNumber(BackUpDirNumber,
                                         isPreserveBackupData,
                                         isUseComPortStoreData);
    if(result == false)
    {
        // disconnect
		META_DisconnectWithTarget();
        asMsg = "Failed to get data using META_MISC_GetRID API";
        Synchronize( StopFakeProgress );
        return -1;
    }


    // create backup folder
    int status = CreateBackupDir(BackUpDirNumber, BackupCalibrationDataFolderPath);
    if ( CREATE_FAIL == status) {
        // disconnect
		META_DisconnectWithTarget();
        asMsg = "Failed to create Backup Folder!" + BackupCalibrationDataFolderPath ;
        Synchronize( StopFakeProgress );
        return -1;
    }
    if ( CREATE_DIR_EXIST == status) {
        // disconnect
		META_DisconnectWithTarget();
        asMsg = "There is a  directory with the same RID/COM#:  " + BackupCalibrationDataFolderPath + ".";
        Synchronize( StopFakeProgress );
        return -1;
    }

    // backup calibration data
    if( META_SUCCESS != ( ret = META_QueryIfFunctionSupportedByTarget ( 5000, "META_MISC_GetIMEILocation" )) ) {

        // other error message, return
        if ( META_FUNC_NOT_IMPLEMENT_YET != ret )  {
            // disconnect
		    META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
	        return ret;
        }

        MISC_BACKUP_REQ_T   meta_backup_req;
        ArgMETABackupCalibrationData( BackupCalibrationDataFolderPath, meta_backup_req );
        if( META_SUCCESS != ( ret = META_BasicBackupCalibrationData( &meta_backup_req, &g_stopflag )) ) {
            // disconnect
		    META_DisconnectWithTarget();
            // stop fake progress
		    Synchronize( StopFakeProgress );
            return ret;
        }

    } else {

        // support META_MISC_GetIMEILocation fucntion (patched load)
        // call META backup calibration data api (for patched load)
        MISC_BACKUP_REQ_T   meta_backup_req;
        ArgMETABackupCalibrationData( BackupCalibrationDataFolderPath, meta_backup_req );
        if( META_SUCCESS != ( ret = META_BackupCalibrationData( &meta_backup_req, &g_stopflag )) ) {
            // disconnect
		    META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
	        return ret;
	    }

    }


    // to check if the backup is compelete
    BACKUP_RESULT_T cnf;
    ArgMETAGetBackupResultInfo( cnf );
    AnsiString  folderPath = "";
    folderPath =  BackupCalibrationDataFolderPath + "\\";
    char* fpath =  new  char[ folderPath.Length() +  1 ];
    strcpy ( fpath, folderPath.c_str() );
	if( META_SUCCESS != ( ret =  META_GetBackupResultInfo( fpath , &cnf ) ) ) {
        // disconnect
		META_DisconnectWithTarget();
        // stop fake progress
		Synchronize( StopFakeProgress );
        return ret;
    }
    free ( fpath );


    // call watchdog to reboot the phone
    FtWatchDog  meta_watchdog_req;
    ArgMETAEnableWatchDogTimer( meta_watchdog_req );
    if( META_SUCCESS != ( ret = META_EnableWatchDogTimer( 5000, &meta_watchdog_req) ) ) {
        // disconnect
		META_DisconnectWithTarget();
		// stop fake progress
		Synchronize( StopFakeProgress );
        return ret;
	}

    // disconnect
	META_DisconnectWithTarget();

    // stop fake progress
	Synchronize( FinishFakeProgress );

    return ret;
}
//---------------------------------------------------------------------------
int  tboot_1::EnterMETARestore (bool& isPreserveBackupData,
                                bool& isUseComPortStoreData,
                                AnsiString& BackupCalibrationDataFolderPath)
{
    g_error_code_mode       = META;

    // update ResetFakeProgress
	progress_hint           = "Enter Meta to Restore Calibration Data ...";
	progress_forecolor      = (TColor)(0x00FFCCCC);
	Synchronize( ResetFakeProgress );

    // clear hw status
    MainForm->sb_Main->Panels->Items[5]->Text = "";
    MainForm->m_HWStatus->Panels->Items[0]->Text = "";
    MainForm->m_HWStatus->Panels->Items[1]->Text = "";
    MainForm->m_HWStatus->Panels->Items[2]->Text = "";
    MainForm->m_HWStatus->Panels->Items[3]->Text = "";
  	MainForm->m_HWStatus->Hint = "";

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    int ret;

	if( META_SUCCESS != ( ret = EnterMETAMode()) ) {
        META_DisconnectWithTarget();
		return ret;
	}

    AnsiString BackUpDirNumber;
    if(BackupCalibrationDataFolderPath == "")
    {
        bool result = PrepareBackUpDirNumber(BackUpDirNumber,
                                             isPreserveBackupData,
                                             isUseComPortStoreData);
        if(result == false)
        {
            // disconnect
		    META_DisconnectWithTarget();
            asMsg = "Failed to get data using META_MISC_GetRID API";
            Synchronize( StopFakeProgress );
            return -1;
        }
        BackupCalibrationDataFolderPath = ExtractFilePath( Application->ExeName ) + "backup" + '\\' + BackUpDirNumber;
    }

    SyncStartFakeProgress();

    // check if the backup folder exist
    if ( ! CheckBackupDirExist(BackupCalibrationDataFolderPath) ) {

        asMsg = "Backup folder does not exist. Please select the option 'Backup->Download/Format->Restore' to backup first.";
        // disconnect
        META_DisconnectWithTarget();
		// stop fake progress
		Synchronize( StopFakeProgress );

        return -1;
    }


     // enter meta mode to reset the FAT area to factory default
    if( RESET_TO_FACTORY == g_FormatAfterDownloadOption )
    {
        // call reset to factory default
    	if( META_SUCCESS != ( ret = META_NVRAM_ResetToFactoryDefault( 40000 ) ) ) {
            // disconnect
		    META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
	    	return ret;
	    }
    }


    // backup calibration data
    if( META_SUCCESS != (ret = META_QueryIfFunctionSupportedByTarget ( 5000, "META_MISC_GetIMEILocation" )) ) {

        // other error message, return
        if ( META_FUNC_NOT_IMPLEMENT_YET != ret )  {
            // disconnect
		    META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
	        return ret;
        }

        // not support META_MISC_GetIMEILocation fucntion
        // call basic META restore calibration data api (for non-patched load)
        MISC_RESTORE_REQ_T    meta_restore_req;
        ArgMETARestoreCalibrationData( BackupCalibrationDataFolderPath, meta_restore_req );
        if( META_SUCCESS != ( ret = META_BasicRestoreCalibrationData( &meta_restore_req,  &g_stopflag )) ) {
            // disconnect
            META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
		    return ret;
	    }
    } else {

        // support META_MISC_GetIMEILocation fucntion
        // call META restore calibration data api (for patched load)
        MISC_RESTORE_REQ_T    meta_restore_req;
        ArgMETARestoreCalibrationData( BackupCalibrationDataFolderPath, meta_restore_req );
        if( META_SUCCESS != ( ret = META_RestoreCalibrationData( &meta_restore_req,  &g_stopflag ) ) ) {
            // disconnect
		    META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
	        return ret;
	    }
    }


    // to check if the restore sccuess
    RESTORE_RESULT_T cnf;
    ArgMETAGetRestoreResultInfo( cnf );
    AnsiString  folderpath  = "";
    folderpath = BackupCalibrationDataFolderPath + "\\";
    char* fpath =  new  char[ folderpath.Length() +  1 ];
    strcpy ( fpath, folderpath.c_str() );
	if( META_SUCCESS != ( ret = META_GetRestoreResultInfo ( fpath , &cnf ) ) ) {
        // disconnect
		META_DisconnectWithTarget();
        // stop fake progress
		Synchronize( StopFakeProgress );
        return ret;
    }
    free ( fpath );


	// Preserve calibration data
	if(isPreserveBackupData == false){
      // delete all backup file
      char *path = new  char[ BackupCalibrationDataFolderPath.Length() +  1 ];
      strcpy( path ,  BackupCalibrationDataFolderPath.c_str() );
      if( META_SUCCESS != ( ret = META_DeleteAllFilesInBackupFolder ( path ) ) ) {
          // disconnect
          META_DisconnectWithTarget();
          // stop fake progress
          Synchronize( StopFakeProgress );
          return ret;
      }
      free ( path );
	}

    // disconnect
	META_DisconnectWithTarget();

	if(isPreserveBackupData == false){
        // remove backup folder
        if ( !RemoveBackupDir(BackupCalibrationDataFolderPath) )
        {
            asMsg = "Failed to remove Backup folder!";
            Synchronize( StopFakeProgress );
            // stop fake progress
    	    Synchronize( FinishFakeProgress );
            return -1;
        }
	}

	// stop fake progress
	Synchronize( FinishFakeProgress );

    return ret;

}
//---------------------------------------------------------------------------
int tboot_1::EnterMETAUpload( void )
{
    g_error_code_mode       = META;

    // update ResetFakeProgress
	progress_hint           = "Enter META to Upload Files ...";
	progress_forecolor      = (TColor)(0x008B658B);
	Synchronize( ResetFakeProgress );

    // clear hw status
    MainForm->sb_Main->Panels->Items[5]->Text    = "";
    MainForm->m_HWStatus->Panels->Items[0]->Text = "";
    MainForm->m_HWStatus->Panels->Items[1]->Text = "";
    MainForm->m_HWStatus->Panels->Items[2]->Text = "";
    MainForm->m_HWStatus->Panels->Items[3]->Text = "";
  	MainForm->m_HWStatus->Hint = "";

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    //-1. connect with meta
    int ret;

	if( META_SUCCESS != ( ret=EnterMETAMode()) ) {
        META_DisconnectWithTarget();
		return ret;
	}

    SyncStartFakeProgress();

    //-2. enter meta mode to reset the FAT area to factory default (if required)
    if( RESET_TO_FACTORY == g_FormatAfterDownloadOption )
    {
        // call reset to factory default
    	if( META_SUCCESS != ( ret = META_NVRAM_ResetToFactoryDefault( 40000 ) ) ) {
            // disconnect
		    META_DisconnectWithTarget();
		    // stop fake progress
		    Synchronize( StopFakeProgress );
	    	return ret;
	    }
    }


    //-3. call META upload files api
    MISC_UPLOAD_REQ_EX_T meta_upload_req;
    AnsiString imei_status;

    ArgMETAUploadFilesToTarget(meta_upload_req, &imei_status);

    if( META_SUCCESS != ( ret = META_UploadFilesToTarget_Ex( &meta_upload_req,  &g_stopflag ) ) ) {
        META_DisconnectWithTarget();
        // stop fake progress
        Synchronize( StopFakeProgress );
        return ret;
    }

    // disconnect
	META_DisconnectWithTarget();
	// stop fake progress
	Synchronize( FinishFakeProgress );

    asMsg = imei_status;
    SyncShowMsg();

    return ret;
}
//---------------------------------------------------------------------------
CREATE_DIRECTORY_STATE_E tboot_1::CreateBackupDir(const AnsiString BackUpDirNumber,
                                                    AnsiString& BackupCalibrationDataFolderPath)
{
    // create the backup foloder accroding to the com port number
	AnsiString BackupHomeDirectory = ExtractFilePath( Application->ExeName ) + "backup";

	// make sure "backup" directory has alreay exist or create it!
	if ( !CreateDirectory( BackupHomeDirectory.c_str(), NULL) )
	{
		if(GetLastError() != ERROR_ALREADY_EXISTS )
		{
			//g_BackupCalibrationDataFolderPath = "";
			return CREATE_FAIL;
		}
	}

    BackupCalibrationDataFolderPath = ExtractFilePath( Application->ExeName ) + "backup" + '\\' + BackUpDirNumber;

    // Create COM# or RID directory
    if ( !CreateDirectory( BackupCalibrationDataFolderPath.c_str(), NULL) ) {
        // backup folder has already exist, return OK
        if ( GetLastError() == ERROR_ALREADY_EXISTS  ) return CREATE_DIR_EXIST;

        // ProcessErrorMessage( );
        BackupCalibrationDataFolderPath = "";
        return CREATE_FAIL;
    }

    return CREATE_SUCCESS;
}
//---------------------------------------------------------------------------
bool tboot_1::RemoveBackupDir(const AnsiString BackupCalibrationDataFolderPath)
{
    if ( !RemoveDirectory(  BackupCalibrationDataFolderPath.c_str() )) {
        return false;
    }

    //g_BackupCalibrationDataFolderPath = "";
    return true;
}
//---------------------------------------------------------------------------
bool tboot_1::CheckBackupDirExist(const AnsiString BackupCalibrationDataFolderPath)
{
    // the backup foloder accroding to the com port number
    //AnsiString BackupCalibrationDataFolderPath =  "";
    //BackupCalibrationDataFolderPath =  ExtractFilePath( Application->ExeName ) + "backup" + '\\' + BackUpDirNumber;

    if ( !DirectoryExists(  BackupCalibrationDataFolderPath.c_str() )) {
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
void tboot_1::DebugDA()
{
	int ret;
	HANDLE  hCOM = INVALID_HANDLE_VALUE;
    com_sentry	s( hCOM, COM_PORT );

    if( INVALID_HANDLE_VALUE == hCOM ) {
		// stop timer
		Synchronize(StopTimer);
		asMsg = "Open COM" + IntToStr(COM_PORT) + "failed!";
		Synchronize( ShowMsg );
    	return;
	}

    // fill BOOT_FLASHTOOL_ARG
    BOOT_FLASHTOOL_ARG	                boot_ft;
	BOOT_RESULT			                boot_result;
	ArgBootFlashTool( boot_ft, &boot_result );
   	ret = Boot_FlashTool( hCOM, &boot_ft, &boot_result, &g_stopflag );

	if( S_DONE != ret ) {
        // get error message
        GetErrorMessag( (STATUS_E) ret, result.m_da_report.m_msp_err_code );
		// stop timer
		Synchronize(StopTimer);
		if( BOOT_STOP != g_stopflag ) {
			Synchronize( ShowMsg );
		}
		return;
	}

	COMMTIMEOUTS	timeouts;

	timeouts.ReadIntervalTimeout         = 0;
	timeouts.ReadTotalTimeoutMultiplier  = 1;
	timeouts.ReadTotalTimeoutConstant    = 1000;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant   = 1000;

	if( !::SetCommTimeouts( hCOM, &timeouts )) {
		// stop timer
		Synchronize( StopTimer );
		ret     = GetLastError();
		asMsg   = "SetCommTimeouts(): Error: ret=" + IntToStr(ret);
		Synchronize( ShowMsg );
		return;
	}

	bool bIsUpToDate = false;
	m_RecvString = "";

	while( g_bDebugDA && (BOOT_STOP!=g_stopflag) )
    {
		unsigned char  ch      = 0;
		unsigned long  readlen = 0;
		bool   bResult = ReadFile( hCOM, &ch, 1, &readlen, NULL );

		if( true == bResult && 1 == readlen )
        {
			AnsiString  str;
			m_RecvChar = ch;
			str.printf("%c", ch);
			m_RecvString += str;

			if( 0 == ( m_RecvString.Length()%1024 ) )
            {
				Synchronize( UpdateDebugDAWindow );
				bIsUpToDate = true;
			}
			else {
				bIsUpToDate = false;
			}
    	}
    	else if( !bIsUpToDate ) {
			Synchronize( UpdateDebugDAWindow );
			bIsUpToDate = true;
    	}
	}

	// stop timer
	Synchronize( StopTimer );
	return;
}
//---------------------------------------------------------------------------
void __fastcall tboot_1::UpdateDebugDAWindow()
{
    SCROLLINFO si;

	AnsiString s;
    //s = MainForm->DebugDAWindow->Text + IntToHex(m_RecvChar, 2) + " ";
    //s.printf("%s%c", MainForm->DebugDAWindow->Text.c_str(), m_RecvChar);
   	MainForm->DebugDAWindow->Text = m_RecvString;

    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_RANGE | SIF_POS | SIF_PAGE;
    if( ::GetScrollInfo( MainForm->DebugDAWindow->Handle, SB_VERT, &si ) == TRUE ){
        si.nTrackPos = si.nMax - si.nPage;
        ::SetScrollInfo( MainForm->DebugDAWindow->Handle, SB_VERT, &si, TRUE );
    }
}

//---------------------------------------------------------------------------
void tboot_1::SyncDisplayFlashType()
{
	Synchronize( DisplayFlashType );
}

void __fastcall tboot_1::DisplayFlashType()
{
	ConvertDAReportToString(&result.m_da_report, m_strNorFlashHint, sizeof(m_strNorFlashHint), m_strNandFlashHint, sizeof(m_strNandFlashHint), m_strHWDetectionReport, sizeof(m_strHWDetectionReport));
	MainForm->HWStatusBarUpdate(&result.m_da_report, m_strNorFlashHint, m_strNandFlashHint, m_strHWDetectionReport);
}

//---------------------------------------------------------------------------
static int __stdcall cb_download_da_init(void *usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;


    t->StartTimer();

	t->progress_hint            = "Download DA now...";
    t->progress_forecolor       = clRed;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_download_da(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg)
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_download_flash_init(void *usr_arg)
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_hint            = "Download firmware to flash now...";
	t->progress_forecolor       = clBlue;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_download_flash(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg)
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_download_bloader_init(void *usr_arg)
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_hint            = "Download BootLoader now...";
	t->progress_forecolor       = (TColor) 0x00A00070;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_download_bloader(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg)
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}

//---------------------------------------------------------------------------
static int __stdcall cb_format_report_init(HW_StorageType_E storage_type, unsigned int begin_addr, unsigned int length, void *usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_hint.printf("Format %s flash:  begin_addr(0x%08X), length(0x%08X). ", (HW_STORAGE_NOR==storage_type)?"NOR":"NAND", begin_addr, length);

	t->progress_forecolor      = clGreen;
	t->progress_percentage     = 0;

	t->bUpdateFinishBytes      = false;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_format_report(unsigned char percent, void *usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;

	t->bUpdateFinishBytes       = false;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_readback_flash_init( HW_StorageType_E  storage_type, unsigned int rb_addr, unsigned int rb_length, const char *rb_filepath, void *usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

    AnsiString s;
    if( NULL != rb_filepath ) {
    	s.printf("Readback %s flash to \"%s\", ADDR(0x%08X), LEN(0x%08X). ", (HW_STORAGE_NOR==storage_type)?"NOR":"NAND", rb_filepath, rb_addr, rb_length);
    }
    else {
    	s.printf("Readback %s flash to buffer, ADDR(0x%08X), LEN(0x%08X). ", (HW_STORAGE_NOR==storage_type)?"NOR":"NAND", rb_filepath, rb_addr, rb_length);
	}
	t->progress_hint            = s;
	t->progress_forecolor       = clBlue;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_readback_flash(unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_da_report(const DA_REPORT_T  *p_da_report, void *usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	if( MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked )
    {
		for( unsigned int index = 0; index < g_DL_HANDLE_LIST_nodes.size(); index++ )
        {
			if( g_DL_HANDLE_LIST_nodes[index].m_dl_handle == p_da_report->m_dl_handle )
            {
				t->dl_arg.m_otp_enable          = g_DL_HANDLE_LIST_nodes[index].m_param.m_otp_enable;
				t->dl_arg.m_otp_op              = g_DL_HANDLE_LIST_nodes[index].m_param.m_otp_op;
				t->dl_arg.m_otp.m_addr          = g_DL_HANDLE_LIST_nodes[index].m_param.m_otp.m_addr;
				t->dl_arg.m_otp.m_data          = g_DL_HANDLE_LIST_nodes[index].m_param.m_otp.m_data;
				t->dl_arg.m_otp.m_len           = g_DL_HANDLE_LIST_nodes[index].m_param.m_otp.m_len;
				t->dl_arg.m_ft_cfg_enable       = g_DL_HANDLE_LIST_nodes[index].m_param.m_ft_cfg_enable;
				t->dl_arg.m_ft_cfg.m_security_cfg.m_uint32 = g_DL_HANDLE_LIST_nodes[index].m_param.m_ft_cfg.m_security_cfg.m_uint32;
				t->dl_arg.m_sec_ro_enable       = g_DL_HANDLE_LIST_nodes[index].m_param.m_sec_ro_enable;
				t->dl_arg.m_sec_ro.m_data       = g_DL_HANDLE_LIST_nodes[index].m_param.m_sec_ro.m_data;
				t->dl_arg.m_sec_ro.m_len        = g_DL_HANDLE_LIST_nodes[index].m_param.m_sec_ro.m_len;
		        t->dl_arg.m_cust_para_enable    = g_DL_HANDLE_LIST_nodes[index].m_param.m_cust_para_enable;
				t->dl_arg.m_cust_para.m_data    = g_DL_HANDLE_LIST_nodes[index].m_param.m_cust_para.m_data;
				t->dl_arg.m_cust_para.m_len     = g_DL_HANDLE_LIST_nodes[index].m_param.m_cust_para.m_len;

				break;
			}
		}
	}

    t->result.m_da_report = *p_da_report;
	t->SyncDisplayFlashType();

    return 0;
}
//---------------------------------------------------------------------------
bool PowerOnOxford( HANDLE hCard, void *usr_arg)
{
    tboot_1 *t = (tboot_1*) usr_arg;
    int (*mio_pull)(HANDLE handle, int which, int up);

	HINSTANCE g_hDLL = LoadLibrary("miodll.dll");

	if ( g_hDLL == NULL ) //if it is still NULL
	{
        t->asMsg.printf( "[Error] Loaded Oxford's DLL but failed" );
        t->SyncShowMsg();
		return false;
	}

	mio_pull = (int (*)(HANDLE,int,int))GetProcAddress( g_hDLL, "mio_pull" );

	if (hCard == (HANDLE)-1)
	{
        t->asMsg.printf( "[Error] Called Oxford's mio_open but failed" );
        t->SyncShowMsg();
		return false;
	}

    // Power-off
    mio_pull( hCard, 0, 1 );
    Sleep( 200 );
    mio_pull( hCard, 0, 0 );
    Sleep( 200 );

    // Power-on
    mio_pull(hCard, 1, 1);
	return true;
}
//---------------------------------------------------------------------------
static int __stdcall cb_com_init_stage( HANDLE hCOM, void *usr_arg)
{
    tboot_1 *t = (tboot_1*) usr_arg;

    OxQueryDeviceInfo deviceInfo;
    DWORD bytesReturned;
    BOOL ok;

    deviceInfo.dwRequestedType = OX_FOR_SERIAL_DRIVER;

    ok = DeviceIoControl( hCOM,
                          IOCTL_OXSER_QUERY_INFO,
                          &deviceInfo, sizeof(deviceInfo),
                          &deviceInfo, sizeof(deviceInfo),
                          &bytesReturned,
                          NULL);

    if ( ok )
    {
        if ( bytesReturned >= sizeof(deviceInfo) && deviceInfo.dwMagicNumber == OXMAGICNUMBER )
        {
            // if Oxford Card, then call PowerOnOxford()
            if ( deviceInfo.ChipId == OXCF950_CHIP_ID )
            {
                bool bPoweronOk = PowerOnOxford( hCOM, usr_arg );
                if ( !bPoweronOk )
                {
                    t->asMsg.printf("Failed to power on Oxford");
                    t->SyncShowMsg();
                    return 1;
                };

                return 0;
            }
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
static int __stdcall cb_in_brom_stage( unsigned int brom_handle, HANDLE hCOM, void *usr_arg)
{

	unsigned short	value16;
    BBCHIP_TYPE	    *p_bbchip = (BBCHIP_TYPE *)usr_arg;

	if ( g_bPullDownGPIO17 && NULL!=p_bbchip && MT6218B==*p_bbchip )
    {
		// configure GPIO17 mode to GPIO
		value16 = 0x000C;
		Brom_Write16Cmd(brom_handle, hCOM, (0x80120000+0x0120+0x08)/*GPIO_MODE3_CLR*/, &value16, 1);

		// configure GPIO17 direction as output
		value16 = 0x0002;
		Brom_Write16Cmd(brom_handle, hCOM, (0x80120000+0x0010+0x04)/*GPIO_DIR2_SET*/, &value16, 1);

		// pull down GPIO17 to disable vibrator
		value16 = 0x0002;
		Brom_Write16Cmd(brom_handle, hCOM, (0x80120000+0x00a0+0x08)/*GPIO_DOUT2_CLR*/, &value16, 1);
	}

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_format_statistics( const FormatStatisticsReport_T* p_report, void* usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;
	float			bad_block_ratio;
	float			float_size;
	unsigned int	size_of_bits;
	AnsiString		str_format_size;

	size_of_bits = ( p_report->m_fmt_length<<3 );
	float_size   = size_of_bits;

	if( 0x40000000 <= size_of_bits )
    {
		if( 0 == (size_of_bits%0x40000000) ) {
			str_format_size.printf("%dGb", size_of_bits>>30);
		} else {
			str_format_size.printf("%.2fGb", float_size/0x40000000);
		}
    }
	else if( 0x100000 <= size_of_bits )
    {
		if( 0 == (size_of_bits%0x100000) ) {
			str_format_size.printf("%dMb", size_of_bits>>20);
		}
		else {
			str_format_size.printf("%.2fMb", float_size/0x100000);
		}
	}
	else if( 0x400 <= size_of_bits )
    {
		if( 0 == (size_of_bits%0x400) ) {
			str_format_size.printf("%dKb", size_of_bits>>10);
		}
		else {
			str_format_size.printf("%.2fKb", float_size/0x400);
		}
	}
    else {
		str_format_size.printf("%db", size_of_bits);
	}

	bad_block_ratio = (p_report->m_bad_blocks+p_report->m_err_blocks);
	bad_block_ratio /= p_report->m_total_blocks;
	bad_block_ratio *= 100;

	t->asMsg.printf("Format Range: 0x%08X ~ 0x%08X\n"
					"Format Size: %s\n"
					"\n"
					"Bad Blocks: %lu\n"
					"Error Blocks: %lu\n"
					"Total Blocks: %lu\n"
					"\n"
					"Bad/Error Block Ratio: (%.3f%%)\n"
                    "\n"
                    "Press 'OK' to continue\n"
					,p_report->m_fmt_begin_addr, p_report->m_fmt_begin_addr+p_report->m_fmt_length-1
					,str_format_size.c_str()
					,p_report->m_bad_blocks
					,p_report->m_err_blocks
					,p_report->m_total_blocks
					,bad_block_ratio);

	t->SyncShowMsg();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_post_process_init( void *usr_arg )
{

	tboot_1 *t = (tboot_1 *)usr_arg;

    AnsiString s;
    s.printf("Proceed post-process now...");
	t->progress_hint        = s;
	t->progress_forecolor   = (TColor) 0x00990000;
	t->progress_percentage  = 0;

	t->bUpdateFinishBytes   = false;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_post_process( unsigned char percent, void *usr_arg )
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage  = percent;

	t->bUpdateFinishBytes   = false;
	t->SyncUpdateProgress();

    return 0;
}

//---------------------------------------------------------------------------
static int __stdcall cb_progress_init( void *usr_arg )
{

	tboot_1 *t = (tboot_1 *)usr_arg;

    t->StartTimer();

	t->progress_hint            = "Extract NFB image now...";
    t->progress_forecolor       = (TColor) 0x0088A050;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_progress( unsigned char percent,
                                  unsigned int  sent_bytes,
                                  unsigned int  total_bytes,
                                  void*         usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_security_pre_process_notify ( void *usr_arg )
{
	tboot_1 *t = (tboot_1 *)usr_arg;

    t->asMsg.printf("Proceed 1st time download pre-process ... (it takes about 10~15 seconds)\n\nPlease press OK to close the window!");
	t->SyncShowMsg();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_download_fat_change_notify(void *usr_arg)
{
    tboot_1 *t = (tboot_1 *)usr_arg;

    t->asMsg.printf("FAT region has been changed. Please format FAT before you try to power-on the handset.");
	t->SyncShowMsg();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_update_param_init ( void *usr_arg )
{
	tboot_1 *t = (tboot_1 *)usr_arg;

    AnsiString s;
    s.printf("Update parameters now ...");
	t->progress_hint        = s;
	t->progress_forecolor   = (TColor) 0x008A28E5;
	t->progress_percentage  = 0;

	t->bUpdateFinishBytes   = false;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_update_param( unsigned char percent, void *usr_arg )
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage  = percent;

	t->bUpdateFinishBytes   = false;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_enable_boot_init ( void *usr_arg )
{
	tboot_1 *t = (tboot_1 *)usr_arg;

    AnsiString s;
    s.printf("Enable boot now ...");
	t->progress_hint        = s;
	t->progress_forecolor   = (TColor) 0x008A28E5;
	t->progress_percentage  = 0;

	t->bUpdateFinishBytes   = false;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_enable_boot( unsigned char percent, void *usr_arg )
{
	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage  = percent;

	t->bUpdateFinishBytes   = false;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_security_post_process_notify(void *usr_arg)
{

	tboot_1 *t=(tboot_1 *)usr_arg;

	t->asMsg.printf("Proceed 1st time download post-process ... (it takes about 10~15 seconds)\n\nPlease press OK to continue!");
	t->SyncShowMsg();

    return 0;
}
/******************************************************************************/
/* Parameter Page Modules                                                     */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
int tboot_1::Parameter ( void )
{
    int     ret = S_DONE;
    bool    bParameterUpdate 	= false;
	bool    bGpsFWUpdate		= false;

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

	if( !g_bParameterPage ) {
	    asMsg.printf("Please Setup any parameter first!");
		return -1;
    }

    if( MainForm->cb_otp->Checked    || MainForm->cb_sec_setting->Checked ||
        MainForm->cb_sec_ro->Checked || MainForm->cb_cust->Checked  ){
        bParameterUpdate = true;
    }
    if( MainForm->cb_gps->Checked ){
        bGpsFWUpdate = true;
    }

	if( bParameterUpdate || bGpsFWUpdate )
    {
        // fill COM_PORT_SETTING parameter
        ArgComPortSetting( com );

        // fill FLASHTOOL_ARG
        ArgFlashTool( arg, result );

        // fill FLASHTOOL Parameter
        FLASHTOOL_PARAMETER_ARG     pm_arg;
        FLASHTOOL_PARAMETER_RESULT  pm_result;
        ArgFlashToolParameter( pm_arg, pm_result );

	    ret = FlashParameter( &com, &arg, &result, &pm_arg, &pm_result, &g_stopflag );
		if( S_DONE != ret ) return ret;

        // OTP Readback file save
        if( MainForm->cb_otp->Checked && MainForm->rb_otp_Read->Checked && g_ParamReadbackConfig.m_otp_enable )
        {
		    if( MainForm->SaveFile(g_ParamReadbackConfig.m_otp_filepath.c_str(), pm_arg.m_param.m_otp.m_data,pm_arg.m_param.m_otp.m_len) )
            {
			    asMsg.printf("Failed to open \"%s\"!", g_ParamReadbackConfig.m_otp_filepath.c_str());
				Synchronize(ShowMsg);
            }
        }

        // secure RO setting Readback file save
		if( MainForm->cb_sec_ro->Checked && MainForm->rb_sec_ro_Read->Checked && g_ParamReadbackConfig.m_sec_ro_enable )
        {
		    if( MainForm->SaveFile(g_ParamReadbackConfig.m_sec_ro_filepath.c_str(),pm_arg.m_param.m_sec_ro.m_data,pm_arg.m_param.m_sec_ro.m_len) )
            {
                asMsg.printf("Failed to open \"%s\"!", g_ParamReadbackConfig.m_sec_ro_filepath.c_str());
                Synchronize(ShowMsg);
            }
        }

        // customer Parameter Readback file save
		if( MainForm->cb_cust->Checked && MainForm->rb_cust_Read->Checked && g_ParamReadbackConfig.m_cust_para_enable )
        {
            if( MainForm->SaveFile(g_ParamReadbackConfig.m_cust_para_filepath.c_str(),pm_arg.m_param.m_cust_para.m_data,pm_arg.m_param.m_cust_para.m_len) )
            {
			    asMsg.printf("Failed to open \"%s\"!", g_ParamReadbackConfig.m_cust_para_filepath.c_str());
                Synchronize(ShowMsg);
            }
        }

        // GPS FW Download/Readback (gps_brom.dll)
        if( bGpsFWUpdate )
        {
            if ( g_bUsbDLRB )
            {
                asMsg = " GPS Download/Readback was forbidden by USB!\n Please use the UART cable to Download/Readback GPS ROM. ";
                return -1;
            }

		    if( MainForm->rb_gps_Write->Checked && g_ParamDownloadConfig.m_gps_enable )
            {
                g_error_code_mode = GPS;
		        ret = GPSDownload();
                if( S_DONE != ret ) return ret;
            }
            else if( MainForm->rb_gps_Read->Checked && g_ParamReadbackConfig.m_gps_enable )
            {
                g_error_code_mode = GPS;
		        ret = GPSReadback();
                if( S_DONE != ret ) return ret;
            }
        }
    } // bParameterUpdate || bGpsFWUpdate

    return ret;
}

/******************************************************************************/
/* GPS Modules                                                                */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
static int __stdcall cb_gps_download_da_init( void *usr_arg );
static int __stdcall cb_gps_download_da( unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg );
static int __stdcall cb_gps_download_flash_init( void *usr_arg );
static int __stdcall cb_gps_download_flash( unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg );
static int __stdcall cb_gps_readback_flash_init( const char *rb_filepath, void *usr_arg );
static int __stdcall cb_gps_readback_flash( unsigned char percent, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg );
//---------------------------------------------------------------------------
WRAPPER_RETURN tboot_1::GPSDownload(void)
{

    CBromDllWrapper	    m_wrapper;
    WRAPPER_RETURN      ret;
    AnsiString          strAppDir;
    AnsiString          strLog	= "C:\\BROM_DLL_gps.log";	// default value.
    AnsiString          strDLL;
    AnsiString          strDA;
    AnsiString          strScat;
    AnsiString          strImg;
    CallBackFlashtool   cbFlashtool;
    CallBackDownload    cbDownload;

    // Init path
    strAppDir   = ExtractFilePath(Application->ExeName);
    strDLL      = strAppDir + "gps_brom.dll";
    strDA       = strAppDir + "gps_DA.bin";
    strScat     = strAppDir + "scat.txt";

    // get the  "mt33XX_gps_core.bin" file path
    if ( g_bParameterPage ) {
        strImg   = g_ParamDownloadConfig.m_gps_rom_filepath;
    }
    else {
        strImg   = g_DL_HANDLE_NODE.m_param.m_gps_rom_filepath;
    }


    memset( &cbFlashtool, 0, sizeof(cbFlashtool));
    memset( &cbDownload,  0, sizeof(cbDownload));

    cbFlashtool.cb_download_da_init         = cb_gps_download_da_init;
    cbFlashtool.cb_download_da_init_arg     = this;
    cbFlashtool.cb_download_da              = cb_gps_download_da;
    cbFlashtool.cb_download_da_arg          = this;

    cbDownload.cb_download_flash_init       = cb_gps_download_flash_init;
    cbDownload.cb_download_flash_init_arg   = this;
    cbDownload.cb_download_flash            = cb_gps_download_flash;
    cbDownload.cb_download_flash_arg        = this;

    //- 1. Load BromDLL library
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnLoadBromDLL(strDLL.c_str())) ) {
        if( ERROR_DLL_NOT_LOAD == ret ){
            ShowMessage("[Error] Loaded GPS BROM DLL but failed");
	    	return ERROR_DLL_NOT_LOAD;
        }
        else{
            return ret;
        }
    }

    m_wrapper.fnDebugOn(strLog.c_str());  //-- Debug On

    //- 2. Load DA
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnLoadDA(strDA.c_str())) ){
        return ret;
    }

    //- 3. Load Scatter file and Firmware
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnLoadImageByScatter(strScat.c_str(),strImg.c_str())) ){
        return ret;
    }

    //- 4. Download Firmware
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnDownloadFirmware(COM_PORT, &cbFlashtool, &cbDownload, &g_stopflag)) ){
        return ret;
    }

    m_wrapper.fnDebugOff(); //-- Debug Off

    return COMPLETE_SUCCESS;
}
//---------------------------------------------------------------------------

WRAPPER_RETURN tboot_1::GPSReadback( void )
{

    CBromDllWrapper	    m_wrapper;
    WRAPPER_RETURN      ret;
    AnsiString          strAppDir;
    AnsiString          strLog	= "C:\\BROM_DLL_gps.log";	// default value.
    AnsiString          strDLL;
    AnsiString          strDA;
    AnsiString          strRB;
    unsigned int        szRBAddr;
    unsigned int        szRBLen;
    CallBackFlashtool   cbFlashtool;
    CallBackReadback    cbReadback;

    // Init path
    strAppDir       = ExtractFilePath(Application->ExeName);
    strDLL          = strAppDir + "gps_brom.dll";
    strDA           = strAppDir + "gps_DA.bin";
//    strScat       = strAppDir + "scat.txt";

    if ( g_bParameterPage ) {
        strRB   	= g_ParamReadbackConfig.m_gps_rom_filepath;
        szRBAddr	= 0;
        szRBLen		= g_ParamReadbackConfig.m_gps_rom_filesize;
    }
    else {
        strRB   	= g_DL_HANDLE_NODE.m_param.m_gps_rom_filepath;
        szRBAddr	= 0;
        szRBLen		= g_DL_HANDLE_NODE.m_param.m_gps_rom_filesize;
    }


    memset(&cbFlashtool, 0, sizeof(cbFlashtool));
    memset(&cbReadback,  0, sizeof(cbReadback));

    cbFlashtool.cb_download_da_init         = cb_gps_download_da_init;
    cbFlashtool.cb_download_da_init_arg     = this;
    cbFlashtool.cb_download_da              = cb_gps_download_da;
    cbFlashtool.cb_download_da_arg          = this;

    cbReadback.cb_readback_flash_init       = cb_gps_readback_flash_init;
    cbReadback.cb_readback_flash_init_arg   = this;
    cbReadback.cb_readback_flash            = cb_gps_readback_flash;
    cbReadback.cb_readback_flash_arg        = this;

    //- 1. Load BromDLL library
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnLoadBromDLL(strDLL.c_str())) ) {
        if( ERROR_DLL_NOT_LOAD == ret ){
            ShowMessage("[Error] Loaded GPS BROM DLL but failed");
	    	return ERROR_DLL_NOT_LOAD;
        }
        else{
            return ret;
        }
    }

    m_wrapper.fnDebugOn(strLog.c_str());  //-- Debug On

    //- 2. Load DA
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnLoadDA(strDA.c_str())) ){
        return ret;
    }

    //- 3. Setup RB List
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnSetRBList(strRB.c_str(),szRBAddr,szRBLen)) ){
        return ret;
    }

    //- 4. Readback Firmware
    if( COMPLETE_SUCCESS != (ret = m_wrapper.fnReadbackFirmware(COM_PORT, &cbFlashtool, &cbReadback, &g_stopflag)) ){
        return ret;
    }

    m_wrapper.fnDebugOff(); //-- Debug Off

    return COMPLETE_SUCCESS;
}

//---------------------------------------------------------------------------
static int __stdcall cb_gps_download_da_init( void* usr_arg )
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_hint            = "Download GPS's DA now...";
    t->progress_forecolor       = clSkyBlue;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_gps_download_da( unsigned char  percent,
                                         unsigned int   sent_bytes,
                                         unsigned int   total_bytes,
                                         void* usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_gps_download_flash_init( void* usr_arg )
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_hint            = "Download firmware to GPS's flash now...";
	t->progress_forecolor       = clSkyBlue;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_gps_download_flash( unsigned char   percent,
                                            unsigned int    sent_bytes,
                                            unsigned int    total_bytes,
                                            void* usr_arg)
{

	tboot_1 *t=(tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_gps_readback_flash_init( const char* rb_filepath,
                                                 void* usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

    AnsiString s;

   	s.printf("Readback GPS FW from flash to \"%s\". ", rb_filepath);

	t->progress_hint            = s;
	t->progress_forecolor       = clSkyBlue;
	t->progress_percentage      = 0;
	t->progress_finished_bytes  = 0;

	t->bUpdateFinishBytes       = true;
	t->SyncInitProgress();

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_gps_readback_flash( unsigned char percent,
                                            unsigned int sent_bytes,
                                            unsigned int total_bytes,
                                            void* usr_arg)
{

	tboot_1 *t = (tboot_1 *)usr_arg;

	t->progress_percentage      = percent;
	t->progress_finished_bytes  = sent_bytes;
	t->progress_total_bytes     = total_bytes;

	t->bUpdateFinishBytes       = true;
	t->SyncUpdateProgress();

    return 0;
}

/******************************************************************************/
/* Memory Test Modules                                                        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
static int __stdcall cb_memorytest_init(void *user_arg);
static int __stdcall cb_memorytest_progress(unsigned int current_progress, void *usr_arg);
//---------------------------------------------------------------------------
int tboot_1::MemoryTest()
{
    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    // Reset Memory Test 's Result Memo
    MainForm->m_MT_ResultRichEditor->Text = "";
    MainForm->m_MT_ResultRichEditor->Update();

    //-1. Connect and get the HW information
    ArgFlashToolConnect(cn_arg, cn_result);
    int ret = FlashTool_Connect( COM_PORT, &cn_arg, &cn_result, &g_stopflag, &g_ft_handle);
    if( S_DONE != ret ) {
        FlashTool_Disconnect( &g_ft_handle );
        return ret;
    }

    // Try to enable SRAM
    if ( S_DONE != cn_result.m_da_report.m_ext_ram_ret ) {
        ret = EnableSRAM( &cn_result );
    }

    // if SRAM was not found, try to detect DRAM
    if ( S_DONE != cn_result.m_da_report.m_ext_ram_ret ) {
        ret = EnableDRAM( &cn_result );
    }

    //-2. update the memory detection report
    UpdateMemoryTestDetection ( );


    // FlashTool_MemoryTest_Arg initial setting
    FlashTool_MemoryTest_Arg            mt_arg;
    FlashTool_MemoryTest_Result         mt_result;
    ArgFlashToolMemoryTest( mt_arg , mt_result );

    // RAM test
    if (  MainForm->RAMCheckBox->Checked == true )
    {
        UpdateMemoryTestRichEditor("============\t\t RAM Test\t\t ============\r\n",  clPurple   );

        // if SRAM/DRAM is not dectected
        if ( HW_RAM_UNKNOWN == cn_result.m_da_report.m_ext_ram_type ) {
            UpdateMemoryTestRichEditor("SKIP! External RAM was not detected!\r\n",  clRed   );
        } else {
            // do RAM test
            ret = RAMTest( &mt_arg, &mt_result );
        }
    }

    // NOR Flash test
    if (  MainForm->NORCheckBox->Checked == true )
    {
        UpdateMemoryTestRichEditor("============\t\t NOR Test\t\t ============\r\n",  clPurple   );

        // if NOR Flash is not found
		if( S_DONE != cn_result.m_da_report.m_nor_ret ) {
            UpdateMemoryTestRichEditor("SKIP! NOR Flash was not detected!\r\n",  clRed   );
        } else {
            // do NOR test
            ret = NORTest( &mt_arg, &mt_result );

        }
    }

    // NAND Flash test
    if (  MainForm->NANDCheckBox->Checked == true )
    {
        UpdateMemoryTestRichEditor("============\t\t NAND Test\t\t ============\r\n",  clPurple  );

        if ( S_DONE != cn_result.m_da_report.m_nand_ret ) {
            UpdateMemoryTestRichEditor("SKIP! NAND Flash was not detected!\r\n",  clRed   );
        } else {
            // do NAND test
            ret = NANDTest( &mt_arg, &mt_result );
        }
    }

    // Disconnect
    FlashTool_Disconnect( &g_ft_handle );

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::EnableSRAM(FlashTool_Connect_Result *cn_result)
{
    FlashTool_EnableSRAM_Result sram_result;

    const int ret = FlashTool_EnableSRAM(g_ft_handle, &sram_result);

    if (ret != S_DONE)
    {
        return ret;
    }

    cn_result->m_da_report.m_ext_ram_ret         = sram_result.m_ext_ram_ret;
    cn_result->m_da_report.m_ext_ram_type        = sram_result.m_ext_ram_type;
    cn_result->m_da_report.m_ext_ram_chip_select = sram_result.m_ext_ram_chip_select;
    cn_result->m_da_report.m_ext_ram_size        = sram_result.m_ext_ram_size;

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::EnableDRAM( FlashTool_Connect_Result   *cn_result)
{
    int  ret;
    DRAM_SETTING    dram_cfg;

    //-1. Get the DRAM EMI Setting
    // from the DRAM Setting from UI
    if ( g_EMI_NFI_Setting.m_dram_cfg.m_valid == TRUE ) {
        memcpy( &dram_cfg, &g_EMI_NFI_Setting.m_dram_cfg, sizeof(DRAM_SETTING) );
    }
    else {
        MultiLoadNode*  p_node = g_pCurNode;

        // from the MTK_ROM_INFO field in the ROM
        if( NULL == g_pCurNode ) {
            // Multi-Load
            if( MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked )
            {
                if(NULL == g_pLastNode)
                {
                    ShowMessage("Please choose a load first.\nIf no load exists, please click the right button in the Multi-Load window, then use \"Add a Load\" to add a load first!");
                    return S_DA_SETUP_DRAM_FAIL;
                }

                p_node = g_pLastNode;
            }
            // Single Load
            else
            {
                ShowMessage("Please choose a load first!");
                return S_DA_SETUP_DRAM_FAIL;
            }
        }

        DL_HANDLE_T &dl_handle = p_node->m_dl_handle;
        ret = DL_GetDRAMSetting ( dl_handle, &dram_cfg );
        if ( S_DONE != ret ) return ret;
    }

    //-2. enable DRAM
    FlashTool_EnableDRAM_Result   dram_result;
    ret = FlashTool_EnableDRAM ( g_ft_handle, dram_cfg, &dram_result );
    if ( S_DONE != ret ) return ret;

    //-3. update the connect result
    cn_result->m_da_report.m_ext_ram_ret         = dram_result.m_ext_ram_ret;
    cn_result->m_da_report.m_ext_ram_type        = dram_result.m_ext_ram_type;
    cn_result->m_da_report.m_ext_ram_chip_select = dram_result.m_ext_ram_chip_select;
    cn_result->m_da_report.m_ext_ram_size        = dram_result.m_ext_ram_size;

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::RAMTest( FlashTool_MemoryTest_Arg        *mt_arg,
                      FlashTool_MemoryTest_Result     *mt_result)
{
    int ret = S_DONE;

    // RAM Test 1.1 : Bus Test
    if ( MainForm->cb_MT_RAM_Data_Bus_Test->Checked )
    {
        UpdateMemoryTestRichEditor( "Data Bus Test :\r\n",  clBlack );
        ret = RAMDataBusTest( mt_arg, mt_result );
        if( S_DONE != ret ) {
            UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed );
        } else {
            UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue );
        }
	}

    // RAM Test 1.2 : Addr Test
	if ( MainForm->cb_MT_RAM_Addr_Bus_Test->Checked )
    {
        UpdateMemoryTestRichEditor( "Address Bus Test :\r\n",  clBlack );
        ret = RAMAddrBusTest( mt_arg,  mt_result );
        if( S_DONE != ret ) {
            UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed   );
        } else {
            UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue   );

        }
	 }

     // RAM Test 1.3 : Pattern Test
     if ( MainForm->cb_MT_RAM_Pattern_Test->Checked )
     {
        // define the pattern set for pattern test
        int test_pattern[8] = {0x44332211, 0xA5A5A5A5, 0xA5A5A500,
                               0xA500A500, 0xA5000000, 0x00000000,
                               0xFFFF0000, 0xFFFFFFFF};

        UpdateMemoryTestRichEditor("RAM Pattern Test :\r\nWriting ...",  clBlack   );
        ret = RAMPatternTest( mt_arg,  mt_result, test_pattern, 8 );
        if( S_DONE != ret ) {
            UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed   );
        } else {
            UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue   );

        }
     }

    // RAM Test 1.4 : Increment/Decrement Test
    if ( MainForm->cb_MT_RAM_Inc_Dec_Test->Checked )
    {
        UpdateMemoryTestRichEditor("Increment/Decrement Test :\r\nWriting ...",  clBlack   );
        ret = RAMIncDecTest( mt_arg,  mt_result);
    }

    return ret;
}

//---------------------------------------------------------------------------

int tboot_1::RAMDataBusTest( FlashTool_MemoryTest_Arg        *mt_arg,
                             FlashTool_MemoryTest_Result     *mt_result)
{

    // DRAM or SRAM
    if ( HW_RAM_DRAM == cn_result.m_da_report.m_ext_ram_type ) {
        mt_arg->m_memory_device = HW_MEM_EXT_DRAM;
    } else {
        mt_arg->m_memory_device = HW_MEM_EXT_SRAM;
    }

    // Test method : data bus
	mt_arg->m_test_method = HW_MEM_DATA_BUS_TEST;

    return  FlashTool_MemoryTest( g_ft_handle, mt_arg, mt_result );
}

//---------------------------------------------------------------------------
int tboot_1::RAMAddrBusTest( FlashTool_MemoryTest_Arg        *mt_arg,
                             FlashTool_MemoryTest_Result     *mt_result)
{
    // DRAM or SRAM
    if ( HW_RAM_DRAM == cn_result.m_da_report.m_ext_ram_type ) {
        mt_arg->m_memory_device = HW_MEM_EXT_DRAM;
    } else {
        mt_arg->m_memory_device = HW_MEM_EXT_SRAM;
    }

    // Test method : address bus
    mt_arg->m_test_method = HW_MEM_ADDR_BUS_TEST;

    return FlashTool_MemoryTest( g_ft_handle, mt_arg, mt_result );
}
//---------------------------------------------------------------------------
int tboot_1::RAMPatternTest( FlashTool_MemoryTest_Arg        *mt_arg,
                             FlashTool_MemoryTest_Result     *mt_result,
                             int                             pattern_set[],
                             int                             pattern_size )
{
    for ( int i = 0 ; i < pattern_size ; i++ )
    {
        AnsiString s;
        s.printf("\t0x%08X, ", pattern_set[i]);
        UpdateMemoryTestRichEditor(s,  clBlack);

        const int ret = RAMOnePatternTest( mt_arg, mt_result, pattern_set[i]);

        if (ret != S_DONE)
        {
            return ret;
        }
    }

    return S_DONE;
}

//---------------------------------------------------------------------------
int tboot_1::RAMOnePatternTest( FlashTool_MemoryTest_Arg        *mt_arg,
                                FlashTool_MemoryTest_Result     *mt_result,
                                int                             pattern )
{

    // DRAM or SRAM
    if ( HW_RAM_DRAM == cn_result.m_da_report.m_ext_ram_type ) {
        mt_arg->m_memory_device = HW_MEM_EXT_DRAM;
    } else {
        mt_arg->m_memory_device = HW_MEM_EXT_SRAM;
    }
    // Test method : pattern test
    mt_arg->m_test_method  = HW_MEM_PATTERN_TEST;
    // Start addess
    mt_arg->m_start_addr   = 0;
    // Size
    mt_arg->m_size         = cn_result.m_da_report.m_ext_ram_size;
    // Memory I/O
    mt_arg->m_memory_io    = HW_MEM_IO_32BIT;
    // Assign the Pattern
    mt_arg->m_test_pattern = pattern;

    return FlashTool_MemoryTest( g_ft_handle, mt_arg, mt_result );
}

//---------------------------------------------------------------------------
int tboot_1::RAMIncDecTest( FlashTool_MemoryTest_Arg        *mt_arg,
                            FlashTool_MemoryTest_Result     *mt_result)
{
    // Memory 32 bits I/O increasement/decreasement pattern test
	int ret = RAMIncDecTestIO( mt_arg, mt_result, HW_MEM_IO_32BIT );
	if( S_DONE != ret ) {
	   	UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed   );
    } else {
        UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue   );
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::RAMIncDecTestIO( FlashTool_MemoryTest_Arg        *mt_arg,
                              FlashTool_MemoryTest_Result     *mt_result,
                              HW_MemoryIO_E                   io)
{
    // DRAM or SRAM
    if ( HW_RAM_DRAM == cn_result.m_da_report.m_ext_ram_type ) {
        mt_arg->m_memory_device = HW_MEM_EXT_DRAM;
    } else {
        mt_arg->m_memory_device = HW_MEM_EXT_SRAM;
    }

    // Test method : increment/ decrement test
    mt_arg->m_test_method = HW_MEM_INC_DEC_TEST;
    // Start addess
    mt_arg->m_start_addr  = 0;
    // Size
    mt_arg->m_size        = cn_result.m_da_report.m_ext_ram_size;
    // IO
    mt_arg->m_memory_io   = io;

    return FlashTool_MemoryTest(g_ft_handle, mt_arg, mt_result);
}

//---------------------------------------------------------------------------
int tboot_1::NORTest( FlashTool_MemoryTest_Arg        *mt_arg,
                      FlashTool_MemoryTest_Result     *mt_result)
{
    int ret = S_DONE;

    // Step 2.1 : Address Bus Test
    if ( MainForm->cb_MT_NOR_Addr_Bus_Test->Checked )
    {
        UpdateMemoryTestRichEditor( "Address Bus Test(High Address Pins): ",  clBlack   );
        ret = NORAddrBusTest( mt_arg, mt_result);
    	if( S_DONE != ret ) {
            UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed   );
    	} else {
            UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue   );
        }
    }

    // Step 2.2 : Pattern Test
    if ( MainForm->cb_MT_NOR_Pattern_Test->Checked )
    {
        UpdateMemoryTestRichEditor( "Pattern Test(0x5A5A):\r\n",  clBlack   );
        ret = NORPatternTest( mt_arg, mt_result);
        if( S_DONE != ret ) {
            UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed   );
        } else {
            UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue   );
        }
    }

    return ret;
}
//---------------------------------------------------------------------------
int tboot_1::NORAddrBusTest( FlashTool_MemoryTest_Arg        *mt_arg,
                             FlashTool_MemoryTest_Result     *mt_result)
{
    // memory device : NOR
    mt_arg->m_memory_device = HW_MEM_NOR;

    // Test method
    mt_arg->m_test_method   = HW_MEM_ADDR_BUS_TEST;

    return FlashTool_MemoryTest(g_ft_handle, mt_arg, mt_result);
}
//---------------------------------------------------------------------------
int tboot_1::NORPatternTest( FlashTool_MemoryTest_Arg        *mt_arg,
                             FlashTool_MemoryTest_Result     *mt_result)
{

    // memory device : NOR
    mt_arg->m_memory_device = HW_MEM_NOR;
    // Test method
    mt_arg->m_test_method   = HW_MEM_PATTERN_TEST;
    // Start address
    mt_arg->m_start_addr    = 0;
    // Size
    mt_arg->m_size          = cn_result.m_da_report.m_nor_flash_size;
    // Memory I/O
    mt_arg->m_memory_io     = HW_MEM_IO_16BIT;
    // Test pattern
    mt_arg->m_test_pattern  = 0x5A5A;

    return FlashTool_MemoryTest(g_ft_handle, mt_arg, mt_result);
}

//---------------------------------------------------------------------------
int tboot_1::NANDTest( FlashTool_MemoryTest_Arg        *mt_arg,
                       FlashTool_MemoryTest_Result     *mt_result)
{

    int ret = S_DONE;

    // NAND Flash Test : Pattern Test
    if( MainForm->cb_MT_NAND_Pattern_Test->Checked )
    {
        UpdateMemoryTestRichEditor( "Pattern Test (0x5A):\r\n",  clBlack   );
        ret = NANDPatternTest( mt_arg, mt_result);
        if( S_DONE != ret ) {
            UpdateMemoryTestRichEditor( "FAILED!\r\n",  clRed   );
        } else {
            UpdateMemoryTestRichEditor( "OK!\r\n",  clBlue   );
        }

        if (mt_result->m_num_bad_blocks > 0)
        {
            AnsiString string;
            string.printf("%u bad blocks\r\n", mt_result->m_num_bad_blocks);

            UpdateMemoryTestRichEditor(string, clRed);
        }
    }

    return ret;
}

//---------------------------------------------------------------------------
int tboot_1::NANDPatternTest( FlashTool_MemoryTest_Arg        *mt_arg,
                              FlashTool_MemoryTest_Result     *mt_result)
{

    // memory device : nand
    mt_arg->m_memory_device = HW_MEM_NAND;
    // Test method
    mt_arg->m_test_method   = HW_MEM_PATTERN_TEST;
    // Start address
    mt_arg->m_start_addr    = 0;
    // Size
    mt_arg->m_size          = cn_result.m_da_report.m_nand_flash_size;
    // Memory I/O
    mt_arg->m_memory_io     = HW_MEM_IO_8BIT;
    // Test pattern
    mt_arg->m_test_pattern  = 0x5A;

    return FlashTool_MemoryTest(g_ft_handle, mt_arg, mt_result);
}
//---------------------------------------------------------------------------
void tboot_1::UpdateMemoryTestDetection()
{

    AnsiString s;
    UpdateMemoryTestRichEditor("===============	Memory Detection Report	 ===============\r\n", clHotLight  );

    // Internal SRAM report
    UpdateMemoryTestRichEditor( "Internal RAM:\r", clBlack );
   	switch( cn_result.m_da_report.m_int_sram_ret )
    {
	    case S_DONE:
            s.printf( "\tSize = 0x%08X (%dKB)\r\n",
                      cn_result.m_da_report.m_int_sram_size,
                      cn_result.m_da_report.m_int_sram_size/1024);

            UpdateMemoryTestRichEditor( s, clBlack );
		    break;

	    case S_DA_RAM_FLOARTING:
            UpdateMemoryTestRichEditor( "\tERROR: Internal SRAM data is floating!\r\n", clRed );
		    break;

	    case S_DA_RAM_UNACCESSABLE:
            UpdateMemoryTestRichEditor( "\tERROR: Internal SRAM data is un-accessable!\r\n", clRed );
		    break;

	    case S_DA_RAM_ERROR:
        default:
            UpdateMemoryTestRichEditor( "\tERROR: Internal SRAM was not detected!\r\n", clRed );
		    break;
	}


    // External RAM (SRAM/DRAM) report
    UpdateMemoryTestRichEditor( "External RAM:\r", clBlack );
	switch( cn_result.m_da_report.m_ext_ram_ret )
    {
	    case S_DONE:
            if ( HW_RAM_DRAM == cn_result.m_da_report.m_ext_ram_type) {
                UpdateMemoryTestRichEditor( "\tType = DRAM\r\n", clBlack );
            } else {
                UpdateMemoryTestRichEditor( "\tType = SRAM\r\n", clBlack );
            }

		    if( 0x20000 <= cn_result.m_da_report.m_ext_ram_size ) {
			    s.printf( "\tSize = 0x%08X (%dMB/%dMb)\r\n",
                          cn_result.m_da_report.m_ext_ram_size,
                          cn_result.m_da_report.m_ext_ram_size/1024/1024,
                          cn_result.m_da_report.m_ext_ram_size/1024*8/1024);
		    } else {
			    s.printf( "\tSize = 0x%08X (%dMB/dKb)\r\n",
                          cn_result.m_da_report.m_ext_ram_size,
                          cn_result.m_da_report.m_ext_ram_size/1024,
                          cn_result.m_da_report.m_ext_ram_size/1024*8);
		    }
            UpdateMemoryTestRichEditor( s, clBlack );
		    break;

	    case S_DA_RAM_FLOARTING:
            UpdateMemoryTestRichEditor( "\tERROR: External RAM data is floating!\r\n", clRed );
		    break;

	    case S_DA_RAM_UNACCESSABLE:
            UpdateMemoryTestRichEditor( "\tERROR: External RAM data is un-accessable!\r\n", clRed );
		    break;

	    case S_DA_RAM_ERROR:
	    default:
            UpdateMemoryTestRichEditor( "\tERROR: External RAM was not detected!\r\n", clRed );
		    break;
	}



    // NOR flash report
    UpdateMemoryTestRichEditor( "NOR Flash:\r", clBlack );
    if( S_DONE == cn_result.m_da_report.m_nor_ret )
    {
        s.printf("\tDevice ID = \"%s\" (%d)\r\n\tSize = 0x%08X (%dMB/%dMb)\r\n",
                 GetNorFlashNameByTypeId( cn_result.m_da_report.m_nor_flash_id),
                 cn_result.m_da_report.m_nor_flash_id,
		         cn_result.m_da_report.m_nor_flash_size,
                 cn_result.m_da_report.m_nor_flash_size/1024/1024,
                 cn_result.m_da_report.m_nor_flash_size/1024/1024*8);
        UpdateMemoryTestRichEditor( s, clBlack );
    } else {
        UpdateMemoryTestRichEditor( "\tERROR: NOR Flash was not detected!\r\n", clRed );
    }


    // NAND flash report
    UpdateMemoryTestRichEditor( "NAND Flash:\r", clBlack );
    if( S_DONE == cn_result.m_da_report.m_nand_ret )
    {
        s.printf("\tDevice ID = \"%s\" (%d)\r\n\tSize = 0x%08X (%dMB/%dMb)\r\n",
                 GetNandFlashNameByTypeId( cn_result.m_da_report.m_nand_flash_id),
                 cn_result.m_da_report.m_nand_flash_id,
                 cn_result.m_da_report.m_nand_flash_size,
                 cn_result.m_da_report.m_nand_flash_size/1024/1024,
                 cn_result.m_da_report.m_nand_flash_size/1024/1024*8);
        UpdateMemoryTestRichEditor( s, clBlack );
    } else {
        UpdateMemoryTestRichEditor( "\tERROR: NAND Flash was not detected!\r\n", clRed );
    }

}
//---------------------------------------------------------------------------
void __fastcall tboot_1::UpdateMemoryTestRichEditor(AnsiString s, TColor color )
{
    MainForm->m_MT_ResultRichEditor->SelAttributes->Color = color;
    MainForm->m_MT_ResultRichEditor->Lines->Add( s );
}
//---------------------------------------------------------------------------
static int __stdcall cb_memorytest_init(void *user_arg)
{
    MainForm->m_MT_ResultRichEditor->Lines->Add( "cb line1" );
    MainForm->m_MT_ResultRichEditor->Lines->Insert( MainForm->m_MT_ResultRichEditor->Lines->Count, "cb line2" );

    return 0;
}
//---------------------------------------------------------------------------
static int __stdcall cb_memorytest_progress(unsigned int current_progress, void *usr_arg)
{
    unsigned int line_count;
    AnsiString s, tmp_s;

    FlashTool_MemoryTest_Arg *p_mt_arg = (FlashTool_MemoryTest_Arg *)usr_arg;

    line_count =  MainForm->m_MT_ResultRichEditor->Lines->Count;

    if( p_mt_arg->m_test_method == HW_MEM_DATA_BUS_TEST ){
        s.printf( "[D%d]" , current_progress );

        tmp_s = MainForm->m_MT_ResultRichEditor->Lines->Strings[line_count-1];
        MainForm->m_MT_ResultRichEditor->Lines->Strings[line_count-1] = tmp_s + s;
    }

    else if( p_mt_arg->m_test_method == HW_MEM_ADDR_BUS_TEST ){
        s.printf( "[A%d]" , current_progress );

        tmp_s = MainForm->m_MT_ResultRichEditor->Lines->Strings[line_count-1];
        MainForm->m_MT_ResultRichEditor->Lines->Strings[line_count-1] = tmp_s + s;
    }

    else if( p_mt_arg->m_test_method == HW_MEM_PATTERN_TEST){
        s.printf( "(0x%08X)," , current_progress );

        tmp_s = MainForm->m_MT_ResultRichEditor->Lines->Strings[line_count-1];
        MainForm->m_MT_ResultRichEditor->Lines->Strings[line_count-1] = tmp_s + s;
    }

    return 0;
}



/******************************************************************************/
/* Meta functions                                                             */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
static int __stdcall cb_boot_meta_in_brom_stage(unsigned int brom_handle, HANDLE hCOM, void *usr_arg);
//---------------------------------------------------------------------------

int tboot_1::EnterMETAResetFactoryDefault( void )
{
    // Start timer
  	Synchronize( StartTimer );

    // update ResetFakeProgress
	progress_hint           = "Reset NVRAM to factory default settings ...";
	progress_forecolor      = clGreen;
	Synchronize( ResetFakeProgress );

    if (g_bUsbDLRB)
    {
        // Search for a new USB COM port
        if (!GetDynamicComPort((unsigned short *)&COM_PORT))
        {
            return -1;
        }

        MainForm->sb_Main->Panels->Items[2]->Text = "COM"+IntToStr(COM_PORT);
    }

    int ret;

	if( META_SUCCESS != ( ret=EnterMETAMode() ) ) {
        META_DisconnectWithTarget();
		return ret;
	}

	SyncStartFakeProgress();

    // Call Reset to Factory Default
	if( META_SUCCESS != ( ret=META_NVRAM_ResetToFactoryDefault(40000) ) ) {
		META_DisconnectWithTarget();
		// stop fake progress
		Synchronize( StopFakeProgress );
		return ret;
	}
    //=====================================

	META_DisconnectWithTarget();

	// finish fake progress
	Synchronize( FinishFakeProgress );

	return ret;
}
//---------------------------------------------------------------------------
void tboot_1::SyncStartFakeProgress()
{
   	Synchronize( StartFakeProgress );
}
void __fastcall tboot_1::StartFakeProgress()
{
	MainForm->m_FakeProgressTimer->Enabled = true;
}
void __fastcall tboot_1::ResetFakeProgress()
{
	MainForm->CGauge1->Hint       = progress_hint;
	MainForm->CGauge1->ForeColor  = progress_forecolor;
	MainForm->CGauge1->Progress   = 0;
}
void __fastcall tboot_1::StopFakeProgress()
{
	MainForm->m_FakeProgressTimer->Enabled = false;
}

void __fastcall tboot_1::FinishFakeProgress()
{
	MainForm->CGauge1->Progress             = 100;
	MainForm->m_FakeProgressTimer->Enabled  = false;
}
//---------------------------------------------------------------------------
static int __stdcall cb_boot_meta_in_brom_stage( unsigned int        brom_handle,
                                                 HANDLE              hCOM,
                                                 void                *usr_arg)
{
    return 0;
}


/******************************************************************************/
/* moudles used to fill the FlashTool API's input paresmeters                 */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolConnect( FlashTool_Connect_Arg           &ftConnect,
                                  FlashTool_Connect_Result        &ftConnectResult)
{

    memset( &ftConnect, 0, sizeof(ftConnect));
    memset( &ftConnectResult, 0, sizeof(ftConnectResult));

    // com port timeout setting
	ftConnect.m_com_ms_read_timeout                 = g_COM_Read_Timeout;
	ftConnect.m_com_ms_write_timeout                = g_COM_Write_Timeout;

	// fill BOOT_FLASHTOOL_ARG
	ArgBootFlashTool(ftConnect.m_boot_arg, &(ftConnectResult.m_da_report.m_bbchip_type));

    // security pre-process notify callback
	ftConnect.m_cb_security_pre_process_notify      = cb_security_pre_process_notify;
	ftConnect.m_cb_security_pre_process_notify_arg  = this;

    // chip-select of NOR flash memory
	ftConnect.m_nor_chip_select[0]                  = CS0;
	ftConnect.m_nor_chip_select[1]                  = CS_WITH_DECODER;
	ftConnect.m_nand_chip_select                    = g_bNFI_SelectCS1?CS1:CS0;

    //  NFI access contol register
	ftConnect.m_p_nand_acccon = g_EMI_NFI_Setting.m_bAuto?NULL:&(g_EMI_NFI_Setting.m_nand_acccon);
    ftConnect.m_enable_download_without_battery = MainForm->mi_DownloadWithoutBattery->Checked? _TRUE:_FALSE;

	return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgBootFlashTool( BOOT_FLASHTOOL_ARG          &bft,
                               void                        *m_cb_in_brom_stage_arg)
{


    memset( &bft, 0, sizeof(bft));

    bft.m_bbchip_type                = g_BBCHIP_TYPE;
    bft.m_ext_clock                  = g_EXT_CLOCK;

    bft.m_ms_boot_timeout            = BOOT_INFINITE;
    bft.m_max_start_cmd_retry_count  = g_MaxBromStartCmdRetryCount;

    // Initialization for Edge Moudle
    // This callback function will be invoke after COM port is opened
	// You can do some initialization by using this callback function.
	bft.m_cb_com_init_stage          = cb_com_init_stage;
	bft.m_cb_com_init_stage_arg      = this;

    // This callback function will be invoke after BootROM start cmd is passed.
	// You can issue other BootROM command by brom_handle and hCOM which
    // provides callback arguments,or do whatever you want otherwise.
    bft.m_cb_in_brom_stage           = cb_in_brom_stage;
	bft.m_cb_in_brom_stage_arg       = m_cb_in_brom_stage_arg;

    // speed-up BootROM stage baudrate
    bft.m_speedup_brom_baudrate      = g_SpeedUpBootROMBaudrate?_TRUE:_FALSE;

	// Application's window handle to send WM_BROM_READY_TO_POWER_ON_TGT message
	bft.m_ready_power_on_wnd_handle	 = NULL;
	bft.m_ready_power_on_wparam		 = NULL;
	bft.m_ready_power_on_lparam		 = NULL;

	// serial link authentication arg
	bft.m_auth_handle                = g_AUTH_HANDLE_Loaded ? g_AUTH_HANDLE : NULL;
	bft.m_cb_sla_challenge           = SLA_Challenge;
	bft.m_cb_sla_challenge_arg       = this;
	bft.m_cb_sla_challenge_end       = SLA_Challenge_END;
	bft.m_cb_sla_challenge_end_arg   = this;

    // security certificate
    bft.m_scert_handle = g_SCERT_HANDLE_Loaded ? g_SCERT_HANDLE : NULL;

    // EMI Control for bank0 and bank1
    bft.m_p_bank0_mem_cfg            = g_EMI_Config[0].m_bAuto?NULL:&(g_EMI_Config[0].m_Config);
    bft.m_p_bank1_mem_cfg            = g_EMI_Config[1].m_bAuto?NULL:&(g_EMI_Config[1].m_Config);

    if ( g_bSkipBootRom ) {
    	bft.m_da_handle              = NULL;
    }
    else {
    	bft.m_da_handle              = g_DA_HANDLE;
    }

    // da download progress callack
    bft.m_cb_download_da_init        = cb_download_da_init;
    bft.m_cb_download_da_init_arg    = this;
    bft.m_cb_download_da             = cb_download_da;
    bft.m_cb_download_da_arg         = this;

    // download by USB
    bft.m_usb_enable                 = (_BOOL) g_bUsbDLRB;
    bft.m_usb_com_port_control       = USB_COM_PORT_CONTROL_NO_OPERATION;

    // TODO:: fix memory setting for Multi-load on SV5
    GetExternalMemorySetting(g_pCurNode->m_scat_file.c_str(),
                            &bft.m_external_memory_setting[0]);

	return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgComPortSetting( COM_PORT_SETTING  &comPortSetting )
{
    memset( &comPortSetting, 0 , sizeof (comPortSetting));

    comPortSetting.com.number       = COM_PORT;
    comPortSetting.baudrate         = BAUD_RATE;
    comPortSetting.ms_read_timeout  = g_COM_Read_Timeout;
	comPortSetting.ms_write_timeout = g_COM_Write_Timeout;

	return 0;
}
int tboot_1::ArgComPortSetting_ByhCOM( COM_PORT_SETTING  &comPortSetting )
{
    memset( &comPortSetting, 0 , sizeof (comPortSetting));

    comPortSetting.com.handle       = NULL;
    comPortSetting.baudrate         = BAUD_RATE;
    comPortSetting.ms_read_timeout  = g_COM_Read_Timeout;
	comPortSetting.ms_write_timeout = g_COM_Write_Timeout;

	return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgFlashDownload( void )
{
    // fill COM_PORT_SETTING
    ArgComPortSetting( com );

    // fill FLASHTOOL_ARG
    ArgFlashTool( arg, result );

    // fill FLASHTOOL_DOWNLOAD_ARG
    ArgFlashToolDownload( dl_arg, dl_result );

    return 0;

}
//---------------------------------------------------------------------------
int tboot_1::ArgFlashReadback( void )
{
    // fill COM_PORT_SETTING
    ArgComPortSetting(com);

    // fill FLASHTOOL_ARG
    ArgFlashTool(arg, result);

    // fill FLASHTOOL_READBACK_ARG
    ArgFlashToolReadback( rb_arg, rb_result );

    return 0;
}
//---------------------------------------------------------------------------
// TODO:: remove it
/*
int tboot_1::ArgFlashFormat( void )
{
    // fill COM_PORT_SETTING
    ArgComPortSetting( com );

    // fill FLASHTOOL_ARG
    ArgFlashTool( arg, result );

    // fill FLASHTOOL_FORMAT_ARG
    ArgFlashToolFormat( fmt_arg, fmt_result );

    return 0;
}
*/

int tboot_1::ArgFlashFormatEx( void )
{
    // fill COM_PORT_SETTING
    ArgComPortSetting( com );

    // fill FLASHTOOL_ARG
    ArgFlashTool( arg, result );

    // fill FLASHTOOL_FORMAT_ARG
    ArgFlashToolFormatEx( fmt_arg_ex, fmt_result );

    return 0;
}


//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolDownload( FLASHTOOL_DOWNLOAD_ARG       &dl_arg,
                                   FLASHTOOL_DOWNLOAD_RESULT    &dl_result)
{

    memset( &dl_arg,    0, sizeof(dl_arg));
    memset( &dl_result, 0, sizeof(dl_result));

    if(MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked ){
	    dl_arg.m_dl_handle_list     = g_DL_HANDLE_LIST;
		dl_arg.m_dl_handle          = NULL;
    }
    else{
	    dl_arg.m_dl_handle          = g_DL_HANDLE_NODE.m_dl_handle;
		dl_arg.m_dl_handle_list     = NULL;
    }

    // download progress callback
    dl_arg.m_download_accuracy                   = ACCURACY_AUTO;
    dl_arg.m_cb_download_flash_init              = cb_download_flash_init;
    dl_arg.m_cb_download_flash_init_arg          = this;
    dl_arg.m_cb_download_flash                   = cb_download_flash;
    dl_arg.m_cb_download_flash_arg               = this;
	dl_arg.m_cb_security_post_process_notify     = cb_security_post_process_notify;
	dl_arg.m_cb_security_post_process_notify_arg = this;
	dl_arg.m_cb_download_fat_change_notify     = cb_download_fat_change_notify;
	dl_arg.m_cb_download_fat_change_notify_arg = this;

    // format option
	if( NOR_OPERATION == g_OperationMethod ) {
	    dl_arg.m_format_cfg.m_format_begin_addr = g_ulFormatStartAddr;
	    dl_arg.m_format_cfg.m_format_length     = g_ulFormatLength;
    }
    else {
	    dl_arg.m_format_cfg.m_format_begin_addr = g_ulNandFormatStartAddr;
	    dl_arg.m_format_cfg.m_format_length     = g_ulNandFormatLength;
    }
	dl_arg.m_format_cfg.m_validation = g_FormatValidation?_TRUE:_FALSE;

	switch ( g_FormatAfterDownloadOption )
    {
        case FAT_FORMAT_OPTION:
            // enable auto format
	        dl_arg.m_format_enable                = _TRUE;
		    dl_arg.m_format_cfg.m_auto_format_fat = _TRUE;
			break;

        case MANUAL_FORMAT_OPTION:
            // enable manual format
	        dl_arg.m_format_enable                = _TRUE;
		    dl_arg.m_format_cfg.m_auto_format_fat = _FALSE;
            break;

        case RESET_TO_FACTORY:
        case DISABLE_FORMAT:
        default:
            // disable format
            dl_arg.m_format_enable                = _FALSE;
            dl_arg.m_format_cfg.m_auto_format_fat = _FALSE;
            break;
    }
    dl_arg.m_cb_format_report_init        = cb_format_report_init;
    dl_arg.m_cb_format_report_init_arg    = this;
    dl_arg.m_cb_format_report             = cb_format_report;
    dl_arg.m_cb_format_report_arg         = this;
    dl_arg.m_enable_tgt_res_layout_check  = g_bCheckTgtResourceLayout?_TRUE:_FALSE;
    dl_arg.m_enable_bbchip_ver_check      = g_bCheckECOVer?_TRUE:_FALSE;
    dl_arg.m_downloadstyle_sequential     = g_bDownloadStyleSequential?_TRUE:_FALSE;
    dl_arg.m_cb_format_statistics         = cb_format_statistics;
    dl_arg.m_cb_format_statistics_arg     = this;

    // parameter
    if( !(MainForm->MultiLoad->Checked || MainForm->MultiLoadWithMMAA->Checked)) {
        dl_arg.m_otp_enable         = g_DL_HANDLE_NODE.m_param.m_otp_enable;
	 	dl_arg.m_otp_op             = g_DL_HANDLE_NODE.m_param.m_otp_op;
	 	dl_arg.m_otp.m_addr         = g_DL_HANDLE_NODE.m_param.m_otp.m_addr;
	 	dl_arg.m_otp.m_data         = g_DL_HANDLE_NODE.m_param.m_otp.m_data;
	 	dl_arg.m_otp.m_len          = g_DL_HANDLE_NODE.m_param.m_otp.m_len;
	 	dl_arg.m_ft_cfg_enable      = g_DL_HANDLE_NODE.m_param.m_ft_cfg_enable;
	 	dl_arg.m_ft_cfg.m_security_cfg.m_uint32 = g_DL_HANDLE_NODE.m_param.m_ft_cfg.m_security_cfg.m_uint32;
	 	dl_arg.m_sec_ro_enable      = g_DL_HANDLE_NODE.m_param.m_sec_ro_enable;
	 	dl_arg.m_sec_ro.m_data      = g_DL_HANDLE_NODE.m_param.m_sec_ro.m_data;
	 	dl_arg.m_sec_ro.m_len       = g_DL_HANDLE_NODE.m_param.m_sec_ro.m_len;
        dl_arg.m_cust_para_enable   = g_DL_HANDLE_NODE.m_param.m_cust_para_enable;
		dl_arg.m_cust_para.m_data   = g_DL_HANDLE_NODE.m_param.m_cust_para.m_data;
		dl_arg.m_cust_para.m_len    = g_DL_HANDLE_NODE.m_param.m_cust_para.m_len;
    }

    dl_arg.m_cb_param_process_init		= cb_update_param_init;
    dl_arg.m_cb_param_process_init_arg	= this;
    dl_arg.m_cb_param_process			= cb_update_param;
    dl_arg.m_cb_param_process_arg		= this;

    // post-process callback
    dl_arg.m_cb_post_process_init		= cb_post_process_init;
    dl_arg.m_cb_post_process_init_arg	= this;
    dl_arg.m_cb_post_process			= cb_post_process;
    dl_arg.m_cb_post_process_arg		= this;

    // Boot-Loader download progress callback
    dl_arg.m_cb_download_bloader_init       = cb_download_bloader_init;
    dl_arg.m_cb_download_bloader_init_arg   = this;
    dl_arg.m_cb_download_bloader            = cb_download_bloader;
    dl_arg.m_cb_download_bloader_arg        = this;

    // trigger watch dog timeout after download
    // When GPS download, since DA will enter the relay mode
    // not eanable the watch dog time out
    if ( (!g_DL_HANDLE_NODE.m_param.m_gps_enable ) &&
         (( g_FormatAfterDownloadOption == RESET_TO_FACTORY  )  ||
          ( g_backup_restore_state == BACKUPRESTORE ) ||
          ( g_backup_restore_state == RESTORE ) ||
          ( g_upload_state == UPLOAD ) ))
    {
        // enable watchdog
        dl_arg.m_enable_wdt_timeout      = _TRUE;
        dl_arg.m_ms_wdt_timeout_interval = 5000; // ms
    }
    else {
        // disable watchdog
        dl_arg.m_enable_wdt_timeout      = _FALSE;
        dl_arg.m_ms_wdt_timeout_interval = 0;
    }


    // for GPS project
    dl_arg.m_relay_da_enable           = g_DL_HANDLE_NODE.m_param.m_gps_enable;
    dl_arg.m_gpio_gps.m_power_pin      = g_DL_HANDLE_NODE.m_param.m_gps_power_pin;
    dl_arg.m_gpio_gps.m_reset_pin      = g_DL_HANDLE_NODE.m_param.m_gps_reset_pin;
    dl_arg.m_gpio_gps.m_standby_pin    = g_DL_HANDLE_NODE.m_param.m_gps_standby_pin;
    dl_arg.m_gpio_gps.m_32k_pin        = g_DL_HANDLE_NODE.m_param.m_gps_32k_pin;
    dl_arg.m_enable_reset_to_normal_mode = _FALSE;

    return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolReadback( FLASHTOOL_READBACK_ARG       &rb_arg,
                                   FLASHTOOL_READBACK_RESULT    &rb_result)
{

    memset( &rb_arg,    0, sizeof(rb_arg));
    memset( &rb_result, 0, sizeof(rb_result));


    switch( g_OperationMethod )
    {
	    case NFB_OPERATION:
		    rb_arg.m_storage_type = HW_STORAGE_NAND;
			break;
        case NOR_OPERATION:
		default:
		    rb_arg.m_storage_type = HW_STORAGE_NOR;
    }

    // an object of RB_HANDLE
    rb_arg.m_rb_handle                  = g_RB_HANDLE;
    // the accuracy of the read back progress
    rb_arg.m_readback_accuracy          = ACCURACY_AUTO;
    // readback call back funciton
    rb_arg.m_cb_readback_flash_init     = cb_readback_flash_init;
    rb_arg.m_cb_readback_flash_init_arg = this;
    rb_arg.m_cb_readback_flash          = cb_readback_flash;
    rb_arg.m_cb_readback_flash_arg      = this;

    return 0;
}
//---------------------------------------------------------------------------
/*
int tboot_1::ArgFlashToolFormat( FLASHTOOL_FORMAT_ARG       &fmt_arg,
                                 FLASHTOOL_FORMAT_RESULT    &fmt_result)
{
    memset( &fmt_arg,    0, sizeof(fmt_arg));
    memset( &fmt_result, 0, sizeof(fmt_result));

	switch( g_OperationMethod )
    {
	    case NFB_OPERATION:
		    fmt_arg.m_storage_type                   = HW_STORAGE_NAND;
			fmt_arg.m_format_cfg.m_format_begin_addr = g_ulNandFormatStartAddr;
		    fmt_arg.m_format_cfg.m_format_length     = g_ulNandFormatLength;
			break;

        case NOR_OPERATION:
        default:
			fmt_arg.m_storage_type                   = HW_STORAGE_NOR;
			fmt_arg.m_format_cfg.m_format_begin_addr = g_ulFormatStartAddr;
		    fmt_arg.m_format_cfg.m_format_length     = g_ulFormatLength;
            break;
    }

    fmt_arg.m_cb_format_report_init        = cb_format_report_init;
	fmt_arg.m_cb_format_report_init_arg    = this;
	fmt_arg.m_cb_format_report             = cb_format_report;
	fmt_arg.m_cb_format_report_arg         = this;
	fmt_arg.m_cb_format_statistics         = cb_format_statistics;
	fmt_arg.m_cb_format_statistics_arg     = this;
	fmt_arg.m_erase_flag                   = g_EraseFlag;
	fmt_arg.m_format_cfg.m_validation      = g_FormatValidation?_TRUE:_FALSE;
	fmt_arg.m_format_cfg.m_auto_format_fat = FAT_FORMAT_OPTION == g_FormatOption?_TRUE:_FALSE;


    // trigger watch dog timeout after format
    if ( g_backup_restore_state == BACKUPRESTORE ||
         g_backup_restore_state == RESTORE ||
         g_upload_state == UPLOAD )
    {
        // enable watchdog
        fmt_arg.m_enable_wdt_timeout      = _TRUE;
        fmt_arg.m_ms_wdt_timeout_interval = 5000; // ms
    }
    else {
        // disable watchdog
        fmt_arg.m_enable_wdt_timeout      = _FALSE;
        fmt_arg.m_ms_wdt_timeout_interval = 0;
    }

    return 0;
}
*/

int tboot_1::ArgFlashToolFormatEx( FLASHTOOL_FORMAT_ARG_EX       &fmt_arg_ex,
                                 FLASHTOOL_FORMAT_RESULT    &fmt_result)
{
    memset( &fmt_arg_ex,    0, sizeof(fmt_arg_ex));
    memset( &fmt_result, 0, sizeof(fmt_result));

    switch( g_OperationMethod )
    {
        case NFB_OPERATION:
            fmt_arg_ex.m_format_cfg.storageType = HW_STORAGE_NAND;
            fmt_arg_ex.m_format_cfg.formatAction.uArg.NAND_manual.formatRange.startAddr = g_ulNandFormatStartAddr;
            fmt_arg_ex.m_format_cfg.formatAction.uArg.NAND_manual.formatRange.length= g_ulNandFormatLength;
            fmt_arg_ex.m_format_cfg.formatAction.uArg.NAND_manual.formatFlag = g_EraseFlag;
            break;
        case EMMC_OPERATION:
            // TODO:: need to add format action support on EMMC
            fmt_arg_ex.m_format_cfg.storageType = HW_STORAGE_EMMC;
            //  ...
            break;
        case NOR_OPERATION:
        default:
            fmt_arg_ex.m_format_cfg.storageType = HW_STORAGE_NOR;
            fmt_arg_ex.m_format_cfg.formatAction.uArg.manual.startAddr = g_ulFormatStartAddr;
            fmt_arg_ex.m_format_cfg.formatAction.uArg.manual.length = g_ulFormatLength;
            break;
    }

    fmt_arg_ex.m_cb_format_report_init        = cb_format_report_init;
    fmt_arg_ex.m_cb_format_report_init_arg    = this;
    fmt_arg_ex.m_cb_format_report             = cb_format_report;
    fmt_arg_ex.m_cb_format_report_arg         = this;
    fmt_arg_ex.m_cb_format_statistics         = cb_format_statistics;
    fmt_arg_ex.m_cb_format_statistics_arg     = this;
    fmt_arg_ex.m_format_cfg.validation      = g_FormatValidation?_TRUE:_FALSE;
    FORMAT_ACTION_TYPE_E formatAction;
    switch(g_FormatOption)
    {
        case FAT_FORMAT_OPTION:
            formatAction = FAT_FORMAT;
            break;
        case MANUAL_FORMAT_OPTION:
            formatAction = NORMAL_MANUAL_FORMAT;
            break;
        case EMMC_MANUAL_FORMAT_OPTION:
            formatAction = EMMC_MANUAL_FORMAT;
            break;
        case REGION_FROMAT_OPTION:
            formatAction = REGION_FROMAT;
            break;
        case TOTAL_FORMAT_OPTION:
            formatAction = TOTAL_FORMAT;
            break;
        case DISABLE_FORMAT:
        case RESET_TO_FACTORY:
        default:
            formatAction = NOT_FORMAT;
            break;
    }
    fmt_arg_ex.m_format_cfg.formatAction.type = formatAction;

    // trigger watch dog timeout after format
    if ( g_backup_restore_state == BACKUPRESTORE ||
         g_backup_restore_state == RESTORE ||
         g_upload_state == UPLOAD )
    {
        // enable watchdog
        fmt_arg_ex.m_enable_wdt_timeout      = _TRUE;
        fmt_arg_ex.m_ms_wdt_timeout_interval = 5000; // ms
    }
    else {
        // disable watchdog
        fmt_arg_ex.m_enable_wdt_timeout      = _FALSE;
        fmt_arg_ex.m_ms_wdt_timeout_interval = 0;
    }

    return 0;
}

//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolParameter( FLASHTOOL_PARAMETER_ARG      &pm_arg,
                                    FLASHTOOL_PARAMETER_RESULT   &pm_result)
{

    memset(&pm_arg,    0, sizeof(pm_arg) );
    memset(&pm_result, 0, sizeof(pm_result));

    // DL handle
    pm_arg.m_dl_handle          = g_DL_HANDLE_NODE.m_dl_handle;

    // OTP
    if( MainForm->cb_otp->Checked ){
        if( MainForm->rb_otp_Write->Checked && g_ParamDownloadConfig.m_otp_enable ){
            pm_arg.m_param.m_otp_enable	= g_ParamDownloadConfig.m_otp_enable;
    	    pm_arg.m_param.m_otp_op 	= g_ParamDownloadConfig.m_otp_op;
        	pm_arg.m_param.m_otp.m_addr = g_ParamDownloadConfig.m_otp.m_addr;
        	pm_arg.m_param.m_otp.m_data = g_ParamDownloadConfig.m_otp.m_data;
    	    pm_arg.m_param.m_otp.m_len 	= g_ParamDownloadConfig.m_otp.m_len;
        }
        else if( MainForm->rb_otp_Read->Checked && g_ParamReadbackConfig.m_otp_enable ){
            pm_arg.m_param.m_otp_enable = g_ParamReadbackConfig.m_otp_enable;
    	    pm_arg.m_param.m_otp.m_addr = g_ParamReadbackConfig.m_otp.m_addr;
        	pm_arg.m_param.m_otp.m_data = g_ParamReadbackConfig.m_otp.m_data;
        	pm_arg.m_param.m_otp.m_len 	= g_ParamReadbackConfig.m_otp.m_len;
        }
    }

    // secure RO setting
    if( MainForm->cb_sec_setting->Checked ){
        if( MainForm->rb_sec_setting_Write->Checked && g_ParamDownloadConfig.m_ft_cfg_enable ){
    	    pm_arg.m_param.m_ft_cfg_op 		= PARAM_WRITE;
	       	pm_arg.m_param.m_ft_cfg_enable 	= g_ParamDownloadConfig.m_ft_cfg_enable;
    		pm_arg.m_param.m_ft_cfg.m_security_cfg.m_uint32 = g_ParamDownloadConfig.m_ft_cfg.m_security_cfg.m_uint32;
        }
    }

    // secure RO
    if( MainForm->cb_sec_ro->Checked ){
        if( MainForm->rb_sec_ro_Write->Checked && g_ParamDownloadConfig.m_sec_ro_enable ){
    	    pm_arg.m_param.m_sec_ro_op 		= PARAM_WRITE;
    		pm_arg.m_param.m_sec_ro_enable 	= g_ParamDownloadConfig.m_sec_ro_enable;
        	pm_arg.m_param.m_sec_ro.m_data 	= g_ParamDownloadConfig.m_sec_ro.m_data;
    	    pm_arg.m_param.m_sec_ro.m_len 	= g_ParamDownloadConfig.m_sec_ro.m_len;
        }
        else if( MainForm->rb_sec_ro_Read->Checked && g_ParamReadbackConfig.m_sec_ro_enable ){
    	    pm_arg.m_param.m_sec_ro_op 		= PARAM_READ;
    		pm_arg.m_param.m_sec_ro_enable 	= g_ParamReadbackConfig.m_sec_ro_enable;
    	    pm_arg.m_param.m_sec_ro.m_data 	= g_ParamReadbackConfig.m_sec_ro.m_data;
        	pm_arg.m_param.m_sec_ro.m_len 	= g_ParamReadbackConfig.m_sec_ro.m_len;
        }
    }

    // Customer Parameter
    if( MainForm->cb_cust->Checked ){
        if( MainForm->rb_cust_Write->Checked && g_ParamDownloadConfig.m_cust_para_enable ){
    	    pm_arg.m_param.m_cust_para_op 		= PARAM_WRITE;
            pm_arg.m_param.m_cust_para_enable 	= g_ParamDownloadConfig.m_cust_para_enable;
    	    pm_arg.m_param.m_cust_para.m_data 	= g_ParamDownloadConfig.m_cust_para.m_data;
        	pm_arg.m_param.m_cust_para.m_len 	= g_ParamDownloadConfig.m_cust_para.m_len;
        }
        else if( MainForm->rb_cust_Read->Checked && g_ParamReadbackConfig.m_cust_para_enable ){
    	    pm_arg.m_param.m_cust_para_op 		= PARAM_READ;
            pm_arg.m_param.m_cust_para_enable 	= g_ParamReadbackConfig.m_cust_para_enable;
        	pm_arg.m_param.m_cust_para.m_data 	= g_ParamReadbackConfig.m_cust_para.m_data;
    	    pm_arg.m_param.m_cust_para.m_len 	= g_ParamReadbackConfig.m_cust_para.m_len;
        }
    }

    if( MainForm->cb_gps->Checked ){

		pm_arg.m_relay_da_enable           = g_DL_HANDLE_NODE.m_param.m_gps_enable;
        pm_arg.m_gpio_gps.m_power_pin      = g_DL_HANDLE_NODE.m_param.m_gps_power_pin;
        pm_arg.m_gpio_gps.m_reset_pin      = g_DL_HANDLE_NODE.m_param.m_gps_reset_pin;
        pm_arg.m_gpio_gps.m_standby_pin    = g_DL_HANDLE_NODE.m_param.m_gps_standby_pin;
        pm_arg.m_gpio_gps.m_32k_pin        = g_DL_HANDLE_NODE.m_param.m_gps_32k_pin;

    } else {

		pm_arg.m_relay_da_enable           = _FALSE;
        pm_arg.m_gpio_gps.m_power_pin      = 0xFFFF;
        pm_arg.m_gpio_gps.m_reset_pin      = 0xFFFF;
        pm_arg.m_gpio_gps.m_standby_pin    = 0xFFFF;
        pm_arg.m_gpio_gps.m_32k_pin        = 0xFFFF;

    }

	pm_arg.m_cb_param_process_init		   = cb_update_param_init;
	pm_arg.m_cb_param_process_init_arg	   = this;
	pm_arg.m_cb_param_process			   = cb_update_param;
	pm_arg.m_cb_param_process_arg		   = this;

	pm_arg.m_cb_post_process_init		   = cb_post_process_init;
	pm_arg.m_cb_post_process_init_arg	   = this;
	pm_arg.m_cb_post_process			   = cb_post_process;
	pm_arg.m_cb_post_process_arg		   = this;

    return 0;

}
//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolEnableBoot(
        FLASHTOOL_ENABLE_BOOT_ARG &enable_boot_arg,
        FLASHTOOL_ENABLE_BOOT_RESULT &enable_boot_result)
{
    memset(&enable_boot_arg, 0, sizeof(enable_boot_arg));
    memset(&enable_boot_result, 0, sizeof(enable_boot_result));

    enable_boot_arg.m_boot_cert.m_len = g_BootCert.m_len;
    enable_boot_arg.m_boot_cert.m_data = g_BootCert.m_data;

    enable_boot_arg.m_cb_enable_boot_process_init       = cb_enable_boot_init;
    enable_boot_arg.m_cb_enable_boot_process_init_arg   = this;
    enable_boot_arg.m_cb_enable_boot_process            = cb_enable_boot;
    enable_boot_arg.m_cb_enable_boot_process_arg        = this;

    return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgFlashTool(FLASHTOOL_ARG &arg, FLASHTOOL_RESULT &result)
{
    memset( &arg,    0, sizeof(arg));
    memset( &result, 0, sizeof(result));

    // fill BOOT_FLASHTOOL_ARG
	ArgBootFlashTool( arg.m_boot_arg, &(result.m_da_report.m_bbchip_type) );

	arg.m_cb_da_report                       = cb_da_report;
    arg.m_cb_da_report_arg                   = this;
	arg.m_cb_da_report                       = cb_da_report;
    arg.m_cb_da_report_arg                   = this;
    arg.m_cb_security_pre_process_notify     = cb_security_pre_process_notify;
    arg.m_cb_security_pre_process_notify_arg = this;
	arg.m_baudrate_full_sync_count           = g_Baudrate_FullSync_Count;
	arg.m_nor_chip_select[0]                 = CS0;
    arg.m_nor_chip_select[1]                 = CS_WITH_DECODER;
   	arg.m_nand_chip_select                   = g_bNFI_SelectCS1?CS1:CS0;
    arg.m_p_nand_acccon                      = g_EMI_NFI_Setting.m_bAuto?NULL:&(g_EMI_NFI_Setting.m_nand_acccon);
    arg.m_enable_download_without_battery    = MainForm->mi_DownloadWithoutBattery->Checked? _TRUE:_FALSE;

    // get the DRAM EMI Setting from UI
    if( ext_mem1Form->m_EnableDRAMSetting->Checked ) {
        arg.m_enable_ui_dram_cfg            = _TRUE;
        memcpy(&arg.m_dram_cfg, &g_EMI_NFI_Setting.m_dram_cfg, sizeof(DRAM_SETTING));
    }
    else{
        arg.m_enable_ui_dram_cfg            = _FALSE;
    }

    return 0;
}
//---------------------------------------------------------------------------
/* Phase out
int tboot_1::ArgFlashToolProgress(FlashTool_Progress_Arg  &pgs_arg)
{
    memset(&pgs_arg,  0, sizeof(pgs_arg));

    pgs_arg.m_cb_progress_init      = cb_progress_init;
    pgs_arg.m_cb_progress_init_arg  = this;
    pgs_arg.m_cb_progress           = cb_progress;
    pgs_arg.m_cb_progress_arg       = this;

    return 0;

}
*/
//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolChangeBaudrate(FlashTool_ChangeBaudrate_Arg &cb_arg)
{
    memset(&cb_arg,   0,   sizeof(cb_arg));

    cb_arg.m_baudrate                       = BAUD_RATE;
    cb_arg.m_baudrate_full_sync_count       = g_Baudrate_FullSync_Count;
    cb_arg.m_cb_set_high_speed_baudrate     = NULL;
    cb_arg.m_cb_set_high_speed_baudrate_arg = NULL;

    return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgFlashToolMemoryTest( FlashTool_MemoryTest_Arg    &mt_arg,
                                     FlashTool_MemoryTest_Result &mt_result)
{

    memset(&mt_arg,    0, sizeof(mt_arg));
    memset(&mt_result, 0, sizeof(mt_result));

    mt_arg.m_cb_memorytest_progress         = cb_memorytest_progress;
    mt_arg.m_cb_memorytest_progress_arg     = &mt_arg;
    mt_arg.m_cb_memorytest_progress_init    = NULL;
    mt_arg.m_p_stopflag                     = NULL;

    return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgMETAConnectWithTarget( META_Connect_Req	     &meta_con_req,
                                       META_Connect_Report   &meta_con_report)
{

    static 	META_COMM_BAUDRATE  allow_baudrate[11]={
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


    memset(&meta_con_req,    0, sizeof(meta_con_req));
    memset(&meta_con_report, 0, sizeof(meta_con_report));



    meta_con_req.com_port = COM_PORT;

	memcpy(meta_con_req.baudrate, allow_baudrate, sizeof(meta_con_req.baudrate));

	meta_con_req.flowctrl           = META_SW_FLOWCTRL;
	meta_con_req.ms_connect_timeout = 40000;

    meta_con_req.boot_meta_arg.m_bbchip_type = g_BBCHIP_TYPE;
    meta_con_req.boot_meta_arg.m_ext_clock   = g_EXT_CLOCK;

    meta_con_req.boot_meta_arg.m_ms_boot_timeout           = BOOT_INFINITE;
    meta_con_req.boot_meta_arg.m_max_start_cmd_retry_count = g_MaxBromStartCmdRetryCount;

    // This callback function will be invoke after COM port is opened
	// You can do some initialization by using this callback function.
    meta_con_req.boot_meta_arg.m_cb_com_init_stage     = cb_com_init_stage;
    meta_con_req.boot_meta_arg.m_cb_com_init_stage_arg = this;

    // This callback function will be invoke after BootROM start cmd is passed.
	// You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
	// or do whatever you want otherwise.
	meta_con_req.boot_meta_arg.m_cb_in_brom_stage      = cb_boot_meta_in_brom_stage;
	meta_con_req.boot_meta_arg.m_cb_in_brom_stage_arg  = this;

    // speed-up BootROM stage baudrate
    meta_con_req.boot_meta_arg.m_speedup_brom_baudrate = g_SpeedUpBootROMBaudrate?_TRUE:_FALSE;

	// WM_BROM_READY_TO_POWER_ON_TGT
	meta_con_req.boot_meta_arg.m_ready_power_on_wnd_handle	= NULL;
	meta_con_req.boot_meta_arg.m_ready_power_on_wparam		= NULL;
	meta_con_req.boot_meta_arg.m_ready_power_on_lparam		= NULL;

	// serial link authentication arg
	meta_con_req.boot_meta_arg.m_auth_handle              = g_AUTH_HANDLE_Loaded ? g_AUTH_HANDLE : NULL;
	meta_con_req.boot_meta_arg.m_cb_sla_challenge         = SLA_Challenge;
	meta_con_req.boot_meta_arg.m_cb_sla_challenge_arg     = this;
	meta_con_req.boot_meta_arg.m_cb_sla_challenge_end     = SLA_Challenge_END;
	meta_con_req.boot_meta_arg.m_cb_sla_challenge_end_arg = this;

    // security certificate
    meta_con_req.boot_meta_arg.m_scert_handle = g_SCERT_HANDLE_Loaded ? g_SCERT_HANDLE : NULL;

    meta_con_req.boot_meta_arg.m_usb_enable = _FALSE;

    return 0;

}
//---------------------------------------------------------------------------
int tboot_1::ArgMETAConnectWithTargetViaUSB(
                        META_ConnectByUSB_Req &meta_con_req,
                        META_ConnectByUSB_Report &meta_con_report)
{
    memset(&meta_con_req,    0, sizeof(meta_con_req));
    memset(&meta_con_report, 0, sizeof(meta_con_report));

    meta_con_req.com_port           = COM_PORT;
    meta_con_req.ms_connect_timeout = 40000;

    meta_con_req.boot_meta_arg.m_bbchip_type = g_BBCHIP_TYPE;
    meta_con_req.boot_meta_arg.m_ext_clock   = g_EXT_CLOCK;

    meta_con_req.boot_meta_arg.m_ms_boot_timeout           = BOOT_INFINITE;
    meta_con_req.boot_meta_arg.m_max_start_cmd_retry_count = g_MaxBromStartCmdRetryCount;

    // This callback function will be invoke after COM port is opened
    // You can do some initialization by using this callback function.
    meta_con_req.boot_meta_arg.m_cb_com_init_stage     = cb_com_init_stage;
    meta_con_req.boot_meta_arg.m_cb_com_init_stage_arg = this;

    // This callback function will be invoke after BootROM start cmd is passed.
    // You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
    // or do whatever you want otherwise.
    meta_con_req.boot_meta_arg.m_cb_in_brom_stage      = cb_boot_meta_in_brom_stage;
    meta_con_req.boot_meta_arg.m_cb_in_brom_stage_arg  = this;

    // speed-up BootROM stage baudrate
    meta_con_req.boot_meta_arg.m_speedup_brom_baudrate = g_SpeedUpBootROMBaudrate?_TRUE:_FALSE;

    // WM_BROM_READY_TO_POWER_ON_TGT
    meta_con_req.boot_meta_arg.m_ready_power_on_wnd_handle = NULL;
    meta_con_req.boot_meta_arg.m_ready_power_on_wparam     = NULL;
    meta_con_req.boot_meta_arg.m_ready_power_on_lparam     = NULL;

    // serial link authentication arg
    meta_con_req.boot_meta_arg.m_auth_handle              = g_AUTH_HANDLE_Loaded ? g_AUTH_HANDLE : NULL;
    meta_con_req.boot_meta_arg.m_cb_sla_challenge         = SLA_Challenge;
    meta_con_req.boot_meta_arg.m_cb_sla_challenge_arg     = this;
    meta_con_req.boot_meta_arg.m_cb_sla_challenge_end     = SLA_Challenge_END;
    meta_con_req.boot_meta_arg.m_cb_sla_challenge_end_arg = this;

    // security certificate
    meta_con_req.boot_meta_arg.m_scert_handle = g_SCERT_HANDLE_Loaded ? g_SCERT_HANDLE : NULL;

    meta_con_req.boot_meta_arg.m_usb_enable = _TRUE;

    return 0;
}
//---------------------------------------------------------------------------
static void __stdcall cb_backup_calibration_data_init( unsigned char total, unsigned char cur, void* usr_arg )
{
    AnsiString aTotal = total;
    AnsiString aCur = cur;

    MainForm->sb_Main->Panels->Items[5]->Text = " Total Backup Files : " + aTotal + ", Current Backup File : " + aCur ;

    // reset the output information
    if ( aTotal == aCur ) MainForm->sb_Main->Panels->Items[5]->Text = "";
    return;
}

//---------------------------------------------------------------------------
int tboot_1::ArgMETABackupCalibrationData( const AnsiString BackupCalibrationDataFolderPath, MISC_BACKUP_REQ_T    &meta_backup_req )
{
    memset(&meta_backup_req,    0, sizeof(meta_backup_req));

    AnsiString  iniFilePath = "";
    iniFilePath   = ExtractFilePath( Application->ExeName ) + "BACKUP.INI";
    char *iniPath = new  char[ iniFilePath.Length() +  1 ];
    strcpy( iniPath ,  iniFilePath.c_str() );
    meta_backup_req.m_pIniFilePath     = iniPath;

    char *path = new  char[ BackupCalibrationDataFolderPath.Length() +  1 ];
    strcpy( path ,  BackupCalibrationDataFolderPath.c_str() );
    meta_backup_req.m_pBackupFolderPath = path;

    meta_backup_req.cb_progress         = cb_backup_calibration_data_init;
    meta_backup_req.cb_progress_arg     = this;

    return 0;
}
//---------------------------------------------------------------------------
static void __stdcall cb_restore_calibration_data_init( unsigned char total, unsigned char cur, void* usr_arg )
{
    AnsiString aTotal = total;
    AnsiString aCur = cur;

    MainForm->sb_Main->Panels->Items[5]->Text = " Total Restore Files : " + aTotal + ", Current Restore File : " + aCur ;

    // reset the output information
    if ( aTotal == aCur ) MainForm->sb_Main->Panels->Items[5]->Text = "";

    // clear hw status
    MainForm->m_HWStatus->Panels->Items[0]->Text = "";
    MainForm->m_HWStatus->Panels->Items[1]->Text = "";
    MainForm->m_HWStatus->Panels->Items[2]->Text = "";
    MainForm->m_HWStatus->Panels->Items[3]->Text = "";
  	MainForm->m_HWStatus->Hint = "";

    return;
}

//---------------------------------------------------------------------------
int tboot_1::ArgMETARestoreCalibrationData( const AnsiString BackupCalibrationDataFolderPath,
                                                MISC_RESTORE_REQ_T  &meta_restore_req )
{
    memset(&meta_restore_req,   0, sizeof(meta_restore_req));

    AnsiString  iniFilePath = "";
    iniFilePath  =  ExtractFilePath( Application->ExeName ) + "BACKUP.INI";
    char *iniPath = new  char[ iniFilePath.Length() +  1 ];
    strcpy( iniPath ,  iniFilePath.c_str() );
    meta_restore_req.m_pIniFilePath      = iniPath;

    char *backupPath = new  char[ BackupCalibrationDataFolderPath.Length() +  1 ];
    strcpy( backupPath ,  BackupCalibrationDataFolderPath.c_str() );
    meta_restore_req.m_pBackupFolderPath = backupPath;

    meta_restore_req.cb_progress         = cb_restore_calibration_data_init;
    meta_restore_req.cb_progress_arg     = this;

    return 0;
}
//---------------------------------------------------------------------------
static void __stdcall cb_upload_calibration_data_init( unsigned char total, unsigned char cur, void* usr_arg )
{
    AnsiString aTotal = total;
    AnsiString aCur = cur;

    MainForm->sb_Main->Panels->Items[5]->Text = " Total Upload Files : " + aTotal + ", Current Upload File : " + aCur ;

    // reset the output information
    if ( aTotal == aCur ) MainForm->sb_Main->Panels->Items[5]->Text = "";

    // clear hw status
    MainForm->m_HWStatus->Panels->Items[0]->Text = "";
    MainForm->m_HWStatus->Panels->Items[1]->Text = "";
    MainForm->m_HWStatus->Panels->Items[2]->Text = "";
    MainForm->m_HWStatus->Panels->Items[3]->Text = "";
  	MainForm->m_HWStatus->Hint = "";

    return;
}
//---------------------------------------------------------------------------
static void __stdcall cb_upload_imei(const int imei_num,
                                     const char **errMsg,
                                     const char **imei,
                                     void *usr_arg)
{
    if ((imei_num == 0) || (errMsg == NULL) ||
        (imei == NULL)  || (usr_arg == NULL))
    {
        return;
    }

    AnsiString *status = static_cast<AnsiString *>(usr_arg);

    for (int i=0; i<imei_num ; ++i)
    {
        if ((imei[i] == NULL) || (errMsg[i] == NULL))
        {
            continue;
        }

        (*status) += AnsiString("IMEI") + (i + 1) + ": " + imei[i] +
                     " (" + errMsg[i] + ")\n";
    }
}
//---------------------------------------------------------------------------
int tboot_1::ArgMETAUploadFilesToTarget(MISC_UPLOAD_REQ_EX_T &meta_upload_req,
                                        AnsiString *imei_status)
{
    memset(&meta_upload_req,  0, sizeof(meta_upload_req));

    AnsiString iniFilePath =
            ExtractFilePath( Application->ExeName ) + "BACKUP.INI";
    char *iniPath = new char[iniFilePath.Length() + 1];

    strcpy(iniPath, iniFilePath.c_str());

    meta_upload_req.m_pIniFilePath       = iniPath;
    meta_upload_req.cb_progress          = cb_upload_calibration_data_init;
    meta_upload_req.cb_progress_arg      = this;
    meta_upload_req.cb_imei_progress     = cb_upload_imei;
    meta_upload_req.cb_imei_progress_arg = imei_status;

    return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgMETAEnableWatchDogTimer( FtWatchDog &meta_watchdog_req )
{
    memset(&meta_watchdog_req,    0, sizeof(meta_watchdog_req));

    meta_watchdog_req.ms_timeout_interval = 5000;//ms

    return 0;
}
//---------------------------------------------------------------------------
int tboot_1::ArgMETAGetBackupResultInfo( BACKUP_RESULT_T &cnf )
{
    memset(&cnf,    0,     sizeof(cnf));

    return 0;
}

//---------------------------------------------------------------------------
int tboot_1::ArgMETAGetRestoreResultInfo( RESTORE_RESULT_T &cnf)
{
    memset(&cnf,    0,     sizeof(cnf));

    return 0;
}
/******************************************************************************/
/* Error Code Descrptions                                                     */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
//---------------------------------------------------------------------------
void tboot_1::GetErrorMessag( STATUS_E id, unsigned int m_msp_error_code )
{
    AnsiString prefix="", errMsg ="";

	if( S_SECURITY_ERROR_BEGIN <= id ) {
		prefix.printf(" FLASHTOOL ERROR : %s (%d) , MSP ERROE CODE : 0x%02X. ", StatusToString( id ), id, m_msp_error_code );
	}
	else if ( id == -1 ) {
        prefix.printf(" FLASHTOOL ERROR : FROM UI ");
    }
    else {
		prefix.printf(" FLASHTOOL ERROR : %s ( %d ) ", StatusToString( id ), id);
	}


    switch( id )
    {
//	    case S_STOP: break;
//	    case S_UNDEFINED_ERROR: break;
//	    case S_INVALID_ARGUMENTS: break;
//	    case S_INVALID_BBCHIP_TYPE: break;
//	    case S_INVALID_EXT_CLOCK: break;
//	    case S_GET_DLL_VER_FAIL: break;
//	    case S_INVALID_BUF: break;
//	    case S_BUF_IS_NULL: break;
//	    case S_BUF_LEN_IS_ZERO: break;
//	    case S_BUF_SIZE_TOO_SMALL: break;
//	    case S_NOT_ENOUGH_MEMORY: break;

	    case S_COM_PORT_OPEN_FAIL:
            errMsg.printf("[COM] Failed to open COM %d", COM_PORT);
			break;

	    case S_COM_PORT_SET_TIMEOUT_FAIL:
			errMsg.printf("[COM] Failed to set COM%d timeout!", COM_PORT);
            break;

//	    case S_COM_PORT_SET_STATE_FAIL: break;
//	    case S_COM_PORT_PURGE_FAIL: break;
//	    case S_FILEPATH_NOT_SPECIFIED_YET: break;

	    case S_UNKNOWN_TARGET_BBCHIP:
			errMsg = "[User] Unsupported target baseband chip type!\n\
                     \n\
                     [ACTION]\n\
                     Please upgrade to the latest FlashTool then try again.";
			break;

//	    case S_SKIP_BBCHIP_HW_VER_CHECK: break;

	    case S_UNSUPPORTED_VER_OF_BOOT_ROM:
            errMsg = "[User] Unsupported Boot ROM !\n\
                     \n\
                     [ACTION]\n\
                     Please upgrade to the latest FlashTool then try again.";
            break;

	    case S_UNSUPPORTED_VER_OF_BOOTLOADER:
            errMsg = "[MAUI] Boot Loader Image file contains an unsupported version in its header! Please ask for help.";
            break;

	    case S_UNSUPPORTED_VER_OF_DA:
            errMsg = "[DA] DA binary file contains an unsupported version in its header! Please ask for help.";
            break;

	    case S_UNSUPPORTED_VER_OF_SEC_INFO:
            errMsg = "[MAUI] MAUI binary file contains an unsupported security version! Please ask for help.";
            break;

	    case S_UNSUPPORTED_VER_OF_ROM_INFO:
            errMsg = "[MAUI] MAUI binary file contains an unsupported MAUI version! Please ask for help.";
            break;

	    case S_SEC_INFO_NOT_FOUND:
            errMsg = "[User] Your ROM files doesn't enable secure!\n\
                     Secure-on BB must boot up with secure-on ROM files!\n\
                     \n\
                     [ACTION]\n\
                     Please use a secure-on ROM file then try again.";
            break;

	    case S_ROM_INFO_NOT_FOUND:
            errMsg = "[MAUI] There doesn't exist a header in MAUI file! Please ask for help.";
            break;

	    case S_CUST_PARA_NOT_SUPPORTED:
            errMsg = "[User] The phone doesn't contain Customer Parameter Area!\n\
                     Please check it again.";
            break;

	    case S_CUST_PARA_WRITE_LEN_INCONSISTENT:
            errMsg = "[User] The size of Customer Parameter file is inconsistent with that on the phone!\n\
                     Please check it again.";
            break;

//	    case S_SEC_RO_NOT_SUPPORTED: break;
//	    case S_SEC_RO_WRITE_LEN_INCONSISTENT: break;
//	    case S_ADDR_N_LEN_NOT_32BITS_ALIGNMENT: break;

	    case S_UART_CHKSUM_ERROR:
            errMsg = "[H/W] Read-back SEC_RO or CUST_PARA and its checksum is mismatched \n\
                     with the checksum BROM DLL calculates after receiving through UART! \n\
                     Please check your UART cable is stable or not.";
            break;

//	    case S_NOR_FLASH_BOOT: break;
//	    case S_NAND_FLASH_BOOT: break;

	    case S_UNSUPPORTED_VER_OF_EMI_INFO:
            errMsg = "[MAUI] MAUI binary file contains an EMI setting with an unsupported version! Please ask for help.";
            break;

//=> BOOT ROM error code (2000~2999)
//	    case S_BROM_SET_META_REG_FAIL: break;
//	    case S_BROM_SET_FLASHTOOL_REG_FAIL: break;
//	    case S_BROM_SET_REMAP_REG_FAIL: break;
//	    case S_BROM_SET_EMI_FAIL: break;

	    case S_BROM_DOWNLOAD_DA_FAIL:
			errMsg = "[H/W] Failed to download DA to baseband chip's internal SRAM or external RAM! Possible reasons:\n\
                      1. There's an UART communication problem between FlashTool and BootROM.\n\
                      2. Target might be shutdown unexpectedly.\n\
                      \n\
                      [ACTION]\n\
                      1. Try again.\n\
                      2. Proceed H/W checking in download cable and target side UART path.\n\
                      3. Monitor if power-drop occured in target side.";
			break;

	    case S_BROM_CMD_STARTCMD_FAIL:
			errMsg = "[BROM] Can not pass bootrom start command! Possibly target power up too early.";
			break;

	    case S_BROM_CMD_STARTCMD_TIMEOUT:
			errMsg = "[BROM] BootROM start command timeout!";
			break;

//	    case S_BROM_CMD_JUMP_FAIL:break;
//	    case S_BROM_CMD_WRITE16_MEM_FAIL:break;
//	    case S_BROM_CMD_READ16_MEM_FAIL:break;
//	    case S_BROM_CMD_WRITE16_REG_FAIL:break;
//	    case S_BROM_CMD_READ16_REG_FAIL:break;
//	    case S_BROM_CMD_CHKSUM16_MEM_FAIL:break;
//	    case S_BROM_CMD_WRITE32_MEM_FAIL:break;
//	    case S_BROM_CMD_READ32_MEM_FAIL:break;
//	    case S_BROM_CMD_WRITE32_REG_FAIL:break;
//	    case S_BROM_CMD_READ32_REG_FAIL:break;
//	    case S_BROM_CMD_CHKSUM32_MEM_FAIL:break;
//	    case S_BROM_JUMP_TO_META_MODE_FAIL:break;
//	    case S_BROM_WR16_RD16_MEM_RESULT_DIFF:break;

	    case S_BROM_CHKSUM16_MEM_RESULT_DIFF:
			errMsg = "[H/W] Failed to download DA to baseband chip's internal SRAM!\n\
                     The internal SRAM has the problem that leads to 16 bits checksum error!";
			break;

	    case S_BROM_BBCHIP_HW_VER_INCORRECT:
			errMsg = "[H/W] Target baseband chip is inconsistent with bin files!\n\
                     \
                     [ACTION]\n\
                     Please select the suitable bin files for the target's baseband chip.";
			break;

	    case S_BROM_FAIL_TO_GET_BBCHIP_HW_VER:
			errMsg = "[H/W] Failed to get target baseband chip H/W version!\n\
                     Possible reasons:\n\
                     1. There's an UART communication problem between FlashTool and BootROM.\n\
                     2. Target might be shutdown unexpectedly.\n\
                     \n\
                     [ACTION]\n\
                     1. Try again.\n\
                     2. Proceed H/W checking in download cable and target side UART path.\n\
                     3. Monitor if power-drop occured in target side.";
			break;

	    case S_BROM_AUTOBAUD_FAIL:
			errMsg = "[H/W] Pass start command of BootROM, but failed to multiple BootROM baudrate!\n\
                     Possible reasons:\n\
                     1. There's an UART communication problem between FlashTool and BootROM.\n\
                     2. Target might be shutdown unexpectedly.\n\
                     \n\
                     [ACTION]\n\
                     1. Try again.\n\
                     2. If the problem still exists. Disable \"Options\"->\"Transmission option\"->\"Speed-Up BootROM baudrate\"\n\
                        in advance mode (CTRL+ALT+V) then try again.\n\
                     3. Proceed H/W checking in download cable and target side UART path.\n\
                     4. Monitor if power-drop occured in target side.";
			break;

	    case S_BROM_SPEEDUP_BAUDRATE_FAIL:
			errMsg = "[H/W] Pass start command of BootROM, but failed to multiple BootROM baudrate!\n\
                     Possible reasons:\n\
                     1. There's an UART communication problem between FlashTool and BootROM.\n\
                     2. Target might be shutdown unexpectedly.\n\
                     \n\
                     [ACTION]\n\
                     1. Try again.\n\
                     2. If the problem still exists. Disable \"Options\"->\"Transmission option\"->\"Speed-Up BootROM baudrate\"\n\
                        in advance mode (CTRL+ALT+V) then try again.\n\
                     3. Proceed H/W checking in download cable and target side UART path.\n\
                     4. Monitor if power-drop occured in target side.";
			break;

//	    case S_BROM_LOCK_POWERKEY_FAIL:break;
//	    case S_BROM_WM_APP_MSG_OUT_OF_RANGE:break;
//	    case S_BROM_NOT_SUPPORT_MT6205B:break;

	    case S_BROM_EXCEED_MAX_DATA_BLOCKS:
			errMsg = "[MAUI] The number of Multi-Bin files is more than 10!\n\
                     Please don't build more than 10 binary files.";
			break;

		case S_BROM_DOWNLOAD_EPP_FAIL:
			errMsg = "[EPP] FlashTool environment preparation failed\n";
			break;

//	    case S_BROM_EXTERNAL_SRAM_DETECTION_FAIL:break;
//	    case S_BROM_EXTERNAL_DRAM_DETECTION_FAIL:break;


//=> DA error code (3000~3999)

	    case S_DA_INT_RAM_ERROR:
			errMsg = "[H/W] DA didn't detect internal RAM! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
			break;

	    case S_DA_EXT_RAM_ERROR:
			errMsg = "[H/W] DA didn't detect external RAM! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     2. It maybe a EMI issue.\n\
                     Please ask for help.";
			break;

//	    case S_DA_SETUP_DRAM_FAIL:break;
//	    case S_DA_SETUP_PLL_ERR:break;

	    case S_DA_DRAM_NOT_SUPPORT:
			errMsg = "[H/W] The handset doesn't support DRAM. Please ask for help.";
			break;

	    case S_DA_RAM_FLOARTING:
			errMsg = "[H/W] DA detects that DRAM is floating! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     2. It maybe a EMI issue.\n\
                     Please ask for help.";
			break;

	    case S_DA_RAM_UNACCESSABLE:
            errMsg = "[H/W] DA detects that RAM is un-accessable. It maybe a SMT issue. Please ask for help.";
            break;

	    case S_DA_RAM_ERROR:
            errMsg = "[H/W] DA detects other access error of RAM! Please ask for help.";
            break;

	    case S_DA_DEVICE_NOT_FOUND:
            errMsg = "[H/W] DA didn't detect external SRAM or DRAM on the phone.Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     2. It maybe a EMI issue.\n\
                     3. NOR or NAND hasn't been validated.\n\
                     Please check \"Memory_Validation_List_External.xls\" or ask for help.";
            break;

	    case S_DA_NOR_UNSUPPORTED_DEV_ID:
			errMsg = "[User] Unsupported NOR Flash type!\n\
                     \n\
                     [ACTION]\n\
                     Please upgrade to the latest FlashTool then try again.";
			break;

	    case S_DA_NAND_UNSUPPORTED_DEV_ID:
			errMsg = "[User] Unsupported NAND Flash type!\n\
                     \n\
                     [ACTION]\n\
                     Please upgrade to the latest FlashTool then try again.";
			break;

	    case S_DA_NOR_FLASH_NOT_FOUND:
            errMsg = "[H/W] DA didn't detect NOR Flash on the phone. Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     2. It maybe a EMI issue.\n\
                     3. NOR or NAND hasn't been validated.\n\
                     Please check \"Memory_Validation_List_External.xls\" or ask for help.";
            break;

	    case S_DA_NAND_FLASH_NOT_FOUND:
            errMsg = "[H/W] DA didn't detect NAND Flash on the phone.Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     2. It maybe a NFI issue.\n\
                     3. NOR or NAND hasn't been validated.\n\
                     Please check \"Memory_Validation_List_External.xl\" or ask for help.";
            break;

	    case S_DA_SOC_CHECK_FAIL:
			errMsg = "[User] Failed to run SOC verification!";
			break;

	    case S_DA_NOR_PROGRAM_FAILED:
			errMsg = "[H/W] DA fails to program NOR Flash! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
            break;

	    case S_DA_NOR_ERASE_FAILED:
			errMsg = "[H/W] DA fails to erase NOR Flash! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
            break;

	    case S_DA_NAND_PAGE_PROGRAM_FAILED:
			errMsg = "[H/W] DA fails to page-program NAND Flash! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
            break;

	    case S_DA_NAND_SPARE_PROGRAM_FAILED:
			errMsg = "[H/W] DA fails to spare-program NAND Flash! Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
            break;

//	    case S_DA_NAND_HW_COPYBACK_FAILED:break;
//	    case S_DA_NAND_ERASE_FAILED:break;
//	    case S_DA_TIMEOUT:break;
//	    case S_DA_IN_PROGRESS:break;
//	    case S_DA_SUPERAND_ONLY_SUPPORT_PAGE_READ:break;
//	    case S_DA_SUPERAND_PAGE_PRGRAM_NOT_SUPPORT:break;
//	    case S_DA_SUPERAND_SPARE_PRGRAM_NOT_SUPPORT:break;
//	    case S_DA_SUPERAND_COPYBACK_NOT_SUPPORT:break;
//	    case S_DA_NOR_CMD_SEQUENCE_ERR:break;
//	    case S_DA_NOR_BLOCK_IS_LOCKED:break;
//	    case S_DA_NAND_BLOCK_IS_LOCKED:break;

	    case S_DA_NAND_BLOCK_DATA_UNSTABLE:
			errMsg = "[H/W] Data of the block is unstable after erasing NAND Flash.!\n\
                     Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
            break;

	    case S_DA_NOR_BLOCK_DATA_UNSTABLE:
			errMsg = "[H/W] Data of the block is unstable after erasing NOR Flash.!\n\
                     Possible reasons:\n\
                     1. It maybe a SMT issue.\n\
                     Please ask for help.";
            break;

	    case S_DA_NOR_VPP_RANGE_ERR:
			errMsg = "[H/W] Vpp is out of working range for NOR Flash! Please ask for help.";
            break;

//	    case S_DA_INVALID_BEGIN_ADDR:break;

	    case S_DA_NOR_INVALID_ERASE_BEGIN_ADDR:
			errMsg = "[User] An invalid begging address is given for erase operation on NOR Flash.\n\
                     Please check your erase address again.";
			break;

	    case S_DA_NOR_INVALID_READ_BEGIN_ADDR:
			errMsg = "[User] An invalid begging address is given for read operation on NOR Flash.\n\
                     Please check your read address again.";
			break;

	    case S_DA_NOR_INVALID_PROGRAM_BEGIN_ADDR:
			errMsg = "[User] An invalid begging address is given for program operation on NOR Flash.\n\
                     Please check your program address again.";
			break;

	    case S_DA_INVALID_RANGE:
			errMsg = "[User] Failed to run DA_MEM_CMD command!\n\
                     1. One of the download blocks has invalid range.\n\
                     2. Possibly invalid begin address or end address exceeds target flash size.";
			break;

	    case S_DA_NOR_PROGRAM_AT_ODD_ADDR:
			errMsg = "[User] Failed to run DA_MEM_CMD command!\n\
                     1. One of the download blocks has the ODD begin address!";
			break;

	    case S_DA_NOR_PROGRAM_WITH_ODD_LENGTH:
			errMsg = "[User] Failed to run DA_MEM_CMD command!\n\
                     1. One of the download blocks has the EVEN end address! That is download block is in ODD length.";
			break;

//	    case S_DA_NOR_BUFPGM_NO_SUPPORT:break;
//	    case S_DA_NAND_UNKNOWN_ERR:break;
//	    case S_DA_NAND_BAD_BLOCK:break;
//	    case S_DA_NAND_ECC_1BIT_CORRECT:break;
//	    case S_DA_NAND_ECC_2BITS_ERR:break;
//	    case S_DA_NAND_SPARE_CHKSUM_ERR:break;
//	    case S_DA_NAND_HW_COPYBACK_DATA_INCONSISTENT:break;
//	    case S_DA_NAND_INVALID_PAGE_INDEX:break;
//	    case S_DA_NFI_NOT_SUPPORT:break;
//	    case S_DA_NFI_CS1_NOT_SUPPORT:break;
//	    case S_DA_NFI_16BITS_IO_NOT_SUPPORT:break;

	    case S_DA_NFB_BOOTLOADER_NOT_EXIST:
			errMsg = "[User] BootLoader doesn't exist on NAND flash! Please download BootLoader first!";
			break;

	    case S_DA_NAND_NO_GOOD_BLOCK:
			errMsg = "[H/W] There doesn't exist any good block on NAND Flash!";
			break;

	    case S_DA_BOOTLOADER_IS_TOO_LARGE:
			errMsg = "[MAUI] Boot Loader Image is more than 7 blocks on NAND Flash!\n\nPlease ask for help.";
			break;

//	    case S_DA_SIBLEY_REWRITE_OBJ_MODE_REGION:break;
//	    case S_DA_SIBLEY_WRITE_B_HALF_IN_CTRL_MODE_REGION:break;
//	    case S_DA_SIBLEY_ILLEGAL_CMD:break;
//	    case S_DA_SIBLEY_PROGRAM_AT_THE_SAME_REGIONS:break;
//	    case S_DA_UART_GET_DATA_TIMEOUT:break;
//	    case S_DA_UART_GET_CHKSUM_LSB_TIMEOUT:break;
//	    case S_DA_UART_GET_CHKSUM_MSB_TIMEOUT:break;
//	    case S_DA_UART_DATA_CKSUM_ERROR:break;
//	    case S_DA_UART_RX_BUF_FULL:break;
//	    case S_DA_FLASH_RECOVERY_BUF_NOT_ENOUGH:break;
//	    case S_DA_HANDSET_SEC_INFO_NOT_FOUND:break;
//	    case S_DA_HANDSET_SEC_INFO_MAC_VERIFY_FAIL:break;
//	    case S_DA_HANDSET_ROM_INFO_NOT_FOUND:break;
//	    case S_DA_HANDSET_FAT_INFO_NOT_FOUND:break;
//	    case S_DA_OPERATION_UNSUPPORT_FOR_NFB:break;
//	    case S_DA_BYPASS_POST_PROCESS:break;
//	    case S_DA_NOR_OTP_NOT_SUPPORT:break;
//	    case S_DA_NOR_OTP_EXIST:break;
//	    case S_DA_NOR_OTP_LOCKED:break;
//	    case S_DA_NOR_OTP_GETSIZE_FAIL:break;
//	    case S_DA_NOR_OTP_READ_FAIL:break;
//	    case S_DA_NOR_OTP_PROGRAM_FAIL:break;
//	    case S_DA_NOR_OTP_LOCK_FAIL:break;
//	    case S_DA_NOR_OTP_LOCK_CHECK_STATUS_FAIL:break;

	    case S_DA_BLANK_FLASH:
			errMsg = "[User] There doesn't exist MAUI code and SEC_RO (option) on the flash.\n\
                     Please choose MAUI file and SEC_RO file (option) for downloading.";
			break;

	    case S_DA_CODE_AREA_IS_BLANK:
			errMsg = "[User] There doesn't exist MAUI code on the flash.\n\
                     Please choose MAUI file for downloading.";
			break;

	    case S_DA_SEC_RO_AREA_IS_BLANK:
			errMsg = "[User] There doesn't exist SEC_RO (option) on the flash.\n\
                     Please choose SEC_RO file (option) for downloading.";
			break;

//	    case S_DA_NOR_OTP_UNLOCKED:break;

	    case S_DA_UNSUPPORTED_BBCHIP:
   			errMsg = "[User] BROM DLL is too old to support this baseband chip on the phone!\n\
                     \n\
                     [ACTION]\n\
                     Please upgrade to the latest FlashTool then try again.";
			break;

	    case S_DA_FAT_NOT_EXIST:
   			errMsg = "[User] FAT doesn't exist on the phone!";
			break;

	    case S_DA_EXT_SRAM_NOT_FOUND:
   			errMsg = "[H/W] DA didn't detect external SRAM on the phone! Please ask for help.";
			break;

	    case S_DA_EXT_DRAM_NOT_FOUND:
   			errMsg = "[H/W] DA didn't detect external DRAM on the phone! Please ask for help.";
			break;

	    case S_DA_MT_PIN_LOW:
   			errMsg = "[H/W] DA detects that a pin is always low on the phone while memory test! Please ask for help.";
			break;

	    case S_DA_MT_PIN_HIGH:
   			errMsg = "[H/W] DA detects that a pin is always high on the phone while memory test! Please ask for help.";
			break;

	    case S_DA_MT_PIN_SHORT:
   			errMsg = "[H/W] DA detects that two pin is short on the phone while memory test! Please ask for help.";
			break;

	    case S_DA_MT_BUS_ERROR:
   			errMsg = "[H/W] DA detects bus error on the phone while memory test! Please ask for help.";
			break;

	    case S_DA_MT_ADDR_NOT_2BYTE_ALIGNMENT:
   			errMsg = "[User] Input address is not 2-Byte aligned while memory test!\n\
                     Please ask for help.";
			break;

	    case S_DA_MT_ADDR_NOT_4BYTE_ALIGNMENT:
   			errMsg = "[User] Input address is not 4-Byte aligned while memory test!\n\
                     Please ask for help.";
			break;

	    case S_DA_MT_SIZE_NOT_2BYTE_ALIGNMENT:
   			errMsg = "[User] Input size is not 2-Byte aligned while memory test!\n\
                     Please ask for help.";
			break;

	    case S_DA_MT_SIZE_NOT_4BYTE_ALIGNMENT:
   			errMsg = "[User] Input size is not 4-Byte aligned while memory test!\n\
                     Please ask for help.";
			break;

	    case S_DA_MT_DEDICATED_PATTERN_ERROR:
   			errMsg = "[H/W] DA detects error on the phone while memory test with dedicated pattern! Please ask for help.";
			break;

	    case S_DA_MT_INC_PATTERN_ERROR:
   			errMsg = "[H/W] DA detects error on the phone while memory test with incremental pattern! Please ask for help.";
			break;

	    case S_DA_MT_DEC_PATTERN_ERROR:
   			errMsg = "[H/W] DA detects error on the phone while memory test with decremental pattern! Please ask for help.";
			break;

        case S_DA_NFB_BLOCK_0_IS_BAD:
   			errMsg = "[H/W] DA detects Block 0 of NAND Flash is bad! Please ask for help.";
			break;

	  case S_DA_GEN_DA_VERSION_INFO_TEMP_ILB_FAIL:
			errMsg = "DA version info int temp ILB is not correctl!";
			break;


//=> FlashTool error code (4000~4999)
//	    case S_FT_CALLBACK_DA_REPORT_FAIL:break;

	    case S_FT_DA_NO_RESPONSE:
			errMsg = "DA didn't send response data to FlashTool!";
			break;

	    case S_FT_DA_SYNC_INCORRECT:
			errMsg = "[User] Failed to syn with DA! It is possible to download an illegal DA.";
			break;

	    case S_FT_DA_VERSION_INCORRECT:
			errMsg = "[User] DA version is incorrect!";
			break;

	    case S_FT_DA_INIT_SYNC_ERROR:
			errMsg.printf("Failed to send sync data to DA in initialization stage!");
			break;

//	    case S_FT_GET_DSP_VER_FAIL:break;

	    case S_FT_CHANGE_BAUDRATE_FAIL:
            {
            unsigned int baudrate_tbl[] = {	0, 921600, 460800, 230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 110 };
            errMsg = "Failed to change baudrate to " + IntToStr(baudrate_tbl[com.baudrate]) + ".";
            }
    		break;

	    case S_FT_SET_DOWNLOAD_BLOCK_FAIL:
			errMsg = "Failed to run DA_MEM_CMD command! It is possibly caused by the following reasons.\n\n \
                     1. Download block exceeds target flash size.\n \
                     2. Download block start address is not WORD alignment.\n \
                     3. Download block length is not multiple of WORD.";
			break;

	    case S_FT_DOWNLOAD_FAIL:
			errMsg = "Failed to download bin to flash! Please press CTRL+ALT+T to turn on debug log, then reproduce it again!\n \
                     C:\\BROM_DLL.log would be created and record more detail information.";
			break;

	    case S_FT_READBACK_FAIL:
			errMsg = "Failed to readback data from flash! Please press CTRL+ALT+T to turn on debug log, then reproduce again!\n \
                     C:\\BROM_DLL.log would be created and record more detail information.";
			break;

	    case S_FT_FORMAT_FAIL:
			errMsg = "Failed to format this flash! It is possibly caused by given a wrong format range, please check your format settings again.";
			break;

	    case S_FT_FINISH_CMD_FAIL:
			errMsg = "Failed to run DA_FINISH_CMD command!";
			Synchronize(ShowMsg);
			break;

//	    case S_FT_ENABLE_WATCHDOG_FAIL:break;

	    case S_FT_NFB_DOWNLOAD_BOOTLOADER_FAIL:
			errMsg = "Failed to download BootLoader to NAND flash!";
			break;

	    case S_FT_NFB_DOWNLOAD_CODE_FAIL:
			errMsg = "Failed to download Image(s) to NAND flash!";
			break;

//	    case S_FT_NFB_INVALID_BOOTLOADER_DRAM_SETTING:break;

	    case S_FT_NAND_READADDR_NOT_PAGE_ALIGNMENT:
   			errMsg = "[User] The begging address of read-back operation is not page-aligned on NAND Flash!\n\
                     Please check your read address again.";
			break;

	    case S_FT_NAND_READLEN_NOT_PAGE_ALIGNMENT:
   			errMsg = "[User] The length of read-back operation is not page-aligned on NAND Flash!\n\
                     Please check your read address again.";
			break;

//	    case S_FT_READ_REG16_FAIL:break;
//	    case S_FT_WRITE_REG16_FAIL:break;
//	    case S_FT_CUST_PARA_GET_INFO_FAIL:break;
//	    case S_FT_CUST_PARA_READ_FAIL:break;
//	    case S_FT_CUST_PARA_WRITE_FAIL:break;
//	    case S_FT_INVALID_FTCFG_OPERATION:break;
//	    case S_FT_INVALID_CUST_PARA_OPERATION:break;
//	    case S_FT_INVALID_SEC_RO_OPERATION:break;
//	    case S_FT_INVALID_OTP_OPERATION:break;
//	    case S_FT_POST_PROCESS_FAIL:break;
//	    case S_FT_FTCFG_UPDATE_FAIL:break;
//	    case S_FT_SEC_RO_GET_INFO_FAIL:break;
//	    case S_FT_SEC_RO_READ_FAIL:break;
//	    case S_FT_SEC_RO_WRITE_FAIL:break;
//	    case S_FT_ENABLE_DRAM_FAIL:break;
//	    case S_FT_FS_FINDFIRSTEX_FAIL:break;
//	    case S_FT_FS_FINDNEXTEX_FAIL:break;
//	    case S_FT_FS_FOPEN_FAIL:break;
//	    case S_FT_FS_GETFILESIZE_FAIL:break;
//	    case S_FT_FS_READ_FAIL:break;

	    case S_FT_FS_FILENAME_INVALID:
   			errMsg = "[User] The used filename is valid! Please check if your file exists.";
			break;

	    case S_FT_FS_FILENAME_TOO_LONG:
   			errMsg = "[User] The length of used filename is too long! Please check \n\
                     your filename is valid.";
			break;

//	    case S_FT_FS_ASSERT:break;

	    case S_FT_OTP_ADDR_NOT_WORD_ALIGNMENT:
   			errMsg = "[User] The address given of OTP operation is not word-aligned! \n\
                     Please check your OTP address again.";
			break;

	    case S_FT_OTP_LENGTH_NOT_WORD_ALIGNMENT:
   			errMsg = "[User] The length given of OTP operation is not word-aligned! \n\
                     Please check your OTP length again.";
			break;

	    case S_FT_OTP_INVALID_ADDRESS_RANGE:
   			errMsg = "[User] The address given of OTP operation is out of range! \n\
                     Please check your OTP address and length again.";
			break;

//	    case S_FT_NAND_READ_TO_BUFFER_NOT_SUPPORT:break;
//	    case S_FT_GET_PROJECT_ID_FAIL:break;

	    case S_FT_ENFB_ROM_FILE_SMALL_THAN_HEADER_DESCRIBE:
   			errMsg = "[MAUI] The size of E-NFB Rom file is smaller than that described \n\
                     in header of E-NFB Rom file! Please ask for help.";
			break;

		case S_FT_COMBO_MEMORY_SETTING_NOT_MATCHED:
   			errMsg = "[ComboMemory] EMI Setting or Flash IDs information error\n\
                     Reasion: \n\
                     1. The Config file's EMI setting or flash ID are not match with MAUI bin\n\
                     2. The flash ID of MAUI bin is not match to target phone's flash\n\
                     Please check MAUI load is builded correctly";
			break;

		case S_FT_INVALID_FORMAT:
   			errMsg = "[MAUI] Parsing GFH info from MAUI bin failed \n\
                     Reasion: \n\
                     GFH structure is incorrect \n\
                     Please check MAUI load is builded correctly";
			break;

//	    case S_FT_RW_EXTRACT_NFB_FAIL:break;
//	    case S_FT_MEMORY_TEST_FAIL:break;

//=> FlashTool Handle error code (5000~5999)
	    case S_AUTH_HANDLE_IS_NOT_READY:
   			errMsg = "[User] This is a secure Base-band Chip! Please select the authentication file first.";
			break;

	    case S_INVALID_AUTH_FILE:
   			errMsg = "[User] This is an invalid authentication file with wrong format! Please ask for help.";
			break;

	    case S_INVALID_DA_FILE:
   			errMsg = "[User] This is an invalid DA file with wrong format! Please ask for help.";
			break;

//	    case S_DA_HANDLE_IS_NOT_READY:break;
//	    case S_FTHND_ILLEGAL_INDEX:break;
//	    case S_FTHND_HANDLE_BUSY_NOW:break;
//	    case S_FTHND_FILE_IS_UPDATED:break;

	    case S_FTHND_FILE_IS_NOT_LOADED_YET:
            errMsg = "There is file not loaded yet!\n\
                     1. Please check if the DA path is correct.\n\
                     2. Please check if all the ROM files exist.\n\
                     3. Please check if the scatter file description is sync with the exist ROM files.";
			break;

//	    case S_FTHND_FILE_LOAD_FAIL:break;
//	    case S_FTHND_FILE_UNLOAD_FAIL:break;

	    case S_FTHND_LIST_IS_EMPTY:
            errMsg = "Download or Readback list is empty or all disabled!\n\
                     Please select at least one entry.";
			break;

	    case S_DL_SCAT_INCORRECT_FORMAT:
            errMsg = "[MAUI] This scatter file has incorrect format! Please ask for help.";
            break;

	    case S_DL_SCAT_ADDR_IS_NOT_WORD_ALIGN:
            errMsg = "[MAUI] The address of some Load Region in this scatter file is\n\
                     not word-aligned! Please ask for help.";
            break;

	    case S_DL_SCAT_OFFSET_IS_NOT_WORD_ALIGN:
            errMsg = "[MAUI] The offset of some Load Region in this scatter file is\n\
                     not word-aligned! Please ask for help.";
            break;

	    case S_DL_SCAT_ADDR_IS_NOT_ASCENDING_ORDER:
            errMsg = "[MAUI] The addresses of all Load Region in this scatter file\n\
                     are not ascending in order! Please ask for help.";
            break;

	    case S_DL_SCAT_JUMPTABLE_IS_NOT_ABSOLUTE_ADDR:
            errMsg = "[MAUI] The address of jump table binary file in this scatter file\n\
                     is not absolute address! Please ask for help.";
            break;

	    case S_DL_LOAD_REGION_IS_OVERLAP:
			errMsg = "[MAUI] the addresses of some Load Regions in this scatter file\n\
                     are overlapped! Please ask for help.";
			break;

//	    case S_DL_LOAD_REGION_NOT_FOUND:break;
//	    case S_DL_NOT_RESOURCE_BIN:break;
//	    case S_DL_MULTIBIN_MECHANISM_DISABLED:break;

	    case S_DL_RESOURCE_NOT_MATCH_IN_JUMPTABLE:
			errMsg = "[DL_HANDLE] a bin file doesn't match any entry in jump table!";
			break;

	    case S_DL_RESOURCE_MUST_DOWNLOAD_WITH_JUMPTABLE:
			errMsg = "[User] Resources and jump table are not selected to download \n\
                     together! Please select all resource you wanna download";
			break;

//	    case S_DL_OVERLAP_WITH_EXISTING_RESOURCE:break;
//	    case S_DL_INVALID_RESOURCE_BIN:break;

	    case S_DL_JUMPTABLE_INCONSISTENT_WITH_SCAT:
			errMsg = "[DL_HANDLE] jump table is different from scatter!";
			break;

	    case S_DL_INVALID_JUMPTABLE:
			errMsg = "[MAUI] Jump table is not loaded yet!\n\
                     Because its content is not a correct jump table format.";
			break;

//	    case S_DL_REGION_ADDR_INCONSISTENT_WITH_SCAT:break;

	    case S_DL_REGION_ADDR_INCONSISTENT_WITH_RESOURCE_ADDR:
			errMsg = "[MAUI] The address of resource binary file in this scatter file is \n\
                     inconsistent with the address in the header of resource binary \n\
                     file! Please ask for help.";
			break;

	    case S_DL_INVALID_BOOTLOADER:
			errMsg = "[MAUI] The Boot Loader Image file has invalid information in \n\
                     its header! Please ask for help.";
			break;

//	    case S_DL_BOOTLOADER_IS_NOT_LOADED_YET:break;
//	    case S_DL_BOOTLOADER_NOT_FOUND:break;

	    case S_DL_REMOTE_FILE_UNSUPPORTED_BY_BL_AUTOLOAD:
			errMsg = "[User] Boot Loader Image file is in remote server!\n\
                     Please use a local Boot Loader Image file in your computer.";
			break;

//	    case S_DLIST_SAME_BBCHIP_SW_VER:break;

	    case S_DLIST_BBCHIP_HW_VER_NOT_MATCHED:
			errMsg = "[H/W] Target baseband chip is inconsistent with bin files!\n\
                     \n\
                     [ACTION]\n\
                     Please select the suitable bin files for the target's baseband chip.";
			break;

//	    case S_DLIST_NO_MATCHED_DL_HANDLE_FOUND:break;
//	    case S_DLIST_DL_HANDLE_NOT_IN_LIST:break;
//	    case S_DLIST_DL_HANDLE_ALREADY_IN_LIST:break;
//	    case S_FTHND_CALLBACK_REMOTE_GET_FILE_LEN_FAIL:break;
//	    case S_FTHND_CALLBACK_REMOTE_READ_FILE_FAIL:break;
//	    case S_FTHND_CALLBACK_FILE_INTEGRITY_CHECK_FAIL:break;

	    case S_UNSUPPORTED_VER_OF_AUTH_FILE:
			errMsg = "[User] The authentication file contains an unsupported version!\n\
                     Please check the FlashTool is the latest version, or ask for help.";
			break;

	    case S_DL_PROJECT_ID_DIFF_BETWEEN_MAIN_CODE_AND_JUMP_TBL:
			errMsg = "[MAUI] The project IDs between MAUI code and jump table binary file are different!\n\
                     Please check the MAUI and jump table are in the same project.";
			break;

	    case S_DL_SCAT_OPEN_FAIL:
			errMsg = "[User] This scatter file cannot be opened!\n\
                     Please check if your scatter file exists and is not used by other software.";
			break;

//	    case S_FTHND_CALLBACK_COM_INIT_STAGE_FAIL:break;

        case S_DL_UNSECURE_MAUI_TO_SECURE_BB:
			errMsg = "[User] Un-secure MAUI cannot be downloaded into Secure BB!\n\
                     Please use security-enabled MAUI.";
            break;

//=> security error code (6000~6999)
	    case S_SECURITY_CALLBACK_SLA_CHALLENGE_FAIL:
                errMsg = "[User] Please check the log and check the SLA_Challenge!\n";
   	    break;

	    case S_SECURITY_SLA_WRONG_AUTH_FILE:
			errMsg = "[User] The authentication file with a custom name is different \n\
                     from the custom name in the handset! Please ask the owner of this \n\
                     handset for an authentication file with a correct custom name.";
			break;

	    case S_SECURITY_SLA_INVALID_AUTH_FILE:
			errMsg = "[User] An invalid authentication file is used! This error \n\
                     message will occurs with MSP (MTK Secure Platform) code. \n\
                     Please keep up the MSP code and ask for help.";
			break;

//	    case S_SECURITY_SLA_CHALLENGE_FAIL:break;
//	    case S_SECURITY_SLA_FAIL:break;
//	    case S_SECURITY_DAA_FAIL:break;
//	    case S_SECURITY_SBC_FAIL:break;

	    case S_SECURITY_SF_SECURE_VER_CHECK_FAIL:
			errMsg = "[User] The secure version of the secure-MAUI is older than that in the handset!\n\
                     Please choose a secure-MAUI with an equal or newer secure version.";
			break;

	    case S_SECURITY_SF_HANDSET_SECURE_CUSTOM_NAME_NOT_MATCH:
			errMsg = "[User] The custom name of the secure-MAUI is different from that in the handset!\n\
                     Please choose a secure-MAUI with the same custom name.";
			break;

	    case S_SECURITY_SF_FTCFG_LOCKDOWN:
			errMsg = "[User] The Tool configuration is locked down!";
			break;

	    case S_SECURITY_SF_CODE_DOWNLOAD_FORBIDDEN:
			errMsg = "[User] To download code area is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_CODE_READBACK_FORBIDDEN:
			errMsg = "[User] To read-back code area is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_CODE_FORMAT_FORBIDDEN:
			errMsg = "[User] To format code area is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_SEC_RO_DOWNLOAD_FORBIDDEN:
			errMsg = "[User] To download SEC_RO is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_SEC_RO_READBACK_FORBIDDEN:
			errMsg = "[User] To read-back SEC_RO is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_SEC_RO_FORMAT_FORBIDDEN:
			errMsg = "[User] To format SEC_RO is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_FAT_DOWNLOAD_FORBIDDEN:
			errMsg = "[User] To download FAT area is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_FAT_READBACK_FORBIDDEN:
			errMsg = "[User] To read-back FAT area is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_FAT_FORMAT_FORBIDDEN:
			errMsg = "[User] To format FAT area is forbidden in this phone!";
			break;

	    case S_SECURITY_SF_RESTRICTED_AREA_ACCESS_FORBIDDEN:
			errMsg = "[User] To access restricted area (SEC_RO area and Flash Tool \n\
                     configuration area in the handset) is forbidden in this phone!";
			break;

	    case S_SECURITY_SECURE_CUSTOM_NAME_NOT_MATCH_BETWEEN_AUTH_AND_DL_HANDLE:
			errMsg = "[User] The custom name in the authentication file is different from that in secure-MAUI!\n\
                     Please choose the same custom name between an authentication file and a secure-MAUI!";
			break;

          case S_SECURITY_SECURE_CUSTOM_NAME_NOT_MATCH_BETWEEN_AUTH_AND_BL:
			errMsg = "[User] The custom name in the authentication file is different from that in secure-BootLoader!\n\
                     Please choose the same custom name between an authentication file and a secure-BootLoader!";
			break;


	    case S_SECURITY_DOWNLOAD_FILE_IS_CORRUPTED:
			errMsg = "[MAUI] Some information about security in secure-MAUI file is corrupted or broken! Please ask for help.";
			break;

//	    case S_SECURITY_NOT_SUPPORT:break;

        default:
            break;
    }

    asMsg = prefix + "\n\n" + errMsg + "\n[HINT]:\n"+ asMsg;
}


//---------------------------------------------------------------------------
void tboot_1::GetGPSErrorMessag(WRAPPER_RETURN id)
{
    AnsiString prefix, errMsg;

    if ( id == -1 ) {
        prefix.printf(" GPS MESSAGE : ( UI )\n\n", id);
    } else {
        prefix.printf(" GPS ERROR : ( %d )\n\n", id);
    }
    switch( id )
    {

//	case ERROR_WRONG_PARAM: break;
//	case ERROR_DLL_NOT_LOAD: break;
//	case ERROR_FUNC_INTERNAL_LOGIC: break;
//	case ERROR_SOMEWHERE_COMPLICATE_LOGIC: break;
//	case ERROR_SOMEWHERE_NONEXPECTED:  break;

	case ERROR_FAIL_TO_LOAD_DLL:
		errMsg = "[GPS FW] Dynamic-Load GPS DLL fails!";
		break;

	case ERROR_FAIL_TO_GETPROCADDRESS:
		errMsg = "[GPS FW] Dynamic-Link GPS DLL fails!";
		break;

	case ERROR_FAIL_TO_CREATE_IMAGEOBJ:
		errMsg = "[GPS FW] Create DL Handle fails!";
		break;

	case ERROR_FAIL_TO_CREATE_DAOBJ:
		errMsg = "[GPS FW] Create DA Handle fails!";
		break;

	case ERROR_FAIL_TO_CREATE_RBOBJ:
		errMsg = "[GPS FW] Create RB Handle fails!";
		break;

	case ERROR_FAIL_TO_DESTROY_IMAGEOBJ:
		errMsg = "[GPS FW] Destroy DL Object fails!";
		break;

	case ERROR_FAIL_TO_DESTROY_DAOBJ:
		errMsg = "[GPS FW] Destroy DA Object fails!";
		break;

	case ERROR_FAIL_TO_DESTROY_RBOBJ:
		errMsg = "[GPS FW] Destroy RB Object fails!";
		break;

//	case ERROR_FAIL_TO_SET_DA_STARTADDR:break;
//	case ERROR_SCAT_INCORRECT_FORMAT:break;
//	case ERROR_SCAT_OFFSET_IS_NOT_WORD_ALIG:break;
//	case ERROR_SCAT_ADDR_IS_NOT_WORD_ALIGN:break;
//	case ERROR_SCAT_ADDR_IS_NOT_ASCENDING_ORDER: break;
//	case ERROR_SCAT_FAIL_GET_COUNT:break;
//	case ERROR_SCAT_FAIL_GET_INFO:break;
//	case ERROR_SCAT_FAIL_SETATTR:break;

	case ERROR_DL_FW_INVALID_FORMAT:
		errMsg = "[GPS FW] You select an invalid format GPS FW!\r\nPlease select a correct GPS FW!";
		break;

//	case ERROR_FAIL_TO_SET_RB:break;
//	case ERROR_RB_GET_COUNT:break;
//	case ERROR_RB_FAIL_SETATTR:break;
//	case ERROR_FAIL_TO_SETPACKETLENGTH:break;
//	case ERROR_BOOT_STOP:break;
//	case ERROR_CALLBACK:break;

	case ERROR_FAIL_TO_OPEN_COMPORT:
		errMsg = "[GPS FW] Cannot open COM PORT!";
		break;

//	case ERROR_FAIL_TO_SET_DLBLOCK:break;

	case ERROR_FAIL_TO_DOWNLOAD_DA:
		errMsg = "[GPS FW] Download GPS Loader fails!\n\nPossible reasons:\n1. There doesn't exit GPS Module.\n2. GPIO_GPS_POWER_ON_PIN may be incorrectly defined!\n3. DL Cable is unstable, please try again.";
		break;

	case ERROR_FAIL_TO_FLASHDOWNLOAD:
		errMsg = "[GPS FW] Download GPS FW fails!\n\nPossible reasons:\n1. There doesn't exit GPS Module.\n2. GPIO_GPS_POWER_ON_PIN may be incorrectly defined!\n3. DL Cable is unstable, please try again.";
		break;

	case ERROR_FAIL_TO_FLASHREADBACK:
		errMsg = "[GPS FW] Readback GPS FW fails!\n\nPossible reasons:\n1. There doesn't exit GPS Module.\n2. GPIO_GPS_POWER_ON_PIN may be incorrectly defined!\n3. DL Cable is unstable, please try again.";
		break;

	case ERROR_FAIL_TO_OPEN_DEBUG_LOG:
		errMsg = "[GPS FW] Open Debug Log fails!";
		break;

	case ERROR_FAIL_TO_CLOSE_DEBUG_LOG:
		errMsg = "[GPS FW] Close Debug Log fails!";
		break;

	default:
		break;
    }

    asMsg = prefix +  "\n\n" + errMsg + "\n[HINT]:\n" + asMsg;
}
