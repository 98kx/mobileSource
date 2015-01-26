/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _SETTING_H
#define _SETTING_H

#include <string>
#include <vector>
#include "DOWNLOAD.H"
#include "SharedPtr.h"
#include "XMLSerializable.h"
#include "flashtool_api.h"


class Command;
class XML::Node;

namespace ConsoleMode
{
    //##################################################################
    // GeneralSetting
    //##################################################################
    class GeneralSetting : public XML::Serializable
    {
    public:
        explicit GeneralSetting(const XML::Node &node);
        GeneralSetting(const std::wstring &comPort,
            bool checkBasebandVersion,
            const std::wstring &downloadAgentFilePath,
            const std::wstring &authenticationFilePath);
        virtual ~GeneralSetting();

    public:
        std::wstring GetComPort() const;
        void SetComPort(const std::wstring &comPort);

        bool IsCheckBasebandVersionEnabled() const;

        std::wstring GetDownloadAgentFilePath() const;
        void SetDownloadAgentFilePath(
            const std::wstring &downloadAgentFilePath);

        std::wstring GetAuthenticationFilePath() const;
        void Dump() const;

        // XML::Serializable interface
    public:
        virtual void LoadXML(const XML::Node &node);
        virtual void SaveXML(XML::Node &node) const;

    private:
        std::wstring m_ComPort;
        bool m_CheckBasebandVersion;
        std::wstring m_DownloadAgentFilePath;
        std::wstring m_AuthenticationFilePath;
    };

    class CommandSetting : public XML::Serializable
    {
    public:
        virtual ~CommandSetting() {}

    protected:
        CommandSetting() {}

    public:
        virtual SharedPtr<Command> CreateCommand(
            const GeneralSetting &generalSetting) const = 0;

        // Virtual copy constructor
        virtual CommandSetting* Clone() const = 0;

        // For debugging purpose
        virtual void Dump() const = 0;
    };

    //##################################################################
    // FormatOptions
    //##################################################################
    struct FormatOptions
    {
    public:
        FormatOptions();
        FormatOptions(FORMAT_ACTION_TYPE_E actionType,
            HW_StorageType_E storageType,
            unsigned int manualFormatBeginAddress,
            unsigned int manualFormatLength);

        bool IsAutoFormatEnabled() const;
        bool IsManualFormatEnabled() const;

        void LoadManualFormatOptions(const XML::Node &node);
        void SaveManualFormatOptions(XML::Node &node) const;

    public:
        // For new BROM DLL interfaces, more format options are provided,
        // one boolean variable to indicate the format action is not enough.
        // Modify to use FORMAT_ACTION_TYPE_E in stead.
        // bool m_AutoFormatEnabled;
        FORMAT_ACTION_TYPE_E m_FormatAction;

        HW_StorageType_E m_StorageType;
        unsigned int m_ManualFormatBeginAddress;
        unsigned int m_ManualFormatLength;
    };

    //##################################################################
    // DownloadSetting
    //##################################################################
    class DownloadSetting : public CommandSetting
    {
    public:
        explicit DownloadSetting(const XML::Node &node);
        DownloadSetting(const std::wstring &scatterFilePath,
            const std::vector<std::wstring> &downloadList,
            const FormatOptions &formatOptions,
            bool enableWatchdogTimer);
        virtual ~DownloadSetting();

    public:
        std::wstring GetScatterFilePath() const;
        void SetScatterFilePath(const std::wstring &scatterFilePath);

        std::vector<std::wstring> GetDownloadList() const;
        FormatOptions GetFormatOptions() const;
        bool IsWatchdogTimerEnabled() const;

        // XML::Serializable interface
    public:
        virtual void LoadXML(const XML::Node &node);
        virtual void SaveXML(XML::Node &node) const;

        // CommandSetting interface
    public:
        virtual SharedPtr<Command> CreateCommand(
            const GeneralSetting &generalSetting) const;
        virtual DownloadSetting* Clone() const;
        virtual void Dump() const;

    private:
        void LoadDownloadList(const XML::Node &node);

