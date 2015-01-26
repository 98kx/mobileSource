object m_ParameterReadSetting: Tm_ParameterReadSetting
  Left = 356
  Top = 248
  Width = 396
  Height = 188
  Caption = 'Parameter Settings for Read Operation'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 192
    Top = 16
    Width = 73
    Height = 13
    Caption = 'Read Address :'
  end
  object Label2: TLabel
    Left = 32
    Top = 16
    Width = 68
    Height = 13
    Caption = 'Read Length :'
  end
  object Splitter1: TSplitter
    Left = 16
    Top = 97
    Width = 361
    Height = 7
    Cursor = crHSplit
    Align = alNone
    Beveled = True
  end
  object m_addr: TEdit
    Left = 304
    Top = 16
    Width = 73
    Height = 21
    TabOrder = 0
  end
  object m_len: TEdit
    Left = 104
    Top = 16
    Width = 73
    Height = 21
    TabOrder = 1
  end
  object BitBtn1: TBitBtn
    Left = 16
    Top = 56
    Width = 81
    Height = 25
    Caption = 'Save As...'
    TabOrder = 2
    OnClick = BitBtn1Click
  end
  object m_file: TEdit
    Left = 104
    Top = 56
    Width = 273
    Height = 21
    Enabled = False
    TabOrder = 3
  end
  object m_OK: TButton
    Left = 128
    Top = 112
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 4
    OnClick = m_OKClick
  end
  object m_Cancel: TButton
    Left = 232
    Top = 112
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 5
    OnClick = m_CancelClick
  end
  object m_OpenFileDialog: TOpenDialog
    DefaultExt = '*.*'
    Filter = 'All File (*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 32
    Top = 112
  end
  object m_SaveFileDialog: TSaveDialog
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 64
    Top = 112
  end
end
