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
#ifndef	_FLASHTOOL_HANDLE_H_
#define	_FLASHTOOL_HANDLE_H_

#include "mtk_mcu.h"
#include "mtk_status.h"
#include "mtk_resource.h"
#include "DOWNLOAD.H"

#ifdef	__cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Boolean                                                                      
//------------------------------------------------------------------------------
typedef enum
{
	_FALSE = 0,
	_TRUE = 1
} _BOOL;

extern void __stdcall UpdateMMAAEnable(bool bIsMMAAEnable);

//------------------------------------------------------------------------------
// prototype of remote file operation method callback                           
//------------------------------------------------------------------------------
typedef int (__stdcall *CALLBACK_REMOTE_GET_FILE_LEN)(const char *filepath, unsigned int * const p_file_length, void *user_arg);
typedef int (__stdcall *CALLBACK_REMOTE_READ_FILE)(const char *filepath, unsigned char * const p_file_content, unsigned int file_length, void *user_arg);

//------------------------------------------------------------------------------
// prototype of remote file's signature operation method callback                           
//------------------------------------------------------------------------------
typedef int (__stdcall *CALLBACK_REMOTE_GET_SIG_LEN)(const char *filepath, unsigned int * const p_sig_length, void *user_arg);
typedef int (__stdcall *CALLBACK_REMOTE_READ_SIG)(const char *filepath, unsigned char * const p_sig_content, unsigned int sig_length, void *user_arg);

//------------------------------------------------------------------------------
// prototype of customized file integrity check callback                        
//------------------------------------------------------------------------------
typedef int (__stdcall *CALLBACK_FILE_INTEGRITY_CHECK)(unsigned char * const p_file_content, unsigned int * const p_file_length, void *user_arg);

//----------------------------------------------------------------------------
// Config file parsing
//----------------------------------------------------------------------------
struct ExternalMemorySetting;
struct ROMSetting;

extern int __stdcall GetROMSetting(const char *configFilePath,
                                   struct ROMSetting *romSetting);

extern int __stdcall GetExternalMemorySetting(
                            const char *configFilePath,
                            struct ExternalMemorySetting *externalMemorySetting);

//------------------------------------------------------------------------------
// BL_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------

typedef struct
{
	_BOOL				m_valid;	// _TRUE -> dram setting is valid 
	unsigned int		m_emi_gen_a;
	unsigned int		m_emi_gen_b;
	unsigned int		m_emi_gen_c;
	unsigned int		m_emi_gen_d;
	unsigned int		m_emi_con_i;
	unsigned int		m_emi_con_i_ext;
	unsigned int		m_emi_con_j;
	unsigned int		m_emi_con_k;
	unsigned int		m_emi_con_l;
	unsigned int		m_emi_con_m;
	unsigned int		m_emi_con_n;
	unsigned int		m_emi_del_a;
	unsigned int		m_emi_del_b;
	unsigned int		m_emi_del_c;
	unsigned int		m_emi_del_d;
	unsigned int		m_emi_del_e;
	unsigned int		m_emi_del_f;
	unsigned int		m_emi_del_g;
	unsigned int		m_emi_del_h;
	unsigned int		m_emi_del_i;
	unsigned int		m_emi_del_j;
	unsigned int		m_emi_arb_a;
	unsigned int		m_emi_arb_b;
	unsigned int		m_emi_arb_c;
	unsigned int		m_emi_arb_d;
	unsigned int		m_emi_arb_e;
	unsigned int		m_emi_arb_f;
	unsigned int		m_emi_arb_g;
	unsigned int		m_emi_arb_h;
	unsigned int		m_emi_arb_i;
} DRAM_SETTING;

typedef struct
{
	char				m_version[5];
	char				m_filepath[512];
	unsigned int		m_filesize;
	unsigned int		m_start_addr;
	unsigned int		m_chksum_seed;
	unsigned int		m_chksum;
	DRAM_SETTING		m_dram_cfg;
	_BOOL				m_ext_bootloader_exist;
	char				m_ext_filepath[512];
	unsigned int		m_ext_filesize;
	unsigned int		m_ext_chksum;
	
	_BOOL				m_enable;	// _FALSE -> disbale 
									// _TRUE  -> enable  
} BL_INFO;

