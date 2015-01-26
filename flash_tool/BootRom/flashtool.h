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
#ifndef _FLASHTOOL_H_
#define _FLASHTOOL_H_

#include "mtk_status.h"
#include "brom.h"
#include "DOWNLOAD.H"
#include "flashtool_handle.h"
#include "flashtool_api.h"

#ifdef	__cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// utility functions
//------------------------------------------------------------------------------
extern const char * __stdcall GetNorFlashNameByTypeId(unsigned short type_id);
extern const char * __stdcall GetNandFlashNameByTypeId(unsigned short type_id);
extern const char * __stdcall HWChipSelectToString(HW_ChipSelect_E  cs);
extern const char * __stdcall HWStorageTypeToString(HW_StorageType_E  type);
extern const char * __stdcall HWRamTypeToString(HW_RAMType_E  type);
extern const char * __stdcall HWMemoryTypeToString(HW_MemoryType_E type);
extern const char * __stdcall HWMemoryIOToString(HW_MemoryIO_E type);
extern const char * __stdcall HWMemoryTestMethodToString(HW_MemoryTestMethod_E type);
extern const char * __stdcall ReadFlagToString(NUTL_ReadFlag_E  flag);
extern const char * __stdcall EraseFlagToString(NUTL_EraseFlag_E  flag);
extern const char * __stdcall ProgramFlagToString(NUTL_ProgramFlag_E  flag);
extern const char * __stdcall ParamOpToString(PARAM_OPERATION  op);
extern const char * __stdcall OTPOpToString(OTP_OPERATION  op);

//------------------------------------------------------------------------------
// DA report related function
//------------------------------------------------------------------------------
int __stdcall ConvertDAReportToString(
		const DA_REPORT_T		*p_da_report,
		char					*p_nor_flash_hint,
		unsigned int			nor_flash_hint_size,
		char					*p_nand_flash_hint,
		unsigned int			nand_flash_hint_size,
		char					*p_detail_report,
		unsigned int			detail_report_size);

//------------------------------------------------------------------------------
// prototype of callback function
//------------------------------------------------------------------------------
typedef int (__stdcall *CALLBACK_PARAM_PROGRESS_INIT)(void *usr_arg);
typedef int (__stdcall *CALLBACK_PARAM_PROGRESS)(unsigned char finished_percentage, void *usr_arg);

//------------------------------------------------------------------------------
// COM_PORT structure
//------------------------------------------------------------------------------
typedef union {
	unsigned char	number;
	HANDLE			handle;
} COM_PORT_HANDLE;

typedef struct {
	COM_PORT_HANDLE	com;
	UART_BAUDRATE	baudrate;
	unsigned int	ms_read_timeout;
	unsigned int	ms_write_timeout;
} COM_PORT_SETTING;

//------------------------------------------------------------------------------
// FLASHTOOL_ARG structure
//------------------------------------------------------------------------------

typedef	struct {

	// BootFlashTool arg
	BOOT_FLASHTOOL_ARG						m_boot_arg;

	// da report callback
	CALLBACK_DA_REPORT						m_cb_da_report;
	void *									m_cb_da_report_arg;

	// security pre-process notify callback
	CALLBACK_SECURITY_PRE_PROCESS_NOTIFY	m_cb_security_pre_process_notify;
	void *									m_cb_security_pre_process_notify_arg;

	// set high-speed baudrate callback
	CALLBACK_SET_HIGHSPEED_BAUDRATE			m_cb_set_high_speed_baudrate;
	void *									m_cb_set_high_speed_baudrate_arg;
	unsigned char							m_baudrate_full_sync_count;

	HW_ChipSelect_E		m_nor_chip_select[2];
	HW_ChipSelect_E		m_nand_chip_select;
	unsigned int *		m_p_nand_acccon;

	// enable DRAM setting from UI thread
	_BOOL				m_enable_ui_dram_cfg;
	DRAM_SETTING		m_dram_cfg;

	// to enable BAT_ON check for USBDL w/o battery
	_BOOL			    m_enable_download_without_battery;

} FLASHTOOL_ARG;

typedef	struct {

	// DA report
	DA_REPORT_T			m_da_report;

	DA_INFO				m_da_info;
	AUTH_INFO			m_auth_info;
	SCERT_INFO			m_scert_info;

} FLASHTOOL_RESULT;


