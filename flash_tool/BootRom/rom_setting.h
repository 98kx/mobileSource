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
 *  rom_setting.h
 *
 * Project:
 * --------
 *  Flash Download/Readback/Format Library.
 *
 * Description:
 * ------------
 *  ROM setting
 *
 * Author:
 * -------
 *  Owen Chu (mtk02172)
 *
 ****************************************************************************/
#ifndef _ROM_SETTING_H_
#define _ROM_SETTING_H_

#include "flashtool_handle.h"   // For BBCHIP_TYPE and _BOOL

#define MAX_NUM_ROM_FILES       MAX_LOAD_SECTIONS
#define MAX_ROM_NAME_LEN        128
#define MAX_ROM_FILE_PATH_LEN   512

#ifdef __cplusplus
extern "C" {
#endif


//
// ROMSetting_v01
//
typedef struct ROMSetting_v01
{
    unsigned int numValidEntries;

    char regionName[MAX_NUM_ROM_FILES][MAX_ROM_NAME_LEN + 1];

    // Load address
    unsigned int loadAddr[MAX_NUM_ROM_FILES];
    _BOOL isOffset[MAX_NUM_ROM_FILES];

} ROMSetting_v01;


//
// ROMSetting_v02
//
typedef enum Alignment
{
    Alignment_Invalid = 0,
    Alignment_Block,
    Alignment_Page

} Alignment;


typedef struct BootRegionSetting
{
    Alignment alignment;
    char romFilePath[MAX_NUM_ROM_FILES][MAX_ROM_FILE_PATH_LEN + 1];

} BootRegionSetting;


typedef struct ControlBlockRegionSetting
{
    char romFilePath[MAX_NUM_ROM_FILES][MAX_ROM_FILE_PATH_LEN + 1];

} ControlBlockRegionSetting;


typedef struct MainRegionSetting
{
    Alignment alignment;
    char romFilePath[MAX_NUM_ROM_FILES][MAX_ROM_FILE_PATH_LEN + 1];

} MainRegionSetting;


typedef struct ROMSetting_v02
{
    BBCHIP_TYPE chipType;
    BootRegionSetting bootRegion;
    ControlBlockRegionSetting controlBlockRegion;
    MainRegionSetting mainRegion;

} ROMSetting_v02;


//
// ROMSetting
//
typedef struct ROMSetting
{
    unsigned int version;

    union
    {
        ROMSetting_v01 v01;
        ROMSetting_v02 v02;
    } u;

} ROMSetting;


#ifdef __cplusplus
}
#endif

#endif // _ROM_SETTING_H_
