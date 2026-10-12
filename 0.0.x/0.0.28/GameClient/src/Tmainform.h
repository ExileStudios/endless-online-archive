#ifndef TmainformH
#define TmainformH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <Forms.hpp>
#include <AppEvnts.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "Animationprotector.h"
#include "Classvalues.h"
#include "Configuration.h"
#include "Filelogger.h"
#include "Fileprotection.h"
#include "Gfx_alpha.h"
#include "Gfx_edits.h"
#include "Gfxprotection.h"
#include "Itemvalues.h"
#include "Mapprotection.h"
#include "Npcvalues.h"
#include "Packetcontrols.h"
#include "Register.h"
#include "Sfxprotection.h"
#include "Skillvalues.h"
#include "Stringunit.h"
#include "Sys_edits.h"
#include "Sys_effects.h"
#include "Sys_items.h"
#include "Sys_keyboard.h"
#include "Sys_magicmap.h"
#include "Sys_mouse.h"
#include "Sys_npcs.h"
#include "Sys_players.h"
#include "Sys_soundmap.h"
#include "Sys_weaponmap.h"
#include <TDx_Draw_Library_Install.H>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include "Ctrl_chest.h"
#include "Ctrl_group.h"
#include "Ctrl_inventory.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_msgboard.h"
#include "Ctrl_shop.h"
#include "Ctrl_skills.h"
#include "Ctrl_spells.h"
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
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>

enum NpcGraphic
{
    NpcGraphic_CursedMask = 99,
    NpcGraphic_AncientWraith = 171
};

// The game window: it builds every controller, runs both timers and draws each frame when idle.
struct PACKAGE TMainForm : public Forms::TForm
{
__published:
    TDx_Draw *Dx_Draw1;
    TDx_DrawClipper *PrimaryClipper;
    TDx_DrawSurface *PrimarySurface;
    TDDSurfaceDesc *PrimarySurfaceDesc;
    TDDSCaps *PrimaryCaps;
    TDx_DrawSurface *BackBuffer;
    TDDSurfaceDesc *BackBufferDesc;
    TDDSCaps *BackBufferCaps;
    TDx_DrawSurface *OffscreenSurface;
    TDDSurfaceDesc *OffscreenDesc;
    TDDSCaps *OffscreenCaps;
    TDDPixelFormat *OffscreenPixelFormat;
    TDx_Sound *Dx_Sound1;
    TDx_SoundBuffer *Dx_SoundBuffer1;
    TDSBufferDesc *DSBufferDesc1;
    TDx_SoundBuffer *Dx_SoundBuffer2;
    TDSBufferDesc *DSBufferDesc2;
    TDx_SoundBuffer *Dx_SoundBuffer3;
    TDSBufferDesc *DSBufferDesc3;
    TDx_SoundBuffer *Dx_SoundBuffer4;
    TDSBufferDesc *DSBufferDesc4;
    TDx_SoundBuffer *Dx_SoundBuffer5;
    TDSBufferDesc *DSBufferDesc5;
    TDx_SoundBuffer *Dx_SoundBuffer6;
    TDSBufferDesc *DSBufferDesc6;
    TDx_SoundBuffer *Dx_SoundBuffer7;
    TDSBufferDesc *DSBufferDesc7;
    TDx_Input *Dx_Input1;
    TDIDevCaps *DIDevCaps1;
    TDIKeyboardState *DIKeyboardState1;
    TDx_InputDevice *Dx_InputDevice1;
    TClientSocket *client;
    TApplicationEvents *ApplicationEvents1;
    TTimer *Timer1;
    void __fastcall OnIdle(System::TObject *Sender, bool &Done);
    void __fastcall FormCreate(System::TObject *Sender);
    void __fastcall FormDestroy(System::TObject *Sender);
    void __fastcall Dx_Draw1Create(System::TObject *Sender);
    void __fastcall Dx_Draw1Destroy(System::TObject *Sender);
    void __fastcall PrimarySurfaceSurfaceLost(System::TObject *Sender);
    void __fastcall PrimarySurfaceCreate(System::TObject *Sender);
    void __fastcall Dx_Sound1Create(System::TObject *Sender);
    void __fastcall Dx_Sound1Destroy(System::TObject *Sender);
    void __fastcall FormClose(System::TObject *Sender, Forms::TCloseAction &Action);
    void __fastcall Dx_Draw1EnumDisplayModes(System::TObject *Sender, TDDSurfaceDesc *SurfaceDesc, void *Context,
        bool &Finished);
    void __fastcall Dx_Sound1DSEnumerate(System::TObject *Sender, GUID *Guid, String DriverDescription,
        String DriverModule, void *Context, bool &Finished);
    void __fastcall FormMouseMove(System::TObject *Sender, Classes::TShiftState Shift, int X, int Y);
    void __fastcall FormMouseDown(System::TObject *Sender, Controls::TMouseButton Button, Classes::TShiftState Shift,
        int X, int Y);
    void __fastcall FormMouseWheelDown(System::TObject *Sender, Classes::TShiftState Shift, Windows::TPoint &MousePos,
        bool &Handled);
    void __fastcall FormMouseWheelUp(System::TObject *Sender, Classes::TShiftState Shift, Windows::TPoint &MousePos,
        bool &Handled);
    void __fastcall FormMouseUp(System::TObject *Sender, Controls::TMouseButton Button, Classes::TShiftState Shift,
        int X, int Y);
    void __fastcall Dx_Input1Destroy(System::TObject *Sender);
    void __fastcall Dx_InputDevice1Destroy(System::TObject *Sender);
    void __fastcall Dx_InputDevice1Create(System::TObject *Sender);
    void __fastcall clientError(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket,
        Scktcomp::TErrorEvent ErrorEvent, int &ErrorCode);
    void __fastcall clientRead(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket);
    void __fastcall clientDisconnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket);
    void __fastcall clientConnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket);
    void __fastcall FormKeyDown(System::TObject *Sender, Word &Key, Classes::TShiftState Shift);
    void __fastcall ApplicationEvents1Exception(System::TObject *Sender, Sysutils::Exception *E);
    void __fastcall Timer1Timer(System::TObject *Sender);
