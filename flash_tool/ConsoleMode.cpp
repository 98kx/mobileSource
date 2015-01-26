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
#include <exception>
#include <iostream>
#include <string>
#include <vcl.h>

#include "Config.h"
#include "Command.h"
#include "CommandLineArguments.h"
#include "Logger.h"
#include "Utility.h"

#include "flashtool.h"
#include "meta.h"

namespace ConsoleMode
{
    int Run(const std::string &commandLine)
    {
        const DWORD parentProcessID = GetParentProcessId();

        if (parentProcessID == 0)
        {
            return 1;
        }

        if (!AttachConsole(parentProcessID))
        {
            return 2;
        }

        try
        {
            USES_CONVERSION;
            const CommandLineArguments commandLineArguments(
                                            A2CW(commandLine.c_str()));

            logger.SetBaseLevel(commandLineArguments.GetLoggingLevel());

            META_Init(NULL);
            Brom_Debug_SetLogFilename(
                    W2CA(commandLineArguments.GetLogFilename().c_str()));
            META_DebugOn_With_FilePath(
                    W2CA(commandLineArguments.GetMETALogFilename().c_str()));
            Brom_DebugOn();
            META_DebugOn();

			// Parse the xml file in Config constructor
            const Config config(commandLineArguments);
            config.Dump();

			// Create all the commands accordingly and then execute one by one
            if (!config.CreateCommand()->Execute())
            {
                META_Deinit();
                return 3;
            }

            META_Deinit();

            logger << Logger::Level_Normal
                   << "All operations are done." << endl;
        }
        catch (std::exception &ex)
        {
            logger << Logger::Level_Normal
                   << "[Error] " << ex.what() << endl;
            return 4;
        }

        return 0;
    }
}
