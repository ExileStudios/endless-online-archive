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
#pragma package(smart_init)
#pragma link "TDx_Draw"
#pragma link "TDx_DrawSurface"
#pragma link "TDDSCaps"
#pragma link "TDx_DrawClipper"
#pragma link "TDx_Sound"
#pragma link "TDx_SoundBuffer"
#pragma link "TDSBufferDesc"
#pragma link "TDDPixelFormat"
#pragma link "TDx_Input"
#pragma link "TDIDevCaps"
#pragma link "TDIKeyboardState"
#pragma link "TDx_InputDevice"
#pragma link "TDDSurfaceDesc"
#pragma link "TDx_Input_Library_Defns"
#define BASE_SCREEN_HEIGHT 480.0
#define BASE_SCREEN_WIDTH 640.0
#define SLOW_UPDATE_INTERVAL 20

#pragma resource "*.dfm"
TMainForm *MainForm;

char *TMainForm_UnusedTable1[16] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"};

String TMainForm_UnusedLiterals1()
{
    return "0x";
}

__fastcall TMainForm::TMainForm(Classes::TComponent *owner)
    : Forms::TForm(owner)
{
    mm_timer_id = 0;
    draw_error_count = 0;
    show_frame_rate = false;
    start_time = DateTimeToTimeStamp(Now());
    time_value_a = 0;
    time_value_b = 12;
}

__fastcall TMainForm::~TMainForm()
{
}

// Counts a failed draw; three before Timer1Timer next resets the count switch drawing to BitBlt in the settings.
void TMainForm::CountDrawError()
{
    draw_error_count++;
    if (draw_error_count > 2 && !config->GetUseBitBltDraw())
        config->SetUseBitBltDraw(1);
}

// Alt+F4: on the menu screens it frees everything and quits; in game it asks first.
void __fastcall TMainForm::FormKeyDown(System::TObject *Sender, Word &Key, Classes::TShiftState Shift)
{
    if (Shift.Contains(ssAlt) && Key == 115)
    {
        if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
        {
            Key = (Word)0;
            if (mm_timer_id)
                timeKillEvent(mm_timer_id);
            if (client->Active)
                client->Close();
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
            Key = (Word)0;
            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_Logout,
                MainForm->strings->GetDialogText(DialogText_ExitGame, false),
                MainForm->strings->GetDialogText(DialogText_ExitGame, true));
            MainForm->edits->caret_enabled = false;
        }
    }
}

void __fastcall TMainForm::ActivateForm(System::TObject *sender)
{
    active = true;
}

void __fastcall TMainForm::DeactivateForm(System::TObject *sender)
{
    active = false;
}

void __fastcall TMainForm::MinimizeForm(System::TObject *sender)
{
    minimized = true;
}

void __fastcall TMainForm::RestoreForm(System::TObject *sender)
{
    minimized = false;
}

// Loads the settings and texts, checks the data files, starts DirectInput, DirectSound and DirectDraw, builds every
// controller, then starts the timers.
void __fastcall TMainForm::FormCreate(System::TObject *Sender)
{
    minimized = false;
    active = true;
    Application->OnMinimize = MinimizeForm;
    Application->OnRestore = RestoreForm;
    Application->OnActivate = ActivateForm;
    Application->OnDeactivate = DeactivateForm;
    version_major = 0;
    version_minor = 0;
    version_build = 28;
    files = new FileController();
    config = new Config();
    strings = new StringContainer(config->GetLanguage());
    chat_log = new FFileLog(config->GetLogChat(), config->GetChatLogPath());
    registry = new WinRegister();
    // Without valid data files only the file error message is shown, and the program ends.
    if (files->VerifyFiles())
    {
        // Timer1Timer later counts its ticks in this member; every use of it as a width comes first.
        display_width = 640;
        display_height = 480;
        window_height_padding = 0;
        refresh_rate = 0;
        Width = display_width;
        Height = display_height;
        Position = poScreenCenter;
        if (Dx_Input1->Created)
        {
            Dx_Input1->Destroy();
            Sleep(150);
        }
        Dx_Input1->Create();
        if (!Dx_Input1->Created)
            Application->Terminate();
        Dx_InputDevice1->Create(GUID_SysKeyboard, Dx_Input1);
        if (config->GetSoundEnabled())
        {
            sound_driver_names = new TStringList();
            sound_driver_modules = new TStringList();
            sound_driver_guids = new TList();
            int sound_device_index = 0;
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
                        Dx_Sound1->Create((GUID *)sound_driver_guids->Items[sound_device_index]);
                    sound_device_index++;
                } while (!Dx_Sound1->Created && sound_device_index < sound_driver_guids->Count);
            }
            if (!Dx_Sound1->Created)
            {
                config->SetSoundAvailable(0);
                config->SetSoundEnabled(0);
            }
        }
        if (Dx_Draw1->Created)
        {
            Dx_Draw1->Destroy();
            Sleep(150);
        }
        // Dx_Draw1Create makes the surfaces and, full screen, sets the display mode.
        Dx_Draw1->Create(0);
        if (!Dx_Draw1->Created)
        {
            Dx_Input1->Destroy();
            Dx_Sound1->Destroy();
            Application->Terminate();
        }
        draw_canvas = new TCanvas;
        status_canvas = new TCanvas;
        text_canvas = new TCanvas;
        resource_bitmap = new Graphics::TBitmap;
        interface_bitmap = new Graphics::TBitmap;
        sound_mapper = new SoundMapper();
        magic_mapper = new MagicMapper();
        weapon_mapper = new WeaponMapper();
        keyboard = new KeyBoard();
        mouse = new Mouser();
        sfx = new SfxController(config->GetSoundEnabled());
        animation = new AnimationController(config->GetSkipIntro());
        gfx = new GfxController(config->GetPreloadTileGfx(), config->GetPreloadMapGfx());
        map = new MapController();
        blitter = new ExtBlts(OffscreenSurface);
        players = new Players(config->GetPreloadCharacterGfx());
        npcs = new Npcs(config->GetPreloadCharacterGfx());
        effects = new Effects();
        map_items = new MapItems();
        item_values = new ItemValues();
        npc_values = new NpcValues();
        skill_values = new SkillValues();
        class_values = new ClassValues();
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
        Position = poScreenCenter;
        screen_rect.left = 0;
        screen_rect.top = 0;
        screen_rect.bottom = 480;
        screen_rect.right = 640;
        if (Dx_Draw1->Created && PrimarySurface->Created && OffscreenSurface->Created && Dx_Input1->Created &&
            (!config->GetSoundEnabled() || Dx_Sound1->Created))
        {
            if (!config->GetSkipIntro())
            {
                if (config->GetFullscreen())
                    Screen->Cursor = (TCursor)-1;
            }
            else
                MainForm->sfx->PlayMusic(1);
            if (!config->GetFullscreen())
            {
                Height = 500;
                Width = 640;
                if (ClientHeight < 473)
                {
                    window_height_padding = 473 - ClientHeight;
                    Height = Height + window_height_padding;
                    if (window_height_padding > 16)
                    {
                        Height = 500;
                        window_height_padding = 0;
                    }
                }
            }
            if (net_client->IsIpAddress(config->GetHost()))
                client->Address = config->GetHost();
            else
                client->Host = config->GetHost();
            client->Port = config->GetPort();
            Application->OnIdle = OnIdle;
            mm_timer_id = 0;
            // The fast timer drives animation and input; Timer1 loads and purges graphics.
            mm_timer_id = timeSetEvent(12, 0, MMTimerProc, 0, 1);
            Timer1->Enabled = true;
            randomize();
        }
        else
        {
            Dx_Input1->Destroy();
            Dx_Sound1->Destroy();
            Dx_Draw1->Destroy();
            Application->Terminate();
        }
    }
    else
    {
        String error_text = "Endless File Protection Error (#442)\n";
        error_text = error_text + "Some files are missing or corrupted\n\n";
        error_text = error_text + files->error_message + "\n\n";
        error_text = error_text + "Please, completely uninstall Endless Online\n";
        error_text = error_text + "and re-install the correct client version.\n";
        error_text = error_text + "Goto: http://www.endless-online.com";
        if (MessageDlg(error_text, mtError, TMsgDlgButtons() << mbOK, 0) == 1)
        {
            delete files;
            delete config;
            delete strings;
            delete chat_log;
            delete registry;
            Application->Terminate();
        }
        else
            Application->Terminate();
    }
}

// The version as major.minor.patch, with minor and patch padded to three digits.
String TMainForm::VersionString(int major, int minor, int patch)
{
    String version_text = IntToStr(major);
    String part_text = IntToStr(minor);
    version_text = version_text + ".";
    if (part_text.Length() == 3)
        version_text = version_text + part_text;
    if (part_text.Length() == 1)
        version_text = version_text + "00" + part_text;
    if (part_text.Length() == 2)
        version_text = version_text + "0" + part_text;
    part_text = IntToStr(patch);
    version_text = version_text + ".";
    if (part_text.Length() == 3)
        version_text = version_text + part_text;
    if (part_text.Length() == 1)
        version_text = version_text + "00" + part_text;
    if (part_text.Length() == 2)
        version_text = version_text + "0" + part_text;
    return version_text;
}