public:
    StringContainer *strings;
    FileController *files;
    AnimationController *animation;
    SfxController *sfx;
    GfxController *gfx;
    MapController *map;
    Config *config;
    WinRegister *registry;
    FFileLog *chat_log;
    ItemValues *item_values;
    NpcValues *npc_values;
    SkillValues *skill_values;
    ClassValues *class_values;
    MapItems *map_items;
    Players *players;
    Effects *effects;
    Npcs *npcs;
    Edits *edits;
    EditCanvas *edit_canvas;
    SoundMapper *sound_mapper;
    MagicMapper *magic_mapper;
    WeaponMapper *weapon_mapper;
    KeyBoard *keyboard;
    Mouser *mouse;
    ExtBlts *blitter;
    Client *net_client;
    TCanvas *draw_canvas;
    TCanvas *status_canvas;
    TCanvas *text_canvas;
    Graphics::TBitmap *resource_bitmap;
    Graphics::TBitmap *interface_bitmap;
    Windows::TRect screen_rect;
    TStringList *sound_driver_names;
    TStringList *sound_driver_modules;
    TList *sound_driver_guids;
    int timer_ticks;
    int frame_count;
    bool show_frame_rate;
    int frame_rate;
    int display_width;
    int display_height;
    int window_height_padding;
    int refresh_rate;
    bool redraw_needed;
    bool dragging;
    String form_text;
    int display_mode_count;
    int mm_timer_id;
    int version_major;
    int version_minor;
    int version_build;
    bool minimized;
    bool active;
    int draw_error_count;
    char pad_0x434[4];
    String clock_text;
    bool flip_needed;
    bool animation_tick_due;
    bool present_pending;
    char keyboard_acquired;
    TTimeStamp start_time;
    int time_value_a;
    int time_value_b;

    __fastcall TMainForm(Classes::TComponent *owner);
    virtual __fastcall ~TMainForm();
    void __fastcall ActivateForm(System::TObject *sender);
    void CountDrawError();
    void __fastcall DeactivateForm(System::TObject *sender);
    void __fastcall HandleMMTimerEvent();
    void IntroAnimator();
    void __fastcall MinimizeForm(System::TObject *sender);
    void __fastcall RestoreForm(System::TObject *sender);
    String VersionString(int major, int minor, int patch);
};

extern PACKAGE TMainForm *MainForm;

void __stdcall MMTimerProc(UINT timer_id, UINT message, DWORD_PTR user_data, DWORD_PTR first_reserved,
    DWORD_PTR second_reserved);

