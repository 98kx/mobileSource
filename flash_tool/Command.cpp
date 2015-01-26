/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include "Command.h"

// git test 222 .
// git test 
Command::Command()
{
}

Command::~Command()
{
}

MacroCommand::MacroCommand()
    : m_Commands()
{
}

MacroCommand::~MacroCommand()
{
}

void MacroCommand::Add(const SharedPtr<Command> &command)
{
    m_Commands.push_back(command);
}

void MacroCommand::Remove(const SharedPtr<Command> &command)
{
    m_Commands.remove(command);
}

bool MacroCommand::Execute()
{
    Commands::const_iterator it;

    for (it=m_Commands.begin(); it!=m_Commands.end(); ++it)
    {
        if (!((*it)->Execute()))
        {
            return false;
        }
    }

    return true;
}
