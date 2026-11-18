#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_effect.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include "Gfx_sprites.h"
#include "Gfx_tile.h"
#include "Gfx_wall.h"
#include "Packetproces.h"
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Gfx_blend.h"
#include "Ctrl_book.h"
#include "Ctrl_credits.h"
#include "Ctrl_dialog.h"
#include "Ctrl_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_into.h"
#include "Gfx_intro.h"
#include "Gfx_item.h"
#include "Ctrl_jukebox.h"
#include "Ctrl_law.h"
#include "Gfx_mask.h"
#include "Ctrl_memo.h"
#include "Ctrl_menu.h"
#include "Gfx_menu.h"
#include "Ctrl_musician.h"
#include "Gfx_npcs.h"
#include "Gfx_object.h"
#include "Packetoperations.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_quests.h"
#include "Ctrl_report.h"
#include "Ctrl_social.h"
#include "Ctrl_stats.h"
#include "Gfx_top.h"
#include "Ctrl_chat.h"
#include "Ctrl_gamebars.h"
#include "Ctrl_help.h"
#include "Ctrl_news.h"
#include "Ctrl_playerlist.h"
#include "Ctrl_settings.h"
#include "Ctrl_status.h"
#include <Dialogs.hpp>
#include <dinput.h>
#include <mmsystem.h>
#include <stdlib.h>
#include "Ctrl_chest.h"
#include "Ctrl_group.h"
#include "Ctrl_inventory.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_msgboard.h"
#include "Ctrl_shop.h"
#include "Ctrl_skills.h"
#include "Ctrl_spells.h"
#include "Ctrl_tooltip.h"
#include "Ctrl_trade.h"
#include "Gfx_mapblends.h"
#include "Gfx_mapeffects.h"
#include "Gfx_mapicons.h"
#include "Gfx_mapitems.h"
#include "Gfx_mapmasks.h"
#include "Gfx_mapobjects.h"
#include "Gfx_maptiles.h"
#include "Gfx_maptops.h"
#include "Gfx_mapwalls.h"
#include "Pathfinder.h"
#include "Ctrl_bossbars.h"
#include "Ctrl_popupelement.h"

#pragma package(smart_init)
#pragma link "TDx_Draw"
#pragma link "TDx_DrawClipper"
#pragma link "TDx_DrawSurface"
#pragma link "TDDSurfaceDesc"
#pragma link "TDDSCaps"
#pragma link "TDDPixelFormat"
#pragma link "TDx_Sound"
#pragma link "TDx_SoundBuffer"
#pragma link "TDSBufferDesc"
#pragma link "TDx_Input"
#pragma link "TDIDevCaps"
#pragma link "TDIKeyboardState"
#pragma link "TDx_InputDevice"
#pragma resource "*.dfm"

char *TMainForm_UnusedTable1[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};
TMainForm *MainForm;

String TMainForm_UnusedLiterals1()
{
    return "0x";
}

__fastcall TMainForm::TMainForm(TComponent *owner)
    : TForm(owner)
{
    mm_timer_id = 0;
    draw_error_count = 0;
    show_frame_rate = false;
    start_time = DateTimeToTimeStamp(Now());
    time_value_a = 0;
    time_value_b = 12;
    scale_menu = CreatePopupMenu();
    InsertMenuA(scale_menu, 0, MF_BYPOSITION, 170, String("1x").c_str());
    InsertMenuA(scale_menu, 1, MF_BYPOSITION, 171, String("2x").c_str());
    InsertMenuA(scale_menu, 2, MF_BYPOSITION, 172, String("3x").c_str());
    InsertMenuA(scale_menu, 3, MF_BYPOSITION, 173, String("4x").c_str());
    auto_drop_menu = CreatePopupMenu();
    InsertMenuA(auto_drop_menu, 0, MF_BYPOSITION, 174, String("Minimap Toggled").c_str());
    InsertMenuA(auto_drop_menu, 1, MF_BYPOSITION, 175, String("Minimap Overlayer v2").c_str());
    InsertMenuA(auto_drop_menu, 2, MF_BYPOSITION, 176, String("Minimap Radar").c_str());
    InsertMenuA(auto_drop_menu, 3, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
    InsertMenuA(auto_drop_menu, 4, MF_BYPOSITION, 177, String("Elemental Icons").c_str());
    InsertMenuA(auto_drop_menu, 5, MF_BYPOSITION, 178, String("Extended Tooltips").c_str());
    InsertMenuA(auto_drop_menu, 6, MF_BYPOSITION, 179, String("Minimap Icons").c_str());
    InsertMenuA(auto_drop_menu, 7, MF_BYPOSITION, 180, String("Minimap Grid").c_str());
    InsertMenuA(auto_drop_menu, 8, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
    InsertMenuA(auto_drop_menu, 9, MF_BYPOSITION, 181, String("Wasd movement").c_str());
    HMENU system_menu = GetSystemMenu(Handle, 0);
    InsertMenuA(system_menu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
    InsertMenuA(system_menu, 6, MF_POPUP | MF_BYPOSITION, (unsigned int)scale_menu, String("Size").c_str());
    InsertMenuA(system_menu, 6, MF_POPUP | MF_BYPOSITION, (unsigned int)auto_drop_menu, String("Game").c_str());
}

void __fastcall TMainForm::FormShow(TObject *Sender)
{
    UpdateSystemMenu();
}

// Checks the data files, then loads the settings and texts, starts DirectInput, DirectSound and DirectDraw, builds
// every controller and starts the timers.
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    minimized = false;
    active = true;
    login_screen_delay = 0;
    Application->OnMinimize = MinimizeForm;
    Application->OnRestore = RestoreForm;
    Application->OnActivate = ActivateForm;
    Application->OnDeactivate = DeactivateForm;
    version_major = 0;
    version_minor = 2;
    version_build = 29;
    version_letter = '0';
    files = new FileController();
    config = new Config();
    strings = new StringContainer(config->GetLanguage());
    chat_log = new FFileLog(config->GetLogChat(), config->GetChatLogPath());
    registry = new WinRegister();
    String title =
        "Endless Online " + String(version_major) + "." + String(version_minor) + "." + String(version_build);
    String start_error = "Endless could not start, because of error(s)\n\n";
    if (version_letter != '0')
        title += " " + String(version_letter);
    title += "\n\n";
    // Without valid data files only the file error message is shown, and the program ends.
    if (!files->VerifyFiles())
    {
        String files_message = title;
        files_message += "Endless File Protection Error\n";
        files_message += "Some files are missing or corrupted\n\n";
        files_message += files->error_message + "\n\n";
        files_message += "Please, completely uninstall Endless Online\n";
        files_message += "and re-install the correct client version.\n";
        files_message += "Goto: http://www.endless-online.com";
        if (MessageDlg(files_message, mtError, TMsgDlgButtons() << mbOK, 0) == mrOk)
        {
            delete files;
            delete config;
            delete strings;
            delete chat_log;
            delete registry;
        }
        Application->Terminate();
        return;
    }
    // Timer1Timer later counts its ticks in this member; every use of it as a width comes first.
    display_width = 640;
    display_height = 480;
    window_height_padding = 0;
    unused_value = 0;
    Width = display_width;
    Height = display_height;
    TPoint window_position = config->GetWindowPosition();
    if (window_position.x >= 0 && window_position.y >= 0)
    {
        Left = window_position.x;
        Top = window_position.y;
    }
    else if (window_position.x == -2)
        Position = poDesktopCenter;
    else
        Position = poScreenCenter;
    char start_fullscreen = config->GetFullscreen();
    config->SetFullscreen(false);
    if (Dx_Input1->Created)
    {
        Dx_Input1->Destroy();
        Sleep(150);
    }
    Dx_Input1->Create();
    if (!Dx_Input1->Created)
    {
        String input_message = title;
        input_message += start_error;
        input_message += "Reason: directX could not find keyboard or mouse.\n";
        input_message += "Suggestion: install or fix your directX runtime files";
        if (MessageDlg(input_message, mtError, TMsgDlgButtons() << mbOK, 0) == mrOk)
        {
            delete files;
            delete config;
            delete strings;
            delete chat_log;
            delete registry;
        }
        Dx_Input1->Destroy();
        Application->Terminate();
        return;
    }
    Dx_InputDevice1->Create(GUID_SysKeyboard, Dx_Input1);
    if (config->GetSoundVolume() > 0)
    {
        sound_driver_names = new TStringList();
        sound_driver_modules = new TStringList();
        sound_driver_guids = new TList();
        int driver = 0;
        if (Dx_Sound1->DSEnumerate(0))
        {
            do
            {
                if (Dx_Sound1->Created)
                {
                    Dx_Sound1->Destroy();
                    Sleep(150);
                }
                if (sound_driver_names->Count > 0)
                    Dx_Sound1->Create((_GUID *)sound_driver_guids->Items[driver]);
                driver++;
            } while (!Dx_Sound1->Created && driver < sound_driver_guids->Count);
        }
        if (!Dx_Sound1->Created)
        {
            config->SetSoundAvailable(0);
            config->SetSoundVolume(0);
        }
    }
    if (config->GetSoundVolume() > 0 && !Dx_Sound1->Created)
    {
        String sound_message = title;
        sound_message += start_error;
        sound_message += "Reason: directX directSound failed to start.\n";
        sound_message += "Suggestion: disable all sounds and music of your game";
        if (MessageDlg(sound_message, mtError, TMsgDlgButtons() << mbOK, 0) == mrOk)
        {
            delete files;
            delete config;
            delete strings;
            delete chat_log;
            delete registry;
        }
        Dx_Input1->Destroy();
        Dx_Sound1->Destroy();
        Application->Terminate();
        return;
    }
    if (!config->GetFullscreen())
    {
        if (!config->GetStayOnTop())
            FormStyle = fsNormal;
        if (config->GetSizeable())
            BorderStyle = bsSizeable;
        else
        {
            BorderStyle = bsSingle;
            BorderIcons >> biMaximize;
        }
    }
    if (Dx_Draw1->Created)
    {
        Dx_Draw1->Destroy();
        Sleep(150);
    }
    if (!config->GetDDrawHardware())
        Dx_Draw1->Create((_GUID *)2);
    else
        // Dx_Draw1Create makes the surfaces and, full screen, sets the display mode.
        Dx_Draw1->Create(0);
    if (!Dx_Draw1->Created)
    {
        String directx_message = title;
        directx_message += start_error;
        directx_message += "Reason: directX (min v.7) runtime was not found.\n";
        directx_message += "Suggestion: install directX runtime package";
        if (MessageDlg(directx_message, mtError, TMsgDlgButtons() << mbOK, 0) == mrOk)
        {
            delete files;
            delete config;
            delete strings;
            delete chat_log;
            delete registry;
        }
        Dx_Input1->Destroy();
        Dx_Sound1->Destroy();
        Application->Terminate();
        return;
    }
    if (!PrimarySurface->Created || !OffscreenSurface->Created)
    {
        String surface_message = title;
        surface_message += start_error;
        surface_message += "Reason: directX surfaces could not be initialized properly.\n";
        surface_message += "Suggestion: install or fix your directX files";
        if (MessageDlg(surface_message, mtError, TMsgDlgButtons() << mbOK, 0) == mrOk)
        {
            delete files;
            delete config;
            delete strings;
            delete chat_log;
            delete registry;
        }
        Dx_Input1->Destroy();
        Dx_Sound1->Destroy();
        Dx_Draw1->Destroy();
        Application->Terminate();
        return;
    }
    draw_canvas = new TCanvas();
    status_canvas = new TCanvas();
    text_canvas = new TCanvas();
    resource_bitmap = new Graphics::TBitmap();
    interface_bitmap = new Graphics::TBitmap();
    sound_mapper = new SoundMapper();
    magic_mapper = new MagicMapper();
    weapon_mapper = new WeaponMapper();
    keyboard = new KeyBoard();
    mouse = new Mouser();
    sfx = new SfxController(config->GetSoundVolume(), config->GetMusicVolume());
    animation = new AnimationController(config->GetSkipIntro());
    gfx = new GfxController(config->GetPreloadTileGfx(), config->GetPreloadMapGfx());
    map = new MapController();
    blitter = new ExtBlts(OffscreenSurface);
    players = new Players(config->GetPreloadCharacterGfx());
    npcs = new Npcs(config->GetPreloadCharacterGfx());
    effects = new Effects();
    map_items = new MapItems();
    item_values = new ItemValues(String(".\\pub\\"));
    npc_values = new NpcValues(String(".\\pub\\"));
    skill_values = new SkillValues(String(".\\pub\\"));
    class_values = new ClassValues(String(".\\pub\\"));
    edits = new Edits();
    edit_canvas = new EditCanvas();
    gfx->CreateMenuGraphics();
    gfx->LoadMenuGraphics(config->GetSkipSoftpad());
    gfx->LoadMainMenuScreen(true);
    gfx->LoadSpriteGraphics();
    net_client = new Client();
    flip_needed = false;
    redraw_needed = false;
    dragging = false;
    Width = display_width;
    Height = display_height;
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.bottom = 480;
    screen_rect.right = 640;
    if (!config->GetSkipIntro())
    {
        if (config->GetFullscreen())
            Screen->Cursor = crNone;
    }
    else
        MainForm->sfx->PlayMusic(Music_TitleTheme);
    if (!config->GetFullscreen())
    {
        Height = 500;
        Width = 640;
        if (ClientHeight < 473)
            window_height_padding = 473 - ClientHeight;
        if (config->GetScale() <= 1)
        {
            Height = Height + window_height_padding;
            if (Height > 512)
                Height = 512;
        }
        else
        {
            Width = 640 * config->GetScale();
            Height = 496 * config->GetScale();
        }
    }
    if (net_client->IsIpAddress(config->GetHost()))
        client->Address = config->GetHost();
    else
        client->Host = config->GetHost();
    if (config->GetBuildVersion() > 0)
        version_build = config->GetBuildVersion();
    if (start_fullscreen)
    {
        Width = Screen->Width;
        Height = Screen->Height;
    }
    if (config->GetDebug())
    {
        String debug_message = title + "DEBUGMODE, Erase config.ini file to fix";
        MessageDlg(debug_message, mtWarning, TMsgDlgButtons() << mbOK, 0);
    }
    client->Port = config->GetPort();
    Application->OnIdle = OnIdle;
    mm_timer_id = 0;
    mm_timer_id =
        timeSetEvent(12, 0,
            (void __stdcall (*)(unsigned int, unsigned int, unsigned long, unsigned long, unsigned long))MMTimerProc, 0,
            TIME_PERIODIC);
    Timer1->Enabled = true;
    randomize();
}

// Ticks the window menu's items to match the settings: the minimap mode, its icons and grid, the bars that drop by
// themselves and the rest.
void TMainForm::UpdateSystemMenu()
{
    if (config->GetElementalIcons())
        CheckMenuItem(auto_drop_menu, 177, MF_CHECKED);
    else
        CheckMenuItem(auto_drop_menu, 177, MF_UNCHECKED);
    if (config->GetExtendedTooltip())
        CheckMenuItem(auto_drop_menu, 178, MF_CHECKED);
    else
        CheckMenuItem(auto_drop_menu, 178, MF_UNCHECKED);
    if (config->GetMinimapIcons())
        CheckMenuItem(auto_drop_menu, 179, MF_CHECKED);
    else
        CheckMenuItem(auto_drop_menu, 179, MF_UNCHECKED);
    if (config->GetMinimapGrid())
        CheckMenuItem(auto_drop_menu, 180, MF_CHECKED);
    else
        CheckMenuItem(auto_drop_menu, 180, MF_UNCHECKED);
    if (config->GetWasdKeys())
        CheckMenuItem(auto_drop_menu, 181, MF_CHECKED);
    else
        CheckMenuItem(auto_drop_menu, 181, MF_UNCHECKED);
    if (!config->GetMinimapMode())
        CheckMenuRadioItem(auto_drop_menu, 174, 176, 174, MF_BYCOMMAND);
    if (config->GetMinimapMode() == 1)
        CheckMenuRadioItem(auto_drop_menu, 174, 176, 175, MF_BYCOMMAND);
    if (config->GetMinimapMode() == 2)
        CheckMenuRadioItem(auto_drop_menu, 174, 176, 176, MF_BYCOMMAND);
    if (config->GetScale() == 1)
        CheckMenuRadioItem(scale_menu, 170, 173, 170, MF_BYCOMMAND);
    if (config->GetScale() == 2)
        CheckMenuRadioItem(scale_menu, 170, 173, 171, MF_BYCOMMAND);
    if (config->GetScale() == 3)
        CheckMenuRadioItem(scale_menu, 170, 173, 172, MF_BYCOMMAND);
    if (config->GetScale() == 4)
        CheckMenuRadioItem(scale_menu, 170, 173, 173, MF_BYCOMMAND);
}

// The window menu's own items: the scale from 1 to 4, the status widget, the minimap mode, and toggles for the minimap
// icons and grid, the bars that drop by themselves and the elemental icons.
void __fastcall TMainForm::WMSysCommand(TWMSysCommand &message)
{
    if (message.CmdType == 173)
    {
        Width = 2560;
        Height = 1984;
        config->SetScale(4);
    }
    if (message.CmdType == 172)
    {
        Width = 1920;
        Height = 1488;
        config->SetScale(3);
    }
    if (message.CmdType == 171)
    {
        Width = 1280;
        Height = 992;
        config->SetScale(2);
    }
    if (message.CmdType == 170)
    {
        Width = 640;
        Height = window_height_padding + 500;
        if (Height > 512)
            Height = 512;
        config->SetScale(1);
    }
    // 174 to 176 pick the minimap mode; from 177 on, each item toggles a setting.
    if (message.CmdType == 174)
        config->SetMinimapMode(MinimapMode_Toggled);
    if (message.CmdType == 175)
        config->SetMinimapMode(MinimapMode_Overlay);
    if (message.CmdType == 176)
        config->SetMinimapMode(MinimapMode_Radar);
    if (message.CmdType == 177)
        config->SetElementalIcons(!config->GetElementalIcons());
    if (message.CmdType == 178)
        config->SetExtendedTooltip(!config->GetExtendedTooltip());
    if (message.CmdType == 179)
        config->SetMinimapIcons(!config->GetMinimapIcons());
    if (message.CmdType == 180)
        config->SetMinimapGrid(!config->GetMinimapGrid());
    if (message.CmdType == 181)
        config->SetWasdKeys(!config->GetWasdKeys());
    UpdateSystemMenu();
    TObject::Dispatch(&message);
}

// Logs out when in the game, closes the connection and the chat log, stops the sound and frees DirectX and the
// canvases.
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    if (animation->GetScreenMode() == ScreenMode_Intro)
        gfx->FreeIntroGraphics();
    if (client->Active)
        ((TAbstractSocket *)client)->Close();
    if (chat_log)
        chat_log->Close();
    if (mm_timer_id)
        timeKillEvent((unsigned int)mm_timer_id);
    if (keyboard_acquired)
    {
        Dx_InputDevice1->Unacquire();
        keyboard_acquired = 0;
    }
    if (Dx_Input1->Created)
        Dx_Input1->Destroy();
    if (Dx_Sound1->Created)
        Dx_Sound1->Destroy();
    if (Dx_Draw1->Created && config->GetFullscreen())
        Dx_Draw1->FlipToGDISurface();
    if (Dx_Draw1->Created)
        Dx_Draw1->Destroy();
    delete chat_log;
    delete draw_canvas;
    delete status_canvas;
    delete text_canvas;
    delete resource_bitmap;
    delete interface_bitmap;
}

// Logs out when in game, then closes the connection, the chat log, the fast timer and DirectX.
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (animation->GetScreenMode() == ScreenMode_Game)
        animation->interface_controller->Logout();
    if (client->Active)
        ((TAbstractSocket *)client)->Close();
    if (chat_log)
        chat_log->Close();
    if (mm_timer_id)
        timeKillEvent((unsigned int)mm_timer_id);
    if (Dx_Input1->Created)
        Dx_Input1->Destroy();
    if (Dx_Sound1->Created)
        Dx_Sound1->Destroy();
    if (Dx_Draw1->Created && config->GetFullscreen())
        Dx_Draw1->FlipToGDISurface();
    if (Dx_Draw1->Created)
        Dx_Draw1->Destroy();
}

