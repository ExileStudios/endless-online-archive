#ifndef TMainFormH
#define TMainFormH

#include <AppEvnts.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <Graphics.hpp>
#include <ScktComp.hpp>
#include <Sysutils.hpp>
#include <TDDPixelFormat.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDIDevCaps.H>
#include <TDIKeyboardState.H>
#include <TDSBufferDesc.H>
#include <TDx_Draw.H>
#include <TDx_DrawClipper.H>
#include <TDx_DrawSurface.H>
#include <TDx_Input.H>
#include <TDx_InputDevice.H>
#include <TDx_Sound.H>
#include <TDx_SoundBuffer.H>
#include <vector.h>

#include "Animationprotector.h"
#include "Classvalues.h"
#include "Configuration.h"
#include "Filelogger.h"
#include "Fileprotection.h"
#include "Filethemes.h"
#include "Fonts.h"
#include "Gfx_alpha.h"
#include "Gfx_edits.h"
#include "Gfxprotection.h"
#include "Gfxtransfers.h"
#include "Gfxvault.h"
#include "Itemvalues.h"
#include "Mapprotection.h"
#include "Npcvalues.h"
#include "Packetcontrols.h"
#include "Pubfileheader.h"
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

#define BASE_SCREEN_HEIGHT 480.0
#define GRID_THUMB_TRAVEL 68.0
#define NPC_LIST_ROW_HEIGHT 37
#define NPC_TEXT_LINE_HEIGHT 16
#define NPC_THUMB_TRAVEL 152.0
#define PANEL_THUMB_TRAVEL 50.0
#define SLOW_UPDATE_INTERVAL 20

enum NpcGraphic
{
    NpcGraphic_Wraith = 85,
    NpcGraphic_CursedMask = 99,
    NpcGraphic_AncientWraith = 171
};

