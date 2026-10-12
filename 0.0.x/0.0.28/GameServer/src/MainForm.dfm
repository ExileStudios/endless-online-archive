object GUI: TGUI
  Left = 838
  Top = 208
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Endless Advanced Server'
  ClientHeight = 48
  ClientWidth = 456
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object panel_connections: TPanel
    Left = 5
    Top = 5
    Width = 205
    Height = 33
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 0
  end
  object panel_send: TPanel
    Left = 293
    Top = 5
    Width = 78
    Height = 33
    Hint = 'send'
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 1
  end
  object panel_received: TPanel
    Left = 373
    Top = 5
    Width = 78
    Height = 33
    Hint = 'received'
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 2
  end
  object panel_buffer: TPanel
    Left = 213
    Top = 5
    Width = 78
    Height = 33
    Hint = 'db buffer'
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 3
  end
  object server: TServerSocket
    Active = False
    Port = 0
    ServerType = stNonBlocking
    OnClientConnect = serverClientConnect
    OnClientDisconnect = serverClientDisconnect
    OnClientRead = serverClientRead
    OnClientError = serverClientError
    Left = 136
    Top = 6
  end
  object mysql: TDatabase
    AliasName = 'endless_database'
    DatabaseName = 'endless_db'
    LoginPrompt = False
    SessionName = 'myses'
    Left = 104
    Top = 6
  end
  object myquery: TQuery
    DatabaseName = 'endless_db'
    SessionName = 'myses'
    Left = 40
    Top = 6
  end
  object timer: TTimer
    Enabled = False
    Interval = 65
    OnTimer = timerTimer
    Left = 8
    Top = 6
  end
  object mysession: TSession
    SessionName = 'myses'
    SQLHourGlass = False
    Left = 72
    Top = 6
  end
  object ApplicationEvents1: TApplicationEvents
    OnException = ApplicationEvents1Exception
    Left = 167
    Top = 6
  end
end