//------------------------------------------------------------------------------
// DL_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------
typedef enum
{
	ABSOLUTE_ADDR = 0,
	OFFSET_VALUE
} REGION_ADDR_TYPE;

typedef enum
{
	NORMAL_ROM 		= 0x01,
	RESOURCE_BIN 	= 0x02,
	JUMPTABLE_BIN 	= 0x03,
	ENFB_BIN 		= 0x04,
	UA_BIN 			= 0x05,
	BL_BIN 			= 0x06,
	BOOT_CERT_BIN 	= 0x07,

	UNKNOWN_BIN 	= 0xFF
} ROM_TYPE;

typedef struct
{
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
}ROM_INFO;

typedef enum
{
    // An element with V_ prefix is a virtual element used to define
    // a range for a group of similar ROMs.
    V_BL_ROM       = 0x00000000,
    ARM_BL_ROM     = V_BL_ROM,
    DSP_BL_ROM     = 0x00000001,
    ROOT_CERT_ROM  = 0x00000080,
    PATCH_ROM      = 0x00000081,
    V_BL_ROM_END   = 0x000000FF,

    V_MAUI_ROM     = 0x00000100,
    MAUI_ROM       = V_MAUI_ROM,
    RESOURCE_ROM   = 0x00000101,
    JUMP_TABLE_ROM = 0x00000102,
    V_MAUI_ROM_END = 0x000001FF,

    UNKNOWN_ROM    = 0xFFFFFFFF
} ROM_TYPE_EX;

typedef struct
{
    // General
    unsigned short   index;
    _BOOL            enable;
    ROM_TYPE_EX      rom_type;
    char             name[64];

    // Address
    unsigned int     region_addr;
    REGION_ADDR_TYPE region_addr_type;
    unsigned int     begin_addr;
    unsigned int     end_addr;

    // File information
    char             filepath[512];
    unsigned int     filesize;
} ROM_INFO_EX;

typedef struct
{
	unsigned int		m_is_nfb:1;
	unsigned int		m_security_supported:1;
	unsigned int		m_cust_para_supported:1;
	unsigned int		m_system_drive_on_nand:1;
	unsigned int		m_fota_full:1;
    unsigned int		m_bootloader_usb_supported:1;
    unsigned int		m_brom_usb_supported:1;
} DL_Config_T;

typedef struct
{
	DL_Config_T			m_config;
} DL_INFO;

typedef enum
{
    ROM_VERSION_SV3,
    ROM_VERSION_SV5
} ROM_VERSION;

typedef enum
{
    NOR_BOOT,
    NAND_BOOT,
    EMMC_BOOT
} BOOT_TYPE;

typedef struct
{
    ROM_VERSION  rom_version;
    BOOT_TYPE    boot_type;
    _BOOL        security_supported;
    _BOOL        cust_para_supported;
    _BOOL        system_drive_on_nand;
    _BOOL        fota_full;
    _BOOL        bootloader_usb_supported;
    _BOOL        brom_usb_supported;
} DL_INFO_EX;


struct DL_HANDLE;
typedef struct DL_HANDLE *	DL_HANDLE_T;

extern _BOOL __stdcall DL_IsEnctrypedBin(DL_HANDLE_T pDownloadHandle);

extern int __stdcall DL_Create(DL_HANDLE_T  *p_dl_handle);
extern int __stdcall DL_Clone(DL_HANDLE_T *p_new_dl_handle, DL_HANDLE_T p_dl_handle);
extern int __stdcall DL_Destroy(DL_HANDLE_T  *p_dl_handle);

extern int __stdcall DL_CreateROMEntries(DL_HANDLE_T dl_handle,
                                       const struct ROMSetting *rom_setting);
extern int __stdcall DL_AutoLoadROMFiles(DL_HANDLE_T dl_handle);

extern int __stdcall DL_LoadScatter(DL_HANDLE_T  dl_handle, const char *scatter_filepath);
extern int __stdcall DL_AutoLoadByScatRegionName(DL_HANDLE_T  dl_handle, const char *full_filepath, unsigned char allow_rom_type, _BOOL is_scatter_file);

