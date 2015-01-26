/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2010
 *
 ****************************************************************************/

/*****************************************************************************
 * Filename:
 * ---------
 *  external_memory_setting.h
 *
 * Project:
 * --------
 *  Flash Download/Readback/Format Library.
 *
 * Description:
 * ------------
 *  External memory setting
 *
 * Author:
 * -------
 *  Owen Chu (mtk02172)
 *
 ****************************************************************************/
#ifndef _EXTERNAL_MEMORY_SETTING_H_
#define _EXTERNAL_MEMORY_SETTING_H_

#include "flashtool_handle.h"   // For DRAM_SETTING and _BOOL

// The current maximum number of MCPs supported in
// a Combo-MCP-enabled MAUI load is around 5
#define MAX_NUM_FLASH_INFO_ENTRIES              8
#define MAX_NUM_EXTERNAL_RAM_SETTING_ENTRIES    MAX_NUM_FLASH_INFO_ENTRIES

#ifdef __cplusplus
extern "C" {
#endif


//
// Flash information
//

typedef enum FLASHType
{
    FLASHType_Invalid = 0,
    FLASHType_NAND,
    FLASHType_NOR,
    FLASHType_SERIAL_NOR_FLASH,
    FLASHType_End = 0x42424242

} FLASHType;

typedef struct flashID
{
	unsigned int idNumber; // max = 8
	unsigned char id[8];
}flashID;


// NOR
typedef struct FlashInfo_v01
{
    flashID NOR_ID;
    struct { int dummy; } NOR_geometry;

} FlashInfo_v01;


// NAND
typedef struct FlashInfo_v02
{
    flashID NAND_ID;
    struct { int dummy; } NAND_Geometry;

} FlashInfo_v02;


typedef struct FlashInfo
{
    unsigned int version;
    unsigned int numValidEntries;
    FLASHType flashType;

    union
    {
        FlashInfo_v01 v01[MAX_NUM_FLASH_INFO_ENTRIES];
        FlashInfo_v02 v02[MAX_NUM_FLASH_INFO_ENTRIES];
    } u;

} FlashInfo;


//
// External RAM setting
//
typedef enum DRAMType
{
    DRAMType_Invalid = 0,
    DRAMType_DDR,
    DRAMType_DDR2,
    DRAMType_DDR_166M,
    DRAMType_DDR_200M,
    DRAMType_DDR2_166M,
    DRAMType_DDR2_200M,
    DRAMType_End = 0x42424242

} DRAMType;


typedef struct DRAMSetting_v03
{
    DRAMType ramType;
    unsigned int EMI_CONI_Value;
    unsigned int EMI_CONJ_Value;
    unsigned int EMI_CONK_Value;
    unsigned int EMI_CONL_Value;
    unsigned int EMI_CONN_Value;
    unsigned int EMI_DQSA_Value;
    unsigned int EMI_DRVA_Value;
    unsigned int EMI_DRVB_Value;
    unsigned int EMI_ODLA_Value;
    unsigned int EMI_ODLB_Value;
    unsigned int EMI_ODLC_Value;
    unsigned int EMI_ODLD_Value;
    unsigned int EMI_ODLE_Value;
    unsigned int EMI_ODLG_Value;
} DRAMSetting_v03;

typedef struct DRAMSetting_v05
{
    DRAMType ramType;
    unsigned int EMI_CONI_Value;
    unsigned int EMI_CONJ_Value;
    unsigned int EMI_CONK_Value;
    unsigned int EMI_CONL_Value;
    unsigned int EMI_CONN_Value;
    // remove DQSA
    unsigned int EMI_DRVA_Value;
    unsigned int EMI_DRVB_Value;
    unsigned int EMI_ODLA_Value;
    unsigned int EMI_ODLB_Value;
    unsigned int EMI_ODLC_Value;
    unsigned int EMI_ODLD_Value;
    unsigned int EMI_ODLE_Value;
    unsigned int EMI_ODLF_Value; //New
    unsigned int EMI_ODLG_Value;
    // new 6 register
    unsigned int EMI_DUTA_Value;
    unsigned int EMI_DUTB_Value;
    unsigned int EMI_DUTC_Value;
    unsigned int EMI_DUCA_Value;
    unsigned int EMI_DUCB_Value;
    unsigned int EMI_DUCE_Value;
    unsigned int EMI_IOCL_Value;
} DRAMSetting_v05;


typedef struct ExternalRAMSetting
{
    unsigned int version;
    unsigned int numValidEntries;

    union
    {
        DRAM_SETTING    v00[MAX_NUM_EXTERNAL_RAM_SETTING_ENTRIES];
        DRAMSetting_v03 v01[MAX_NUM_EXTERNAL_RAM_SETTING_ENTRIES];
        DRAMSetting_v05 v05[MAX_NUM_EXTERNAL_RAM_SETTING_ENTRIES];
    } u;

} ExternalRAMSetting;


//
// External memory setting
//
typedef struct ExternalMemorySetting
{
    _BOOL               valid;
    BBCHIP_TYPE         bbchipType;
    ExternalRAMSetting  ramSetting;
    FlashInfo           flashInfo;  // Required for Combo MCP

} ExternalMemorySetting;


#ifdef __cplusplus
}
#endif

#endif // _EXTERNAL_MEMORY_SETTING_H_
