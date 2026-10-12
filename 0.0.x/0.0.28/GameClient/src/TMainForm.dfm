object MainForm: TMainForm
  Left = 386
  Top = 142
  BorderStyle = bsNone
  Caption = 'Endless Online'
  ClientHeight = 488
  ClientWidth = 689
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = True
  Scaled = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseDown = FormMouseDown
  OnMouseMove = FormMouseMove
  OnMouseUp = FormMouseUp
  OnMouseWheelDown = FormMouseWheelDown
  OnMouseWheelUp = FormMouseWheelUp
  PixelsPerInch = 96
  TextHeight = 13
  object Dx_Draw1: TDx_Draw
    OnCreate = Dx_Draw1Create
    OnDestroy = Dx_Draw1Destroy
    OnEnumDisplayModes = Dx_Draw1EnumDisplayModes
    Left = 6
    Top = 9
  end
  object PrimarySurface: TDx_DrawSurface
    OnCreate = PrimarySurfaceCreate
    OnSurfaceLost = PrimarySurfaceSurfaceLost
    Left = 38
    Top = 9
  end
  object PrimarySurfaceDesc: TDDSurfaceDesc
    Flags_ = [ddsd_backbuffercount, ddsd_caps]
    SCaps = PrimaryCaps
    Left = 38
    Top = 41
  end
  object PrimaryCaps: TDDSCaps
    Caps_ = [ddscaps_primarysurface, ddscaps_videomemory]
    Left = 38
    Top = 73
  end
  object PrimaryClipper: TDx_DrawClipper
    Left = 6
    Top = 41
  end
  object BackBuffer: TDx_DrawSurface
    OnSurfaceLost = PrimarySurfaceSurfaceLost
    Left = 70
    Top = 9
  end
  object BackBufferDesc: TDDSurfaceDesc
    Flags_ = [ddsd_caps]
    SCaps = BackBufferCaps
    Left = 70
    Top = 41
  end
  object BackBufferCaps: TDDSCaps
    Caps_ = [ddscaps_backbuffer, ddscaps_videomemory]
    Left = 70
    Top = 73
  end
  object Dx_Sound1: TDx_Sound
    OnCreate = Dx_Sound1Create
    OnDestroy = Dx_Sound1Destroy
    OnDSEnumerate = Dx_Sound1DSEnumerate
    Left = 414
    Top = 9
  end
  object Dx_SoundBuffer1: TDx_SoundBuffer
    Left = 446
    Top = 9
  end
  object DSBufferDesc1: TDSBufferDesc
    Flags_ = [dsbcaps_ctrlvolume, dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 446
    Top = 41
  end
  object Dx_SoundBuffer2: TDx_SoundBuffer
    Left = 478
    Top = 9
  end
  object DSBufferDesc2: TDSBufferDesc
    Flags_ = [dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 478
    Top = 41
  end
  object Dx_SoundBuffer3: TDx_SoundBuffer
    Left = 510
    Top = 9
  end
  object DSBufferDesc3: TDSBufferDesc
    Flags_ = [dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 510
    Top = 41
  end
  object Dx_SoundBuffer4: TDx_SoundBuffer
    Left = 542
    Top = 9
  end
  object DSBufferDesc4: TDSBufferDesc
    Flags_ = [dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 542
    Top = 41
  end
  object Dx_SoundBuffer5: TDx_SoundBuffer
    Left = 574
    Top = 9
  end
  object Dx_SoundBuffer6: TDx_SoundBuffer
    Left = 606
    Top = 9
  end
  object DSBufferDesc5: TDSBufferDesc
    Flags_ = [dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 574
    Top = 41
  end
  object DSBufferDesc6: TDSBufferDesc
    Flags_ = [dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 606
    Top = 41
  end
  object Dx_SoundBuffer7: TDx_SoundBuffer
    Left = 638
    Top = 9
  end
  object DSBufferDesc7: TDSBufferDesc
    Flags_ = [dsbcaps_getcurrentposition2_, dsbcaps_globalfocus]
    Left = 638
    Top = 41
  end
  object OffscreenSurface: TDx_DrawSurface
    Left = 102
    Top = 9
  end
  object OffscreenDesc: TDDSurfaceDesc
    Flags_ = [ddsd_caps]
    SCaps = OffscreenCaps
    Left = 102
    Top = 41
  end
  object OffscreenCaps: TDDSCaps
    Caps_ = [ddscaps_offscreenplain, ddscaps_systemmemory]
    Left = 102
    Top = 73
  end
  object OffscreenPixelFormat: TDDPixelFormat
    Left = 6
    Top = 73
  end
  object Dx_Input1: TDx_Input
    OnDestroy = Dx_Input1Destroy
    Left = 134
    Top = 9
  end
  object DIDevCaps1: TDIDevCaps
    Left = 134
    Top = 41
  end
  object DIKeyboardState1: TDIKeyboardState
    Left = 166
    Top = 41
  end
  object Dx_InputDevice1: TDx_InputDevice
    OnCreate = Dx_InputDevice1Create
    OnDestroy = Dx_InputDevice1Destroy
    Left = 166
    Top = 9
  end
  object client: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 8078
    OnConnect = clientConnect
    OnDisconnect = clientDisconnect
    OnRead = clientRead
    OnError = clientError
    Left = 134
    Top = 73
  end
  object ApplicationEvents1: TApplicationEvents
    OnException = ApplicationEvents1Exception
    Left = 166
    Top = 73
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 12
    OnTimer = Timer1Timer
    Left = 414
    Top = 41
  end
end