// Shared body: a class defines its LoadSurface with DEFINE_LOAD_SURFACE(TheClass); each class has its own
// surface_desc, surface_caps, surface, surface_rect, color_key.
#define DEFINE_LOAD_SURFACE(Class) \
GraphicSize Class::LoadSurface(int library, int resource_id) \
{ \
    GraphicSize size; \
    HDC dc; \
    HDC CompatibleDC; \
    size.width = -1; \
    MainForm->resource_bitmap->Width = 0; \
    MainForm->resource_bitmap->Height = 0; \
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id); \
    surface_desc->Flags = 6; \
    surface_desc->SCaps = surface_caps; \
    surface_desc->SCaps->Caps = 2112; \
    surface_desc->Width = MainForm->resource_bitmap->Width; \
    surface_desc->Height = MainForm->resource_bitmap->Height; \
    if (!surface) \
        surface->Restore(); \
    if (surface->Created) \
        surface->Destroy(); \
    surface->GetSurfaceDesc(surface_desc); \
    surface->Create(surface_desc, MainForm->Dx_Draw1); \
    size.width = MainForm->resource_bitmap->Width; \
    size.height = MainForm->resource_bitmap->Height; \
    surface_rect.left = 0; \
    surface_rect.top = 0; \
    surface_rect.right = MainForm->resource_bitmap->Width; \
    surface_rect.bottom = MainForm->resource_bitmap->Height; \
    if (!MainForm->config->GetUseBitBltDraw()) \
    { \
        if (surface->GetDC(&dc)) \
        { \
            try \
            { \
                MainForm->draw_canvas->Handle = dc; \
                MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap); \
                MainForm->draw_canvas->Handle = 0; \
            } \
            catch (Exception &) \
            { \
                MainForm->CountDrawError(); \
                surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            surface->ReleaseDC(dc); \
        } \
    } \
    else \
    { \
        CompatibleDC = CreateCompatibleDC(0); \
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle); \
        if (surface->GetDC(&dc)) \
        { \
            try \
            { \
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376); \
            } \
            catch (...) \
            { \
                surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            surface->ReleaseDC(dc); \
        } \
        DeleteDC(CompatibleDC); \
    } \
    if (size.width > 0) \
        surface->SetColorKey(8, color_key); \
    return size; \
}

// Shared body: a class defines its LoadSurface with DEFINE_BANK_LOAD_SURFACE(TheClass); each class has its own
// surface_desc, surface_caps, surface, surface_rect.
#define DEFINE_BANK_LOAD_SURFACE(Class) \
GraphicSize Class::LoadSurface(HMODULE library, int resource_id) \
{ \
    GraphicSize size; \
    HDC dc; \
    HDC CompatibleDC; \
    size.width = -1; \
    MainForm->resource_bitmap->Width = 0; \
    MainForm->resource_bitmap->Height = 0; \
    MainForm->resource_bitmap->LoadFromResourceID((int)library, resource_id); \
    surface_desc->Flags = 6; \
    surface_desc->SCaps = surface_caps; \
    surface_desc->SCaps->Caps = 2112; \
    surface_desc->Width = MainForm->resource_bitmap->Width; \
    surface_desc->Height = MainForm->resource_bitmap->Height; \
    if (!surface) \
        surface->Restore(); \
    if (surface->Created) \
        surface->Destroy(); \
    surface->GetSurfaceDesc(surface_desc); \
    surface->Create(surface_desc, MainForm->Dx_Draw1); \
    size.width = MainForm->resource_bitmap->Width; \
    size.height = MainForm->resource_bitmap->Height; \
    surface_rect.left = 0; \
    surface_rect.top = 0; \
    surface_rect.right = MainForm->resource_bitmap->Width; \
    surface_rect.bottom = MainForm->resource_bitmap->Height; \
    if (!MainForm->config->GetUseBitBltDraw()) \
    { \
        if (surface->GetDC(&dc)) \
        { \
            try \
            { \
                MainForm->draw_canvas->Handle = dc; \
                MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap); \
                MainForm->draw_canvas->Handle = 0; \
            } \
            catch (Exception &) \
            { \
                MainForm->CountDrawError(); \
                surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            surface->ReleaseDC(dc); \
        } \
    } \
    else \
    { \
        CompatibleDC = CreateCompatibleDC(0); \
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle); \
        if (surface->GetDC(&dc)) \
        { \
            try \
            { \
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376); \
            } \
            catch (...) \
            { \
                surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            surface->ReleaseDC(dc); \
        } \
        DeleteDC(CompatibleDC); \
    } \
    return size; \
}