//------------------------------------------------------------------------------
// FLASHTOOL_DOWNLOAD structure
//------------------------------------------------------------------------------

typedef	struct {

	// single DL_HANDLE
	DL_HANDLE_T						m_dl_handle;
	// multi DL_HANDLE List
	DL_HANDLE_LIST_T				m_dl_handle_list;

	// DL_HANDLE download progress callback
	ACCURACY							m_download_accuracy;
	CALLBACK_DOWNLOAD_PROGRESS_INIT		m_cb_download_flash_init;
	void *								m_cb_download_flash_init_arg;
	CALLBACK_DOWNLOAD_PROGRESS			m_cb_download_flash;
	void *								m_cb_download_flash_arg;

	// Boot Loader download progress callback
	CALLBACK_BOOTLOADER_DOWNLOAD_PROGRESS_INIT	m_cb_download_bloader_init;
	void *										m_cb_download_bloader_init_arg;
	CALLBACK_BOOTLOADER_DOWNLOAD_PROGRESS		m_cb_download_bloader;
	void *										m_cb_download_bloader_arg;

	// security post-process notify callback
	CALLBACK_SECURITY_POST_PROCESS_NOTIFY		m_cb_security_post_process_notify;
	void *										m_cb_security_post_process_notify_arg;

	// fat range change notify callback
	CALLBACK_DOWNLOAD_FAT_CHANGE_NOTIFY        m_cb_download_fat_change_notify;
	void *                                      m_cb_download_fat_change_notify_arg;

	// write FlashToolCfg/CUST_PARA/SEC_RO/OTP parameters

	// FlashToolCfg
	_BOOL				m_ft_cfg_enable;
	FlashToolCfg_T		m_ft_cfg;
	// CUST_PARA
	_BOOL				m_cust_para_enable;
	CustPara_T			m_cust_para;
	// SEC_RO
	_BOOL				m_sec_ro_enable;
	SecRO_T				m_sec_ro;
	// OTP
	_BOOL				m_otp_enable;
	OTP_OPERATION		m_otp_op;
	OTP_T				m_otp;

	CALLBACK_PARAM_PROGRESS_INIT		m_cb_param_process_init;
	void *								m_cb_param_process_init_arg;
	CALLBACK_PARAM_PROGRESS				m_cb_param_process;
	void *								m_cb_param_process_arg;

	// format option
	_BOOL								m_format_enable;
	FORMAT_CONFIG_T						m_format_cfg;
	// format progress callback
	CALLBACK_FORMAT_PROGRESS_INIT		m_cb_format_report_init;
	void *								m_cb_format_report_init_arg;
	CALLBACK_FORMAT_PROGRESS			m_cb_format_report;
	void *								m_cb_format_report_arg;
	// format statistics callback
	CALLBACK_FORMAT_STATISTICS			m_cb_format_statistics;
	void *								m_cb_format_statistics_arg;

	// post-process callback
	CALLBACK_POST_PROCESS_INIT			m_cb_post_process_init;
	void *								m_cb_post_process_init_arg;
	CALLBACK_POST_PROCESS				m_cb_post_process;
	void *								m_cb_post_process_arg;

	// The flag of m_enable_tgt_res_layout_check is used to control whether if target resource
	// layout checking operation will be performed.
	// _TRUE: Enable target resource layout checking operation.
	// _FALSE: Disable target resource layout checking operation.
	_BOOL				m_enable_tgt_res_layout_check;

	// The flag to check if target side baseband chip version is corresponding to ROM file on PC side.
	_BOOL				m_enable_bbchip_ver_check;

	// Download Style : _FALSE	: best effort erase
	//					_TRUE	: sequential erase (sector by sector)
	_BOOL				m_downloadstyle_sequential;

	// trigger watch dog timeout after downloading
	_BOOL				m_enable_wdt_timeout;
	unsigned short		m_ms_wdt_timeout_interval;

	// disable Finishd Cmd (for GPS ROM download)
	_BOOL				m_relay_da_enable;
	GPIO_GPS_SETTING	m_gpio_gps;

	// enable reset to normal mode after download finished
	_BOOL                       m_enable_reset_to_normal_mode;

} FLASHTOOL_DOWNLOAD_ARG;

typedef	struct {

	FormatStatisticsReport_T	m_format_statistics;

	// NFB boot loader info
	BL_INFO				m_bl_info;

	ROM_INFO			m_rom_info;
	_BOOL				m_BLNotDownloadWarning;

} FLASHTOOL_DOWNLOAD_RESULT;

