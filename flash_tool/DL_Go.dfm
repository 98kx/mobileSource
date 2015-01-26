object Form_DL_GO: TForm_DL_GO
  Left = 78
  Top = 119
  BorderStyle = bsDialog
  Caption = 'Download'
  ClientHeight = 136
  ClientWidth = 330
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
    Caption = 'Please reset target and click [Go !] button to download.'
  end
  object bb_DL_Go: TBitBtn
    Left = 64
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Go !'
    TabOrder = 0
    OnClick = bb_DL_GoClick
    Kind = bkOK
  end
  object bb_DL_Cancel: TBitBtn
    Left = 208
    Top = 96
    Width = 75
    Height = 25
    TabOrder = 1
    OnClick = bb_DL_CancelClick
    Kind = bkCancel
  end
end