// Closes the connection and the chat log, stops the fast timer, and frees DirectX and the canvases.
void __fastcall TMainForm::FormDestroy(System::TObject *Sender)
{
    if (animation->GetScreenMode() == ScreenMode_Intro)
        gfx->FreeIntroGraphics();
    if (client->Active)
        client->Close();
    if (chat_log)
        chat_log->Close();
    if (mm_timer_id)
        timeKillEvent(mm_timer_id);
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
void __fastcall TMainForm::FormClose(System::TObject *Sender, Forms::TCloseAction &Action)
{
    if (animation->GetScreenMode() == ScreenMode_Game)
        animation->interface_controller->Logout();
    if (client->Active)
        client->Close();
    if (chat_log)
        chat_log->Close();
    if (mm_timer_id)
        timeKillEvent(mm_timer_id);
    if (Dx_Input1->Created)
        Dx_Input1->Destroy();
    if (Dx_Sound1->Created)
        Dx_Sound1->Destroy();
    if (Dx_Draw1->Created && config->GetFullscreen())
        Dx_Draw1->FlipToGDISurface();
    if (Dx_Draw1->Created)
        Dx_Draw1->Destroy();
}

// Makes the surfaces: in a window with a clipper on it, or full screen in the display mode with a back buffer.
void __fastcall TMainForm::Dx_Draw1Create(System::TObject *Sender)
{
    if (!config->GetFullscreen())
    {
        try
        {
            if (PrimarySurface->Created)
                PrimarySurface->Destroy();
            if (!config->GetStayOnTop())
                FormStyle = fsNormal;
            if (config->GetSizeable())
                BorderStyle = bsSizeable;
            else
            {
                BorderStyle = bsSingle;
                BorderIcons >> biMaximize;
            }
            Dx_Draw1->SetCooperativeLevel(Handle, 8);
            PrimarySurfaceDesc->Flags = 0;
            PrimarySurfaceDesc->SCaps = PrimaryCaps;
            PrimarySurfaceDesc->SCaps->Caps = 512;
            PrimarySurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            PrimarySurfaceDesc->Flags = 6;
            PrimarySurfaceDesc->SCaps = PrimaryCaps;
            PrimarySurfaceDesc->SCaps->Caps = 64;
            PrimarySurfaceDesc->Width = Width;
            PrimarySurfaceDesc->Height = Height;
            OffscreenSurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            PrimaryClipper->Create(Dx_Draw1);
            PrimaryClipper->SetHWnd(Handle);
            PrimarySurface->SetClipper(PrimaryClipper);
        }
        catch (...)
        {
        }
    }
    else
    {
        try
        {
            if (PrimarySurface->Created)
                PrimarySurface->Destroy();
            BorderStyle = bsNone;
            Dx_Draw1->EnumDisplayModes(0, 0, 0);
            Dx_Draw1->SetCooperativeLevel(Application->Handle, 17);
            PrimarySurfaceDesc->Flags = 33;
            PrimarySurfaceDesc->SCaps = PrimaryCaps;
            PrimaryCaps->Caps = 16920;
            PrimarySurfaceDesc->Width = Width;
            PrimarySurfaceDesc->Height = Height;
            PrimarySurfaceDesc->BackBufferCount = 1;
            Dx_Draw1->SetDisplayMode(display_width, display_height, 16, 0, 0);
            PrimarySurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            if (!PrimarySurface->Created)
            {
                PrimarySurfaceDesc->Flags = 33;
                PrimarySurfaceDesc->BackBufferCount = 1;
                PrimaryCaps->Caps = 536;
                BackBufferCaps->Caps = 4;
                PrimarySurfaceDesc->SCaps = PrimaryCaps;
                BackBufferDesc->SCaps = BackBufferCaps;
                PrimarySurface->Create(PrimarySurfaceDesc, Dx_Draw1);
            }
            OffscreenDesc->Flags = 6;
            OffscreenDesc->SCaps = OffscreenCaps;
            OffscreenDesc->SCaps->Caps = 2112;
            OffscreenDesc->Width = Width;
            OffscreenDesc->Height = Height;
            OffscreenSurface->GetSurfaceDesc(OffscreenDesc);
            OffscreenSurface->Create(OffscreenDesc, Dx_Draw1);
        }
        catch (...)
        {
        }
    }
}

// Full screen, attaches the back buffer to the new primary surface.
void __fastcall TMainForm::PrimarySurfaceCreate(System::TObject *Sender)
{
    if (config->GetFullscreen())
    {
        if (PrimarySurface->GetAttachedSurface(BackBufferCaps, BackBuffer))
            MainForm->BorderStyle = bsNone;
    }
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
        mode_width = SurfaceDesc->Width;
        mode_height = SurfaceDesc->Height;
        refreshed = SurfaceDesc->RefreshRate;
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
            if (display_width == mode_width && display_height == mode_height && bit_depth == 16)
                refresh_rate = refreshed;
            display_mode_count++;
        }
        else
            Finished = 1;
    }
}

// Frees the surfaces.
void __fastcall TMainForm::Dx_Draw1Destroy(System::TObject *Sender)
{
    OffscreenSurface->Destroy();
    if (BackBuffer->Created)
        BackBuffer->Destroy();
    PrimarySurface->Destroy();
}

void __fastcall TMainForm::PrimarySurfaceSurfaceLost(System::TObject *Sender)
{

}

void __stdcall MMTimerProc(UINT timer_id, UINT message, DWORD_PTR user_data, DWORD_PTR first_reserved,
    DWORD_PTR second_reserved)
{
    MainForm->HandleMMTimerEvent();
}

// Loads pending graphics in turns, drops distant players, items and npcs, and purges graphics not used lately.
// The tick count is kept in display_width: the turns follow its remainder by four, the drops come every twentieth
// tick, and the purge comes with the reset above forty.
void __fastcall TMainForm::Timer1Timer(System::TObject *Sender)
{
    if (display_width % 4 == 1)
    {
        players->LoadPendingGraphics();
        npcs->LoadPendingGraphics();
        gfx->tiles->LoadPendingGraphic();
        gfx->tops->LoadPendingGraphic();
        gfx->blends->LoadPendingGraphic();
        gfx->items->LoadPendingGraphics();
    }
    if (display_width % 4 == 2)
    {
        gfx->walls->LoadPendingGraphic();
        gfx->objects->LoadPendingGraphic();
        gfx->tiles->LoadPendingGraphic();
        gfx->tops->LoadPendingGraphic();
        gfx->blends->LoadPendingGraphic();
        gfx->effects->LoadPendingGraphic();
    }
    if (display_width % 4 == 3)
    {
        gfx->masks->LoadPendingGraphic();
        gfx->objects->LoadPendingGraphic();
        gfx->walls->LoadPendingGraphic();
        gfx->tiles->LoadPendingGraphic();
        gfx->items->LoadPendingGraphics();
        gfx->icons->LoadPendingGraphic();
        gfx->effects->LoadPendingGraphic();
    }
    if (!(display_width % SLOW_UPDATE_INTERVAL))
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
    // The frame rate is the frames drawn over the last hundred ticks.
    if (timer_ticks >= 100)
    {
        frame_rate = frame_count;
        timer_ticks = 0;
        frame_count = 0;
    }
    if (!(timer_ticks % 5))
        animation->UpdateTimers();
    // Every tenth tick: the animations, the controllers' timers and the mouse.
    if (!(timer_ticks % 10))
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
    }
    if (timer_ticks % 10 == 1)
        animation_tick_due = true;
    if (!(timer_ticks % SLOW_UPDATE_INTERVAL))
    {
        sfx->UpdateSounds();
        if (animation->IsAnimating())
            animation->UpdateIntroAndDelays();
        effects->RemoveFinished();
    }
    if (!(timer_ticks % 50))
    {
        edits->TimerTick();
        animation->interface_controller->OnTimer();
        map->AdvanceTileFrame();
    }
}

// Sets up the keyboard device and acquires it.
void __fastcall TMainForm::Dx_InputDevice1Create(System::TObject *Sender)
{
    Dx_InputDevice1->SetDataFormat(DIKeyboardState1);
    Dx_InputDevice1->SetCooperativeLevel(Application->Handle, 10);
    keyboard_acquired = Dx_InputDevice1->Acquire();
    if (keyboard_acquired)
    {
        Dx_InputDevice1->Poll();
        return;
    }
}

void __fastcall TMainForm::Dx_InputDevice1Destroy(System::TObject *Sender)
{
    if (keyboard_acquired)
    {
        Dx_InputDevice1->Unacquire();
    }
    return;
}

void __fastcall TMainForm::Dx_Input1Destroy(System::TObject *Sender)
{
    if (Dx_InputDevice1->Created)
        Dx_InputDevice1->Destroy();
}

void __fastcall TMainForm::Dx_Sound1Create(System::TObject *Sender)
{
    Dx_Sound1->SetCooperativeLevel(Application->Handle, 2);
}

// Frees the sound buffers before the sound device goes.
void __fastcall TMainForm::Dx_Sound1Destroy(System::TObject *Sender)
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
void __fastcall TMainForm::Dx_Sound1DSEnumerate(System::TObject *Sender, GUID *Guid, String DriverDescription,
    String DriverModule, void *Context, bool &Finished)
{
    if (sound_driver_names && sound_driver_modules && sound_driver_guids)
    {
        sound_driver_names->Add(DriverDescription);
        sound_driver_modules->Add(DriverModule);
        sound_driver_guids->Add((void *)Guid);
    }
    else
        Finished = 1;
}

void TMainForm::IntroAnimator()
{

}

// Scales the cursor in a window to game coordinates and hands it to the mouse.
void __fastcall TMainForm::FormMouseMove(System::TObject *Sender, Classes::TShiftState Shift, int X, int Y)
{
    int __slot1;
    int __slot2;
    int __slot3;
    int __slot4;
    int __slot5;
    int __slot6;
    int __slot7;
    int __slot8;
    int __slot9;
    int __slot10;
    int mouse_x;
    int mouse_y = Y;
    mouse_x = X;
    if (!config->GetFullscreen())
    {
        double scale_x = ClientWidth / BASE_SCREEN_WIDTH;
        double scale_y = ClientHeight / BASE_SCREEN_HEIGHT;
        double scaled_x = mouse_x / scale_x;
        double scaled_y = mouse_y / scale_y;
        mouse_x = scaled_x;
        mouse_y = scaled_y;
    }
    mouse->SetPosition(mouse_x, mouse_y);
}

