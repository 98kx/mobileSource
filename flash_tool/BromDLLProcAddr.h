// DllProcAddress.h
// : Helper file of DllWrapper.h
// : Get the exported DLL function address

#ifndef	_BROM_DLL_PROC_ADDR_H_
#define	_BROM_DLL_PROC_ADDR_H_

#include "BromDLLWrapper.h"

//- Create
WRAPPER_RETURN	gps_DA_Create(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject);
WRAPPER_RETURN	gps_DL_Create(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject);
WRAPPER_RETURN	gps_RB_Create(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject);
WRAPPER_RETURN	gps_DA_SetStartAddr(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject, UINT nStartAddr);

//- Load binary
WRAPPER_RETURN	gps_DA_LoadByFilepath(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject, const char * pszPath);
WRAPPER_RETURN	gps_DL_Rom_LoadByFilepath(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const char * pszPath);

//- Scatter file
WRAPPER_RETURN	gps_DL_LoadScatter(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const char * pszPath);
WRAPPER_RETURN	gps_DL_GetCount(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, UINT& nCount);
WRAPPER_RETURN	gps_DL_Rom_GetInfoAll(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, GPS_ROM_INFO* pRomInfo, const UINT nMaxRomCount);
WRAPPER_RETURN	gps_DL_Rom_SetEnableAttr(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const UINT nIndex, const BOOL bEnable);

//- Download
WRAPPER_RETURN	gps_FlashDownload(const HMODULE dllModule, COM_PORT_SETTING& comPortSetting, 
								GPS_FLASHTOOL_ARG& argFlashTool, GPS_FLASHTOOL_DOWNLOAD_ARG& argDownload, 
								int * pStopflag, GPS_DA_REPORT_T& daReport);

WRAPPER_RETURN	gps_DL_SetPacketLength(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject, const UINT nPacketLength);

//- SetRB
WRAPPER_RETURN	gps_RB_Append(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, 
							const char * pszPath, unsigned int szAddr, unsigned int szLen);
WRAPPER_RETURN	gps_RB_GetCount(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, UINT& nCount);							
WRAPPER_RETURN	gps_RB_SetEnableAttr(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, const UINT nIndex, const BOOL bEnable);

//- Readback
WRAPPER_RETURN	gps_FlashReadback(const HMODULE dllModule, COM_PORT_SETTING& comPortSetting, 
								  GPS_FLASHTOOL_ARG& argFlashTool, GPS_FLASHTOOL_READBACK_ARG& argReadback, 
								  int * pStopflag, GPS_DA_REPORT_T& daReport);

WRAPPER_RETURN	gps_RB_SetPacketLength(const HMODULE dllModule, GPS_RB_HANDLE_T& RBObject, const UINT nPacketLength);							
							
//- Destroy
WRAPPER_RETURN	gps_DA_Destroy(const HMODULE dllModule, GPS_DA_HANDLE_T& DAObject);
WRAPPER_RETURN	gps_DL_Destroy(const HMODULE dllModule, GPS_DL_HANDLE_T& ImageObject);
WRAPPER_RETURN	gps_RB_Destroy(const HMODULE dllModule, GPS_DL_HANDLE_T& RBObject);

//- Debug
WRAPPER_RETURN	gps_Brom_DebugOn(const HMODULE dllModule, const char * pszPath);
WRAPPER_RETURN	gps_Brom_DebugOff(const HMODULE dllModule);

#endif