extern int __stdcall DL_SetPacketLength(DL_HANDLE_T  dl_handle, unsigned short length);
extern int __stdcall DL_GetCount(DL_HANDLE_T  dl_handle, unsigned short *p_rom_count);
extern int __stdcall DL_GetInfo(DL_HANDLE_T  dl_handle, DL_INFO  *p_dl_info);
extern int __stdcall DL_IsReady(DL_HANDLE_T  dl_handle, ROM_INFO  *p_rom_info, BL_INFO  *p_bl_info, _BOOL  check_if_updated);
extern int __stdcall DL_GetJumpTableInfo(DL_HANDLE_T  dl_handle, MTK_JumpTable  *p_jmptbl_info);
extern int __stdcall DL_GetProjectId(DL_HANDLE_T  dl_handle, char *buf, unsigned int buf_len);
extern int __stdcall DL_GetDRAMSetting(DL_HANDLE_T  dl_handle, DRAM_SETTING  *dram_cfg); 
extern int __stdcall DL_ResourceProjectIdComparisonSetting(DL_HANDLE_T  dl_handle, _BOOL  enable);
extern int __stdcall DL_GetBBChipType(DL_HANDLE_T  dl_handle, BBCHIP_TYPE  *p_bbchip);
extern int __stdcall DL_GetBBChipName(DL_HANDLE_T  dl_handle, char *buf, unsigned int buf_len);
extern int __stdcall DL_GetCustomName(DL_HANDLE_T  dl_handle, char *buf, unsigned int buf_len);
extern int __stdcall DL_GetFlashName(DL_HANDLE_T  dl_handle, char *buf, unsigned int buf_len);
extern int __stdcall DL_GetFlashDeviceCode(DL_HANDLE_T  dl_handle, char *flashID);
extern int __stdcall DL_Rom_GetInfo(DL_HANDLE_T  dl_handle, unsigned short index, ROM_INFO  *p_rom_info);
extern int __stdcall DL_Rom_GetInfoAll(DL_HANDLE_T  dl_handle, ROM_INFO  *p_rom_info, unsigned short max_rom_count);
extern int __stdcall DL_Rom_GetResInfo(DL_HANDLE_T  dl_handle, unsigned short index, MTK_Resource  *p_res_info);
extern int __stdcall DL_Rom_Load(DL_HANDLE_T  dl_handle, unsigned short index, const char *rom_filepath);
extern int __stdcall DL_Rom_Unload(DL_HANDLE_T  dl_handle, unsigned short index);
extern int __stdcall DL_Rom_UnloadAll(DL_HANDLE_T  dl_handle);
extern int __stdcall DL_Rom_SetEnableAttr(DL_HANDLE_T  dl_handle, unsigned short index, _BOOL  attr);

extern int __stdcall DL_GetCount_Ex(DL_HANDLE_T dl_handle, unsigned short *p_rom_count);
extern int __stdcall DL_GetInfo_Ex(DL_HANDLE_T dl_handle, DL_INFO_EX *p_dl_info);
extern int __stdcall DL_IsReady_Ex(DL_HANDLE_T dl_handle, ROM_INFO_EX *p_rom_info, _BOOL check_if_updated);
extern int __stdcall DL_GetFlashDeviceCode_Ex(DL_HANDLE_T dl_handle, char *flashID, unsigned int max_len);

extern int __stdcall DL_Rom_GetInfo_Ex(DL_HANDLE_T dl_handle, unsigned short index, ROM_INFO_EX *p_rom_info);
extern int __stdcall DL_Rom_GetInfoAll_Ex(DL_HANDLE_T dl_handle, ROM_INFO_EX *p_rom_info, unsigned short max_rom_count);
extern int __stdcall DL_Rom_Load_Ex(DL_HANDLE_T dl_handle, unsigned short index, const char *rom_filepath);
extern int __stdcall DL_Rom_Unload_Ex(DL_HANDLE_T dl_handle, unsigned short index);
extern int __stdcall DL_Rom_SetEnableAttr_Ex(DL_HANDLE_T dl_handle, unsigned short index, _BOOL attr);

