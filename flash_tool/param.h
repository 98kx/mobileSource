#ifndef _PARAM_H_
#define _PARAM_H_

#include <vcl.h>
#include "flashtool.h"

typedef enum{
    PARAM_FT_CFG = 0
    ,PARAM_SEC_RO
    ,PARAM_CUST
    ,PARAM_OTP
    ,PARAM_GPS
    ,PARAM_END
} ParameterType_T;

typedef struct {

	void reset() {

        // FlashToolCfg reset
		m_ft_cfg_enable      = _FALSE;
		memset(&m_ft_cfg, 0, sizeof(m_ft_cfg));

		// CUST_PARA parameter reset
		m_cust_para_enable   = _FALSE;
		memset(&m_cust_para, 0, sizeof(m_cust_para));
		m_cust_para_filepath = "";

		// SEC_RO parameter reset
		m_sec_ro_enable    = _FALSE;
		memset(&m_sec_ro, 0, sizeof(m_sec_ro));
		m_sec_ro_filepath  = "";

		// OTP parameter reset
		m_otp_enable       = _FALSE;
		m_otp_op           = OTP_WRITE;
		memset(&m_otp, 0, sizeof(m_otp));
		m_otp_filepath     = "";

        // GPS ROM parameter reset
        m_gps_download     = _FALSE;
        m_gps_enable       = _FALSE;

        m_gps_rom_dirpath  = "";
        m_gps_rom_filepath = "";
        m_gps_ini_filepath = "";

        m_gps_rom_filesize = 0;

        m_gps_power_pin    = 0xFFFF;
        m_gps_reset_pin    = 0xFFFF;
        m_gps_standby_pin  = 0xFFFF;
        m_gps_32k_pin      = 0xFFFF;
	}

	// FlashToolCfg
	_BOOL				m_ft_cfg_enable;
	FlashToolCfg_T		m_ft_cfg;

	// CUST_PARA
	_BOOL				m_cust_para_used;
	_BOOL				m_cust_para_enable;
	CustPara_T			m_cust_para;
	AnsiString			m_cust_para_filepath;

	// SEC_RO
	_BOOL				m_sec_ro_used;
	_BOOL				m_sec_ro_enable;
	SecRO_T				m_sec_ro;
	AnsiString			m_sec_ro_filepath;

	// OTP
	_BOOL				m_otp_used;
	_BOOL				m_otp_enable;
	OTP_OPERATION		m_otp_op;
	OTP_T				m_otp;
	AnsiString			m_otp_filepath;

    // GPS ROM
    _BOOL               m_gps_used;
    _BOOL               m_gps_enable;
    _BOOL               m_gps_download;

    AnsiString          m_gps_rom_dirpath;
    AnsiString          m_gps_rom_filepath;
    AnsiString          m_gps_ini_filepath;

    unsigned int        m_gps_rom_filesize;

    unsigned int        m_gps_power_pin;
    unsigned int        m_gps_reset_pin;
    unsigned int        m_gps_standby_pin;
    unsigned int        m_gps_32k_pin;

} ParamDownloadConfig_T;

typedef struct {
	// CUST_PARA
	_BOOL				m_cust_para_used;
	_BOOL				m_cust_para_enable;
	CustPara_T			m_cust_para;
	AnsiString			m_cust_para_filepath;
	// SEC_RO
	_BOOL				m_sec_ro_used;
	_BOOL				m_sec_ro_enable;
	SecRO_T				m_sec_ro;
	AnsiString			m_sec_ro_filepath;
	// OTP
	_BOOL				m_otp_used;
	_BOOL				m_otp_enable;
	OTP_T				m_otp;
	AnsiString			m_otp_filepath;

    // GPS ROM
    _BOOL               m_gps_used;
    _BOOL               m_gps_enable;
    _BOOL               m_gps_download;

    AnsiString          m_gps_rom_dirpath;
    AnsiString          m_gps_rom_filepath;
    AnsiString          m_gps_ini_filepath;

    unsigned int        m_gps_rom_filesize;

    unsigned int        m_gps_power_pin;
    unsigned int        m_gps_reset_pin;
    unsigned int        m_gps_standby_pin;
    unsigned int        m_gps_32k_pin;

} ParamReadbackConfig_T;

#endif

