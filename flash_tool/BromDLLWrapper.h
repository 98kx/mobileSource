// DllWrapper.h
// : Wrapper class for BromDll.dll
//

#ifndef	_BROM_DLL_WRAPPER_H_
#define	_BROM_DLL_WRAPPER_H_


#include "gps_flashtool_handle.h"
#include "gps_flashtool.h"

//#define unsigned short	UINT;

enum WRAPPER_RETURN  
{		
	COMPLETE_SUCCESS = 0,	
	INIT_VALUE,
	ERROR_NOT_DECIDE,
	//ERROR_GENERAL,
	ERROR_WRONG_PARAM,	
	ERROR_DLL_NOT_LOAD,	
	ERROR_FUNC_INTERNAL_LOGIC,
	ERROR_SOMEWHERE_COMPLICATE_LOGIC,
	ERROR_SOMEWHERE_NONEXPECTED,
	//
	ERROR_FAIL_TO_LOAD_DLL,
	ERROR_FAIL_TO_GETPROCADDRESS,	
	ERROR_FAIL_TO_CREATE_IMAGEOBJ,
	ERROR_FAIL_TO_CREATE_DAOBJ,
	ERROR_FAIL_TO_CREATE_RBOBJ,
	ERROR_FAIL_TO_DESTROY_IMAGEOBJ,
	ERROR_FAIL_TO_DESTROY_DAOBJ,
	ERROR_FAIL_TO_DESTROY_RBOBJ,
	ERROR_FAIL_TO_SET_DA_STARTADDR,
	//
	ERROR_SCAT_INCORRECT_FORMAT,
	ERROR_SCAT_OFFSET_IS_NOT_WORD_ALIG,
	ERROR_SCAT_ADDR_IS_NOT_WORD_ALIGN,
	ERROR_SCAT_ADDR_IS_NOT_ASCENDING_ORDER,
	ERROR_SCAT_FAIL_GET_COUNT,
	ERROR_SCAT_FAIL_GET_INFO,
	ERROR_SCAT_FAIL_SETATTR,
	//
	ERROR_DL_FW_INVALID_FORMAT,
	//
	ERROR_FAIL_TO_SET_RB,	
	ERROR_RB_GET_COUNT,
	ERROR_RB_FAIL_SETATTR,
	//
	ERROR_FAIL_TO_SETPACKETLENGTH,
	ERROR_BOOT_STOP,
	ERROR_CALLBACK,
	ERROR_FAIL_TO_OPEN_COMPORT,	
	ERROR_FAIL_TO_SET_DLBLOCK,
	ERROR_FAIL_TO_DOWNLOAD_DA,
	ERROR_FAIL_TO_FLASHDOWNLOAD,
	ERROR_FAIL_TO_FLASHREADBACK,
	
	//
	ERROR_FAIL_TO_OPEN_DEBUG_LOG,
	ERROR_FAIL_TO_CLOSE_DEBUG_LOG,
	

};


//------------------------------------------------------------------------------
typedef struct tagCBFlashtool
{
	GPS_CALLBACK_BROM_WRITE_BUF_INIT	cb_download_da_init;
	void *								cb_download_da_init_arg;
	GPS_CALLBACK_BROM_WRITE_BUF			cb_download_da;
	void *								cb_download_da_arg;
	GPS_CALLBACK_BEFORE_PROCESS			cb_before_process;
	void *								cb_before_process_arg;
	//CALLBACK_SET_HIGHSPEED_BAUDRATE	cb_set_high_speed_baudrate;
	CALLBACK_IN_BROM_STAGE				cb_in_brom_stage;
} CallBackFlashtool, *LPCallBackFlashtool;

typedef struct tagCBDownload
{
	GPS_CALLBACK_FLASH_DOWNLOAD_INIT	cb_download_flash_init;
	void *								cb_download_flash_init_arg;
	GPS_CALLBACK_FLASH_DOWNLOAD			cb_download_flash;
	void *								cb_download_flash_arg;
	//CALLBACK_FLASH_FORMAT_INIT	cb_format_report_init;
	//CALLBACK_FLASH_FORMAT			cb_format_report;
} CallBackDownload, *LPCallBackDownload;