extern int __stdcall FlashDownload(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_DOWNLOAD_ARG	*p_dl_arg,
		FLASHTOOL_DOWNLOAD_RESULT		*p_dl_result,
		int								*p_stopflag);

extern int __stdcall FlashDownload_ByhCOM(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_DOWNLOAD_ARG	*p_dl_arg,
		FLASHTOOL_DOWNLOAD_RESULT		*p_dl_result,
		int								*p_stopflag);

//------------------------------------------------------------------------------
// FLASHTOOL_READBACK_ARG structure
//------------------------------------------------------------------------------
typedef	struct {

	HW_StorageType_E					m_storage_type;

	RB_HANDLE_T							m_rb_handle;

	// readback progress callback
	ACCURACY							m_readback_accuracy;
	CALLBACK_READBACK_PROGRESS_INIT		m_cb_readback_flash_init;
	void *								m_cb_readback_flash_init_arg;
	CALLBACK_READBACK_PROGRESS			m_cb_readback_flash;
	void *								m_cb_readback_flash_arg;

} FLASHTOOL_READBACK_ARG;

typedef	struct {

	RB_INFO		m_rb_info;

} FLASHTOOL_READBACK_RESULT;

extern int __stdcall FlashReadback(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_READBACK_ARG	*p_rb_arg,
		FLASHTOOL_READBACK_RESULT		*p_rb_result,
		int								*p_stopflag);

extern int __stdcall FlashReadback_ByhCOM(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_READBACK_ARG	*p_rb_arg,
		FLASHTOOL_READBACK_RESULT		*p_rb_result,
		int								*p_stopflag);

//------------------------------------------------------------------------------
// FLASHTOOL_FORMAT_ARG structure
//------------------------------------------------------------------------------
typedef	struct {

	HW_StorageType_E					m_storage_type;

	// format config
	FORMAT_CONFIG_T						m_format_cfg;

	// only take effect on NAND flash
	NUTL_EraseFlag_E  					m_erase_flag;

	// format progress callback
	CALLBACK_FORMAT_PROGRESS_INIT		m_cb_format_report_init;
	void *								m_cb_format_report_init_arg;
	CALLBACK_FORMAT_PROGRESS			m_cb_format_report;
	void *								m_cb_format_report_arg;
	// format statistics callback
	CALLBACK_FORMAT_STATISTICS			m_cb_format_statistics;
	void *								m_cb_format_statistics_arg;

	// trigger watch dog timeout after format
	_BOOL								m_enable_wdt_timeout;
	unsigned short						m_ms_wdt_timeout_interval;

} FLASHTOOL_FORMAT_ARG;

typedef	struct {

	FormatStatisticsReport_T			m_format_statistics;

} FLASHTOOL_FORMAT_RESULT;

extern int __stdcall FlashFormat(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_FORMAT_ARG		*p_fmt_arg,
		FLASHTOOL_FORMAT_RESULT			*p_fmt_result,
		int *p_stopflag);

extern int __stdcall FlashFormat_ByhCOM(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_FORMAT_ARG		*p_fmt_arg,
		FLASHTOOL_FORMAT_RESULT			*p_fmt_result,
		int								*p_stopflag);



typedef	struct {

    FORMAT_CONFIG_EX_T  m_format_cfg;

    // format progress callback
    CALLBACK_FORMAT_PROGRESS_INIT		m_cb_format_report_init;
    void *								m_cb_format_report_init_arg;
    CALLBACK_FORMAT_PROGRESS			m_cb_format_report;
    void *								m_cb_format_report_arg;
    // format statistics callback
    CALLBACK_FORMAT_STATISTICS			m_cb_format_statistics;
    void *								m_cb_format_statistics_arg;

    // trigger watch dog timeout after format
    _BOOL								m_enable_wdt_timeout;
    unsigned short						m_ms_wdt_timeout_interval;

} FLASHTOOL_FORMAT_ARG_EX;

extern int __stdcall FlashFormat_Ex(COM_PORT_SETTING *p_com_setting,
                          const FLASHTOOL_ARG *p_arg,
                          FLASHTOOL_RESULT *p_result,
                          const FLASHTOOL_FORMAT_ARG_EX *p_fmt_arg_ex,
                          FLASHTOOL_FORMAT_RESULT *p_fmt_result,
                          int *p_stopflag);


