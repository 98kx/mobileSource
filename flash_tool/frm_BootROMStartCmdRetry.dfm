object frm_BromStartCmdRetry: Tfrm_BromStartCmdRetry
  Left = 365
  Top = 311
  BorderStyle = bsDialog
  Caption = 'BootROM Start Command Retry Count'
  ClientHeight = 72
  ClientWidth = 295
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object m_OK_Button: TButton
    Left = 240
    Top = 8
    Width = 49
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = m_OK_ButtonClick
  end
  object m_Cancel_Button: TButton
    Left = 240
    Top = 40
    Width = 49
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = m_Cancel_ButtonClick
  end
  object m_RetryCount: TLabeledEdit
    Left = 176
    Top = 24
    Width = 41
    Height = 21
    EditLabel.Width = 161
    EditLabel.Height = 13
    EditLabel.Caption = 'BootROM Start Command Retry Count: '
    LabelPosition = lpLeft
    LabelSpacing = 3
    MaxLength = 3
    TabOrder = 2
  end
end