__fastcall TMainForm::~TMainForm()
{
}

// Counts a failed draw; three before Timer1Timer next resets the count switch drawing to BitBlt in the settings.
void TMainForm::CountDrawError()
{
    draw_error_count++;
    if (draw_error_count > 2 && !config->GetUseBitBltDraw())
        config->SetUseBitBltDraw(true);
}

// Alt+F4: on the menu screens it frees everything and quits; in game it asks first.
void __fastcall TMainForm::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Shift.Contains(ssAlt) && Key == VK_F4)
    {
        if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
        {
            Key = 0;
            if (mm_timer_id)
                timeKillEvent((unsigned int)mm_timer_id);
            if (client->Active)
                ((TAbstractSocket *)client)->Close();
            edits->Clear();
            players->Clear();
            effects->Clear();
            map_items->ClearKeepingProtection();
            gfx->objects->ReleaseGraphics();
            gfx->blends->ReleaseGraphics();
            gfx->masks->ReleaseGraphics();
            gfx->walls->ReleaseGraphics();
            gfx->tiles->ReleaseGraphics();
            gfx->items->ReleaseGraphics();
            gfx->items->ReleaseSubItemGraphics();
            gfx->FreeMenuGraphics();
            gfx->FreeGameGraphics();
            gfx->FreeHoverLabel();
            if (Dx_Sound1->Created)
                Dx_Sound1->Destroy();
            if (Dx_Input1->Created)
                Dx_Input1->Destroy();
            if (Dx_Draw1->Created && config->GetFullscreen())
                Dx_Draw1->FlipToGDISurface();
            if (Dx_Draw1->Created)
                Dx_Draw1->Destroy();
            delete animation;
            delete gfx;
            Application->Terminate();
        }
        if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
        {
            Key = 0;
            MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_Logout,
                MainForm->strings->GetDialogText(DialogText_ExitGame, false),
                MainForm->strings->GetDialogText(DialogText_ExitGame, true));
            MainForm->edits->caret_enabled = false;
        }
    }
}

bool TMainForm::isActive()
{
    if (!active)
        return false;
    if (minimized)
        return false;
    return true;
}

void TMainForm::PaintFullDelayed()
{
    if (animation->GetScreenMode() == ScreenMode_Game)
    {
        flip_needed = true;
        redraw_needed = true;
        animation->interface_controller->RequestFullRedraw(1);
        animation->interface_controller->Invalidate();
        animation->interface_controller->InvalidateActivePanel();
    }
}

void __fastcall TMainForm::ActivateForm(TObject *sender)
{
    if (Dx_InputDevice1->Created)
        Dx_InputDevice1->Destroy();
    Dx_InputDevice1->Create(GUID_SysKeyboard, Dx_Input1);
    if (keyboard->WasDeletePressed() || config->GetForceBuffer())
        full_paint_delay = 2;
    active = true;
}

void __fastcall TMainForm::DeactivateForm(TObject *sender)
{
    if (Dx_InputDevice1->Created)
        Dx_InputDevice1->Destroy();
    active = false;
}

void __fastcall TMainForm::ApplicationEvents1Activate(TObject *Sender)
{
    active = true;
}

void __fastcall TMainForm::ApplicationEvents1Deactivate(TObject *Sender)
{
    active = false;
}

void __fastcall TMainForm::MinimizeForm(TObject *sender)
{
    minimized = true;
}

void __fastcall TMainForm::RestoreForm(TObject *sender)
{
    minimized = false;
}

String TMainForm::VersionString(int major, int minor, int build, char letter)
{
    String result = String(major) + "." + String(minor) + "." + String(build);
    if (letter != '0')
        result = result + letter;
    return result;
}

