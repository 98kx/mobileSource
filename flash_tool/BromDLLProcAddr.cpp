// DllProcAddress.h
// : Helper file of DllWrapper.h
// : Get the exported DLL function address

#include "BromDllProcAddr.h"

//------------------------------------------------------------------------------
WRAPPER_RETURN	gps_DA_Create(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_DA_Create) (GPS_DA_HANDLE_T* p_da_handle );
	PFN_DA_Create GPS_DA_Create = (PFN_DA_Create) GetProcAddress(dllModule, "GPS_DA_Create");
	if (NULL == GPS_DA_Create) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DA_Create(&DAObject)) {
		return ERROR_FAIL_TO_CREATE_DAOBJ;
	}
	return COMPLETE_SUCCESS;
}//END_OF gps_DA_Create()


WRAPPER_RETURN	gps_DL_Create(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_DL_Create) (GPS_DL_HANDLE_T* p_dl_handle);
	PFN_DL_Create GPS_DL_Create = (PFN_DL_Create) GetProcAddress(dllModule, "GPS_DL_Create");
	if (NULL == GPS_DL_Create) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DL_Create(&ImageObject)) {
		return ERROR_FAIL_TO_CREATE_IMAGEOBJ;
	}
	return COMPLETE_SUCCESS;
}//END_OF gps_DL_Create()

WRAPPER_RETURN	gps_RB_Create(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_RB_Create) (GPS_RB_HANDLE_T* p_rb_handle);
	PFN_RB_Create GPS_RB_Create = (PFN_RB_Create) GetProcAddress(dllModule, "GPS_RB_Create");
	if (NULL == GPS_RB_Create) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_RB_Create(&RBObject)) {
		return ERROR_FAIL_TO_CREATE_RBOBJ;
	}
	return COMPLETE_SUCCESS;
}//END_OF gps_RB_Create()

WRAPPER_RETURN	gps_DA_SetStartAddr(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject, UINT nStartAddr)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_DA_SetStartAddr) (GPS_DA_HANDLE_T da_handle, unsigned int da_start_addr);
	PFN_DA_SetStartAddr GPS_DA_SetStartAddr = (PFN_DA_SetStartAddr) GetProcAddress(dllModule, "GPS_DA_SetStartAddr");
	if (NULL == GPS_DA_SetStartAddr) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DA_SetStartAddr(DAObject, (unsigned int) nStartAddr)) {
		return ERROR_FAIL_TO_SET_DA_STARTADDR;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DA_SetStartAddr()


//------------------------------------------------------------------------------
WRAPPER_RETURN	gps_DA_LoadByFilepath(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject, const char * pszPath)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_DA_LoadByFilepath) (GPS_DA_HANDLE_T da_handle, const char *da_filepath);
	PFN_DA_LoadByFilepath GPS_DA_LoadByFilepath = (PFN_DA_LoadByFilepath) GetProcAddress(dllModule, "GPS_DA_LoadByFilepath");
	if (NULL == GPS_DA_LoadByFilepath) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DA_LoadByFilepath(DAObject, pszPath)) {
		return ERROR_FAIL_TO_CREATE_DAOBJ;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DA_LoadByFilepath()


WRAPPER_RETURN	gps_DL_Rom_LoadByFilepath(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const char * pszPath)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_DL_Rom_LoadByFilepath) (GPS_DL_HANDLE_T dl_handle, unsigned short index, const char *filepath);
	PFN_DL_Rom_LoadByFilepath GPS_DL_Rom_LoadByFilepath = (PFN_DL_Rom_LoadByFilepath) GetProcAddress(dllModule, "GPS_DL_Rom_LoadByFilepath");
	if (NULL == GPS_DL_Rom_LoadByFilepath) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

    int nRet;
	if (COMPLETE_SUCCESS != (nRet = GPS_DL_Rom_LoadByFilepath(ImageObject, 0, pszPath)) ) {	//Note: We just have one ROM in the list
		return ERROR_DL_FW_INVALID_FORMAT;
	}

	return (WRAPPER_RETURN)nRet;
}//END_OF gps_DL_Rom_LoadByFilepath()


