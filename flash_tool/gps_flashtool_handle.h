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
 *  flashtool_handle.h
 *
 * Project:
 * --------
 *  Flash Download/Readback/Format Library.
 *
 * Description:
 * ------------
 *  The exported C interface APIs to deal with FlashTool handles.
 *
 * Author:
 * -------
 *  Amos Hsu (mtk00539)
 *
 *******************************************************************************/
#ifndef	_GPS_FLASHTOOL_HANDLE_H_
#define	_GPS_FLASHTOOL_HANDLE_H_

#include "flashtool_handle.h"

#include "gps_brom.h"
#include "gps_mtk_resource.h"

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// return code                                                                  
//------------------------------------------------------------------------------

#define FTHND_RET(ret)	(ret&0x0000FF00)

#define FTHND_OK									0x000000
#define FTHND_ERROR									0x000100
#define FTHND_NO_MEMORY								0x000200
#define FTHND_INVALID_ARGUMENTS						0x000300
#define FTHND_ILLEGAL_INDEX							0x000400
#define FTHND_HANDLE_BUSY_NOW						0x000500
#define FTHND_FILE_IS_UPDATED						0x000600
#define FTHND_FILE_IS_NOT_LOADED					0x000700
#define FTHND_FILE_LOAD_FAIL						0x000800
#define FTHND_FILE_UNLOAD_FAIL						0x000900
#define FTHND_LIST_IS_EMPTY							0x000A00
#define FTHND_SCAT_INCORRECT_FORMAT					0x000B00
#define FTHND_SCAT_ADDR_IS_NOT_WORD_ALIGN			0x000C00
#define FTHND_SCAT_OFFSET_IS_NOT_WORD_ALIGN			0x000D00
#define FTHND_SCAT_ADDR_IS_NOT_ASCENDING_ORDER		0x000E00
#define FTHND_SCAT_REGION_IS_OVERLAP				0x000F00
#define FTHND_SCAT_JUMPTABLE_IS_NOT_ABSOLUTE_ADDR	0x001000
#define FTHND_SCAT_NOT_RES_LOAD_REGION_INDEX		0x001100
#define FTHND_RES_MECHANISM_DISABLED				0x001200
#define FTHND_RES_NO_MATCH_IN_JUMPTABLE				0x001300
#define FTHND_RES_IS_NOT_DOWNLOAD_WITH_JUMPTABLE	0x001400
#define FTHND_RES_DOWNLOAD_SELECT_INCOMPLETE		0x001500
#define FTHND_RES_INCORRECT_FORMAT					0x001600
#define FTHND_JUMPTABLE_INCONSISTENT_WITH_SCAT		0x001700
#define FTHND_JUMPTABLE_INCORRECT_FORMAT			0x001800
#define FTHND_SCAT_ABSOLUTE_REGION_ADDR_CHANGED		0x001900
#define FTHND_UNKNOWN_BBCHIP_TYPE					0x001A00
#define FTHND_UNKNOWN_CUSTOM_NAME					0x001B00
#define FTHND_INPUT_BUFFER_TOO_SMALL				0x001C00
#define FTHND_NO_MATCHED_DA_FOUND					0x001D00
#define FTHND_RES_BASE_ADDR_NOT_MATCH				0x001E00

#define FTHND_FILE_LOAD_NOT_VALID					0x001F00

//------------------------------------------------------------------------------
// GPS_DL_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------

typedef struct {
	char 				name[64];
	unsigned int 		region_addr;
	REGION_ADDR_TYPE	addr_type;	// ABSOLUTE_ADDR -> region_addr is absolute address 
									// OFFSET_VALUE  -> region_addr is offset value 
	unsigned int 		begin_addr;
	unsigned int 		end_addr;
	char 				filepath[256];
	unsigned int 		filesize;
	unsigned short 		index;

	ROM_TYPE			rom_type;

	_BOOL				enable;	// _FALSE -> disbale 
								// _TRUE  -> enable  
}GPS_ROM_INFO;

