/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _COMMAND_LINE_ARGUMENTS_H
#define _COMMAND_LINE_ARGUMENTS_H

#include <stdexcept>
#include <string>
#include "Logger.h"


namespace ConsoleMode
{
    class CommandLineArguments
    {
    public:
        explicit CommandLineArguments(const std::wstring &commandLine);

    public:
        std::wstring GetConfigFilename() const;
        std::wstring GetComPort() const;
        std::wstring GetDownloadAgentFilename() const;
        std::wstring GetScatterFilename() const;
        std::wstring GetLogFilename() const;
        std::wstring GetMETALogFilename() const;
        Logger::Level GetLoggingLevel() const;

    private:
        void Parse(const std::wstring &commandLine);

    private:
        std::wstring m_ConfigFilename;
        std::wstring m_ComPort;
        std::wstring m_DownloadAgentFilename;
        std::wstring m_ScatterFilename;
        std::wstring m_LogFilename;
        std::wstring m_METALogFilename;
        Logger::Level m_LoggingLevel;
    };

    class InvalidCommandLineArgument : public std::logic_error
    {
    public:
        InvalidCommandLineArgument(const std::string &what);
    };
}

#endif // _COMMAND_LINE_ARGUMENTS_H
