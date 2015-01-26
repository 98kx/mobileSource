// DllWrapper.cpp
// : Wrapper class for BromDll.dll
//


#include "BromDllWrapper.h"
#include "BromDllProcAddr.h"


CBromDllWrapper::CBromDllWrapper(void)
{
	m_hBromdll = NULL;
	m_DAHandle = NULL;
	m_DLHandle = NULL;

	m_pCbFlashTool = NULL;
	m_pCbDownload = NULL;
}

CBromDllWrapper::~CBromDllWrapper(void)
{
	if (m_hBromdll){
		//Free resource
		if (m_DAHandle) {
			::gps_DA_Destroy(m_hBromdll, m_DAHandle);
			m_DAHandle = NULL;
		}
	
		if (m_DLHandle) {
            ::gps_DL_Destroy(m_hBromdll, m_DLHandle);
			m_DLHandle = NULL;
		}
	
		::FreeLibrary(m_hBromdll);
		m_hBromdll = NULL;
    }
}//END_OF Dtor


// -----------------------------------------------------------------------------
WRAPPER_RETURN CBromDllWrapper::fnLoadBromDLL(const char * pszFullPath)
{
    m_hBromdll   = LoadLibrary(pszFullPath);

    if( NULL == m_hBromdll ){
		return ERROR_DLL_NOT_LOAD;
    }

    return COMPLETE_SUCCESS;
}//END_OF fnLoadBromDLL()


WRAPPER_RETURN CBromDllWrapper::fnLoadDA(const char * pszFullPath)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Create DA_HANDLE
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DA_Create(m_hBromdll, m_DAHandle))) {
		return returnValue;
	}
	if (NULL == m_DAHandle) {
		//Should have returned above
		return ERROR_FUNC_INTERNAL_LOGIC;
	}

	//- 2. Load DA
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DA_LoadByFilepath(m_hBromdll, m_DAHandle, pszFullPath))) {
		return returnValue;
	}

	//- 3. Set up relative info
	UINT nStartAddr = 0x00100800;
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DA_SetStartAddr(m_hBromdll, m_DAHandle, nStartAddr))) {
		return returnValue;
	}

    return COMPLETE_SUCCESS;
}//END_OF fnLoadDA()


WRAPPER_RETURN CBromDllWrapper::fnLoadImageByScatter(const char * pszImageFullPath, const char * pszScatterFullPath)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Create Image Object
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DL_Create(m_hBromdll, m_DLHandle))) {
		return returnValue;
	}
	if (NULL == m_DLHandle) {
		//Should have returned above
		return ERROR_FUNC_INTERNAL_LOGIC;
	}

	//- 2. Load Scatter file
	if (COMPLETE_SUCCESS != (returnValue = this->fnSetByScatter(pszImageFullPath))) {
		return returnValue;
	}

	//- 3. Load Image file
	if (COMPLETE_SUCCESS != (returnValue = this->fnLoadImage(pszScatterFullPath))) {
		return returnValue;
	}

    return COMPLETE_SUCCESS;
}//END_OF fnLoadImageByScatter()


WRAPPER_RETURN CBromDllWrapper::fnLoadImage(const char * pszFullPath)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Load DL Image
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DL_Rom_LoadByFilepath(m_hBromdll, m_DLHandle, pszFullPath))) {
		return returnValue;
	}
	return COMPLETE_SUCCESS;
}//END_OF fnLoadImage()


WRAPPER_RETURN CBromDllWrapper::fnSetByScatter(const char * pszFullPath)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. ImageObject must be created first
	if (NULL == m_DLHandle) {
		return ERROR_SOMEWHERE_NONEXPECTED;	//This function shouldn't be called! Check logic.
	}

	//- 2. Load scatter file
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DL_LoadScatter(m_hBromdll, m_DLHandle, pszFullPath))) {
		return returnValue;
	}

	//- 3. Get ROM number
	UINT nCount = 0;
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DL_GetCount(m_hBromdll, m_DLHandle, nCount))) {
		return returnValue;
	}
	if (nCount <= 0) {
		return ERROR_SCAT_INCORRECT_FORMAT;
	}

    //- 4. Setup ROM information
	for (UINT i = 0; i < nCount; i++)
	{
		if (COMPLETE_SUCCESS != (returnValue = ::gps_DL_Rom_SetEnableAttr(m_hBromdll, m_DLHandle, i, TRUE))) {
			return returnValue;
		}
	}

	return COMPLETE_SUCCESS;
}//END_OF fnLoadImage()


