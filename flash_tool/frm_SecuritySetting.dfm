object m_SecuritySetting: Tm_SecuritySetting
  Left = 208
  Top = 229
  Width = 361
  Height = 300
  Caption = 'Security Settings'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PopupMenu = m_SecuritySettingRightClick
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object m_GroupSecuritySetting: TGroupBox
    Left = 8
    Top = 8
    Width = 337
    Height = 257
    Caption = ' Security Settings '
    TabOrder = 0
    object m_LockDown: TCheckBox
      Left = 16
      Top = 16
      Width = 153
      Height = 25
      Caption = 'Lockdown Security Setting'
      TabOrder = 0
    end
    object m_FAT_RB_Disabled: TCheckBox
      Left = 16
      Top = 150
      Width = 177
      Height = 25
      Caption = 'Forbid to readback FAT area'
      TabOrder = 1
    end
    object m_CODE_DL_Disabled: TCheckBox
      Left = 16
      Top = 35
      Width = 177
      Height = 25
      Caption = 'Forbid to download CODE area'
      TabOrder = 2
    end
    object m_CODE_RB_Disabled: TCheckBox
      Left = 16
      Top = 54
      Width = 177
      Height = 25
      Caption = 'Forbid to readback CODE area'
      TabOrder = 3
    end
    object m_CODE_FMT_Disabled: TCheckBox
      Left = 16
      Top = 73
      Width = 177
      Height = 25
      Caption = 'Forbid to format CODE area'
      TabOrder = 4
    end
    object m_SECRO_DL_Disabled: TCheckBox
      Left = 16
      Top = 92
      Width = 201
      Height = 25
      Caption = 'Forbid to download SEC_RO area'
      TabOrder = 5
    end
    object m_SECRO_RB_Disabled: TCheckBox
      Left = 16
      Top = 112
      Width = 193
      Height = 25
      Caption = 'Forbid to readback SEC_RO area'
      TabOrder = 6
    end
    object m_FAT_DL_Disabled: TCheckBox
      Left = 16
      Top = 131
      Width = 185
      Height = 25
      Caption = 'Forbid to download FAT area'
      TabOrder = 7
    end
    object m_FAT_FMT_Disabled: TCheckBox
      Left = 16
      Top = 169
      Width = 177
      Height = 25
      Caption = 'Forbid to format FAT area'
      TabOrder = 8
    end
    object m_RestrictedAreaForbidden: TCheckBox
      Left = 16
      Top = 188
      Width = 193
      Height = 25
      Caption = 'Restricted area access forbidden'
      TabOrder = 9
    end
  end
  object m_OK: TButton
    Left = 264
    Top = 24
    Width = 65
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = m_OKClick
  end
  object m_Cancel: TButton
    Left = 264
    Top = 56
    Width = 65
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = m_CancelClick
  end
  object m_SecuritySettingRightClick: TPopupMenu
    Left = 240
    Top = 112
    object m_CheckAll: TMenuItem
      Caption = 'Check all'
      OnClick = m_CheckAllClick
    end
    object m_UncheckAll: TMenuItem
      Caption = 'Un-check all'
      OnClick = m_UncheckAllClick
    end
  end
end
