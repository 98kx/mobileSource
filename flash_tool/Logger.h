/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _LOGGER_H
#define _LOGGER_H

#include <iostream>


namespace ConsoleMode
{
    class Logger
    {
    public:
        enum Level
        {
            Level_Quiet = 0,
            Level_Normal,
            Level_Verbose
        };

        Logger();
        ~Logger();

    public:
        // For general objects
        template <typename T>
        Logger& operator<<(const T &obj)
        {
            if (!Suppressed())
            {
                std::cout << obj;
            }

            return *this;
        }

        // For manipulators
        Logger& operator<<(std::ostream& (*f)(std::ostream&));
        Logger& operator<<(std::ios_base& (*f)(std::ios_base&));

        // For setting the current logging level
        Logger& operator<<(Level level);

        void SetBaseLevel(Level level);

    private:
        bool Suppressed();

    private:
        Level m_BaseLevel;
        Level m_CurrentLevel;
    };

    extern Logger logger;
}

#endif // _LOGGER_H
