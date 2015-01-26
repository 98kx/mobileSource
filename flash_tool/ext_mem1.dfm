object ext_mem1Form: Text_mem1Form
  Left = 242
  Top = 207
  BorderStyle = bsDialog
  Caption = 'EMI & NFI Setup'
  ClientHeight = 538
  ClientWidth = 940
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
  object GroupBox1: TGroupBox
    Left = 8
    Top = 3
    Width = 921
    Height = 526
    TabOrder = 1
    object m_Panel_ManualEMI_1: TPanel
      Left = 16
      Top = 16
      Width = 137
      Height = 25
      BevelOuter = bvLowered
      Caption = 'EMI Configuration'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
    object Panel1: TPanel
      Left = 16
      Top = 40
      Width = 284
      Height = 97
      BevelOuter = bvLowered
      TabOrder = 2
      object Label5: TLabel
        Left = 19
        Top = 23
        Width = 91
        Height = 13
        Align = alCustom
        Caption = 'NOR EMI (ECS0) : '
      end
      object Label6: TLabel
        Left = 12
        Top = 55
        Width = 98
        Height = 13
        Caption = 'SRAM EMI (ECS1) : '
      end
      object m_Bank1_Option: TComboBox
        Left = 112
        Top = 52
        Width = 69
        Height = 21
        ImeMode = imClose
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 0
        Text = 'MANUAL'
        OnChange = m_Bank1_OptionChange
        Items.Strings = (
          'MANUAL'
          'AUTO')
      end
      object m_Bank0_Option: TComboBox
        Left = 112
        Top = 20
        Width = 69
        Height = 21
        BiDiMode = bdLeftToRight
        ImeMode = imClose
        ItemHeight = 13
        ItemIndex = 0
        ParentBiDiMode = False
        TabOrder = 1
        Text = 'MANUAL'
        OnChange = m_Bank0_OptionChange
        Items.Strings = (
          'MANUAL'
          'AUTO')
      end
      object m_Bank0_EMI: TEdit
        Left = 188
        Top = 20
        Width = 70
        Height = 21
        TabOrder = 2
        Text = '0x00804102'
      end
      object m_Bank1_EMI: TEdit
        Left = 188
        Top = 52
        Width = 70
        Height = 21
        TabOrder = 3
        Text = '0x00804102'
      end
    end
    object m_Panel_ManualEMI_2: TPanel
      Left = 16
      Top = 136
      Width = 284
      Height = 289
      BevelOuter = bvLowered
      TabOrder = 1
      object Label4: TLabel
        Left = 15
        Top = 17
        Width = 205
        Height = 13
        Caption = 'Please setup the EMI config for each bank.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label3: TLabel
        Left = 15
        Top = 41
        Width = 248
        Height = 39
        Caption = 
          'If you choose AUTO config, FlashTool automatically configure the' +
          ' EMI setting by different external clock rate. For example: '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label1: TLabel
        Left = 34
        Top = 88
        Width = 174
        Height = 39
        Caption = 
          '13MHz:  EMI setting is 0x00004001. 26MHz:  EMI setting is 0x0000' +
          '4102. 52MHz:  EMI setting is 0x00004304.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object Label2: TLabel
        Left = 15
        Top = 140
        Width = 253
        Height = 26
        Caption = 
          'If you choose MANUAL config, you have to manually specify the EM' +
          'I config for each bank.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
    end
    object Panel2: TPanel
      Left = 306
      Top = 40
      Width = 527
      Height = 385
      BevelOuter = bvLowered
      TabOrder = 3
      object Label9: TLabel
        Left = 55
        Top = 34
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI GEN A:'
      end
      object Label15: TLabel
        Left = 55
        Top = 66
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI GEN B:'
      end
      object Label10: TLabel
        Left = 55
        Top = 162
        Width = 54
        Height = 13
        Align = alCustom
        Caption = 'EMI CON I:'
      end
      object Label11: TLabel
        Left = 38
        Top = 194
        Width = 72
        Height = 13
        Align = alCustom
        Caption = 'EMI CON I Ext:'
      end
      object Label12: TLabel
        Left = 55
        Top = 258
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI CON K:'
      end
      object Label13: TLabel
        Left = 55
        Top = 290
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI CON L:'
      end
      object Label7: TLabel
        Left = 55
        Top = 98
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI GEN C:'
      end
      object Label8: TLabel
        Left = 55
        Top = 322
        Width = 60
        Height = 13
        Align = alCustom
        Caption = 'EMI CON M:'
      end
      object Label16: TLabel
        Left = 55
        Top = 130
        Width = 59
        Height = 13
        Align = alCustom
        Caption = 'EMI GEN D:'
      end
      object Label17: TLabel
        Left = 55
        Top = 226
        Width = 56
        Height = 13
        Align = alCustom
        Caption = 'EMI CON J:'
      end
      object Label18: TLabel
        Left = 55
        Top = 354
        Width = 59
        Height = 13
        Align = alCustom
        Caption = 'EMI CON N:'
      end
      object Label19: TLabel
        Left = 215
        Top = 34
        Width = 56
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL A:'
      end
      object Label20: TLabel
        Left = 215
        Top = 66
        Width = 56
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL B:'
      end
      object Label21: TLabel
        Left = 215
        Top = 98
        Width = 56
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL C:'
      end
      object Label22: TLabel
        Left = 215
        Top = 130
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL D:'
      end
      object Label23: TLabel
        Left = 215
        Top = 162
        Width = 56
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL E:'
      end
      object Label24: TLabel
        Left = 215
        Top = 194
        Width = 55
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL F:'
      end
      object Label25: TLabel
        Left = 215
        Top = 226
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL G:'
      end
      object Label26: TLabel
        Left = 215
        Top = 258
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL H:'
      end
      object Label27: TLabel
        Left = 215
        Top = 290
        Width = 52
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL I:'
      end
      object Label28: TLabel
        Left = 215
        Top = 322
        Width = 54
        Height = 13
        Align = alCustom
        Caption = 'EMI DEL J:'
      end
      object Label29: TLabel
        Left = 373
        Top = 34
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB A:'
      end
      object Label30: TLabel
        Left = 373
        Top = 66
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB B:'
      end
      object Label31: TLabel
        Left = 373
        Top = 98
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB C:'
      end
      object Label32: TLabel
        Left = 373
        Top = 130
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB D:'
      end
      object Label33: TLabel
        Left = 373
        Top = 162
        Width = 57
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB E:'
      end
      object Label34: TLabel
        Left = 373
        Top = 194
        Width = 56
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB F:'
      end
      object Label35: TLabel
        Left = 373
        Top = 226
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB G:'
      end
      object Label36: TLabel
        Left = 373
        Top = 258
        Width = 58
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB H:'
      end
      object Label37: TLabel
        Left = 373
        Top = 290
        Width = 53
        Height = 13
        Align = alCustom
        Caption = 'EMI ARB I:'
      end
      object m_EnableDRAMSetting: TCheckBox
        Left = 8
        Top = 6
        Width = 225
        Height = 17
        Caption = 'Enable Manual DRAM EMI Configuration'
        TabOrder = 0
        OnClick = m_EnableDRAMSettingClick
      end
      object m_emi_gen_a: TEdit
        Left = 122
        Top = 30
        Width = 70
        Height = 21
        TabOrder = 1
        Text = '0'
      end
      object m_emi_gen_b: TEdit
        Left = 122
        Top = 61
        Width = 70
        Height = 21
        TabOrder = 2
        Text = '0'
      end
      object m_emi_con_i: TEdit
        Left = 122
        Top = 156
        Width = 70
        Height = 21
        TabOrder = 3
        Text = '0'
      end
      object m_emi_con_i_ext: TEdit
        Left = 122
        Top = 188
        Width = 70
        Height = 21
        TabOrder = 4
        Text = '0'
      end
      object m_emi_con_k: TEdit
        Left = 122
        Top = 252
        Width = 70
        Height = 21
        TabOrder = 5
        Text = '0'
      end
      object m_emi_con_l: TEdit
        Left = 122
        Top = 284
        Width = 70
        Height = 21
        TabOrder = 6
        Text = '0'
      end
      object m_emi_gen_c: TEdit
        Left = 122
        Top = 93
        Width = 70
        Height = 21
        TabOrder = 7
        Text = '0'
      end
      object m_emi_con_m: TEdit
        Left = 122
        Top = 316
        Width = 70
        Height = 21
        TabOrder = 8
        Text = '0'
      end
      object m_emi_gen_d: TEdit
        Left = 122
        Top = 125
        Width = 70
        Height = 21
        TabOrder = 9
        Text = '0'
      end
      object m_emi_con_j: TEdit
        Left = 122
        Top = 220
        Width = 70
        Height = 21
        TabOrder = 10
        Text = '0'
      end
      object m_emi_con_n: TEdit
        Left = 122
        Top = 348
        Width = 70
        Height = 21
        TabOrder = 11
        Text = '0'
      end
      object m_emi_del_a: TEdit
        Left = 280
        Top = 30
        Width = 70
        Height = 21
        TabOrder = 12
        Text = '0'
      end
      object m_emi_del_b: TEdit
        Left = 280
        Top = 62
        Width = 70
        Height = 21
        TabOrder = 13
        Text = '0'
      end
      object m_emi_del_c: TEdit
        Left = 280
        Top = 94
        Width = 70
        Height = 21
        TabOrder = 14
        Text = '0'
      end
      object m_emi_del_d: TEdit
        Left = 280
        Top = 126
        Width = 70
        Height = 21
        TabOrder = 15
        Text = '0'
      end
      object m_emi_del_e: TEdit
        Left = 280
        Top = 158
        Width = 70
        Height = 21
        TabOrder = 16
        Text = '0'
      end
      object m_emi_del_f: TEdit
        Left = 280
        Top = 190
        Width = 70
        Height = 21
        TabOrder = 17
        Text = '0'
      end
      object m_emi_del_g: TEdit
        Left = 280
        Top = 222
        Width = 70
        Height = 21
        TabOrder = 18
        Text = '0'
      end
      object m_emi_del_h: TEdit
        Left = 280
        Top = 254
        Width = 70
        Height = 21
        TabOrder = 19
        Text = '0'
      end
      object m_emi_del_i: TEdit
        Left = 280
        Top = 286
        Width = 70
        Height = 21
        TabOrder = 20
        Text = '0'
      end
      object m_emi_del_j: TEdit
        Left = 280
        Top = 318
        Width = 70
        Height = 21
        TabOrder = 21
        Text = '0'
      end
      object m_emi_arb_a: TEdit
        Left = 438
        Top = 30
        Width = 70
        Height = 21
        TabOrder = 22
        Text = '0'
      end
      object m_emi_arb_b: TEdit
        Left = 438
        Top = 62
        Width = 70
        Height = 21
        TabOrder = 23
        Text = '0'
      end
      object m_emi_arb_c: TEdit
        Left = 438
        Top = 94
        Width = 70
        Height = 21
        TabOrder = 24
        Text = '0'
      end
      object m_emi_arb_d: TEdit
        Left = 438
        Top = 126
        Width = 70
        Height = 21
        TabOrder = 25
        Text = '0'
      end
      object m_emi_arb_e: TEdit
        Left = 438
        Top = 158
        Width = 70
        Height = 21
        TabOrder = 26
        Text = '0'
      end
      object m_emi_arb_f: TEdit
        Left = 438
        Top = 190
        Width = 70
        Height = 21
        TabOrder = 27
        Text = '0'
      end
      object m_emi_arb_g: TEdit
        Left = 438
        Top = 222
        Width = 70
        Height = 21
        TabOrder = 28
        Text = '0'
      end
      object m_emi_arb_h: TEdit
        Left = 438
        Top = 254
        Width = 70
        Height = 21
        TabOrder = 29
        Text = '0'
      end
      object m_emi_arb_i: TEdit
        Left = 438
        Top = 286
        Width = 70
        Height = 21
        TabOrder = 30
        Text = '0'
      end
    end
    object Panel4: TPanel
      Left = 16
      Top = 440
      Width = 137
      Height = 25
      BevelOuter = bvLowered
      Caption = 'NFI Configuration'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 5
    end
    object Panel3: TPanel
      Left = 16
      Top = 464
      Width = 817
      Height = 49
      BevelOuter = bvLowered
      TabOrder = 4
      object Label14: TLabel
        Left = 18
        Top = 18
        Width = 81
        Height = 13
        Align = alCustom
        Caption = 'NAND ACCCON:'
      end
      object m_nand_acccon: TEdit
        Left = 186
        Top = 14
        Width = 70
        Height = 21
        TabOrder = 0
        Text = '0x000007FF'
      end
      object m_nfi_Option: TComboBox
        Left = 104
        Top = 14
        Width = 69
        Height = 21
        ImeMode = imClose
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 1
        Text = 'MANUAL'
        OnChange = m_nfi_OptionChange
        Items.Strings = (
          'MANUAL'
          'AUTO')
      end
    end
  end
  object btn_EMI_Cancel: TButton
    Left = 853
    Top = 80
    Width = 65
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = btn_EMI_CancelClick
  end
  object btn_EMI_OK: TButton
    Left = 853
    Top = 48
    Width = 65
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = btn_EMI_OKClick
  end
end