    private:
        std::wstring m_ScatterFilePath;
        std::vector<std::wstring> m_DownloadList;
        FormatOptions m_FormatOptions;
        bool m_WatchdogTimerEnabled;
    };

    //##################################################################
    // FormatSetting
    //##################################################################
    class FormatSetting : public CommandSetting
    {
    public:
        explicit FormatSetting(const XML::Node &node);
        FormatSetting(const FormatOptions &formatOptions,
            bool enableWatchdogTimer);
        virtual ~FormatSetting();

    public:
        FormatOptions GetFormatOptions() const;
        bool IsWatchdogTimerEnabled() const;

        // XML::Serializable interface
    public:
        virtual void LoadXML(const XML::Node &node);
        virtual void SaveXML(XML::Node &node) const;

        // CommandSetting interface
    public:
        virtual SharedPtr<Command> CreateCommand(
            const GeneralSetting &generalSetting) const;
        virtual FormatSetting* Clone() const;
        virtual void Dump() const;

    private:
        FormatOptions m_FormatOptions;
        bool m_WatchdogTimerEnabled;
    };

    //##################################################################
    // ResetToFactoryDefaultSetting
    //##################################################################
    class ResetToFactoryDefaultSetting : public CommandSetting
    {
    public:
        explicit ResetToFactoryDefaultSetting(const XML::Node &node);
        explicit ResetToFactoryDefaultSetting(bool enableWatchdogTimer);
        virtual ~ResetToFactoryDefaultSetting();

    public:
        bool IsWatchdogTimerEnabled() const;

        // XML::Serializable interface
    public:
        virtual void LoadXML(const XML::Node &node);
        virtual void SaveXML(XML::Node &node) const;

        // CommandSetting interface
    public:
        virtual SharedPtr<Command> CreateCommand(
            const GeneralSetting &generalSetting) const;
        virtual ResetToFactoryDefaultSetting* Clone() const;
        virtual void Dump() const;

    private:
        bool m_WatchdogTimerEnabled;
    };

    //##################################################################
    // BackupSetting
    //##################################################################
    class BackupSetting : public CommandSetting
    {
    public:
        explicit BackupSetting(const XML::Node &node);
        BackupSetting(const std::wstring &iniFilePath,
            const std::wstring &backupDirectoryPath,
            bool enableWatchdogTimer);
        virtual ~BackupSetting();

    public:
        std::wstring GetIniFilePath() const;
        std::wstring GetBackupDirectoryPath() const;
        bool IsWatchdogTimerEnabled() const;

        // XML::Serializable interface
    public:
        virtual void LoadXML(const XML::Node &node);
        virtual void SaveXML(XML::Node &node) const;

        // CommandSetting interface
    public:
        virtual SharedPtr<Command> CreateCommand(
            const GeneralSetting &generalSetting) const;
        virtual BackupSetting* Clone() const;
        virtual void Dump() const;

    private:
        std::wstring m_IniFilePath;
        std::wstring m_BackupDirectoryPath;
        bool m_WatchdogTimerEnabled;
    };

    //##################################################################
    // RestoreSetting
    //##################################################################
    class RestoreSetting : public CommandSetting
    {
    public:
        explicit RestoreSetting(const XML::Node &node);
        RestoreSetting(const std::wstring &iniFilePath,
            const std::wstring &restoreDirectoryPath,
            bool enableWatchdogTimer);
        virtual ~RestoreSetting();

    public:
        std::wstring GetIniFilePath() const;
        std::wstring GetRestoreDirectoryPath() const;
        bool IsWatchdogTimerEnabled() const;

        // XML::Serializable interface
    public:
        virtual void LoadXML(const XML::Node &node);
        virtual void SaveXML(XML::Node &node) const;

        // CommandSetting interface
    public:
        virtual SharedPtr<Command> CreateCommand(
            const GeneralSetting &generalSetting) const;
        virtual RestoreSetting* Clone() const;
        virtual void Dump() const;

    private:
        std::wstring m_IniFilePath;
        std::wstring m_RestoreDirectoryPath;
        bool m_WatchdogTimerEnabled;
    };
}

#endif // _SETTING_H