// Scales a click in a window to game coordinates, focuses the edit box under it, then hands the click to the mouse.
void __fastcall TMainForm::FormMouseDown(System::TObject *Sender, Controls::TMouseButton Button,
    Classes::TShiftState Shift, int X, int Y)
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
                sfx->PlaySfx(4);
                break;
            }
            edits->MoveNext();
        } while (!edits->IsAtEnd());
    }
    if (!Button)
        Sys_mouse_HandleClick((int *)mouse, X, Y, 0);
    else
        Sys_mouse_HandleClick((int *)mouse, X, Y, 1);
}

void __fastcall TMainForm::FormMouseWheelDown(System::TObject *Sender, Classes::TShiftState Shift, Windows::TPoint &MousePos,
    bool &Handled)
{
    int __slot1;
    mouse->HandleWheel(2);
}

void __fastcall TMainForm::FormMouseWheelUp(System::TObject *Sender, Classes::TShiftState Shift, Windows::TPoint &MousePos,
    bool &Handled)
{
    int __slot1;
    mouse->HandleWheel(1);
}

void __fastcall TMainForm::FormMouseUp(System::TObject *Sender, Controls::TMouseButton Button,
    Classes::TShiftState Shift, int X, int Y)
{
    mouse->HandleMouseUp();
}