// Shared body: a class defines its UpdateScrollHover with DEFINE_UPDATE_SCROLL_HOVER(TheClass); each class has its own
// window_x, window_y, DragScrollThumb, scroll_up_hovered, scroll_down_hovered.
#define DEFINE_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    if (x >= this->window_x + 252 && x <= this->window_x + 267) \
    { \
        if (y >= this->window_y + 58 && y <= this->window_y + 229) \
            this->DragScrollThumb(y); \
    } \
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59) \
    { \
        if (!scroll_up_hovered) \
        { \
            scroll_up_hovered = 1; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else if (scroll_up_hovered) \
    { \
        scroll_up_hovered = 0; \
        MainForm->redraw_needed = true; \
    } \
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243) \
    { \
        if (!scroll_down_hovered) \
        { \
            scroll_down_hovered = 1; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else if (scroll_down_hovered) \
    { \
        scroll_down_hovered = 0; \
        MainForm->redraw_needed = true; \
    } \
}

// Adds a text to the text page, wrapped to the page width, with an optional blank line after it.
// Shared body: a class defines its AddText with DEFINE_ADD_TEXT(TheClass); each class has its own link_line_index,
// lines.
#define DEFINE_ADD_TEXT(Class) \
void Class::AddText(String text, bool add_blank_line) \
{ \
    int max_width; \
    HDC dc; \
    if (text.Length() > 0) \
    { \
        /* A line starting with an asterisk is the clickable link */ \
        if (text[1] == '*') \
            link_line_index = lines->Count; \
    } \
    max_width = 200; \
    if (MainForm->OffscreenSurface->GetDC(&dc)) \
    { \
        try \
        { \
            MainForm->text_canvas->Handle = dc; \
            MainForm->text_canvas->Font->Name = "MS Sans Serif"; \
            MainForm->text_canvas->Font->Size = 7; \
            do \
            { \
                String fitted_text; \
                if (MainForm->text_canvas->TextWidth(text) <= max_width) \
                { \
                    lines->Add(text); \
                    text = ""; \
                } \
                else \
                { \
                    char split_done = 0; \
                    for (int i = 1; i <= text.Length(); i++) \
                    { \
                        if (text[i] != ' ' && i != text.Length()) \
                            continue; \
                        String prefix = text.SubString(1, i); \
                        if (MainForm->text_canvas->TextWidth(prefix) >= max_width) \
                        { \
                            lines->Add(fitted_text); \
                            text.Delete(1, fitted_text.Length()); \
                            split_done = 1; \
                            break; \
                        } \
                        fitted_text = prefix; \
                    } \
                    if (!split_done) \
                    { \
                        lines->Add(text); \
                        text = ""; \
                    } \
                } \
            } while (text.Length() > 0); \
            MainForm->text_canvas->Handle = 0; \
        } \
        catch (Exception &) \
        { \
        } \
        MainForm->OffscreenSurface->ReleaseDC(dc); \
    } \
    if (add_blank_line) \
        lines->Add(""); \
    MainForm->redraw_needed = true; \
}

// Shared body: a class defines its LoadItemSlot with DEFINE_LOAD_ITEM_SLOT(TheClass); each class has its own
// surface_desc, surface_caps, item_slot_surface, item_slot_rect, color_key.
#define DEFINE_LOAD_ITEM_SLOT(Class) \
GraphicSize Class::LoadItemSlot(HMODULE library, int resource_id) \
{ \
    GraphicSize size; \
    HDC dc; \
    HDC CompatibleDC; \
    size.width = -1; \
    MainForm->resource_bitmap->Width = 0; \
    MainForm->resource_bitmap->Height = 0; \
    MainForm->resource_bitmap->LoadFromResourceID((int)library, resource_id); \
    surface_desc->Flags = 6; \
    surface_desc->SCaps = surface_caps; \
    surface_desc->SCaps->Caps = 2112; \
    surface_desc->Width = MainForm->resource_bitmap->Width; \
    surface_desc->Height = MainForm->resource_bitmap->Height; \
    if (!item_slot_surface) \
        item_slot_surface->Restore(); \
    if (item_slot_surface->Created) \
        item_slot_surface->Destroy(); \
    item_slot_surface->GetSurfaceDesc(surface_desc); \
    item_slot_surface->Create(surface_desc, MainForm->Dx_Draw1); \
    size.width = MainForm->resource_bitmap->Width; \
    size.height = MainForm->resource_bitmap->Height; \
    item_slot_rect.left = 0; \
    item_slot_rect.top = 0; \
    item_slot_rect.right = MainForm->resource_bitmap->Width; \
    item_slot_rect.bottom = MainForm->resource_bitmap->Height; \
    if (!MainForm->config->GetUseBitBltDraw()) \
    { \
        if (item_slot_surface->GetDC(&dc)) \
        { \
            try \
            { \
                MainForm->draw_canvas->Handle = dc; \
                MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap); \
                MainForm->draw_canvas->Handle = 0; \
            } \
            catch (Exception &) \
            { \
                MainForm->CountDrawError(); \
                item_slot_surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            item_slot_surface->ReleaseDC(dc); \
        } \
    } \
    else \
    { \
        CompatibleDC = CreateCompatibleDC(0); \
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle); \
        if (item_slot_surface->GetDC(&dc)) \
        { \
            try \
            { \
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376); \
            } \
            catch (...) \
            { \
                item_slot_surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            item_slot_surface->ReleaseDC(dc); \
        } \
        DeleteDC(CompatibleDC); \
    } \
    if (size.width > 0) \
        item_slot_surface->SetColorKey(8, color_key); \
    return size; \
}

// Shared body: a class defines its LoadBackground with DEFINE_LOAD_BACKGROUND(TheClass); each class has its own
// surface_desc, surface_caps, background_surface, background_rect.
#define DEFINE_LOAD_BACKGROUND(Class) \
GraphicSize Class::LoadBackground(HMODULE library, int resource_id) \
{ \
    GraphicSize size; \
    HDC dc; \
    HDC CompatibleDC; \
    size.width = -1; \
    MainForm->resource_bitmap->Width = 0; \
    MainForm->resource_bitmap->Height = 0; \
    MainForm->resource_bitmap->LoadFromResourceID((int)library, resource_id); \
    surface_desc->Flags = 6; \
    surface_desc->SCaps = surface_caps; \
    surface_desc->SCaps->Caps = 2112; \
    surface_desc->Width = MainForm->resource_bitmap->Width; \
    surface_desc->Height = MainForm->resource_bitmap->Height; \
    if (!background_surface) \
        background_surface->Restore(); \
    if (background_surface->Created) \
        background_surface->Destroy(); \
    background_surface->GetSurfaceDesc(surface_desc); \
    background_surface->Create(surface_desc, MainForm->Dx_Draw1); \
    size.width = MainForm->resource_bitmap->Width; \
    size.height = MainForm->resource_bitmap->Height; \
    background_rect.left = 0; \
    background_rect.top = 0; \
    background_rect.right = MainForm->resource_bitmap->Width; \
    background_rect.bottom = MainForm->resource_bitmap->Height; \
    if (!MainForm->config->GetUseBitBltDraw()) \
    { \
        if (background_surface->GetDC(&dc)) \
        { \
            try \
            { \
                MainForm->draw_canvas->Handle = dc; \
                MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap); \
                MainForm->draw_canvas->Handle = 0; \
            } \
            catch (Exception &) \
            { \
                MainForm->CountDrawError(); \
                background_surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            background_surface->ReleaseDC(dc); \
        } \
    } \
    else \
    { \
        CompatibleDC = CreateCompatibleDC(0); \
        SelectObject(CompatibleDC, MainForm->resource_bitmap->Handle); \
        if (background_surface->GetDC(&dc)) \
        { \
            try \
            { \
                BitBlt(dc, 0, 0, size.width, size.height, CompatibleDC, 0, 0, 13369376); \
            } \
            catch (...) \
            { \
                background_surface->ReleaseDC(dc); \
                size.width = -1; \
                size.height = -1; \
            } \
            background_surface->ReleaseDC(dc); \
        } \
        DeleteDC(CompatibleDC); \
    } \
    return size; \
}

// Shared body: a class defines its UpdateScrollHover with DEFINE_HELP_UPDATE_SCROLL_HOVER(TheClass); each class has
// its own DragScrollThumb, scroll_up_hovered, scroll_up_held, scroll_down_hovered, scroll_down_held, scroll_dirty.
#define DEFINE_HELP_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    char changed = 0; \
    if (x >= 569 && x <= 585) \
    { \
        if (y >= 365 && y <= 432) \
            DragScrollThumb(y); \
        if (y >= 350 && y <= 365) \
        { \
            if (!scroll_up_hovered) \
            { \
                scroll_up_hovered = 1; \
                changed = 1; \
            } \
        } \
        else if (scroll_up_hovered) \
        { \
            scroll_up_hovered = 0; \
            scroll_up_held = 0; \
            changed = 1; \
        } \
        if (y >= 432 && y <= 447) \
        { \
            if (!scroll_down_hovered) \
            { \
                scroll_down_hovered = 1; \
                changed = 1; \
            } \
        } \
        else if (scroll_down_hovered) \
        { \
            scroll_down_hovered = 0; \
            scroll_down_held = 0; \
            changed = 1; \
        } \
    } \
    else if (scroll_up_hovered || scroll_down_hovered) \
    { \
        scroll_up_hovered = 0; \
        scroll_down_hovered = 0; \
        scroll_up_held = 0; \
        scroll_down_held = 0; \
        changed = 1; \
    } \
    if (changed) \
        scroll_dirty = 1; \
}