typedef struct tagCBReadback
{
	GPS_CALLBACK_FLASH_READBACK_INIT	cb_readback_flash_init;
	void *								cb_readback_flash_init_arg;
	GPS_CALLBACK_FLASH_READBACK			cb_readback_flash;
	void *								cb_readback_flash_arg;
} CallBackReadback, *LPCallBackReadback;



//------------------------------------------------------------------------------
#include "gps_flashtool.h"

class CBromDllWrapper
{
public:
	CBromDllWrapper(void);
public:
	~CBromDllWrapper(void);

public:
	WRAPPER_RETURN		fnLoadBromDLL(const char * pszFullPath);	//- 1. Load BromDLL library
	WRAPPER_RETURN		fnLoadDA(const char * pszFullPath);			//- 2. Load DA
	WRAPPER_RETURN		fnLoadImageByScatter(const char * pszImageFullPath, const char * pszScatterFullPath);				//- 3. Load Scatter file and Firmware
	WRAPPER_RETURN		fnDownloadFirmware(unsigned int szCOM, LPCallBackFlashtool pcbFlashtool, LPCallBackDownload pcbDownload, int * pStopflag);	//- 4. Download Firmware

	WRAPPER_RETURN		fnSetRBList(const char * pszRBFullPath, unsigned int szRBAddr, unsigned int szRBLen);
	WRAPPER_RETURN		fnReadbackFirmware(unsigned int szCOM, LPCallBackFlashtool pcbFlashtool, LPCallBackReadback pcbReadback, int * pStopflag);
	
	WRAPPER_RETURN 		fnDebugOn(const char * pszPath);
	WRAPPER_RETURN 		fnDebugOff(void);

protected:
	//- Download
	WRAPPER_RETURN		fnLoadImage(const char * pszFullPath);	
	WRAPPER_RETURN		fnSetByScatter(const char * pszFullPath);	

	WRAPPER_RETURN		fnSetDLPacketLength(UINT nPacketLength);		

	WRAPPER_RETURN		fnArgDownload(GPS_FLASHTOOL_DOWNLOAD_ARG& argDownload);	

	//- Readback
	WRAPPER_RETURN		fnSetRB(const char * pszRBFullPath, unsigned int szRBAddr, unsigned int szRBLen);

	WRAPPER_RETURN		fnSetRBPacketLength(UINT nPacketLength);
	
	WRAPPER_RETURN		fnArgReadback(GPS_FLASHTOOL_READBACK_ARG& argReadback);

	//- Common
	WRAPPER_RETURN		fnArgCOMPortConnect(COM_PORT_SETTING& comPortSetting, unsigned int szCOM);
	WRAPPER_RETURN		fnArgFlashtool(GPS_FLASHTOOL_ARG& argFlashtool);


protected:
	HMODULE				m_hBromdll;
	GPS_DA_HANDLE_T		m_DAHandle;
	GPS_DL_HANDLE_T		m_DLHandle;
	GPS_RB_HANDLE_T		m_RBHandle;

private:
#if 1
	LPCallBackFlashtool				m_pCbFlashTool;
	LPCallBackDownload				m_pCbDownload;
	LPCallBackReadback				m_pCbReadback;
#else
	CALLBACK_BROM_WRITE_BUF_INIT	m_cb_download_da_init;
	CALLBACK_BROM_WRITE_BUF			m_cb_download_da;
	CALLBACK_BEFORE_PROCESS			m_cb_before_process;
	//CALLBACK_SET_HIGHSPEED_BAUDRATE	m_cb_set_high_speed_baudrate;
	CALLBACK_IN_BROM_STAGE			m_cb_in_brom_stage;

	CALLBACK_FLASH_DOWNLOAD_INIT	m_cb_download_flash_init;
	CALLBACK_FLASH_DOWNLOAD			m_cb_download_flash;
	CALLBACK_FLASH_FORMAT_INIT		m_cb_format_report_init;
	CALLBACK_FLASH_FORMAT			m_cb_format_report;
#endif
};


#endif