extern int __stdcall DL_IsNFB(DL_HANDLE_T  dl_handle, _BOOL  *p_is_nfb);
extern int __stdcall DL_SetCallback(DL_HANDLE_T  dl_handle, 
									CALLBACK_REMOTE_GET_FILE_LEN	cb_remote_get_file_len,
									void *							cb_remote_get_file_len_arg,
									CALLBACK_REMOTE_READ_FILE		cb_remote_read_file,
									void *							cb_remote_read_file_arg,
									CALLBACK_REMOTE_GET_SIG_LEN		cb_remote_get_sig_len,
									void *							cb_remote_get_sig_len_arg,
									CALLBACK_REMOTE_READ_SIG		cb_remote_read_sig,
									void *							cb_remote_read_sig_arg,
									CALLBACK_FILE_INTEGRITY_CHECK	cb_file_integrity_check,
									void *							cb_file_integrity_check_arg);

extern int __stdcall DL_GetBootType(DL_HANDLE_T dl_handle, BOOT_TYPE *boot_type);

// BL_HANDLE related 
extern int __stdcall DL_BL_IsReady(DL_HANDLE_T  dl_handle, BL_INFO  *p_bl_info, _BOOL  check_if_updated);
extern int __stdcall DL_BL_Load(DL_HANDLE_T  dl_handle, const char *bl_filepath);
extern int __stdcall DL_BL_EXT_Load(DL_HANDLE_T  dl_handle, const char *bl_ext_filepath);
extern int __stdcall DL_BL_GetInfo(DL_HANDLE_T  dl_handle, BL_INFO  *p_bl_info);
extern int __stdcall DL_BL_SetEnableAttr(DL_HANDLE_T  dl_handle, _BOOL  attr);

//------------------------------------------------------------------------------
// DL_HANDLE_LIST: Exported Functions                                           
//------------------------------------------------------------------------------
struct DL_HANDLE_LIST;
typedef struct DL_HANDLE_LIST *	DL_HANDLE_LIST_T;
extern int __stdcall DL_CreateList(DL_HANDLE_LIST_T * dl_handle_list);
extern int __stdcall DL_CloneList(DL_HANDLE_LIST_T *p_new_dl_handle_list, DL_HANDLE_LIST_T p_dl_handle_list);
extern int __stdcall DL_DestroyList(DL_HANDLE_LIST_T * dl_handle_list);
extern int __stdcall DL_AddHandleToList(DL_HANDLE_LIST_T dl_handle_list, DL_HANDLE_T dl_handle);
extern int __stdcall DL_UpdateHandleToList(DL_HANDLE_LIST_T dl_handle_list, DL_HANDLE_T org_dl_handle, DL_HANDLE_T new_dl_handle);
extern int __stdcall DL_DeleteHandleFromList(DL_HANDLE_LIST_T dl_handle_list, DL_HANDLE_T dl_handle);
extern int __stdcall DL_GetHandleNumber(DL_HANDLE_LIST_T dl_handle_list, int * HandleNumber);
extern int __stdcall DL_GetHandle(DL_HANDLE_LIST_T dl_handle_list, int index, DL_HANDLE_T *dl_handle);
extern int __stdcall DL_CheckHandleInList(DL_HANDLE_LIST_T dl_handle_list, DL_HANDLE_T dl_handle);
extern int __stdcall DL_CheckHandleSameSWVersionInList(DL_HANDLE_LIST_T dl_handle_list, DL_HANDLE_T dl_handle);

//------------------------------------------------------------------------------
// DA_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------

typedef struct
{
	char			filepath[256];
	unsigned int	start_addr;
	unsigned char	*buf;
	unsigned int	buf_len;
	unsigned int	main_prog_size;
	char			version[13];
	char			last_modified_date[256];
}DA_INFO;