// Shared body: a class defines its Tick with DEFINE_TICK(TheClass); each class has its own scroll_repeat_delay,
// scroll_up_held, ScrollUp, scroll_down_held, ScrollDown.
#define DEFINE_TICK(Class) \
void Class::Tick() \
{ \
    if (scroll_repeat_delay > 0) \
        scroll_repeat_delay--; \
    else \
    { \
        if (scroll_up_held) \
            ScrollUp(); \
        if (scroll_down_held) \
            ScrollDown(); \
    } \
}

// Lights the scroll arrow under the mouse, lets go of an arrow the mouse has left, and drags a grabbed thumb.
// Shared body: a class defines its UpdateScrollHover with DEFINE_SKILL_UPDATE_SCROLL_HOVER(TheClass); each class has
// its own DragScrollThumb, scroll_up_hovered, scroll_up_held, scroll_down_hovered, scroll_down_held, buttons_dirty.
#define DEFINE_SKILL_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    char changed = 0; \
    if (x >= 569 && x <= 585) \
    { \
        if (y >= 347 && y <= 430) \
            DragScrollThumb(y); \
        if (y >= 332 && y <= 347) \
        { \
            if (!scroll_up_hovered) \
            { \
                scroll_up_hovered = 1; \
                changed = 1; \
            } \
        } \
        else if (scroll_up_hovered) \
        { \
            scroll_up_hovered = 0; \
            scroll_up_held = 0; \
            changed = 1; \
        } \
        if (y >= 432 && y <= 447) \
        { \
            if (!scroll_down_hovered) \
            { \
                scroll_down_hovered = 1; \
                changed = 1; \
            } \
        } \
        else if (scroll_down_hovered) \
        { \
            scroll_down_hovered = 0; \
            scroll_down_held = 0; \
            changed = 1; \
        } \
    } \
    else if (scroll_up_hovered || scroll_down_hovered) \
    { \
        scroll_up_hovered = 0; \
        scroll_down_hovered = 0; \
        scroll_up_held = 0; \
        scroll_down_held = 0; \
        changed = 1; \
    } \
    if (changed) \
        buttons_dirty = 1; \
}

