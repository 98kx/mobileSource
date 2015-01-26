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
 *  brom.h
 *
 * Project:
 * --------
 *  BootRom Library
 *
 * Description:
 * ------------
 *  Exported C interface APIs for BootRom library.
 *
 * Author:
 * -------
 *  Amos Hsu (mtk00539)
 *
 *******************************************************************************/
#ifndef _GPS_BROM_H_
#define _GPS_BROM_H_

#include "brom.h"

#include <windows.h>
#include "gps_mtk_mcu.h"

#ifdef	__cplusplus
extern "C" {
#endif

// The magic value to stop boot process 
#define BOOT_STOP	9876

// boot infinitely
#define BOOT_INFINITE	0xFFFFFFF

// default start cmd retry count 
#define DEFAULT_BROM_START_CMD_RETRY_COUNT	1


//------------------------------------------------------------------------------
// return code                                                                  
//------------------------------------------------------------------------------

#define BROM_RET(ret)	(ret&0x00FF0000)

#define BROM_OK									0x000000
#define BROM_ERROR								0x010000
#define BROM_NO_MEMORY							0x020000
#define BROM_INVALID_ARGUMENTS					0x030000
#define BROM_SET_COM_STATE_FAIL					0x040000
#define BROM_PURGE_COM_FAIL						0x050000
#define BROM_SET_META_REG_FAIL					0x060000
#define BROM_SET_FLASHTOOL_REG_FAIL				0x070000
#define BROM_SET_REMAP_REG_FAIL					0x080000
#define BROM_SET_MEM_WAIT_STATE_FAIL			0x090000
#define BROM_DOWNLOAD_DA_FAIL					0x0A0000
#define BROM_CMD_START_FAIL						0x0B0000
#define BROM_CMD_JUMP_FAIL						0x0C0000
#define BROM_CMD_WRITE16_MEM_FAIL				0x0D0000
#define BROM_CMD_READ16_MEM_FAIL				0x0E0000
#define BROM_CMD_WRITE16_REG_FAIL				0x0F0000
#define BROM_CMD_READ16_REG_FAIL				0x100000
#define BROM_CMD_CHKSUM16_MEM_FAIL				0x110000
#define BROM_CMD_WRITE32_MEM_FAIL				0x120000
#define BROM_CMD_READ32_MEM_FAIL				0x130000
#define BROM_CMD_WRITE32_REG_FAIL				0x140000
#define BROM_CMD_READ32_REG_FAIL				0x150000
#define BROM_CMD_CHKSUM32_MEM_FAIL				0x160000
#define BROM_WR16_RD16_MEM_RESULT_DIFF			0x170000
#define BROM_WR16_RD16_REG_RESULT_DIFF			0x180000
#define BROM_WR32_RD32_MEM_RESULT_DIFF			0x190000
#define BROM_WR32_RD32_REG_RESULT_DIFF			0x1A0000
#define BROM_CHKSUM16_MEM_RESULT_DIFF			0x1B0000
#define BROM_CHKSUM32_MEM_RESULT_DIFF			0x1C0000
#define BROM_BBCHIP_HW_VER_INCORRECT			0x1D0000
#define BROM_FAIL_TO_GET_BBCHIP_HW_VER			0x1E0000
#define BROM_SKIP_BBCHIP_HW_VER_CHECK			0x1F0000
#define BROM_UNKNOWN_BBCHIP						0x200000
#define BROM_UNKNOWN_TGT_BBCHIP					0x210000
#define BROM_BBCHIP_DSP_VER_INCORRECT			0x220000
#define BROM_MULTIPLE_BAUDRATE_FAIL				0x230000
#define BROM_JUMP_TO_NFB_DETECTION_FAIL			0x240000

//------------------------------------------------------------------------------
// prototype of percentage callback function in BootROM stage                   
//------------------------------------------------------------------------------
typedef int (__stdcall *GPS_CALLBACK_IN_BROM_STAGE)(unsigned int brom_handle, HANDLE hCOM, void *usr_arg);

//------------------------------------------------------------------------------
// prototype of percentage callback function for download a buffer into target  
//------------------------------------------------------------------------------
typedef int (__stdcall *GPS_CALLBACK_BROM_WRITE_BUF_INIT)(void *usr_arg);
typedef int (__stdcall *GPS_CALLBACK_BROM_WRITE_BUF)(unsigned char finished_percentage, unsigned int sent_bytes, unsigned int total_bytes, void *usr_arg);

//------------------------------------------------------------------------------
// runtime debug trace                                                          
//------------------------------------------------------------------------------
int __stdcall GPS_Brom_DebugOn(void);
int __stdcall GPS_Brom_DebugOff(void);
int __stdcall GPS_Brom_DebugClear(void);

//------------------------------------------------------------------------------
// translate enum to string                                                     
//------------------------------------------------------------------------------
const char * __stdcall GPS_BBChipTypeToName(const GPS_BBCHIP_TYPE  bbchip);
const char * __stdcall GPS_ExtClockToName(const EXT_CLOCK  ext_clock);


//------------------------------------------------------------------------------
// boot FlashTool download mode                                                 
//------------------------------------------------------------------------------
typedef struct {

	// [IN] the PC side bbchip revsion name that is used to compare with target bbchip 
	// eg: "MT6218B_EN", if this field is NULL it will skip comparison with target bbchip 
	const char 		*m_p_bbchip_name;

	// EMI Control for Bank0 and Bank1 
	unsigned int	*m_p_bank0_mem_cfg;		// bank0 memory config 
	unsigned int	*m_p_bank1_mem_cfg;		// bank0 memory config 

	// Download Agent 
	unsigned int		m_da_start_addr;		// DA start address 
	const unsigned char	*m_da_buf;				// buffer stored DA code 
	unsigned int		m_da_len;				// length of DA buffer 

	// Callback function for initialization before downloading DA code into target
	GPS_CALLBACK_BROM_WRITE_BUF_INIT	m_cb_download_da_init;
	// The user input argument for callback function
	void 	*m_cb_download_da_init_arg;

	// Callback function while downloading DA code into target
	GPS_CALLBACK_BROM_WRITE_BUF		m_cb_download_da;
	// The user input argument for callback function
	void 	*m_cb_download_da_arg;

	// This callback function will be invoke after BootROM start cmd is passed. 
	// You can issue other BootROM command by brom_handle and hCOM which provides callback arguments, 
	// or do whatever you want otherwise. 
	GPS_CALLBACK_IN_BROM_STAGE		m_cb_in_brom_stage;
	void	*m_cb_in_brom_stage_arg;

	// speed-up BootROM stage baudrate 
	_BOOL	m_speedup_brom_baudrate;

} GPS_BOOT_FLASHTOOL_ARG;

int  __stdcall GPS_Boot_FlashTool(
		HANDLE hCOM,
		GPS_BBCHIP_TYPE  bbchip, EXT_CLOCK  ext_clock,
		int *p_bootstop,
		unsigned int ms_boot_timeout,
		const GPS_BOOT_FLASHTOOL_ARG  *p_arg);

int  __stdcall GPS_Boot_FlashTool_Ex(
		HANDLE hCOM,
		GPS_BBCHIP_TYPE  bbchip, EXT_CLOCK  ext_clock,
		int *p_bootstop,
		unsigned int ms_boot_timeout,
		unsigned int max_start_cmd_retry_count,
		const GPS_BOOT_FLASHTOOL_ARG  *p_arg);

int  __stdcall GPS_AutoBoot_FlashTool(
		HANDLE hCOM,
		GPS_BBCHIP_TYPE  *p_bbchip, EXT_CLOCK  *p_ext_clock,
		int *p_bootstop,
		unsigned int ms_boot_timeout,
		const GPS_BOOT_FLASHTOOL_ARG  *p_arg);

int  __stdcall GPS_AutoBoot_FlashTool_Ex(
		HANDLE hCOM,
		GPS_BBCHIP_TYPE  *p_bbchip, EXT_CLOCK  *p_ext_clock,
		int *p_bootstop,
		unsigned int ms_boot_timeout,
		unsigned int max_start_cmd_retry_count,
		const GPS_BOOT_FLASHTOOL_ARG  *p_arg);

//------------------------------------------------------------------------------
// get brom.dll version                                                         
//------------------------------------------------------------------------------
int __stdcall GPS_GetBromDLLVersion( unsigned int *major_ver, unsigned int *minor_ver, unsigned int *build_num );

//------------------------------------------------------------------------------
// BootROM Command                                                               
//------------------------------------------------------------------------------
int __stdcall GPS_Brom_Create(unsigned int *p_brom_handle, GPS_BBCHIP_TYPE  bbchip, EXT_CLOCK  ext_clock, int *p_bootstop, unsigned int ms_boot_timeout, unsigned int max_start_cmd_retry_count);
int __stdcall GPS_Brom_CreateAutoBoot(unsigned int *p_brom_handle, GPS_BBCHIP_TYPE  *p_bbchip, EXT_CLOCK  *p_ext_clock, int *p_bootstop, unsigned int ms_boot_timeout, unsigned int max_start_cmd_retry_count);
int __stdcall GPS_Brom_Destroy(unsigned int *p_brom_handle);
int __stdcall GPS_Brom_StartCmd(unsigned int brom_handle, HANDLE hCOM);
int __stdcall GPS_Brom_Write16Cmd(unsigned int brom_handle, HANDLE hCOM, unsigned int write_addr, unsigned short *data, unsigned int num_of_word);
int __stdcall GPS_Brom_Read16Cmd(unsigned int brom_handle, HANDLE hCOM, unsigned int read_addr, unsigned short *data, unsigned int num_of_word);
int __stdcall GPS_Brom_Write32Cmd(unsigned int brom_handle, HANDLE hCOM, unsigned int write_addr, unsigned int *data, unsigned int num_of_dword);
int __stdcall GPS_Brom_Read32Cmd(unsigned int brom_handle, HANDLE hCOM, unsigned int read_addr, unsigned int *data, unsigned int num_of_dword);
int __stdcall GPS_Brom_Checksum16Cmd(unsigned int brom_handle, HANDLE hCOM, unsigned int chksum_addr, unsigned int num_of_word, unsigned short *chksum);
int __stdcall GPS_Brom_JumpCmd(unsigned int brom_handle, HANDLE hCOM, unsigned int jump_addr, unsigned char bWaitForAck);
int __stdcall GPS_Brom_DownloadData(unsigned int brom_handle, HANDLE hCOM, unsigned int base_addr, const unsigned char *buf_in, unsigned int num_of_byte, GPS_CALLBACK_BROM_WRITE_BUF_INIT  cb_init,  void *cb_init_arg, GPS_CALLBACK_BROM_WRITE_BUF  cb,  void *cb_arg);

//int __stdcall Brom_SetBaudrate(unsigned int brom_handle, HANDLE hCOM, unsigned int base_addr, const unsigned char *buf_in, unsigned int num_of_byte, CALLBACK_BROM_WRITE_BUF_INIT  cb_init,  void *cb_init_arg, CALLBACK_BROM_WRITE_BUF  cb,  void *cb_arg);

//int __stdcall Brom_SetPMICte(unsigned int brom_handle, HANDLE hCOM, unsigned int base_addr, const unsigned char *buf_in, unsigned int num_of_byte, CALLBACK_BROM_WRITE_BUF_INIT  cb_init,  void *cb_init_arg, CALLBACK_BROM_WRITE_BUF  cb,  void *cb_arg);

#ifdef	__cplusplus
}
#endif

#endif


