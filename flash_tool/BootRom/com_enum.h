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
 *  com_enum.h
 *
 * Project:
 * --------
 *  BootRom Library
 *
 * Description:
 * ------------
 *  Enumerate serial ports.
 *
 * Author:
 * -------
 *  Amos Hsu (mtk00539)
 *
 *******************************************************************************/
#ifndef _COM_ENUM_H_
#define _COM_ENUM_H_

#define	COM_ENUM_OK						0
#define	COM_ENUM_INVALID_ARGUMENT		1
#define	COM_ENUM_NO_COM_PORT_FOUND		2
#define	COM_ENUM_COM_ARRAY_TOO_SMALL	3
#define	COM_ENUM_ERROR					4

typedef enum {
	COM_ENUM_BY_CREATEFILE = 0,
	COM_ENUM_BY_REGISTRY
} COM_ENUM_METHOD;

#ifdef	__cplusplus
extern "C" {
#endif

extern int __stdcall ComPortEnumerate(unsigned short *p_com_port_array, unsigned short *p_com_port_count);
extern int __stdcall ComPortEnumerateWithPrefix(unsigned short *p_com_port_array, unsigned short *p_com_port_count, const char *registryValuePrefix);
extern int __stdcall ComPortEnumerateEx(COM_ENUM_METHOD  method, unsigned short *p_com_port_array, unsigned short *p_com_port_count);
extern int __stdcall UsbComPortEnumerate(unsigned short *p_com_port_array, unsigned short *p_com_port_count);

#ifdef	__cplusplus
}
#endif

#endif