extern int __stdcall FlashFormat_ByhCOM_Ex(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_FORMAT_ARG_EX	*p_fmt_arg_ex,
		FLASHTOOL_FORMAT_RESULT		*p_fmt_result,
		int								*p_stopflag);

//------------------------------------------------------------------------------
// PARAMETER_ARG structure
//------------------------------------------------------------------------------

typedef struct {
	// FlashToolCfg
	_BOOL				m_ft_cfg_enable;
	PARAM_OPERATION		m_ft_cfg_op;
	FlashToolCfg_T		m_ft_cfg;
	// CUST_PARA
	_BOOL				m_cust_para_enable;
	PARAM_OPERATION		m_cust_para_op;
	CustPara_T			m_cust_para;
	// SEC_RO
	_BOOL				m_sec_ro_enable;
	PARAM_OPERATION		m_sec_ro_op;
	SecRO_T				m_sec_ro;
	// OTP
	_BOOL				m_otp_enable;
	OTP_OPERATION		m_otp_op;
	OTP_T				m_otp;
} PARAMETER_ARG;

typedef	struct {

	DL_HANDLE_T			m_dl_handle;

	// parameters
	PARAMETER_ARG		m_param;

	// update parameter progress callback
	CALLBACK_PARAM_PROGRESS_INIT		m_cb_param_process_init;
	void *								m_cb_param_process_init_arg;
	CALLBACK_PARAM_PROGRESS				m_cb_param_process;
	void *								m_cb_param_process_arg;

	// post-process callback for Update SEC_RO
	CALLBACK_POST_PROCESS_INIT			m_cb_post_process_init;
	void *								m_cb_post_process_init_arg;
	CALLBACK_POST_PROCESS				m_cb_post_process;
	void *								m_cb_post_process_arg;

	// enter relay mode (for GPS ROM download)
	_BOOL				m_relay_da_enable;
	GPIO_GPS_SETTING	m_gpio_gps;

} FLASHTOOL_PARAMETER_ARG;

typedef	struct {

	// dummy

} FLASHTOOL_PARAMETER_RESULT;

extern int __stdcall FlashParameter(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_PARAMETER_ARG	*p_pm_arg,
		FLASHTOOL_PARAMETER_RESULT		*p_pm_result,
		int								*p_stopflag);

extern int __stdcall FlashParameter_ByhCOM(
		COM_PORT_SETTING				*p_com_setting,
		const FLASHTOOL_ARG				*p_arg,
		FLASHTOOL_RESULT				*p_result,
		const FLASHTOOL_PARAMETER_ARG	*p_pm_arg,
		FLASHTOOL_PARAMETER_RESULT		*p_pm_result,
		int								*p_stopflag);

//----------------------------------------------------------------------------
// FLASHTOOL_ENABLE_BOOT_ARG structure
//----------------------------------------------------------------------------
typedef struct {

    BootCert_T  m_boot_cert;

    // enable boot progress callback
    CALLBACK_PARAM_PROGRESS_INIT    m_cb_enable_boot_process_init;
    void                            *m_cb_enable_boot_process_init_arg;
    CALLBACK_PARAM_PROGRESS         m_cb_enable_boot_process;
    void                            *m_cb_enable_boot_process_arg;

} FLASHTOOL_ENABLE_BOOT_ARG;

typedef struct {

    // dummy

} FLASHTOOL_ENABLE_BOOT_RESULT;

extern int __stdcall FlashEnableBoot(
        COM_PORT_SETTING                *p_com_setting,
        const FLASHTOOL_ARG             *p_arg,
        FLASHTOOL_RESULT                *p_result,
        const FLASHTOOL_ENABLE_BOOT_ARG *p_enable_boot_arg,
        FLASHTOOL_ENABLE_BOOT_RESULT    *p_enable_boot_result,
        int                             *p_stopflag);

extern int __stdcall FlashEnableBoot_ByhCOM(
        COM_PORT_SETTING                *p_com_setting,
        const FLASHTOOL_ARG             *p_arg,
        FLASHTOOL_RESULT                *p_result,
        const FLASHTOOL_ENABLE_BOOT_ARG *p_enable_boot_arg,
        FLASHTOOL_ENABLE_BOOT_RESULT    *p_enable_boot_result,
        int                             *p_stopflag);
//------------------------------------------------------------------------------

#ifdef	__cplusplus
}
#endif

#endif