// The game window: it builds every controller, runs both timers and draws each frame when idle.
class PACKAGE TMainForm : public TForm
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
    TDx_SoundBuffer *Dx_SoundBuffer5;
    TDx_SoundBuffer *Dx_SoundBuffer6;
    TDx_SoundBuffer *Dx_SoundBuffer7;
    TDx_Input *Dx_Input1;
    TDIDevCaps *DIDevCaps1;
    TDIKeyboardState *DIKeyboardState1;
    TDx_InputDevice *Dx_InputDevice1;
    TClientSocket *client;
    TApplicationEvents *ApplicationEvents1;
    TTimer *Timer1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall Dx_Draw1Create(TObject *Sender);
    void __fastcall Dx_Draw1Destroy(TObject *Sender);
    void __fastcall PrimarySurfaceSurfaceLost(TObject *Sender);
    void __fastcall PrimarySurfaceCreate(TObject *Sender);
    void __fastcall Dx_Sound1Create(TObject *Sender);
    void __fastcall Dx_Sound1Destroy(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall Dx_Draw1EnumDisplayModes(TObject *Sender, TDDSurfaceDesc *SurfaceDesc, void *Context,
    bool &Finished);
    void __fastcall Dx_Sound1DSEnumerate(TObject *Sender, _GUID *Guid, String DriverDescription,
    String DriverModule, void *Context, bool &Finished);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseWheelDown(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled);
    void __fastcall FormMouseWheelUp(TObject *Sender, TShiftState Shift, TPoint &MousePos, bool &Handled);
    void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall Dx_Input1Destroy(TObject *Sender);
    void __fastcall Dx_InputDevice1Destroy(TObject *Sender);
    void __fastcall Dx_InputDevice1Create(TObject *Sender);
    void __fastcall clientError(TObject *Sender, TCustomWinSocket *Socket,
    TErrorEvent ErrorEvent, int &ErrorCode);
    void __fastcall clientRead(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall clientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall clientConnect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ApplicationEvents1Exception(TObject *Sender, Exception *E);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall ApplicationEvents1Activate(TObject *Sender);
    void __fastcall ApplicationEvents1Deactivate(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:
public:
    Fonts *fonts;
    GfxVault *gfx_vault;
    GfxTransfers *gfx_transfers;
    StringContainer *strings;
    FileController *files;
    AnimationController *animation;
    SfxController *sfx;
    GfxController *gfx;
    MapController *map;
    Config *config;
    FileThemes *themes;
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
    TRect screen_rect;
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
    int display_bit_depth;
    int refresh_rate;
    int unused_value;
    bool redraw_needed;
    bool dragging;
    String form_text;
    int display_mode_count;
    int mm_timer_id;
    short version_major;
    short version_minor;
    short version_build;
    char version_letter;
    short min_theme_version_major;
    short min_theme_version_minor;
    bool minimized;
    char unused_bytes[1];
    bool active;
    bool initialized;
    unsigned char login_screen_delay;
    unsigned char full_paint_delay;
    int draw_error_count;
    char second_unused_bytes[4];
    long start_time;
    long current_time;
    String clock_text;
    bool flip_needed;
    bool animation_tick_due;
    char keyboard_acquired;
    TTimeStamp start_stamp;
    int time_value_a;
    int time_value_b;
    HMENU scale_menu;
    HMENU status_widget_menu;
    HMENU minimap_menu;
    HMENU auto_drop_menu;
    bool item_drag_drawn;
    bool spell_drag_drawn;
    bool skill_drag_drawn;
    int credit_name_color;

    __fastcall TMainForm(TComponent *owner);
    virtual __fastcall ~TMainForm();
    void UpdateSystemMenu();
    void __fastcall WMSysCommand(TWMSysCommand &message);
    void CountDrawError();
    bool isActive();
    void PaintFullDelayed();
    void __fastcall ActivateForm(TObject *sender);
    void __fastcall DeactivateForm(TObject *sender);
    void __fastcall MinimizeForm(TObject *sender);
    void __fastcall RestoreForm(TObject *sender);
    String VersionString(int major, int minor, int build, char letter);
    void __fastcall HandleMMTimerEvent();
    void IntroAnimator();
    void RenderMainLoop();
    void RenderMainLoop_Game();
    void RenderGame_ViewPort_Frame();
    void RenderGame_BottomPanel();
    void RenderGame_MapLoadScreen();
    void RenderMainLoop_Menu();
    void SampleCreditNameColor();
    void RenderMainLoop_Intro();
    void RenderFpsOverlay();
    void RenderOffscreenToPrimary();
    void RenderDrag();
    void RenderDragUndo();
    char FormIsOnPrimaryScreen();
    String FormatClock(int value);

BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(WM_SYSCOMMAND, TWMSysCommand, WMSysCommand)
END_MESSAGE_MAP(TForm)
};

void __stdcall MMTimerProc(unsigned int timer_id, unsigned int message, unsigned long user_data,
    unsigned long first_reserved, unsigned long second_reserved);
extern PACKAGE TMainForm *MainForm;

// Shared body: a class defines its GetScrollIconRect with DEFINE_GET_SCROLL_ICON_RECT(TheClass).
#define DEFINE_GET_SCROLL_ICON_RECT(Class) \
TRect Class::GetScrollIconRect(int icon) \
{ \
    TRect scroll_icon_rect; \
    scroll_icon_rect.left = 0; \
    scroll_icon_rect.top = 15 * icon; \
    scroll_icon_rect.right = 16; \
    scroll_icon_rect.bottom = 15 * icon + 15; \
    return scroll_icon_rect; \
}

// Shared body: a class defines its LoadSurface with DEFINE_LOAD_SURFACE(TheClass); each class has its own surface_desc,
// surface_caps, surface, surface_rect, color_key.
#define DEFINE_LOAD_SURFACE(Class) \
TPoint Class::LoadSurface(int library, int resource_id) \
{ \
    TPoint size; \
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id)) \
        return TPoint(-1, -1); \
    size = *(TPoint *)MainForm->gfx_transfers; \
    surface_desc->SCaps = surface_caps; \
    MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y); \
    surface_rect.left = 0; \
    surface_rect.top = 0; \
    surface_rect.right = size.x; \
    surface_rect.bottom = size.y; \
    if (MainForm->gfx_transfers->BeginDraw(surface)) \
    { \
        MainForm->gfx_transfers->Draw(surface); \
        MainForm->gfx_transfers->EndDraw(surface); \
        surface->SetColorKey(DDCKEY_SRCBLT, color_key); \
        return size; \
    } \
    surface->SetColorKey(DDCKEY_SRCBLT, color_key); \
    return TPoint(-1, -1); \
}