WRAPPER_RETURN CBromDllWrapper::fnDownloadFirmware(unsigned int szCOM, LPCallBackFlashtool pcbFlashtool, LPCallBackDownload pcbDownload, int * pStopflag)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }
	if (! pcbFlashtool || ! pcbDownload) {
		return ERROR_WRONG_PARAM;
	}

	m_pCbFlashTool = pcbFlashtool;
	m_pCbDownload = pcbDownload;
	
	//-- 0. Update package Length (DL, RB only)
	const UINT nPacketLength = 1024;
	if (COMPLETE_SUCCESS != this->fnSetDLPacketLength(nPacketLength)){
		return ERROR_FAIL_TO_SETPACKETLENGTH;
	}

	//-- 1. Handle parameter
	//-- (COM Port)
	COM_PORT_SETTING comPortSetting;
	if (COMPLETE_SUCCESS != this->fnArgCOMPortConnect(comPortSetting, szCOM)){
		return ERROR_NOT_DECIDE;
	}

	//-- (FLAHSTOOL_ARG)
	GPS_FLASHTOOL_ARG argFlashTool;
	memset(&argFlashTool, 0, sizeof(argFlashTool));
	if (COMPLETE_SUCCESS != this->fnArgFlashtool(argFlashTool)){
		return ERROR_NOT_DECIDE;
	}

	//-- (FLAHSTOOL_DOWNLOAD_ARG)
	GPS_FLASHTOOL_DOWNLOAD_ARG argDownload;
	memset(&argDownload, 0, sizeof(argDownload));
	if (COMPLETE_SUCCESS != this->fnArgDownload(argDownload)){
		return ERROR_NOT_DECIDE;
	}

	//-- 2. Download and check stop flag
	*pStopflag = 0;
	GPS_DA_REPORT_T	daReport;
    WRAPPER_RETURN returnValue;

	if (COMPLETE_SUCCESS != (returnValue = ::gps_FlashDownload(m_hBromdll, comPortSetting, argFlashTool, argDownload, pStopflag, daReport))) {
		return returnValue;
	}

	if (BOOT_STOP == *pStopflag ) {
		return ERROR_BOOT_STOP;	//why?
	}

	return COMPLETE_SUCCESS;
}//END_OF fnDownloadFirmware()


//------------------------------------------------------------------------------

WRAPPER_RETURN CBromDllWrapper::fnSetRBList(const char * pszRBFullPath, unsigned int szRBAddr, unsigned int szRBLen)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Create Image Object
	if (COMPLETE_SUCCESS != (returnValue = ::gps_RB_Create(m_hBromdll, m_RBHandle))) {
		return returnValue;
	}
	if (NULL == m_RBHandle) {
		//Should have returned above
		return ERROR_FUNC_INTERNAL_LOGIC;
	}

	//- 2. Setup RB file
	if (COMPLETE_SUCCESS != (returnValue = this->fnSetRB(pszRBFullPath,szRBAddr,szRBLen))) {
		return returnValue;
	}

	//- 3. Get RB number
	UINT nCount = 0;
	if (COMPLETE_SUCCESS != (returnValue = ::gps_RB_GetCount(m_hBromdll, m_RBHandle, nCount))) {
		return returnValue;
	}
	if (nCount <= 0) {
		return ERROR_RB_GET_COUNT;
	}

    //- 4. Setup RB information
	for (UINT i = 0; i < nCount; i++)
	{
		if (COMPLETE_SUCCESS != (returnValue = ::gps_RB_SetEnableAttr(m_hBromdll, m_RBHandle, i, TRUE))) {
			return returnValue;
		}
	}	

    return COMPLETE_SUCCESS;
}//END_OF fnSetRBList()

WRAPPER_RETURN CBromDllWrapper::fnSetRB(const char * pszRBFullPath, unsigned int szRBAddr, unsigned int szRBLen)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Setup RB file
	if (COMPLETE_SUCCESS != (returnValue = ::gps_RB_Append(m_hBromdll, m_RBHandle, pszRBFullPath,szRBAddr,szRBLen))) {
		return returnValue;
	}
	
	return COMPLETE_SUCCESS;
}//END_OF fnSetRB()

