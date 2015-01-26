/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _FLASHTOOL_COMMAND_H
#define _FLASHTOOL_COMMAND_H

#include <string>
#include "Command.h"
#include "NonCopyable.h"
#include "flashtool.h"
#include "meta.h"
#include "Setting.h"


/*
* Take care of download handle initialization and deinitialization
*/
class DownloadHandle: private NonCopyable
{
public:
	DownloadHandle(const ConsoleMode::DownloadSetting &downloadSetting);
	~DownloadHandle();

	operator DL_HANDLE_T() const
	{
		return m_Handle;
	}

private:
	DL_HANDLE_T m_Handle;
};

//##################################################################
// DownloadCommand
//##################################################################

class DownloadCommand : public Command
{
public:
	DownloadCommand(const ConsoleMode::GeneralSetting  &generalSetting,
		const ConsoleMode::DownloadSetting &downloadSetting);
	virtual ~DownloadCommand();

public:
	virtual bool Execute();

private:
	const ConsoleMode::GeneralSetting  m_GeneralSetting;
	const ConsoleMode::DownloadSetting m_DownloadSetting;
	DownloadHandle m_DownloadHandle;
};

//##################################################################
// FormatCommand
//##################################################################

class FormatCommand : public Command
{
public:
	FormatCommand(const ConsoleMode::GeneralSetting &generalSetting,
		const ConsoleMode::FormatSetting  &formatSetting);
	virtual ~FormatCommand();

public:
	virtual bool Execute();

private:
	const ConsoleMode::GeneralSetting m_GeneralSetting;
	const ConsoleMode::FormatSetting  m_FormatSetting;
};

//##################################################################
// ResetToFactoryDefaultCommand
//##################################################################

class ResetToFactoryDefaultCommand : public Command
{
public:
	ResetToFactoryDefaultCommand(
		const ConsoleMode::GeneralSetting &generalSetting,
		const ConsoleMode::ResetToFactoryDefaultSetting
		&resetToFactoryDefaultSetting);
	virtual ~ResetToFactoryDefaultCommand();

public:
	virtual bool Execute();

private:
	const ConsoleMode::GeneralSetting m_GeneralSetting;
	const ConsoleMode::ResetToFactoryDefaultSetting
		m_ResetToFactoryDefaultSetting;
};

//##################################################################
// BackupCommand
//##################################################################

class BackupCommand : public Command
{
public:
	BackupCommand(const ConsoleMode::GeneralSetting &generalSetting,
		const ConsoleMode::BackupSetting &backupSetting);
	virtual ~BackupCommand();

public:
	virtual bool Execute();

private:
	bool CreateDirectory(const std::wstring &directoryPath);

private:
	const ConsoleMode::GeneralSetting m_GeneralSetting;
	const ConsoleMode::BackupSetting  m_BackupSetting;
};

//##################################################################
// RestoreCommand
//##################################################################

class RestoreCommand: public Command
{
public:
	RestoreCommand(const ConsoleMode::GeneralSetting &generalSetting,
		const ConsoleMode::RestoreSetting &restoreSetting);
	virtual ~RestoreCommand();

public:
	virtual bool Execute();

private:
	const ConsoleMode::GeneralSetting m_GeneralSetting;
	const ConsoleMode::RestoreSetting m_RestoreSetting;
};

#endif // _FLASHTOOL_COMMAND_H