// Shared body: a class defines its ClipToView with DEFINE_CLIP_TO_VIEW(TheClass); each class has its own clip_left,
// clip_top, clip_right, clip_bottom, view_left, view_top, view_right, view_bottom.
#define DEFINE_CLIP_TO_VIEW(Class) \
Windows::TRect Class::ClipToView(Class *self, Windows::TRect rect) \
{ \
    TRect clipped; \
    clipped = rect; \
    int width = rect.right - rect.left; \
    int height = rect.bottom - rect.top; \
    self->clip_left = 0; \
    self->clip_top = 0; \
    self->clip_right = 0; \
    self->clip_bottom = 0; \
    if (rect.left < self->view_left) \
    { \
        self->clip_left = self->view_left - rect.left; \
        clipped.left = self->view_left; \
    } \
    if (rect.top < self->view_top) \
    { \
        self->clip_top = self->view_top - rect.top; \
        clipped.top = self->view_top; \
    } \
    if (rect.left + width > self->view_right) \
    { \
        self->clip_right = rect.left + width - self->view_right; \
        clipped.right = self->view_right; \
    } \
    if (rect.top + height > self->view_bottom) \
    { \
        self->clip_bottom = rect.top + height - self->view_bottom; \
        clipped.bottom = self->view_bottom; \
    } \
    return clipped; \
}