// Makes the surfaces: in a window with a clipper on it, or full screen in the display mode with a back buffer.
void __fastcall TMainForm::Dx_Draw1Create(TObject *Sender)
{
    if (PrimarySurface->Created)
        PrimarySurface->Destroy();
    if (config->GetFullscreen())
    {
        try
        {
            Dx_Draw1->EnumDisplayModes(0, 0, 0);
            Dx_Draw1->SetCooperativeLevel(Application->Handle, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
            PrimarySurfaceDesc->Flags = DDSD_BACKBUFFERCOUNT;
            PrimarySurfaceDesc->SCaps = PrimaryCaps;
            BackBufferDesc->SCaps = BackBufferCaps;
            PrimaryCaps->Caps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE;
            PrimarySurfaceDesc->Width = Width;
            PrimarySurfaceDesc->Height = Height;
            PrimarySurfaceDesc->BackBufferCount = 1;
            PrimarySurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            Dx_Draw1->SetDisplayMode(display_width, display_height, display_bit_depth, 0, 0);
            OffscreenDesc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
            OffscreenDesc->SCaps = OffscreenCaps;
            OffscreenDesc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
            OffscreenDesc->Width = Width;
            OffscreenDesc->Height = Height;
            OffscreenSurface->GetSurfaceDesc(OffscreenDesc);
            OffscreenSurface->Create(OffscreenDesc, Dx_Draw1);
            return;
        }
        catch (...)
        {
        }
    }
    if (!config->GetFullscreen())
    {
        try
        {
            Dx_Draw1->SetCooperativeLevel(Handle, DDSCL_NORMAL);
            PrimarySurfaceDesc->Flags = 0;
            PrimarySurfaceDesc->SCaps = PrimaryCaps;
            PrimarySurfaceDesc->SCaps->Caps = DDSCAPS_PRIMARYSURFACE;
            PrimarySurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            PrimarySurfaceDesc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
            PrimarySurfaceDesc->SCaps = PrimaryCaps;
            PrimarySurfaceDesc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN;
            PrimarySurfaceDesc->Width = Width;
            PrimarySurfaceDesc->Height = Height;
            OffscreenSurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            PrimaryClipper->Create(Dx_Draw1);
            PrimaryClipper->SetHWnd(Handle);
            PrimarySurface->SetClipper(PrimaryClipper);
            return;
        }
        catch (...)
        {
        }
    }
}

// Full screen, attaches the back buffer to the new primary surface.
void __fastcall TMainForm::PrimarySurfaceCreate(TObject *Sender)
{
    if (config->GetFullscreen() && PrimarySurface->GetAttachedSurface(BackBufferCaps, BackBuffer))
        MainForm->BorderStyle = bsNone;
}

// Counts the display modes, and keeps the refresh rate of the 16-bit mode at the window size.
void __fastcall TMainForm::Dx_Draw1EnumDisplayModes(System::TObject *Sender, TDDSurfaceDesc *SurfaceDesc, void *Context,
    bool &Finished)
{
    int mode_width;
    int mode_height;
    int refreshed;
    int bit_depth;
    if (config->GetFullscreen())
    {
        int mode_width = SurfaceDesc->Width;
        int mode_height = SurfaceDesc->Height;
        int refreshed = SurfaceDesc->RefreshRate;
        if ((SurfaceDesc->PixelFormat->Flags[0] & 0x800) != 0)
            bit_depth = 1;
        else
        {
            if ((SurfaceDesc->PixelFormat->Flags[0] & 0x1000) != 0)
                bit_depth = 2;
            else
            {
                if ((SurfaceDesc->PixelFormat->Flags[0] & 8) != 0)
                    bit_depth = 4;
                else
                {
                    if ((SurfaceDesc->PixelFormat->Flags[0] & 0x20) != 0)
                        bit_depth = 8;
                    else
                    {
                        if ((SurfaceDesc->PixelFormat->Flags[0] & 0x10) != 0)
                            bit_depth = 8;
                        else
                        {
                            if ((SurfaceDesc->PixelFormat->Flags[0] & 2) != 0)
                                bit_depth = SurfaceDesc->PixelFormat->AlphaBitDepth[0];
                            else if ((SurfaceDesc->PixelFormat->Flags[0] & 1) != 0)
                                bit_depth = SurfaceDesc->PixelFormat->AlphaBitDepth[0];
                            else if ((SurfaceDesc->PixelFormat->Flags[0] & 0x8000) != 0)
                                bit_depth = SurfaceDesc->PixelFormat->AlphaBitDepth[0];
                            else
                            {
                                if ((SurfaceDesc->PixelFormat->Flags[0] & 0x80000) != 0)
                                    bit_depth = SurfaceDesc->PixelFormat->BumpBitCount[0];
                                else if ((SurfaceDesc->PixelFormat->Flags[0] & 0x40000) != 0)
                                    bit_depth = SurfaceDesc->PixelFormat->BumpBitCount[0];
                                else
                                {
                                    if ((SurfaceDesc->PixelFormat->Flags[0] & 0x40) != 0)
                                        bit_depth = SurfaceDesc->PixelFormat->RGBBitCount[0];
                                    else
                                    {
                                        if ((SurfaceDesc->PixelFormat->Flags[0] & 0x200) != 0)
                                            bit_depth = SurfaceDesc->PixelFormat->YUVBitCount[0];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (display_mode_count < 50)
        {
            if (display_width == mode_width && display_height == mode_height && bit_depth > display_bit_depth)
            {
                display_bit_depth = bit_depth;
                refresh_rate = refreshed;
            }
            display_mode_count++;
        }
        else
            Finished = true;
    }
}

// Frees the surfaces.
void __fastcall TMainForm::Dx_Draw1Destroy(TObject *Sender)
{
    OffscreenSurface->Destroy();
    if (BackBuffer->Created)
        BackBuffer->Destroy();
    PrimarySurface->Destroy();
}

void __fastcall TMainForm::PrimarySurfaceSurfaceLost(TObject *Sender)
{
}

void __stdcall MMTimerProc(UINT timer_id, UINT message, DWORD_PTR user_data, DWORD_PTR first_reserved,
    DWORD_PTR second_reserved)
{
    MainForm->HandleMMTimerEvent();
}

// Loads pending graphics in turns, plays queued music, drops distant players, items and npcs, and purges graphics not
// used lately. The tick count is kept in display_width: the turns follow its remainder by four, the music and the
// screenshot key come every twelfth tick, the drops every twe
void __fastcall TMainForm::Timer1Timer(TObject *Sender)
{
    // Graphics load on three ticks of four; the music is checked every 12th, and players gone
    // from view are dropped every 20th.
    if (display_width % 4 == 1)
    {
        players->LoadPendingGraphics();
        if (!config->GetDebug())
        {
            npcs->LoadPendingGraphics();
            gfx->tiles->LoadPendingGraphic();
            gfx->tops->LoadPendingGraphic();
            gfx->blends->LoadPendingGraphic();
            gfx->items->LoadPendingGraphics();
        }
        if (login_screen_delay > 0)
        {
            login_screen_delay--;
            if (!login_screen_delay)
                mouse->OpenLoginScreen();
        }
        if (full_paint_delay > 0)
        {
            full_paint_delay--;
            if (!full_paint_delay)
                PaintFullDelayed();
        }
    }
    if (display_width % 4 == 2 && !config->GetDebug())
    {
        gfx->walls->LoadPendingGraphic();
        gfx->objects->LoadPendingGraphic();
        gfx->tiles->LoadPendingGraphic();
        gfx->tops->LoadPendingGraphic();
        gfx->blends->LoadPendingGraphic();
        gfx->effects->LoadPendingGraphic();
        gfx->items->LoadPendingGraphics();
    }
    if (display_width % 4 == 3 && !config->GetDebug())
    {
        gfx->masks->LoadPendingGraphic();
        gfx->objects->LoadPendingGraphic();
        gfx->walls->LoadPendingGraphic();
        gfx->tiles->LoadPendingGraphic();
        gfx->items->LoadPendingGraphics();
        gfx->icons->LoadPendingGraphic();
        gfx->effects->LoadPendingGraphic();
    }
    if (display_width % SLOW_UPDATE_INTERVAL == 0)
    {
        players->RemoveExpiredPlayers(map->GetCenterX(), map->GetCenterY());
        map_items->RemoveDistantItems(map->GetCenterX(), map->GetCenterY());
        npcs->RemoveExpiredNpcs();
    }
    if (display_width > 40)
    {
        draw_error_count = 0;
        gfx->objects->PurgeUnusedGraphics();
        gfx->masks->PurgeUnusedGraphics();
        gfx->walls->PurgeUnusedGraphics();
        gfx->tiles->PurgeUnusedGraphics();
        gfx->tops->PurgeUnusedGraphics();
        gfx->blends->PurgeUnusedGraphics();
        gfx->items->PurgeUnusedGraphics();
        gfx->icons->PurgeUnusedGraphics();
        gfx->effects->PurgeUnusedGraphics();
        display_width = 0;
    }
    display_width++;
}

// The fast timer's tick: the keyboard on every tick, then animation, controllers and sound on their own periods.
void __fastcall TMainForm::HandleMMTimerEvent()
{
    timer_ticks++;
    keyboard->PollKeyboard();
    // The frame rate is the frames drawn over the last two hundred ticks.
    // The frame rate is the frames drawn over the last hundred ticks.
    if (timer_ticks >= 100)
    {
        frame_rate = frame_count;
        timer_ticks = 0;
        frame_count = 0;
    }
    // Every tenth tick the timers; every twentieth the animations, the path, the mouse and the windows.
    if (timer_ticks % 5 == 0)
        animation->UpdateTimers();
    if (timer_ticks % 10 == 0)
    {
        animation->interface_controller->UpdateScreenEffects();
        map->path_finder->FollowPath();
        players->Animate();
        effects->Animate();
        npcs->Animate();
        map_items->UpdateHoverLabel();
        map->Animate();
        mouse->HandleMove();
        if (animation->jukebox)
            animation->jukebox->Tick();
        if (animation->musician)
            animation->musician->Tick();
        if (animation->menu)
            animation->menu->OnTimer();
        if (animation->interface_controller->chat)
            animation->interface_controller->chat->OnTimer();
        if (animation->interface_controller->spells)
            animation->interface_controller->spells->Tick();
        if (animation->interface_controller->skills)
            animation->interface_controller->skills->Tick();
        if (animation->interface_controller->status)
            animation->interface_controller->status->Tick();
        if (animation->social)
            animation->social->OnTimer();
        if (animation->interface_controller->player_list)
            animation->interface_controller->player_list->Tick();
        if (animation->interface_controller->group)
            animation->interface_controller->group->Tick();
        if (animation->trade)
            animation->trade->Tick();
        if (animation->guild)
            animation->guild->Tick();
        if (animation->dialog)
            animation->dialog->Tick();
        if (animation->memo)
            animation->memo->Tick();
        if (animation->report)
            animation->report->Tick();
        if (animation->law)
            animation->law->Tick();
        if (animation->inn)
            animation->inn->Tick();
        if (animation->learn)
            animation->learn->Tick();
        if (animation->shop)
            animation->shop->Tick();
        if (animation->message_board)
            animation->message_board->Tick();
        if (animation->locker)
            animation->locker->Tick();
        if (animation->game)
            animation->game->Tick();
        if (animation->interface_controller->news)
            animation->interface_controller->news->Tick();
        if (animation->interface_controller->help)
            animation->interface_controller->help->Tick();
        if (animation->interface_controller->inventory)
            animation->interface_controller->inventory->Tick();
        if (animation->interface_controller->game_bars)
            animation->interface_controller->game_bars->Tick();
        if (animation->intro->is_animating)
            IntroAnimator();
        if (animation->popup)
        {
            animation->popup->Tick();
            animation->popup->Render();
        }
        if (animation->boss_bars)
        {
            animation->boss_bars->Update();
            animation->boss_bars->RenderNames();
        }
    }
    if (timer_ticks % 10 == 1)
        animation_tick_due = true;
    if (timer_ticks % SLOW_UPDATE_INTERVAL == 0)
    {
        sfx->UpdateSounds();
        if (animation->IsAnimating())
            animation->UpdateIntroAndDelays();
        effects->RemoveFinished();
    }
    if (timer_ticks % 50 == 0)
    {
        edits->TimerTick();
        animation->interface_controller->OnTimer();
        map->AdvanceTileFrame();
    }
}

// Sets up the keyboard device and acquires it.
void __fastcall TMainForm::Dx_InputDevice1Create(TObject *Sender)
{
    Dx_InputDevice1->SetDataFormat((TDIData *)DIKeyboardState1);
    Dx_InputDevice1->SetCooperativeLevel(Application->Handle, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
    keyboard_acquired = Dx_InputDevice1->Acquire();
    if (keyboard_acquired)
        Dx_InputDevice1->Poll();
}

void __fastcall TMainForm::Dx_InputDevice1Destroy(TObject *Sender)
{
    if (keyboard_acquired)
        Dx_InputDevice1->Unacquire();
}

void __fastcall TMainForm::Dx_Input1Destroy(TObject *Sender)
{
    if (Dx_InputDevice1->Created)
        Dx_InputDevice1->Destroy();
}

void __fastcall TMainForm::Dx_Sound1Create(TObject *Sender)
{
    Dx_Sound1->SetCooperativeLevel(Application->Handle, DSSCL_PRIORITY);
}

// Frees the sound buffers before the sound device goes.
void __fastcall TMainForm::Dx_Sound1Destroy(TObject *Sender)
{
    if (Dx_SoundBuffer7->Created)
        Dx_SoundBuffer4->Destroy();
    if (Dx_SoundBuffer6->Created)
        Dx_SoundBuffer4->Destroy();
    if (Dx_SoundBuffer5->Created)
        Dx_SoundBuffer4->Destroy();
    if (Dx_SoundBuffer4->Created)
        Dx_SoundBuffer4->Destroy();
    if (Dx_SoundBuffer3->Created)
        Dx_SoundBuffer3->Destroy();
    if (Dx_SoundBuffer2->Created)
        Dx_SoundBuffer2->Destroy();
    if (Dx_SoundBuffer1->Created)
        Dx_SoundBuffer1->Destroy();
}

// Lists each sound driver: its name, module and GUID.
void __fastcall TMainForm::Dx_Sound1DSEnumerate(TObject *Sender, _GUID *Guid, String DriverDescription,
    String DriverModule, void *Context, bool &Finished)
{
    if (sound_driver_names && sound_driver_modules && sound_driver_guids)
    {
        sound_driver_names->Add(DriverDescription);
        sound_driver_modules->Add(DriverModule);
        sound_driver_guids->Add(Guid);
    }
    else
        Finished = true;
}

void TMainForm::IntroAnimator()
{
}

// Scales the cursor in a window to game coordinates and hands it to the mouse.
void __fastcall TMainForm::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    int mouse_x = X;
    if (!config->GetFullscreen())
    {
        double scale_x = ClientWidth / BASE_SCREEN_WIDTH;
        double scale_y = ClientHeight / BASE_SCREEN_HEIGHT;
        double scaled_x = mouse_x / scale_x;
        double scaled_y = Y / scale_y;
        mouse_x = scaled_x;
        Y = scaled_y;
    }
    mouse->SetPosition(mouse_x, Y);
}

// Scales a click in a window to game coordinates, focuses the edit box under it, then hands the click to the mouse.
void __fastcall TMainForm::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!config->GetFullscreen())
    {
        double scale_x = ClientWidth / BASE_SCREEN_WIDTH;
        double scale_y = ClientHeight / BASE_SCREEN_HEIGHT;
        double scaled_x = X / scale_x;
        double scaled_y = Y / scale_y;
        X = scaled_x;
        Y = scaled_y;
    }
    if (edits->GetCount() >= 1 && edits->caret_enabled)
    {
        edits->MoveFirst();
        do
        {
            Edit handle = edits->GetCurrentEdit();
            if (X >= handle.x && X <= handle.x + handle.width && Y >= handle.y && Y <= handle.y + handle.height)
            {
                edits->FocusClickedEdit(handle);
                sfx->PlaySfx(SfxSound_Select);
                break;
            }
            edits->MoveNext();
        } while (!edits->IsAtEnd());
    }
    if (Button == mbLeft)
        mouse->HandleMouseDown(X, Y, MouseButton_Left);
    else
        mouse->HandleMouseDown(X, Y, MouseButton_Right);
}

void __fastcall TMainForm::FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    mouse->HandleWheel(WheelDirection_Down);
}

void __fastcall TMainForm::FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled)
{
    mouse->HandleWheel(WheelDirection_Up);
}

void __fastcall TMainForm::FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    mouse->HandleMouseUp();
}

// Draws the game view: the ground, the shadows and the map's rows with their people and effects, then the own
// character, the screen tints, the minimap, the status icons and every open window and box.
void __fastcall TMainForm::OnIdle(TObject *sender, bool &done)
{
    int j;
    int m;
    int n;
    int kk;
    int i;
    int k;
    int ii;
    int jj;
    int object_spec;
    HDC timed_text_dc;
    if (!PrimarySurface->Created)
        return;
    if (!OffscreenSurface->Created)
        return;
    if (sfx->TakeMusicPending())
        sfx->PlayMusic(sfx->GetQueuedMusic());
    if (!sfx->IsMusicPlaying())
    {
        if (sfx->IsMusicRetryReady())
        {
            sfx->StartMusicRetryDelay();
            if (map->GetMusicMode() > 2)
            {
                if (map->GetMusicMode() < 6)
                    sfx->QueueMusic(map->GetMusicId());
            }
        }
    }
    // Print Screen saves a screenshot.
    if (GetAsyncKeyState(VK_SNAPSHOT) != 0)
        gfx->interface_graphics->SaveScreenshot();
    if (animation->GetScreenMode() == ScreenMode_Game)
    {
        if (animation->interface_controller->IsFullRedrawPending())
        {
            flip_needed = true;
            redraw_needed = true;
            OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_background,
                &gfx->interface_graphics->hud_background_rect, DDBLTFAST_WAIT);
            animation->interface_controller->ClearFullRedraw();
        }
        if (animation_tick_due)
        {
            animation_tick_due = false;
            // While warping or loading, only the frame, the bars and the loading text are drawn.
            if (map->warp_pending || map->loading_map || animation->interface_controller->timed_text_visible)
            {
                OffscreenSurface->Blt(&gfx->game->view_clip_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT,
                    gfx->game->view_fill_fx);
                OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_frame,
                    &gfx->interface_graphics->hud_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(7, 53, gfx->interface_graphics->hud_left_frame,
                    &gfx->interface_graphics->hud_left_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(49, 7, gfx->interface_graphics->hud_top_frame,
                    &gfx->interface_graphics->hud_top_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(55, 0, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->status_icon_left_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(77, 0, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->status_icon_right_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(101, 0, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->status_bars_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(125, 4, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->GetStatusBarFillRect(0,
                        animation->interface_controller->game_bars->GetHealthBarSegments()),
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(234, 4, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->GetStatusBarFillRect(1,
                        animation->interface_controller->game_bars->GetManaBarSegments()),
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(345, 4, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->GetStatusBarFillRect(2,
                        animation->interface_controller->game_bars->GetStaminaBarSegments()),
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                OffscreenSurface->BltFast(454, 4, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->GetStatusBarFillRect(3,
                        animation->interface_controller->game_bars->GetExperienceBarSegments()),
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                animation->interface_controller->game_bars->MarkDirty();
                gfx->interface_graphics->DrawCornerButton();
                if (animation->interface_controller->timed_text_visible)
                {
                    if (OffscreenSurface->GetDC(&timed_text_dc))
                    {
                        try
                        {
                            status_canvas->Handle = timed_text_dc;
                            status_canvas->Brush->Style = bsClear;
                            status_canvas->Font->Name = "Arial";
                            status_canvas->Font->Color = clWhite;
                            status_canvas->Font->Size = 9;
                            int timed_text_width =
                                status_canvas->TextWidth(animation->interface_controller->timed_text);
                            status_canvas->TextOut(320 - timed_text_width / 2, 156,
                                animation->interface_controller->timed_text);
                            status_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(timed_text_dc);
                    }
                }
                // While the map downloads, its size shows in kilobytes.
                if (map->loading_map)
                {
                    OffscreenSurface->BltFast(230, 138, gfx->interface_graphics->loading_icon,
                        &gfx->interface_graphics->loading_icon_rect, DDBLTFAST_WAIT);
                    HDC loading_dc;
                    if (OffscreenSurface->GetDC(&loading_dc))
                    {
                        try
                        {
                            status_canvas->Handle = loading_dc;
                            status_canvas->Brush->Style = bsClear;
                            status_canvas->Font->Name = "Arial";
                            status_canvas->Font->Color = clWhite;
                            status_canvas->Font->Size = 7;
                            status_canvas->TextOut(280, 146,
                                "Loading world data [" + IntToStr(map->map_file_size / 1024 + 1) + " Kb]");
                            status_canvas->TextOut(310, 164, "Please wait");
                            status_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(loading_dc);
                    }
                }
                // An accepted warp asks for the new map with the warp session.
                if (map->map_request_pending)
                {
                    map->map_request_pending = false;
                    String map_request = MainForm->net_client->EncodeNumber(map->map_id, 2);
                    map_request = map_request + MainForm->net_client->EncodeNumber(map->warp_session_id, 2);
                    net_client->SendPacket(AC_GET, OP_MAP, map_request);
                }
                flip_needed = true;
            }
            else if (map->ConsumeCursorMoved() || npcs->ConsumeRedraw() || players->ConsumeRedraw() || redraw_needed ||
                edits->ConsumeTextChanged())
            {
                if (!map->warp_pending)
                {
                    redraw_needed = false;
                    int player_index = 0;
                    int npc_index = 0;
                    int effect_index = 0;
                    int item_index = 0;
                    int own_player_index = -1;
                    char own_has_weapon = false;
                    char own_has_shield = false;
                    int shake_offset = 0;
                    // A screen shake moves everything drawn by its current offset.
                    if (animation->interface_controller->is_shaking)
                        shake_offset = animation->interface_controller->shake_offset;
                    OffscreenSurface->Blt(&gfx->game->view_clip_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT,
                        gfx->game->view_fill_fx);
                    // The tiles around the centre; the rows run on to the bottom of the map.
                    int first_x = map->GetCenterX() - 28;
                    int last_x = map->GetCenterX() + 28;
                    int first_y = map->GetCenterY() - 28;
                    int last_y = map->GetHeight();
                    if (first_x < 0)
                        first_x = 0;
                    if (first_y < 0)
                        first_y = 0;
                    if (map->GetWidth() < last_x)
                        last_x = map->GetWidth();
                    for (i = first_y; i <= last_y; i++)
                    {
                        for (j = first_x; j <= last_x; j++)
                        {
                            int ground_x = 32 * j - 32 * i + map->GetScrollOffsetX() + 278;
                            int ground_y = map->GetScrollOffsetY() + (16 * j + 16 * i) + 134;
                            if (shake_offset)
                                ground_x += shake_offset;
                            if (map->ground_tiles[j][i] >= 1)
                            {
                                if (net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), j, i))
                                {
                                    if (gfx->tiles->RequestGraphic(map->ground_tiles[j][i]))
                                    {
                                        gfx->game->BlitClipped(ground_x, ground_y,
                                            gfx->tiles->GetGraphic(map->ground_tiles[j][i]).resource->surface,
                                            gfx->tiles->GetGraphic(map->ground_tiles[j][i]).resource->GetFrameRect(map->tile_frame));
                                        if (gfx->tiles->GetGraphic(map->ground_tiles[j][i]).resource->is_animated)
                                            MainForm->map->has_animated_tiles = true;
                                    }
                                    else
                                        redraw_needed = true;
                                }
                            }
                        }
                    }
                    // Shadows, when set, go over the ground and under everything else.
                    if (config->GetShowShadows())
                    {
                        for (k = first_y; k <= last_y; k++)
                        {
                            for (m = first_x; m <= last_x; m++)
                            {
                                int shadow_x = 32 * m - 32 * k + map->GetScrollOffsetX() + 278;
                                int shadow_y = 16 * m + 16 * k + map->GetScrollOffsetY() + 134;
                                if (shake_offset)
                                    shadow_x += shake_offset;
                                if (map->blend_tiles[m][k] >= 1)
                                {
                                    if (net_client->IsObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), m, k))
                                    {
                                        if (gfx->blends->RequestGraphic(map->blend_tiles[m][k]))
                                            gfx->game->BlitClippedAlpha(shadow_x - 24, shadow_y - 12,
                                                gfx->blends->GetGraphic(map->blend_tiles[m][k]).resource->surface,
                                                gfx->blends->GetGraphic(map->blend_tiles[m][k]).resource->surface_rect,
                                                48);
                                    }
                                }
                            }
                        }
                    }
                    for (n = first_y - 1; n <= last_y; n++)
                    {
                        for (ii = first_x; ii <= last_x; ii++)
                        {
                            int screen_x = 32 * ii - 32 * n + map->GetScrollOffsetX() + 278;
                            int screen_y = map->GetScrollOffsetY() + (16 * ii + 16 * n) + 134;
                            if (shake_offset)
                                screen_x += shake_offset;
                            if (n >= 0)
                            {
                                if (map->HasPathMarker())
                                {
                                    if (ii == map->GetPathMarkerX())
                                    {
                                        if (n == map->GetPathMarkerY())
                                        {
                                            if (map->GetPathMarkerTimer() > 4)
                                                gfx->game->BlitClippedAlpha(screen_x, screen_y,
                                                    gfx->interface_graphics->tile_cursor,
                                                    gfx->interface_graphics->GetWideCellRect(3),
                                                    24 * map->GetPathMarkerTimer() + 12);
                                            else
                                                gfx->game->BlitClippedAlpha(screen_x, screen_y,
                                                    gfx->interface_graphics->tile_cursor,
                                                    gfx->interface_graphics->GetWideCellRect(4),
                                                    24 * map->GetPathMarkerTimer() + 12);
                                        }
                                    }
                                }
                                if (ii == map->GetCursorX())
                                {
                                    if (n == map->GetCursorY())
                                    {
                                        if (!map->blocked[ii][n])
                                        {
                                            if (map->spec_grid[ii][n] != 18)
                                            {
                                                if (!animation->game->player_menu_visible ||
                                                    animation->game->player_menu_hovered_option < 0)
                                                {
                                                    if (MainForm->map_items->HasItemAt(ii, n))
                                                        gfx->game->BlitClipped(screen_x, screen_y,
                                                            gfx->interface_graphics->tile_cursor,
                                                            gfx->interface_graphics->GetWideCellRect(2));
                                                    else if (map->has_tile_spec[ii][n] ||
                                                        players->HasVisiblePlayerAt(ii, n) || npcs->HasNpcAt(ii, n))
                                                        gfx->game->BlitClipped(screen_x, screen_y,
                                                            gfx->interface_graphics->tile_cursor,
                                                            gfx->interface_graphics->GetWideCellRect(1));
                                                    else
                                                        gfx->game->BlitClipped(screen_x, screen_y,
                                                            gfx->interface_graphics->tile_cursor,
                                                            gfx->interface_graphics->GetWideCellRect(0));
                                                }
                                            }
                                        }
                                    }
                                }
                                if ((int)MainForm->map_items->GetCount() > item_index)
                                {
                                    char item_found;
                                    do
                                    {
                                        item_found = false;
                                        MapItem map_item = map_items->GetItem(item_index);
                                        if (ii == map_item.x)
                                        {
                                            if (n == map_item.y)
                                            {
                                                int item_graphic = item_values->GetGraphic(map_item.item_id) - 1;
                                                if (item_graphic >= 0)
                                                {
                                                    if (map_item.item_id == ItemId_Gold)
                                                    {
                                                        item_graphic = 134;
                                                        if (map_item.amount > 1)
                                                            item_graphic = 135;
                                                        if (map_item.amount > 99)
                                                            item_graphic = 136;
                                                        if (map_item.amount > 9999)
                                                            item_graphic = 137;
                                                        if (map_item.amount > 99999)
                                                            item_graphic = 138;
                                                    }
                                                    if (net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(),
                                                        ii, n))
                                                    {
                                                        if (gfx->items->RequestGraphic(item_graphic))
                                                        {
                                                            GraphicSize item_size =
                                                                gfx->items->GetGraphicSize(item_graphic);
                                                            int item_x = screen_x + 32 - item_size.width / 2;
                                                            int item_y = screen_y + 16 - item_size.height / 2;
                                                            gfx->game->BlitClipped(item_x, item_y,
                                                                gfx->items->GetGraphic(item_graphic).resource->surface,
                                                                gfx->items->GetGraphic(item_graphic).resource->surface_rect);
                                                        }
                                                        else
                                                            redraw_needed = true;
                                                    }
                                                }
                                                item_found = true;
                                                item_index++;
                                            }
                                        }
                                    } while ((int)MainForm->map_items->GetCount() > item_index && item_found);
                                }
                                if (map->overlay_tiles[ii][n] >= 1)
                                {
                                    if (net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                                    {
                                        if (gfx->tiles->RequestGraphic(map->overlay_tiles[ii][n]))
                                            gfx->game->BlitClipped(screen_x, screen_y - 32,
                                                gfx->tiles->GetGraphic(map->overlay_tiles[ii][n]).resource->surface,
                                                gfx->tiles->GetGraphic(map->overlay_tiles[ii][n]).resource->surface_rect);
                                    }
                                }
                                if (map->objects[ii][n] >= 1)
                                {
                                    if (net_client->IsTallObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), ii,
                                        n))
                                    {
                                        char draw_object = true;
                                        object_spec = map->GetTileSpec(ii, n);
                                        if (object_spec == TileSpec_Trap && map->trap_reveal_timer < 1)
                                            draw_object = false;
                                        if (object_spec == TileSpec_HiddenSpikes)
                                        {
                                            if (players->FindPlayerAt(ii, n) < PlayerId_Unset)
                                                draw_object = false;
                                        }
                                        if (draw_object)
                                        {
                                            if (gfx->objects->RequestGraphic(map->objects[ii][n]))
                                            {
                                                GraphicSize object_size =
                                                    gfx->objects->GetGraphicSize(map->objects[ii][n]);
                                                int object_x = screen_x + 30 - object_size.width / 2;
                                                int object_y = screen_y + 30 - object_size.height;
                                                gfx->game->BlitClipped(object_x, object_y,
                                                    gfx->objects->GetGraphic(map->objects[ii][n]).resource->surface,
                                                    gfx->objects->GetGraphic(map->objects[ii][n]).resource->surface_rect);
                                            }
                                            else
                                                redraw_needed = true;
                                        }
                                    }
                                }
                                // The lists are sorted by position, so each one is walked in step with the tiles.
                                if ((int)MainForm->players->GetCount() > player_index)
                                {
                                    char player_found;
                                    do
                                    {
                                        player_found = false;
                                        Player player = MainForm->players->GetPlayer(player_index);
                                        if (net_client->GetPlayerId() == player.player_id)
                                        {
                                            own_player_index = player_index;
                                            if (player.weapon > 0)
                                                own_has_weapon = true;
                                            if (player.shield > ShieldGraphic_None)
                                                own_has_shield = true;
                                        }
                                        if (ii == player.x)
                                        {
                                            if (n == player.y)
                                            {
                                                // A hidden player is drawn only for themselves and for admin levels above 3.
                                                if (!player.hidden || net_client->GetPlayerId() == player.player_id ||
                                                    MainForm->net_client->GetAdminLevel() > HIDDEN_VIEW_ADMIN_LEVEL)
                                                {
                                                    if (player.effect_running)
                                                    {
                                                        if (player.effect_base_layer)
                                                        {
                                                            if (gfx->effects->RequestGraphic(player.effect_id))
                                                            {
                                                                GraphicSize behind_effect_size =
                                                                    gfx->effects->GetGraphicSize(player.effect_id);
                                                                int behind_effect_x;
                                                                int effect_frame;
                                                                behind_effect_x =
                                                                    32 * player.x - 32 * player.y +
                                                                        MainForm->map->GetScrollOffsetX() + 309;
                                                                int behind_effect_y =
                                                                    16 * player.x + 16 * player.y +
                                                                        MainForm->map->GetScrollOffsetY() + 120;
                                                                behind_effect_x -= behind_effect_size.width / 2;
                                                                behind_effect_y -= behind_effect_size.height / 2;
                                                                if (player.walking)
                                                                {
                                                                    if (player.walk_direction == FacingDirection_Left)
                                                                    {
                                                                        behind_effect_x =
                                                                            behind_effect_x + 32 -
                                                                                (8 * player.walk_frame + 8);
                                                                        behind_effect_y =
                                                                            behind_effect_y + 16 -
                                                                                (4 * player.walk_frame + 4);
                                                                    }
                                                                    if (player.walk_direction == FacingDirection_Up)
                                                                    {
                                                                        behind_effect_x =
                                                                            8 * player.walk_frame + behind_effect_x -
                                                                                24;
                                                                        behind_effect_y =
                                                                            behind_effect_y + 16 -
                                                                                (4 * player.walk_frame + 4);
                                                                    }
                                                                    if (player.walk_direction == FacingDirection_Right)
                                                                    {
                                                                        behind_effect_x =
                                                                            8 * player.walk_frame + behind_effect_x -
                                                                                24;
                                                                        behind_effect_y =
                                                                            4 * player.walk_frame + behind_effect_y -
                                                                                12;
                                                                    }
                                                                    if (player.walk_direction == FacingDirection_Down)
                                                                    {
                                                                        behind_effect_x =
                                                                            behind_effect_x + 32 -
                                                                                (8 * player.walk_frame + 8);
                                                                        behind_effect_y =
                                                                            4 * player.walk_frame + behind_effect_y -
                                                                                12;
                                                                    }
                                                                }
                                                                if (player.effect_id == SpellEffect_FireBall)
                                                                    behind_effect_y -= 160 - 30 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_IceBall)
                                                                    behind_effect_y -= 160 - 30 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_EnergyBall)
                                                                    behind_effect_y -= 10 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_Boulders)
                                                                    behind_effect_y -= 160 - 30 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_Aura)
                                                                    behind_effect_y -= 12;
                                                                if (player.effect_id == SpellEffect_Heaven)
                                                                    behind_effect_y -= 114;
                                                                if (player.effect_id == SpellEffect_DarkBeam)
                                                                    behind_effect_y -= 114;
                                                                if (player.effect_id == SpellEffect_GreenFlame)
                                                                    behind_effect_y -= 44;
                                                                if (player.effect_id == SpellEffect_Whirl)
                                                                {
                                                                    behind_effect_y -= 10;
                                                                    if (!player.effect_frame)
                                                                        behind_effect_x -= 20;
                                                                    if (player.effect_frame == 1)
                                                                        behind_effect_y += 14;
                                                                    if (player.effect_frame == 2)
                                                                        behind_effect_x += 20;
                                                                    if (player.effect_frame == 3)
                                                                        behind_effect_y -= 14;
                                                                }
                                                                if (player.effect_id == SpellEffect_DarkBeam ||
                                                                    player.effect_id == SpellEffect_Heaven ||
                                                                    player.effect_id == SpellEffect_Aura)
                                                                {
                                                                    effect_frame = player.effect_loops_left;
                                                                    if (effect_frame > 0)
                                                                        effect_frame = random(2) + 3;
                                                                    gfx->game->BlitClipped(behind_effect_x +
                                                                        shake_offset, behind_effect_y,
                                                                        gfx->effects->GetGraphic(player.effect_id).resource->layer0_surface,
                                                                        gfx->effects->GetGraphic(player.effect_id).resource->GetFrameRect(effect_frame));
                                                                }
                                                                else
                                                                    gfx->game->BlitClipped(behind_effect_x +
                                                                        shake_offset, behind_effect_y,
                                                                        gfx->effects->GetGraphic(player.effect_id).resource->layer0_surface,
                                                                        gfx->effects->GetGraphic(player.effect_id).resource->GetFrameRect(player.effect_frame));
                                                            }
                                                        }
                                                    }
                                                    if (player.dying || player.hiding || player.unhiding ||
                                                        player.hidden)
                                                        gfx->sprites->DrawPlayer(player, true, shake_offset);
                                                    else
                                                        gfx->sprites->DrawPlayer(player, false, shake_offset);
                                                    if (player.attacking)
                                                    {
                                                        if (player.attack_frame >= 2)
                                                        {
                                                            if (player.weapon > 0)
                                                            {
                                                                int weapon_effect_x =
                                                                    32 * player.x - 32 * player.y +
                                                                        MainForm->map->GetScrollOffsetX() + 267;
                                                                int weapon_effect_y =
                                                                    16 * player.x + 16 * player.y +
                                                                        MainForm->map->GetScrollOffsetY() + 100;
                                                                if (shake_offset)
                                                                    weapon_effect_x += shake_offset;
                                                                if (player.direction == FacingDirection_Left)
                                                                {
                                                                    weapon_effect_y -= 15;
                                                                    weapon_effect_x -= 4;
                                                                }
                                                                if (player.direction == FacingDirection_Up)
                                                                {
                                                                    weapon_effect_y -= 15;
                                                                    weapon_effect_x += 32;
                                                                }
                                                                if (player.direction == FacingDirection_Right)
                                                                    weapon_effect_x += 28;
                                                                if (!weapon_mapper->IsRangedWeapon(player.weapon))
                                                                {
                                                                    int weapon_effect_type =
                                                                        weapon_mapper->weapon_effect_types[player.weapon];
                                                                    gfx->game->BlitClippedHalfBlend(weapon_effect_x,
                                                                        weapon_effect_y,
                                                                        gfx->interface_graphics->weapon_effects,
                                                                        gfx->interface_graphics->GetCellRect(weapon_effect_type,
                                                                            player.direction));
                                                                }
                                                            }
                                                        }
                                                    }
                                                    if (player.effect_running)
                                                    {
                                                        if (player.effect_blend_layer || player.effect_solid_layer)
                                                        {
                                                            if (gfx->effects->RequestGraphic(player.effect_id))
                                                            {
                                                                GraphicSize front_effect_size =
                                                                    gfx->effects->GetGraphicSize(player.effect_id);
                                                                int front_effect_x;
                                                                int front_effect_frame;
                                                                int flicker_roll;
                                                                front_effect_x =
                                                                    32 * player.x - 32 * player.y +
                                                                        MainForm->map->GetScrollOffsetX() + 309;
                                                                int front_effect_y =
                                                                    16 * player.x + 16 * player.y +
                                                                        MainForm->map->GetScrollOffsetY() + 120;
                                                                front_effect_x -= front_effect_size.width / 2;
                                                                front_effect_y -= front_effect_size.height / 2;
                                                                if (player.walking)
                                                                {
                                                                    if (player.walk_direction == FacingDirection_Left)
                                                                    {
                                                                        front_effect_x =
                                                                            front_effect_x + 32 -
                                                                                (8 * player.walk_frame + 8);
                                                                        front_effect_y =
                                                                            front_effect_y + 16 -
                                                                                (4 * player.walk_frame + 4);
                                                                    }
                                                                    if (player.walk_direction == FacingDirection_Up)
                                                                    {
                                                                        front_effect_x =
                                                                            8 * player.walk_frame + front_effect_x - 24;
                                                                        front_effect_y =
                                                                            front_effect_y + 16 -
                                                                                (4 * player.walk_frame + 4);
                                                                    }
                                                                    if (player.walk_direction == FacingDirection_Right)
                                                                    {
                                                                        front_effect_x =
                                                                            8 * player.walk_frame + front_effect_x - 24;
                                                                        front_effect_y =
                                                                            front_effect_y + 4 * player.walk_frame - 12;
                                                                    }
                                                                    if (player.walk_direction == FacingDirection_Down)
                                                                    {
                                                                        front_effect_x =
                                                                            front_effect_x + 32 -
                                                                                (8 * player.walk_frame + 8);
                                                                        front_effect_y =
                                                                            front_effect_y + 4 * player.walk_frame - 12;
                                                                    }
                                                                }
                                                                if (player.effect_id == SpellEffect_FireBall)
                                                                    front_effect_y -= 160 - 30 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_IceBall)
                                                                    front_effect_y -= 160 - 30 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_EnergyBall)
                                                                    front_effect_y -= 10 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_Boulders)
                                                                    front_effect_y -= 160 - 30 * player.effect_frame;
                                                                if (player.effect_id == SpellEffect_Aura)
                                                                    front_effect_y -= 12;
                                                                if (player.effect_id == SpellEffect_Heaven)
                                                                    front_effect_y -= 114;
                                                                if (player.effect_id == SpellEffect_DarkBeam)
                                                                    front_effect_y -= 114;
                                                                if (player.effect_id == SpellEffect_GreenFlame)
                                                                    front_effect_y -= 44;
                                                                if (player.effect_id == SpellEffect_Whirl)
                                                                {
                                                                    front_effect_y -= 10;
                                                                    if (!player.effect_frame)
                                                                        front_effect_x -= 20;
                                                                    if (player.effect_frame == 1)
                                                                        front_effect_y += 14;
                                                                    if (player.effect_frame == 2)
                                                                        front_effect_x += 20;
                                                                    if (player.effect_frame == 3)
                                                                        front_effect_y -= 14;
                                                                }
                                                                if (player.effect_id == SpellEffect_DarkBeam ||
                                                                    player.effect_id == SpellEffect_Heaven ||
                                                                    player.effect_id == SpellEffect_Aura)
                                                                {
                                                                    front_effect_frame = player.effect_loops_left;
                                                                    flicker_roll = random(2);
                                                                    if (front_effect_frame > 0)
                                                                        front_effect_frame = 4;
                                                                    if (player.effect_id == SpellEffect_DarkBeam)
                                                                        front_effect_frame = random(2) + 3;
                                                                    if (flicker_roll > 0)
                                                                    {
                                                                        if (player.effect_blend_layer)
                                                                            gfx->game->BlitClippedHalfBlend(front_effect_x +
                                                                                shake_offset, front_effect_y,
                                                                                gfx->effects->GetGraphic(player.effect_id).resource->layer1_surface,
                                                                                gfx->effects->GetGraphic(player.effect_id).resource->GetFrameRect(front_effect_frame));
                                                                    }
                                                                }
                                                                else
                                                                {
                                                                    if (player.effect_blend_layer)
                                                                        gfx->game->BlitClippedHalfBlend(front_effect_x +
                                                                            shake_offset, front_effect_y,
                                                                            gfx->effects->GetGraphic(player.effect_id).resource->layer1_surface,
                                                                            gfx->effects->GetGraphic(player.effect_id).resource->GetFrameRect(player.effect_frame));
                                                                    if (player.effect_solid_layer)
                                                                        gfx->game->BlitClipped(front_effect_x +
                                                                            shake_offset, front_effect_y,
                                                                            gfx->effects->GetGraphic(player.effect_id).resource->layer2_surface,
                                                                            gfx->effects->GetGraphic(player.effect_id).resource->GetFrameRect(player.effect_frame));
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                                player_found = true;
                                                player_index++;
                                            }
                                        }
                                    } while ((int)MainForm->players->GetCount() > player_index && player_found);
                                }
                                if ((int)MainForm->npcs->GetCount() > npc_index)
                                {
                                    char npc_found;
                                    do
                                    {
                                        npc_found = false;
                                        Npc npc = MainForm->npcs->GetNpc(npc_index);
                                        if (ii == npc.x)
                                        {
                                            if (n == npc.y)
                                            {
                                                if (npc.effect_running)
                                                {
                                                    if (npc.effect_base_layer)
                                                    {
                                                        if (gfx->effects->RequestGraphic(npc.effect_id))
                                                        {
                                                            GraphicSize npc_behind_effect_size =
                                                                gfx->effects->GetGraphicSize(npc.effect_id);
                                                            int npc_behind_effect_x;
                                                            int npc_behind_effect_y[1];
                                                            npc_behind_effect_x =
                                                                32 * npc.x - 32 * npc.y +
                                                                    MainForm->map->GetScrollOffsetX() + 309;
                                                            npc_behind_effect_y[0] =
                                                                16 * npc.x + 16 * npc.y +
                                                                    MainForm->map->GetScrollOffsetY() + 120;
                                                            npc_behind_effect_x -= npc_behind_effect_size.width / 2;
                                                            npc_behind_effect_y[0] -= npc_behind_effect_size.height / 2;
                                                            if (npc.walking)
                                                            {
                                                                if (npc.walk_direction == FacingDirection_Left)
                                                                {
                                                                    npc_behind_effect_x =
                                                                        npc_behind_effect_x + 32 -
                                                                            (8 * npc.walk_frame + 8);
                                                                    npc_behind_effect_y[0] =
                                                                        npc_behind_effect_y[0] + 16 -
                                                                            (4 * npc.walk_frame + 4);
                                                                }
                                                                if (npc.walk_direction == FacingDirection_Up)
                                                                {
                                                                    npc_behind_effect_x =
                                                                        8 * npc.walk_frame + npc_behind_effect_x - 24;
                                                                    npc_behind_effect_y[0] =
                                                                        npc_behind_effect_y[0] + 16 -
                                                                            (4 * npc.walk_frame + 4);
                                                                }
                                                                if (npc.walk_direction == FacingDirection_Right)
                                                                {
                                                                    npc_behind_effect_x =
                                                                        8 * npc.walk_frame + npc_behind_effect_x - 24;
                                                                    npc_behind_effect_y[0] =
                                                                        4 * npc.walk_frame + npc_behind_effect_y[0] -
                                                                            12;
                                                                }
                                                                if (npc.walk_direction == FacingDirection_Down)
                                                                {
                                                                    npc_behind_effect_x =
                                                                        npc_behind_effect_x + 32 -
                                                                            (8 * npc.walk_frame + 8);
                                                                    npc_behind_effect_y[0] =
                                                                        4 * npc.walk_frame + npc_behind_effect_y[0] -
                                                                            12;
                                                                }
                                                            }
                                                            if (npc.effect_id == SpellEffect_FireBall)
                                                                npc_behind_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_IceBall)
                                                                npc_behind_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_EnergyBall)
                                                                npc_behind_effect_y[0] -= 10 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_Boulders)
                                                                npc_behind_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_Aura)
                                                                npc_behind_effect_y[0] -= 12;
                                                            if (npc.effect_id == SpellEffect_GreenFlame)
                                                                npc_behind_effect_y[0] -= 44;
                                                            if (npc.effect_id == SpellEffect_Whirl)
                                                            {
                                                                npc_behind_effect_y[0] -= 10;
                                                                if (!npc.effect_frame)
                                                                    npc_behind_effect_x -= 20;
                                                                if (npc.effect_frame == 1)
                                                                    npc_behind_effect_y[0] += 14;
                                                                if (npc.effect_frame == 2)
                                                                    npc_behind_effect_x += 20;
                                                                if (npc.effect_frame == 3)
                                                                    npc_behind_effect_y[0] -= 14;
                                                            }
                                                            gfx->game->BlitClipped(npc_behind_effect_x + shake_offset,
                                                                npc_behind_effect_y[0],
                                                                gfx->effects->GetGraphic(npc.effect_id).resource->layer0_surface,
                                                                gfx->effects->GetGraphic(npc.effect_id).resource->GetFrameRect(npc.effect_frame));
                                                        }
                                                    }
                                                }
                                                if (npc.dying)
                                                {
                                                    if (npc.death_timer > 2)
                                                        gfx->npc_cache->DrawNpc(npc, 30 * npc.death_timer - 60,
                                                            shake_offset);
                                                }
                                                else if (npc.graphic == NpcGraphic_Wraith || npc.graphic ==
                                                    NpcGraphic_CursedMask || npc.graphic == NpcGraphic_AncientWraith)
                                                {
                                                    int npc_alpha = random(20) + 44;
                                                    if (npc.attacking || npc.alt_attacking)
                                                        npc_alpha = 90;
                                                    gfx->npc_cache->DrawNpc(npc, npc_alpha, shake_offset);
                                                }
                                                else
                                                    gfx->npc_cache->DrawNpc(npc, 255, shake_offset);
                                                if (npc.effect_running)
                                                {
                                                    if (npc.effect_blend_layer || npc.effect_solid_layer)
                                                    {
                                                        if (gfx->effects->RequestGraphic(npc.effect_id))
                                                        {
                                                            GraphicSize npc_front_effect_size =
                                                                gfx->effects->GetGraphicSize(npc.effect_id);
                                                            int npc_front_effect_x;
                                                            int npc_front_effect_y[1];
                                                            npc_front_effect_x =
                                                                32 * npc.x - 32 * npc.y +
                                                                    MainForm->map->GetScrollOffsetX() + 309;
                                                            npc_front_effect_y[0] =
                                                                16 * npc.x + 16 * npc.y +
                                                                    MainForm->map->GetScrollOffsetY() + 120;
                                                            npc_front_effect_x -= npc_front_effect_size.width / 2;
                                                            npc_front_effect_y[0] -= npc_front_effect_size.height / 2;
                                                            if (npc.walking)
                                                            {
                                                                if (npc.walk_direction == FacingDirection_Left)
                                                                {
                                                                    npc_front_effect_x =
                                                                        npc_front_effect_x + 32 -
                                                                            (8 * npc.walk_frame + 8);
                                                                    npc_front_effect_y[0] =
                                                                        npc_front_effect_y[0] + 16 -
                                                                            (4 * npc.walk_frame + 4);
                                                                }
                                                                if (npc.walk_direction == FacingDirection_Up)
                                                                {
                                                                    npc_front_effect_x =
                                                                        8 * npc.walk_frame + npc_front_effect_x - 24;
                                                                    npc_front_effect_y[0] =
                                                                        npc_front_effect_y[0] + 16 -
                                                                            (4 * npc.walk_frame + 4);
                                                                }
                                                                if (npc.walk_direction == FacingDirection_Right)
                                                                {
                                                                    npc_front_effect_x =
                                                                        8 * npc.walk_frame + npc_front_effect_x - 24;
                                                                    npc_front_effect_y[0] =
                                                                        npc_front_effect_y[0] + 4 * npc.walk_frame - 12;
                                                                }
                                                                if (npc.walk_direction == FacingDirection_Down)
                                                                {
                                                                    npc_front_effect_x =
                                                                        npc_front_effect_x + 32 -
                                                                            (8 * npc.walk_frame + 8);
                                                                    npc_front_effect_y[0] =
                                                                        npc_front_effect_y[0] + 4 * npc.walk_frame - 12;
                                                                }
                                                            }
                                                            if (npc.effect_id == SpellEffect_FireBall)
                                                                npc_front_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_IceBall)
                                                                npc_front_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_EnergyBall)
                                                                npc_front_effect_y[0] -= 10 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_Boulders)
                                                                npc_front_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                                            if (npc.effect_id == SpellEffect_Aura)
                                                                npc_front_effect_y[0] -= 12;
                                                            if (npc.effect_id == SpellEffect_GreenFlame)
                                                                npc_front_effect_y[0] -= 44;
                                                            if (npc.effect_id == SpellEffect_Whirl)
                                                            {
                                                                npc_front_effect_y[0] -= 10;
                                                                if (!npc.effect_frame)
                                                                    npc_front_effect_x -= 20;
                                                                if (npc.effect_frame == 1)
                                                                    npc_front_effect_y[0] += 14;
                                                                if (npc.effect_frame == 2)
                                                                    npc_front_effect_x += 20;
                                                                if (npc.effect_frame == 3)
                                                                    npc_front_effect_y[0] -= 14;
                                                            }
                                                            if (npc.effect_blend_layer)
                                                                gfx->game->BlitClippedHalfBlend(npc_front_effect_x +
                                                                    shake_offset, npc_front_effect_y[0],
                                                                    gfx->effects->GetGraphic(npc.effect_id).resource->layer1_surface,
                                                                    gfx->effects->GetGraphic(npc.effect_id).resource->GetFrameRect(npc.effect_frame));
                                                            if (npc.effect_solid_layer)
                                                                gfx->game->BlitClipped(npc_front_effect_x +
                                                                    shake_offset, npc_front_effect_y[0],
                                                                    gfx->effects->GetGraphic(npc.effect_id).resource->layer2_surface,
                                                                    gfx->effects->GetGraphic(npc.effect_id).resource->GetFrameRect(npc.effect_frame));
                                                        }
                                                    }
                                                }
                                                npc_found = true;
                                                npc_index++;
                                            }
                                        }
                                    } while ((int)MainForm->npcs->GetCount() > npc_index && npc_found);
                                }
                                if (MainForm->effects->GetCount() > effect_index)
                                {
                                    char effect_found;
                                    do
                                    {
                                        effect_found = false;
                                        MapEffect map_effect = MainForm->effects->GetEffect(effect_index);
                                        if (ii == map_effect.x && n == map_effect.y)
                                        {
                                            if (!map_effect.finished)
                                            {
                                                if (gfx->effects->RequestGraphic(map_effect.effect_id))
                                                {
                                                    GraphicSize effect_size =
                                                        gfx->effects->GetGraphicSize(map_effect.effect_id);
                                                    int effect_x =
                                                        32 * map_effect.x - 32 * map_effect.y +
                                                            MainForm->map->GetScrollOffsetX() + 309;
                                                    int map_effect_y =
                                                        16 * map_effect.x + 16 * map_effect.y +
                                                            MainForm->map->GetScrollOffsetY() + 120;
                                                    effect_x -= effect_size.width / 2;
                                                    map_effect_y -= effect_size.height / 2;
                                                    if (map_effect.has_blend_layer)
                                                        gfx->game->BlitClippedHalfBlend(effect_x + shake_offset,
                                                            map_effect_y,
                                                            gfx->effects->GetGraphic(map_effect.effect_id).resource->layer1_surface,
                                                            gfx->effects->GetGraphic(map_effect.effect_id).resource->GetFrameRect(map_effect.frame));
                                                    if (map_effect.has_solid_layer)
                                                        gfx->game->BlitClipped(effect_x + shake_offset, map_effect_y,
                                                            gfx->effects->GetGraphic(map_effect.effect_id).resource->layer2_surface,
                                                            gfx->effects->GetGraphic(map_effect.effect_id).resource->GetFrameRect(map_effect.frame));
                                                }
                                            }
                                            effect_found = true;
                                            effect_index++;
                                        }
                                    } while (MainForm->effects->GetCount() > effect_index && effect_found);
                                }
                                if (map->masks[ii][n] >= 1)
                                {
                                    if (net_client->IsObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                                    {
                                        if (gfx->masks->RequestGraphic(map->masks[ii][n]))
                                        {
                                            GraphicSize mask_size = gfx->masks->GetGraphicSize(map->masks[ii][n]);
                                            int mask_x = screen_x + 30 - mask_size.width / 2;
                                            int mask_y = screen_y + 30 - mask_size.height;
                                            gfx->game->BlitClipped(mask_x, mask_y,
                                                gfx->masks->GetGraphic(map->masks[ii][n]).resource->surface,
                                                gfx->masks->GetGraphic(map->masks[ii][n]).resource->surface_rect);
                                        }
                                    }
                                }
                                if (map->walls[ii][n] >= 1)
                                {
                                    if (net_client->IsWallInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                                    {
                                        if (gfx->walls->RequestGraphic(map->walls[ii][n]))
                                        {
                                            GraphicSize wall_size = gfx->walls->GetGraphicSize(map->walls[ii][n]);
                                            int wall_y = screen_y + 31 - wall_size.height;
                                            gfx->game->BlitClipped(screen_x, wall_y,
                                                gfx->walls->GetGraphic(map->walls[ii][n]).resource->surface,
                                                gfx->walls->GetGraphic(map->walls[ii][n]).resource->GetFrameRect(map->tile_frame));
                                            if (gfx->walls->GetGraphic(map->walls[ii][n]).resource->is_animated)
                                                MainForm->map->has_animated_tiles = true;
                                        }
                                        else
                                            redraw_needed = true;
                                    }
                                }
                            }
                            if (n < map->GetHeight())
                            {
                                if (map->walls_next_row[ii][n + 1] >= 1)
                                {
                                    if (net_client->IsWallInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n + 1))
                                    {
                                        if (gfx->walls->RequestGraphic(map->walls_next_row[ii][n + 1]))
                                        {
                                            GraphicSize lower_wall_size =
                                                gfx->walls->GetGraphicSize(map->walls_next_row[ii][n + 1]);
                                            int lower_wall_y = screen_y + 31 - lower_wall_size.height;
                                            gfx->game->BlitClipped(screen_x, lower_wall_y + 16,
                                                gfx->walls->GetGraphic(map->walls_next_row[ii][n +
                                                    1]).resource->surface,
                                                gfx->walls->GetGraphic(map->walls_next_row[ii][n +
                                                    1]).resource->GetFrameRect(map->tile_frame));
                                            if (gfx->walls->GetGraphic(map->walls_next_row[ii][n +
                                                1]).resource->is_animated)
                                                MainForm->map->has_animated_tiles = true;
                                        }
                                        else
                                            redraw_needed = true;
                                    }
                                }
                            }
                            if (n >= 0)
                            {
                                if (map->top_tiles[ii][n] >= 1)
                                {
                                    if (net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                                    {
                                        if (gfx->tops->RequestGraphic(map->top_tiles[ii][n]))
                                            gfx->game->BlitClipped(screen_x, screen_y - 64,
                                                gfx->tops->GetGraphic(map->top_tiles[ii][n]).resource->surface,
                                                gfx->tops->GetGraphic(map->top_tiles[ii][n]).resource->surface_rect);
                                    }
                                }
                            }
                        }
                    }
                    int top_mask_cell_x;
                    int top_mask_cell_y;
                    for (jj = first_y - 1; jj <= last_y; jj++)
                    {
                        for (kk = first_x; kk <= last_x; kk++)
                        {
                            top_mask_cell_x = 32 * kk - 32 * jj + map->GetScrollOffsetX() + 278;
                            top_mask_cell_y = map->GetScrollOffsetY() + (16 * kk + 16 * jj) + 134;
                            if (shake_offset)
                                top_mask_cell_x += shake_offset;
                            if (map->top_masks[kk][jj] >= 1)
                            {
                                if (net_client->IsObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), kk, jj))
                                {
                                    if (gfx->masks->RequestGraphic(map->top_masks[kk][jj]))
                                    {
                                        GraphicSize top_mask_size = gfx->masks->GetGraphicSize(map->top_masks[kk][jj]);
                                        int top_mask_x = top_mask_cell_x + 30 - top_mask_size.width / 2;
                                        int top_mask_y = top_mask_cell_y + 30 - top_mask_size.height;
                                        gfx->game->BlitClipped(top_mask_x, top_mask_y,
                                            gfx->masks->GetGraphic(map->top_masks[kk][jj]).resource->surface,
                                            gfx->masks->GetGraphic(map->top_masks[kk][jj]).resource->surface_rect);
                                    }
                                }
                            }
                        }
                    }
                    int status_icon_x;
                    int icon_y;
                    unsigned char icon_count;
                    // The own character is drawn once more, over everything on the map.
                    if (own_player_index != -1)
                    {
                        Player own_player = players->GetPlayer(own_player_index);
                        if (net_client->GetPlayerId() == own_player.player_id)
                            gfx->sprites->DrawPlayer(own_player, true, shake_offset);
                    }
                    gfx->tiles->AgeGraphics();
                    gfx->walls->AgeGraphics();
                    gfx->objects->AgeGraphics();
                    gfx->masks->AgeGraphics();
                    gfx->tops->AgeGraphics();
                    gfx->blends->AgeGraphics();
                    gfx->items->AgeGraphics();
                    gfx->icons->AgeGraphics();
                    gfx->effects->AgeGraphics();
                    // Minimap mode 1 is the whole map over the view, mode 2 the radar in its corner.
                    if (MainForm->gfx->interface_graphics->minimap_visible == 1)
                        gfx->interface_graphics->DrawFullMinimap();
                    if (MainForm->gfx->interface_graphics->minimap_visible == 2)
                        gfx->interface_graphics->DrawMinimap();
                    gfx->sprites->DrawNameLabel();
                    // The status icons from the left: a boss, map type 3, map attributes 1 and 2, the shield and
                    // the weapon; a spell awaiting its target shows at the right end.
                    status_icon_x = 18;
                    icon_y = 282;
                    icon_count = 0;
                    if (map->HasBoss())
                    {
                        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface,
                            gfx->interface_graphics->status_icons,
                            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Boss, false), 140);
                        status_icon_x += 18;
                        icon_count++;
                    }
                    if (map->GetMapType() == PVP_MAP_TYPE)
                    {
                        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface,
                            gfx->interface_graphics->status_icons,
                            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Pvp, false), 140);
                        status_icon_x += 18;
                        icon_count++;
                    }
                    if (map->GetMapAttribute() == 1 || map->GetMapAttribute() == 2)
                    {
                        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface,
                            gfx->interface_graphics->status_icons,
                            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Drain, false), 140);
                        status_icon_x += 18;
                        icon_count++;
                    }
                    if (icon_count >= 2)
                    {
                        if (MainForm->gfx->interface_graphics->minimap_visible == 2)
                        {
                            if (!config->GetRadarPosition())
                                status_icon_x = 145;
                        }
                        status_icon_x += 10;
                    }
                    else if (own_has_shield)
                    {
                        if (own_has_weapon)
                            status_icon_x += 10;
                    }
                    if (own_has_shield)
                    {
                        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface,
                            gfx->interface_graphics->status_icons,
                            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Shield, false), 140);
                        status_icon_x += 17;
                        icon_count++;
                    }
                    if (own_has_weapon)
                    {
                        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface,
                            gfx->interface_graphics->status_icons,
                            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Weapon, false), 140);
                        status_icon_x += 18;
                        icon_count++;
                    }
                    if (animation->interface_controller->spells->awaiting_target)
                        MainForm->blitter->AlphaBlt(602, icon_y, OffscreenSurface,
                            gfx->interface_graphics->status_icons,
                            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_SpellTarget, false), 140);
                    OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_frame,
                        &gfx->interface_graphics->hud_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    OffscreenSurface->BltFast(7, 53, gfx->interface_graphics->hud_left_frame,
                        &gfx->interface_graphics->hud_left_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    OffscreenSurface->BltFast(49, 7, gfx->interface_graphics->hud_top_frame,
                        &gfx->interface_graphics->hud_top_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    animation->interface_controller->game_bars->MarkDirty();
                    // Then the popups, the boss bars and each open window over the map; the boxes go last.
                    if (animation->popup->is_visible)
                        animation->popup->Draw();
                    if (animation->boss_bars->IsAnyVisible())
                        animation->boss_bars->Draw();
                    if (animation->game->player_menu_visible)
                    {
                        gfx->game->BlitClippedHalfBlend(animation->game->player_menu_x + 4,
                            animation->game->player_menu_y + 4, gfx->game->extra_surface,
                            gfx->game->player_menu_shadow_rect);
                        gfx->game->BlitClipped(animation->game->player_menu_x, animation->game->player_menu_y,
                            gfx->game->extra_surface, gfx->game->player_menu_rect);
                        if (animation->game->player_menu_hovered_option >= 0)
                            gfx->game->BlitClipped(animation->game->player_menu_x + 13,
                                animation->game->player_menu_y + 15 * animation->game->player_menu_hovered_option + 9,
                                gfx->game->extra_surface,
                                gfx->game->GetPopupItemRect(animation->game->player_menu_hovered_option));
                    }
                    if (animation->quests->is_visible)
                    {
                        gfx->game->DrawQuestWindow();
                        gfx->game->DrawQuestButtons();
                        HDC quest_dc;
                        if (OffscreenSurface->GetDC(&quest_dc))
                        {
                            try
                            {
                                text_canvas->Handle = quest_dc;
                                gfx->game->DrawQuestText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(quest_dc);
                        }
                    }
                    if (animation->stat->is_visible)
                    {
                        gfx->game->DrawStatWindow();
                        gfx->game->DrawStatButtons();
                        HDC stat_dc;
                        if (OffscreenSurface->GetDC(&stat_dc))
                        {
                            try
                            {
                                text_canvas->Handle = stat_dc;
                                gfx->game->DrawStatText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(stat_dc);
                        }
                    }
                    if (animation->barber->is_visible)
                    {
                        gfx->game->DrawBarberWindow();
                        gfx->game->DrawBarberButtons();
                        HDC barber_dc;
                        if (OffscreenSurface->GetDC(&barber_dc))
                        {
                            try
                            {
                                text_canvas->Handle = barber_dc;
                                gfx->game->DrawBarberText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(barber_dc);
                        }
                    }
                    if (animation->chest->is_visible)
                    {
                        gfx->game->DrawChestWindow();
                        gfx->game->DrawChestButtons();
                        HDC chest_dc;
                        if (OffscreenSurface->GetDC(&chest_dc))
                        {
                            try
                            {
                                text_canvas->Handle = chest_dc;
                                gfx->game->DrawChestText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(chest_dc);
                        }
                    }
                    if (animation->memo->is_visible)
                    {
                        gfx->game->DrawMemoWindow();
                        gfx->game->DrawMemoButtons();
                        HDC memo_dc;
                        if (OffscreenSurface->GetDC(&memo_dc))
                        {
                            try
                            {
                                text_canvas->Handle = memo_dc;
                                gfx->game->DrawMemoText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(memo_dc);
                        }
                    }
                    if (animation->dialog->is_visible)
                    {
                        gfx->game->DrawDialogWindow();
                        gfx->game->DrawDialogButtons();
                        HDC dialog_dc;
                        if (OffscreenSurface->GetDC(&dialog_dc))
                        {
                            try
                            {
                                text_canvas->Handle = dialog_dc;
                                gfx->game->DrawDialogText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(dialog_dc);
                        }
                    }
                    if (animation->report->is_visible)
                    {
                        gfx->game->DrawReportWindow();
                        gfx->game->DrawReportButtons();
                        HDC report_dc;
                        if (OffscreenSurface->GetDC(&report_dc))
                        {
                            try
                            {
                                text_canvas->Handle = report_dc;
                                gfx->game->DrawReportText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(report_dc);
                        }
                    }
                    if (animation->guild->is_visible)
                    {
                        gfx->game->DrawGuildWindow();
                        gfx->game->DrawGuildButtons();
                        HDC guild_dc;
                        if (OffscreenSurface->GetDC(&guild_dc))
                        {
                            try
                            {
                                text_canvas->Handle = guild_dc;
                                gfx->game->DrawGuildText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(guild_dc);
                        }
                    }
                    if (animation->inn->is_visible)
                    {
                        gfx->game->DrawInnWindow();
                        gfx->game->DrawInnButtons();
                        HDC inn_dc;
                        if (OffscreenSurface->GetDC(&inn_dc))
                        {
                            try
                            {
                                text_canvas->Handle = inn_dc;
                                gfx->game->DrawInnText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(inn_dc);
                        }
                    }
                    if (animation->law->is_visible)
                    {
                        gfx->game->DrawLawWindow();
                        gfx->game->DrawLawButtons();
                        HDC law_dc;
                        if (OffscreenSurface->GetDC(&law_dc))
                        {
                            try
                            {
                                text_canvas->Handle = law_dc;
                                gfx->game->DrawLawText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(law_dc);
                        }
                    }
                    if (animation->locker->is_visible)
                    {
                        gfx->game->DrawLockerWindow();
                        gfx->game->DrawLockerButtons();
                        animation->locker->tooltip->DrawFrame();
                        HDC locker_dc;
                        if (OffscreenSurface->GetDC(&locker_dc))
                        {
                            try
                            {
                                text_canvas->Handle = locker_dc;
                                gfx->game->DrawLockerText();
                                animation->locker->tooltip->DrawString();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(locker_dc);
                        }
                    }
                    if (animation->shop->is_visible)
                    {
                        gfx->game->DrawShopWindow();
                        gfx->game->DrawShopButtons();
                        animation->shop->tooltip->DrawFrame();
                        HDC shop_dc;
                        if (OffscreenSurface->GetDC(&shop_dc))
                        {
                            try
                            {
                                text_canvas->Handle = shop_dc;
                                gfx->game->DrawShopText();
                                animation->shop->tooltip->DrawString();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(shop_dc);
                        }
                    }
                    if (animation->learn->is_visible)
                    {
                        gfx->game->DrawLearnWindow();
                        gfx->game->DrawLearnButtons();
                        animation->learn->tooltip->DrawFrame();
                        HDC learn_dc;
                        if (OffscreenSurface->GetDC(&learn_dc))
                        {
                            try
                            {
                                text_canvas->Handle = learn_dc;
                                gfx->game->DrawLearnText();
                                animation->learn->tooltip->DrawString();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(learn_dc);
                        }
                    }
                    if (animation->jukebox->is_visible)
                    {
                        gfx->game->DrawJukeboxWindow();
                        gfx->game->DrawJukeboxButtons();
                        HDC jukebox_dc;
                        if (OffscreenSurface->GetDC(&jukebox_dc))
                        {
                            try
                            {
                                text_canvas->Handle = jukebox_dc;
                                gfx->game->DrawJukeboxText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(jukebox_dc);
                        }
                    }
                    if (animation->musician->is_visible)
                    {
                        gfx->game->DrawMusicianWindow();
                        gfx->game->DrawMusicianButtons();
                    }
                    if (animation->bank->is_visible)
                    {
                        gfx->game->DrawBankWindow();
                        gfx->game->DrawBankButtons();
                        HDC bank_dc;
                        if (OffscreenSurface->GetDC(&bank_dc))
                        {
                            try
                            {
                                text_canvas->Handle = bank_dc;
                                gfx->game->DrawBankText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(bank_dc);
                        }
                    }
                    if (animation->book->is_visible)
                    {
                        gfx->game->DrawBookWindow();
                        gfx->game->DrawBookButtons();
                        HDC book_dc;
                        if (OffscreenSurface->GetDC(&book_dc))
                        {
                            try
                            {
                                text_canvas->Handle = book_dc;
                                gfx->game->DrawBookText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(book_dc);
                        }
                    }
                    if (animation->paperdoll->is_visible)
                    {
                        gfx->game->DrawPaperdollWindow();
                        gfx->game->DrawPaperdollButtons();
                        animation->paperdoll->tooltip->DrawFrame();
                        HDC paperdoll_dc;
                        if (OffscreenSurface->GetDC(&paperdoll_dc))
                        {
                            try
                            {
                                text_canvas->Handle = paperdoll_dc;
                                gfx->game->DrawPaperdollText();
                                animation->paperdoll->tooltip->DrawString();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(paperdoll_dc);
                        }
                    }
                    if (animation->trade->is_visible)
                    {
                        gfx->game->DrawTradeWindow();
                        gfx->game->DrawTradeButtons();
                        animation->trade->tooltip->DrawFrame();
                        HDC trade_dc;
                        if (OffscreenSurface->GetDC(&trade_dc))
                        {
                            try
                            {
                                text_canvas->Handle = trade_dc;
                                gfx->game->DrawTradeText();
                                animation->trade->tooltip->DrawString();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(trade_dc);
                        }
                    }
                    if (animation->game->amount_box_visible)
                    {
                        OffscreenSurface->BltFast(animation->game->amount_box_x, animation->game->amount_box_y,
                            gfx->interface_graphics->control_sheet, &gfx->game->amount_box_rect, DDBLTFAST_WAIT);
                        if (animation->game->amount_box_kind == AmountBoxKind_JunkItem)
                            OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                                animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                                &gfx->game->junk_title_rect, DDBLTFAST_WAIT);
                        if (animation->game->amount_box_kind == AmountBoxKind_Amount)
                            OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                                animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                                &gfx->game->amount_title_rect, DDBLTFAST_WAIT);
                        if (animation->game->amount_box_kind == AmountBoxKind_TradeAddItem)
                            OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                                animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                                &gfx->game->trade_title_rect, DDBLTFAST_WAIT);
                        if (animation->game->amount_box_kind == AmountBoxKind_BankDeposit ||
                            animation->game->amount_box_kind == AmountBoxKind_BankWithdraw)
                            OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                                animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                                &gfx->game->bank_title_rect, DDBLTFAST_WAIT);
                        if (animation->game->amount_box_kind == AmountBoxKind_ShopBuy ||
                            animation->game->amount_box_kind == AmountBoxKind_ShopSell ||
                            animation->game->amount_box_kind == AmountBoxKind_LockerAddItem)
                            OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                                animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                                &gfx->game->shop_title_rect, DDBLTFAST_WAIT);
                        if (animation->game->amount_slider_grabbed)
                            OffscreenSurface->BltFast(animation->game->amount_box_x +
                                animation->game->amount_slider_offset + 25, animation->game->amount_box_y + 96,
                                MainForm->gfx->interface_graphics->scroll_icons,
                                &MainForm->gfx->interface_graphics->GetScrollIconRect(6), DDBLTFAST_WAIT);
                        else
                        {
                            int knob_icon = 5;
                            if (animation->game->amount_slider_hovered)
                                knob_icon = 6;
                            if (edits->GetText(EditId_Amount).Length() > 0)
                            {
                                int amount = StrToInt(edits->GetText(EditId_Amount));
                                int slider_width = 107;
                                amount--;
                                double slider_step = (double)slider_width / (animation->game->amount_max - 1);
                                double knob_offset = amount * slider_step;
                                animation->game->amount_slider_offset = knob_offset;
                                if (animation->game->amount_slider_offset < 0)
                                    OffscreenSurface->BltFast(animation->game->amount_box_x + 25,
                                        animation->game->amount_box_y + 96,
                                        MainForm->gfx->interface_graphics->scroll_icons,
                                        &MainForm->gfx->interface_graphics->GetScrollIconRect(knob_icon),
                                        DDBLTFAST_WAIT);
                                else
                                    OffscreenSurface->BltFast(animation->game->amount_box_x +
                                        animation->game->amount_slider_offset + 25, animation->game->amount_box_y + 96,
                                        MainForm->gfx->interface_graphics->scroll_icons,
                                        &MainForm->gfx->interface_graphics->GetScrollIconRect(knob_icon),
                                        DDBLTFAST_WAIT);
                            }
                            else
                            {
                                animation->game->amount_slider_offset = 0;
                                OffscreenSurface->BltFast(animation->game->amount_box_x + 25,
                                    animation->game->amount_box_y + 96, MainForm->gfx->interface_graphics->scroll_icons,
                                    &MainForm->gfx->interface_graphics->GetScrollIconRect(knob_icon), DDBLTFAST_WAIT);
                            }
                        }
                        HDC amount_dc;
                        if (OffscreenSurface->GetDC(&amount_dc))
                        {
                            try
                            {
                                draw_canvas->Handle = amount_dc;
                                gfx->game->DrawItemAmountText();
                                draw_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(amount_dc);
                        }
                        gfx->game->DrawItemAmountButtons();
                    }
                    if (animation->message_board->is_visible)
                    {
                        gfx->game->DrawMsgBoardWindow();
                        gfx->game->DrawMsgBoardButtons();
                        HDC msg_board_dc;
                        if (OffscreenSurface->GetDC(&msg_board_dc))
                        {
                            try
                            {
                                text_canvas->Handle = msg_board_dc;
                                gfx->game->DrawMsgBoardText();
                                edit_canvas->DrawEditBoxes(EditGroup_MsgBoard);
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(msg_board_dc);
                        }
                    }
                    if (animation->social->is_open)
                    {
                        gfx->game->DrawSocialWindow();
                        gfx->game->DrawSocialButtons();
                        HDC social_dc;
                        if (OffscreenSurface->GetDC(&social_dc))
                        {
                            try
                            {
                                text_canvas->Handle = social_dc;
                                gfx->game->DrawSocialText();
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(social_dc);
                        }
                    }
                    if (animation->game->form_visible)
                    {
                        gfx->game->DrawListBox();
                        gfx->game->DrawListBoxButtons();
                        HDC form_dc;
                        if (OffscreenSurface->GetDC(&form_dc))
                        {
                            try
                            {
                                text_canvas->Handle = form_dc;
                                gfx->game->DrawFormText();
                                edit_canvas->DrawEditBoxes(EditGroup_InputForm);
                                text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(form_dc);
                        }
                    }
                    if (animation->game->input_box_visible)
                    {
                        OffscreenSurface->BltFast(animation->game->input_box_x, animation->game->input_box_y,
                            gfx->game->input_box_surface, &gfx->game->input_box_rect, DDBLTFAST_WAIT);
                        HDC input_box_dc;
                        if (OffscreenSurface->GetDC(&input_box_dc))
                        {
                            try
                            {
                                draw_canvas->Handle = input_box_dc;
                                gfx->game->DrawInputBoxText();
                                draw_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(input_box_dc);
                        }
                        gfx->game->DrawInputBoxButtons();
                    }
                    if (animation->game->large_box_visible)
                    {
                        OffscreenSurface->BltFast(animation->game->large_box_x, animation->game->large_box_y,
                            gfx->game->text_box_surface, &gfx->game->text_box_rect, DDBLTFAST_WAIT);
                        HDC text_box_dc;
                        if (OffscreenSurface->GetDC(&text_box_dc))
                        {
                            try
                            {
                                draw_canvas->Handle = text_box_dc;
                                gfx->game->DrawTextBoxText();
                                draw_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(text_box_dc);
                        }
                        gfx->game->DrawTextBoxButtons();
                    }
                    if (animation->game->message_visible)
                    {
                        OffscreenSurface->BltFast(animation->game->message_x, animation->game->message_y,
                            gfx->game->message_box_surface, &gfx->game->message_box_rect, DDBLTFAST_WAIT);
                        HDC message_box_dc;
                        if (OffscreenSurface->GetDC(&message_box_dc))
                        {
                            try
                            {
                                draw_canvas->Handle = message_box_dc;
                                gfx->game->DrawMessageBoxText();
                                draw_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            OffscreenSurface->ReleaseDC(message_box_dc);
                        }
                        gfx->game->DrawMessageBoxButtons();
                    }
                    if (animation->interface_controller->game_bars->TestAndClearDirty())
                    {
                        gfx->interface_graphics->DrawStatusBars();
                        HDC status_bar_dc;
                        if (animation->interface_controller->game_bars->show_health_text ||
                            animation->interface_controller->game_bars->show_mana_text ||
                            animation->interface_controller->game_bars->show_stamina_text ||
                            animation->interface_controller->game_bars->show_experience_text)
                        {
                            if (OffscreenSurface->GetDC(&status_bar_dc))
                            {
                                try
                                {
                                    text_canvas->Handle = status_bar_dc;
                                    gfx->interface_graphics->DrawStatusBarText();
                                    text_canvas->Handle = 0;
                                }
                                catch (Exception &)
                                {
                                }
                                OffscreenSurface->ReleaseDC(status_bar_dc);
                            }
                        }
                    }
                    gfx->interface_graphics->DrawCornerButton();
                    flip_needed = true;
                }
            }
        }
        HDC status_dc;
        int raise_button_frame;
        if (animation->interface_controller->TakeInvalidated())
        {
            flip_needed = true;
            OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
        }
        if (!animation->interface_controller->IsFullRedrawPending())
        {
            if (MainForm->animation->GetSubState() == GamePanel_Status)
            {
                if (animation->interface_controller->status->TestAndClearDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    if (OffscreenSurface->GetDC(&status_dc))
                    {
                        try
                        {
                            text_canvas->Handle = status_dc;
                            gfx->interface_graphics->DrawStatusValues();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(status_dc);
                    }
                }
                if (animation->interface_controller->status->stat_points > 0)
                {
                    raise_button_frame = 0;
                    // The stat-raise buttons blink while points are left to spend.
                    if (animation->interface_controller->status->GetBlinkPhase() > 5)
                        raise_button_frame = 1;
                    MainForm->OffscreenSurface->BltFast(208, 337, gfx->interface_graphics->control_sheet,
                        &gfx->interface_graphics->GetRaiseButtonRect(raise_button_frame), DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(208, 355, gfx->interface_graphics->control_sheet,
                        &gfx->interface_graphics->GetRaiseButtonRect(raise_button_frame), DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(208, 373, gfx->interface_graphics->control_sheet,
                        &gfx->interface_graphics->GetRaiseButtonRect(raise_button_frame), DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(208, 391, gfx->interface_graphics->control_sheet,
                        &gfx->interface_graphics->GetRaiseButtonRect(raise_button_frame), DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(208, 409, gfx->interface_graphics->control_sheet,
                        &gfx->interface_graphics->GetRaiseButtonRect(raise_button_frame), DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(208, 427, gfx->interface_graphics->control_sheet,
                        &gfx->interface_graphics->GetRaiseButtonRect(raise_button_frame), DDBLTFAST_WAIT);
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Chat)
            {
                if (animation->interface_controller->chat->TakeTextDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawChatPanel();
                    HDC chat_dc;
                    if (OffscreenSurface->GetDC(&chat_dc))
                    {
                        try
                        {
                            text_canvas->Handle = chat_dc;
                            gfx->interface_graphics->DrawChatText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(chat_dc);
                    }
                }
                if (animation->interface_controller->chat->TakeScrollbarDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawChatScrollButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Playerlist)
            {
                if (animation->interface_controller->player_list->TestAndClearListDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawPlayerList();
                    HDC player_list_dc;
                    if (OffscreenSurface->GetDC(&player_list_dc))
                    {
                        try
                        {
                            text_canvas->Handle = player_list_dc;
                            gfx->interface_graphics->DrawPlayerListText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(player_list_dc);
                    }
                }
                if (animation->interface_controller->player_list->TestAndClearScrollDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawPlayerListScrollButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Spells)
            {
                if (animation->interface_controller->spells->TestAndClearDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawSpellPanel();
                    HDC spell_dc;
                    if (OffscreenSurface->GetDC(&spell_dc))
                    {
                        try
                        {
                            text_canvas->Handle = spell_dc;
                            gfx->interface_graphics->DrawSpellInfoText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(spell_dc);
                    }
                    flip_needed = true;
                }
                if (animation->interface_controller->spells->TestAndClearButtonsDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawSpellScrollButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Skills)
            {
                if (animation->interface_controller->skills->TestAndClearDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawSkillPanel();
                    HDC skill_dc;
                    if (OffscreenSurface->GetDC(&skill_dc))
                    {
                        try
                        {
                            text_canvas->Handle = skill_dc;
                            gfx->interface_graphics->DrawSkillInfoText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(skill_dc);
                    }
                    flip_needed = true;
                }
                if (animation->interface_controller->skills->TestAndClearButtonsDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawSkillScrollButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Inventory)
            {
                if (animation->interface_controller->inventory->TestAndClearItemsDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    MainForm->animation->interface_controller->inventory->MarkHoverDirty();
                    gfx->interface_graphics->DrawInventoryGrid();
                    HDC inventory_dc;
                    if (OffscreenSurface->GetDC(&inventory_dc))
                    {
                        try
                        {
                            text_canvas->Handle = inventory_dc;
                            gfx->interface_graphics->DrawInventoryText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(inventory_dc);
                    }
                }
                if (animation->interface_controller->inventory->TestAndClearHoverDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawInventoryButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Group)
            {
                if (animation->interface_controller->group->TestAndClearListDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawGroupList();
                    HDC group_dc;
                    if (OffscreenSurface->GetDC(&group_dc))
                    {
                        try
                        {
                            text_canvas->Handle = group_dc;
                            gfx->interface_graphics->DrawGroupText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(group_dc);
                    }
                    HDC group_bar_dc;
                    if (OffscreenSurface->GetDC(&group_bar_dc))
                    {
                        try
                        {
                            draw_canvas->Handle = group_bar_dc;
                            gfx->interface_graphics->DrawGroupHealthBars();
                            draw_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(group_bar_dc);
                    }
                }
                if (animation->interface_controller->group->TestAndClearScrollDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawGroupScrollButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Settings)
            {
                if (animation->interface_controller->settings->TestAndClearDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawSettingMarkers();
                    HDC settings_dc;
                    if (OffscreenSurface->GetDC(&settings_dc))
                    {
                        try
                        {
                            text_canvas->Handle = settings_dc;
                            gfx->interface_graphics->DrawSettingsText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(settings_dc);
                    }
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_News)
            {
                if (animation->interface_controller->news->TestAndClearTextDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawNewsList();
                    HDC news_dc;
                    if (OffscreenSurface->GetDC(&news_dc))
                    {
                        try
                        {
                            text_canvas->Handle = news_dc;
                            gfx->interface_graphics->DrawNewsText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(news_dc);
                    }
                }
                if (animation->interface_controller->news->TestAndClearScrollDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawNewsScrollButtons();
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Help)
            {
                if (animation->interface_controller->help->TestAndClearTextDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                    gfx->interface_graphics->DrawHelpList();
                    HDC help_dc;
                    if (OffscreenSurface->GetDC(&help_dc))
                    {
                        try
                        {
                            text_canvas->Handle = help_dc;
                            gfx->interface_graphics->DrawHelpText();
                            text_canvas->Handle = 0;
                        }
                        catch (Exception &)
                        {
                        }
                        OffscreenSurface->ReleaseDC(help_dc);
                    }
                }
                if (animation->interface_controller->help->TestAndClearScrollDirty())
                {
                    flip_needed = true;
                    gfx->interface_graphics->DrawHelpScrollButtons();
                }
            }
            if (animation->interface_controller->TakeButtonHoverDirty())
            {
                flip_needed = true;
                gfx->interface_graphics->DrawTabButtons();
            }
            if (animation->interface_controller->TakeCornerHoverDirty())
            {
                flip_needed = true;
                gfx->interface_graphics->DrawCornerButton();
            }
            int status_change = animation->interface_controller->TakeStatusChange();
            if (status_change == 1)
            {
                OffscreenSurface->BltFast(99, 452, gfx->interface_graphics->tooltip_background,
                    &gfx->interface_graphics->tooltip_rect, DDBLTFAST_WAIT);
                HDC status_message_dc;
                if (OffscreenSurface->GetDC(&status_message_dc))
                {
                    try
                    {
                        status_canvas->Handle = status_message_dc;
                        status_canvas->Brush->Color = clBlack;
                        status_canvas->Brush->Style = bsClear;
                        status_canvas->Pen->Color = clBlack;
                        status_canvas->Pen->Style = psSolid;
                        if (!config->GetTextTheme())
                            status_canvas->Font->Color = clBlack;
                        if (config->GetTextTheme() == 1)
                            status_canvas->Font->Color = (TColor)0xB3B4B4;
                        status_canvas->Font->Name = "Arial";
                        status_canvas->Font->Size = 7;
                        status_canvas->TextOut(99, 455, animation->interface_controller->GetStatusMessage());
                        status_canvas->Handle = 0;
                    }
                    catch (Exception &)
                    {
                    }
                    OffscreenSurface->ReleaseDC(status_message_dc);
                }
            }
            if (status_change == 2)
                OffscreenSurface->BltFast(99, 452, gfx->interface_graphics->tooltip_background,
                    &gfx->interface_graphics->tooltip_rect, DDBLTFAST_WAIT);
        }
        // The clock is drawn again only when its text changes.
        String current_clock_text = TimeToStr(Time());
        if (clock_text != current_clock_text)
        {
            clock_text = current_clock_text;
            OffscreenSurface->BltFast(543, 452, gfx->interface_graphics->clock_background,
                &gfx->interface_graphics->clock_rect, DDBLTFAST_WAIT);
            HDC clock_dc;
            if (OffscreenSurface->GetDC(&clock_dc))
            {
                try
                {
                    status_canvas->Handle = clock_dc;
                    status_canvas->Brush->Color = clBlack;
                    status_canvas->Brush->Style = bsClear;
                    status_canvas->Pen->Color = clBlack;
                    status_canvas->Pen->Style = psSolid;
                    if (!config->GetTextTheme())
                        status_canvas->Font->Color = clBlack;
                    if (config->GetTextTheme() == 1)
                        status_canvas->Font->Color = (TColor)0xB5B4B3;
                    status_canvas->Font->Name = "Arial";
                    status_canvas->Font->Size = 7;
                    int clock_x = (86 - status_canvas->TextWidth(clock_text)) / 2 + 543;
                    status_canvas->TextOut(clock_x, 455, current_clock_text);
                    status_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(clock_dc);
            }
            flip_needed = true;
        }
        if (edits->GetCount() >= 1)
        {
            if (edits->HasDirtyEdit() || edits->ConsumeTickRedraw())
            {
                if (edits->ConsumeDirty(EditId_Chat))
                {
                    gfx->interface_graphics->DrawChatModeIcon();
                    OffscreenSurface->BltFast(98, 309, gfx->interface_graphics->chat_input_background,
                        &gfx->interface_graphics->chat_input_rect, DDBLTFAST_WAIT);
                    flip_needed = true;
                }
                HDC chat_edit_dc;
                if (OffscreenSurface->GetDC(&chat_edit_dc))
                {
                    try
                    {
                        text_canvas->Handle = chat_edit_dc;
                        edit_canvas->DrawEditBoxes(EditGroup_Game);
                        text_canvas->Handle = 0;
                    }
                    catch (Exception &)
                    {
                    }
                    OffscreenSurface->ReleaseDC(chat_edit_dc);
                }
            }
        }
    }
    if (animation->GetScreenMode() == ScreenMode_Menu)
    {
        flip_needed = true;
        OffscreenSurface->BltFast(0, 0, gfx->menu->background, &gfx->menu->background_rect, DDBLTFAST_WAIT);
        if (animation->GetSubState() == MenuScreen_Main || animation->GetSubState() == MenuScreen_Credits ||
            animation->GetSubState() == MenuScreen_Login || animation->GetSubState() == MenuScreen_GridAccountName ||
            animation->GetSubState() == MenuScreen_PasswordGrid)
            gfx->menu->DrawMainMenuButtons();
        if (animation->GetSubState() == MenuScreen_CharacterSelect)
        {
            gfx->menu->DrawCharacterSelect();
            gfx->sprites->DrawPreviewPlayers(0);
            HDC slot_text_dc;
            if (OffscreenSurface->GetDC(&slot_text_dc))
            {
                try
                {
                    text_canvas->Handle = slot_text_dc;
                    gfx->menu->DrawCharacterSlotText();
                    text_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(slot_text_dc);
            }
        }
        if (animation->GetSubState() == MenuScreen_CreateAccount)
            gfx->menu->DrawAccountCreationButtons();
        if (animation->GetSubState() == MenuScreen_Login || animation->GetSubState() == MenuScreen_GridAccountName ||
            animation->GetSubState() == MenuScreen_PasswordGrid)
            gfx->menu->DrawConfirmButtons();
        if (animation->GetSubState() == MenuScreen_PasswordGrid)
        {
            HDC login_dc;
            if (OffscreenSurface->GetDC(&login_dc))
            {
                try
                {
                    text_canvas->Handle = login_dc;
                    gfx->menu->DrawLoginText();
                    text_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(login_dc);
            }
            gfx->menu->DrawKeyHighlights();
        }
        HDC version_dc;
        if (animation->GetSubState() == MenuScreen_Main)
        {
            if (OffscreenSurface->GetDC(&version_dc))
            {
                try
                {
                    status_canvas->Handle = version_dc;
                    status_canvas->Brush->Color = clBlack;
                    status_canvas->Brush->Style = bsClear;
                    status_canvas->Font->Color = (TColor)0x96AABE;
                    status_canvas->Font->Name = "Arial";
                    status_canvas->Font->Size = 7;
                    status_canvas->TextOut(28, 456,
                        VersionString(version_major, version_minor, version_build, version_letter) + " - " +
                            config->GetHost());
                    status_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(version_dc);
            }
        }
        if (MainForm->animation->menu->is_loading_world)
        {
            TRect loading_frame_rect;
            loading_frame_rect.top = 0;
            loading_frame_rect.left = 220 * (MainForm->animation->menu->loading_frame / 2);
            loading_frame_rect.bottom = 127;
            loading_frame_rect.right = 220 * (MainForm->animation->menu->loading_frame / 2) + 220;
            OffscreenSurface->BltFast(397, 333, gfx->menu->animation_sheet, &loading_frame_rect, DDBLTFAST_WAIT);
            HDC loading_text_dc;
            if (OffscreenSurface->GetDC(&loading_text_dc))
            {
                try
                {
                    draw_canvas->Handle = loading_text_dc;
                    draw_canvas->Brush->Color = clBlack;
                    draw_canvas->Brush->Style = bsClear;
                    draw_canvas->Font->Color = (TColor)0xC8F0F0;
                    draw_canvas->Font->Name = "MS Sans Serif";
                    draw_canvas->Font->Size = 10;
                    char text_shown = false;
                    if (!text_shown)
                    {
                        if (animation->menu->IsMapNeeded())
                        {
                            if (animation->menu->IsEnteringGame())
                            {
                                draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingMap));
                                text_shown = true;
                            }
                        }
                    }
                    if (!text_shown)
                    {
                        if (animation->menu->IsItemFileNeeded())
                        {
                            if (animation->menu->IsEnteringGame())
                            {
                                draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingItems));
                                text_shown = true;
                            }
                        }
                    }
                    if (!text_shown)
                    {
                        if (animation->menu->IsNpcFileNeeded())
                        {
                            if (animation->menu->IsEnteringGame())
                            {
                                draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingNpcs));
                                text_shown = true;
                            }
                        }
                    }
                    if (!text_shown)
                    {
                        if (animation->menu->IsSkillFileNeeded())
                        {
                            if (animation->menu->IsEnteringGame())
                            {
                                draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingSkills));
                                text_shown = true;
                            }
                        }
                    }
                    if (!text_shown)
                    {
                        if (animation->menu->IsClassFileNeeded())
                        {
                            if (animation->menu->IsEnteringGame())
                            {
                                draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingClasses));
                                text_shown = true;
                            }
                        }
                    }
                    if (!text_shown)
                    {
                        if (animation->menu->IsReservedFileNeeded())
                        {
                            if (animation->menu->IsEnteringGame())
                            {
                                draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingGuilds));
                                text_shown = true;
                            }
                        }
                    }
                    if (!text_shown)
                    {
                        if (animation->menu->IsEnteringGame())
                            draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_LoadingGame));
                        else
                            draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_Wait));
                    }
                    draw_canvas->Brush->Color = clBlack;
                    draw_canvas->Brush->Style = bsClear;
                    draw_canvas->Font->Color = (TColor)0xB9B9B9;
                    draw_canvas->Font->Name = "MS Sans Serif";
                    draw_canvas->Font->Size = 8;
                    String tip_text;
                    if (animation->menu->loading_tip <= 6)
                        tip_text = MainForm->strings->GetText(animation->menu->loading_tip + 369);
                    if (animation->menu->loading_tip == 7)
                        tip_text = MainForm->strings->GetText(GameText_ItsRecommendedToChange);
                    if (animation->menu->loading_tip == 8)
                        tip_text = MainForm->strings->GetText(GameText_MakeSureNooneIs);
                    int tip_y = 394;
                    int break_index = 0;
                    char more_lines = true;
                    while (true)
                    {
                        if (draw_canvas->TextWidth(tip_text) > 180)
                        {
                            for (int prefix_length = 1; prefix_length <= tip_text.Length(); prefix_length++)
                            {
                                int prefix_width = draw_canvas->TextWidth(tip_text.SubString(1, prefix_length));
                                if (tip_text[prefix_length] == ' ' || prefix_length == tip_text.Length())
                                {
                                    if (prefix_width > 180 || prefix_length == tip_text.Length())
                                    {
                                        draw_canvas->TextOut(416, tip_y, tip_text.SubString(1, break_index - 1));
                                        tip_text.Delete(1, break_index);
                                        break;
                                    }
                                    break_index = prefix_length;
                                }
                            }
                        }
                        else
                        {
                            draw_canvas->TextOut(416, tip_y, tip_text);
                            more_lines = false;
                        }
                        tip_y += 16;
                        break_index = 0;
                        if (!more_lines)
                            break;
                    }
                    draw_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(loading_text_dc);
                if (animation->menu->IsRequestDelayDone())
                    net_client->sender->RequestGameData();
            }
        }
        if (animation->menu->is_form_open)
        {
            OffscreenSurface->BltFast(animation->menu->form_x, animation->menu->form_y,
                gfx->menu->create_character_background, &gfx->menu->create_character_rect, DDBLTFAST_WAIT);
            gfx->menu->DrawCreateCharacterControls();
            gfx->sprites->DrawPreviewPlayers(1);
        }
        HDC menu_edit_dc;
        if (edits->GetCount() >= 1)
        {
            if (OffscreenSurface->GetDC(&menu_edit_dc))
            {
                try
                {
                    text_canvas->Handle = menu_edit_dc;
                    edit_canvas->DrawEditBoxes(EditGroup_All);
                    text_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(menu_edit_dc);
            }
        }
        HDC credits_dc;
        if (animation->credits->is_running)
        {
            if (OffscreenSurface->GetDC(&credits_dc))
            {
                try
                {
                    draw_canvas->Handle = credits_dc;
                    draw_canvas->Brush->Color = clBlack;
                    draw_canvas->Brush->Style = bsClear;
                    draw_canvas->Font->Color = clWhite;
                    draw_canvas->Font->Size = 8;
                    draw_canvas->Font->Name = "Arial";
                    for (int credit_index = 0; animation->credits->shown_lines->Count > credit_index; credit_index++)
                    {
                        draw_canvas->Pen->Color = (TColor)0x28E6FA;
                        draw_canvas->Font->Size = 20;
                        draw_canvas->Font->Name = "Arial";
                        int indent = 0;
                        if (animation->credits->shown_styles->Strings[credit_index] == "t")
                        {
                            draw_canvas->Font->Size = 16;
                            draw_canvas->Font->Color = clWhite;
                            draw_canvas->Rectangle(350,
                                StrToInt(animation->credits->shown_y->Strings[credit_index]) + 29, 550,
                                StrToInt(animation->credits->shown_y->Strings[credit_index]) + 30);
                        }
                        if (animation->credits->shown_styles->Strings[credit_index] == "h")
                            draw_canvas->Font->Color = (TColor)0x28E6FA;
                        if (animation->credits->shown_styles->Strings[credit_index] == "n")
                        {
                            draw_canvas->Font->Color = (TColor)0xA1428;
                            indent = 10;
                        }
                        if (animation->credits->shown_styles->Strings[credit_index] == "s")
                        {
                            draw_canvas->Font->Color = clGray;
                            draw_canvas->Font->Size = 8;
                            indent = 10;
                        }
                        draw_canvas->TextOut(indent + 340, StrToInt(animation->credits->shown_y->Strings[credit_index]),
                            animation->credits->shown_lines->Strings[credit_index]);
                    }
                    draw_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(credits_dc);
            }
        }
        if (animation->menu->is_text_window_open)
        {
            OffscreenSurface->BltFast(animation->menu->text_window_x, animation->menu->text_window_y,
                gfx->menu->list_box_background, &gfx->menu->list_box_rect, DDBLTFAST_WAIT);
            HDC menu_list_dc;
            if (OffscreenSurface->GetDC(&menu_list_dc))
            {
                try
                {
                    draw_canvas->Handle = menu_list_dc;
                    gfx->menu->DrawListBoxText();
                    draw_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(menu_list_dc);
            }
            gfx->menu->DrawListBoxControls();
        }
        if (animation->menu->is_message_open)
        {
            OffscreenSurface->BltFast(animation->menu->message_x, animation->menu->message_y,
                gfx->menu->message_box_background, &gfx->menu->message_box_rect, DDBLTFAST_WAIT);
            HDC menu_message_dc;
            if (OffscreenSurface->GetDC(&menu_message_dc))
            {
                try
                {
                    draw_canvas->Handle = menu_message_dc;
                    gfx->menu->DrawMessageBoxText();
                    draw_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(menu_message_dc);
            }
            gfx->menu->DrawMessageBoxControls();
        }
    }
    if (animation->GetScreenMode() == ScreenMode_Intro)
    {
        if (animation->intro->clear_screen_pending)
        {
            TRect intro_rect;
            intro_rect.left = 0;
            intro_rect.top = 0;
            intro_rect.right = 640;
            intro_rect.bottom = 480;
            OffscreenSurface->Blt(&intro_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, gfx->intro->fill_fx);
            animation->intro->clear_screen_pending = false;
            flip_needed = true;
        }
        else if (animation->intro->logo_redraw_pending || animation->intro->is_animating)
        {
            TRect intro_panel_rect;
            intro_panel_rect.left = 0;
            intro_panel_rect.top = 0;
            intro_panel_rect.right = 358;
            intro_panel_rect.bottom = 187;
            OffscreenSurface->BltFast(141, 125, gfx->intro->surface, &intro_panel_rect, DDBLTFAST_WAIT);
            animation->intro->logo_redraw_pending = false;
            flip_needed = true;
        }
    }
    if (flip_needed)
    {
        if (show_frame_rate)
        {
            if (animation->GetScreenMode() == ScreenMode_Game)
                OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_frame,
                    &gfx->interface_graphics->hud_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            HDC frame_rate_dc;
            if (OffscreenSurface->GetDC(&frame_rate_dc))
            {
                try
                {
                    status_canvas->Handle = frame_rate_dc;
                    status_canvas->Brush->Color = clBlack;
                    status_canvas->Brush->Style = bsClear;
                    status_canvas->Font->Color = clWhite;
                    status_canvas->Font->Name = "Arial";
                    status_canvas->Font->Size = 7;
                    status_canvas->TextOut(2, 2, IntToStr(frame_rate));
                    status_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(frame_rate_dc);
            }
        }
    }
    char item_drag_drawn = false;
    char spell_drag_drawn = false;
    char skill_drag_drawn = false;
    if (dragging)
    {
        if (flip_needed || animation->interface_controller->inventory->TestAndClearDragDirty())
        {
            if (animation->GetSubState() == GamePanel_Inventory)
            {
                gfx->interface_graphics->DrawDraggedItem();
                flip_needed = true;
                item_drag_drawn = true;
            }
        }
        if (flip_needed || animation->interface_controller->spells->TestAndClearDragDirty())
        {
            if (animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                gfx->interface_graphics->DrawDraggedSpell();
                flip_needed = true;
                spell_drag_drawn = true;
            }
        }
        if (flip_needed || animation->interface_controller->skills->TestAndClearDragDirty())
        {
            if (animation->GetSubState() == GamePanel_Skills)
            {
                gfx->interface_graphics->DrawDraggedSkill();
                flip_needed = true;
                skill_drag_drawn = true;
            }
        }
    }
    if (config->GetFullscreen())
    {
        if (flip_needed)
        {
            flip_needed = false;
            if (config->GetUsePageFlip())
            {
                if (!BackBuffer)
                    BackBuffer->Restore();
                BackBuffer->BltFast(0, 0, OffscreenSurface, &screen_rect, DDBLTFAST_WAIT);
                if (!PrimarySurface)
                    PrimarySurface->Restore();
                PrimarySurface->Flip(BackBuffer, DDFLIP_WAIT);
            }
            else
            {
                if (!PrimarySurface)
                    PrimarySurface->Restore();
                PrimarySurface->BltFast(0, 0, OffscreenSurface, &screen_rect, DDBLTFAST_WAIT);
            }
            if (PrimarySurface->ErrorValue == DDERR_SURFACELOST || OffscreenSurface->ErrorValue == DDERR_SURFACELOST ||
                BackBuffer->ErrorValue == DDERR_SURFACELOST)
                Dx_Draw1->RestoreAllSurfaces();
        }
    }
    if (!config->GetFullscreen())
    {
        if (flip_needed)
        {
            flip_needed = false;
            TRect window_rect = BoundsRect;
            window_rect.top += 20;
            if (window_height_padding > 0)
                window_rect.top += window_height_padding;
            if (!PrimarySurface)
                PrimarySurface->Restore();
            PrimarySurface->Flip(OffscreenSurface, DDFLIP_WAIT);
            Dx_Draw1->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN);
            if (!OffscreenSurface)
                OffscreenSurface->Restore();
            PrimarySurface->Blt(&window_rect, OffscreenSurface, 0, DDBLT_WAIT, 0);
            if (PrimarySurface->ErrorValue == DDERR_SURFACELOST || OffscreenSurface->ErrorValue == DDERR_SURFACELOST)
                Dx_Draw1->RestoreAllSurfaces();
        }
    }
    if (item_drag_drawn)
        gfx->interface_graphics->RestoreItemDragBackground();
    if (spell_drag_drawn)
        gfx->interface_graphics->RestoreSpellDragBackground();
    if (skill_drag_drawn)
        gfx->interface_graphics->RestoreSkillDragBackground();
    frame_count++;
    int dw_milliseconds = 1;
    if (animation->GetScreenMode() == ScreenMode_Intro)
        dw_milliseconds = 10;
    Sleep((unsigned int)dw_milliseconds);
    done = false;
}

void __fastcall TMainForm::clientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
    int &ErrorCode)
{
    Socket->Close();
    ErrorCode = 0;
}

void __fastcall TMainForm::clientDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
    net_client->HandleDisconnect();
}

void __fastcall TMainForm::clientRead(TObject *Sender, TCustomWinSocket *Socket)
{
    net_client->ReceiveData(Socket, Socket->ReceiveText());
}

void __fastcall TMainForm::clientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
    net_client->SendInitRequest();
}

void __fastcall TMainForm::ApplicationEvents1Exception(TObject *Sender, Exception *E)
{
    String message = E->Message;
}
