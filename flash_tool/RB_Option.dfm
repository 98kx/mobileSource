object Form_RB_Option: TForm_RB_Option
  Left = 287
  Top = 177
  BorderStyle = bsDialog
  Caption = 'Readback block start address'
  ClientHeight = 352
  ClientWidth = 420
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
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object bb_ReadBack_OK: TBitBtn
    Left = 336
    Top = 32
    Width = 75
    Height = 25
    TabOrder = 1
    Visible = False
    OnClick = bb_ReadBack_OKClick
    Kind = bkOK
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 313
    Height = 337
    ActivePage = TabSheet1
    TabIndex = 0
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Readback block start address'
      object gb_Readback_Parameter_Type: TGroupBox
        Left = 16
        Top = 8
        Width = 241
        Height = 73
        Caption = 'Type'
        TabOrder = 0
        TabStop = True
        object rb_ReadbackHex: TRadioButton
          Left = 24
          Top = 16
          Width = 113
          Height = 17
          Caption = 'Hex'
          Checked = True
          TabOrder = 0
          TabStop = True
          OnClick = rb_ReadbackHexClick
        end
        object rb_ReadbackDec: TRadioButton
          Left = 24
          Top = 40
          Width = 185
          Height = 17
          Caption = 'Decimal (auto convert to Hex)'
          TabOrder = 1
          OnClick = rb_ReadbackFlagClick
        end
      end
      object m_Group_ReadBackFlag: TGroupBox
        Left = 16
        Top = 88
        Width = 241
        Height = 113
        Caption = ' Read Method '
        TabOrder = 1
        TabStop = True
        object m_Radio_ReadPageSpare: TRadioButton
          Left = 24
          Top = 16
          Width = 201
          Height = 17
          Caption = 'Read Page + Spare'
          Checked = True
          TabOrder = 0
          TabStop = True
        end
        object m_Radio_ReadPageOnly: TRadioButton
          Left = 24
          Top = 40
          Width = 185
          Height = 17
          Caption = 'Read Page Only'
          TabOrder = 1
        end
        object m_Radio_ReadSpareOnly: TRadioButton
          Left = 24
          Top = 64
          Width = 185
          Height = 17
          Caption = 'Read Spare Only'
          TabOrder = 2
        end
        object m_Radio_ReadPageWithECC: TRadioButton
          Left = 24
          Top = 88
          Width = 185
          Height = 17
          Caption = 'Read Page With ECC'
          TabOrder = 3
        end
      end
      object m_Group_ReadBackRange: TGroupBox
        Left = 16
        Top = 208
        Width = 241
        Height = 89
        TabOrder = 2
        object lb_StartAddr: TLabel
          Left = 16
          Top = 20
          Width = 72
          Height = 13
          Alignment = taRightJustify
          Caption = 'Start Address : '
        end
        object lb_Length: TLabel
          Left = 47
          Top = 52
          Width = 42
          Height = 13
          Alignment = taRightJustify
          Caption = 'Length : '
        end
        object ed_RB_Length: TEdit
          Left = 96
          Top = 52
          Width = 121
          Height = 21
          TabOrder = 1
          Text = '0'
        end
        object ed_RB_StartAddr: TEdit
          Left = 96
          Top = 20
          Width = 121
          Height = 21
          TabOrder = 0
          Text = '0'
        end
      end
    end
  end
  object bb_Cancel: TBitBtn
    Left = 336
    Top = 64
    Width = 75
    Height = 25
    TabOrder = 2
    OnClick = bb_CancelClick
    Kind = bkCancel
  end
end
