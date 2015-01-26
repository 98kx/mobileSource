object COM_Option: TCOM_Option
  Left = 217
  Top = 132
  Width = 490
  Height = 372
  Caption = 'COM Port Option'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 7
    Top = 5
    Width = 466
    Height = 332
    TabOrder = 0
    object m_Panel_COM_Option: TPanel
      Left = 16
      Top = 16
      Width = 169
      Height = 25
      BevelOuter = bvLowered
      Caption = 'COM Port Timeout'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
    object Panel1: TPanel
      Left = 16
      Top = 40
      Width = 353
      Height = 105
      BevelOuter = bvLowered
      TabOrder = 1
      object Label5: TLabel
        Left = 25
        Top = 23
        Width = 98
        Height = 13
        Align = alCustom
        Caption = 'Read Timeout (ms) : '
      end
      object Label6: TLabel
        Left = 25
        Top = 55
        Width = 97
        Height = 13
        Caption = 'Write Timeout (ms) : '
      end
      object m_COM_Read_Timeout: TEdit
        Left = 228
        Top = 20
        Width = 70
        Height = 21
        TabOrder = 0
        Text = '12345678'
      end
      object m_COM_Read_Timeout_Option: TComboBox
        Left = 128
        Top = 20
        Width = 91
        Height = 21
        BiDiMode = bdLeftToRight
        ImeMode = imClose
        ItemHeight = 13
        ItemIndex = 0
        ParentBiDiMode = False
        TabOrder = 1
        Text = 'NO TIMEOUT'
        OnChange = m_COM_Read_Timeout_OptionChange
        Items.Strings = (
          'NO TIMEOUT'
          'DEFAULT'
          'CUSTOM')
      end
      object m_COM_Write_Timeout_Option: TComboBox
        Left = 128
        Top = 52
        Width = 91
        Height = 21
        ImeMode = imClose
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 2
        Text = 'NO TIMEOUT'
        OnChange = m_COM_Write_Timeout_OptionChange
        Items.Strings = (
          'NO TIMEOUT'
          'DEFAULT'
          'CUSTOM')
      end
      object m_COM_Write_Timeout: TEdit
        Left = 228
        Top = 52
        Width = 70
        Height = 21
        TabOrder = 3
        Text = '12345678'
      end
    end
    object Panel2: TPanel
      Left = 16
      Top = 160
      Width = 169
      Height = 25
      BevelOuter = bvLowered
      Caption = 'Baudrate Full Sync'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
    end
    object Panel3: TPanel
      Left = 16
      Top = 184
      Width = 353
      Height = 129
      BevelOuter = bvLowered
      TabOrder = 3
      object Label1: TLabel
        Left = 25
        Top = 23
        Width = 83
        Height = 13
        Align = alCustom
        Caption = 'Full Sync Count : '
      end
      object Label2: TLabel
        Left = 24
        Top = 56
        Width = 293
        Height = 26
        Caption = 
          'Baudrate Full Sync is sending char from 0x00 to 0xFF to test if ' +
          'the baudrate is stable.'
        WordWrap = True
      end
      object Label3: TLabel
        Left = 24
        Top = 88
        Width = 304
        Height = 26
        Caption = 
          'Each char will repeatly send for N times. N is the Full Sync Cou' +
          'nt you set. If N is zero, it means the Baudrate Full Sync is dis' +
          'abled.'
        WordWrap = True
      end
      object m_Baudrate_FullSync_Count: TEdit
        Left = 108
        Top = 20
        Width = 70
        Height = 21
        TabOrder = 0
        Text = '12345678'
      end
    end
  end
  object btn_COM_Option_OK: TButton
    Left = 391
    Top = 48
    Width = 65
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = btn_COM_Option_OKClick
  end
  object btn_COM_Option_Cancel: TButton
    Left = 391
    Top = 80
    Width = 65
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = btn_COM_Option_CancelClick
  end
end