//------------------------------------------------------------------------------
WRAPPER_RETURN	gps_DL_LoadScatter(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const char * pszPath)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_DL_LoadScatter) (GPS_DL_HANDLE_T dl_handle, const char* scatter_filepath);
	PFN_DL_LoadScatter GPS_DL_LoadScatter = (PFN_DL_LoadScatter)GetProcAddress(dllModule, "GPS_DL_LoadScatter");
	if (NULL == GPS_DL_LoadScatter) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	int nRet;
	if (COMPLETE_SUCCESS != (nRet = GPS_DL_LoadScatter(ImageObject, pszPath)))
	{
		switch (nRet)
		{
		case FTHND_INVALID_ARGUMENTS:
			return ERROR_WRONG_PARAM;
		case FTHND_SCAT_INCORRECT_FORMAT:
			return ERROR_SCAT_INCORRECT_FORMAT;
		case FTHND_SCAT_OFFSET_IS_NOT_WORD_ALIGN:
			return ERROR_SCAT_OFFSET_IS_NOT_WORD_ALIG;
		case FTHND_SCAT_ADDR_IS_NOT_WORD_ALIGN:
			return ERROR_SCAT_ADDR_IS_NOT_WORD_ALIGN;
		case FTHND_SCAT_ADDR_IS_NOT_ASCENDING_ORDER:
			return ERROR_SCAT_ADDR_IS_NOT_ASCENDING_ORDER;
		case FTHND_HANDLE_BUSY_NOW:
			//What?
		default:
			return ERROR_SOMEWHERE_NONEXPECTED;
		}
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_LoadScatter()


WRAPPER_RETURN	gps_DL_GetCount(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, UINT& nCount)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}
	
	typedef int (WINAPI* PFN_DL_GetCount) (GPS_DL_HANDLE_T dl_handle, unsigned short* p_rom_count);
	PFN_DL_GetCount GPS_DL_GetCount = (PFN_DL_GetCount) GetProcAddress(dllModule, "GPS_DL_GetCount");
	if (NULL == GPS_DL_GetCount) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DL_GetCount(ImageObject, (unsigned short *)&nCount)) {
		return ERROR_SCAT_FAIL_GET_COUNT;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_GetCount()


WRAPPER_RETURN	gps_DL_Rom_GetInfoAll(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, GPS_ROM_INFO* pRomInfo, const UINT nMaxRomCount)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_DL_Rom_GetInfoAll) (GPS_DL_HANDLE_T dl_handle, GPS_ROM_INFO* p_rom_info, unsigned short max_rom_count);
	PFN_DL_Rom_GetInfoAll GPS_DL_Rom_GetInfoAll = (PFN_DL_Rom_GetInfoAll) GetProcAddress(dllModule, "GPS_DL_Rom_GetInfoAll");
	if (NULL == GPS_DL_Rom_GetInfoAll) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DL_Rom_GetInfoAll(ImageObject, pRomInfo, nMaxRomCount)) {
		return ERROR_SCAT_FAIL_GET_INFO;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_Rom_GetInfoAll()


WRAPPER_RETURN	gps_DL_Rom_SetEnableAttr(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const UINT nIndex, const BOOL bEnable)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_DL_Rom_SetEnableAttr) (GPS_DL_HANDLE_T dl_handle, unsigned short index, _BOOL attr);
	PFN_DL_Rom_SetEnableAttr GPS_DL_Rom_SetEnableAttr = (PFN_DL_Rom_SetEnableAttr) GetProcAddress(dllModule, "GPS_DL_Rom_SetEnableAttr");
	if (NULL == GPS_DL_Rom_SetEnableAttr) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DL_Rom_SetEnableAttr(ImageObject, nIndex, (_BOOL)bEnable)) {
		return ERROR_SCAT_FAIL_SETATTR;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_Rom_SetEnableAttr()


//------------------------------------------------------------------------------
WRAPPER_RETURN	gps_FlashDownload(const HMODULE dllModule, COM_PORT_SETTING& comPortSetting, 
								  GPS_FLASHTOOL_ARG& argFlashTool, GPS_FLASHTOOL_DOWNLOAD_ARG& argDownload, 
								  int * pStopflag, GPS_DA_REPORT_T& daReport)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_FlashDownload) (COM_PORT_SETTING *p_com_setting, GPS_FLASHTOOL_ARG *p_arg, GPS_FLASHTOOL_DOWNLOAD_ARG *p_dl_arg, int *p_stopflag, GPS_DA_REPORT_T *p_da_report);
	PFN_FlashDownload GPS_FlashDownload = (PFN_FlashDownload) GetProcAddress(dllModule, "GPS_FlashDownload");
	if (NULL == GPS_FlashDownload) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	int nRet;
	if (COMPLETE_SUCCESS != (nRet = GPS_FlashDownload(&comPortSetting, &argFlashTool, &argDownload, pStopflag, &daReport))) 
	{
		switch (FT_RET(nRet)) 
		{
		case FT_OK:
			return COMPLETE_SUCCESS;
		case FT_INVALID_ARGUMENTS:
			return ERROR_WRONG_PARAM;
		case FT_COM_PORT_OPEN_ERR:
			return ERROR_FAIL_TO_OPEN_COMPORT;
		case FT_CALLBACK_ERROR:
			return ERROR_CALLBACK;
		case FT_DA_SET_DOWNLOAD_BLOCK_FAIL:
			return ERROR_FAIL_TO_SET_DLBLOCK;
		case FT_DA_DOWNLOAD_FAIL:
			return ERROR_FAIL_TO_DOWNLOAD_DA;
		default:
			//Check ret value
			return ERROR_FAIL_TO_FLASHDOWNLOAD;
		}
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_FlashDownload()


WRAPPER_RETURN	gps_DL_SetPacketLength(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const UINT nPacketLength)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_DL_SetPacketLength) (GPS_DL_HANDLE_T lpBaseAddress, unsigned short PacketLength);
	PFN_DL_SetPacketLength GPS_DL_SetPacketLength = (PFN_DL_SetPacketLength) GetProcAddress(dllModule, "GPS_DL_SetPacketLength");
	if (NULL == GPS_DL_SetPacketLength) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DL_SetPacketLength(ImageObject, nPacketLength)) {
		return ERROR_FAIL_TO_DESTROY_DAOBJ;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_SetPacketLength()


//------------------------------------------------------------------------------
WRAPPER_RETURN	gps_RB_Append(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, const char * pszPath, unsigned int szAddr, unsigned int szLen)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_RB_Append) (GPS_RB_HANDLE_T  rb_handle, const char *rb_filepath, unsigned int rb_addr, unsigned int rb_len);
	PFN_RB_Append GPS_RB_Append = (PFN_RB_Append) GetProcAddress(dllModule, "GPS_RB_Append");
	if (NULL == GPS_RB_Append) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_RB_Append(RBObject, pszPath, szAddr, szLen)) {	//Note: We just have one RB in the list
		return ERROR_FAIL_TO_SET_RB;
	}
	
	return COMPLETE_SUCCESS;
}//END_OF gps_RB_Append()