struct DA_HANDLE;
typedef struct DA_HANDLE *		DA_HANDLE_T;
extern int __stdcall DA_Create(DA_HANDLE_T  *p_da_handle);
extern int __stdcall DA_Destroy(DA_HANDLE_T  *p_da_handle);
extern int __stdcall DA_IsReady(DA_HANDLE_T  da_handle, DA_INFO  *p_da_info, _BOOL  check_if_updated);
extern int __stdcall DA_GetInfo(DA_HANDLE_T  da_handle, DA_INFO  *p_da_info);
extern int __stdcall DA_SetDefaultStartAddr(DA_HANDLE_T  da_handle, unsigned int da_start_addr);
extern int __stdcall DA_Load(DA_HANDLE_T  da_handle, const char *da_filepath, _BOOL  bDA_Validation);
extern int __stdcall DA_Unload(DA_HANDLE_T  da_handle);
extern int __stdcall DA_SetCallback(DA_HANDLE_T  da_handle, 
									CALLBACK_REMOTE_GET_FILE_LEN	cb_remote_get_file_len,
									void *							cb_remote_get_file_len_arg,
									CALLBACK_REMOTE_READ_FILE		cb_remote_read_file,
									void *							cb_remote_read_file_arg,
									CALLBACK_REMOTE_GET_SIG_LEN		cb_remote_get_sig_len,
									void *							cb_remote_get_sig_len_arg,
									CALLBACK_REMOTE_READ_SIG		cb_remote_read_sig,
									void *							cb_remote_read_sig_arg,
									CALLBACK_FILE_INTEGRITY_CHECK	cb_file_integrity_check,
									void *							cb_file_integrity_check_arg);

//------------------------------------------------------------------------------
// RB_HANDLE: Exported Functions                                                
//------------------------------------------------------------------------------
typedef struct
{
	char				filepath[256];
	unsigned int		readback_addr;
	unsigned int		readback_len;
	_BOOL				enable;	// _FALSE -> disbale 
								// _TRUE  -> enable  

	unsigned short		index;

	NUTL_ReadFlag_E		m_read_flag;

}RB_INFO;

struct RB_HANDLE;
typedef struct RB_HANDLE *	RB_HANDLE_T;
extern int __stdcall RB_Create(RB_HANDLE_T  *p_rb_handle);
extern int __stdcall RB_Destroy(RB_HANDLE_T  *p_rb_handle);
extern int __stdcall RB_SetPacketLength(RB_HANDLE_T  rb_handle, unsigned short length);
extern int __stdcall RB_GetCount(RB_HANDLE_T  rb_handle, unsigned short *p_rb_count);
extern int __stdcall RB_IsReady(RB_HANDLE_T  rb_handle, RB_INFO  *p_rb_info);
extern int __stdcall RB_GetInfo(RB_HANDLE_T  rb_handle, unsigned short index, RB_INFO  *p_rb_info);
extern int __stdcall RB_GetInfoAll(RB_HANDLE_T  rb_handle, RB_INFO  *p_rb_info, unsigned short max_rb_count);
extern int __stdcall RB_Append(RB_HANDLE_T  rb_handle, const char *rb_filepath, unsigned int rb_addr, unsigned int rb_len);
extern int __stdcall RB_Append_ReadToBuf(RB_HANDLE_T  rb_handle, unsigned int rb_addr, unsigned int rb_len, char *buf, unsigned int buf_len);
extern int __stdcall RB_Delete(RB_HANDLE_T  rb_handle, unsigned short index);
extern int __stdcall RB_SetFilepath(RB_HANDLE_T  rb_handle, unsigned short index, const char *rb_filepath);
extern int __stdcall RB_SetReadbackFlag(RB_HANDLE_T  rb_handle, unsigned short index, NUTL_ReadFlag_E  flag);
extern int __stdcall RB_SetReadbackAddr(RB_HANDLE_T  rb_handle, unsigned short index, unsigned int rb_addr);
extern int __stdcall RB_SetReadbackLength(RB_HANDLE_T  rb_handle, unsigned short index, unsigned int rb_len);
extern int __stdcall RB_ClearAll(RB_HANDLE_T  rb_handle);
extern int __stdcall RB_SetEnableAttr(RB_HANDLE_T  rb_handle, unsigned short index, _BOOL  attr);
extern int __stdcall RB_EnableReadbackFAT(RB_HANDLE_T  rb_handle, unsigned short index, _BOOL  attr);