// Shared body: a class defines its GetClippedSourceRect with DEFINE_GET_CLIPPED_SOURCE_RECT(TheClass); each class has
// its own is_mirrored, clip_left, clip_right, clip_top, clip_bottom.
#define DEFINE_GET_CLIPPED_SOURCE_RECT(Class) \
Windows::TRect Class::GetClippedSourceRect(Class *self, Windows::TRect rect) \
{ \
    TRect clipped; \
    clipped = rect; \
    if (!self->is_mirrored) \
    { \
        if (self->clip_left > 0) \
            clipped.left += self->clip_left; \
        if (self->clip_right > 0) \
            clipped.right -= self->clip_right; \
    } \
    else \
    { \
        if (self->clip_right > 0) \
            clipped.left += self->clip_right; \
        if (self->clip_left > 0) \
            clipped.right -= self->clip_left; \
    } \
    if (self->clip_top > 0) \
        clipped.top += self->clip_top; \
    if (self->clip_bottom > 0) \
        clipped.bottom -= self->clip_bottom; \
    return clipped; \
}

// Shared body: a class defines its TestAndClearDirty with DEFINE_TEST_AND_CLEAR_DIRTY(TheClass); each class has its
// own is_dirty.
#define DEFINE_TEST_AND_CLEAR_DIRTY(Class) \
bool Class::TestAndClearDirty() \
{ \
    bool was_dirty = 0; \
    if (is_dirty) \
    { \
        is_dirty = 0; \
        was_dirty = 1; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its IsInViewRange with DEFINE_IS_IN_VIEW_RANGE(TheClass).
#define DEFINE_IS_IN_VIEW_RANGE(Class) \
bool Class::IsInViewRange(int center_x, int center_y, int x, int y) \
{ \
    bool in_range = 0; \
    int dx = x - center_x; \
    int dy = y - center_y; \
    if (dx < 0) \
        dx = 0 - dx; \
    if (dy < 0) \
        dy = 0 - dy; \
    if (center_x < x && center_y < y) \
    { \
        if (dx + dy <= 14) \
            in_range = 1; \
    } \
    else \
    { \
        if (dx + dy <= 11) \
            in_range = 1; \
    } \
    return in_range; \
}

// Shared body: a class defines its LoadCornerButton with DEFINE_LOAD_CORNER_BUTTON(TheClass); each class has its own
// corner_button, color_key.
#define DEFINE_LOAD_CORNER_BUTTON(Class) \
void Class::LoadCornerButton(int library, int resource_id) \
{ \
    HDC dc; \
    MainForm->resource_bitmap->Width = 0; \
    MainForm->resource_bitmap->Height = 0; \
    MainForm->resource_bitmap->Transparent = false; \
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id); \
    if (corner_button->GetDC(&dc)) \
    { \
        try \
        { \
            MainForm->draw_canvas->Handle = dc; \
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap); \
        } \
        catch (Exception &) \
        { \
        } \
        corner_button->ReleaseDC(dc); \
    } \
    corner_button->SetColorKey(8, color_key); \
}

// Shared body: a class defines its LoadScrollIcons with DEFINE_LOAD_SCROLL_ICONS(TheClass); each class has its own
// scroll_icons.
#define DEFINE_LOAD_SCROLL_ICONS(Class) \
void Class::LoadScrollIcons(int library, int resource_id) \
{ \
    HDC dc; \
    MainForm->resource_bitmap->Width = 0; \
    MainForm->resource_bitmap->Height = 0; \
    MainForm->resource_bitmap->Transparent = false; \
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id); \
    if (scroll_icons->GetDC(&dc)) \
    { \
        try \
        { \
            MainForm->draw_canvas->Handle = dc; \
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap); \
        } \
        catch (Exception &) \
        { \
        } \
        scroll_icons->ReleaseDC(dc); \
    } \
}

