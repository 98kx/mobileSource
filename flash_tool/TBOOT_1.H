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
*  tboot_1.h
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  header file of Boot ROM communicate thread
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/
//---------------------------------------------------------------------------

#ifndef tboot_1H
#define tboot_1H
//---------------------------------------------------------------------------
#include <Classes.hpp>


#include "mem.h"
#include "meta.h"
#include "flashtool.h"
#include "com_enum.h"
#include "com_util.h"
#include "oxdevicequery.h"

#include "BromDLLWrapper.h"


typedef enum{
	CREATE_FAIL = 0,
	CREATE_DIR_EXIST = 1,
	CREATE_SUCCESS = 2,
}CREATE_DIRECTORY_STATE_E;


//---------------------------------------------------------------------------
class tboot_1 : public TThread
{

protected:
        void _fastcall Execute();
        bool IsUSBCOMPortReady(unsigned short comPortNum);
        void FlashTool();
        void DebugDA();

public:
        __fastcall tboot_1( bool CreateSuspended );

		// update progress
        void SyncInitProgress();
        void _fastcall InitProgress();
        void _fastcall UpdateProgress();

        void SyncUpdateProgress();
        void _fastcall ClearStatusBar();

        void SyncDisplayFlashType();
        void _fastcall DisplayFlashType();


        void SyncShowMsg();
        void _fastcall ShowMsg();


		void _fastcall ShowOK_Form();

        void _fastcall StartTimer();
		void _fastcall StopTimer();

		void _fastcall UpdateDebugDAWindow();

        bool GetDynamicComPort( unsigned short *com_port );




public:
/******************************************************************************/
/* Download/Format/Readback/EnableBoot Modules                                */
/******************************************************************************/
        int  Download( void );
        int  Download_Internal();
        int  Format( void );
        int  Format_Internal();
        int  BackupDownloadRestore( void );
        int  BackupFormatRestore( void );
        int  DownloadRestore( void );
        int  DownloadUpload( void );
        int  FormatRestore( void );
        int  FormatUpload( void );
        int  RestoreOnly( void );
        int  UploadOnly( void );

        int  EnableBoot();
        int  FlashToolAPITest( void );
        /* Phase out
        int  RomWriter( void );
        */


        void  ShowFlashToolFinalResult( int ret );

/******************************************************************************/
/* Parameter Page Modules                                                     */
/******************************************************************************/
        int Parameter ( void );

/******************************************************************************/
/* GPS Modules                                                                */
/******************************************************************************/
        WRAPPER_RETURN GPSDownload ( void );
        WRAPPER_RETURN GPSReadback ( void );

/******************************************************************************/
/* Memory Test Modules                                                        */
/******************************************************************************/
        int MemoryTest ( void );
        int EnableSRAM ( FlashTool_Connect_Result    *cn_result);
        int EnableDRAM ( FlashTool_Connect_Result    *cn_result);
        // Memory Test : RAM test functions
        int RAMTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int RAMDataBusTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int RAMAddrBusTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int RAMPatternTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result, int pattern_set[], int pattern_size);
        int RAMOnePatternTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result, int pattern);
        int RAMIncDecTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int RAMIncDecTestIO ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result, HW_MemoryIO_E io);
        // Memory Test : NOR test functions
        int NORTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int NORAddrBusTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int NORPatternTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        // Memory Test : NAND test functions
        int NANDTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);
        int NANDPatternTest ( FlashTool_MemoryTest_Arg *mt_arg,FlashTool_MemoryTest_Result *mt_result);


        void UpdateMemoryTestDetection();
        void _fastcall UpdateMemoryTestRichEditor(AnsiString s, TColor color);


/******************************************************************************/
/* Meta Modules                                                               */
/******************************************************************************/
        // Meta Module
        int  EnterMETAMode();
        int  EnterMETAResetFactoryDefault( void );
        int  EnterMETABackup(bool& isPreserveBackupData,
                             bool& isUseComPortStoreData,
                             AnsiString& BackupCalibrationDataFolderPath);
        int  EnterMETARestore(bool& isPreserveBackupData,
                              bool& isUseComPortStoreData,
                              AnsiString& BackupCalibrationDataFolderPath);
        int  EnterMETAUpload( void );

        CREATE_DIRECTORY_STATE_E CreateBackupDir(const AnsiString BackUpDirNumber, 
                                                    AnsiString& BackupCalibrationDataFolderPath);
        bool RemoveBackupDir(const AnsiString BackupCalibrationDataFolderPath);
        bool CheckBackupDirExist(const AnsiString BackupCalibrationDataFolderPath);
        bool PrepareBackUpDirNumber(AnsiString& BackUpDirNumber,
                                        bool& isPreserveBackupData,
                                        bool& isUseComPortStoreData);

        void SyncStartFakeProgress();

        void _fastcall StartFakeProgress();
		void _fastcall ResetFakeProgress();
		void _fastcall StopFakeProgress();
		void _fastcall FinishFakeProgress();


