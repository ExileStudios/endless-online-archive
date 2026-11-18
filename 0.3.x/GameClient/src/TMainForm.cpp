#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Gfx_effect.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
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
#include "Ctrl_hotkeys.h"
#include "Ctrl_notification.h"
#include "Gfx_global_npcs.h"
#include "Gfx_global_sprites.h"
#include "Gfx_mapnpcs.h"
#include <multimon.h>
#include "Ctrl_crafter.h"
#include "Ctrl_visualdoc.h"
#include "Ctrl_captcha.h"

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
    if (!initialized)
        return;
    else
    {
        mm_timer_id = 0;
        draw_error_count = 0;
        show_frame_rate = false;
        start_stamp = DateTimeToTimeStamp(Now());
        time_value_a = 0;
        time_value_b = 12;
        scale_menu = CreatePopupMenu();
        InsertMenuA(scale_menu, 0, MF_BYPOSITION, 170, String("Scale: 1x").c_str());
        InsertMenuA(scale_menu, 1, MF_BYPOSITION, 171, String("Scale: 2x").c_str());
        InsertMenuA(scale_menu, 2, MF_BYPOSITION, 172, String("Scale: 3x").c_str());
        InsertMenuA(scale_menu, 3, MF_BYPOSITION, 173, String("Scale: 4x").c_str());
        status_widget_menu = CreatePopupMenu();
        InsertMenuA(status_widget_menu, 0, MF_BYPOSITION, 174, String("Clock").c_str());
        InsertMenuA(status_widget_menu, 1, MF_BYPOSITION, 175, String("Counter").c_str());
        InsertMenuA(status_widget_menu, 2, MF_BYPOSITION, 176, String("Weight").c_str());
        InsertMenuA(status_widget_menu, 3, MF_BYPOSITION, 177, String("Coords").c_str());
        InsertMenuA(status_widget_menu, 4, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(status_widget_menu, 5, MF_BYPOSITION, 178, String("Reset Counter").c_str());
        minimap_menu = CreatePopupMenu();
        InsertMenuA(minimap_menu, 0, MF_BYPOSITION, 179, String("Minimap Toggled").c_str());
        InsertMenuA(minimap_menu, 1, MF_BYPOSITION, 180, String("Minimap Overlayer v2").c_str());
        InsertMenuA(minimap_menu, 2, MF_BYPOSITION, 181, String("Minimap Radar").c_str());
        InsertMenuA(minimap_menu, 3, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(minimap_menu, 4, MF_BYPOSITION, 182, String("Minimap Icons").c_str());
        InsertMenuA(minimap_menu, 5, MF_BYPOSITION, 183, String("Minimap Grid").c_str());
        auto_drop_menu = CreatePopupMenu();
        InsertMenuA(auto_drop_menu, 0, MF_BYPOSITION, 184, String("Drop bars Damage").c_str());
        InsertMenuA(auto_drop_menu, 1, MF_BYPOSITION, 185, String("Drop bars Recovery").c_str());
        InsertMenuA(auto_drop_menu, 2, MF_BYPOSITION, 186, String("Drop bars Exp").c_str());
        InsertMenuA(auto_drop_menu, 3, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(auto_drop_menu, 4, MF_BYPOSITION, 187, String("Elemental Icons").c_str());
        InsertMenuA(auto_drop_menu, 5, MF_BYPOSITION, 188, String("Extended Tooltips").c_str());
        InsertMenuA(auto_drop_menu, 6, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(auto_drop_menu, 7, MF_BYPOSITION, 189, String("Show Chat Balloons").c_str());
        InsertMenuA(auto_drop_menu, 8, MF_BYPOSITION, 190, String("Show Balloon Names").c_str());
        InsertMenuA(auto_drop_menu, 9, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(auto_drop_menu, 10, MF_BYPOSITION, 191, String("Show Grid Cursor").c_str());
        InsertMenuA(auto_drop_menu, 11, MF_BYPOSITION, 192, String("Refresh (F12)").c_str());
        InsertMenuA(auto_drop_menu, 12, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(auto_drop_menu, 13, MF_BYPOSITION, 193, String("Wasd movement").c_str());
        InsertMenuA(auto_drop_menu, 14, MF_BYPOSITION, 194, String("Always ghost npc").c_str());
        HMENU system_menu = GetSystemMenu(Handle, 0);
        InsertMenuA(system_menu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenuA(system_menu, 6, MF_POPUP | MF_BYPOSITION, (unsigned int)scale_menu, String("Size").c_str());
        InsertMenuA(system_menu, 6, MF_POPUP | MF_BYPOSITION, (unsigned int)status_widget_menu,
            String("Widget").c_str());
        InsertMenuA(system_menu, 6, MF_POPUP | MF_BYPOSITION, (unsigned int)minimap_menu, String("Minimap").c_str());
        InsertMenuA(system_menu, 6, MF_POPUP | MF_BYPOSITION, (unsigned int)auto_drop_menu, String("Game").c_str());
    }
}

void __fastcall TMainForm::FormShow(TObject *Sender)
{
    if (!initialized)
    {
        ((TCustomForm *)this)->Close();
        return;
    }
    UpdateSystemMenu();
}

// Checks the data files, then loads the settings and texts, starts DirectInput, DirectSound and DirectDraw, builds
// every controller and starts the timers.
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    initialized = false;
    minimized = false;
    login_screen_delay = 0;
    active = false;
    version_major = 0;
    version_minor = 3;
    version_build = 29;
    version_letter = 'K';
    min_theme_version_major = 0;
    min_theme_version_minor = 3;
    files = new FileController();
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
        files_message += "Some files are missing, blocked or corrupted\n\n";
        files_message += files->error_message + "\n\n";
        if (files->error_kind == "V3")
        {
            files_message += "Missing sound path or files detected.\n";
            files_message += "Make sure the following files are present:\n\n";
            files_message += "folder: \\sfx\\client\\\n";
            files_message += "folder: \\sfx\\ctrl\\\n";
            files_message += "folder: \\sfx\\effect\\\n";
            files_message += "folder: \\sfx\\instr\\\n\n";
        }
        files_message += "1) Please, do not move Endless (endless.exe) outside \n";
        files_message += "of it's program folder.\n\n";
        files_message += "2) The file could potentially be blocked. Try to allow\n";
        files_message += "the file inside your virus scanner or Windows Defender.\n\n";
        files_message += "3) If this message is repeated consider to completely uninstall\n";
        files_message += "Endless Online and re-install the correct client version.\n";
        files_message += "Goto: http://www.endless-online.com";
        if (MessageDlg(files_message, mtError, TMsgDlgButtons() << mbAbort << mbIgnore, 0) == mrAbort)
        {
            delete files;
            delete config;
            delete strings;
            delete registry;
            ((TCustomForm *)this)->Close();
            return;
        }
    }
    config = new Config();
    themes = new FileThemes(String(".\\gfx\\themes\\"), min_theme_version_major, min_theme_version_minor);
    strings = new StringContainer(config->GetLanguage());
    chat_log = new FFileLog(config->GetLogChat(), config->GetChatLogPath());
    registry = new WinRegister();
    fonts = new Fonts(true);
    gfx_vault = new GfxVault();
    gfx_transfers = new GfxTransfers();
    if (themes->LoadTheme(config->GetThemeFile()))
    {
        if (themes->is_outdated)
        {
            String theme_message = title;
            theme_message += "Installed theme [" + themes->theme_name + " ] is not up-to-date.\n";
            theme_message += "Suggestion: fix or change theme using configuration tool.\n\n";
            theme_message +=
                "Min. needed theme version: " + String(min_theme_version_major) + "." +
                    String(min_theme_version_minor) + "\n";
            theme_message +=
                "Theme target version: " + String(themes->target_major_version) + "." +
                    String(themes->target_minor_version);
            MessageDlg(theme_message, mtInformation, TMsgDlgButtons() << mbOK, 0);
        }
        gfx_vault->SetGfxFileName(GfxLibrary_MenuInterface, themes->first_graphics_path);
        gfx_vault->SetGfxFileName(GfxLibrary_GameInterface, themes->second_graphics_path);
    }
    Application->OnMinimize = MinimizeForm;
    Application->OnRestore = RestoreForm;
    Application->OnActivate = ActivateForm;
    Application->OnDeactivate = DeactivateForm;
    // Timer1Timer later counts its ticks in this member; every use of it as a width comes first.
    display_width = 640;
    display_height = 480;
    window_height_padding = 0;
    unused_value = 0;
    Width = display_width;
    Height = display_height;
    TPoint window_position = config->GetWindowPosition();
    if (window_position.x < 0)
    {
        if (window_position.x == -2)
            Position = poDesktopCenter;
        else
            Position = poScreenCenter;
    }
    else
    {
        Position = poDesigned;
        Left = window_position.x;
        Top = window_position.y;
    }
    char start_fullscreen = config->GetFullscreen();
    config->SetFullscreen(false);
    if (Dx_Input1->Created)
        Dx_Input1->Destroy();
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
                    Dx_Sound1->Destroy();
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
        Dx_Draw1->Destroy();
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
        directx_message += "Suggestion: Read the troubleshooting:\n\n";
        directx_message += "Troubleshoot-1: finish pending Windows Update(s)\n";
        directx_message += "Troubleshoot-2: reboot your system and try again.\n";
        directx_message += "Troubleshoot-3: (re)install directX runtime package.\n\n";
        directx_message += "If all fails download 'DDrawCompat', this is a ddraw.dll file\n";
        directx_message += "that you should place in the Endless.exe folder, and try again.";
        if (MessageDlg(directx_message, mtError, TMsgDlgButtons() << mbAbort << mbIgnore, 0) == mrAbort)
        {
            Dx_Input1->Destroy();
            Dx_Sound1->Destroy();
            Application->Terminate();
            return;
        }
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
    String pub_path = ".\\pub\\";
    players = new Players(config->GetPreloadCharacterGfx());
    npcs = new Npcs(config->GetPreloadCharacterGfx());
    effects = new Effects();
    map_items = new MapItems();
    item_values = new ItemValues(pub_path, false);
    npc_values = new NpcValues(pub_path, false);
    skill_values = new SkillValues(pub_path, false);
    class_values = new ClassValues(pub_path, false);
    edits = new Edits(themes->color_edits);
    edit_canvas = new EditCanvas();
    gfx->CreateMenuGraphics();
    gfx->LoadMenuGraphics(config->GetSkipSoftpad());
    gfx->LoadMainMenuScreen(true);
    gfx->LoadSpriteGraphics();
    edit_canvas->LoadResource();
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
        if (config->GetForceClientSize())
        {
            ClientWidth = 640 * config->GetScale();
            ClientHeight = 480 * config->GetScale();
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
    int sound_buffers = config->GetSoundBuffers();
    config->SetSoundBuffers(sound_buffers + 1);
    client->Port = config->GetPort();
    mm_timer_id = 0;
    mm_timer_id =
        timeSetEvent(6, 0,
            (void __stdcall (*)(unsigned int, unsigned int, unsigned long, unsigned long, unsigned long))MMTimerProc, 0,
            TIME_PERIODIC);
    Timer1->Enabled = true;
    initialized = true;
    randomize();
    time(&start_time);
}

// Ticks the window menu's items to match the settings: the minimap mode, its icons and grid, the bars that drop by
// themselves and the rest.
void TMainForm::UpdateSystemMenu()
{
    if (initialized)
    {
        if (!config->GetMinimapMode())
            CheckMenuRadioItem(minimap_menu, 179, 181, 179, MF_BYCOMMAND);
        if (config->GetMinimapMode() == 1)
            CheckMenuRadioItem(minimap_menu, 179, 181, 180, MF_BYCOMMAND);
        if (config->GetMinimapMode() == 2)
            CheckMenuRadioItem(minimap_menu, 179, 181, 181, MF_BYCOMMAND);
        if (config->GetMinimapIcons())
            CheckMenuItem(minimap_menu, 182, MF_CHECKED);
        else
            CheckMenuItem(minimap_menu, 182, MF_UNCHECKED);
        if (config->GetMinimapGrid())
            CheckMenuItem(minimap_menu, 183, MF_CHECKED);
        else
            CheckMenuItem(minimap_menu, 183, MF_UNCHECKED);
        if (config->GetAutoDropDamageBar())
            CheckMenuItem(auto_drop_menu, 184, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 184, MF_UNCHECKED);
        if (config->GetAutoDropRecoverBar())
            CheckMenuItem(auto_drop_menu, 185, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 185, MF_UNCHECKED);
        if (config->GetAutoDropExpBar())
            CheckMenuItem(auto_drop_menu, 186, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 186, MF_UNCHECKED);
        if (config->GetElementalIcons())
            CheckMenuItem(auto_drop_menu, 187, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 187, MF_UNCHECKED);
        if (config->GetExtendedTooltip())
            CheckMenuItem(auto_drop_menu, 188, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 188, MF_UNCHECKED);
        if (config->GetShowBalloons())
            CheckMenuItem(auto_drop_menu, 189, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 189, MF_UNCHECKED);
        if (config->GetShowBalloonName())
            CheckMenuItem(auto_drop_menu, 190, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 190, MF_UNCHECKED);
        if (config->GetShowGridCursor())
            CheckMenuItem(auto_drop_menu, 191, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 191, MF_UNCHECKED);
        if (config->GetWasdKeys())
            CheckMenuItem(auto_drop_menu, 193, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 193, MF_UNCHECKED);
        if (config->GetAlwaysGhost())
            CheckMenuItem(auto_drop_menu, 194, MF_CHECKED);
        else
            CheckMenuItem(auto_drop_menu, 194, MF_UNCHECKED);
        if (!config->GetStatusWidget())
            CheckMenuRadioItem(status_widget_menu, 174, 177, 174, MF_BYCOMMAND);
        if (config->GetStatusWidget() == 1)
            CheckMenuRadioItem(status_widget_menu, 174, 177, 175, MF_BYCOMMAND);
        if (config->GetStatusWidget() == 2)
            CheckMenuRadioItem(status_widget_menu, 174, 177, 176, MF_BYCOMMAND);
        if (config->GetStatusWidget() == 3)
            CheckMenuRadioItem(status_widget_menu, 174, 177, 177, MF_BYCOMMAND);
        if (config->GetScale() == 1)
            CheckMenuRadioItem(scale_menu, 170, 173, 170, MF_BYCOMMAND);
        if (config->GetScale() == 2)
            CheckMenuRadioItem(scale_menu, 170, 173, 171, MF_BYCOMMAND);
        if (config->GetScale() == 3)
            CheckMenuRadioItem(scale_menu, 170, 173, 172, MF_BYCOMMAND);
        if (config->GetScale() == 4)
            CheckMenuRadioItem(scale_menu, 170, 173, 173, MF_BYCOMMAND);
    }
}

// The window menu's own items: the scale from 1 to 4, the status widget, the minimap mode, and toggles for the minimap
// icons and grid, the bars that drop by themselves and the elemental icons.
void __fastcall TMainForm::WMSysCommand(TWMSysCommand &message)
{
    if (!initialized)
        TObject::Dispatch(&message);
    if (message.CmdType == 173)
    {
        config->SetScale(4);
        if (config->GetForceClientSize())
        {
            ClientWidth = 640 * config->GetScale();
            ClientHeight = 480 * config->GetScale();
        }
        else
        {
            Width = 640 * config->GetScale();
            Height = 496 * config->GetScale();
        }
    }
    if (message.CmdType == 172)
    {
        config->SetScale(3);
        if (config->GetForceClientSize())
        {
            ClientWidth = 640 * config->GetScale();
            ClientHeight = 480 * config->GetScale();
        }
        else
        {
            Width = 640 * config->GetScale();
            Height = 496 * config->GetScale();
        }
    }
    if (message.CmdType == 171)
    {
        config->SetScale(2);
        if (config->GetForceClientSize())
        {
            ClientWidth = 640 * config->GetScale();
            ClientHeight = 480 * config->GetScale();
        }
        else
        {
            Width = 640 * config->GetScale();
            Height = 496 * config->GetScale();
        }
    }
    if (message.CmdType == 170)
    {
        config->SetScale(1);
        if (config->GetForceClientSize())
        {
            ClientWidth = 640 * config->GetScale();
            ClientHeight = 480 * config->GetScale();
        }
        else
        {
            Width = 640;
            Height = window_height_padding + 500;
            if (Height > 512)
                Height = 512;
        }
    }
    // 174 to 177 pick the status widget and 179 to 181 the minimap mode; from 182 on, each item
    // toggles a setting.
    if (message.CmdType == 174)
        config->SetStatusWidget(0);
    if (message.CmdType == 175)
        config->SetStatusWidget(1);
    if (message.CmdType == 176)
        config->SetStatusWidget(2);
    if (message.CmdType == 177)
        config->SetStatusWidget(3);
    if (message.CmdType == 178)
        time(&start_time);
    if (message.CmdType == 179)
        config->SetMinimapMode(MinimapMode_Toggled);
    if (message.CmdType == 180)
        config->SetMinimapMode(MinimapMode_Overlay);
    if (message.CmdType == 181)
        config->SetMinimapMode(MinimapMode_Radar);
    if (message.CmdType == 182)
        config->SetMinimapIcons(!config->GetMinimapIcons());
    if (message.CmdType == 183)
        config->SetMinimapGrid(!config->GetMinimapGrid());
    if (message.CmdType == 184)
        config->SetAutoDropDamageBar(!config->GetAutoDropDamageBar());
    if (message.CmdType == 185)
        config->SetAutoDropRecoverBar(!config->GetAutoDropRecoverBar());
    if (message.CmdType == 186)
        config->SetAutoDropExpBar(!config->GetAutoDropExpBar());
    if (message.CmdType == 187)
        config->SetElementalIcons(!config->GetElementalIcons());
    if (message.CmdType == 188)
        config->SetExtendedTooltip(!config->GetExtendedTooltip());
    if (message.CmdType == 189)
        config->SetShowBalloons(!config->GetShowBalloons());
    if (message.CmdType == 190)
        config->SetShowBalloonName(!config->GetShowBalloonName());
    if (message.CmdType == 191)
        config->SetShowGridCursor(!config->GetShowGridCursor());
    if (message.CmdType == 192)
        net_client->sender->RequestRefresh();
    if (message.CmdType == 193)
        config->SetWasdKeys(!config->GetWasdKeys());
    if (message.CmdType == 194)
        config->SetAlwaysGhost(!config->GetAlwaysGhost());
    UpdateSystemMenu();
    TObject::Dispatch(&message);
}

// Logs out when in the game, closes the connection and the chat log, stops the sound and frees DirectX and the
// canvases.
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    if (!initialized)
        return;
    if (animation->GetScreenMode() == ScreenMode_Game)
        animation->interface_controller->Logout();
    initialized = false;
    if (client->Active)
        ((TAbstractSocket *)client)->Close();
    if (chat_log)
        chat_log->Close();
    Dx_SoundBuffer1->Stop();
    Dx_SoundBuffer2->Stop();
    Dx_SoundBuffer3->Stop();
    Dx_SoundBuffer4->Stop();
    Dx_SoundBuffer5->Stop();
    Dx_SoundBuffer6->Stop();
    Dx_SoundBuffer7->Stop();
    int wait_count = 0;
    // Each sound buffer is given up to 1000 short waits to stop playing.
    while (Dx_SoundBuffer1->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    while (Dx_SoundBuffer2->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    while (Dx_SoundBuffer3->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    while (Dx_SoundBuffer4->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    while (Dx_SoundBuffer5->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    while (Dx_SoundBuffer6->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    while (Dx_SoundBuffer7->Playing)
    {
        Sleep(1);
        wait_count++;
        if (wait_count > 1000)
            break;
    }
    if (Dx_SoundBuffer1->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_SoundBuffer2->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_SoundBuffer3->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_SoundBuffer4->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_SoundBuffer5->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_SoundBuffer6->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_SoundBuffer7->Created)
        Dx_SoundBuffer1->Destroy();
    if (Dx_Input1->Created)
        Dx_Input1->Destroy();
    if (Dx_Sound1->Created)
        Dx_Sound1->Destroy();
    if (Dx_Draw1->Created && config->GetFullscreen())
        Dx_Draw1->FlipToGDISurface();
    if (Dx_Draw1->Created)
        Dx_Draw1->Destroy();
    gfx_vault->CloseAllLibraries();
    delete fonts;
    delete gfx_vault;
    delete gfx_transfers;
    delete chat_log;
}

// Logs out when in game, then closes the connection, the chat log, the fast timer and DirectX.
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    if (initialized)
    {
        Dx_SoundBuffer1->Stop();
        Dx_SoundBuffer2->Stop();
        Dx_SoundBuffer3->Stop();
        Dx_SoundBuffer4->Stop();
        Dx_SoundBuffer5->Stop();
        Dx_SoundBuffer6->Stop();
        Dx_SoundBuffer7->Stop();
        Timer1->Enabled = false;
        timeKillEvent((unsigned int)mm_timer_id);
        Sleep(200);
    }
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

// F10 does nothing; Alt+F4 on the menu screens frees everything and quits, and in the game asks first.
void __fastcall TMainForm::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_F10)
        Key = 0;
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
            gfx->npc_cache->ReleaseGraphics();
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
    if (Shift.Contains(ssAlt))
        Key = 0;
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
    if (initialized)
    {
        if (Dx_InputDevice1->Created)
            Dx_InputDevice1->Destroy();
        Dx_InputDevice1->Create(GUID_SysKeyboard, Dx_Input1);
        if (keyboard->WasDeletePressed() || config->GetForceBuffer())
            full_paint_delay = 2;
        active = true;
    }
}

void __fastcall TMainForm::DeactivateForm(TObject *sender)
{
    if (Dx_InputDevice1->Created)
        Dx_InputDevice1->Destroy();
    active = false;
}

void __fastcall TMainForm::ApplicationEvents1Activate(TObject *Sender)
{
    if (initialized)
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

// Scales the cursor in a window to game coordinates and hands it to the mouse.
void __fastcall TMainForm::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    int mouse_x = X;
    if (initialized)
    {
        if (!config->GetFullscreen())
        {
            double scale_x = ClientWidth / 640.0;
            double scale_y = ClientHeight / BASE_SCREEN_HEIGHT;
            double scaled_x = mouse_x / scale_x;
            double scaled_y = Y / scale_y;
            mouse_x = scaled_x;
            Y = scaled_y;
        }
        mouse->SetPosition(mouse_x, Y);
    }
}

// Scales a click in a window to game coordinates, focuses the edit box under it, then hands the click to the mouse.
void __fastcall TMainForm::FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (!config->GetFullscreen())
    {
        double scale_x = ClientWidth / 640.0;
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

// Loads pending graphics in turns, plays queued music, drops distant players, items and npcs, and purges graphics not
// used lately. The tick count is kept in display_width: the turns follow its remainder by four, the music and the
// screenshot key come every twelfth tick, the drops every twentieth, and the purge with the reset above 48.
void __fastcall TMainForm::Timer1Timer(TObject *Sender)
{
    // Graphics load on three ticks of four; the music is checked every 12th, and players gone
    // from view are dropped every 20th.
    if (display_width % 4 == 1)
    {
        players->LoadPendingGraphics();
        if (!config->GetDebug())
        {
            gfx->tiles->LoadPendingGraphic();
            gfx->tops->LoadPendingGraphic();
            gfx->blends->LoadPendingGraphic();
            gfx->items->LoadPendingGraphics();
            gfx->npc_cache->LoadPendingGraphic();
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
    if (display_width % 12 == 0)
    {
        if (sfx->TakeMusicPending())
            sfx->PlayMusic(sfx->GetQueuedMusic());
        if (!sfx->IsMusicPlaying() && sfx->IsMusicRetryReady())
        {
            sfx->StartMusicRetryDelay();
            if (map->GetMusicMode() > 2 && map->GetMusicMode() < 6)
                sfx->QueueMusic(map->GetMusicId());
        }
        // Print Screen saves a screenshot.
        if (GetAsyncKeyState(VK_SNAPSHOT) != 0)
            gfx->interface_graphics->SaveScreenshot();
    }
    if (display_width % SLOW_UPDATE_INTERVAL == 0)
    {
        players->RemoveExpiredPlayers(map->GetCenterX(), map->GetCenterY());
        map_items->RemoveDistantItems(map->GetCenterX(), map->GetCenterY());
        npcs->RemoveExpiredNpcs();
    }
    if (display_width > 48)
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
        gfx->npc_cache->PurgeUnusedGraphics();
        gfx_vault->AgeLibraries();
        display_width = 0;
    }
    RenderMainLoop();
    display_width++;
}

// The fast timer's tick: the keyboard on every tick, then animation, controllers and sound on their own periods.
void __fastcall TMainForm::HandleMMTimerEvent()
{
    timer_ticks++;
    keyboard->PollKeyboard();
    // The frame rate is the frames drawn over the last two hundred ticks.
    if (timer_ticks >= 200)
    {
        frame_rate = frame_count;
        timer_ticks = 0;
        frame_count = 0;
    }
    // Every tenth tick the timers; every twentieth the animations, the path, the mouse and the windows.
    if (timer_ticks % 10 == 0)
        animation->UpdateTimers();
    if (timer_ticks % 20 == 0)
    {
        animation->interface_controller->UpdateScreenEffects();
        map->path_finder->FollowPath();
        players->Animate();
        effects->Animate();
        npcs->Animate();
        map_items->UpdateHoverLabel();
        map->Animate();
        mouse->HandleMove();
        if (animation->captcha)
            animation->captcha->Update();
        if (animation->visual_doc)
            animation->visual_doc->Tick();
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
        if (animation->crafter)
            animation->crafter->Update();
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
        if (animation->notification)
            animation->notification->Tick();
    }
    if (timer_ticks % 20 == 1)
        animation_tick_due = true;
    if (timer_ticks % 40 == 0)
    {
        sfx->UpdateSounds();
        if (animation->IsAnimating())
            animation->UpdateIntroAndDelays();
        effects->RemoveFinished();
    }
    if (timer_ticks % 100 == 0)
    {
        edits->TimerTick();
        animation->interface_controller->OnTimer();
        map->AdvanceTileFrame();
    }
}

void TMainForm::IntroAnimator()
{
}

void TMainForm::RenderMainLoop()
{
    if (animation->GetScreenMode() == ScreenMode_Game)
        RenderMainLoop_Game();
    if (animation->GetScreenMode() == ScreenMode_Menu)
        RenderMainLoop_Menu();
    if (animation->GetScreenMode() == ScreenMode_Intro)
        RenderMainLoop_Intro();
    RenderFpsOverlay();
    RenderDrag();
    RenderOffscreenToPrimary();
    RenderDragUndo();
    frame_count++;
}

void TMainForm::RenderMainLoop_Game()
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
            RenderGame_MapLoadScreen();
        else
        {
            if (map->ConsumeCursorMoved() || npcs->ConsumeRedraw() || players->ConsumeRedraw() || redraw_needed ||
                edits->ConsumeTextChanged())
                RenderGame_ViewPort_Frame();
            RenderGame_BottomPanel();
        }
    }
}

// Draws the game view: the ground, the shadows and the map's rows with their people and effects, then the own
// character, the screen tints, the minimap, the status icons and every open window and box.
void TMainForm::RenderGame_ViewPort_Frame()
{
    bool loop_entered = false;
    int j;
    int m;
    int n;
    int kk;
    int i;
    int ii;
    int jj;
    int item_graphic;
    int object_spec;
    int player_index;
    int npc_index;
    int effect_index;
    int item_index;
    int own_player_index;
    char own_has_weapon;
    char own_has_shield;
    int shake_offset;
    int first_x;
    int last_x;
    int first_y;
    int last_y;
    int ground_x;
    int ground_y;
    TileWrapper *ground_tile;
    int blend_y;
    int shadow_x;
    int shadow_y;
    BlendWrapper *blend;
    redraw_needed = false;
    player_index = 0;
    npc_index = 0;
    effect_index = 0;
    item_index = 0;
    own_player_index = -1;
    own_has_weapon = false;
    own_has_shield = false;
    shake_offset = 0;
    // A screen shake moves everything drawn by its current offset.
    if (animation->interface_controller->is_shaking)
        shake_offset = animation->interface_controller->shake_offset;
    OffscreenSurface->Blt(&gfx->game->view_clip_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, gfx->game->view_fill_fx);
    // The tiles around the centre; the rows run on to the bottom of the map.
    first_x = map->GetCenterX() - 28;
    last_x = map->GetCenterX() + 28;
    first_y = map->GetCenterY() - 28;
    last_y = map->GetHeight();
    if (first_x < 0)
        first_x = 0;
    if (first_y < 0)
        first_y = 0;
    if (map->GetWidth() < last_x)
        last_x = map->GetWidth();
    blitter->ResetDarkness(255, 255);
    for (i = first_y; i <= last_y; i++)
    {
        loop_entered = true;
        for (j = first_x; j <= last_x; j++)
        {
            loop_entered = true;
            ground_x = map->GetScrollOffsetX() + (32 * j - 32 * i) + 278;
            ground_y = map->GetScrollOffsetY() + (16 * j + 16 * i) + 134;
            if (shake_offset)
                ground_x += shake_offset;
            if (map->ground_tiles[j][i] >= 1 && net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), j, i))
            {
                if (gfx->tiles->RequestGraphic(map->ground_tiles[j][i]))
                {
                    ground_tile = gfx->tiles->GetGraphic(map->ground_tiles[j][i]);
                    if (ground_tile)
                    {
                        gfx->game->BlitClipped(ground_x, ground_y, ground_tile->resource->surface,
                            ground_tile->resource->GetFrameRect(map->tile_frame));
                        if (ground_tile->resource->is_animated)
                            MainForm->map->has_animated_tiles = true;
                    }
                }
                else
                    redraw_needed = true;
            }
        }
    }
    // Shadows, when set, go over the ground and under everything else.
    if (config->GetShowShadows())
    {
        for (blend_y = first_y; blend_y <= last_y; blend_y++)
        {
            loop_entered = true;
            for (m = first_x; m <= last_x; m++)
            {
                loop_entered = true;
                shadow_x = map->GetScrollOffsetX() + (32 * m - 32 * blend_y) + 278;
                shadow_y = map->GetScrollOffsetY() + (16 * m + 16 * blend_y) + 134;
                if (shake_offset)
                    shadow_x += shake_offset;
                if (map->blend_tiles[m][blend_y] >= 1)
                {
                    if (net_client->IsObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), m, blend_y))
                    {
                        if (gfx->blends->RequestGraphic(map->blend_tiles[m][blend_y]))
                        {
                            blend = gfx->blends->GetGraphic(map->blend_tiles[m][blend_y]);
                            if (blend)
                                gfx->game->BlitClippedAlpha(shadow_x - 24, shadow_y - 12, blend->resource->surface,
                                    blend->resource->surface_rect, 48);
                        }
                    }
                }
            }
        }
    }
    for (n = first_y - 1; n <= last_y; n++)
    {
        loop_entered = true;
        for (ii = first_x; ii <= last_x; ii++)
        {
            loop_entered = true;
            int screen_x;
            screen_x = map->GetScrollOffsetX() + (32 * ii - 32 * n) + 278;
            int screen_y;
            screen_y = map->GetScrollOffsetY() + (16 * ii + 16 * n) + 134;
            if (shake_offset)
                screen_x += shake_offset;
            if (n >= 0)
            {
                if (map->HasPathMarker() && ii == map->GetPathMarkerX() && n == map->GetPathMarkerY())
                {
                    if (map->GetPathMarkerTimer() > 4)
                        gfx->game->BlitClippedAlpha(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                            gfx->interface_graphics->GetWideCellRect(3), 24 * map->GetPathMarkerTimer() + 12);
                    else
                        gfx->game->BlitClippedAlpha(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                            gfx->interface_graphics->GetWideCellRect(4), 24 * map->GetPathMarkerTimer() + 12);
                }
                if (ii == map->GetCursorX() && n == map->GetCursorY() && config->GetShowGridCursor() &&
                    !map->blocked[ii][n] && map->spec_grid[ii][n] != 18 &&
                    (!animation->game->player_menu_visible || animation->game->player_menu_hovered_option < 0))
                {
                    if (MainForm->map_items->HasItemAt(ii, n))
                        gfx->game->BlitClipped(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                            gfx->interface_graphics->GetWideCellRect(2));
                    else if (map->has_tile_spec[ii][n] || players->HasVisiblePlayerAt(ii, n) || npcs->HasNpcAt(ii, n))
                        gfx->game->BlitClipped(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                            gfx->interface_graphics->GetWideCellRect(1));
                    else
                        gfx->game->BlitClipped(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                            gfx->interface_graphics->GetWideCellRect(0));
                }
                if ((int)MainForm->map_items->GetCount() > item_index)
                {
                    char item_found;
                    do
                    {
                        loop_entered = true;
                        item_found = false;
                        MapItem map_item = map_items->GetItem(item_index);
                        if (ii == map_item.x && n == map_item.y)
                        {
                            item_graphic = item_values->GetGraphic(map_item.item_id) - 1;
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
                                if (net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                                {
                                    if (gfx->items->RequestGraphic(item_graphic))
                                    {
                                        GraphicSize item_size = gfx->items->GetGraphicSize(item_graphic);
                                        int item_x;
                                        int item_y;
                                        ItemWrapper *item_sprite;
                                        item_x = screen_x + 32 - item_size.width / 2;
                                        item_y = screen_y + 16 - item_size.height / 2;
                                        item_sprite = gfx->items->GetGraphic(item_graphic);
                                        if (item_sprite)
                                            gfx->game->BlitClipped(item_x, item_y, item_sprite->resource->surface,
                                                item_sprite->resource->surface_rect);
                                    }
                                    else
                                        redraw_needed = true;
                                }
                            }
                            item_found = true;
                            item_index++;
                        }
                    } while ((int)MainForm->map_items->GetCount() > item_index && item_found);
                }
                if (map->overlay_tiles[ii][n] >= 1 &&
                    net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n) &&
                    gfx->tiles->RequestGraphic(map->overlay_tiles[ii][n]))
                {
                    TileWrapper *raised_tile;
                    raised_tile = gfx->tiles->GetGraphic(map->overlay_tiles[ii][n]);
                    if (raised_tile)
                        gfx->game->BlitClipped(screen_x, screen_y - 32, raised_tile->resource->surface,
                            raised_tile->resource->surface_rect);
                }
                if (map->objects[ii][n] >= 1 &&
                    net_client->IsTallObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                {
                    char draw_object = true;
                    object_spec = map->GetTileSpec(ii, n);
                    if (object_spec == TileSpec_Trap && map->trap_reveal_timer < 1)
                        draw_object = false;
                    if (object_spec == TileSpec_HiddenSpikes && players->FindPlayerAt(ii, n) < PlayerId_Unset)
                        draw_object = false;
                    if (draw_object)
                    {
                        if (gfx->objects->RequestGraphic(map->objects[ii][n]))
                        {
                            GraphicSize object_size = gfx->objects->GetGraphicSize(map->objects[ii][n]);
                            int object_x;
                            int object_y;
                            ObjectWrapper *object_sprite;
                            object_x = screen_x + 30 - object_size.width / 2;
                            object_y = screen_y + 30 - object_size.height;
                            object_sprite = gfx->objects->GetGraphic(map->objects[ii][n]);
                            if (object_sprite)
                                gfx->game->BlitClipped(object_x, object_y, object_sprite->resource->surface,
                                    object_sprite->resource->surface_rect);
                        }
                        else
                            redraw_needed = true;
                    }
                }
                // The lists are sorted by position, so each one is walked in step with the tiles.
                if ((int)MainForm->players->GetCount() > player_index)
                {
                    char player_found;
                    do
                    {
                        loop_entered = true;
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
                        if (ii == player.x && n == player.y)
                        {
                            // A hidden player is drawn only for themselves and for admin levels above 3.
                            if (!player.hidden || net_client->GetPlayerId() == player.player_id ||
                                MainForm->net_client->GetAdminLevel() > HIDDEN_VIEW_ADMIN_LEVEL)
                            {
                                if (player.effect_running && player.effect_base_layer &&
                                    gfx->effects->RequestGraphic(player.effect_id))
                                {
                                    GraphicSize behind_effect_size = gfx->effects->GetGraphicSize(player.effect_id);
                                    int behind_effect_x;
                                    int effect_frame;
                                    EffectWrapper *loop_effect;
                                    EffectWrapper *back_effect;
                                    behind_effect_x =
                                        32 * player.x - 32 * player.y + MainForm->map->GetScrollOffsetX() + 309;
                                    int behind_effect_y =
                                        16 * player.x + 16 * player.y + MainForm->map->GetScrollOffsetY() + 120;
                                    behind_effect_x -= behind_effect_size.width / 2;
                                    behind_effect_y = behind_effect_y - behind_effect_size.height / 2;
                                    if (player.walking)
                                    {
                                        if (player.walk_direction == FacingDirection_Left)
                                        {
                                            behind_effect_x = behind_effect_x + 32 - (8 * player.walk_frame + 8);
                                            behind_effect_y = behind_effect_y + 16 - (4 * player.walk_frame + 4);
                                        }
                                        if (player.walk_direction == FacingDirection_Up)
                                        {
                                            behind_effect_x = 8 * player.walk_frame + behind_effect_x - 24;
                                            behind_effect_y = behind_effect_y + 16 - (4 * player.walk_frame + 4);
                                        }
                                        if (player.walk_direction == FacingDirection_Right)
                                        {
                                            behind_effect_x = 8 * player.walk_frame + behind_effect_x - 24;
                                            behind_effect_y = behind_effect_y + 4 * player.walk_frame - 12;
                                        }
                                        if (player.walk_direction == FacingDirection_Down)
                                        {
                                            behind_effect_x = behind_effect_x + 32 - (8 * player.walk_frame + 8);
                                            behind_effect_y = behind_effect_y + 4 * player.walk_frame - 12;
                                        }
                                    }
                                    if (player.effect_id == SpellEffect_FireBall)
                                        behind_effect_y = behind_effect_y - (160 - 30 * player.effect_frame);
                                    if (player.effect_id == SpellEffect_IceBall)
                                        behind_effect_y = behind_effect_y - (160 - 30 * player.effect_frame);
                                    if (player.effect_id == SpellEffect_EnergyBall)
                                        behind_effect_y = behind_effect_y - 10 * player.effect_frame;
                                    if (player.effect_id == SpellEffect_Boulders)
                                        behind_effect_y = behind_effect_y - (160 - 30 * player.effect_frame);
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
                                            behind_effect_y = behind_effect_y + 14;
                                        if (player.effect_frame == 2)
                                            behind_effect_x += 20;
                                        if (player.effect_frame == 3)
                                            behind_effect_y -= 14;
                                    }
                                    if (player.effect_id == SpellEffect_DarkBeam ||
                                        player.effect_id == SpellEffect_Heaven || player.effect_id == SpellEffect_Aura)
                                    {
                                        effect_frame = player.effect_loops_left;
                                        if (effect_frame > 0)
                                            effect_frame = random(2) + 3;
                                        loop_effect = gfx->effects->GetGraphic(player.effect_id);
                                        if (loop_effect)
                                            gfx->game->BlitClipped(behind_effect_x + shake_offset, behind_effect_y,
                                                loop_effect->resource->layer0_surface,
                                                loop_effect->resource->GetFrameRect(effect_frame));
                                    }
                                    else
                                    {
                                        back_effect = gfx->effects->GetGraphic(player.effect_id);
                                        if (back_effect)
                                            gfx->game->BlitClipped(behind_effect_x + shake_offset, behind_effect_y,
                                                back_effect->resource->layer0_surface,
                                                back_effect->resource->GetFrameRect(player.effect_frame));
                                    }
                                }
                                if (player.dying || player.hiding || player.unhiding || player.hidden)
                                    gfx->sprites->DrawPlayer(&player, true, shake_offset);
                                else
                                    gfx->sprites->DrawPlayer(&player, false, shake_offset);
                                if (player.attacking && player.attack_frame >= 2 && player.weapon > 0)
                                {
                                    int weapon_effect_x;
                                    weapon_effect_x =
                                        32 * player.x - 32 * player.y + MainForm->map->GetScrollOffsetX() + 267;
                                    int weapon_effect_y;
                                    weapon_effect_y =
                                        16 * player.x + 16 * player.y + MainForm->map->GetScrollOffsetY() + 100;
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
                                        int weapon_effect_type = weapon_mapper->weapon_effect_types[player.weapon];
                                        gfx->game->BlitClippedHalfBlend(weapon_effect_x, weapon_effect_y,
                                            gfx->interface_graphics->weapon_effects,
                                            gfx->interface_graphics->GetCellRect(weapon_effect_type, player.direction));
                                    }
                                }
                                if (player.effect_running && (player.effect_blend_layer || player.effect_solid_layer) &&
                                    gfx->effects->RequestGraphic(player.effect_id))
                                {
                                    GraphicSize front_effect_size = gfx->effects->GetGraphicSize(player.effect_id);
                                    int front_effect_x;
                                    int front_effect_frame;
                                    int flicker_roll;
                                    EffectWrapper *front_loop_effect;
                                    EffectWrapper *front_effect;
                                    EffectWrapper *top_effect;
                                    front_effect_x =
                                        32 * player.x - 32 * player.y + MainForm->map->GetScrollOffsetX() + 309;
                                    int front_effect_y =
                                        16 * player.x + 16 * player.y + MainForm->map->GetScrollOffsetY() + 120;
                                    front_effect_x -= front_effect_size.width / 2;
                                    front_effect_y = front_effect_y - front_effect_size.height / 2;
                                    if (player.walking)
                                    {
                                        if (player.walk_direction == FacingDirection_Left)
                                        {
                                            front_effect_x = front_effect_x + 32 - (8 * player.walk_frame + 8);
                                            front_effect_y = front_effect_y + 16 - (4 * player.walk_frame + 4);
                                        }
                                        if (player.walk_direction == FacingDirection_Up)
                                        {
                                            front_effect_x = 8 * player.walk_frame + front_effect_x - 24;
                                            front_effect_y = front_effect_y + 16 - (4 * player.walk_frame + 4);
                                        }
                                        if (player.walk_direction == FacingDirection_Right)
                                        {
                                            front_effect_x = 8 * player.walk_frame + front_effect_x - 24;
                                            front_effect_y = front_effect_y + 4 * player.walk_frame - 12;
                                        }
                                        if (player.walk_direction == FacingDirection_Down)
                                        {
                                            front_effect_x = front_effect_x + 32 - (8 * player.walk_frame + 8);
                                            front_effect_y = front_effect_y + 4 * player.walk_frame - 12;
                                        }
                                    }
                                    if (player.effect_id == SpellEffect_FireBall)
                                        front_effect_y = front_effect_y - (160 - 30 * player.effect_frame);
                                    if (player.effect_id == SpellEffect_IceBall)
                                        front_effect_y = front_effect_y - (160 - 30 * player.effect_frame);
                                    if (player.effect_id == SpellEffect_EnergyBall)
                                        front_effect_y = front_effect_y - 10 * player.effect_frame;
                                    if (player.effect_id == SpellEffect_Boulders)
                                        front_effect_y = front_effect_y - (160 - 30 * player.effect_frame);
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
                                            front_effect_y = front_effect_y + 14;
                                        if (player.effect_frame == 2)
                                            front_effect_x += 20;
                                        if (player.effect_frame == 3)
                                            front_effect_y -= 14;
                                    }
                                    if (player.effect_id == SpellEffect_DarkBeam ||
                                        player.effect_id == SpellEffect_Heaven || player.effect_id == SpellEffect_Aura)
                                    {
                                        front_effect_frame = player.effect_loops_left;
                                        flicker_roll = random(2);
                                        if (front_effect_frame > 0)
                                            front_effect_frame = 4;
                                        if (player.effect_id == SpellEffect_DarkBeam)
                                            front_effect_frame = random(2) + 3;
                                        if (flicker_roll > 0 && player.effect_blend_layer)
                                        {
                                            front_loop_effect = gfx->effects->GetGraphic(player.effect_id);
                                            if (front_loop_effect)
                                                gfx->game->BlitClippedHalfBlend(front_effect_x + shake_offset,
                                                    front_effect_y, front_loop_effect->resource->layer1_surface,
                                                    front_loop_effect->resource->GetFrameRect(front_effect_frame));
                                        }
                                    }
                                    else
                                    {
                                        if (player.effect_blend_layer)
                                        {
                                            front_effect = gfx->effects->GetGraphic(player.effect_id);
                                            if (front_effect)
                                                gfx->game->BlitClippedHalfBlend(front_effect_x + shake_offset,
                                                    front_effect_y, front_effect->resource->layer1_surface,
                                                    front_effect->resource->GetFrameRect(player.effect_frame));
                                        }
                                        if (player.effect_solid_layer)
                                        {
                                            top_effect = gfx->effects->GetGraphic(player.effect_id);
                                            if (top_effect)
                                                gfx->game->BlitClipped(front_effect_x + shake_offset, front_effect_y,
                                                    top_effect->resource->layer2_surface,
                                                    top_effect->resource->GetFrameRect(player.effect_frame));
                                        }
                                    }
                                }
                            }
                            player_found = true;
                            player_index++;
                        }
                    } while ((int)MainForm->players->GetCount() > player_index && player_found);
                }
                if ((int)MainForm->npcs->GetCount() > npc_index)
                {
                    char npc_found;
                    do
                    {
                        loop_entered = true;
                        npc_found = false;
                        Npc npc = MainForm->npcs->GetNpc(npc_index);
                        if (ii == npc.x && n == npc.y)
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
                                        int npc_behind_effect_y;
                                        EffectWrapper *npc_back_effect;
                                        npc_behind_effect_x =
                                            32 * npc.x - 32 * npc.y + MainForm->map->GetScrollOffsetX() + 309;
                                        npc_behind_effect_y =
                                            16 * npc.x + 16 * npc.y + MainForm->map->GetScrollOffsetY() + 120;
                                        npc_behind_effect_x -= npc_behind_effect_size.width / 2;
                                        npc_behind_effect_y -= npc_behind_effect_size.height / 2;
                                        if (npc.walking)
                                        {
                                            if (npc.walk_direction == FacingDirection_Left)
                                            {
                                                npc_behind_effect_x =
                                                    npc_behind_effect_x + 32 - (8 * npc.walk_frame + 8);
                                                npc_behind_effect_y =
                                                    npc_behind_effect_y + 16 - (4 * npc.walk_frame + 4);
                                            }
                                            if (npc.walk_direction == FacingDirection_Up)
                                            {
                                                npc_behind_effect_x = 8 * npc.walk_frame + npc_behind_effect_x - 24;
                                                npc_behind_effect_y =
                                                    npc_behind_effect_y + 16 - (4 * npc.walk_frame + 4);
                                            }
                                            if (npc.walk_direction == FacingDirection_Right)
                                            {
                                                npc_behind_effect_x = 8 * npc.walk_frame + npc_behind_effect_x - 24;
                                                npc_behind_effect_y = 4 * npc.walk_frame + npc_behind_effect_y - 12;
                                            }
                                            if (npc.walk_direction == FacingDirection_Down)
                                            {
                                                npc_behind_effect_x =
                                                    npc_behind_effect_x + 32 - (8 * npc.walk_frame + 8);
                                                npc_behind_effect_y = 4 * npc.walk_frame + npc_behind_effect_y - 12;
                                            }
                                        }
                                        if (npc.effect_id == SpellEffect_FireBall)
                                            npc_behind_effect_y -= 160 - 30 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_IceBall)
                                            npc_behind_effect_y -= 160 - 30 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_EnergyBall)
                                            npc_behind_effect_y -= 10 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_Boulders)
                                            npc_behind_effect_y -= 160 - 30 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_Aura)
                                            npc_behind_effect_y -= 12;
                                        if (npc.effect_id == SpellEffect_GreenFlame)
                                            npc_behind_effect_y -= 44;
                                        if (npc.effect_id == SpellEffect_Whirl)
                                        {
                                            npc_behind_effect_y -= 10;
                                            if (!npc.effect_frame)
                                                npc_behind_effect_x -= 20;
                                            if (npc.effect_frame == 1)
                                                npc_behind_effect_y += 14;
                                            if (npc.effect_frame == 2)
                                                npc_behind_effect_x += 20;
                                            if (npc.effect_frame == 3)
                                                npc_behind_effect_y -= 14;
                                        }
                                        npc_back_effect = gfx->effects->GetGraphic(npc.effect_id);
                                        if (npc_back_effect)
                                            gfx->game->BlitClipped(npc_behind_effect_x + shake_offset,
                                                npc_behind_effect_y, npc_back_effect->resource->layer0_surface,
                                                npc_back_effect->resource->GetFrameRect(npc.effect_frame));
                                    }
                                }
                            }
                            unsigned int npc_x;
                            int npc_y;
                            if (!npc.hidden)
                            {
                                char npc_alpha;
                                if (gfx->npc_cache->RequestGraphic(npc.graphic))
                                {
                                    npc_x = 32 * npc.x - 32 * npc.y + MainForm->map->GetScrollOffsetX() + 309;
                                    npc_y = 16 * npc.x + 16 * npc.y + MainForm->map->GetScrollOffsetY() + 174;
                                    npc_alpha = -1;
                                    if (npc.dying)
                                    {
                                        if (npc.death_countdown > 1 && npc.death_countdown < 10)
                                            npc_alpha = 25 * npc.death_countdown;
                                        else
                                            npc_alpha = '\n';
                                    }
                                    if (npc.graphic == NpcGraphic_Wraith || npc.graphic == NpcGraphic_CursedMask ||
                                        npc.graphic == NpcGraphic_AncientWraith)
                                    {
                                        npc_alpha = (char)random(40) + 24;
                                        if (npc.attacking || npc.alt_attacking)
                                            npc_alpha = 'Z';
                                    }
                                    gfx->npcs->DrawNpc(npc_x, npc_y, npc, shake_offset, npc_alpha);
                                }
                                else
                                    redraw_needed = true;
                            }
                            if (npc.effect_running)
                            {
                                if (npc.effect_blend_layer || npc.effect_solid_layer)
                                {
                                    if (gfx->effects->RequestGraphic(npc.effect_id))
                                    {
                                        GraphicSize npc_front_effect_size = gfx->effects->GetGraphicSize(npc.effect_id);
                                        int npc_front_effect_x;
                                        int npc_front_effect_y;
                                        npc_front_effect_x =
                                            32 * npc.x - 32 * npc.y + MainForm->map->GetScrollOffsetX() + 309;
                                        npc_front_effect_y =
                                            16 * npc.x + 16 * npc.y + MainForm->map->GetScrollOffsetY() + 120;
                                        npc_front_effect_x -= npc_front_effect_size.width / 2;
                                        npc_front_effect_y -= npc_front_effect_size.height / 2;
                                        if (npc.walking)
                                        {
                                            if (npc.walk_direction == FacingDirection_Left)
                                            {
                                                npc_front_effect_x = npc_front_effect_x + 32 - (8 * npc.walk_frame + 8);
                                                npc_front_effect_y = npc_front_effect_y + 16 - (4 * npc.walk_frame + 4);
                                            }
                                            if (npc.walk_direction == FacingDirection_Up)
                                            {
                                                npc_front_effect_x = 8 * npc.walk_frame + npc_front_effect_x - 24;
                                                npc_front_effect_y = npc_front_effect_y + 16 - (4 * npc.walk_frame + 4);
                                            }
                                            if (npc.walk_direction == FacingDirection_Right)
                                            {
                                                npc_front_effect_x = 8 * npc.walk_frame + npc_front_effect_x - 24;
                                                npc_front_effect_y = 4 * npc.walk_frame + npc_front_effect_y - 12;
                                            }
                                            if (npc.walk_direction == FacingDirection_Down)
                                            {
                                                npc_front_effect_x = npc_front_effect_x + 32 - (8 * npc.walk_frame + 8);
                                                npc_front_effect_y = 4 * npc.walk_frame + npc_front_effect_y - 12;
                                            }
                                        }
                                        if (npc.effect_id == SpellEffect_FireBall)
                                            npc_front_effect_y -= 160 - 30 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_IceBall)
                                            npc_front_effect_y -= 160 - 30 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_EnergyBall)
                                            npc_front_effect_y -= 10 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_Boulders)
                                            npc_front_effect_y -= 160 - 30 * npc.effect_frame;
                                        if (npc.effect_id == SpellEffect_Aura)
                                            npc_front_effect_y -= 12;
                                        if (npc.effect_id == SpellEffect_GreenFlame)
                                            npc_front_effect_y -= 44;
                                        if (npc.effect_id == SpellEffect_Whirl)
                                        {
                                            npc_front_effect_y -= 10;
                                            if (!npc.effect_frame)
                                                npc_front_effect_x -= 20;
                                            if (npc.effect_frame == 1)
                                                npc_front_effect_y += 14;
                                            if (npc.effect_frame == 2)
                                                npc_front_effect_x += 20;
                                            if (npc.effect_frame == 3)
                                                npc_front_effect_y -= 14;
                                        }
                                        if (npc.effect_blend_layer)
                                        {
                                            EffectWrapper *npc_front_effect;
                                            npc_front_effect = gfx->effects->GetGraphic(npc.effect_id);
                                            if (npc_front_effect)
                                                gfx->game->BlitClippedHalfBlend(npc_front_effect_x + shake_offset,
                                                    npc_front_effect_y, npc_front_effect->resource->layer1_surface,
                                                    npc_front_effect->resource->GetFrameRect(npc.effect_frame));
                                        }
                                        if (npc.effect_solid_layer)
                                        {
                                            EffectWrapper *npc_top_effect;
                                            npc_top_effect = gfx->effects->GetGraphic(npc.effect_id);
                                            if (npc_top_effect)
                                                gfx->game->BlitClipped(npc_front_effect_x + shake_offset,
                                                    npc_front_effect_y, npc_top_effect->resource->layer2_surface,
                                                    npc_top_effect->resource->GetFrameRect(npc.effect_frame));
                                        }
                                    }
                                }
                            }
                            npc_found = true;
                            npc_index++;
                        }
                    } while ((int)MainForm->npcs->GetCount() > npc_index && npc_found);
                }
                if (MainForm->effects->GetCount() > effect_index)
                {
                    char effect_found;
                    do
                    {
                        loop_entered = true;
                        effect_found = false;
                        MapEffect map_effect = MainForm->effects->GetEffect(effect_index);
                        if (ii == map_effect.x && n == map_effect.y)
                        {
                            if (!map_effect.finished && gfx->effects->RequestGraphic(map_effect.effect_id))
                            {
                                GraphicSize effect_size = gfx->effects->GetGraphicSize(map_effect.effect_id);
                                int effect_x;
                                int effect_y;
                                effect_x =
                                    32 * map_effect.x - 32 * map_effect.y + MainForm->map->GetScrollOffsetX() + 309;
                                effect_y =
                                    16 * map_effect.x + 16 * map_effect.y + MainForm->map->GetScrollOffsetY() + 120;
                                effect_x -= effect_size.width / 2;
                                effect_y -= effect_size.height / 2;
                                if (map_effect.has_blend_layer)
                                {
                                    EffectWrapper *map_effect_front;
                                    map_effect_front = gfx->effects->GetGraphic(map_effect.effect_id);
                                    if (map_effect_front)
                                        gfx->game->BlitClippedHalfBlend(effect_x + shake_offset, effect_y,
                                            map_effect_front->resource->layer1_surface,
                                            map_effect_front->resource->GetFrameRect(map_effect.frame));
                                }
                                if (map_effect.has_solid_layer)
                                {
                                    EffectWrapper *map_effect_top;
                                    map_effect_top = gfx->effects->GetGraphic(map_effect.effect_id);
                                    if (map_effect_top)
                                        gfx->game->BlitClipped(effect_x + shake_offset, effect_y,
                                            map_effect_top->resource->layer2_surface,
                                            map_effect_top->resource->GetFrameRect(map_effect.frame));
                                }
                            }
                            effect_found = true;
                            effect_index++;
                        }
                    } while (MainForm->effects->GetCount() > effect_index && effect_found);
                }
                if (map->masks[ii][n] >= 1 &&
                    net_client->IsObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n) &&
                    gfx->masks->RequestGraphic(map->masks[ii][n]))
                {
                    GraphicSize mask_size = gfx->masks->GetGraphicSize(map->masks[ii][n]);
                    int mask_x;
                    int mask_y;
                    MaskWrapper *mask;
                    mask_x = screen_x + 30 - mask_size.width / 2;
                    mask_y = screen_y + 30 - mask_size.height;
                    mask = gfx->masks->GetGraphic(map->masks[ii][n]);
                    if (mask)
                        gfx->game->BlitClipped(mask_x, mask_y, mask->resource->surface, mask->resource->surface_rect);
                }
                if (map->walls[ii][n] >= 1 &&
                    net_client->IsWallInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                {
                    if (gfx->walls->RequestGraphic(map->walls[ii][n]))
                    {
                        GraphicSize wall_size = gfx->walls->GetGraphicSize(map->walls[ii][n]);
                        int wall_y;
                        WallWrapper *wall;
                        wall_y = screen_y + 31 - wall_size.height;
                        wall = gfx->walls->GetGraphic(map->walls[ii][n]);
                        if (wall)
                        {
                            gfx->game->BlitClipped(screen_x, wall_y, wall->resource->surface,
                                wall->resource->GetFrameRect(map->tile_frame));
                            if (wall->resource->is_animated)
                                MainForm->map->has_animated_tiles = true;
                        }
                    }
                    else
                        redraw_needed = true;
                }
            }
            if (n < map->GetHeight() && map->walls_next_row[ii][n + 1] >= 1 &&
                net_client->IsWallInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n + 1))
            {
                if (gfx->walls->RequestGraphic(map->walls_next_row[ii][n + 1]))
                {
                    GraphicSize lower_wall_size = gfx->walls->GetGraphicSize(map->walls_next_row[ii][n + 1]);
                    int side_wall_y;
                    WallWrapper *side_wall;
                    side_wall_y = screen_y + 31 - lower_wall_size.height;
                    side_wall = gfx->walls->GetGraphic(map->walls_next_row[ii][n + 1]);
                    if (side_wall)
                    {
                        gfx->game->BlitClipped(screen_x, side_wall_y + 16, side_wall->resource->surface,
                            side_wall->resource->GetFrameRect(map->tile_frame));
                        if (side_wall->resource->is_animated)
                            MainForm->map->has_animated_tiles = true;
                    }
                }
                else
                    redraw_needed = true;
            }
            if (n >= 0 && map->top_tiles[ii][n] >= 1 &&
                net_client->IsInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n) &&
                gfx->tops->RequestGraphic(map->top_tiles[ii][n]))
            {
                TopWrapper *roof;
                roof = gfx->tops->GetGraphic(map->top_tiles[ii][n]);
                if (roof)
                    gfx->game->BlitClipped(screen_x, screen_y - 64, roof->resource->surface,
                        roof->resource->surface_rect);
            }
        }
    }
    int top_mask_cell_x;
    int top_mask_cell_y;
    for (jj = first_y - 1; jj <= last_y; jj++)
    {
        loop_entered = true;
        for (kk = first_x; kk <= last_x; kk++)
        {
            loop_entered = true;
            top_mask_cell_x = map->GetScrollOffsetX() + (32 * kk - 32 * jj) + 278;
            top_mask_cell_y = map->GetScrollOffsetY() + (16 * kk + 16 * jj) + 134;
            if (shake_offset)
                top_mask_cell_x += shake_offset;
            if (map->top_masks[kk][jj] >= 1 &&
                net_client->IsObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), kk, jj) &&
                gfx->masks->RequestGraphic(map->top_masks[kk][jj]))
            {
                GraphicSize top_mask_size = gfx->masks->GetGraphicSize(map->top_masks[kk][jj]);
                int top_mask_x;
                int top_mask_y;
                MaskWrapper *overlay_mask;
                top_mask_x = top_mask_cell_x + 30 - top_mask_size.width / 2;
                top_mask_y = top_mask_cell_y + 30 - top_mask_size.height;
                overlay_mask = gfx->masks->GetGraphic(map->top_masks[kk][jj]);
                if (overlay_mask)
                    gfx->game->BlitClipped(top_mask_x, top_mask_y, overlay_mask->resource->surface,
                        overlay_mask->resource->surface_rect);
            }
        }
    }
    unsigned char low_health_alpha;
    unsigned char flash_alpha;
    int flash_color;
    int status_icon_x;
    int icon_y;
    unsigned char icon_count;
    blitter->AddMapLights();
    blitter->ApplyDarkness(OffscreenSurface);
    blitter->DrawMapEffects(OffscreenSurface);
    // The own character is drawn once more, over everything on the map.
    if (own_player_index != -1)
    {
        Player own_player = players->GetPlayer(own_player_index);
        if (net_client->GetPlayerId() == own_player.player_id)
            gfx->sprites->DrawPlayer(&own_player, true, shake_offset);
    }
    // Low health tints the view, and a flash tints it in its own colour.
    if (MainForm->animation->interface_controller->status->IsLowHealth())
    {
        low_health_alpha = MainForm->animation->interface_controller->status->GetLowHealthAlpha();
        blitter->TintScreen(OffscreenSurface, 1, low_health_alpha, 0xAA0000);
        blitter->TintScreen(OffscreenSurface, 2, low_health_alpha, 0xAA0000);
        blitter->TintScreen(OffscreenSurface, 3, low_health_alpha, 0xAA0000);
        blitter->TintScreen(OffscreenSurface, 4, low_health_alpha, 0xAA0000);
    }
    if (MainForm->animation->interface_controller->status->GetFlashAlpha() > 0)
    {
        flash_alpha = MainForm->animation->interface_controller->status->GetFlashAlpha();
        flash_color = MainForm->animation->interface_controller->status->GetFlashColor();
        blitter->TintScreen(OffscreenSurface, 1, flash_alpha, flash_color);
        blitter->TintScreen(OffscreenSurface, 2, flash_alpha, flash_color);
        blitter->TintScreen(OffscreenSurface, 3, flash_alpha, flash_color);
        blitter->TintScreen(OffscreenSurface, 4, flash_alpha, flash_color);
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
    gfx->npc_cache->AgeGraphics();
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
        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface, gfx->interface_graphics->status_icons,
            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Boss, false), 140);
        status_icon_x += 18;
        icon_count++;
    }
    if (map->GetMapType() == PVP_MAP_TYPE)
    {
        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface, gfx->interface_graphics->status_icons,
            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Pvp, false), 140);
        status_icon_x += 18;
        icon_count++;
    }
    if (map->GetMapAttribute() == 1 || map->GetMapAttribute() == 2)
    {
        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface, gfx->interface_graphics->status_icons,
            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Drain, false), 140);
        status_icon_x += 18;
        icon_count++;
    }
    if (icon_count >= 2)
    {
        if (MainForm->gfx->interface_graphics->minimap_visible == 2 && !config->GetRadarPosition())
            status_icon_x = 145;
        status_icon_x += 10;
    }
    else if (own_has_shield && own_has_weapon)
        status_icon_x += 10;
    if (own_has_shield)
    {
        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface, gfx->interface_graphics->status_icons,
            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Shield, false), 140);
        status_icon_x += 17;
        icon_count++;
    }
    if (own_has_weapon)
    {
        MainForm->blitter->AlphaBlt(status_icon_x, icon_y, OffscreenSurface, gfx->interface_graphics->status_icons,
            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_Weapon, false), 140);
        status_icon_x += 18;
        icon_count++;
    }
    if (animation->interface_controller->spells->awaiting_target)
        MainForm->blitter->AlphaBlt(602, icon_y, OffscreenSurface, gfx->interface_graphics->status_icons,
            &gfx->interface_graphics->GetStatusIconRect(StatusIcon_SpellTarget, false), 140);
    animation->interface_controller->game_bars->MarkDirty();
    // Then the popups, the boss bars and each open window over the map; the boxes go last.
    if (animation->popup->is_visible)
        animation->popup->Draw();
    if (animation->boss_bars->IsAnyVisible())
        animation->boss_bars->Draw();
    if (animation->notification->IsVisible())
        animation->notification->Draw();
    if (animation->game->player_menu_visible)
    {
        gfx->game->BlitClippedHalfBlend(animation->game->player_menu_x + 4, animation->game->player_menu_y + 4,
            gfx->game->extra_surface, gfx->game->player_menu_shadow_rect);
        gfx->game->BlitClipped(animation->game->player_menu_x, animation->game->player_menu_y, gfx->game->extra_surface,
            gfx->game->player_menu_rect);
        if (animation->game->player_menu_hovered_option >= 0)
            gfx->game->BlitClipped(animation->game->player_menu_x + 13,
                animation->game->player_menu_y + 15 * animation->game->player_menu_hovered_option + 9,
                gfx->game->extra_surface, gfx->game->GetPopupItemRect(animation->game->player_menu_hovered_option));
    }
    if (animation->quests->is_visible)
    {
        gfx->game->DrawQuestWindow();
        gfx->game->DrawQuestButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawQuestText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->stat->is_visible)
    {
        gfx->game->DrawStatWindow();
        gfx->game->DrawStatButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawStatText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->barber->is_visible)
    {
        gfx->game->DrawBarberWindow();
        gfx->game->DrawBarberButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawBarberText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->chest->is_visible)
    {
        gfx->game->DrawChestWindow();
        gfx->game->DrawChestButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawChestText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->memo->is_visible)
    {
        gfx->game->DrawMemoWindow();
        gfx->game->DrawMemoButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawMemoText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->dialog->is_visible)
    {
        gfx->game->DrawDialogWindow();
        gfx->game->DrawDialogButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawDialogText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->report->is_visible)
    {
        gfx->game->DrawReportWindow();
        gfx->game->DrawReportButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawReportText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->guild->is_visible)
    {
        gfx->game->DrawGuildWindow();
        gfx->game->DrawGuildButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawGuildText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->inn->is_visible)
    {
        gfx->game->DrawInnWindow();
        gfx->game->DrawInnButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawInnText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->law->is_visible)
    {
        gfx->game->DrawLawWindow();
        gfx->game->DrawLawButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawLawText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->locker->is_visible)
    {
        gfx->game->DrawLockerWindow();
        gfx->game->DrawLockerButtons();
        animation->locker->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawLockerText();
                animation->locker->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->shop->is_visible)
    {
        gfx->game->DrawShopWindow();
        gfx->game->DrawShopButtons();
        animation->shop->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawShopText();
                animation->shop->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->crafter->is_visible)
    {
        gfx->game->DrawCrafterWindow();
        gfx->game->DrawCrafterButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawCrafterText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        animation->crafter->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                animation->crafter->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->learn->is_visible)
    {
        gfx->game->DrawLearnWindow();
        gfx->game->DrawLearnButtons();
        animation->learn->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawLearnText();
                animation->learn->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->jukebox->is_visible)
    {
        gfx->game->DrawJukeboxWindow();
        gfx->game->DrawJukeboxButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawJukeboxText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
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
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawBankText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->book->is_visible)
    {
        gfx->game->DrawBookWindow();
        gfx->game->DrawBookButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawBookText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->paperdoll->is_visible)
    {
        gfx->game->DrawPaperdollWindow();
        gfx->game->DrawPaperdollButtons();
        animation->paperdoll->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawPaperdollText();
                animation->paperdoll->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->captcha->is_visible)
    {
        gfx->game->DrawCaptchaWindow();
        animation->captcha->Draw();
        gfx->game->DrawCaptchaButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawCaptchaText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
            edit_canvas->DrawEditBoxes(EditGroup_InputForm);
        }
    }
    if (animation->visual_doc->is_visible)
    {
        gfx->game->DrawVisualDocWindow();
        animation->visual_doc->Draw();
        gfx->game->DrawVisualDocButtons();
    }
    if (animation->hotkeys->is_visible)
    {
        gfx->game->DrawHotkeys();
        animation->hotkeys->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawNothing();
                animation->hotkeys->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->trade->is_visible)
    {
        gfx->game->DrawTradeWindow();
        gfx->game->DrawTradeButtons();
        animation->trade->tooltip->DrawFrame();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawTradeText();
                animation->trade->tooltip->DrawString();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->game->amount_box_visible)
    {
        OffscreenSurface->BltFast(animation->game->amount_box_x, animation->game->amount_box_y,
            gfx->interface_graphics->control_sheet, &gfx->game->amount_box_rect, DDBLTFAST_WAIT);
        if (animation->game->amount_box_kind == AmountBoxKind_JunkItem)
            OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10,
                gfx->interface_graphics->control_sheet, &gfx->game->junk_title_rect, DDBLTFAST_WAIT);
        if (animation->game->amount_box_kind == AmountBoxKind_Amount)
            OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10,
                gfx->interface_graphics->control_sheet, &gfx->game->amount_title_rect, DDBLTFAST_WAIT);
        if (animation->game->amount_box_kind == AmountBoxKind_TradeAddItem)
            OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10,
                gfx->interface_graphics->control_sheet, &gfx->game->trade_title_rect, DDBLTFAST_WAIT);
        if (animation->game->amount_box_kind == AmountBoxKind_BankDeposit ||
            animation->game->amount_box_kind == AmountBoxKind_BankWithdraw)
            OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10,
                gfx->interface_graphics->control_sheet, &gfx->game->bank_title_rect, DDBLTFAST_WAIT);
        if (animation->game->amount_box_kind == AmountBoxKind_ShopBuy ||
            animation->game->amount_box_kind == AmountBoxKind_ShopSell ||
            animation->game->amount_box_kind == AmountBoxKind_LockerAddItem)
            OffscreenSurface->BltFast(animation->game->amount_box_x + 11, animation->game->amount_box_y + 10,
                gfx->interface_graphics->control_sheet, &gfx->game->shop_title_rect, DDBLTFAST_WAIT);
        if (animation->game->amount_slider_grabbed)
            OffscreenSurface->BltFast(animation->game->amount_box_x + animation->game->amount_slider_offset + 25,
                animation->game->amount_box_y + 96, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(6), DDBLTFAST_WAIT);
        else
        {
            int unused_value;
            int knob_icon = 5;
            if (animation->game->amount_slider_hovered)
                knob_icon = 6;
            String amount_text = edits->GetText(EditId_Amount);
            if (edits->GetText(EditId_Amount).Length() > 0)
            {
                int amount = StrToInt(edits->GetText(EditId_Amount));
                int slider_width = 107;
                amount--;
                double slider_step = (double)slider_width / (animation->game->amount_max - 1);
                double knob_offset = amount * slider_step;
                animation->game->amount_slider_offset = knob_offset;
                if (animation->game->amount_slider_offset < 0)
                    OffscreenSurface->BltFast(animation->game->amount_box_x + 25, animation->game->amount_box_y + 96,
                        MainForm->gfx->interface_graphics->scroll_icons,
                        &MainForm->gfx->interface_graphics->GetScrollIconRect(knob_icon), DDBLTFAST_WAIT);
                else
                    OffscreenSurface->BltFast(animation->game->amount_box_x + animation->game->amount_slider_offset +
                        25, animation->game->amount_box_y + 96, MainForm->gfx->interface_graphics->scroll_icons,
                        &MainForm->gfx->interface_graphics->GetScrollIconRect(knob_icon), DDBLTFAST_WAIT);
            }
            else
            {
                animation->game->amount_slider_offset = 0;
                OffscreenSurface->BltFast(animation->game->amount_box_x + 25, animation->game->amount_box_y + 96,
                    MainForm->gfx->interface_graphics->scroll_icons,
                    &MainForm->gfx->interface_graphics->GetScrollIconRect(knob_icon), DDBLTFAST_WAIT);
            }
        }
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawItemAmountText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->game->DrawItemAmountButtons();
    }
    if (animation->message_board->is_visible)
    {
        gfx->game->DrawMsgBoardWindow();
        gfx->game->DrawMsgBoardButtons();
        edit_canvas->DrawEditBoxes(EditGroup_MsgBoard);
    }
    if (animation->social->is_open)
    {
        gfx->game->DrawSocialWindow();
        gfx->game->DrawSocialButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawSocialText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->game->form_visible)
    {
        gfx->game->DrawListBox();
        gfx->game->DrawListBoxButtons();
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->game->DrawFormText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
            edit_canvas->DrawEditBoxes(EditGroup_InputForm);
        }
    }
    if (animation->game->input_box_visible)
    {
        OffscreenSurface->BltFast(animation->game->input_box_x, animation->game->input_box_y,
            gfx->game->input_box_surface, &gfx->game->input_box_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreenIf(2))
        {
            try
            {
                gfx->game->DrawInputBoxText();
            }
            catch (Exception &)
            {
            }
            MainForm->fonts->End();
        }
        gfx->game->DrawInputBoxButtons();
    }
    if (animation->game->large_box_visible)
    {
        OffscreenSurface->BltFast(animation->game->large_box_x, animation->game->large_box_y,
            gfx->game->text_box_surface, &gfx->game->text_box_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreenIf(2))
        {
            try
            {
                gfx->game->DrawTextBoxText();
            }
            catch (Exception &)
            {
            }
            MainForm->fonts->End();
        }
        gfx->game->DrawTextBoxButtons();
    }
    if (animation->game->message_visible)
    {
        OffscreenSurface->BltFast(animation->game->message_x, animation->game->message_y,
            gfx->game->message_box_surface, &gfx->game->message_box_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreenIf(100))
        {
            try
            {
                gfx->game->DrawMessageBoxText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->game->DrawMessageBoxButtons();
    }
    if (animation->interface_controller->game_bars->TestAndClearDirty())
    {
        gfx->interface_graphics->DrawStatusBars();
        if ((animation->interface_controller->game_bars->show_health_text ||
            animation->interface_controller->game_bars->show_mana_text ||
            animation->interface_controller->game_bars->show_stamina_text ||
            animation->interface_controller->game_bars->show_experience_text) && fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->interface_graphics->DrawStatusBarText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    gfx->interface_graphics->DrawCornerButton();
    flip_needed = true;
}

// Redraws the parts of the bottom panel that changed: the open panel, the status widget and the chat line.
void TMainForm::RenderGame_BottomPanel()
{
    int seconds;
    int dc;
    int status_change;
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawStatusValues();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
                }
            }
            if (animation->interface_controller->status->stat_points > 0)
            {
                int raise_state = 0;
                // The stat-raise buttons blink while points are left to spend.
                if (animation->interface_controller->status->GetBlinkPhase() > 5)
                    raise_state = 1;
                MainForm->OffscreenSurface->BltFast(208, 337, gfx->interface_graphics->control_sheet,
                    &gfx->interface_graphics->GetRaiseButtonRect(raise_state), DDBLTFAST_WAIT);
                MainForm->OffscreenSurface->BltFast(208, 355, gfx->interface_graphics->control_sheet,
                    &gfx->interface_graphics->GetRaiseButtonRect(raise_state), DDBLTFAST_WAIT);
                MainForm->OffscreenSurface->BltFast(208, 373, gfx->interface_graphics->control_sheet,
                    &gfx->interface_graphics->GetRaiseButtonRect(raise_state), DDBLTFAST_WAIT);
                MainForm->OffscreenSurface->BltFast(208, 391, gfx->interface_graphics->control_sheet,
                    &gfx->interface_graphics->GetRaiseButtonRect(raise_state), DDBLTFAST_WAIT);
                MainForm->OffscreenSurface->BltFast(208, 409, gfx->interface_graphics->control_sheet,
                    &gfx->interface_graphics->GetRaiseButtonRect(raise_state), DDBLTFAST_WAIT);
                MainForm->OffscreenSurface->BltFast(208, 427, gfx->interface_graphics->control_sheet,
                    &gfx->interface_graphics->GetRaiseButtonRect(raise_state), DDBLTFAST_WAIT);
            }
        }
        if (MainForm->animation->GetSubState() == GamePanel_Chat)
        {
            if (animation->interface_controller->chat->TakeTextDirty())
            {
                OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                    &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                gfx->interface_graphics->DrawChatPanel();
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawChatText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawPlayerListText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawSpellInfoText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawSkillInfoText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawInventoryText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawGroupText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
                }
                if (OffscreenSurface->GetDC((void **)&dc))
                {
                    try
                    {
                        gfx->interface_graphics->DrawGroupHealthBars(dc);
                    }
                    catch (Exception &)
                    {
                    }
                    OffscreenSurface->ReleaseDC((void *)dc);
                }
            }
            if (animation->interface_controller->group->TestAndClearScrollDirty())
            {
                flip_needed = true;
                gfx->interface_graphics->DrawGroupScrollButtons();
            }
        }
        if (MainForm->animation->GetSubState() == GamePanel_Settings &&
            animation->interface_controller->settings->TestAndClearDirty())
        {
            OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
            gfx->interface_graphics->DrawSettingMarkers();
            if (fonts->BeginOffscreenIf(1))
            {
                try
                {
                    gfx->interface_graphics->DrawSettingsText();
                }
                catch (Exception &)
                {
                }
                fonts->End();
            }
        }
        if (MainForm->animation->GetSubState() == GamePanel_News)
        {
            if (animation->interface_controller->news->TestAndClearTextDirty())
            {
                OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                    &gfx->interface_graphics->panel_rect, DDBLTFAST_WAIT);
                gfx->interface_graphics->DrawNewsList();
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawNewsText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
                if (fonts->BeginOffscreenIf(1))
                {
                    try
                    {
                        gfx->interface_graphics->DrawHelpText();
                    }
                    catch (Exception &)
                    {
                    }
                    fonts->End();
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
        status_change = animation->interface_controller->TakeStatusChange();
        if (status_change == 1)
        {
            OffscreenSurface->BltFast(99, 452, gfx->interface_graphics->tooltip_background,
                &gfx->interface_graphics->tooltip_rect, DDBLTFAST_WAIT);
            if (fonts->BeginOffscreen())
            {
                try
                {
                    fonts->color = themes->color_bottom_bar;
                    fonts->face = 0;
                    fonts->size = 7;
                    fonts->DrawString(99, 455, animation->interface_controller->GetStatusMessage());
                }
                catch (Exception &)
                {
                }
                fonts->End();
            }
        }
        if (status_change == 2)
            OffscreenSurface->BltFast(99, 452, gfx->interface_graphics->tooltip_background,
                &gfx->interface_graphics->tooltip_rect, DDBLTFAST_WAIT);
    }
    String current_clock_text;
    // The status widget: 0 the clock, 1 the time since start, 2 the weight carried, 3 and up the position.
    if (!config->GetStatusWidget())
        current_clock_text = TimeToStr(Time());
    else if (config->GetStatusWidget() == 1)
    {
        time(&current_time);
        long elapsed = current_time - start_time;
        String elapsed_text = IntToStr(elapsed);
        int clock_hours = 0;
        int clock_minutes = 0;
        seconds = elapsed_text.ToInt();
        if (seconds > 3599)
        {
            clock_hours = seconds / 3600;
            seconds = seconds % 3600;
        }
        if (seconds > 59)
        {
            clock_minutes = seconds / 60;
            seconds = seconds % 60;
        }
        current_clock_text = "";
        if (clock_hours > 0)
            current_clock_text += FormatClock(clock_hours) + "h ";
        current_clock_text += FormatClock(clock_minutes) + "m ";
        current_clock_text += FormatClock(seconds) + "s";
    }
    else if (config->GetStatusWidget() == 2)
    {
        current_clock_text = IntToStr(animation->interface_controller->status->weight);
        current_clock_text += " / ";
        current_clock_text += IntToStr(animation->interface_controller->status->max_weight);
        current_clock_text += " w";
    }
    else if (config->GetStatusWidget() >= 3)
    {
        current_clock_text = IntToStr(MainForm->players->GetPositionExtra(MainForm->net_client->GetPlayerId()));
        current_clock_text += " // ";
        current_clock_text += IntToStr(MainForm->players->GetX(MainForm->net_client->GetPlayerId()));
        current_clock_text += ",";
        current_clock_text += IntToStr(MainForm->players->GetY(MainForm->net_client->GetPlayerId()));
    }
    // The widget is drawn again only when its text changes.
    if (current_clock_text != clock_text)
    {
        clock_text = current_clock_text;
        OffscreenSurface->BltFast(543, 452, gfx->interface_graphics->clock_background,
            &gfx->interface_graphics->clock_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreen())
        {
            try
            {
                fonts->color = themes->color_bottom_bar;
                fonts->face = 0;
                fonts->size = 7;
                int clock_x = (86 - (short)fonts->TextWidth(current_clock_text)) / 2 + 543;
                fonts->DrawString(clock_x, 455, current_clock_text);
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        flip_needed = true;
    }
    if (edits->GetCount() >= 1 && (edits->HasDirtyEdit() || edits->ConsumeTickRedraw()))
    {
        if (edits->ConsumeDirty(EditId_Chat))
        {
            gfx->interface_graphics->DrawChatModeIcon();
            OffscreenSurface->BltFast(98, 309, gfx->interface_graphics->chat_input_background,
                &gfx->interface_graphics->chat_input_rect, DDBLTFAST_WAIT);
            flip_needed = true;
        }
        edit_canvas->DrawEditBoxes(EditGroup_Game);
    }
}

// The screen while a map loads: the view cleared, then the top bars with the health, mana and stamina fills.
void TMainForm::RenderGame_MapLoadScreen()
{
    OffscreenSurface->Blt(&gfx->game->view_clip_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, gfx->game->view_fill_fx);
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
            animation->interface_controller->game_bars->GetManaBarSegments()), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
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
    if (animation->interface_controller->timed_text_visible && fonts->BeginOffscreen())
    {
        try
        {
            fonts->face = 0;
            fonts->color = 0xFFFFFF;
            fonts->size = 10;
            short text_width = fonts->TextWidth(animation->interface_controller->timed_text);
            fonts->DrawString(320 - (short)(text_width / 2), 156, animation->interface_controller->timed_text);
        }
        catch (Exception &)
        {
        }
        fonts->End();
    }
    // While the map downloads, its size shows in kilobytes.
    if (map->loading_map)
    {
        OffscreenSurface->BltFast(230, 138, gfx->interface_graphics->loading_icon,
            &gfx->interface_graphics->loading_icon_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreen())
        {
            try
            {
                fonts->face = 0;
                fonts->color = 0xFFFFFF;
                fonts->size = 7;
                fonts->DrawString(280, 146, "Loading world data [" + IntToStr(map->map_file_size / 1024 + 1) + " Kb]");
                fonts->DrawString(310, 164, "Please wait");
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    // An accepted warp asks for the new map with the warp session.
    if (map->map_request_pending)
    {
        map->map_request_pending = false;
        String packet = MainForm->net_client->EncodeNumber(map->map_id, 2);
        packet = packet + MainForm->net_client->EncodeNumber(map->warp_session_id, 2);
        net_client->SendPacket(AC_GET, OP_MAP, packet);
    }
    flip_needed = true;
}

// Draws the menu screens: the background, then the buttons, forms and texts of the screen showing.
void TMainForm::RenderMainLoop_Menu()
{
    int line_end;
    TRect loading_rect;
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
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->menu->DrawCharacterSlotText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
    }
    if (animation->GetSubState() == MenuScreen_CreateAccount)
        gfx->menu->DrawAccountCreationButtons();
    if (animation->GetSubState() == MenuScreen_PasswordReset)
    {
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->menu->DrawPasswordResetText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->menu->DrawPasswordResetControls();
    }
    if (animation->GetSubState() == MenuScreen_SignupCode)
    {
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->menu->DrawSignupCodeText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->menu->DrawSignupCodeControls();
    }
    if (animation->GetSubState() == MenuScreen_Login || animation->GetSubState() == MenuScreen_GridAccountName ||
        animation->GetSubState() == MenuScreen_PasswordGrid)
        gfx->menu->DrawConfirmButtons();
    if (animation->GetSubState() == MenuScreen_PasswordGrid)
    {
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->menu->DrawLoginText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->menu->DrawKeyHighlights();
    }
    if (animation->GetSubState() == MenuScreen_Main)
    {
        if (fonts->BeginOffscreen())
        {
            fonts->color = themes->color_menu_version;
            fonts->face = 0;
            fonts->size = 7;
            String version_text =
                VersionString(version_major, version_minor, version_build, version_letter) + " -h: " +
                    config->GetHost();
            if (themes->is_loaded)
                version_text += " -t: " + themes->theme_name;
            fonts->DrawString(28, 456, version_text);
            fonts->End();
        }
    }
    if (MainForm->animation->menu->is_loading_world)
    {
        loading_rect.top = 0;
        loading_rect.left = 220 * (MainForm->animation->menu->loading_frame / 2);
        loading_rect.bottom = 127;
        loading_rect.right = 220 * (MainForm->animation->menu->loading_frame / 2) + 220;
        OffscreenSurface->BltFast(397, 333, gfx->menu->animation_sheet, &loading_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreen())
        {
            try
            {
                fonts->color = MainForm->themes->color_dialog_caption;
                fonts->face = 1;
                fonts->size = 10;
                char load_started = false;
                if (!load_started && animation->menu->IsMapNeeded() && animation->menu->IsEnteringGame())
                {
                    fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_UpdatingMap));
                    load_started = true;
                }
                if (!load_started && animation->menu->IsItemFileNeeded() && animation->menu->IsEnteringGame())
                {
                    fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_UpdatingItems));
                    load_started = true;
                }
                if (!load_started && animation->menu->IsNpcFileNeeded() && animation->menu->IsEnteringGame())
                {
                    fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_UpdatingNpcs));
                    load_started = true;
                }
                if (!load_started && animation->menu->IsSkillFileNeeded() && animation->menu->IsEnteringGame())
                {
                    fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_UpdatingSkills));
                    load_started = true;
                }
                if (!load_started && animation->menu->IsClassFileNeeded() && animation->menu->IsEnteringGame())
                {
                    fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_UpdatingClasses));
                    load_started = true;
                }
                if (!load_started && animation->menu->IsReservedFileNeeded() && animation->menu->IsEnteringGame())
                {
                    fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_UpdatingGuilds));
                    load_started = true;
                }
                if (!load_started)
                {
                    if (animation->menu->IsEnteringGame())
                        fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_LoadingGame));
                    else
                        fonts->DrawString(410, 341, MainForm->strings->GetText(GameText_Wait));
                }
                fonts->color = MainForm->themes->color_dialog_soft;
                line_end = 0;
                fonts->face = 1;
                fonts->size = 8;
                int tip_y = 394;
                bool more_tip = true;
                String tip = animation->menu->loading_tip;
                for (; more_tip; line_end = 0)
                {
                    if ((short)fonts->TextWidth(tip) > 180)
                    {
                        for (int cut = 1; cut <= tip.Length(); cut++)
                        {
                            int tip_width = (short)fonts->TextWidth(tip.SubString(1, cut));
                            int unused_value = 0;
                            if (tip[cut] == ' ' || cut == tip.Length())
                            {
                                if (tip_width > 180 || cut == tip.Length())
                                {
                                    fonts->DrawString(416, tip_y, tip.SubString(1, line_end - 1));
                                    tip.Delete(1, line_end);
                                    break;
                                }
                                line_end = cut;
                            }
                        }
                    }
                    else
                    {
                        fonts->DrawString(416, tip_y, tip);
                        more_tip = false;
                    }
                    tip_y += 16;
                }
            }
            catch (Exception &)
            {
            }
            fonts->End();
            if (animation->menu->IsRequestDelayDone())
                net_client->sender->RequestGameData();
        }
    }
    if (animation->menu->is_form_open)
    {
        OffscreenSurface->BltFast(animation->menu->form_x, animation->menu->form_y,
            gfx->menu->create_character_background, &gfx->menu->create_character_rect, DDBLTFAST_WAIT);
        gfx->menu->DrawChangePasswordText();
        gfx->menu->DrawCreateCharacterControls();
        gfx->sprites->DrawPreviewPlayers(1);
    }
    if (edits->GetCount() >= 1)
    {
        TRect icon_rect;
        icon_rect.left = 0;
        icon_rect.top = 0;
        icon_rect.right = 20;
        icon_rect.bottom = 18;
        edit_canvas->DrawEditBoxes(EditGroup_All);
    }
    if (animation->credits->is_running && fonts->BeginOffscreenIf(3))
    {
        try
        {
            fonts->color = 0xFFFFFF;
            fonts->size = 10;
            fonts->face = 0;
            for (int credit_line = 0; credit_line < animation->credits->shown_lines->Count; credit_line++)
            {
                fonts->size = 10;
                fonts->face = 0;
                int name_indent = 0;
                line_end = 0;
                if (animation->credits->shown_styles->Strings[credit_line] == "t")
                {
                    fonts->size = 10;
                    fonts->color = 0xFFFFFF;
                }
                if (animation->credits->shown_styles->Strings[credit_line] == "h")
                    fonts->color = 0x28E6FA;
                if (animation->credits->shown_styles->Strings[credit_line] == "n")
                {
                    fonts->color = credit_name_color;
                    line_end = 10;
                }
                if (animation->credits->shown_styles->Strings[credit_line] == "s")
                {
                    fonts->color = 0x808080;
                    line_end = 10;
                    fonts->size = 10;
                }
                fonts->DrawString((short)line_end + 340, StrToInt(animation->credits->shown_y->Strings[credit_line]),
                    animation->credits->shown_lines->Strings[credit_line]);
            }
        }
        catch (Exception &)
        {
        }
        fonts->End();
    }
    if (animation->menu->is_text_window_open)
    {
        OffscreenSurface->BltFast(animation->menu->text_window_x, animation->menu->text_window_y,
            gfx->menu->list_box_background, &gfx->menu->list_box_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->menu->DrawListBoxText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->menu->DrawListBoxControls();
    }
    if (animation->menu->is_message_open)
    {
        OffscreenSurface->BltFast(animation->menu->message_x, animation->menu->message_y,
            gfx->menu->message_box_background, &gfx->menu->message_box_rect, DDBLTFAST_WAIT);
        if (fonts->BeginOffscreenIf(1))
        {
            try
            {
                gfx->menu->DrawMessageBoxText();
            }
            catch (Exception &)
            {
            }
            fonts->End();
        }
        gfx->menu->DrawMessageBoxControls();
    }
}

