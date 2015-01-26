/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2002
*
*****************************************************************************/

/*******************************************************************************
* Filename:
* ---------
*  Flash_tool.cpp
*
* Project:
* --------
*  Flash tool set
*
* Description:
* ------------
*  Flash tool set main project file
*
* Author:
* -------
*  FZ Hsu (mtk00303)
*
*******************************************************************************/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ConsoleMode.h"

USEFORM("main.cpp", MainForm);
USEFORM("DA_Option.cpp", Form_DA_Option);
USEFORM("ext_mem1.cpp", ext_mem1Form);
USEFORM("OK_Wnd.cpp", Form_OK);
USEFORM("about.cpp", Form_About);
USEFORM("RB_Option.cpp", Form_RB_Option);
USEFORM("DL_Go.cpp", Form_DL_GO);
USEFORM("RB_Go.cpp", Form_RB_GO);
USEFORM("frm_FormatOption.cpp", FormatOption);
USEFORM("frm_BootROMStartCmdRetry.cpp", frm_BromStartCmdRetry);
USEFORM("frm_COM_Option.cpp", COM_Option);
USEFORM("frm_SecuritySetting.cpp", m_SecuritySetting);
USEFORM("frm_OTPSetting.cpp", m_OTPSetting);
USEFORM("frm_ParameterReadSetting.cpp", m_ParameterReadSetting);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR commandLine, int)
{
    try
    {
        if ((commandLine != NULL) && (::strlen(commandLine) != 0))
        {
            return ConsoleMode::Run(GetCommandLine());
        }

        Application->Initialize();
        Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TForm_DA_Option), &Form_DA_Option);
                 Application->CreateForm(__classid(Text_mem1Form), &ext_mem1Form);
                 Application->CreateForm(__classid(TForm_OK), &Form_OK);
                 Application->CreateForm(__classid(TForm_About), &Form_About);
                 Application->CreateForm(__classid(TForm_RB_Option), &Form_RB_Option);
                 Application->CreateForm(__classid(TForm_DL_GO), &Form_DL_GO);
                 Application->CreateForm(__classid(TForm_RB_GO), &Form_RB_GO);
                 Application->CreateForm(__classid(TFormatOption), &FormatOption);
                 Application->CreateForm(__classid(Tfrm_BromStartCmdRetry), &frm_BromStartCmdRetry);
                 Application->CreateForm(__classid(TCOM_Option), &COM_Option);
                 Application->CreateForm(__classid(Tm_SecuritySetting), &m_SecuritySetting);
                 Application->CreateForm(__classid(Tm_OTPSetting), &m_OTPSetting);
                 Application->CreateForm(__classid(Tm_ParameterReadSetting), &m_ParameterReadSetting);
                 Application->HintPause     = 100; // 0.1 sec
        Application->HintHidePause = 100000; // 100 sec
        Application->Run();
    }
    catch ( Exception &exception )
    {
        Application->ShowException( &exception );
    }

    return 0;
}
//---------------------------------------------------------------------------