// Encodes a number as base-253 digits plus one, lowest first, padded to min_length with the filler byte.
// Shared body: a class defines its EncodeNumber with DEFINE_ENCODE_NUMBER(TheClass).
#define DEFINE_ENCODE_NUMBER(Class) \
String Class::EncodeNumber(unsigned int value, int width) \
{ \
    unsigned int remaining; \
    char digit_char; \
    unsigned int quotient; \
    int i; \
    unsigned int remainder; \
    remaining = value; \
    String result = ""; \
    try \
    { \
        do \
        { \
            double exact_quotient = remaining / 253.0; \
            quotient = (int)exact_quotient; \
            remainder = remaining % NUMBER_ENCODING_BASE; \
            digit_char = remainder; \
            digit_char++; \
            result.Insert(digit_char, result.Length() + 1); \
            if (quotient >= 1) \
                remaining = quotient; \
        } while (quotient >= 1); \
    } \
    catch (...) \
    { \
        result = ""; \
    } \
    if (result.Length() < width) \
    { \
        char filler = -2; \
        int pad_count = width - result.Length(); \
        for (i = 0; i < pad_count; i++) \
            result.Insert((char)filler, result.Length() + 1); \
    } \
    return result; \
}

// Shared body: a class defines its Capitalize with DEFINE_CAPITALIZE(TheClass).
#define DEFINE_CAPITALIZE(Class) \
String Class::Capitalize(String text) \
{ \
    String result; \
    if (text.Length() >= 2) \
    { \
        result = text.SubString(2, text.Length() - 1); \
        result = UpperCase(text[1]) + result; \
    } \
    else \
        result = text; \
    return result; \
}

