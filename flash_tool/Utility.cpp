/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vcl.h>
#include <vector>

#include "com_enum.h"


/**
 * Search for a newly created COM port.
 *
 * @param newCOMPortNum [out] a newly created COM port's number
 *
 * @return true for success or false for failure
 */
bool SearchNewCOMPort(unsigned short &newCOMPortNum)
{
    using std::mismatch;
    using std::pair;
    using std::vector;

    vector<unsigned short> oldCOMPortList(255);
    unsigned short oldNumCOMPorts = oldCOMPortList.size();

    // Retrieve the original COM port list
    if (ComPortEnumerate(&oldCOMPortList[0], &oldNumCOMPorts) != COM_ENUM_OK)
    {
        return false;
    }

    const DWORD startTick = ::GetTickCount();

    while (::GetTickCount() - startTick < 30000)
    {
        vector<unsigned short> newCOMPortList(255);
        unsigned short numCOMPorts = newCOMPortList.size();

        // Retrieve the new COM port list
        if (ComPortEnumerate(&newCOMPortList[0], &numCOMPorts) != COM_ENUM_OK)
        {
            return false;
        }

        if (numCOMPorts < oldNumCOMPorts)
        {
            oldCOMPortList.swap(newCOMPortList);
            continue;
        }

        // Compare the old list with the new list
        typedef vector<unsigned short>::iterator COMIter;

        pair<COMIter, COMIter> result = mismatch(newCOMPortList.begin(),
                                                 newCOMPortList.end(),
                                                 oldCOMPortList.begin());

        // Return the first new COM port's number, if any.
        if (result.first != newCOMPortList.end())
        {
            newCOMPortNum = *(result.first);
            return true;
        }
    }

    return false;
}


/**
 * Get the size of the given file.
 *
 * @param filename a filename
 *
 * @return size of the given file
 */
unsigned int GetFileSize(const std::string &filename)
{
    //
    // FIXME: Should use some filesystem API instead
    //
    unsigned int size;
    std::ifstream is(filename.c_str(), std::ios::in | std::ios::binary);

    is.seekg(0, std::ios::end);
    size = is.tellg();
    is.close();

    return size;
}

/**
 * Get the content of the given file. The caller is responsible for memory
 * deallocation.
 *
 * @param filename a filename
 *
 * @return content of the given file
 */
unsigned char* GetFileContent(const std::string &filename)
{
    std::ifstream is(filename.c_str(), std::ios::in | std::ios::binary);
    is.seekg(0, std::ios::end);

    const unsigned int size = is.tellg();
    unsigned char *content = new unsigned char[size];

    is.seekg(0, std::ios::beg);
    is.read(content, size);
    is.close();

    return content;
}


struct PROCESS_BASIC_INFORMATION
{
    PVOID Reserved1;
    void* PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    ULONG_PTR ParentProcessId;
};

// GetParentProcessId will use the NtQueryInformationProcess function
// exported by NtDll to retrieve the parent process id for the current
// process and if for some reason it doesn't work, it returns 0
DWORD GetParentProcessId()
{
    // Much easier in ASM but C/C++ looks so much better
    typedef ULONG NTSTATUS;
    typedef NTSTATUS (WINAPI *pNtQueryInformationProcess)
                                (HANDLE ,UINT ,PVOID ,ULONG , PULONG);

    // Some locals
    PROCESS_BASIC_INFORMATION pbi;
    ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));

    // Get NtQueryInformationProcess
    pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
                            GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")),
                                           "NtQueryInformationProcess");

    // Sanity check although there's no reason for it to have failed
    if (NtQIP == 0)
    {
        return 0;
    }

    // Now we can call NtQueryInformationProcess, the second
    // param 0 == ProcessBasicInformation
    NTSTATUS status = NtQIP(GetCurrentProcess(), 0, (void*) &pbi,
                            sizeof(PROCESS_BASIC_INFORMATION), 0);

    if (status != 0x00000000)
    {
        return 0;
    }
    else
    {
        return pbi.ParentProcessId;
    }
}
