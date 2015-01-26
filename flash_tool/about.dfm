object Form_About: TForm_About
  Left = 496
  Top = 187
  BorderStyle = bsDialog
  Caption = 'About FlashTool'
  ClientHeight = 356
  ClientWidth = 337
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 321
    Height = 305
    TabOrder = 0
    object VersionInfo: TLabel
      Left = 24
      Top = 24
      Width = 107
      Height = 24
      Caption = 'Version 3.1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -21
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lb_BuildDate: TLabel
      Left = 24
      Top = 64
      Width = 80
      Height = 13
      Caption = 'Build 2002.12.31'
    end
    object Label3: TLabel
      Left = 24
      Top = 280
      Width = 275
      Height = 13
      Caption = 'Copyright (C) 2004-2010 MediaTek Inc. All rights reserved.'
    end
    object brom_dll_version: TLabel
      Left = 24
      Top = 96
      Width = 67
      Height = 13
      Caption = 'brom.dll v2.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object meta_dll_version: TLabel
      Left = 24
      Top = 160
      Width = 104
      Height = 13
      Caption = 'META_DLL.dll v2.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object brom_dll_release_type: TLabel
      Left = 24
      Top = 112
      Width = 83
      Height = 13
      Caption = 'RELEASE_TYPE'
    end
    object meta_dll_release_type: TLabel
      Left = 24
      Top = 176
      Width = 83
      Height = 13
      Caption = 'RELEASE_TYPE'
    end
    object brom_dll_build_date: TLabel
      Left = 24
      Top = 128
      Width = 67
      Height = 13
      Caption = 'BUILD_DATE'
    end
    object meta_dll_build_date: TLabel
      Left = 24
      Top = 192
      Width = 67
      Height = 13
      Caption = 'BUILD_DATE'
    end
    object da_version: TLabel
      Left = 24
      Top = 224
      Width = 40
      Height = 13
      Caption = 'DA v2.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object da_build_date: TLabel
      Left = 24
      Top = 240
      Width = 67
      Height = 13
      Caption = 'BUILD_DATE'
    end
  end
  object Button1: TButton
    Left = 136
    Top = 320
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = Button1Click
  end
end
