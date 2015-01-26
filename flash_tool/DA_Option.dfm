object Form_DA_Option: TForm_DA_Option
  Left = 296
  Top = 172
  BorderStyle = bsDialog
  Caption = 'Non-standard Download Agent Start Address'
  ClientHeight = 116
  ClientWidth = 373
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 265
    Height = 97
    ActivePage = ts_Download
    TabIndex = 0
    TabOrder = 2
    object ts_Download: TTabSheet
      Caption = 'Non-standard DA Start Address'
      ImageIndex = 2
      object Label3: TLabel
        Left = 8
        Top = 18
        Width = 69
        Height = 13
        Caption = 'Start Address: '
      end
      object ed_StartAddr: TMaskEdit
        Left = 80
        Top = 16
        Width = 113
        Height = 21
        Hint = 'Default 0x40000800'
        CharCase = ecLowerCase
        EditMask = '\0xAAAAAAAA;'
        MaxLength = 10
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        Text = '0x        '
        OnKeyPress = ed_StartAddrKeyPress
      end
    end
  end
  object bb_DA_Cancel: TBitBtn
    Left = 288
    Top = 40
    Width = 75
    Height = 25
    TabOrder = 1
    OnClick = bb_DA_CancelClick
    Kind = bkCancel
  end
  object bb_OK: TBitBtn
    Left = 288
    Top = 8
    Width = 75
    Height = 25
    TabOrder = 0
    OnClick = bb_OKClick
    Kind = bkOK
  end
end
