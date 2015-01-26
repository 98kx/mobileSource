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
#include <stdexcept>

#include "Config.h"
#include "CommandLineArguments.h"
#include "FlashToolCommand.h"
#include "Setting.h"
#include "XMLDocument.h"


using std::runtime_error;
using std::string;
using std::vector;
using std::wstring;
using ConsoleMode::CommandLineArguments;
using ConsoleMode::Config;
using ConsoleMode::CommandSetting;
using ConsoleMode::GeneralSetting;
using XML::Document;
using XML::Node;

// add some commit for git test . shangjing 
static const wstring DOCUMENT_ROOT_NODE_NAME(L"flashtool-config");
static const wstring DOCUMENT_VERSION(L"3.0");


Config::Config()
    : m_GeneralSetting(NULL),
      m_CommandSettings()
{
}

Config::Config(const CommandLineArguments &commandLineArguments)
    : m_GeneralSetting(NULL),
      m_CommandSettings()
{
    LoadFile(commandLineArguments.GetConfigFilename());
    assert(m_GeneralSetting != NULL);

    if (!commandLineArguments.GetComPort().empty())
    {
        // Overwrite the COM port setting defined in the config file
        m_GeneralSetting->SetComPort(commandLineArguments.GetComPort());
    }

    if (!commandLineArguments.GetDownloadAgentFilename().empty())
    {
        // Overwrite the download agent setting defined in the config file
        m_GeneralSetting->SetDownloadAgentFilePath(
                commandLineArguments.GetDownloadAgentFilename());
    }

    if (!commandLineArguments.GetScatterFilename().empty())
    {
        // Overwrite the scatter file setting defined in the config file
        for (CommandSettings::iterator it = m_CommandSettings.begin();
             it != m_CommandSettings.end();
             ++it)
        {
            DownloadSetting *downloadSetting =
                    dynamic_cast<DownloadSetting *>(it->Get());

            if (downloadSetting != NULL)
            {
                downloadSetting->SetScatterFilePath(
                        commandLineArguments.GetScatterFilename());
            }
        }
    }
}

Config::~Config()
{
    if (m_GeneralSetting != NULL)
    {
        delete m_GeneralSetting;
    }
}

void Config::LoadFile(const wstring &filename)
{
    Document document(filename);
    const Node rootNode = document.GetRootNode();
    assert(rootNode.GetName() == DOCUMENT_ROOT_NODE_NAME);

    const wstring documentVersion(rootNode.GetAttribute(L"version"));

    if (documentVersion == L"1.0")
    {
        document.Validate(L"ConsoleMode_v1_0.xsd");
    }
    else if (documentVersion == L"2.0")
    {
        document.Validate(L"ConsoleMode_v2_0.xsd");
    }
    else if (documentVersion == L"3.0")
    {
        document.Validate(L"ConsoleMode_v3_0.xsd");
    }
    else if (documentVersion.empty())
    {
        throw runtime_error(string("No configuration version specified"));
    }
    else
    {
        USES_CONVERSION;
        throw runtime_error(string("Unrecognized configuration version: ") +
                            W2CA(documentVersion.c_str()));
    }

    const Node operationNode =
            (documentVersion == L"1.0") ? rootNode.GetChildNode(0) : rootNode;
    const unsigned int numSettingNodes = operationNode.GetNumChildNodes();

    // We should have at least two setting nodes, including one
    // "general-setting" node.
    assert(numSettingNodes >= 2);

    //
    // Get general setting 
    //
    const Node generalSettingNode = operationNode.GetChildNode(0);
    assert(generalSettingNode.GetName() == L"general-setting");

    if (m_GeneralSetting != NULL)
    {
        delete m_GeneralSetting;
    }

    m_GeneralSetting = new GeneralSetting(generalSettingNode);

    //
    // Get command settings
    //
    m_CommandSettings.clear();

    for (unsigned int childIndex=1; childIndex<numSettingNodes; ++childIndex)
    {
        const Node settingNode = operationNode.GetChildNode(childIndex);
        CommandSetting *commandSetting;

        if (settingNode.GetName() == L"download-setting")
        {
            m_CommandSettings.push_back(
                SharedPtr<CommandSetting>(new DownloadSetting(settingNode)));
        }
        else if (settingNode.GetName() == L"format-setting")
        {
            m_CommandSettings.push_back(
                    SharedPtr<CommandSetting>(new FormatSetting(settingNode)));
        }
        else if (settingNode.GetName() == L"reset-to-factory-default-setting")
        {
            m_CommandSettings.push_back(
                    SharedPtr<CommandSetting>(
                            new ResetToFactoryDefaultSetting(settingNode)));
        }
        else if (settingNode.GetName() == L"backup-setting")
        {
            m_CommandSettings.push_back(
                    SharedPtr<CommandSetting>(new BackupSetting(settingNode)));
        }
        else if (settingNode.GetName() == L"restore-setting")
        {
            m_CommandSettings.push_back(
                SharedPtr<CommandSetting>(new RestoreSetting(settingNode)));
        }
        else
        {
            USES_CONVERSION;
            throw runtime_error(string("Unrecognized setting: ") +
                                W2CA(settingNode.GetName().c_str()));
        }
    }
}

void Config::SaveFile(const wstring &filename) const
{
    assert(m_GeneralSetting != NULL);
    assert(!m_CommandSettings.empty());

    Document document(L"1.0", L"UTF-8", DOCUMENT_ROOT_NODE_NAME);
    Node rootNode = document.GetRootNode();

    rootNode.SetAttribute(L"version", DOCUMENT_VERSION);
    m_GeneralSetting->SaveXML(rootNode);

    for (CommandSettings::const_iterator it = m_CommandSettings.begin();
         it != m_CommandSettings.end();
         ++it)
    {
        (*it)->SaveXML(rootNode);
    }

    document.Beautify();
    document.Save(filename);
}

void Config::SetGeneralSetting(const GeneralSetting &generalSetting)
{
    if (m_GeneralSetting != NULL)
    {
        delete m_GeneralSetting;
    }

    m_GeneralSetting = new GeneralSetting(generalSetting);
}

void Config::AddCommandSetting(const CommandSetting &commandSetting)
{
    m_CommandSettings.push_back(
            SharedPtr<CommandSetting>(commandSetting.Clone()));
}

SharedPtr<Command> Config::CreateCommand() const
{
    assert(m_GeneralSetting != NULL);
    assert(!m_CommandSettings.empty());

    SharedPtr<MacroCommand> macroCommand(new MacroCommand);

    for (CommandSettings::const_iterator it = m_CommandSettings.begin();
         it != m_CommandSettings.end();
         ++it)
    {
        macroCommand->Add((*it)->CreateCommand(*m_GeneralSetting));
    }

    return macroCommand;
}

void Config::Dump() const
{
    if (m_GeneralSetting != NULL)
    {
        m_GeneralSetting->Dump();
    }

    for (CommandSettings::const_iterator it = m_CommandSettings.begin();
         it != m_CommandSettings.end();
         ++it)
    {
        (*it)->Dump();
    }
}
