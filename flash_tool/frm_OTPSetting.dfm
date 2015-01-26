object m_OTPSetting: Tm_OTPSetting
  Left = 403
  Top = 372
  Width = 500
  Height = 250
  Caption = 'OTP Settings'
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
  object m_GroupOTPPara: TGroupBox
    Left = 8
    Top = 96
    Width = 473
    Height = 113
    Caption = ' OTP Parameters '
    Color = clBtnFace
    Ctl3D = True
    ParentColor = False
    ParentCtl3D = False
    TabOrder = 0
    object Label1: TLabel
      Left = 256
      Top = 32
      Width = 72
      Height = 13
      Caption = 'Write Address :'
    end
    object Label2: TLabel
      Left = 16
      Top = 32
      Width = 67
      Height = 13
      Caption = 'Write Length :'
    end
    object m_otp_addr: TEdit
      Left = 336
      Top = 24
      Width = 121
      Height = 21
      TabOrder = 0
    end
    object m_otp_len: TEdit
      Left = 104
      Top = 24
      Width = 121
      Height = 21
      TabOrder = 1
    end
    object BitBtn1: TBitBtn
      Left = 8
      Top = 72
      Width = 81
      Height = 25
      Caption = 'OTP file path'
      TabOrder = 2
      OnClick = BitBtn1Click
    end
    object m_otp_file: TEdit
      Left = 104
      Top = 72
      Width = 353
      Height = 21
      Enabled = False
      TabOrder = 3
    end
  end
  object m_OK: TButton
    Left = 304
    Top = 16
    Width = 65
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = m_OKClick
  end
  object m_Cancel: TButton
    Left = 304
    Top = 56
    Width = 65
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = m_CancelClick
  end
  object m_GroupOTPOperation: TRadioGroup
    Left = 8
    Top = 0
    Width = 177
    Height = 89
    Caption = ' OTP Operations '
    TabOrder = 3
  end
  object m_Lock: TRadioButton
    Left = 16
    Top = 64
    Width = 113
    Height = 17
    Caption = 'Lock Only'
    TabOrder = 4
    OnClick = m_LockClick
  end
  object m_Write: TRadioButton
    Left = 16
    Top = 16
    Width = 113
    Height = 17
    Caption = 'Write Only'
    Checked = True
    TabOrder = 5
    TabStop = True
    OnClick = m_WriteClick
  end
  object m_WriteLock: TRadioButton
    Left = 16
    Top = 40
    Width = 113
    Height = 17
    Caption = 'Write and Lock'
    TabOrder = 6
    OnClick = m_WriteLockClick
  end
  object m_SelectOTPFile: TOpenDialog
    DefaultExt = '*.*'
    Filter = 'All File (*.*)|*.*'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 40
    Top = 192
  end
end
