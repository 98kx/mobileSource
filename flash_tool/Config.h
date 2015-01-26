/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include <vector>
#include "Setting.h"
#include "SharedPtr.h"


class Command;

namespace ConsoleMode
{
    class CommandLineArguments;

    class Config
    {
    public:
        Config();
        explicit Config(const CommandLineArguments &commandLineArguments);
        ~Config();

    public:
        void LoadFile(const std::wstring &filename);
        void SaveFile(const std::wstring &filename) const;

        void SetGeneralSetting(const GeneralSetting &generalSetting);
        void AddCommandSetting(const CommandSetting &commandSetting);
        SharedPtr<Command> CreateCommand() const;
        void Dump() const;

    private:
        // FIXME
        GeneralSetting *m_GeneralSetting;

        typedef std::vector<SharedPtr<CommandSetting> > CommandSettings;
        CommandSettings m_CommandSettings;
    };
}

#endif // _CONFIG_H