WRAPPER_RETURN	gps_RB_GetCount(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, UINT& nCount)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}
	
	typedef int (WINAPI* PFN_RB_GetCount) (GPS_RB_HANDLE_T dl_handle, unsigned short* p_rb_count);
	PFN_RB_GetCount GPS_RB_GetCount = (PFN_RB_GetCount) GetProcAddress(dllModule, "GPS_RB_GetCount");
	if (NULL == GPS_RB_GetCount) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_RB_GetCount(RBObject, (unsigned short *)&nCount)) {
		return ERROR_RB_GET_COUNT;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_RB_GetCount()

WRAPPER_RETURN	gps_RB_SetEnableAttr(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, const UINT nIndex, const BOOL bEnable)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_RB_SetEnableAttr) (GPS_RB_HANDLE_T rb_handle, unsigned short index, _BOOL attr);
	PFN_RB_SetEnableAttr GPS_RB_SetEnableAttr = (PFN_RB_SetEnableAttr) GetProcAddress(dllModule, "GPS_RB_SetEnableAttr");
	if (NULL == GPS_RB_SetEnableAttr) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_RB_SetEnableAttr(RBObject, nIndex, (_BOOL)bEnable)) {
		return ERROR_RB_FAIL_SETATTR;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_RB_SetEnableAttr()

WRAPPER_RETURN	gps_RB_SetPacketLength(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, const UINT nPacketLength)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_RB_SetPacketLength) (GPS_RB_HANDLE_T lpBaseAddress, unsigned short PacketLength);
	PFN_RB_SetPacketLength GPS_RB_SetPacketLength = (PFN_RB_SetPacketLength) GetProcAddress(dllModule, "GPS_RB_SetPacketLength");
	if (NULL == GPS_RB_SetPacketLength) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_RB_SetPacketLength(RBObject, nPacketLength)) {
		return ERROR_FAIL_TO_SETPACKETLENGTH;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_RB_SetPacketLength()

WRAPPER_RETURN	gps_FlashReadback(const HMODULE dllModule, COM_PORT_SETTING& comPortSetting, 
								  GPS_FLASHTOOL_ARG& argFlashTool, GPS_FLASHTOOL_READBACK_ARG& argReadback, 
								  int * pStopflag, GPS_DA_REPORT_T& daReport)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI* PFN_FlashReadback) (COM_PORT_SETTING *p_com_setting, GPS_FLASHTOOL_ARG *p_arg, GPS_FLASHTOOL_READBACK_ARG *p_rb_arg, int *p_stopflag, GPS_DA_REPORT_T *p_da_report);
	PFN_FlashReadback GPS_FlashReadback = (PFN_FlashReadback) GetProcAddress(dllModule, "GPS_FlashReadback");
	if (NULL == GPS_FlashReadback) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	int nRet;
	if (COMPLETE_SUCCESS != (nRet = GPS_FlashReadback(&comPortSetting, &argFlashTool, &argReadback, pStopflag, &daReport))) 
	{
		switch (FT_RET(nRet)) 
		{
		case FT_OK:
			return COMPLETE_SUCCESS;
		case FT_INVALID_ARGUMENTS:
			return ERROR_WRONG_PARAM;
		case FT_COM_PORT_OPEN_ERR:
			return ERROR_FAIL_TO_OPEN_COMPORT;
		case FT_CALLBACK_ERROR:
			return ERROR_CALLBACK;
		case FT_DA_SET_DOWNLOAD_BLOCK_FAIL:
			return ERROR_FAIL_TO_SET_DLBLOCK;
		case FT_DA_DOWNLOAD_FAIL:
			return ERROR_FAIL_TO_DOWNLOAD_DA;
		default:
			//Check ret value
			return ERROR_FAIL_TO_FLASHREADBACK;
		}
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_FlashDownload()

//------------------------------------------------------------------------------


WRAPPER_RETURN	gps_DA_Destroy(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_DA_Destroy) (GPS_DA_HANDLE_T* p_da_handle);
	PFN_DA_Destroy GPS_DA_Destroy = (PFN_DA_Destroy) GetProcAddress(dllModule, "GPS_DA_Destroy");
	if (NULL == GPS_DA_Destroy) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DA_Destroy(&DAObject)) {
		return ERROR_FAIL_TO_DESTROY_DAOBJ;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DA_Destroy()

WRAPPER_RETURN	gps_DL_Destroy(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_DL_Destroy) (GPS_DL_HANDLE_T* p_dl_handle);
	PFN_DL_Destroy GPS_DL_Destroy = (PFN_DL_Destroy) GetProcAddress(dllModule, "GPS_DL_Destroy");
	if (NULL == GPS_DL_Destroy) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_DL_Destroy(&ImageObject)) {
		return ERROR_FAIL_TO_DESTROY_IMAGEOBJ;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_Destroy()

WRAPPER_RETURN	gps_RB_Destroy(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_RB_Destroy) (GPS_RB_HANDLE_T* p_rb_handle);
	PFN_RB_Destroy GPS_RB_Destroy = (PFN_RB_Destroy) GetProcAddress(dllModule, "GPS_RB_Destroy");
	if (NULL == GPS_RB_Destroy) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_RB_Destroy(&RBObject)) {
		return ERROR_FAIL_TO_DESTROY_RBOBJ;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_DL_Destroy()

//------------------------------------------------------------------------------
WRAPPER_RETURN	gps_Brom_DebugOn(const HMODULE dllModule, const char * pszPath)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_Brom_DebugOn) (const char * pszPath);  //- setup log path for debug
	PFN_Brom_DebugOn GPS_Brom_DebugOn = (PFN_Brom_DebugOn) GetProcAddress(dllModule, "GPS_Brom_DebugOn");
	if (NULL == GPS_Brom_DebugOn) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_Brom_DebugOn(pszPath)) {
		return ERROR_FAIL_TO_OPEN_DEBUG_LOG;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_Brom_DebugOn()

WRAPPER_RETURN	gps_Brom_DebugOff(const HMODULE dllModule)
{
	if (! dllModule) {
		return ERROR_DLL_NOT_LOAD;
	}

	typedef int (WINAPI *PFN_Brom_DebugOff) (void);
	PFN_Brom_DebugOff GPS_Brom_DebugOff = (PFN_Brom_DebugOff) GetProcAddress(dllModule, "GPS_Brom_DebugOff");
	if (NULL == GPS_Brom_DebugOff) {
		return ERROR_FAIL_TO_GETPROCADDRESS;
	}

	if (COMPLETE_SUCCESS != GPS_Brom_DebugOff()) {
		return ERROR_FAIL_TO_CLOSE_DEBUG_LOG;
	}

	return COMPLETE_SUCCESS;
}//END_OF gps_Brom_DebugOff()

