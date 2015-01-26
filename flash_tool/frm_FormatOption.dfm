object FormatOption: TFormatOption
  Left = 1315
  Top = 29
  BorderStyle = bsDialog
  Caption = ' Format Options'
  ClientHeight = 661
  ClientWidth = 818
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = OnShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 801
    Height = 649
    TabOrder = 0
    object m_Panel_AutoFormat1: TPanel
      Left = 16
      Top = 166
      Width = 177
      Height = 25
      BevelOuter = bvLowered
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 4
      object m_Radio_AutoFormat: TRadioButton
        Left = 8
        Top = 4
        Width = 129
        Height = 17
        Caption = 'Format FAT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = m_Radio_AutoFormatClick
      end
    end
    object m_Panel_ManualFormat1: TPanel
      Left = 16
      Top = 360
      Width = 177
      Height = 25
      BevelOuter = bvLowered
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 5
      object m_Radio_ManualFormat: TRadioButton
        Left = 8
        Top = 4
        Width = 129
        Height = 17
        Caption = 'Manual Format FAT'
        TabOrder = 0
        OnClick = m_Radio_ManualFormatClick
      end
    end
    object m_Panel_ManualFormat2: TPanel
      Left = 16
      Top = 384
      Width = 649
      Height = 249
      BevelOuter = bvLowered
      TabOrder = 6
      object Label4: TLabel
        Left = 43
        Top = 17
        Width = 324
        Height = 16
        Caption = 'Specify the begin address and length of the format area.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label8: TLabel
        Left = 43
        Top = 104
        Width = 214
        Height = 16
        Caption = 'WARNING! Calibration data is erased.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object FormatLength: TLabeledEdit
        Left = 176
        Top = 76
        Width = 137
        Height = 21
        EditLabel.Width = 102
        EditLabel.Height = 13
        EditLabel.Caption = 'Format Length (Hex) :'
        LabelPosition = lpLeft
        LabelSpacing = 3
        TabOrder = 1
      end
      object FormatStartAddr: TLabeledEdit
        Left = 176
        Top = 44
        Width = 137
        Height = 21
        EditLabel.Width = 102
        EditLabel.Height = 13
        EditLabel.Caption = 'Begin Address (Hex) :'
        LabelPosition = lpLeft
        LabelSpacing = 3
        TabOrder = 0
      end
      object m_Group_EraseFlag: TRadioGroup
        Left = 40
        Top = 128
        Width = 585
        Height = 105
        Caption = ' Erase Flag '
        Items.Strings = (
          'Normal Erase (Bad block is skipped)'
          'Forcedly Erase (Bad block mark is erased)'
          'Mark as Bad Block')
        TabOrder = 2
      end
    end
    object m_Panel_DisableFormat1: TPanel
      Left = 16
      Top = 16
      Width = 177
      Height = 25
      BevelOuter = bvLowered
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      object m_Radio_DisableFormat: TRadioButton
        Left = 8
        Top = 4
        Width = 129
        Height = 17
        Caption = 'Do Not Format FAT'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = m_Radio_DisableFormatClick
      end
    end
    object m_Panel_DisableFormat2: TPanel
      Left = 16
      Top = 40
      Width = 649
      Height = 33
      BevelOuter = bvLowered
      TabOrder = 7
      object Label3: TLabel
        Left = 43
        Top = 8
        Width = 338
        Height = 16
        Caption = 'The FAT area is not formatted after the download process.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object CheckBox1: TCheckBox
        Left = 672
        Top = 40
        Width = 97
        Height = 17
        Caption = 'CheckBox1'
        TabOrder = 0
      end
    end
    object m_Panel_AutoFormat2: TPanel
      Left = 16
      Top = 190
      Width = 649
      Height = 59
      BevelOuter = bvLowered
      TabOrder = 1
      object Label1: TLabel
        Left = 43
        Top = 7
        Width = 265
        Height = 16
        Caption = 'FlashTool automatically formats the FAT area.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label2: TLabel
        Left = 42
        Top = 32
        Width = 370
        Height = 16
        Caption = 'The format begin address and  format length need not to be set.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
    end
    object m_Panel_ResetToFactoryDefault1: TPanel
      Left = 16
      Top = 80
      Width = 177
      Height = 25
      BevelOuter = bvLowered
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
      object m_Radio_ResetToFactoryDefault: TRadioButton
        Left = 8
        Top = 4
        Width = 161
        Height = 21
        Caption = 'Reset to Factory Default'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = m_Radio_ResetToFactoryDefaultClick
      end
    end
    object m_Panel_ResetToFactoryDefault2: TPanel
      Left = 16
      Top = 104
      Width = 649
      Height = 57
      BevelOuter = bvLowered
      TabOrder = 8
      object Label5: TLabel
        Left = 43
        Top = 8
        Width = 499
        Height = 16
        Caption = 
          'FlashTool resets  NVRAM data items that have NVRAM_CATEGORY_FACT' +
          'ORY attribute.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label9: TLabel
        Left = 43
        Top = 31
        Width = 212
        Height = 16
        Caption = 'NOTICE! Only available from w05.52.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
    end
    object m_FormatValidation: TCheckBox
      Left = 688
      Top = 112
      Width = 73
      Height = 25
      Hint = 'Perform read back check to verify if all the data is 0xFF'
      Caption = 'Validation'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = m_FormatValidationClick
    end
    object m_Panel_TotalFormat1: TPanel
      Left = 16
      Top = 256
      Width = 177
      Height = 25
      BevelOuter = bvLowered
      TabOrder = 9
      object m_Radio_TotalFormat: TRadioButton
        Left = 11
        Top = 8
        Width = 113
        Height = 14
        Caption = 'Total Format'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = m_Radio_TotalFormatClick
      end
    end
    object m_Panel_TotalFormat2: TPanel
      Left = 16
      Top = 280
      Width = 649
      Height = 73
      BevelOuter = bvLowered
      TabOrder = 10
      object Label6: TLabel
        Left = 40
        Top = 8
        Width = 298
        Height = 16
        Caption = 'FlashTool automatically formats the total flash area.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label7: TLabel
        Left = 40
        Top = 47
        Width = 214
        Height = 16
        Caption = 'WARNING! Calibration data is erased.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Pitch = fpFixed
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label10: TLabel
        Left = 40
        Top = 28
        Width = 370
        Height = 16
        Caption = 'The format begin address and  format length need not to be set.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
    end
  end
  object btn_FormatCancel: TButton
    Left = 696
    Top = 72
    Width = 89
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = btn_FormatCancelClick
  end
  object btn_FormatOK: TButton
    Left = 696
    Top = 32
    Width = 89
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = btn_FormatOKClick
  end
end