// Shared body: a class defines its TestAndClearDirty with DEFINE_TEST_AND_CLEAR_DIRTY(TheClass); each class has its own
// is_dirty.
#define DEFINE_TEST_AND_CLEAR_DIRTY(Class) \
bool Class::TestAndClearDirty() \
{ \
    bool was_dirty = false; \
    if (is_dirty) \
    { \
        is_dirty = false; \
        was_dirty = true; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its Tick with DEFINE_TICK(TheClass); each class has its own scroll_repeat_delay,
// scroll_up_held, ScrollUp, scroll_down_held, ScrollDown.
#define DEFINE_TICK(Class) \
void Class::Tick() \
{ \
    if (scroll_repeat_delay > 0) \
    { \
        scroll_repeat_delay--; \
        return; \
    } \
    if (scroll_up_held) \
        ScrollUp(); \
    if (scroll_down_held) \
        ScrollDown(); \
}

// Shared body: a class defines its UpdateScrollHover with DEFINE_HELP_UPDATE_SCROLL_HOVER(TheClass); each class has its
// own DragScrollThumb, scroll_up_hovered, scroll_up_held, scroll_down_hovered, scroll_down_held, scroll_dirty.
#define DEFINE_HELP_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    bool changed = false; \
    DragScrollThumb(y); \
    if (x >= 569 && x <= 585) \
    { \
        if (y >= 350 && y <= 365) \
        { \
            if (!scroll_up_hovered) \
            { \
                scroll_up_hovered = true; \
                changed = true; \
            } \
        } \
        else if (scroll_up_hovered) \
        { \
            scroll_up_hovered = false; \
            scroll_up_held = false; \
            changed = true; \
        } \
        if (y >= 432 && y <= 447) \
        { \
            if (!scroll_down_hovered) \
            { \
                scroll_down_hovered = true; \
                changed = true; \
            } \
        } \
        else if (scroll_down_hovered) \
        { \
            scroll_down_hovered = false; \
            scroll_down_held = false; \
            changed = true; \
        } \
    } \
    else if (scroll_up_hovered || scroll_down_hovered) \
    { \
        scroll_up_hovered = false; \
        scroll_down_hovered = false; \
        scroll_up_held = false; \
        scroll_down_held = false; \
        changed = true; \
    } \
    if (changed) \
        scroll_dirty = true; \
}

// Shared body: a class defines its TestAndClearScrollDirty with DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(TheClass); each
// class has its own scroll_dirty.
#define DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(Class) \
bool Class::TestAndClearScrollDirty() \
{ \
    bool was_dirty = false; \
    if (scroll_dirty) \
    { \
        scroll_dirty = false; \
        was_dirty = true; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its TestAndClearListDirty with DEFINE_TEST_AND_CLEAR_LIST_DIRTY(TheClass); each class
// has its own list_dirty.
#define DEFINE_TEST_AND_CLEAR_LIST_DIRTY(Class) \
bool Class::TestAndClearListDirty() \
{ \
    bool was_dirty = false; \
    if (list_dirty) \
    { \
        list_dirty = false; \
        was_dirty = true; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its TestAndClearDragDirty with DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(TheClass); each class
// has its own drag_dirty.
#define DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(Class) \
bool Class::TestAndClearDragDirty() \
{ \
    bool was_dirty = false; \
    if (drag_dirty) \
    { \
        drag_dirty = false; \
        was_dirty = true; \
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

// Shared body: a class defines its LoadBackground with DEFINE_LOAD_BACKGROUND(TheClass); each class has its own
// surface_desc, surface_caps, background_surface, background_rect.
#define DEFINE_LOAD_BACKGROUND(Class) \
TPoint Class::LoadBackground(int library, int resource_id) \
{ \
    TPoint size; \
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id)) \
        return TPoint(-1, -1); \
    size = *(TPoint *)MainForm->gfx_transfers; \
    surface_desc->SCaps = surface_caps; \
    MainForm->gfx_transfers->CreateSurface(background_surface, surface_desc, size.x, size.y); \
    background_rect.left = 0; \
    background_rect.top = 0; \
    background_rect.right = size.x; \
    background_rect.bottom = size.y; \
    if (MainForm->gfx_transfers->BeginDraw(background_surface)) \
    { \
        MainForm->gfx_transfers->Draw(background_surface); \
        MainForm->gfx_transfers->EndDraw(background_surface); \
        return size; \
    } \
    return TPoint(-1, -1); \
}

// Shared body: a class defines its LoadItemSlot with DEFINE_LOAD_ITEM_SLOT(TheClass); each class has its own
// surface_desc, surface_caps, item_slot_surface, item_slot_rect, color_key.
#define DEFINE_LOAD_ITEM_SLOT(Class) \
TPoint Class::LoadItemSlot(int library, int resource_id) \
{ \
    TPoint size; \
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id)) \
        return TPoint(-1, -1); \
    size = *(TPoint *)MainForm->gfx_transfers; \
    surface_desc->SCaps = surface_caps; \
    MainForm->gfx_transfers->CreateSurface(item_slot_surface, surface_desc, size.x, size.y); \
    item_slot_rect.left = 0; \
    item_slot_rect.top = 0; \
    item_slot_rect.right = size.x; \
    item_slot_rect.bottom = size.y; \
    if (MainForm->gfx_transfers->BeginDraw(item_slot_surface)) \
    { \
        MainForm->gfx_transfers->Draw(item_slot_surface); \
        MainForm->gfx_transfers->EndDraw(item_slot_surface); \
        item_slot_surface->SetColorKey(DDCKEY_SRCBLT, color_key); \
        return size; \
    } \
    item_slot_surface->SetColorKey(DDCKEY_SRCBLT, color_key); \
    return TPoint(-1, -1); \
}

// Hides the window and frees its graphics.
// Shared body: a class defines its Close with DEFINE_CLOSE(TheClass); each class has its own tooltip, is_visible,
// npc_session_id.
#define DEFINE_CLOSE(Class) \
void Class::Close() \
{ \
    tooltip->Hide(); \
    is_visible = false; \
    npc_session_id = -1; \
    MainForm->gfx->FreeShopGraphics(); \
    MainForm->redraw_needed = true; \
}

// Shared body: a class defines its UpdateScrollHover with DEFINE_SHOP_UPDATE_SCROLL_HOVER(TheClass); each class has its
// own DragScrollThumb, window_x, window_y, scroll_up_hovered, scroll_down_hovered.
#define DEFINE_SHOP_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    DragScrollThumb(y); \
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59) \
    { \
        if (!scroll_up_hovered) \
        { \
            scroll_up_hovered = true; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else if (scroll_up_hovered) \
    { \
        scroll_up_hovered = false; \
        MainForm->redraw_needed = true; \
    } \
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243) \
    { \
        if (!scroll_down_hovered) \
        { \
            scroll_down_hovered = true; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else if (scroll_down_hovered) \
    { \
        scroll_down_hovered = false; \
        MainForm->redraw_needed = true; \
    } \
}

// Shared body: a class defines its UpdateScrollHover with DEFINE_UPDATE_SCROLL_HOVER(TheClass); each class has its own
// window_x, window_y, DragScrollThumb, scroll_up_hovered, scroll_down_hovered.
#define DEFINE_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    if (x >= window_x + 252 && x <= window_x + 267) \
    { \
        if (y >= window_y + 58 && y <= window_y + 229) \
            DragScrollThumb(y); \
    } \
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59) \
    { \
        if (!scroll_up_hovered) \
        { \
            scroll_up_hovered = true; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else if (scroll_up_hovered) \
    { \
        scroll_up_hovered = false; \
        MainForm->redraw_needed = true; \
    } \
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243) \
    { \
        if (!scroll_down_hovered) \
        { \
            scroll_down_hovered = true; \
            MainForm->redraw_needed = true; \
        } \
    } \
    else if (scroll_down_hovered) \
    { \
        scroll_down_hovered = false; \
        MainForm->redraw_needed = true; \
    } \
}

// Shared body: a class defines its LoadSurface with DEFINE_BANK_LOAD_SURFACE(TheClass); each class has its own
// surface_desc, surface_caps, surface, surface_rect.
#define DEFINE_BANK_LOAD_SURFACE(Class) \
TPoint Class::LoadSurface(int library, int resource_id) \
{ \
    TPoint size; \
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id)) \
        return TPoint(-1, -1); \
    size = *(TPoint *)MainForm->gfx_transfers; \
    surface_desc->SCaps = surface_caps; \
    MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y); \
    surface_rect.left = 0; \
    surface_rect.top = 0; \
    surface_rect.right = size.x; \
    surface_rect.bottom = size.y; \
    if (MainForm->gfx_transfers->BeginDraw(surface)) \
    { \
        MainForm->gfx_transfers->Draw(surface); \
        MainForm->gfx_transfers->EndDraw(surface); \
        return size; \
    } \
    return TPoint(-1, -1); \
}

