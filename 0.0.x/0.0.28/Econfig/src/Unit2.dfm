object Form2: TForm2
  Left = 584
  Top = 243
  BorderStyle = bsDialog
  Caption = 'Information'
  ClientHeight = 194
  ClientWidth = 415
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 4
    Top = 4
    Width = 407
    Height = 132
    Align = alClient
    BorderStyle = bsNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Lines.Strings = (
      'Memo1'
    )
    ParentFont = False
    ReadOnly = True
    TabOrder = 5
  end
  object Panel14: TPanel
    Left = 0
    Top = 0
    Width = 415
    Height = 4
    Align = alTop
    BevelOuter = bvNone
    Color = 15856629
    TabOrder = 0
  end
  object Panel16: TPanel
    Left = 0
    Top = 4
    Width = 4
    Height = 132
    Align = alLeft
    BevelOuter = bvNone
    Color = 15856629
    TabOrder = 1
  end
  object Panel13: TPanel
    Left = 0
    Top = 136
    Width = 415
    Height = 4
    Align = alBottom
    BevelOuter = bvNone
    Color = 15856629
    TabOrder = 2
  end
  object Panel2: TPanel
    Left = 0
    Top = 140
    Width = 415
    Height = 54
    Align = alBottom
    BevelOuter = bvNone
    Color = 15133164
    TabOrder = 3
    object SpeedButton2: TSpeedButton
      Left = 144
      Top = 7
      Width = 137
      Height = 41
      Hint = 'exit configuration without making any changes'
      Caption = 'ok'
      Flat = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      OnClick = SpeedButton2Click
    end
  end
  object Panel15: TPanel
    Left = 411
    Top = 4
    Width = 4
    Height = 132
    Align = alRight
    BevelOuter = bvNone
    Color = 15856629
    TabOrder = 4
  end
end
