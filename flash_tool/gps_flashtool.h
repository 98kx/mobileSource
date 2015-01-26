/*******************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2003
 *
 ******************************************************************************/

/*******************************************************************************
 * Filename:
 * ---------
 *  flashtool.h
 *
 * Project:
 * --------
 *  Flash Download/Readback/Format Library.
 *
 * Description:
 * ------------
 *  Exported C interface APIs for FlashTool Library.
 *
 * Author:
 * -------
 *  Amos Hsu (mtk00539)
 *
 *******************************************************************************/
#ifndef _GPS_FLASHTOOL_H_
#define _GPS_FLASHTOOL_H_

#include "flashtool.h"

#include "gps_brom.h"
#include "gps_DOWNLOAD.H"
#include "gps_flashtool_handle.h"

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// return code                                                                  
//------------------------------------------------------------------------------

#define FT_RET(ret)	(ret&0x000000FF)

#define FT_OK							0x000000
#define FT_ERROR						0x000001
#define FT_INVALID_ARGUMENTS			0x000002
#define FT_COM_PORT_OPEN_ERR			0x000003
#define FT_DA_HANDLE_ERROR				0x000004
#define FT_DL_HANDLE_ERROR				0x000005
#define FT_RB_HANDLE_ERROR				0x000006
#define FT_BROM_ERROR					0x000007
#define FT_COM_PORT_SET_TIMEOUT_ERR		0x000008
#define FT_DA_NO_RESPONSE				0x000009
#define FT_DA_SYNC_INCORRECT			0x00000A
#define FT_DA_VERSION_INCORRECT			0x00000B
#define FT_DA_UNKNOWN_FLASH_DEVICE		0x00000C
#define FT_DA_SET_EXT_CLOCK_FAIL		0x00000D
#define FT_DA_SET_BBCHIP_TYPE_FAIL		0x00000E
#define FT_DA_CHANGE_BAUDRATE_FAIL		0x00000F
#define FT_DA_SET_DOWNLOAD_BLOCK_FAIL	0x000010
#define FT_DA_DOWNLOAD_FAIL				0x000011
#define FT_DA_READBACK_FAIL				0x000012
#define FT_DA_FORMAT_FAIL				0x000013
#define FT_DA_FINISH_CMD_FAIL			0x000014
#define FT_DA_SOC_CHECK_FAIL			0x000015
#define FT_DA_BBCHIP_DSP_VER_INCORRECT	0x000016
#define FT_SKIP_AUTO_FORMAT_FAT			0x000017
#define FT_DA_HW_ERROR					0x000018
#define FT_DA_ENABLE_WATCHDOG_FAIL		0x000019
#define FT_CALLBACK_ERROR				0x000020
                                          
//------------------------------------------------------------------------------
// flash device string table                                                    
//------------------------------------------------------------------------------
extern const char * __stdcall GPS_GetFlashDeviceNameByTypeId( DEVICE_INFO	type_id );

//------------------------------------------------------------------------------
// DA report structure                                                          
//------------------------------------------------------------------------------
typedef	struct {
	unsigned char	expected_da_major_ver;
	unsigned char	expected_da_minor_ver;
	unsigned char	da_major_ver;
	unsigned char	da_minor_ver;
	DEVICE_INFO		flash_device_id;
	unsigned int	flash_size;
	unsigned int	fat_begin_addr;
	unsigned int	fat_length;
	unsigned int	ext_sram_size;
}GPS_DA_REPORT_T;

//------------------------------------------------------------------------------
// prototype of callback function                                               
//------------------------------------------------------------------------------
typedef int (__stdcall *GPS_CALLBACK_BEFORE_PROCESS)(const GPS_DA_REPORT_T  *p_da_report, void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_SET_HIGHSPEED_BAUDRATE)(unsigned char BaudrateId, unsigned int *p_baudrate, void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_FLASH_DOWNLOAD_INIT)(void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_FLASH_DOWNLOAD)(unsigned char finished_percentage, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_FLASH_FORMAT_INIT)(void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_FLASH_FORMAT)(unsigned char finished_percentage, void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_FLASH_READBACK_INIT)(const char *rb_filepath, void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_FLASH_READBACK)(unsigned char finished_percentage, unsigned int read_bytes, unsigned int total_bytes, void *usr_arg);

//------------------------------------------------------------------------------
// GPS_FLASHTOOL_ARG structure
//------------------------------------------------------------------------------

#define	WAIT_STATE_AUTO_DETECT_BY_FREQUENCY		NULL

typedef	struct {

	GPS_BBCHIP_TYPE			bbchip;
	EXT_CLOCK			ext_clock;

	unsigned int		*p_bank0_mem_cfg;
	unsigned int		*p_bank1_mem_cfg;

	GPS_DA_HANDLE_T			da_handle;

	GPS_CALLBACK_BROM_WRITE_BUF_INIT	cb_download_da_init;
	void 	*cb_download_da_init_arg;

	GPS_CALLBACK_BROM_WRITE_BUF		cb_download_da;
	void 	*cb_download_da_arg;

	GPS_CALLBACK_BEFORE_PROCESS		cb_before_process;
	void 	*cb_before_process_arg;

	GPS_CALLBACK_SET_HIGHSPEED_BAUDRATE		cb_set_high_speed_baudrate;
	void 	*cb_set_high_speed_baudrate_arg;

	unsigned int		ms_boot_timeout;
	unsigned int		max_start_cmd_retry_count;

	GPS_DA_INFO		da_info;

	// [OUT] the PC side bbchip name
	const char  *p_bbchip_name;

	// This callback function will be invoke after BootROM start cmd is passed.
	// You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
	// or do whatever you want otherwise.
	GPS_CALLBACK_IN_BROM_STAGE		cb_in_brom_stage;
	void	*cb_in_brom_stage_arg;

	unsigned char	m_baudrate_full_sync_count;

	// speed-up BootROM stage baudrate
	_BOOL	m_speedup_brom_baudrate;

}GPS_FLASHTOOL_ARG;