// Picks the credit names' colour from two pixels of the menu background: a lighter colour when either is dark.
void TMainForm::SampleCreditNameColor()
{
    unsigned char top_red;
    char dark;
    int bottom_pixel;
    unsigned char bottom_red;
    int top_pixel = blitter->ReadPixel(500, 240, gfx->menu->background);
    top_red = top_pixel;
    unsigned char top_green = (unsigned short)top_pixel >> 8;
    unsigned char top_blue = top_pixel >> 16;
    bottom_pixel = blitter->ReadPixel(500, 340, gfx->menu->background);
    unsigned char bottom_green = (unsigned short)bottom_pixel >> 8;
    bottom_red = bottom_pixel;
    unsigned char bottom_blue = bottom_pixel >> 16;
    dark = false;
    credit_name_color = 0xA1428;
    if (top_red < 60 && top_green < 60 && top_blue < 60)
        dark = true;
    if (bottom_red < 60 && bottom_green < 60 && bottom_blue < 60)
        dark = true;
    if (top_red + top_red + top_blue < 120)
        dark = true;
    if (bottom_red + bottom_red + bottom_blue < 120)
        dark = true;
    if (dark)
        credit_name_color = 0x78828C;
}

void TMainForm::RenderMainLoop_Intro()
{
    TRect clear_rect;
    TRect logo_rect;
    if (animation->intro->clear_screen_pending)
    {
        clear_rect.left = 0;
        clear_rect.top = 0;
        clear_rect.right = 640;
        clear_rect.bottom = 480;
        OffscreenSurface->Blt(&clear_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, gfx->intro->fill_fx);
        animation->intro->clear_screen_pending = false;
        flip_needed = true;
    }
    else if (animation->intro->logo_redraw_pending || animation->intro->is_animating)
    {
        logo_rect.left = 0;
        logo_rect.top = 0;
        logo_rect.right = 358;
        logo_rect.bottom = 187;
        OffscreenSurface->BltFast(141, 125, gfx->intro->surface, &logo_rect, DDBLTFAST_WAIT);
        animation->intro->logo_redraw_pending = false;
        flip_needed = true;
    }
}

