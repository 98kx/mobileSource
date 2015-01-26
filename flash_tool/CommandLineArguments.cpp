/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include <atlbase.h>
#include <iostream>
#include <stdexcept>

#include "CommandLineArguments.h"
#include "argcargv.h"
#include "XGetopt.h"


using ConsoleMode::CommandLineArguments;
using ConsoleMode::InvalidCommandLineArgument;
using ConsoleMode::Logger;
using std::string;
using std::wstring;

const string CONSOLE_MODE_USAGE(
    "\nUsage: Flash_tool.exe OPTION...\n"
    "Run FlashTool in console mode.\n"
    "Example: Flash_tool.exe -i config_file\n"
    "\nMandatory:\n"
    "  -i    console mode configuration file\n"
    "\nOverwriting settings specified in the configuration file:\n"
    "  -p    COM port\n"
    "  -d    download agent file path\n"
    "  -s    scatter file path\n"
    "\nMiscellaneous:\n"
    "  -o    specify a BROM DLL log file path (default: C:\\BROM_DLL.log)\n"
    "  -m    specify a META DLL log file path (default: C:\\META_DLL.log)\n"
    "  -q    suppress console mode messages\n"
    "  -v    show all console mode messages\n"
    "  -h    display this help and exit\n"
);

CommandLineArguments::CommandLineArguments(const wstring &commandLine)
    : m_ConfigFilename(),
      m_ComPort(),
      m_DownloadAgentFilename(),
      m_ScatterFilename(),
      m_LogFilename(L"C:\\BROM_DLL.log"),
      m_METALogFilename(L"C:\\META_DLL.log"),
      m_LoggingLevel(Logger::Level_Normal)
{
    Parse(commandLine);
}

wstring CommandLineArguments::GetConfigFilename() const
{
    return m_ConfigFilename;
}

wstring CommandLineArguments::GetComPort() const
{
    return m_ComPort;
}

wstring CommandLineArguments::GetDownloadAgentFilename() const
{
    return m_DownloadAgentFilename;
}

wstring CommandLineArguments::GetScatterFilename() const
{
    return m_ScatterFilename;
}

wstring CommandLineArguments::GetLogFilename() const
{
    return m_LogFilename;
}

wstring CommandLineArguments::GetMETALogFilename() const
{
    return m_METALogFilename;
}

Logger::Level CommandLineArguments::GetLoggingLevel() const
{
    return m_LoggingLevel;
}

//
// TODO:
//   T:\Flash Tool\Flash_tool.exe -i test.xml
//          ^^^
//      additional spaces lead to parse error
//
void CommandLineArguments::Parse(const wstring &commandLine)
{
    USES_CONVERSION;
    const int argc = _ConvertCommandLineToArgcArgv(commandLine.c_str());

    while (1)
    {
        int c = getopt(argc, _ppszArgv, L"i:p:d:s:o:m:qvh");

        if (c == EOF)
        {
            break;
        }

        switch (c)
        {
            case L'i':
                m_ConfigFilename = optarg;
                break;

            case L'p':
                m_ComPort = optarg;
                break;

            case L'd':
                m_DownloadAgentFilename = optarg;
                break;

            case L's':
                m_ScatterFilename = optarg;
                break;

            case L'o':
                m_LogFilename = optarg;
                break;
                            
            case L'm':
                m_METALogFilename = optarg;
                break;

            case L'q':
                m_LoggingLevel = Logger::Level_Quiet;
                break;

            case L'v':
                m_LoggingLevel = Logger::Level_Verbose;
                break;

            case L'h':
                std::cout << CONSOLE_MODE_USAGE;
                exit(0);

            default:
                std::cout << CONSOLE_MODE_USAGE;
                exit(-1);
        }
    }

    if (m_ConfigFilename.size() == 0)
    {
        throw InvalidCommandLineArgument(
                "Please specify a configuration file with the -i option");
    }
}

InvalidCommandLineArgument::InvalidCommandLineArgument(const string &what)
    : std::logic_error(what)
{
}