/******************************************************************************/
/* fill the FlashTool Args                                                    */
/******************************************************************************/

        int ArgComPortSetting( COM_PORT_SETTING &comPortSetting );
        int ArgComPortSetting_ByhCOM( COM_PORT_SETTING  &comPortSetting );
        int ArgFlashTool( FLASHTOOL_ARG &arg, FLASHTOOL_RESULT &result );
        int ArgBootFlashTool( BOOT_FLASHTOOL_ARG &bootFlashTool, void * m_cb_in_brom_stage_arg );
        int ArgFlashToolConnect( FlashTool_Connect_Arg &ftConnect, FlashTool_Connect_Result &ftConnectResult );
        int ArgFlashDownload( void );
        int ArgFlashReadback( void );
        // int ArgFlashFormat( void );
        int ArgFlashFormatEx( void );
        int ArgFlashToolDownload( FLASHTOOL_DOWNLOAD_ARG &dl_arg, FLASHTOOL_DOWNLOAD_RESULT &dl_result );
        int ArgFlashToolReadback( FLASHTOOL_READBACK_ARG &rb_arg, FLASHTOOL_READBACK_RESULT &rb_result );
        // int ArgFlashToolFormat( FLASHTOOL_FORMAT_ARG &fmt_arg, FLASHTOOL_FORMAT_RESULT &fmt_result );
        int ArgFlashToolFormatEx( FLASHTOOL_FORMAT_ARG_EX &fmt_arg_ex, FLASHTOOL_FORMAT_RESULT    &fmt_result);
        int ArgFlashToolParameter( FLASHTOOL_PARAMETER_ARG &pm_arg, FLASHTOOL_PARAMETER_RESULT &pm_result );
        int ArgFlashToolEnableBoot( FLASHTOOL_ENABLE_BOOT_ARG &enable_boot_arg, FLASHTOOL_ENABLE_BOOT_RESULT &enable_boot_result );
         /* Phase out
        int ArgFlashToolProgress( FlashTool_Progress_Arg  &pgs_arg );
        */
        int ArgFlashToolChangeBaudrate( FlashTool_ChangeBaudrate_Arg &cb_arg );
        int ArgFlashToolMemoryTest( FlashTool_MemoryTest_Arg &mt_arg, FlashTool_MemoryTest_Result &mt_result );

/******************************************************************************/
/* fill the MEAT Args                                                         */
/******************************************************************************/

        int ArgMETAConnectWithTarget( META_Connect_Req &meta_con_req, META_Connect_Report &meta_con_report );
        int ArgMETAConnectWithTargetViaUSB( META_ConnectByUSB_Req &meta_con_req, META_ConnectByUSB_Report &meta_con_report );
        int ArgMETABackupCalibrationData( const AnsiString BackupCalibrationDataFolderPath, MISC_BACKUP_REQ_T &meta_backup_req );
        int ArgMETAEnableWatchDogTimer( FtWatchDog &meta_watchdog_req );
        int ArgMETARestoreCalibrationData( const AnsiString BackupCalibrationDataFolderPath, MISC_RESTORE_REQ_T   &meta_restore_req );
        int ArgMETAGetBackupResultInfo( BACKUP_RESULT_T &cnf );
        int ArgMETAGetRestoreResultInfo( RESTORE_RESULT_T &cnf );
        int ArgMETAUploadFilesToTarget( MISC_UPLOAD_REQ_EX_T &meta_upload_req, AnsiString *imei_status);


/******************************************************************************/
/* fill the FlashTool Args                                                    */
/******************************************************************************/
        // unity functions
        void GetGPSErrorMessag( WRAPPER_RETURN id);
        void GetErrorMessag( STATUS_E id, unsigned int m_msp_error_code);

public:
		bool	 		bUpdateFinishBytes;
		unsigned char	progress_percentage;
		unsigned long	progress_finished_bytes;
		unsigned long	progress_total_bytes;
		AnsiString		progress_hint;
		AnsiString		progress_flashtype;
		TColor			progress_forecolor;

        unsigned char   m_RecvChar;

        COM_PORT_SETTING            com;
		FLASHTOOL_ARG				arg;
		FLASHTOOL_RESULT			result;
		FLASHTOOL_DOWNLOAD_ARG		dl_arg;
		FLASHTOOL_DOWNLOAD_RESULT	dl_result;
		FLASHTOOL_READBACK_ARG		rb_arg;
		FLASHTOOL_READBACK_RESULT	rb_result;
//		FLASHTOOL_FORMAT_ARG		fmt_arg;
		FLASHTOOL_FORMAT_ARG_EX		fmt_arg_ex;		
		FLASHTOOL_FORMAT_RESULT		fmt_result;


        FlashTool_Connect_Arg        cn_arg;
        FlashTool_Connect_Result     cn_result;


		char	m_strHWDetectionReport[2048];
		char	m_strNorFlashHint[512];
		char	m_strNandFlashHint[512];

        AnsiString      asMsg;

        AnsiString      m_RecvString;
};
//---------------------------------------------------------------------------
#endif