// Adds a text to the text page, wrapped to the page width, with an optional blank line after it.
// Shared body: a class defines its AddText with DEFINE_ADD_TEXT(TheClass); each class has its own link_line_index,
// lines.
#define DEFINE_ADD_TEXT(Class) \
void Class::AddText(String text, bool add_blank_line) \
{ \
    if (text.Length() > 0 && text[1] == '*') \
        link_line_index = lines->Count; \
    int max_width = 200; \
    if (MainForm->fonts->BeginOffscreenIf(200)) \
    { \
        try \
        { \
            MainForm->fonts->face = 1; \
            MainForm->fonts->size = 8; \
            do \
            { \
                String fitted_text; \
                if ((short)MainForm->fonts->TextWidth(text) <= max_width) \
                { \
                    lines->Add(text); \
                    text = ""; \
                } \
                else \
                { \
                    char split_done = false; \
                    for (int i = 1; i <= text.Length(); i++) \
                    { \
                        if (text[i] == ' ' || i == text.Length()) \
                        { \
                            String prefix = text.SubString(1, i); \
                            if ((short)MainForm->fonts->TextWidth(prefix) >= max_width) \
                            { \
                                lines->Add(fitted_text); \
                                text.Delete(1, fitted_text.Length()); \
                                split_done = true; \
                                break; \
                            } \
                            fitted_text = prefix; \
                        } \
                    } \
                    if (!split_done) \
                    { \
                        lines->Add(text); \
                        text = ""; \
                    } \
                } \
            } while (text.Length() > 0); \
        } \
        catch (Exception &) \
        { \
        } \
        MainForm->fonts->End(); \
    } \
    if (add_blank_line) \
        lines->Add(""); \
    MainForm->redraw_needed = true; \
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

// Lights the scroll arrow under the mouse, lets go of an arrow the mouse has left, and drags a grabbed thumb.
// Shared body: a class defines its UpdateScrollHover with DEFINE_SKILL_UPDATE_SCROLL_HOVER(TheClass); each class has
// its own DragScrollThumb, scroll_up_hovered, scroll_up_held, scroll_down_hovered, scroll_down_held, buttons_dirty.
#define DEFINE_SKILL_UPDATE_SCROLL_HOVER(Class) \
void Class::UpdateScrollHover(int x, int y) \
{ \
    bool changed = false; \
    DragScrollThumb(y); \
    if (x >= 569 && x <= 585) \
    { \
        if (y >= 332 && y <= 347) \
        { \
            if (!scroll_up_hovered) \
            { \
                scroll_up_hovered = true; \
                changed = true; \
            } \
        } \
        else if (scroll_up_hovered) \
        { \
            scroll_up_hovered = false; \
            scroll_up_held = false; \
            changed = true; \
        } \
        if (y >= 432 && y <= 447) \
        { \
            if (!scroll_down_hovered) \
            { \
                scroll_down_hovered = true; \
                changed = true; \
            } \
        } \
        else if (scroll_down_hovered) \
        { \
            scroll_down_hovered = false; \
            scroll_down_held = false; \
            changed = true; \
        } \
    } \
    else if (scroll_up_hovered || scroll_down_hovered) \
    { \
        scroll_up_hovered = false; \
        scroll_down_hovered = false; \
        scroll_up_held = false; \
        scroll_down_held = false; \
        changed = true; \
    } \
    if (changed) \
        buttons_dirty = true; \
}

// Shared body: a class defines its MouseUp with DEFINE_MOUSE_UP(TheClass); each class has its own drag_started,
// is_dragging, MouseDown, last_mouse_x, last_mouse_y, drag_delay_ticks, scroll_up_held, scroll_down_held,
// thumb_grabbed.
#define DEFINE_MOUSE_UP(Class) \
void Class::MouseUp() \
{ \
    if (drag_started && is_dragging) \
    { \
        MouseDown(last_mouse_x, last_mouse_y, 0); \
        drag_delay_ticks = 1000; \
        drag_started = false; \
    } \
    else \
    { \
        drag_delay_ticks = 1000; \
        drag_started = false; \
    } \
    scroll_up_held = false; \
    scroll_down_held = false; \
    thumb_grabbed = false; \
}

// Shared body: a class defines its TestAndClearButtonsDirty with DEFINE_TEST_AND_CLEAR_BUTTONS_DIRTY(TheClass); each
// class has its own buttons_dirty.
#define DEFINE_TEST_AND_CLEAR_BUTTONS_DIRTY(Class) \
bool Class::TestAndClearButtonsDirty() \
{ \
    bool was_dirty = false; \
    if (buttons_dirty) \
    { \
        buttons_dirty = false; \
        was_dirty = true; \
    } \
    return was_dirty; \
}

// Shared body: a class defines its LoadSurface with DEFINE_ICON_LOAD_SURFACE(TheClass); each class has its own
// surface_desc, surface_caps, surface, color_key.
#define DEFINE_ICON_LOAD_SURFACE(Class) \
TPoint Class::LoadSurface(int library, int resource_id) \
{ \
    TPoint size; \
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id)) \
        return TPoint(-1, -1); \
    size = *(TPoint *)MainForm->gfx_transfers; \
    surface_desc->SCaps = surface_caps; \
    MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y); \
    if (MainForm->gfx_transfers->BeginDraw(surface)) \
    { \
        MainForm->gfx_transfers->Draw(surface); \
        MainForm->gfx_transfers->EndDraw(surface); \
        surface->SetColorKey(DDCKEY_SRCBLT, color_key); \
        return size; \
    } \
    surface->SetColorKey(DDCKEY_SRCBLT, color_key); \
    return TPoint(-1, -1); \
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

// Shared body: a class defines its Close with DEFINE_HOTKEYS_CLOSE(TheClass); each class has its own is_visible.
#define DEFINE_HOTKEYS_CLOSE(Class) \
void Class::Close() \
{ \
    if (is_visible) \
    { \
        is_visible = false; \
        MainForm->redraw_needed = true; \
    } \
}

#endif