//----------------------------------------------------------------------------
// AUTH_HANDLE: Exported Functions
//----------------------------------------------------------------------------
typedef struct
{
    unsigned int  m_version;
    char          m_filepath[512];
    unsigned char m_secure_custom_name[32];
    unsigned char m_description[512];
} AUTH_INFO;

struct AUTH_HANDLE;
typedef struct AUTH_HANDLE * AUTH_HANDLE_T;

extern int __stdcall AUTH_Create(AUTH_HANDLE_T *p_auth_handle);

extern int __stdcall AUTH_Destroy(AUTH_HANDLE_T *p_auth_handle);

extern int __stdcall AUTH_IsReady(AUTH_HANDLE_T auth_handle,
                                  AUTH_INFO *p_auth_info,
                                  _BOOL  bCheckIfUpdated);

extern int __stdcall AUTH_GetInfo(AUTH_HANDLE_T auth_handle,
                                  AUTH_INFO *p_auth_info);

extern int __stdcall AUTH_Load(AUTH_HANDLE_T auth_handle,
                               const char *auth_filepath);

extern int __stdcall AUTH_Unload(AUTH_HANDLE_T auth_handle);

extern int __stdcall AUTH_SetCallback(
                        AUTH_HANDLE_T auth_handle, 
                        CALLBACK_REMOTE_GET_FILE_LEN cb_remote_get_file_len,
                        void *cb_remote_get_file_len_arg,
                        CALLBACK_REMOTE_READ_FILE cb_remote_read_file,
                        void *cb_remote_read_file_arg,
                        CALLBACK_REMOTE_GET_SIG_LEN cb_remote_get_sig_len,
                        void *cb_remote_get_sig_len_arg,
                        CALLBACK_REMOTE_READ_SIG cb_remote_read_sig,
                        void *cb_remote_read_sig_arg,
                        CALLBACK_FILE_INTEGRITY_CHECK cb_file_integrity_check,
                        void *cb_file_integrity_check_arg);

//----------------------------------------------------------------------------
// SCERT_HANDLE: Exported Functions
//----------------------------------------------------------------------------
typedef struct
{
    unsigned int  m_version;
    char          m_filepath[512];
    unsigned char m_secure_custom_name[32];
    unsigned char m_description[512];
} SCERT_INFO;

struct SCERT_HANDLE;
typedef struct SCERT_HANDLE * SCERT_HANDLE_T;

extern int __stdcall SCERT_Create(SCERT_HANDLE_T *p_scert_handle);

extern int __stdcall SCERT_Destroy(SCERT_HANDLE_T *p_scert_handle);

extern int __stdcall SCERT_IsReady(SCERT_HANDLE_T scert_handle,
                                   SCERT_INFO *p_scert_info,
                                   _BOOL bCheckIfUpdated);

extern int __stdcall SCERT_GetInfo(SCERT_HANDLE_T scert_handle,
                                   SCERT_INFO *p_scert_info);

extern int __stdcall SCERT_Load(SCERT_HANDLE_T scert_handle,
                                const char *scert_filepath);

extern int __stdcall SCERT_Unload(SCERT_HANDLE_T scert_handle);

extern int __stdcall SCERT_SetCallback(
                        SCERT_HANDLE_T scert_handle, 
                        CALLBACK_REMOTE_GET_FILE_LEN cb_remote_get_file_len,
                        void *cb_remote_get_file_len_arg,
                        CALLBACK_REMOTE_READ_FILE cb_remote_read_file,
                        void *cb_remote_read_file_arg,
                        CALLBACK_REMOTE_GET_SIG_LEN cb_remote_get_sig_len,
                        void *cb_remote_get_sig_len_arg,
                        CALLBACK_REMOTE_READ_SIG cb_remote_read_sig,
                        void *cb_remote_read_sig_arg,
                        CALLBACK_FILE_INTEGRITY_CHECK cb_file_integrity_check,
                        void *cb_file_integrity_check_arg);

#ifdef	__cplusplus
}
#endif

#endif
