/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _COMMAND_H
#define _COMMAND_H

#include <list>
#include "SharedPtr.h"


class Command 
{
protected:
    Command();

public:
    virtual ~Command();
    virtual bool Execute() = 0;
};

class MacroCommand : public Command
{
public:
    MacroCommand();
    virtual ~MacroCommand();

public:
    void Add(const SharedPtr<Command> &command);
    void Remove(const SharedPtr<Command> &command);
    virtual bool Execute();

private:
    typedef std::list<SharedPtr<Command> > Commands;
    Commands m_Commands;
};

#endif // _COMMAND_H 