struct GPS_DL_HANDLE;
typedef struct GPS_DL_HANDLE *	GPS_DL_HANDLE_T;
extern int __stdcall GPS_DL_Create(GPS_DL_HANDLE_T  *p_dl_handle);
extern int __stdcall GPS_DL_Destroy(GPS_DL_HANDLE_T  *p_dl_handle);
extern int __stdcall GPS_DL_LoadScatter(GPS_DL_HANDLE_T  dl_handle, const char *scatter_filepath);
extern int __stdcall GPS_DL_SetPacketLength(GPS_DL_HANDLE_T  dl_handle, unsigned short length);
extern int __stdcall GPS_DL_GetCount(GPS_DL_HANDLE_T  dl_handle, unsigned short *p_rom_count);
extern int __stdcall GPS_DL_IsReady(GPS_DL_HANDLE_T  dl_handle, GPS_ROM_INFO  *p_rom_info, _BOOL  check_if_updated);
extern int __stdcall GPS_DL_GetJumpTableInfo(GPS_DL_HANDLE_T  dl_handle, GPS_MTK_JumpTable  *p_jmptbl_info);
extern int __stdcall GPS_DL_ResourceProjectIdComparisonSetting(GPS_DL_HANDLE_T  dl_handle, _BOOL  enable);
extern int __stdcall GPS_DL_AutoLoadByScatRegionName(GPS_DL_HANDLE_T  dl_handle, const char *full_filepath, unsigned char allow_rom_type);
extern int __stdcall GPS_DL_GetBBChipType(GPS_DL_HANDLE_T  dl_handle, GPS_BBCHIP_TYPE  *p_bbchip);
extern int __stdcall GPS_DL_GetBBChipName(GPS_DL_HANDLE_T  dl_handle, char *buf, unsigned int buf_len);
extern int __stdcall GPS_DL_GetCustomName(GPS_DL_HANDLE_T  dl_handle, char *buf, unsigned int buf_len);
extern int __stdcall GPS_DL_Rom_GetInfo(GPS_DL_HANDLE_T  dl_handle, unsigned short index, GPS_ROM_INFO  *p_rom_info);
extern int __stdcall GPS_DL_Rom_GetInfoAll(GPS_DL_HANDLE_T  dl_handle, GPS_ROM_INFO  *p_rom_info, unsigned short max_rom_count);
extern int __stdcall GPS_DL_Rom_GetResInfo(GPS_DL_HANDLE_T  dl_handle, unsigned short index, GPS_MTK_Resource  *p_res_info);
extern int __stdcall GPS_DL_Rom_SetFilepath(GPS_DL_HANDLE_T  dl_handle, unsigned short index, const char *rom_filepath);
extern int __stdcall GPS_DL_Rom_LoadByFilepath(GPS_DL_HANDLE_T  dl_handle, unsigned short index, const char *rom_filepath);
extern int __stdcall GPS_DL_Rom_Load(GPS_DL_HANDLE_T  dl_handle, unsigned short index);
extern int __stdcall GPS_DL_Rom_Unload(GPS_DL_HANDLE_T  dl_handle, unsigned short index);
extern int __stdcall GPS_DL_Rom_LoadAll(GPS_DL_HANDLE_T  dl_handle, GPS_ROM_INFO  *p_rom_info);
extern int __stdcall GPS_DL_Rom_UnloadAll(GPS_DL_HANDLE_T  dl_handle);
extern int __stdcall GPS_DL_Rom_SetEnableAttr(GPS_DL_HANDLE_T  dl_handle, unsigned short index, _BOOL  attr);

//------------------------------------------------------------------------------
// GPS_DA_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------

typedef struct {
	char			filepath[256];
	unsigned int	start_addr;
	unsigned char	*buf;
	unsigned int	buf_len;
}GPS_DA_INFO;

