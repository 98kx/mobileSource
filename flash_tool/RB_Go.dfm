object Form_RB_GO: TForm_RB_GO
  Left = 78
  Top = 281
  BorderStyle = bsDialog
  Caption = 'Readback'
  ClientHeight = 136
  ClientWidth = 331
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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 24
    Top = 16
    Width = 289
    Height = 25
    AutoSize = False
    Caption = 'Please reset target and click [Go !] button to readback.'
  end
  object bb_RB_Go: TBitBtn
    Left = 64
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Go !'
    TabOrder = 0
    OnClick = bb_RB_GoClick
    Kind = bkOK
  end
  object bb_RB_Cancel: TBitBtn
    Left = 208
    Top = 96
    Width = 75
    Height = 25
    TabOrder = 1
    OnClick = bb_RB_CancelClick
    Kind = bkCancel
  end
end
