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
#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include "FlashToolCommand.h"
#include "Logger.h"
#include "Setting.h"
#include "XMLNode.h"


using ConsoleMode::BackupSetting;
using ConsoleMode::CommandSetting;
using ConsoleMode::FormatOptions;
using ConsoleMode::DownloadSetting;
using ConsoleMode::FormatSetting;
using ConsoleMode::GeneralSetting;
using ConsoleMode::ResetToFactoryDefaultSetting;
using ConsoleMode::RestoreSetting;
using XML::Node;
using std::dec;
using std::endl;
using std::hex;
using std::setfill;
using std::setw;
using std::vector;
using std::wostringstream;
using std::wstring;

static void ThrowSettingError(const wstring &nodeName)
{
    USES_CONVERSION;
    throw std::runtime_error(std::string("Unrecognized setting: ") +
        W2CA(nodeName.c_str()));
}

//##################################################################
// GeneralSetting
//##################################################################

GeneralSetting::GeneralSetting(const Node &node)
: m_ComPort(),
m_CheckBasebandVersion(true),
m_DownloadAgentFilePath(),
m_AuthenticationFilePath()
{
    LoadXML(node);
}

GeneralSetting::GeneralSetting(const wstring &comPort,
                               bool checkBasebandVersion,
                               const wstring &downloadAgentFilePath,
                               const wstring &authenticationFilePath)
                               : m_ComPort(comPort),
                               m_CheckBasebandVersion(checkBasebandVersion),
                               m_DownloadAgentFilePath(downloadAgentFilePath),
                               m_AuthenticationFilePath(authenticationFilePath)
{
}

GeneralSetting::~GeneralSetting()
{
}

wstring GeneralSetting::GetComPort() const
{
    return m_ComPort;
}

void GeneralSetting::SetComPort(const wstring &comPort)
{
    m_ComPort = comPort;
}

bool GeneralSetting::IsCheckBasebandVersionEnabled() const
{
    return m_CheckBasebandVersion;
}

wstring GeneralSetting::GetDownloadAgentFilePath() const
{
    return m_DownloadAgentFilePath;
}

void GeneralSetting::SetDownloadAgentFilePath(
    const wstring &downloadAgentFilePath)
{
    m_DownloadAgentFilePath = downloadAgentFilePath;
}

wstring GeneralSetting::GetAuthenticationFilePath() const
{
    return m_AuthenticationFilePath;
}

void GeneralSetting::Dump() const
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << "[General setting]"    << endl
        << "    COM port = "      << W2CA(m_ComPort.c_str()) << endl
        << "    Check bbchip hw ver = "
        << (m_CheckBasebandVersion ? "Yes" : "No") << endl
        << "    DA filepath = "
        << W2CA(m_DownloadAgentFilePath.c_str()) << endl;

    if (!m_AuthenticationFilePath.empty())
    {
        logger << "    Authentication filepath = "
            << W2CA(m_AuthenticationFilePath.c_str()) << endl;
    }
}