void TMainForm::RenderFpsOverlay()
{
    if (!flip_needed)
        return;
    if (!show_frame_rate)
        return;
    if (fonts->BeginOffscreen())
    {
        try
        {
            fonts->color = 0xFFFFFF;
            fonts->face = 0;
            fonts->size = 7;
            fonts->DrawString(2, 2, IntToStr(frame_rate));
        }
        catch (Exception &)
        {
        }
        fonts->End();
    }
}

// Puts the finished frame on screen when it changed: by a page flip or a copy full screen, through the window
// otherwise.
void TMainForm::RenderOffscreenToPrimary()
{
    void *window_dc;
    HDC offscreen_dc;
    if (flip_needed)
    {
        flip_needed = false;
        if (config->GetFullscreen())
        {
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
        else
        {
            TRect target_rect = BoundsRect;
            target_rect.top += 20;
            if (window_height_padding > 0)
                target_rect.top += window_height_padding;
            if (!PrimarySurface)
                PrimarySurface->Restore();
            if (!OffscreenSurface)
                OffscreenSurface->Restore();
            if (!FormIsOnPrimaryScreen())
            {
                if (OffscreenSurface->GetDC(&offscreen_dc))
                {
                    window_dc = GetDC(Handle);
                    SetStretchBltMode(window_dc, COLORONCOLOR);
                    StretchBlt(window_dc, 0, 0, ClientWidth, ClientHeight, offscreen_dc, 3, 0, 633, 476, SRCCOPY);
                    ReleaseDC(Handle, window_dc);
                    OffscreenSurface->ReleaseDC(offscreen_dc);
                }
            }
            else
            {
                Dx_Draw1->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN);
                PrimarySurface->Blt(&target_rect, OffscreenSurface, 0, DDBLT_WAIT, 0);
                if (PrimarySurface->ErrorValue == DDERR_SURFACELOST ||
                    OffscreenSurface->ErrorValue == DDERR_SURFACELOST)
                    Dx_Draw1->RestoreAllSurfaces();
            }
        }
    }
}