//------------------------------------------------------------------------------
// flash download
//------------------------------------------------------------------------------

typedef enum {
	FORMAT_FAT_AUTO = 0,		// automatically format the FAT area on flash 
	FORMAT_FAT_MANUAL			// format according to the format begin_addr and length 
} GPS_FORMAT_FAT_OPTION;

typedef	struct {

	GPS_FORMAT_FAT_OPTION	option;

	// begin_addr and length only take effect when option is FORMAT_MANUAL 
	unsigned int		fat_begin_addr;
	unsigned int		fat_length;

} GPS_FORMAT_FAT_CONFIG_T;

typedef	struct {
	
	GPS_DL_HANDLE_T		dl_handle;

	GPS_CALLBACK_FLASH_DOWNLOAD_INIT	cb_download_flash_init;
	void	*cb_download_flash_init_arg;

	ACCURACY	download_accuracy;
	GPS_CALLBACK_FLASH_DOWNLOAD		cb_download_flash;
	void	*cb_download_flash_arg;

	GPS_CALLBACK_FLASH_FORMAT_INIT	cb_format_report_init;
	void	*cb_format_report_init_arg;

	GPS_CALLBACK_FLASH_FORMAT	cb_format_report;
	void	*cb_format_report_arg;

	GPS_ROM_INFO		rom_info;
	
	// The flag of enable_tgt_res_layout_check is used to control whether if target resource 
	// layout checking operation will be performed.                                          
	// _TRUE: Enable target resource layout checking operation.                         
	// _FALSE: Disable target resource layout checking operation.                       
	_BOOL			enable_tgt_res_layout_check;

	// This array is used to store the index of GPS_DL_HANDLE if and only if you got the 
	// (FT_DL_HANDLE_ERROR|FTHND_RES_DOWNLOAD_SELECT_INCOMPLETE) error code.         
	// In order to maintain target side resource and jump table integrity, each index points 
	// to a resource which is required to download but you DID NOT select it. 
	unsigned short	required_resource_count;
	unsigned short	required_resource[MAX_RESOURCE_ENTRY];

	// The flag to check if target side baseband chip ECO version is corresponding to ROM file on PC side. 
	_BOOL			enable_bbchip_eco_check;

	// format option 
	_BOOL					enable_fat_format;
	GPS_FORMAT_FAT_CONFIG_T		fmt_fat;

	// trigger watch dog timeout after downloading 
	_BOOL					enable_wdt_timeout;
	unsigned short			ms_wdt_timeout_interval;

}GPS_FLASHTOOL_DOWNLOAD_ARG;

extern int __stdcall GPS_FlashDownload(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_DOWNLOAD_ARG	*p_dl_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_FlashDownload_ByhCOM(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_DOWNLOAD_ARG	*p_dl_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_AutoBoot_FlashDownload(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_DOWNLOAD_ARG	*p_dl_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_AutoBoot_FlashDownload_ByhCOM(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_DOWNLOAD_ARG	*p_dl_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

//------------------------------------------------------------------------------
// flash readback                                                               
//------------------------------------------------------------------------------

typedef	struct {
	
	GPS_RB_HANDLE_T	 rb_handle;

	GPS_CALLBACK_FLASH_READBACK_INIT	cb_readback_flash_init;
	void	*cb_readback_flash_init_arg;

	ACCURACY	readback_accuracy;
	GPS_CALLBACK_FLASH_READBACK		cb_readback_flash;
	void	*cb_readback_flash_arg;

	GPS_RB_INFO		rb_info;
	
}GPS_FLASHTOOL_READBACK_ARG;

extern int __stdcall GPS_FlashReadback(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_READBACK_ARG	*p_rb_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_FlashReadback_ByhCOM(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_READBACK_ARG	*p_rb_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_AutoBoot_FlashReadback(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_READBACK_ARG	*p_rb_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_AutoBoot_FlashReadback_ByhCOM(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_READBACK_ARG	*p_rb_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

//------------------------------------------------------------------------------
// flash format                                                                 
//------------------------------------------------------------------------------

typedef	struct {
	
	unsigned int	format_addr;
	unsigned int	format_length;

	GPS_CALLBACK_FLASH_FORMAT_INIT	cb_format_report_init;
	void	*cb_format_report_init_arg;

	GPS_CALLBACK_FLASH_FORMAT	cb_format_report;
	void	*cb_format_report_arg;
	
}GPS_FLASHTOOL_FORMAT_ARG;

extern int __stdcall GPS_FlashFormat(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_FORMAT_ARG	*p_fmt_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_FlashFormat_ByhCOM(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_FORMAT_ARG	*p_fmt_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_AutoBoot_FlashFormat(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_FORMAT_ARG	*p_fmt_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

extern int __stdcall GPS_AutoBoot_FlashFormat_ByhCOM(
		COM_PORT_SETTING	*p_com_setting,
		GPS_FLASHTOOL_ARG	*p_arg,
		GPS_FLASHTOOL_FORMAT_ARG	*p_fmt_arg,
		int *p_stopflag,
		GPS_DA_REPORT_T  *p_da_report);

#ifdef	__cplusplus
}
#endif

#endif