void GeneralSetting::LoadXML(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"com-port")
        {
            m_ComPort = childNode.GetText();
        }
        else if (childNode.GetName() == L"check-bbchip-hardware-version")
        {
            m_CheckBasebandVersion = (childNode.GetText() == L"true");
        }
        else if (childNode.GetName() == L"download-agent-file-path")
        {
            m_DownloadAgentFilePath = childNode.GetText();
        }
        else if (childNode.GetName() == L"authentication-file-path")
        {
            m_AuthenticationFilePath = childNode.GetText();
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void GeneralSetting::SaveXML(Node &node) const
{
    Node rootNode(node.AppendChildNode(L"general-setting"));

    rootNode.AppendChildNode(L"com-port", m_ComPort);

    rootNode.AppendChildNode(
        L"check-bbchip-hardware-version",
        m_CheckBasebandVersion ? L"true" : L"false");

    rootNode.AppendChildNode(L"download-agent-file-path",
        m_DownloadAgentFilePath);

    if (!m_AuthenticationFilePath.empty())
    {
        rootNode.AppendChildNode(L"authentication-file-path",
            m_AuthenticationFilePath);
    }
}

//##################################################################
// FormatOptions
//##################################################################

FormatOptions::FormatOptions()
: m_FormatAction(NOT_FORMAT),
m_StorageType(HW_STORAGE_NOR),
m_ManualFormatBeginAddress(0),
m_ManualFormatLength(0)
{
}

FormatOptions::FormatOptions(FORMAT_ACTION_TYPE_E actionType,
                             HW_StorageType_E storageType,
                             unsigned int manualFormatBeginAddress,
                             unsigned int manualFormatLength)
                             : m_FormatAction(actionType),
                             m_StorageType(storageType),
                             m_ManualFormatBeginAddress(manualFormatBeginAddress),
                             m_ManualFormatLength(manualFormatLength)
{
}

bool FormatOptions::IsAutoFormatEnabled() const
{
    return (m_FormatAction == FAT_FORMAT);
}

bool FormatOptions::IsManualFormatEnabled() const
{
    return (m_FormatAction == NORMAL_MANUAL_FORMAT) ;
}

void FormatOptions::LoadManualFormatOptions(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"storage")
        {
            if (childNode.GetText() == L"NOR")
            {
                m_StorageType = HW_STORAGE_NOR;
            }
            else if (childNode.GetText() == L"NAND")
            {
                m_StorageType = HW_STORAGE_NAND;
            }
            else
            {
                ThrowSettingError(childNode.GetName());
            }
        }
        else if (childNode.GetName() == L"begin-address")
        {
            ::swscanf(childNode.GetText().c_str(),
                L"%X",
                &m_ManualFormatBeginAddress);
        }
        else if (childNode.GetName() == L"length")
        {
            ::swscanf(childNode.GetText().c_str(),
                L"%X",
                &m_ManualFormatLength);
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void FormatOptions::SaveManualFormatOptions(Node &node) const
{
    if (m_StorageType == HW_STORAGE_NOR)
    {
        node.AppendChildNode(L"storage", L"NOR");
    }
    else if (m_StorageType == HW_STORAGE_NAND)
    {
        node.AppendChildNode(L"storage", L"NAND");
    }
    else
    {
        assert(0);
    }

    wostringstream address;
    address << hex << "0x"
        << setfill('0') << setw(8) << m_ManualFormatBeginAddress;
    node.AppendChildNode(L"begin-address", address.str());

    wostringstream length;
    length << hex << "0x" 
        << setfill('0') << setw(8) << m_ManualFormatLength;
    node.AppendChildNode(L"length", length.str());
}

//##################################################################
// DownloadSetting
//##################################################################

DownloadSetting::DownloadSetting(const Node &node)
: m_ScatterFilePath(),
m_DownloadList(),
m_FormatOptions(),
m_WatchdogTimerEnabled(false)
{
    LoadXML(node);
}

DownloadSetting::DownloadSetting(const wstring &scatterFilePath,
                                 const vector<wstring> &downloadList,
                                 const FormatOptions &formatOptions,
                                 bool enableWatchdogTimer)
                                 : m_ScatterFilePath(scatterFilePath),
                                 m_DownloadList(downloadList),
                                 m_FormatOptions(formatOptions),
                                 m_WatchdogTimerEnabled(enableWatchdogTimer)
{
}

DownloadSetting::~DownloadSetting()
{
}

wstring DownloadSetting::GetScatterFilePath() const
{
    return m_ScatterFilePath;
}

void DownloadSetting::SetScatterFilePath(const wstring &scatterFilePath)
{
    m_ScatterFilePath = scatterFilePath;
}

std::vector<wstring> DownloadSetting::GetDownloadList() const
{
    return m_DownloadList;
}

FormatOptions DownloadSetting::GetFormatOptions() const
{
    return m_FormatOptions;
}

bool DownloadSetting::IsWatchdogTimerEnabled() const
{
    return m_WatchdogTimerEnabled;
}

void DownloadSetting::LoadXML(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"scatter-file-path")
        {
            m_ScatterFilePath = childNode.GetText();
        }
        else if (childNode.GetName() == L"download-list")
        {
            LoadDownloadList(childNode);
        }
        else if (childNode.GetName() == L"auto-format-FAT-setting")
        {
            m_FormatOptions.m_FormatAction = FAT_FORMAT;
        }
        else if (childNode.GetName() == L"manual-format-setting")
        {
            m_FormatOptions.m_FormatAction = NORMAL_MANUAL_FORMAT;
            m_FormatOptions.LoadManualFormatOptions(childNode);
        }
        else if (childNode.GetName() == L"enable-watchdog-timer")
        {
            m_WatchdogTimerEnabled = true;
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void DownloadSetting::SaveXML(Node &node) const
{
    Node rootNode(node.AppendChildNode(L"download-setting"));

    rootNode.AppendChildNode(L"scatter-file-path", m_ScatterFilePath);

    Node childNode(rootNode.AppendChildNode(L"download-list"));

    for (vector<wstring>::const_iterator it = m_DownloadList.begin();
        it != m_DownloadList.end();
        ++it)
    {
        childNode.AppendChildNode(L"download-item", *it);
    }

    if (m_FormatOptions.IsAutoFormatEnabled())
    {
        rootNode.AppendChildNode(L"auto-format-FAT-setting");
    }
    else if (m_FormatOptions.IsManualFormatEnabled())
    {
        Node childNode(rootNode.AppendChildNode(L"manual-format-setting"));
        m_FormatOptions.SaveManualFormatOptions(childNode);
    }

    if (IsWatchdogTimerEnabled())
    {
        rootNode.AppendChildNode(L"enable-watchdog-timer");
    }
}

SharedPtr<Command> DownloadSetting::CreateCommand(
    const GeneralSetting &generalSetting) const
{
    return SharedPtr<Command>(new DownloadCommand(generalSetting, *this));
}

DownloadSetting* DownloadSetting::Clone() const
{
    return new DownloadSetting(*this);
}

void DownloadSetting::Dump() const
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << "[Download setting]"   << endl
        << "    Scatter filepath = "
        << W2CA(m_ScatterFilePath.c_str()) << endl;

    if (m_DownloadList.size() > 0)
    {
        logger << "    Download list = ";

        for (vector<wstring>::const_iterator it = m_DownloadList.begin();
            it != m_DownloadList.end();
            ++it)
        {
            logger << W2CA(it->c_str()) << " ";
        }

        logger << endl;
    }

    if (m_FormatOptions.IsAutoFormatEnabled())
    {
        logger << "    Auto format enabled = Yes" << endl;
    }
    else if (m_FormatOptions.IsManualFormatEnabled())
    {
        logger << "    Storage = ";

        if (m_FormatOptions.m_StorageType == HW_STORAGE_NOR)
        {
            logger << "NOR" << endl;
        }
        else if (m_FormatOptions.m_StorageType == HW_STORAGE_NAND)
        {
            logger << "NAND" << endl;
        }
        else
        {
            assert(0);
        }

        logger << hex;
        logger << "    Manual format begin address = 0x"
            << setfill('0') << setw(8)
            << m_FormatOptions.m_ManualFormatBeginAddress << endl;
        logger << "    Manual format length = 0x"
            << setfill('0') << setw(8)
            << m_FormatOptions.m_ManualFormatLength << endl;
        logger << dec;
    }

    if (IsWatchdogTimerEnabled())
    {
        logger << "    Watchdog timer enabled = Yes" << endl;
    }
}


void DownloadSetting::LoadDownloadList(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"download-item")
        {
            m_DownloadList.push_back(childNode.GetText());
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

//##################################################################
// FormatSetting
//##################################################################

FormatSetting::FormatSetting(const Node &node)
: m_FormatOptions(),
m_WatchdogTimerEnabled(false)
{
    LoadXML(node);
}

FormatSetting::FormatSetting(const FormatOptions &formatOptions,
                             bool enableWatchdogTimer)
                             : m_FormatOptions(formatOptions),
                             m_WatchdogTimerEnabled(enableWatchdogTimer)
{
}

FormatSetting::~FormatSetting()
{
}

FormatOptions FormatSetting::GetFormatOptions() const
{
    return m_FormatOptions;
}

bool FormatSetting::IsWatchdogTimerEnabled() const
{
    return m_WatchdogTimerEnabled;
}

void FormatSetting::LoadXML(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"auto-format-FAT-setting")
        {
            m_FormatOptions.m_FormatAction = FAT_FORMAT;
        }
        else if (childNode.GetName() == L"manual-format-setting")
        {
            m_FormatOptions.m_FormatAction = NORMAL_MANUAL_FORMAT;
            m_FormatOptions.LoadManualFormatOptions(childNode);
        }
        else if (childNode.GetName() == L"enable-watchdog-timer")
        {
            m_WatchdogTimerEnabled = true;
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void FormatSetting::SaveXML(Node &node) const
{
    Node rootNode(node.AppendChildNode(L"format-setting"));

    if (m_FormatOptions.IsAutoFormatEnabled())
    {
        rootNode.AppendChildNode(L"auto-format-FAT-setting");
    }
    else if (m_FormatOptions.IsManualFormatEnabled())
    {
        Node childNode(rootNode.AppendChildNode(L"manual-format-setting"));
        m_FormatOptions.SaveManualFormatOptions(childNode);
    }
    else
    {
        assert(0);
    }

    if (IsWatchdogTimerEnabled())
    {
        rootNode.AppendChildNode(L"enable-watchdog-timer");
    }
}

SharedPtr<Command> FormatSetting::CreateCommand(
    const GeneralSetting &generalSetting) const
{
    return SharedPtr<Command>(new FormatCommand(generalSetting, *this));
}

FormatSetting* FormatSetting::Clone() const
{
    return new FormatSetting(*this);
}

void FormatSetting::Dump() const
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << "[Format setting]" << endl;

    if (m_FormatOptions.IsAutoFormatEnabled())
    {
        logger << "    Auto format enabled = Yes" << endl;
    }
    else if (m_FormatOptions.IsManualFormatEnabled())
    {
        logger << "    Storage = ";

        if (m_FormatOptions.m_StorageType == HW_STORAGE_NOR)
        {
            logger << "NOR" << endl;
        }
        else if (m_FormatOptions.m_StorageType == HW_STORAGE_NAND)
        {
            logger << "NAND" << endl;
        }
        else
        {
            assert(0);
        }

        logger << hex;
        logger << "    Manual format begin address = 0x"
            << setfill('0') << setw(8)
            << m_FormatOptions.m_ManualFormatBeginAddress << endl;
        logger << "    Manual format length = 0x"
            << setfill('0') << setw(8)
            << m_FormatOptions.m_ManualFormatLength << endl;
        logger << dec;
    }

    if (IsWatchdogTimerEnabled())
    {
        logger << "    Watchdog timer enabled = Yes" << endl;
    }
}

//##################################################################
// ResetToFactoryDefaultSetting
//##################################################################

ResetToFactoryDefaultSetting::ResetToFactoryDefaultSetting(const Node &node)
: m_WatchdogTimerEnabled(false)
{
    LoadXML(node);
}

ResetToFactoryDefaultSetting::ResetToFactoryDefaultSetting(
    bool enableWatchdogTimer)
    : m_WatchdogTimerEnabled(enableWatchdogTimer)
{
}

ResetToFactoryDefaultSetting::~ResetToFactoryDefaultSetting()
{
}

bool ResetToFactoryDefaultSetting::IsWatchdogTimerEnabled() const
{
    return m_WatchdogTimerEnabled;
}

void ResetToFactoryDefaultSetting::LoadXML(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"enable-watchdog-timer")
        {
            m_WatchdogTimerEnabled = true;
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void ResetToFactoryDefaultSetting::SaveXML(Node &node) const
{
    Node rootNode(node.AppendChildNode(L"reset-to-factory-default-setting"));

    if (IsWatchdogTimerEnabled())
    {
        rootNode.AppendChildNode(L"enable-watchdog-timer");
    }
}

SharedPtr<Command> ResetToFactoryDefaultSetting::CreateCommand(
    const GeneralSetting &generalSetting) const
{
    return SharedPtr<Command>(
        new ResetToFactoryDefaultCommand(generalSetting, *this));
}

ResetToFactoryDefaultSetting* ResetToFactoryDefaultSetting::Clone() const
{
    return new ResetToFactoryDefaultSetting(*this);
}

void ResetToFactoryDefaultSetting::Dump() const
{
    logger << Logger::Level_Normal
        << "[ResetToFactoryDefault setting]"   << endl;

    if (IsWatchdogTimerEnabled())
    {
        logger << "    Watchdog timer enabled = Yes" << endl;
    }
}

//##################################################################
// BackupSetting
//##################################################################

BackupSetting::BackupSetting(const Node &node)
: m_IniFilePath(),
m_BackupDirectoryPath(),
m_WatchdogTimerEnabled(false)
{
    LoadXML(node);
}

BackupSetting::BackupSetting(const wstring &iniFilePath,
                             const wstring &backupDirectoryPath,
                             bool enableWatchdogTimer)
                             : m_IniFilePath(iniFilePath),
                             m_BackupDirectoryPath(backupDirectoryPath),
                             m_WatchdogTimerEnabled(enableWatchdogTimer)
{
}

BackupSetting::~BackupSetting()
{
}

wstring BackupSetting::GetIniFilePath() const
{
    return m_IniFilePath;
}

wstring BackupSetting::GetBackupDirectoryPath() const
{
    return m_BackupDirectoryPath;
}

bool BackupSetting::IsWatchdogTimerEnabled() const
{
    return m_WatchdogTimerEnabled;
}

void BackupSetting::LoadXML(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"ini-file-path")
        {
            m_IniFilePath = childNode.GetText();
        }
        else if (childNode.GetName() == L"backup-directory-path")
        {
            m_BackupDirectoryPath = childNode.GetText();
        }
        else if (childNode.GetName() == L"enable-watchdog-timer")
        {
            m_WatchdogTimerEnabled = true;
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void BackupSetting::SaveXML(Node &node) const
{
    Node rootNode(node.AppendChildNode(L"backup-setting"));

    rootNode.AppendChildNode(L"ini-file-path", m_IniFilePath);
    rootNode.AppendChildNode(L"backup-directory-path", m_BackupDirectoryPath);

    if (IsWatchdogTimerEnabled())
    {
        rootNode.AppendChildNode(L"enable-watchdog-timer");
    }
}

SharedPtr<Command> BackupSetting::CreateCommand(
    const GeneralSetting &generalSetting) const
{
    return SharedPtr<Command>(new BackupCommand(generalSetting, *this));
}

BackupSetting* BackupSetting::Clone() const
{
    return new BackupSetting(*this);
}

void BackupSetting::Dump() const
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << "[Backup setting]"   << endl;

    logger << "    Ini file path = "
        << W2CA(m_IniFilePath.c_str()) << endl
        << "    Backup directory path = "
        << W2CA(m_BackupDirectoryPath.c_str()) << endl;

    if (IsWatchdogTimerEnabled())
    {
        logger << "    Watchdog timer enabled = Yes" << endl;
    }
}

//##################################################################
// RestoreSetting
//##################################################################

RestoreSetting::RestoreSetting(const Node &node)
: m_IniFilePath(),
m_RestoreDirectoryPath(),
m_WatchdogTimerEnabled(false)
{
    LoadXML(node);
}

RestoreSetting::RestoreSetting(const wstring &iniFilePath,
                               const wstring &restoreDirectoryPath,
                               bool enableWatchdogTimer)
                               : m_IniFilePath(iniFilePath),
                               m_RestoreDirectoryPath(restoreDirectoryPath),
                               m_WatchdogTimerEnabled(enableWatchdogTimer)
{
}

RestoreSetting::~RestoreSetting()
{
}

wstring RestoreSetting::GetIniFilePath() const
{
    return m_IniFilePath;
}

wstring RestoreSetting::GetRestoreDirectoryPath() const
{
    return m_RestoreDirectoryPath;
}

bool RestoreSetting::IsWatchdogTimerEnabled() const
{
    return m_WatchdogTimerEnabled;
}

void RestoreSetting::LoadXML(const Node &node)
{
    for (unsigned int childIndex=0;
        childIndex<node.GetNumChildNodes();
        ++childIndex)
    {
        const Node childNode = node.GetChildNode(childIndex);

        if (childNode.GetName() == L"ini-file-path")
        {
            m_IniFilePath = childNode.GetText();
        }
        else if (childNode.GetName() == L"restore-directory-path")
        {
            m_RestoreDirectoryPath = childNode.GetText();
        }
        else if (childNode.GetName() == L"enable-watchdog-timer")
        {
            m_WatchdogTimerEnabled = true;
        }
        else
        {
            ThrowSettingError(childNode.GetName());
        }
    }
}

void RestoreSetting::SaveXML(Node &node) const
{
    Node rootNode(node.AppendChildNode(L"restore-setting"));

    rootNode.AppendChildNode(L"ini-file-path", m_IniFilePath);
    rootNode.AppendChildNode(L"restore-directory-path", m_RestoreDirectoryPath);

    if (IsWatchdogTimerEnabled())
    {
        rootNode.AppendChildNode(L"enable-watchdog-timer");
    }
}

SharedPtr<Command> RestoreSetting::CreateCommand(
    const GeneralSetting &generalSetting) const
{
    return SharedPtr<Command>(new RestoreCommand(generalSetting, *this));
}

RestoreSetting* RestoreSetting::Clone() const
{
    return new RestoreSetting(*this);
}

void RestoreSetting::Dump() const
{
    USES_CONVERSION;

    logger << Logger::Level_Normal
        << "[Restore setting]" << endl;

    logger << "    Ini file path = "
        << W2CA(m_IniFilePath.c_str()) << endl
        << "    Restore directory path = "
        << W2CA(m_RestoreDirectoryPath.c_str()) << endl;

    if (IsWatchdogTimerEnabled())
    {
        logger << "    Watchdog timer enabled = Yes" << endl;
    }
}
