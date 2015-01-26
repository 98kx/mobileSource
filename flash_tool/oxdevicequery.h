/*******************************************************************************
 * oxdevicequery.h
 *______________________________________________________________________________
 *
 * Oxford Semiconductor Proprietary and Confidential Information
 *
 * Copyright (C) 2005-2006 Oxford Semiconductor Ltd.
 *
 * NOTE: THIS FILE EXISTS IN TWO CVS REPOSITORIES & MUST BE KEPT THE SAME. THESE
 * FILES ARE IN THE FOLLOWING DIRECTORIES:
 *       /tech/sw_uarts/root/driver_src/w2kxpx64/COMMON
 *       /chips/cf952/root/software/OxCFU950/src
 *
 *******************************************************************************
 */

//
// The device query defines of type ID's and structures for query for the
// presence and EPROM IO interfaces of Oxford Semiconductors devices.
//

#ifndef _OX_DEVICE_QUERY

#define _OX_DEVICE_QUERY

//------------------------------------------------------------------------------


#define CTL_CODE(t,f,m,a) 			(((t)<<16)|((a)<<14)|((f)<<2)|(m))
#define FILE_DEVICE_SERIAL_PORT		27
#define METHOD_BUFFERED		 		0
#define FILE_READ_ACCESS		 	0x00000001

#define OXCF950_CHIP_ID   			0x16C95008

//------------------------------------------------------------------------------
// IOCTL code for serial driver and multifunction driver.

#define IOCTL_OXSER_QUERY_INFO \
    CTL_CODE( FILE_DEVICE_SERIAL_PORT, 63, METHOD_BUFFERED, FILE_READ_ACCESS )

//------------------------------------------------------------------------------
// IOCTL code for USB driver.

// See the usbioctl.h for the reserved space of IODEF.
#define RESERVED_AREA_OF_USB_IOCTL        300
#define IOCTL_CUSTOM_BIT                  (1<<11)
//#define OXCFU950_IOCTL_BASE             RESERVED_AREA_OF_USB_IOCTL
#define OXCFU950_IOCTL_BASE               0
#define OXCFU950_IOCTL_FUNCTION           (IOCTL_CUSTOM_BIT|OXCFU950_IOCTL_BASE)

// The first IOCTL code is reserved for use by OxProg. For the other codes, see
// IOCTL.h in the source of the CFU950 USB driver.
#define OXCFU950_QUERY_INFO                 (OXCFU950_IOCTL_FUNCTION+0)


//------------------------------------------------------------------------------
// Define the card type enumerations (as put in dwRequestedType in the structure
// defined below).

// The serial driver only processes the IOCTL if it contains this code:
#define     OX_FOR_SERIAL_DRIVER              0
// The multifunction only processes the IOCTL if it contains this code:
#define     OX_FOR_MULTIFUNCTION_DRIVER       1
// The USB driver only processes the IOCTL if it contains this code:
#define     OX_FOR_USB_DRIVER                 2
// The USB INFO ONLY driver only processes the IOCTL if it contains this code:
#define     OX_FOR_USB_INFO_ONLY_DRIVER       3


//------------------------------------------------------------------------------
// We set this number in the dwMagicNumber Field as a valid response check.
//

#define OXMAGICNUMBER       0x01020304

//------------------------------------------------------------------------------
// Whether it is an IO or memory mapped port (goes in the dwIoType field).
//

#define IO_TYPE             0
#define MEMORY_TYPE         1

//------------------------------------------------------------------------------

#define OX_DEVICE_ID_SIZE   512
#define OX_RESOURCES_SIZE   2048

struct OxQueryDeviceInfo {
    // To be filled in by the caller.
    ULONG   dwRequestedType;
    ULONG   dwIoType;
    // The version of the interface.
    ULONG   dwVersion;
    TCHAR   DeviceId[OX_DEVICE_ID_SIZE];
    // Number of bytes of data recorded in Resources.
    ULONG   dwResourceLength;
    // The content of Resources depends on the type of device, which can be
    // determined by looking at ChipId.
    unsigned char Resources[OX_RESOURCES_SIZE];
    ULONG   dwMagicNumber;
    // The four bytes read from the UART - e.g. 0x16, 0xC9, 0x50, 0x0A for the
    // mPCI954. This is also filled in by the USB driver according to the
    // version number in the UART part of the device.
    ULONG   ChipId;
};

//------------------------------------------------------------------------------
// If the device is a CFU950, the Resources field of the OxQueryDeviceInfo
// structure is as follows:
/*
typedef struct _CFU950_RESOURCE_INFO {

    PHYSICAL_ADDRESS         ioBase_;
    ULONG                    ioLength_;

    PHYSICAL_ADDRESS         memBase_;
    ULONG                    memLength_;

} CFU950_RESOURCE_INFO, *PCFU950_RESOURCE_INFO;
*/


#define IOCTL_OXSER_CF950_GPIO \
    CTL_CODE( FILE_DEVICE_SERIAL_PORT, 62, METHOD_BUFFERED, FILE_READ_ACCESS )


#endif


/*-----------------------------------------------------------------------------*
 *                           End of oxdevicequery.h                            *
 *-----------------------------------------------------------------------------*
 */