void TMainForm::RenderDrag()
{
    item_drag_drawn = false;
    spell_drag_drawn = false;
    skill_drag_drawn = false;
    if (dragging)
    {
        if ((flip_needed || animation->interface_controller->inventory->TestAndClearDragDirty()) &&
            animation->GetSubState() == GamePanel_Inventory)
        {
            gfx->interface_graphics->DrawDraggedItem();
            flip_needed = true;
            item_drag_drawn = true;
        }
        if ((flip_needed || animation->interface_controller->spells->TestAndClearDragDirty()) &&
            animation->GetSubState() == GamePanel_Spells)
        {
            gfx->interface_graphics->DrawDraggedSpell();
            flip_needed = true;
            spell_drag_drawn = true;
        }
        if ((flip_needed || animation->interface_controller->skills->TestAndClearDragDirty()) &&
            animation->GetSubState() == GamePanel_Skills)
        {
            gfx->interface_graphics->DrawDraggedSkill();
            flip_needed = true;
            skill_drag_drawn = true;
        }
    }
}

void TMainForm::RenderDragUndo()
{
    if (item_drag_drawn)
        gfx->interface_graphics->RestoreItemDragBackground();
    if (spell_drag_drawn)
        gfx->interface_graphics->RestoreSpellDragBackground();
    if (skill_drag_drawn)
        gfx->interface_graphics->RestoreSkillDragBackground();
}

char TMainForm::FormIsOnPrimaryScreen()
{
    tagMONITORINFO monitor_info;
    try
    {
        int monitor = (int)xMonitorFromWindow(Handle, MONITOR_DEFAULTTONEAREST);
        monitor_info.cbSize = 40;
        xGetMonitorInfo((void *)monitor, &monitor_info);
        return monitor_info.dwFlags == MONITORINFOF_PRIMARY;
    }
    catch (...)
    {
    }
    return true;
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

String TMainForm::FormatClock(int value)
{
    if (value < 10)
        return "0" + IntToStr(value);
    return IntToStr(value);
}
