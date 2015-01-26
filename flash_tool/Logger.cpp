/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>
#include "Logger.h"


namespace ConsoleMode
{
    Logger logger;
}

using ConsoleMode::Logger;
using std::ios_base;
using std::ostream;

Logger::Logger()
    : m_BaseLevel(Level_Normal),
      m_CurrentLevel(Level_Normal)
{
}

Logger::~Logger()
{
}

Logger& Logger::operator<<(ostream& (*f)(ostream&))
{
    if (!Suppressed())
    {
        std::cout << f;
    }

    return *this;
}

Logger& Logger::operator<<(ios_base& (*f)(ios_base&))
{
    if (!Suppressed())
    {
        std::cout << f;
    }

    return *this;
}

Logger& Logger::operator<<(Level level)
{
    m_CurrentLevel = level;
    return *this;
}

void Logger::SetBaseLevel(Level level)
{
    m_BaseLevel = level;
}

bool Logger::Suppressed()
{
    return (m_BaseLevel == Level_Quiet) ||
           (m_CurrentLevel == Level_Quiet) ||
           (m_CurrentLevel > m_BaseLevel);
}
