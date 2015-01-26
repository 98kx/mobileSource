/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>


bool SearchNewCOMPort(unsigned short &comPortNum);

unsigned int GetFileSize(const std::string &filename);
unsigned char* GetFileContent(const std::string &filename);

DWORD GetParentProcessId();

#endif // _UTILITY_H