WRAPPER_RETURN CBromDllWrapper::fnReadbackFirmware(unsigned int szCOM, LPCallBackFlashtool pcbFlashtool, LPCallBackReadback pcbReadback, int * pStopflag)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }
	if (! pcbFlashtool || ! pcbReadback) {
		return ERROR_WRONG_PARAM;
	}

	m_pCbFlashTool	= pcbFlashtool;
	m_pCbReadback 	= pcbReadback;
	
	//-- 0. Update package Length (DL, RB only)
	const UINT nPacketLength = 1024;
	if (COMPLETE_SUCCESS != this->fnSetRBPacketLength(nPacketLength)){
		return ERROR_FAIL_TO_SETPACKETLENGTH;
	}

	//-- 1. Handle parameter
	//-- (COM Port)
	COM_PORT_SETTING comPortSetting;
	if (COMPLETE_SUCCESS != this->fnArgCOMPortConnect(comPortSetting, szCOM)){
		return ERROR_NOT_DECIDE;
	}

	//-- (FLAHSTOOL_ARG)
	GPS_FLASHTOOL_ARG argFlashTool;
	memset(&argFlashTool, 0, sizeof(argFlashTool));
	if (COMPLETE_SUCCESS != this->fnArgFlashtool(argFlashTool)){
		return ERROR_NOT_DECIDE;
	}

	//-- (FLASHTOOL_READBACK_ARG)
	GPS_FLASHTOOL_READBACK_ARG argReadback;
	memset(&argReadback, 0, sizeof(argReadback));
	if (COMPLETE_SUCCESS != this->fnArgReadback(argReadback)){
		return ERROR_NOT_DECIDE;
	}

	//-- 2. Readback and check stop flag
	*pStopflag = 0;
	GPS_DA_REPORT_T	daReport;
    WRAPPER_RETURN returnValue;

	if (COMPLETE_SUCCESS != (returnValue = ::gps_FlashReadback(m_hBromdll, comPortSetting, argFlashTool, argReadback, pStopflag, daReport))) {
		return returnValue;
	}

	if (BOOT_STOP == *pStopflag ) {
		return ERROR_BOOT_STOP;	//why?
	}

	return COMPLETE_SUCCESS;
}//END_OF fnReadbackFirmware()

//------------------------------------------------------------------------------
WRAPPER_RETURN CBromDllWrapper::fnSetDLPacketLength(UINT nPacketLength)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Setup DL Packet Length
	if (COMPLETE_SUCCESS != (returnValue = ::gps_DL_SetPacketLength(m_hBromdll, m_DLHandle, nPacketLength))) {
		return returnValue;
	}
	return COMPLETE_SUCCESS;
}//END_OF fnSetDLPacketLength()

WRAPPER_RETURN CBromDllWrapper::fnSetRBPacketLength(UINT nPacketLength)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. Setup RB Packet Length
	if (COMPLETE_SUCCESS != (returnValue = ::gps_RB_SetPacketLength(m_hBromdll, m_RBHandle, nPacketLength))) {
		return returnValue;
	}
	return COMPLETE_SUCCESS;
}//END_OF fnSetRBPacketLength()

//------------------------------------------------------------------------------
WRAPPER_RETURN CBromDllWrapper::fnArgCOMPortConnect(COM_PORT_SETTING& comPortSetting, unsigned int szCOM)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

	comPortSetting.com.number = szCOM;   //2;//"COM1:";
	//comPortSetting.com.handle = xx;

	comPortSetting.baudrate = UART_BAUD_115200; // GPS Module device in Phone using 115200 baudrate
	//comPortSetting.baudrate = UART_BAUD_9600;	//BBK WinCE42 device using 9600 baudrate
	comPortSetting.ms_read_timeout = COM_DEFAULT_TIMEOUT;	//flashtool.h
	comPortSetting.ms_write_timeout = COM_DEFAULT_TIMEOUT;

	return COMPLETE_SUCCESS;
}//END_OF fnArgCOMPortConnect()