// Draws a frame when something changed: the map, its people and the windows in game, the menus, or the intro.
// Then it shows the frame, puts back what the dragged icons covered, and sleeps briefly.
void __fastcall TMainForm::OnIdle(System::TObject *Sender, bool &Done)
{
    if (!PrimarySurface->Created)
        return;
    if (!OffscreenSurface->Created)
        return;
    if (sfx->TakeMusicPending())
        sfx->PlayMusic(sfx->GetQueuedMusic());
    if (!sfx->IsMusicPlaying() && sfx->IsMusicRetryReady())
    {
        sfx->StartMusicRetryDelay();
        if (map->GetMusicMode() > 2 && map->GetMusicMode() < 6)
            sfx->QueueMusic(map->GetMusicId());
    }
    // Print Screen saves a screenshot.
    if (GetAsyncKeyState(44))
        gfx->interface_graphics->SaveScreenshot();
    if (animation->GetScreenMode() == ScreenMode_Game)
    {
        if (animation->interface_controller->IsFullRedrawPending())
        {
            flip_needed = true;
            redraw_needed = true;
            OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_background,
                &gfx->interface_graphics->hud_background_rect, 16);
            animation->interface_controller->ClearFullRedraw();
        }
        if (animation_tick_due)
        {
            animation_tick_due = false;
            // While warping or loading, only the frame, the bars and the loading text are drawn.
            if (map->warp_pending || map->loading_map || animation->interface_controller->timed_text_visible)
            {
                OffscreenSurface->Blt(&gfx->game->view_clip_rect, 0, 0, 16778240, gfx->game->view_fill_fx);
                OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_frame,
                    &gfx->interface_graphics->hud_frame_rect, 17);
                OffscreenSurface->BltFast(7, 53, gfx->interface_graphics->hud_left_frame,
                    &gfx->interface_graphics->hud_left_frame_rect, 17);
                OffscreenSurface->BltFast(49, 7, gfx->interface_graphics->hud_top_frame,
                    &gfx->interface_graphics->hud_top_frame_rect, 17);
                OffscreenSurface->BltFast(55, 0, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->status_icon_left_rect, 17);
                OffscreenSurface->BltFast(77, 0, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->status_icon_right_rect, 17);
                OffscreenSurface->BltFast(101, 0, gfx->interface_graphics->hud_sheet,
                    &gfx->interface_graphics->status_bars_frame_rect, 17);
                OffscreenSurface->BltFast(125, 4, gfx->interface_graphics->hud_sheet,
                    &InterfaceGraphics::GetStatusBarFillRect(gfx->interface_graphics, 0,
                    animation->interface_controller->game_bars->GetHealthBarSegments()), 17);
                OffscreenSurface->BltFast(234, 4, gfx->interface_graphics->hud_sheet,
                    &InterfaceGraphics::GetStatusBarFillRect(gfx->interface_graphics, 1,
                    animation->interface_controller->game_bars->GetManaBarSegments()), 17);
                OffscreenSurface->BltFast(345, 4, gfx->interface_graphics->hud_sheet,
                    &InterfaceGraphics::GetStatusBarFillRect(gfx->interface_graphics, 2,
                    animation->interface_controller->game_bars->GetStaminaBarSegments()), 17);
                OffscreenSurface->BltFast(454, 4, gfx->interface_graphics->hud_sheet,
                    &InterfaceGraphics::GetStatusBarFillRect(gfx->interface_graphics, 3,
                    animation->interface_controller->game_bars->GetExperienceBarSegments()), 17);
                animation->interface_controller->game_bars->MarkDirty();
                gfx->interface_graphics->DrawCornerButton();
                HDC timed_text_dc;
                if (animation->interface_controller->timed_text_visible && OffscreenSurface->GetDC(&timed_text_dc))
                {
                    try
                    {
                        status_canvas->Handle = timed_text_dc;
                        status_canvas->Brush->Style = bsClear;
                        status_canvas->Font->Name = "Arial";
                        status_canvas->Font->Color = clWhite;
                        status_canvas->Font->Size = 9;
                        int timed_text_width = this->status_canvas->TextWidth(this->animation->interface_controller->timed_text);
                        status_canvas->TextOut(320 - timed_text_width / 2, 156,
                            animation->interface_controller->timed_text);
                        status_canvas->Handle = 0;
                    }
                    catch (Exception &)
                    {
                    }
                    OffscreenSurface->ReleaseDC(timed_text_dc);
                }
                if (map->loading_map)
                {
                    OffscreenSurface->BltFast(230, 138, gfx->interface_graphics->loading_icon,
                        &gfx->interface_graphics->loading_icon_rect, 16);
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
                if (map->map_request_pending)
                {
                    map->map_request_pending = false;
                    String map_request = Client::EncodeNumber(MainForm->net_client, map->map_id, 2);
                    map_request = map_request + Client::EncodeNumber(MainForm->net_client, map->warp_session_id, 2);
                    net_client->SendPacket(AC_GET, OP_MAP, map_request);
                }
                flip_needed = true;
            }
            else if ((map->ConsumeCursorMoved() || npcs->ConsumeRedraw() || players->ConsumeRedraw() || redraw_needed ||
                edits->ConsumeTextChanged()) && !map->warp_pending)
            {
                redraw_needed = false;
                int player_index = 0;
                int npc_index = 0;
                int effect_index = 0;
                int item_index = 0;
                int own_player_index = -1;
                char own_has_weapon = 0;
                char own_has_shield = 0;
                int shake_offset = 0;
                if (animation->interface_controller->is_shaking)
                    shake_offset = animation->interface_controller->shake_offset;
                OffscreenSurface->Blt(&gfx->game->view_clip_rect, 0, 0, 16778240, gfx->game->view_fill_fx);
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
                for (int i = first_y; i <= last_y; i++)
                {
                    for (int j = first_x; j <= last_x; j++)
                    {
                        int ground_x = 32 * j - 32 * i + map->GetScrollOffsetX() + 278;
                        int ground_y = 16 * j + 16 * i + this->map->GetScrollOffsetY() + 134;
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
                                        Gfx_tile_GetFrameRect(gfx->tiles->GetGraphic(map->ground_tiles[j][i]).resource,
                                        map->tile_frame));
                                    if (gfx->tiles->GetGraphic(map->ground_tiles[j][i]).resource->is_animated)
                                        MainForm->map->has_animated_tiles = true;
                                }
                                else
                                    redraw_needed = true;
                            }
                        }
                    }
                }
                if (config->GetShowShadows())
                {
                    for (int k = first_y; k <= last_y; k++)
                    {
                        for (int m = first_x; m <= last_x; m++)
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
                                            gfx->blends->GetGraphic(map->blend_tiles[m][k]).resource->surface_rect, 48);
                                }
                            }
                        }
                    }
                }
                // Row by row from the back: each tile's items, overlay, object, people, effects, masks and walls.
                for (int n = first_y - 1; n <= last_y; n++)
                {
                    for (int ii = first_x; ii <= last_x; ii++)
                    {
                        int screen_x = 32 * ii - 32 * n + map->GetScrollOffsetX() + 278;
                        int screen_y = 16 * ii + 16 * n + this->map->GetScrollOffsetY() + 134;
                        if (shake_offset)
                            screen_x += shake_offset;
                        if (n >= 0)
                        {
                            if (map->HasPathMarker() && ii == map->GetPathMarkerX() && n == map->GetPathMarkerY())
                            {
                                if (map->GetPathMarkerTimer() > 4)
                                    gfx->game->BlitClippedAlpha(screen_x, screen_y,
                                        gfx->interface_graphics->tile_cursor,
                                        Gfx_interface_GetTileCursorRect(gfx->interface_graphics, 3),
                                        24 * map->GetPathMarkerTimer() + 12);
                                else
                                    gfx->game->BlitClippedAlpha(screen_x, screen_y,
                                        gfx->interface_graphics->tile_cursor,
                                        Gfx_interface_GetTileCursorRect(gfx->interface_graphics, 4),
                                        24 * map->GetPathMarkerTimer() + 12);
                            }
                            if (ii == map->GetCursorX() && n == map->GetCursorY() && !map->blocked[ii][n] &&
                                !map->minimap_mask[ii][n] &&
                                (!animation->game->player_menu_visible ||
                                animation->game->player_menu_hovered_option < 0))
                            {
                                if (MainForm->map_items->HasItemAt(ii, n))
                                    gfx->game->BlitClipped(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                                        Gfx_interface_GetTileCursorRect(gfx->interface_graphics, 2));
                                else if (map->has_tile_spec[ii][n] || players->HasVisiblePlayerAt(ii, n) ||
                                    npcs->HasNpcAt(ii, n))
                                    gfx->game->BlitClipped(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                                        Gfx_interface_GetTileCursorRect(gfx->interface_graphics, 1));
                                else
                                    gfx->game->BlitClipped(screen_x, screen_y, gfx->interface_graphics->tile_cursor,
                                        Gfx_interface_GetTileCursorRect(gfx->interface_graphics, 0));
                            }
                            if ((int)MainForm->map_items->GetCount() > item_index)
                            {
                                char item_found;
                                do
                                {
                                    item_found = 0;
                                    MapItem map_item = map_items->GetItem(item_index);
                                    if (ii == map_item.x && n == map_item.y)
                                    {
                                        int item_graphic = item_values->GetGraphic(map_item.item_id) - 1;
                                        if (item_graphic >= 0)
                                        {
                                            if (map_item.item_id == 1)
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
                                        item_found = 1;
                                        item_index++;
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
                                if (net_client->IsTallObjectInDrawRange(map->GetCenterX(), map->GetCenterY(), ii, n))
                                {
                                    char draw_object = 1;
                                    int object_spec = this->map->GetTileSpec(ii, n);
                                    if (object_spec == 33 && map->trap_reveal_timer < 1)
                                        draw_object = 0;
                                    if (object_spec == 35 && players->FindPlayerAt(ii, n) < PlayerId_Unset)
                                        draw_object = 0;
                                    if (draw_object)
                                    {
                                        if (gfx->objects->RequestGraphic(map->objects[ii][n]))
                                        {
                                            GraphicSize object_size = gfx->objects->GetGraphicSize(map->objects[ii][n]);
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
                                    player_found = 0;
                                    Player player = MainForm->players->GetPlayer(player_index);
                                    if (net_client->GetPlayerId() == player.player_id)
                                    {
                                        own_player_index = player_index;
                                        if (player.weapon > 0)
                                            own_has_weapon = 1;
                                        if (player.shield > 0)
                                            own_has_shield = 1;
                                    }
                                    if ((unsigned int)ii == player.x && (unsigned int)n == player.y)
                                    {
                                        // A hidden player is drawn only for themselves and for admin levels above 3.
                                        if (!player.hidden || net_client->GetPlayerId() == player.player_id ||
                                            MainForm->net_client->GetAdminLevel() > HIDDEN_VIEW_ADMIN_LEVEL)
                                        {
                                            if (player.effect_running && player.effect_base_layer &&
                                                gfx->effects->RequestGraphic(player.effect_id))
                                            {
                                                GraphicSize behind_effect_size = gfx->effects->GetGraphicSize(player.effect_id);
                                                int behind_effect_x = 32 * player.x - 32 * player.y +
                                                    MainForm->map->GetScrollOffsetX() + 309;
                                                int behind_effect_y = 16 * player.x + 16 * player.y +
                                                    MainForm->map->GetScrollOffsetY() + 120;
                                                behind_effect_x -= behind_effect_size.width / 2;
                                                behind_effect_y -= behind_effect_size.height / 2;
                                                if (player.walking)
                                                {
                                                    if (player.walk_direction == FacingDirection_Left)
                                                    {
                                                        behind_effect_x = behind_effect_x + 32 -
                                                            (8 * player.walk_frame + 8);
                                                        behind_effect_y = behind_effect_y + 16 -
                                                            (4 * player.walk_frame + 4);
                                                    }
                                                    if (player.walk_direction == FacingDirection_Up)
                                                    {
                                                        behind_effect_x = 8 * player.walk_frame + behind_effect_x - 24;
                                                        behind_effect_y = behind_effect_y + 16 -
                                                            (4 * player.walk_frame + 4);
                                                    }
                                                    if (player.walk_direction == FacingDirection_Right)
                                                    {
                                                        behind_effect_x = 8 * player.walk_frame + behind_effect_x - 24;
                                                        behind_effect_y = 4 * player.walk_frame + behind_effect_y - 12;
                                                    }
                                                    if (player.walk_direction == FacingDirection_Down)
                                                    {
                                                        behind_effect_x = behind_effect_x + 32 -
                                                            (8 * player.walk_frame + 8);
                                                        behind_effect_y = 4 * player.walk_frame + behind_effect_y - 12;
                                                    }
                                                }
                                                if (player.effect_id == SpellEffect_FireBall)
                                                    behind_effect_y -= 160 - 30 * player.effect_frame;
                                                if (player.effect_id == 22)
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
                                                    int behind_effect_frame = player.effect_loops_left;
                                                    if (behind_effect_frame > 0)
                                                        behind_effect_frame = random(2) + 3;
                                                    this->gfx->game->BlitClipped(behind_effect_x + shake_offset,
                                                        behind_effect_y,
                                                        this->gfx->effects->GetGraphic(player.effect_id).resource->layer0_surface,
                                                        Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(player.effect_id).resource,
                                                        behind_effect_frame));
                                                }
                                                else
                                                    this->gfx->game->BlitClipped(behind_effect_x + shake_offset,
                                                        behind_effect_y,
                                                        this->gfx->effects->GetGraphic(player.effect_id).resource->layer0_surface,
                                                        Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(player.effect_id).resource,
                                                        player.effect_frame));
                                            }
                                            if (player.dying || player.hiding || player.unhiding || player.hidden)
                                                gfx->sprites->DrawPlayer(player, 1, shake_offset);
                                            else
                                                gfx->sprites->DrawPlayer(player, 0, shake_offset);
                                            if (player.attacking && player.attack_frame >= 2 && player.weapon > 0)
                                            {
                                                int weapon_effect_x = 32 * player.x - 32 * player.y +
                                                    MainForm->map->GetScrollOffsetX() + 267;
                                                int weapon_effect_y = 16 * player.x + 16 * player.y +
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
                                                    int weapon_effect_type = weapon_mapper->weapon_effect_types[player.weapon];
                                                    gfx->game->BlitClippedHalfBlend(weapon_effect_x, weapon_effect_y,
                                                        gfx->interface_graphics->weapon_effects,
                                                        Gfx_interface_GetWeaponEffectRect(gfx->interface_graphics,
                                                        weapon_effect_type, player.direction));
                                                }
                                            }
                                            if (player.effect_running &&
                                                (player.effect_blend_layer || player.effect_solid_layer) &&
                                                gfx->effects->RequestGraphic(player.effect_id))
                                            {
                                                GraphicSize front_effect_size = gfx->effects->GetGraphicSize(player.effect_id);
                                                int front_effect_x = 32 * player.x - 32 * player.y +
                                                    MainForm->map->GetScrollOffsetX() + 309;
                                                int v43b = 16 * player.x + 16 * player.y +
                                                    MainForm->map->GetScrollOffsetY() + 120;
                                                front_effect_x -= front_effect_size.width / 2;
                                                v43b -= front_effect_size.height / 2;
                                                if (player.walking)
                                                {
                                                    if (player.walk_direction == FacingDirection_Left)
                                                    {
                                                        front_effect_x = front_effect_x + 32 -
                                                            (8 * player.walk_frame + 8);
                                                        v43b = v43b + 16 - (4 * player.walk_frame + 4);
                                                    }
                                                    if (player.walk_direction == FacingDirection_Up)
                                                    {
                                                        front_effect_x = 8 * player.walk_frame + front_effect_x - 24;
                                                        v43b = v43b + 16 - (4 * player.walk_frame + 4);
                                                    }
                                                    if (player.walk_direction == FacingDirection_Right)
                                                    {
                                                        front_effect_x = 8 * player.walk_frame + front_effect_x - 24;
                                                        v43b = 4 * player.walk_frame + v43b - 12;
                                                    }
                                                    if (player.walk_direction == FacingDirection_Down)
                                                    {
                                                        front_effect_x = front_effect_x + 32 -
                                                            (8 * player.walk_frame + 8);
                                                        v43b = 4 * player.walk_frame + v43b - 12;
                                                    }
                                                }
                                                if (player.effect_id == SpellEffect_FireBall)
                                                    v43b -= 160 - 30 * player.effect_frame;
                                                if (player.effect_id == 22)
                                                    v43b -= 160 - 30 * player.effect_frame;
                                                if (player.effect_id == SpellEffect_EnergyBall)
                                                    v43b -= 10 * player.effect_frame;
                                                if (player.effect_id == SpellEffect_Boulders)
                                                    v43b -= 160 - 30 * player.effect_frame;
                                                if (player.effect_id == SpellEffect_Aura)
                                                    v43b -= 12;
                                                if (player.effect_id == SpellEffect_Heaven)
                                                    v43b -= 114;
                                                if (player.effect_id == SpellEffect_DarkBeam)
                                                    v43b -= 114;
                                                if (player.effect_id == SpellEffect_GreenFlame)
                                                    v43b -= 44;
                                                if (player.effect_id == SpellEffect_Whirl)
                                                {
                                                    v43b -= 10;
                                                    if (!player.effect_frame)
                                                        front_effect_x -= 20;
                                                    if (player.effect_frame == 1)
                                                        v43b += 14;
                                                    if (player.effect_frame == 2)
                                                        front_effect_x += 20;
                                                    if (player.effect_frame == 3)
                                                        v43b -= 14;
                                                }
                                                if (player.effect_id == SpellEffect_DarkBeam ||
                                                    player.effect_id == SpellEffect_Heaven ||
                                                    player.effect_id == SpellEffect_Aura)
                                                {
                                                    int front_effect_frame = player.effect_loops_left;
                                                    int flicker_roll = random(2);
                                                    if (front_effect_frame > 0)
                                                        front_effect_frame = 4;
                                                    if (player.effect_id == SpellEffect_DarkBeam)
                                                        front_effect_frame = random(2) + 3;
                                                    if (flicker_roll > 0 && player.effect_blend_layer)
                                                        this->gfx->game->BlitClippedHalfBlend(front_effect_x +
                                                            shake_offset, v43b,
                                                            this->gfx->effects->GetGraphic(player.effect_id).resource->layer1_surface,
                                                            Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(player.effect_id).resource,
                                                            front_effect_frame));
                                                }
                                                else
                                                {
                                                    if (player.effect_blend_layer)
                                                        this->gfx->game->BlitClippedHalfBlend(front_effect_x +
                                                            shake_offset, v43b,
                                                            this->gfx->effects->GetGraphic(player.effect_id).resource->layer1_surface,
                                                            Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(player.effect_id).resource,
                                                            player.effect_frame));
                                                    if (player.effect_solid_layer)
                                                        this->gfx->game->BlitClipped(front_effect_x + shake_offset,
                                                            v43b,
                                                            this->gfx->effects->GetGraphic(player.effect_id).resource->layer2_surface,
                                                            Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(player.effect_id).resource,
                                                            player.effect_frame));
                                                }
                                            }
                                        }
                                        player_found = 1;
                                        player_index++;
                                    }
                                } while ((int)MainForm->players->GetCount() > player_index && player_found);
                            }
                            if ((int)MainForm->npcs->GetCount() > npc_index)
                            {
                                char npc_found;
                                do
                                {
                                    npc_found = 0;
                                    Npc npc = MainForm->npcs->GetNpc(npc_index);
                                    if ((unsigned int)ii == npc.x && (unsigned int)n == npc.y)
                                    {
                                        if (npc.effect_running)
                                        {
                                            if (npc.effect_base_layer)
                                            {
                                                if (gfx->effects->RequestGraphic(npc.effect_id))
                                                {
                                                    GraphicSize npc_behind_effect_size = gfx->effects->GetGraphicSize(npc.effect_id);
                                                    int npc_behind_effect_x = 32 * npc.x - 32 * npc.y +
                                                        MainForm->map->GetScrollOffsetX() + 309;
                                                    int npc_behind_effect_y = 16 * npc.x + 16 * npc.y +
                                                        MainForm->map->GetScrollOffsetY() + 120;
                                                    npc_behind_effect_x -= npc_behind_effect_size.width / 2;
                                                    npc_behind_effect_y -= npc_behind_effect_size.height / 2;
                                                    if (npc.walking)
                                                    {
                                                        if (npc.walk_direction == FacingDirection_Left)
                                                        {
                                                            npc_behind_effect_x = npc_behind_effect_x + 32 -
                                                                (8 * npc.walk_frame + 8);
                                                            npc_behind_effect_y = npc_behind_effect_y + 16 -
                                                                (4 * npc.walk_frame + 4);
                                                        }
                                                        if (npc.walk_direction == FacingDirection_Up)
                                                        {
                                                            npc_behind_effect_x = 8 * npc.walk_frame +
                                                                npc_behind_effect_x - 24;
                                                            npc_behind_effect_y = npc_behind_effect_y + 16 -
                                                                (4 * npc.walk_frame + 4);
                                                        }
                                                        if (npc.walk_direction == FacingDirection_Right)
                                                        {
                                                            npc_behind_effect_x = 8 * npc.walk_frame +
                                                                npc_behind_effect_x - 24;
                                                            npc_behind_effect_y = npc_behind_effect_y +
                                                                4 * npc.walk_frame - 12;
                                                        }
                                                        if (npc.walk_direction == FacingDirection_Down)
                                                        {
                                                            npc_behind_effect_x = npc_behind_effect_x + 32 -
                                                                (8 * npc.walk_frame + 8);
                                                            npc_behind_effect_y = npc_behind_effect_y +
                                                                4 * npc.walk_frame - 12;
                                                        }
                                                    }
                                                    if (npc.effect_id == SpellEffect_FireBall)
                                                        npc_behind_effect_y -= 160 - 30 * npc.effect_frame;
                                                    if (npc.effect_id == 22)
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
                                                    this->gfx->game->BlitClipped(npc_behind_effect_x + shake_offset,
                                                        npc_behind_effect_y,
                                                        this->gfx->effects->GetGraphic(npc.effect_id).resource->layer0_surface,
                                                        Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(npc.effect_id).resource,
                                                        npc.effect_frame));
                                                }
                                            }
                                        }
                                        if (npc.dying)
                                        {
                                            if (npc.death_countdown > 2)
                                                gfx->npcs->DrawNpc(npc, 30 * npc.death_countdown - 60, shake_offset);
                                        }
                                        else if (npc.graphic == 85 || npc.graphic == NpcGraphic_CursedMask ||
                                            npc.graphic == NpcGraphic_AncientWraith)
                                        {
                                            int npc_alpha = random(20) + 44;
                                            if (npc.attacking || npc.alt_attacking)
                                                npc_alpha = 90;
                                            gfx->npcs->DrawNpc(npc, npc_alpha, shake_offset);
                                        }
                                        else
                                            gfx->npcs->DrawNpc(npc, 255, shake_offset);
                                        if (npc.effect_running && (npc.effect_blend_layer || npc.effect_solid_layer) &&
                                            gfx->effects->RequestGraphic(npc.effect_id))
                                        {
                                            GraphicSize npc_front_effect_size = gfx->effects->GetGraphicSize(npc.effect_id);
                                            int npc_front_effect_x = 32 * npc.x - 32 * npc.y +
                                                MainForm->map->GetScrollOffsetX() + 309;
                                            int npc_front_effect_y[1];
                                            npc_front_effect_y[0] = 16 * npc.x + 16 * npc.y +
                                                MainForm->map->GetScrollOffsetY() + 120;
                                            npc_front_effect_x -= npc_front_effect_size.width / 2;
                                            npc_front_effect_y[0] -= npc_front_effect_size.height / 2;
                                            if (npc.walking)
                                            {
                                                if (npc.walk_direction == FacingDirection_Left)
                                                {
                                                    npc_front_effect_x = npc_front_effect_x + 32 -
                                                        (8 * npc.walk_frame + 8);
                                                    npc_front_effect_y[0] = npc_front_effect_y[0] + 16 -
                                                        (4 * npc.walk_frame + 4);
                                                }
                                                if (npc.walk_direction == FacingDirection_Up)
                                                {
                                                    npc_front_effect_x = 8 * npc.walk_frame + npc_front_effect_x - 24;
                                                    npc_front_effect_y[0] = npc_front_effect_y[0] + 16 -
                                                        (4 * npc.walk_frame + 4);
                                                }
                                                if (npc.walk_direction == FacingDirection_Right)
                                                {
                                                    npc_front_effect_x = 8 * npc.walk_frame + npc_front_effect_x - 24;
                                                    npc_front_effect_y[0] = npc_front_effect_y[0] + 4 * npc.walk_frame -
                                                        12;
                                                }
                                                if (npc.walk_direction == FacingDirection_Down)
                                                {
                                                    npc_front_effect_x = npc_front_effect_x + 32 -
                                                        (8 * npc.walk_frame + 8);
                                                    npc_front_effect_y[0] = npc_front_effect_y[0] + 4 * npc.walk_frame -
                                                        12;
                                                }
                                            }
                                            if (npc.effect_id == SpellEffect_FireBall)
                                                npc_front_effect_y[0] -= 160 - 30 * npc.effect_frame;
                                            if (npc.effect_id == 22)
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
                                                this->gfx->game->BlitClippedHalfBlend(npc_front_effect_x + shake_offset,
                                                    npc_front_effect_y[0],
                                                    this->gfx->effects->GetGraphic(npc.effect_id).resource->layer1_surface,
                                                    Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(npc.effect_id).resource,
                                                    npc.effect_frame));
                                            if (npc.effect_solid_layer)
                                                this->gfx->game->BlitClipped(npc_front_effect_x + shake_offset,
                                                    npc_front_effect_y[0],
                                                    this->gfx->effects->GetGraphic(npc.effect_id).resource->layer2_surface,
                                                    Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(npc.effect_id).resource,
                                                    npc.effect_frame));
                                        }
                                        npc_found = 1;
                                        npc_index++;
                                    }
                                } while ((int)MainForm->npcs->GetCount() > npc_index && npc_found);
                            }
                            if (MainForm->effects->GetCount() > effect_index)
                            {
                                char effect_found;
                                do
                                {
                                    effect_found = 0;
                                    MapEffect map_effect = MainForm->effects->GetEffect(effect_index);
                                    if (ii == map_effect.x && n == map_effect.y)
                                    {
                                        if (!map_effect.finished && gfx->effects->RequestGraphic(map_effect.effect_id))
                                        {
                                            GraphicSize effect_size = gfx->effects->GetGraphicSize(map_effect.effect_id);
                                            int effect_x = 32 * map_effect.x - 32 * map_effect.y +
                                                MainForm->map->GetScrollOffsetX() + 309;
                                            int effect_y = 16 * map_effect.x + 16 * map_effect.y +
                                                MainForm->map->GetScrollOffsetY() + 120;
                                            effect_x -= effect_size.width / 2;
                                            effect_y -= effect_size.height / 2;
                                            if (map_effect.has_blend_layer)
                                                this->gfx->game->BlitClippedHalfBlend(effect_x + shake_offset, effect_y,
                                                    this->gfx->effects->GetGraphic(map_effect.effect_id).resource->layer1_surface,
                                                    Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(map_effect.effect_id).resource,
                                                    map_effect.frame));
                                            if (map_effect.has_solid_layer)
                                                this->gfx->game->BlitClipped(effect_x + shake_offset, effect_y,
                                                    this->gfx->effects->GetGraphic(map_effect.effect_id).resource->layer2_surface,
                                                    Gfx_effect_GetFrameRect(this->gfx->effects->GetGraphic(map_effect.effect_id).resource,
                                                    map_effect.frame));
                                        }
                                        effect_found = 1;
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
                                            Gfx_wall_GetFrameRect(gfx->walls->GetGraphic(map->walls[ii][n]).resource,
                                            map->tile_frame));
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
                                        GraphicSize lower_wall_size = gfx->walls->GetGraphicSize(map->walls_next_row[ii][n + 1]);
                                        int lower_wall_y = screen_y + 31 - lower_wall_size.height;
                                        gfx->game->BlitClipped(screen_x, lower_wall_y + 16,
                                            gfx->walls->GetGraphic(map->walls_next_row[ii][n + 1]).resource->surface,
                                            Gfx_wall_GetFrameRect(gfx->walls->GetGraphic(map->walls_next_row[ii][n + 1]).resource,
                                            map->tile_frame));
                                        if (gfx->walls->GetGraphic(map->walls_next_row[ii][n + 1]).resource->is_animated)
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
                for (int jj = first_y - 1; jj <= last_y; jj++)
                {
                    for (int kk = first_x; kk <= last_x; kk++)
                    {
                        int top_mask_cell_x = 32 * kk - 32 * jj + map->GetScrollOffsetX() + 278;
                        int top_mask_cell_y = 16 * kk + 16 * jj + this->map->GetScrollOffsetY() + 134;
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
                // The own character is drawn once more, over everything on the map.
                if (own_player_index != -1)
                {
                    Player own_player = players->GetPlayer(own_player_index);
                    if (net_client->GetPlayerId() == own_player.player_id)
                        gfx->sprites->DrawPlayer(own_player, 1, shake_offset);
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
                // The minimap: walkable cells, specs, players and npcs.
                if (MainForm->gfx->interface_graphics->minimap_visible && !MainForm->map->IsMinimapDisabled())
                {
                    int minimap_player_index = 0;
                    int minimap_npc_index = 0;
                    for (int mm = 0; mm <= this->map->GetHeight(); mm++)
                    {
                        for (int nn = 0; nn <= this->map->GetWidth(); nn++)
                        {
                            int minimap_origin_x = 0 - 14 * this->map->GetCenterX() + 14 * this->map->GetCenterY();
                            int minimap_origin_y = 0 - 7 * this->map->GetCenterX() - 7 * this->map->GetCenterY();
                            int v43bb = 14 * nn - 14 * mm + minimap_origin_x + 278;
                            int minimap_y = 7 * nn + 7 * mm + minimap_origin_y + 134;
                            char x_border;
                            char y_border = 0;
                            if (!this->map->minimap_mask[nn][mm])
                            {
                                x_border = 0;
                                y_border = 0;
                                if (!nn)
                                    x_border = 1;
                                else
                                {
                                    if (this->map->minimap_mask[nn - 1][mm] == 1)
                                        x_border = 1;
                                }
                                if (!mm)
                                    y_border = 1;
                                else
                                {
                                    if (this->map->minimap_mask[nn][mm - 1] == 1)
                                        y_border = 1;
                                }
                                if (x_border || y_border)
                                {
                                    if (x_border && !y_border)
                                        this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                            this->gfx->interface_graphics->minimap_tiles,
                                            Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 0));
                                    if (y_border && !x_border)
                                        this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                            this->gfx->interface_graphics->minimap_tiles,
                                            Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 1));
                                }
                                else
                                    this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                        this->gfx->interface_graphics->minimap_tiles,
                                        Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 2));
                            }
                            if (this->map->blocked[nn][mm] || this->map->minimap_mask[nn][mm] == 2)
                            {
                                if (this->map->minimap_mask[nn][mm] == 2)
                                    this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                        this->gfx->interface_graphics->minimap_tiles,
                                        Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 2));
                                if (this->map->minimap_mask[nn][mm] != 1)
                                    this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                        this->gfx->interface_graphics->minimap_tiles,
                                        Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 3));
                            }
                            char player_on_cell = 0;
                            if ((int)MainForm->players->GetCount() > minimap_player_index)
                            {
                                char minimap_player_found;
                                do
                                {
                                    minimap_player_found = 0;
                                    Player minimap_player = MainForm->players->GetPlayer(minimap_player_index);
                                    if ((unsigned int)nn == minimap_player.x && (unsigned int)mm == minimap_player.y)
                                    {
                                        if (!minimap_player.hidden)
                                            player_on_cell = 1;
                                        if (this->net_client->GetPlayerId() == minimap_player.player_id)
                                            this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                                this->gfx->interface_graphics->minimap_tiles,
                                                Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 6));
                                        else if (!minimap_player.hidden)
                                            this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                                this->gfx->interface_graphics->minimap_tiles,
                                                Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 4));
                                        minimap_player_found = 1;
                                        minimap_player_index++;
                                    }
                                } while ((int)MainForm->players->GetCount() > minimap_player_index &&
                                    minimap_player_found);
                            }
                            if (!player_on_cell && this->map->has_tile_spec[nn][mm])
                                this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                    this->gfx->interface_graphics->minimap_tiles,
                                    Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 7));
                            if ((int)MainForm->npcs->GetCount() > minimap_npc_index)
                            {
                                char minimap_npc_found;
                                do
                                {
                                    minimap_npc_found = 0;
                                    Npc minimap_npc = MainForm->npcs->GetNpc(minimap_npc_index);
                                    if ((unsigned int)nn == minimap_npc.x && (unsigned int)mm == minimap_npc.y)
                                    {
                                        if (minimap_npc.graphic != 85)
                                        {
                                            NpcTypeInfo npc_type = MainForm->npc_values->GetType(minimap_npc.npc_id);
                                            if (npc_type.npc_type > NpcType_Friendly && npc_type.npc_type < NpcType_Shop)
                                                this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                                    this->gfx->interface_graphics->minimap_tiles,
                                                    Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 5));
                                            else
                                                this->gfx->game->BlitClippedHalfBlend(v43bb, minimap_y,
                                                    this->gfx->interface_graphics->minimap_tiles,
                                                    Gfx_interface_GetMinimapEdgeRect(this->gfx->interface_graphics, 8));
                                        }
                                        minimap_npc_found = 1;
                                        minimap_npc_index++;
                                    }
                                } while ((int)MainForm->npcs->GetCount() > minimap_npc_index && minimap_npc_found);
                            }
                        }
                    }
                }
                gfx->sprites->DrawNameLabel();
                int status_icon_x = 14;
                if (map->GetMapType() == PVP_MAP_TYPE)
                {
                    MainForm->blitter->AlphaBlt(status_icon_x, 285, OffscreenSurface,
                        gfx->interface_graphics->status_icons,
                        &Gfx_interface_GetStatusIconRect(gfx->interface_graphics, 3, false), 140);
                    status_icon_x += 24;
                }
                if (animation->interface_controller->spells->awaiting_target)
                {
                    MainForm->blitter->AlphaBlt(status_icon_x, 285, OffscreenSurface,
                        gfx->interface_graphics->status_icons,
                        &Gfx_interface_GetStatusIconRect(gfx->interface_graphics, 2, false), 140);
                    status_icon_x += 24;
                }
                if (own_has_weapon)
                {
                    MainForm->blitter->AlphaBlt(status_icon_x, 285, OffscreenSurface,
                        gfx->interface_graphics->status_icons,
                        &Gfx_interface_GetStatusIconRect(gfx->interface_graphics, 0, false), 140);
                    status_icon_x += 24;
                }
                if (own_has_shield)
                {
                    MainForm->blitter->AlphaBlt(status_icon_x, 285, OffscreenSurface,
                        gfx->interface_graphics->status_icons,
                        &Gfx_interface_GetStatusIconRect(gfx->interface_graphics, 1, false), 140);
                    status_icon_x += 24;
                }
                OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_frame,
                    &gfx->interface_graphics->hud_frame_rect, 17);
                OffscreenSurface->BltFast(7, 53, gfx->interface_graphics->hud_left_frame,
                    &gfx->interface_graphics->hud_left_frame_rect, 17);
                OffscreenSurface->BltFast(49, 7, gfx->interface_graphics->hud_top_frame,
                    &gfx->interface_graphics->hud_top_frame_rect, 17);
                animation->interface_controller->game_bars->MarkDirty();
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
                            Gfx_game_GetMenuRowRect(gfx->game, animation->game->player_menu_hovered_option));
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
                    HDC locker_dc;
                    if (OffscreenSurface->GetDC(&locker_dc))
                    {
                        try
                        {
                            text_canvas->Handle = locker_dc;
                            gfx->game->DrawLockerText();
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
                    HDC shop_dc;
                    if (OffscreenSurface->GetDC(&shop_dc))
                    {
                        try
                        {
                            text_canvas->Handle = shop_dc;
                            gfx->game->DrawShopText();
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
                    HDC learn_dc;
                    if (OffscreenSurface->GetDC(&learn_dc))
                    {
                        try
                        {
                            text_canvas->Handle = learn_dc;
                            gfx->game->DrawLearnText();
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
                    HDC paperdoll_dc;
                    if (OffscreenSurface->GetDC(&paperdoll_dc))
                    {
                        try
                        {
                            text_canvas->Handle = paperdoll_dc;
                            gfx->game->DrawPaperdollText();
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
                    HDC trade_dc;
                    if (OffscreenSurface->GetDC(&trade_dc))
                    {
                        try
                        {
                            text_canvas->Handle = trade_dc;
                            gfx->game->DrawTradeText();
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
                        gfx->interface_graphics->control_sheet, &gfx->game->amount_box_rect, 16);
                    if (animation->game->amount_box_kind == AmountBoxKind_JunkItem)
                        OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                            animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                            &gfx->game->junk_title_rect, 16);
                    if (animation->game->amount_box_kind == 3)
                        OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                            animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                            &gfx->game->amount_title_rect, 16);
                    if (animation->game->amount_box_kind == AmountBoxKind_TradeAddItem)
                        OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                            animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                            &gfx->game->trade_title_rect, 16);
                    if (animation->game->amount_box_kind == AmountBoxKind_BankDeposit ||
                        animation->game->amount_box_kind == AmountBoxKind_BankWithdraw)
                        OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                            animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                            &gfx->game->bank_title_rect, 16);
                    if (animation->game->amount_box_kind == AmountBoxKind_ShopBuy ||
                        animation->game->amount_box_kind == AmountBoxKind_ShopSell ||
                        animation->game->amount_box_kind == AmountBoxKind_LockerAddItem)
                        OffscreenSurface->BltFast(animation->game->amount_box_x + 11,
                            animation->game->amount_box_y + 10, gfx->interface_graphics->control_sheet,
                            &gfx->game->shop_title_rect, 16);
                    if (animation->game->amount_slider_grabbed)
                        this->OffscreenSurface->BltFast(this->animation->game->amount_box_x +
                            this->animation->game->amount_slider_offset + 25, this->animation->game->amount_box_y + 96,
                            MainForm->gfx->interface_graphics->scroll_icons,
                            &InterfaceGraphics::GetScrollIconRect(MainForm->gfx->interface_graphics, 6), 16);
                    else
                    {
                        int knob_icon = 5;
                        if (animation->game->amount_slider_hovered)
                            knob_icon = 6;
                        if (edits->GetText(13).Length() > 0)
                        {
                            int typed_amount = StrToInt(edits->GetText(13));
                            int slider_width = 107;
                            typed_amount--;
                            double slider_step = (double)slider_width / (double)(animation->game->amount_max - 1);
                            double knob_offset = typed_amount * slider_step;
                            animation->game->amount_slider_offset = knob_offset;
                            if (animation->game->amount_slider_offset < 0)
                                OffscreenSurface->BltFast(animation->game->amount_box_x + 25,
                                    animation->game->amount_box_y + 96, MainForm->gfx->interface_graphics->scroll_icons,
                                    &InterfaceGraphics::GetScrollIconRect(MainForm->gfx->interface_graphics, knob_icon),
                                    16);
                            else
                                this->OffscreenSurface->BltFast(this->animation->game->amount_box_x +
                                    this->animation->game->amount_slider_offset + 25,
                                    this->animation->game->amount_box_y + 96,
                                    MainForm->gfx->interface_graphics->scroll_icons,
                                    &InterfaceGraphics::GetScrollIconRect(MainForm->gfx->interface_graphics, knob_icon),
                                    16);
                        }
                        else
                        {
                            animation->game->amount_slider_offset = 0;
                            OffscreenSurface->BltFast(animation->game->amount_box_x + 25,
                                animation->game->amount_box_y + 96, MainForm->gfx->interface_graphics->scroll_icons,
                                &InterfaceGraphics::GetScrollIconRect(MainForm->gfx->interface_graphics, knob_icon),
                                16);
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
                            edit_canvas->DrawEditBoxes(109);
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
                            gfx->game->DrawListBoxText();
                            edit_canvas->DrawEditBoxes(105);
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
                        gfx->game->input_box_surface, &gfx->game->input_box_rect, 16);
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
                        gfx->game->text_box_surface, &gfx->game->text_box_rect, 16);
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
                        gfx->game->message_box_surface, &gfx->game->message_box_rect, 16);
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
                    if (animation->interface_controller->game_bars->show_health_text ||
                        animation->interface_controller->game_bars->show_mana_text ||
                        animation->interface_controller->game_bars->show_stamina_text ||
                        animation->interface_controller->game_bars->show_experience_text)
                    {
                        HDC status_bar_dc;
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
        if (animation->interface_controller->TakeInvalidated())
        {
            flip_needed = true;
            OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                &gfx->interface_graphics->panel_rect, 16);
        }
        if (!animation->interface_controller->IsFullRedrawPending())
        {
            if (MainForm->animation->GetSubState() == GamePanel_Status)
            {
                if (animation->interface_controller->status->TestAndClearDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, 16);
                    HDC status_dc;
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
                    int raise_button_frame = 0;
                    if (animation->interface_controller->status->GetBlinkPhase() > 5)
                        raise_button_frame = 1;
                    MainForm->OffscreenSurface->BltFast(208, 337, gfx->interface_graphics->control_sheet,
                        &InterfaceGraphics::GetRaiseButtonRect(gfx->interface_graphics, raise_button_frame), 16);
                    MainForm->OffscreenSurface->BltFast(208, 355, gfx->interface_graphics->control_sheet,
                        &InterfaceGraphics::GetRaiseButtonRect(gfx->interface_graphics, raise_button_frame), 16);
                    MainForm->OffscreenSurface->BltFast(208, 373, gfx->interface_graphics->control_sheet,
                        &InterfaceGraphics::GetRaiseButtonRect(gfx->interface_graphics, raise_button_frame), 16);
                    MainForm->OffscreenSurface->BltFast(208, 391, gfx->interface_graphics->control_sheet,
                        &InterfaceGraphics::GetRaiseButtonRect(gfx->interface_graphics, raise_button_frame), 16);
                    MainForm->OffscreenSurface->BltFast(208, 409, gfx->interface_graphics->control_sheet,
                        &InterfaceGraphics::GetRaiseButtonRect(gfx->interface_graphics, raise_button_frame), 16);
                    MainForm->OffscreenSurface->BltFast(208, 427, gfx->interface_graphics->control_sheet,
                        &InterfaceGraphics::GetRaiseButtonRect(gfx->interface_graphics, raise_button_frame), 16);
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Chat)
            {
                if (animation->interface_controller->chat->TakeTextDirty())
                {
                    OffscreenSurface->BltFast(102, 330, gfx->interface_graphics->panel_background,
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                        &gfx->interface_graphics->panel_rect, 16);
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
                    &gfx->interface_graphics->tooltip_rect, 16);
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
                        status_canvas->Font->Color = clBlack;
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
                    &gfx->interface_graphics->tooltip_rect, 16);
        }
        // The clock is drawn again only when its text changes.
        String current_clock_text = TimeToStr(Time());
        if (clock_text != current_clock_text)
        {
            clock_text = current_clock_text;
            OffscreenSurface->BltFast(543, 452, gfx->interface_graphics->clock_background,
                &gfx->interface_graphics->clock_rect, 16);
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
                    status_canvas->Font->Color = clBlack;
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
        if (edits->GetCount() >= 1 && (edits->HasDirtyEdit() || edits->ConsumeTickRedraw()))
        {
            if (edits->ConsumeDirty(EditId_Chat))
            {
                gfx->interface_graphics->DrawChatModeIcon();
                OffscreenSurface->BltFast(98, 309, gfx->interface_graphics->chat_input_background,
                    &gfx->interface_graphics->chat_input_rect, 16);
                flip_needed = true;
            }
            HDC chat_edit_dc;
            if (OffscreenSurface->GetDC(&chat_edit_dc))
            {
                try
                {
                    text_canvas->Handle = chat_edit_dc;
                    edit_canvas->DrawEditBoxes(3);
                    text_canvas->Handle = 0;
                }
                catch (Exception &)
                {
                }
                OffscreenSurface->ReleaseDC(chat_edit_dc);
            }
        }
    }
    if (animation->GetScreenMode() == ScreenMode_Menu)
    {
        flip_needed = true;
        OffscreenSurface->BltFast(0, 0, gfx->menu->background, &gfx->menu->background_rect, 16);
        if (animation->GetSubState() == MenuScreen_Main || animation->GetSubState() == MenuScreen_Credits || animation->GetSubState() == MenuScreen_Login ||
            animation->GetSubState() == MenuScreen_GridAccountName || animation->GetSubState() == MenuScreen_PasswordGrid)
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
        if (animation->GetSubState() == MenuScreen_Login || animation->GetSubState() == MenuScreen_GridAccountName || animation->GetSubState() == MenuScreen_PasswordGrid)
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
        if (animation->GetSubState() == MenuScreen_Main && OffscreenSurface->GetDC(&version_dc))
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
                    VersionString(version_major, version_minor, version_build) + " - " + config->GetHost());
                status_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            OffscreenSurface->ReleaseDC(version_dc);
        }
        if (MainForm->animation->menu->is_loading_world)
        {
            TRect loading_frame_rect;
            loading_frame_rect.top = 0;
            loading_frame_rect.left = 220 * (MainForm->animation->menu->loading_frame / 2);
            loading_frame_rect.bottom = 127;
            loading_frame_rect.right = 220 * (MainForm->animation->menu->loading_frame / 2) + 220;
            OffscreenSurface->BltFast(397, 333, gfx->menu->animation_sheet, &loading_frame_rect, 16);
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
                    char text_shown = 0;
                    if (!text_shown && animation->menu->IsMapNeeded() && animation->menu->IsEnteringGame())
                    {
                        draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingMap));
                        text_shown = 1;
                    }
                    if (!text_shown && animation->menu->IsItemFileNeeded() && animation->menu->IsEnteringGame())
                    {
                        draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingItems));
                        text_shown = 1;
                    }
                    if (!text_shown && animation->menu->IsNpcFileNeeded() && animation->menu->IsEnteringGame())
                    {
                        draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingNpcs));
                        text_shown = 1;
                    }
                    if (!text_shown && animation->menu->IsSkillFileNeeded() && animation->menu->IsEnteringGame())
                    {
                        draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingSkills));
                        text_shown = 1;
                    }
                    if (!text_shown && animation->menu->IsClassFileNeeded() && animation->menu->IsEnteringGame())
                    {
                        draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingClasses));
                        text_shown = 1;
                    }
                    if (!text_shown && animation->menu->IsReservedFileNeeded() && animation->menu->IsEnteringGame())
                    {
                        draw_canvas->TextOut(410, 341, MainForm->strings->GetText(GameText_UpdatingGuilds));
                        text_shown = 1;
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
                    char more_lines = 1;
                    while (true)
                    {
                        if (draw_canvas->TextWidth(tip_text) > 180)
                        {
                            for (int i1 = 1; i1 <= tip_text.Length(); i1++)
                            {
                                int prefix_width = draw_canvas->TextWidth(tip_text.SubString(1, i1));
                                if (tip_text[i1] == ' ' || i1 == tip_text.Length())
                                {
                                    if (prefix_width > 180 || i1 == tip_text.Length())
                                    {
                                        draw_canvas->TextOut(416, tip_y, tip_text.SubString(1, break_index - 1));
                                        tip_text.Delete(1, break_index);
                                        break;
                                    }
                                    break_index = i1;
                                }
                            }
                        }
                        else
                        {
                            draw_canvas->TextOut(416, tip_y, tip_text);
                            more_lines = 0;
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
                gfx->menu->create_character_background, &gfx->menu->create_character_rect, 16);
            gfx->menu->DrawCreateCharacterControls();
            gfx->sprites->DrawPreviewPlayers(1);
        }
        HDC menu_edit_dc;
        if (edits->GetCount() >= 1 && OffscreenSurface->GetDC(&menu_edit_dc))
        {
            try
            {
                text_canvas->Handle = menu_edit_dc;
                edit_canvas->DrawEditBoxes(240);
                text_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            OffscreenSurface->ReleaseDC(menu_edit_dc);
        }
        HDC credits_dc;
        if (animation->credits->is_running && OffscreenSurface->GetDC(&credits_dc))
        {
            try
            {
                draw_canvas->Handle = credits_dc;
                draw_canvas->Brush->Color = clBlack;
                draw_canvas->Brush->Style = bsClear;
                draw_canvas->Font->Color = clWhite;
                draw_canvas->Font->Size = 8;
                draw_canvas->Font->Name = "Arial";
                int credit_index = 0;
                while (animation->credits->shown_lines->Count > credit_index)
                {
                    draw_canvas->Pen->Color = (TColor)0x46AADC;
                    draw_canvas->Font->Size = 20;
                    draw_canvas->Font->Name = "Arial";
                    int indent = 0;
                    if (animation->credits->shown_styles->Strings[credit_index] == "title")
                    {
                        draw_canvas->Font->Size = 16;
                        draw_canvas->Font->Color = clWhite;
                        draw_canvas->Rectangle(350, StrToInt(animation->credits->shown_y->Strings[credit_index]) + 29,
                            550, StrToInt(animation->credits->shown_y->Strings[credit_index]) + 30);
                    }
                    if (animation->credits->shown_styles->Strings[credit_index] == "head")
                        draw_canvas->Font->Color = (TColor)0x5A8CAF;
                    if (animation->credits->shown_styles->Strings[credit_index] == "name")
                    {
                        draw_canvas->Font->Color = (TColor)0xA1428;
                        indent = 10;
                    }
                    if (animation->credits->shown_styles->Strings[credit_index] == "small")
                    {
                        draw_canvas->Font->Color = clGray;
                        draw_canvas->Font->Size = 8;
                        indent = 10;
                    }
                    draw_canvas->TextOut(indent + 340, StrToInt(animation->credits->shown_y->Strings[credit_index]),
                        animation->credits->shown_lines->Strings[credit_index]);
                    credit_index++;
                }
                draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            OffscreenSurface->ReleaseDC(credits_dc);
        }
        if (animation->menu->is_text_window_open)
        {
            OffscreenSurface->BltFast(animation->menu->text_window_x, animation->menu->text_window_y,
                gfx->menu->list_box_background, &gfx->menu->list_box_rect, 16);
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
                gfx->menu->message_box_background, &gfx->menu->message_box_rect, 16);
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
            OffscreenSurface->Blt(&intro_rect, 0, 0, 16778240, gfx->intro->fill_fx);
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
            OffscreenSurface->BltFast(141, 125, gfx->intro->surface, &intro_panel_rect, 16);
            animation->intro->logo_redraw_pending = false;
            flip_needed = true;
        }
    }
    if (flip_needed && show_frame_rate)
    {
        if (animation->GetScreenMode() == ScreenMode_Game)
            OffscreenSurface->BltFast(0, 0, gfx->interface_graphics->hud_frame,
                &gfx->interface_graphics->hud_frame_rect, 17);
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
    char item_drag_drawn = 0;
    char spell_drag_drawn = 0;
    char skill_drag_drawn = 0;
    if (dragging)
    {
        if ((flip_needed || animation->interface_controller->inventory->TestAndClearDragDirty()) &&
            animation->GetSubState() == GamePanel_Inventory)
        {
            gfx->interface_graphics->DrawDraggedItem();
            flip_needed = true;
            item_drag_drawn = 1;
        }
        if ((flip_needed || animation->interface_controller->spells->TestAndClearDragDirty()) &&
            animation->GetSubState() == 8)
        {
            gfx->interface_graphics->DrawDraggedSpell();
            flip_needed = true;
            spell_drag_drawn = 1;
        }
        if ((flip_needed || animation->interface_controller->skills->TestAndClearDragDirty()) &&
            animation->GetSubState() == GamePanel_Skills)
        {
            gfx->interface_graphics->DrawDraggedSkill();
            flip_needed = true;
            skill_drag_drawn = 1;
        }
    }
    if (config->GetFullscreen() && flip_needed)
    {
        flip_needed = false;
        present_pending = false;
        if (config->GetUsePageFlip())
        {
            if (!BackBuffer)
                BackBuffer->Restore();
            BackBuffer->BltFast(0, 0, OffscreenSurface, &screen_rect, 16);
            if (!PrimarySurface)
                PrimarySurface->Restore();
            PrimarySurface->Flip(BackBuffer, 1);
        }
        else
        {
            if (!PrimarySurface)
                PrimarySurface->Restore();
            PrimarySurface->BltFast(0, 0, OffscreenSurface, &screen_rect, 16);
        }
        if (PrimarySurface->ErrorValue == DDERR_SURFACELOST || OffscreenSurface->ErrorValue == DDERR_SURFACELOST ||
            BackBuffer->ErrorValue == DDERR_SURFACELOST)
            Dx_Draw1->RestoreAllSurfaces();
    }
    if (!config->GetFullscreen() && flip_needed)
    {
        flip_needed = false;
        present_pending = false;
        TRect window_rect = BoundsRect;
        window_rect.top += 20;
        if (window_height_padding > 0)
            window_rect.top += window_height_padding;
        if (!PrimarySurface)
            PrimarySurface->Restore();
        PrimarySurface->Flip(OffscreenSurface, 1);
        Dx_Draw1->WaitForVerticalBlank(1);
        if (!OffscreenSurface)
            OffscreenSurface->Restore();
        PrimarySurface->Blt(&window_rect, OffscreenSurface, 0, 16777216, 0);
        if (PrimarySurface->ErrorValue == DDERR_SURFACELOST || OffscreenSurface->ErrorValue == DDERR_SURFACELOST)
            Dx_Draw1->RestoreAllSurfaces();
    }
    if (item_drag_drawn)
        gfx->interface_graphics->RestoreItemDragBackground();
    if (spell_drag_drawn)
        gfx->interface_graphics->RestoreSpellDragBackground();
    if (skill_drag_drawn)
        gfx->interface_graphics->RestoreSkillDragBackground();
    frame_count++;
    DWORD dwMilliseconds = 1;
    if (animation->GetScreenMode() == ScreenMode_Intro)
        dwMilliseconds = 10;
    // A short sleep on each pass, longer during the intro.
    Sleep(dwMilliseconds);
    Done = 0;
}

void __fastcall TMainForm::clientError(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket,
    Scktcomp::TErrorEvent ErrorEvent, int &ErrorCode)
{
    Socket->Close();
    ErrorCode = 0;
}

void __fastcall TMainForm::clientDisconnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    net_client->HandleDisconnect();
}

void __fastcall TMainForm::clientRead(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    net_client->ReceiveData(Socket, Socket->ReceiveText());
}

void __fastcall TMainForm::clientConnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    net_client->SendInitRequest();
}

void __fastcall TMainForm::ApplicationEvents1Exception(System::TObject *Sender, Sysutils::Exception *E)
{
    String message = E->Message;
}