struct GPS_DA_HANDLE;
typedef struct GPS_DA_HANDLE *	GPS_DA_HANDLE_T;
extern int __stdcall GPS_DA_Create(GPS_DA_HANDLE_T  *p_da_handle);
extern int __stdcall GPS_DA_Destroy(GPS_DA_HANDLE_T  *p_da_handle);
extern int __stdcall GPS_DA_IsReady(GPS_DA_HANDLE_T  da_handle, GPS_DA_INFO  *p_da_info, _BOOL  check_if_updated);
extern int __stdcall GPS_DA_GetInfo(GPS_DA_HANDLE_T  da_handle, GPS_DA_INFO  *p_da_info);
extern int __stdcall GPS_DA_SetStartAddr(GPS_DA_HANDLE_T  da_handle, unsigned int da_start_addr);
extern int __stdcall GPS_DA_SetFilepath(GPS_DA_HANDLE_T  da_handle, const char *da_filepath);
extern int __stdcall GPS_DA_LoadByFilepath(GPS_DA_HANDLE_T  da_handle, const char *da_filepath);
extern int __stdcall GPS_DA_Load(GPS_DA_HANDLE_T  da_handle);
extern int __stdcall GPS_DA_Unload(GPS_DA_HANDLE_T  da_handle);
extern int __stdcall GPS_DA_AutoLoadByBBChipType(GPS_DA_HANDLE_T  da_handle, const GPS_BBCHIP_TYPE  bbchip_type);
extern int __stdcall GPS_DA_AutoLoadByCustomName(GPS_DA_HANDLE_T  da_handle, const char *custom_name);

//------------------------------------------------------------------------------
// GPS_RB_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------
typedef struct {
	char				filepath[256];
	unsigned int		readback_addr;
	unsigned int		readback_len;
	_BOOL				enable;	// _FALSE -> disbale 
								// _TRUE  -> enable  
	unsigned short		index;
}GPS_RB_INFO;

struct GPS_RB_HANDLE;
typedef struct GPS_RB_HANDLE *	GPS_RB_HANDLE_T;
extern int __stdcall GPS_RB_Create(GPS_RB_HANDLE_T  *p_rb_handle);
extern int __stdcall GPS_RB_Destroy(GPS_RB_HANDLE_T  *p_rb_handle);
extern int __stdcall GPS_RB_SetPacketLength(GPS_RB_HANDLE_T  rb_handle, unsigned short length);
extern int __stdcall GPS_RB_GetCount(GPS_RB_HANDLE_T  rb_handle, unsigned short *p_rb_count);
extern int __stdcall GPS_RB_IsReady(GPS_RB_HANDLE_T  rb_handle, GPS_RB_INFO  *p_rb_info);
extern int __stdcall GPS_RB_GetInfo(GPS_RB_HANDLE_T  rb_handle, unsigned short index, GPS_RB_INFO  *p_rb_info);
extern int __stdcall GPS_RB_GetInfoAll(GPS_RB_HANDLE_T  rb_handle, GPS_RB_INFO  *p_rb_info, unsigned short max_rb_count);
extern int __stdcall GPS_RB_Append(GPS_RB_HANDLE_T  rb_handle, const char *rb_filepath, unsigned int rb_addr, unsigned int rb_len);
extern int __stdcall GPS_RB_Delete(GPS_RB_HANDLE_T  rb_handle, unsigned short index);
extern int __stdcall GPS_RB_SetFilepath(GPS_RB_HANDLE_T  rb_handle, unsigned short index, const char *rb_filepath);
extern int __stdcall GPS_RB_SetReadbackAddr(GPS_RB_HANDLE_T  rb_handle, unsigned short index, unsigned int rb_addr);
extern int __stdcall GPS_RB_SetReadbackLength(GPS_RB_HANDLE_T  rb_handle, unsigned short index, unsigned int rb_len);
extern int __stdcall GPS_RB_ClearAll(GPS_RB_HANDLE_T  rb_handle);
extern int __stdcall GPS_RB_SetEnableAttr(GPS_RB_HANDLE_T  rb_handle, unsigned short index, _BOOL  attr);

#ifdef	__cplusplus
}
#endif

#endif