// Shared body: a class defines its TestAndClearScrollDirty with DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(TheClass); each
// class has its own scroll_dirty.
#define DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(Class) \
bool Class::TestAndClearScrollDirty() \
{ \
    bool was_dirty = 0; \
    if (scroll_dirty) \
    { \
        scroll_dirty = 0; \
        was_dirty = 1; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its MouseUp with DEFINE_MOUSE_UP(TheClass); each class has its own drag_started,
// is_dragging, MouseDown, last_mouse_x, last_mouse_y, drag_delay_ticks, scroll_up_held, scroll_down_held.
#define DEFINE_MOUSE_UP(Class) \
void Class::MouseUp() \
{ \
    if (drag_started && is_dragging) \
    { \
        MouseDown(last_mouse_x, last_mouse_y, 0); \
        drag_delay_ticks = 1000; \
        drag_started = 0; \
    } \
    else \
    { \
        drag_delay_ticks = 1000; \
        drag_started = 0; \
    } \
    scroll_up_held = 0; \
    scroll_down_held = 0; \
}

// Shared body: a class defines its TestAndClearDragDirty with DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(TheClass); each class
// has its own drag_dirty.
#define DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(Class) \
bool Class::TestAndClearDragDirty() \
{ \
    bool was_dirty = 0; \
    if (drag_dirty) \
    { \
        drag_dirty = 0; \
        was_dirty = 1; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its GetItemAmount with DEFINE_GET_ITEM_AMOUNT(TheClass); each class has its own items.
#define DEFINE_GET_ITEM_AMOUNT(Class) \
int Class::GetItemAmount(int item_id) \
{ \
    int amount = 0; \
    vector<InventoryItem>::iterator it; \
    for (it = items.begin(); it != items.end(); it++) \
    { \
        if (item_id == it->item_id) \
        { \
            amount = it->amount; \
            break; \
        } \
    } \
    return amount; \
}

// Shared body: a class defines its Capitalize with DEFINE_CAPITALIZE(TheClass).
#define DEFINE_CAPITALIZE(Class) \
String Class::Capitalize(Class *self, String text) \
{ \
    String result; \
    if (text.Length() >= 2) \
    { \
        result = text.SubString(2, text.Length() - 1); \
        result = UpperCase(String(text[1])) + result; \
    } \
    else \
        result = text; \
    return result; \
}

// Shared body: a class defines its MouseMove with DEFINE_SKILL_MOUSE_MOVE(TheClass); each class has its own
// is_dragging, drag_x, drag_y, MarkDragDirty, UpdateScrollHover, UpdateHover.
#define DEFINE_SKILL_MOUSE_MOVE(Class) \
void Class::MouseMove(int x, int y) \
{ \
    if (is_dragging && (x != drag_x || y != drag_y)) \
    { \
        drag_x = x; \
        drag_y = y; \
        MarkDragDirty(); \
    } \
    UpdateScrollHover(x, y); \
    UpdateHover(x, y); \
}

// Shared body: a class defines its CloseBalloon with DEFINE_CLOSE_BALLOON(TheClass); each class has its own
// balloon_shown, balloon_timer, balloon, redraw.
#define DEFINE_CLOSE_BALLOON(Class) \
void Class::CloseBalloon() \
{ \
    if (balloon_shown) \
    { \
        balloon_shown = 0; \
        balloon_timer = 0; \
        delete balloon; \
    } \
    redraw = 1; \
}

// Shared body: a class defines its TestAndClearButtonsDirty with DEFINE_TEST_AND_CLEAR_BUTTONS_DIRTY(TheClass); each
// class has its own buttons_dirty.
#define DEFINE_TEST_AND_CLEAR_BUTTONS_DIRTY(Class) \
bool Class::TestAndClearButtonsDirty() \
{ \
    bool was_dirty = 0; \
    if (buttons_dirty) \
    { \
        buttons_dirty = 0; \
        was_dirty = 1; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its TestAndClearListDirty with DEFINE_TEST_AND_CLEAR_LIST_DIRTY(TheClass); each class
// has its own list_dirty.
#define DEFINE_TEST_AND_CLEAR_LIST_DIRTY(Class) \
bool Class::TestAndClearListDirty() \
{ \
    bool was_dirty = 0; \
    if (list_dirty) \
    { \
        list_dirty = 0; \
        was_dirty = 1; \
    } \
    return was_dirty; \
}

// Empties the text page and scrolls back to the top.
// Shared body: a class defines its ClearText with DEFINE_CLEAR_TEXT(TheClass); each class has its own lines,
// unused_lines, link_line_index, hovered_row, top_line, scroll_thumb_offset.
#define DEFINE_CLEAR_TEXT(Class) \
void Class::ClearText() \
{ \
    lines->Clear(); \
    unused_lines->Clear(); \
    link_line_index = 0; \
    hovered_row = 0; \
    top_line = 0; \
    scroll_thumb_offset = 0; \
}

// Shared body: a class defines its GetScrollIconRect with DEFINE_GET_SCROLL_ICON_RECT(TheClass).
#define DEFINE_GET_SCROLL_ICON_RECT(Class) \
Windows::TRect Class::GetScrollIconRect(Class *self, int icon) \
{ \
    TRect scroll_icon_rect; \
    scroll_icon_rect.left = 0; \
    scroll_icon_rect.top = 15 * icon; \
    scroll_icon_rect.right = 16; \
    scroll_icon_rect.bottom = 15 * icon + 15; \
    return scroll_icon_rect; \
}

#endif
