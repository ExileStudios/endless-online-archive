object MainForm: TMainForm
  Left = 356
  Top = 140
  Width = 242
  Height = 118
  Caption = 'Advanced Watchdog'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object trayermenu: TPopupMenu
    MenuAnimation = [maBottomToTop]
    OnPopup = trayermenuPopup
    Left = 8
    Top = 8
    object connected1: TMenuItem
      Caption = 'connected'
      OnClick = connected1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object watchdog1: TMenuItem
      Caption = 'watchdog'
      OnClick = watchdog1Click
    end
    object generator1: TMenuItem
      Caption = 'generator'
      OnClick = generator1Click
    end
    object logdowntime1: TMenuItem
      Caption = 'log downtime'
      OnClick = logdowntime1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object exitwatchdog1: TMenuItem
      Caption = 'exit watchdog'
      OnClick = exitwatchdog1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object cancel1: TMenuItem
      Caption = 'cancel'
    end
  end
  object watchtimer: TTimer
    OnTimer = watchtimerTimer
    Left = 40
    Top = 8
  end
  object client: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Host = '127.0.0.1'
    Port = 8078
    OnConnect = clientConnect
    OnRead = clientRead
    OnError = clientError
    Left = 72
    Top = 8
  end
  object ApplicationEvents1: TApplicationEvents
    OnException = ApplicationEvents1Exception
    Left = 104
    Top = 8
  end
end