WRAPPER_RETURN CBromDllWrapper::fnArgFlashtool(GPS_FLASHTOOL_ARG& argFlashtool)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

	argFlashtool.bbchip					        = MT3301;	//mtk_mcu.h
	argFlashtool.ext_clock				        = EXT_26M;	//mtk_mcu.h
	argFlashtool.p_bank0_mem_cfg		        = NULL;	//&g_EMI_Config;	//0x00004102
	argFlashtool.p_bank1_mem_cfg		        = NULL;	//&g_EMI_Config;

	argFlashtool.da_handle				        = m_DAHandle;

	argFlashtool.cb_download_da_init	        = m_pCbFlashTool->cb_download_da_init;
	argFlashtool.cb_download_da_init_arg        = m_pCbFlashTool->cb_download_da_init_arg;
	argFlashtool.cb_download_da			        = m_pCbFlashTool->cb_download_da;
	argFlashtool.cb_download_da_arg		        = m_pCbFlashTool->cb_download_da_arg;
	argFlashtool.cb_before_process		        = NULL;    //m_pCbFlashTool->cb_before_process;
	argFlashtool.cb_before_process_arg	        = this;
	argFlashtool.cb_set_high_speed_baudrate     = NULL;
	argFlashtool.cb_set_high_speed_baudrate_arg = NULL;
	argFlashtool.ms_boot_timeout				= BOOT_INFINITE;
	argFlashtool.max_start_cmd_retry_count		= DEFAULT_BROM_START_CMD_RETRY_COUNT; //1
	argFlashtool.cb_in_brom_stage				= NULL;     //m_pCbFlashTool->cb_in_brom_stage;
	argFlashtool.cb_in_brom_stage_arg			= &(argFlashtool.bbchip);
	argFlashtool.m_baudrate_full_sync_count		= 1;	//Why this value?
	argFlashtool.m_speedup_brom_baudrate		= _TRUE;

	return COMPLETE_SUCCESS;
}//END_OF fnArgFlashtool()

WRAPPER_RETURN CBromDllWrapper::fnArgDownload(GPS_FLASHTOOL_DOWNLOAD_ARG& argDownload)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

	argDownload.dl_handle					= m_DLHandle;
	argDownload.download_accuracy			= ACCURACY_AUTO;
	argDownload.cb_download_flash_init		= m_pCbDownload->cb_download_flash_init;
	argDownload.cb_download_flash_init_arg	= m_pCbDownload->cb_download_flash_init_arg;//this;
	argDownload.cb_download_flash			= m_pCbDownload->cb_download_flash;
	argDownload.cb_download_flash_arg		= m_pCbDownload->cb_download_flash_arg;		//this;

	//argDownload.cb_format_report_init		= m_pCbDownload->cb_format_report_init;	//No format
	//argDownload.cb_format_report_init_arg = this;
	//argDownload.cb_format_report			= m_pCbDownload->cb_format_report;
	//argDownload.cb_format_report_arg		= this;
	argDownload.enable_tgt_res_layout_check = _TRUE;
	argDownload.enable_bbchip_eco_check		= _TRUE;

	return COMPLETE_SUCCESS;
}//END_OF fnArgDownload()

WRAPPER_RETURN CBromDllWrapper::fnArgReadback(GPS_FLASHTOOL_READBACK_ARG& argReadback)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

	argReadback.rb_handle					= m_RBHandle;
	argReadback.readback_accuracy			= ACCURACY_AUTO;
	argReadback.cb_readback_flash_init		= m_pCbReadback->cb_readback_flash_init;
	argReadback.cb_readback_flash_init_arg	= m_pCbReadback->cb_readback_flash_init_arg;
	argReadback.cb_readback_flash			= m_pCbReadback->cb_readback_flash;
	argReadback.cb_readback_flash_arg		= m_pCbReadback->cb_readback_flash_arg;
//	argReadback.rb_info						=;

	return COMPLETE_SUCCESS;
}//END_OF fnArgDownload()


WRAPPER_RETURN CBromDllWrapper::fnDebugOn(const char * pszPath)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. DebugOn
	if (COMPLETE_SUCCESS != (returnValue = ::gps_Brom_DebugOn(m_hBromdll,pszPath))) {
		return returnValue;
	}
	return COMPLETE_SUCCESS;
}//END_OF fnDebugOn()

WRAPPER_RETURN CBromDllWrapper::fnDebugOff(void)
{
    if( !m_hBromdll ){
        return ERROR_DLL_NOT_LOAD;
    }

    WRAPPER_RETURN returnValue;

	//- 1. DebugOff
	if (COMPLETE_SUCCESS != (returnValue = ::gps_Brom_DebugOff(m_hBromdll))) {
		return returnValue;
	}
	return COMPLETE_SUCCESS;
}//END_OF fnDebugOn()

