#include <vcl.h>
#pragma hdrstop

#include "Gfx_interface.h"
#include "Tmainform.h"
#include "Gfx_icon.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Gfx_subitem.h"
#include "Ctrl_chat.h"
#include "Ctrl_gamebars.h"
#include "Ctrl_help.h"
#include "Ctrl_news.h"
#include "Ctrl_playerlist.h"
#include "Ctrl_settings.h"
#include "Ctrl_status.h"
#include <io.h>
#include <vector.h>
#include "Ctrl_group.h"
#include "Ctrl_inventory.h"
#include "Ctrl_skills.h"
#include "Ctrl_spells.h"
#include "Ctrl_tooltip.h"
#include "Gfx_mapicons.h"
#include "Gfx_mapitems.h"

#pragma package(smart_init)

// Creates every interface surface at its fixed size and sets the source rects of the HUD sheet's pieces.
InterfaceGraphics::InterfaceGraphics()
{
    theme_0_colors[0] = 0;
    theme_0_colors[1] = 0xA5CACF;
    theme_0_colors[2] = 0xBBE3E3;
    theme_0_colors[3] = 0x4C5AF0;
    theme_0_colors[4] = 0xBBC3C4;
    theme_0_colors[5] = 0xBBD7E2;
    theme_0_colors[6] = 0x42DFE2;
    theme_1_colors[0] = 0xB4B4B4;
    theme_1_colors[1] = 0xB2C7C8;
    theme_1_colors[2] = 0xC1BFB2;
    theme_1_colors[3] = 0x4250CD;
    theme_1_colors[4] = 0xCECDB1;
    theme_1_colors[5] = 0xC7B2BB;
    theme_1_colors[6] = 0xB2C7BB;
    hud_background = new TDx_DrawSurface(NULL);
    clock_background = new TDx_DrawSurface(NULL);
    tooltip_background = new TDx_DrawSurface(NULL);
    chat_panel_surface = new TDx_DrawSurface(NULL);
    chat_mode_icons = new TDx_DrawSurface(NULL);
    chat_input_background = new TDx_DrawSurface(NULL);
    small_icons = new TDx_DrawSurface(NULL);
    chat_tabs = new TDx_DrawSurface(NULL);
    corner_button = new TDx_DrawSurface(NULL);
    minimap_tiles = new TDx_DrawSurface(NULL);
    status_icons = new TDx_DrawSurface(NULL);
    hud_sheet = new TDx_DrawSurface(NULL);
    tab_buttons = new TDx_DrawSurface(NULL);
    list_icons = new TDx_DrawSurface(NULL);
    control_sheet = new TDx_DrawSurface(NULL);
    scroll_icons = new TDx_DrawSurface(NULL);
    panel_background = new TDx_DrawSurface(NULL);
    loading_icon = new TDx_DrawSurface(NULL);
    group_icons = new TDx_DrawSurface(NULL);
    hud_frame = new TDx_DrawSurface(NULL);
    hud_left_frame = new TDx_DrawSurface(NULL);
    hud_top_frame = new TDx_DrawSurface(NULL);
    tile_cursor = new TDx_DrawSurface(NULL);
    emote_sheet = new TDx_DrawSurface(NULL);
    weapon_effects = new TDx_DrawSurface(NULL);
    drag_save_surface = new TDx_DrawSurface(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 640;
    surface_desc->Height = 480;
    hud_background->GetSurfaceDesc(surface_desc);
    hud_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 86;
    surface_desc->Height = 18;
    clock_background->GetSurfaceDesc(surface_desc);
    clock_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 443;
    surface_desc->Height = 39;
    tooltip_background->GetSurfaceDesc(surface_desc);
    tooltip_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 443;
    surface_desc->Height = 100;
    chat_panel_surface->GetSurfaceDesc(surface_desc);
    chat_panel_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 491;
    surface_desc->Height = 18;
    chat_input_background->GetSurfaceDesc(surface_desc);
    chat_input_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 13;
    surface_desc->Height = 312;
    small_icons->GetSurfaceDesc(surface_desc);
    small_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 478;
    surface_desc->Height = 32;
    chat_tabs->GetSurfaceDesc(surface_desc);
    chat_tabs->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 51;
    surface_desc->Height = 106;
    corner_button->GetSurfaceDesc(surface_desc);
    corner_button->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 243;
    surface_desc->Height = 14;
    minimap_tiles->GetSurfaceDesc(surface_desc);
    minimap_tiles->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 83;
    surface_desc->Height = 168;
    chat_mode_icons->GetSurfaceDesc(surface_desc);
    chat_mode_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 72;
    surface_desc->Height = 209;
    tab_buttons->GetSurfaceDesc(surface_desc);
    tab_buttons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 49;
    surface_desc->Height = 53;
    hud_frame->GetSurfaceDesc(surface_desc);
    hud_frame->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 4;
    surface_desc->Height = 257;
    hud_left_frame->GetSurfaceDesc(surface_desc);
    hud_left_frame->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 540;
    surface_desc->Height = 4;
    hud_top_frame->GetSurfaceDesc(surface_desc);
    hud_top_frame->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 320;
    surface_desc->Height = 32;
    tile_cursor->GetSurfaceDesc(surface_desc);
    tile_cursor->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 323;
    surface_desc->Height = 478;
    control_sheet->GetSurfaceDesc(surface_desc);
    control_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 300;
    surface_desc->Height = 30;
    list_icons->GetSurfaceDesc(surface_desc);
    list_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 16;
    surface_desc->Height = 330;
    scroll_icons->GetSurfaceDesc(surface_desc);
    scroll_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 40;
    surface_desc->Height = 40;
    loading_icon->GetSurfaceDesc(surface_desc);
    loading_icon->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 485;
    surface_desc->Height = 119;
    panel_background->GetSurfaceDesc(surface_desc);
    panel_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 3000;
    surface_desc->Height = 50;
    emote_sheet->GetSurfaceDesc(surface_desc);
    emote_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 232;
    surface_desc->Height = 522;
    weapon_effects->GetSurfaceDesc(surface_desc);
    weapon_effects->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 56;
    surface_desc->Height = 30;
    group_icons->GetSurfaceDesc(surface_desc);
    group_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 80;
    surface_desc->Height = 40;
    status_icons->GetSurfaceDesc(surface_desc);
    status_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 439;
    surface_desc->Height = 64;
    hud_sheet->GetSurfaceDesc(surface_desc);
    hud_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 50;
    surface_desc->Height = 102;
    drag_save_surface->GetSurfaceDesc(surface_desc);
    drag_save_surface->Create(surface_desc, MainForm->Dx_Draw1);
    drag_rect.top = 0;
    drag_rect.left = 0;
    drag_rect.bottom = 102;
    drag_rect.right = 50;
    hud_background_rect.top = 0;
    hud_background_rect.left = 0;
    hud_background_rect.bottom = 480;
    hud_background_rect.right = 640;
    clock_rect.top = 0;
    clock_rect.left = 0;
    clock_rect.bottom = 18;
    clock_rect.right = 86;
    tooltip_rect.top = 0;
    tooltip_rect.left = 0;
    tooltip_rect.bottom = 18;
    tooltip_rect.right = 443;
    chat_input_rect.top = 0;
    chat_input_rect.left = 0;
    chat_input_rect.bottom = 18;
    chat_input_rect.right = 491;
    hud_frame_rect.top = 0;
    hud_frame_rect.left = 0;
    hud_frame_rect.bottom = 53;
    hud_frame_rect.right = 49;
    hud_left_frame_rect.top = 0;
    hud_left_frame_rect.left = 0;
    hud_left_frame_rect.bottom = 257;
    hud_left_frame_rect.right = 4;
    hud_top_frame_rect.top = 0;
    hud_top_frame_rect.left = 0;
    hud_top_frame_rect.bottom = 4;
    hud_top_frame_rect.right = 540;
    panel_rect.top = 0;
    panel_rect.left = 0;
    panel_rect.bottom = 119;
    panel_rect.right = 485;
    status_bars_frame_rect.top = 0;
    status_bars_frame_rect.left = 0;
    status_bars_frame_rect.bottom = 14;
    status_bars_frame_rect.right = 439;
    miss_label_rect.top = 28;
    miss_label_rect.left = 132;
    miss_label_rect.bottom = 38;
    miss_label_rect.right = 161;
    health_bar_frame_rect.top = 28;
    health_bar_frame_rect.left = 0;
    health_bar_frame_rect.bottom = 35;
    health_bar_frame_rect.right = 40;
    health_bar_shadow_rect.top = 56;
    health_bar_shadow_rect.left = 0;
    health_bar_shadow_rect.bottom = 63;
    health_bar_shadow_rect.right = 40;
    bar_text_box_rect.top = 29;
    bar_text_box_rect.left = 220;
    bar_text_box_rect.bottom = 49;
    bar_text_box_rect.right = 329;
    loading_icon_rect.top = 0;
    loading_icon_rect.left = 0;
    loading_icon_rect.bottom = 40;
    loading_icon_rect.right = 40;
    status_icon_left_rect.top = 29;
    status_icon_left_rect.left = 331;
    status_icon_left_rect.bottom = 43;
    status_icon_left_rect.right = 353;
    status_icon_right_rect.top = 29;
    status_icon_right_rect.left = 353;
    status_icon_right_rect.bottom = 43;
    status_icon_right_rect.right = 375;
    book_scrollbar_rect.top = 0;
    book_scrollbar_rect.left = 303;
    book_scrollbar_rect.bottom = 238;
    book_scrollbar_rect.right = 322;
    minimap_visible = false;
}

int InterfaceGraphics::GetThemeColor(int theme, int shade)
{
    if (theme > 1)
        shade = 1;
    if (shade > 5)
        shade = 5;
    if (theme == 1)
        return theme_1_colors[shade];
    return theme_0_colors[shade];
}

// One piece of the chat tab strip: the whole strip, or one of the wide, medium or small tabs in pairs.
TRect InterfaceGraphics::GetChatTabRect(int tab)
{
    TRect chat_tab_rect;
    if (tab <= 0)
    {
        chat_tab_rect.left = 0;
        chat_tab_rect.top = 0;
        chat_tab_rect.right = 441;
        chat_tab_rect.bottom = 15;
    }
    if (tab == 1 || tab == 2)
    {
        int wide_index = tab - 1;
        chat_tab_rect.left = 132 * wide_index;
        chat_tab_rect.top = 16;
        chat_tab_rect.right = 132 * wide_index + 132;
        chat_tab_rect.bottom = 31;
    }
    if (tab == 3 || tab == 4)
    {
        int medium_index = tab - 3;
        chat_tab_rect.left = 43 * medium_index + 264;
        chat_tab_rect.top = 16;
        chat_tab_rect.right = 43 * medium_index + 307;
        chat_tab_rect.bottom = 31;
    }
    if (tab == 5 || tab == 6)
    {
        int small_index = tab - 5;
        chat_tab_rect.left = 19 * small_index + 440;
        chat_tab_rect.top = 16;
        chat_tab_rect.right = 19 * small_index + 458;
        chat_tab_rect.bottom = 31;
    }
    return chat_tab_rect;
}

// One damage digit sprite of the HUD sheet; the style picks the row.
TRect InterfaceGraphics::GetDamageDigitRect(int style, int digit)
{
    TRect damage_digit_rect;
    damage_digit_rect.left = 9 * digit + 40;
    damage_digit_rect.right = 9 * digit + 49;
    damage_digit_rect.top = 10 * style + 28;
    damage_digit_rect.bottom = 10 * style + 39;
    return damage_digit_rect;
}

// The fill of an overhead health bar of a given length; the colour picks the row.
TRect InterfaceGraphics::GetHealthBarFillRect(int color, int length)
{
    TRect health_bar_rect;
    health_bar_rect.left = 2;
    health_bar_rect.right = length + 2;
    health_bar_rect.top = 7 * color + 37;
    health_bar_rect.bottom = 7 * color + 40;
    return health_bar_rect;
}

// The fill of one HUD status bar for a number of segments; the later bars sit a little further right on the sheet.
TRect InterfaceGraphics::GetStatusBarFillRect(int bar, int segments)
{
    TRect status_bar_fill_rect;
    int gap = 0;
    if (bar > 1)
        gap = 2;
    status_bar_fill_rect.left = 109 * bar + gap + 24;
    status_bar_fill_rect.right = 109 * bar + gap + 4 * segments + 24;
    status_bar_fill_rect.top = 18;
    status_bar_fill_rect.bottom = 23;
    return status_bar_fill_rect;
}

// One icon of the list icon sheet, by row and column.
TRect InterfaceGraphics::GetListIconRect(int row, int column)
{
    TRect list_icon_rect;
    list_icon_rect.left = 15 * column;
    list_icon_rect.top = 15 * row;
    list_icon_rect.right = 15 * column + 15;
    list_icon_rect.bottom = 15 * row + 15;
    return list_icon_rect;
}

// One side panel tab button, plain or lit.
TRect InterfaceGraphics::GetTabButtonRect(int tab, int state)
{
    TRect tab_button_rect;
    tab_button_rect.left = 36 * state;
    tab_button_rect.top = 19 * tab;
    tab_button_rect.right = 36 * state + 35;
    tab_button_rect.bottom = 19 * tab + 18;
    return tab_button_rect;
}

// The settings row marker, plain or lit.
TRect InterfaceGraphics::GetSettingMarkerRect(int state)
{
    TRect setting_marker_rect;
    setting_marker_rect.left = 19 * state;
    setting_marker_rect.top = 0;
    setting_marker_rect.right = 19 * state + 19;
    setting_marker_rect.bottom = 15;
    return setting_marker_rect;
}

// The stat raise button of the spell and skill panels, in either blink state.
TRect InterfaceGraphics::GetRaiseButtonRect(int state)
{
    TRect raise_button_rect;
    raise_button_rect.left = 19 * state + 215;
    raise_button_rect.top = 386;
    raise_button_rect.right = 19 * state + 234;
    raise_button_rect.bottom = 401;
    return raise_button_rect;
}

// One icon of the interface scroll icon strip.
DEFINE_GET_SCROLL_ICON_RECT(InterfaceGraphics)

// The chat mode icon for one mode.
TRect InterfaceGraphics::GetChatModeIconRect(int mode)
{
    TRect chat_mode_icon_rect;
    chat_mode_icon_rect.left = 0;
    chat_mode_icon_rect.top = 21 * mode;
    chat_mode_icon_rect.right = 83;
    chat_mode_icon_rect.bottom = 21 * mode + 21;
    return chat_mode_icon_rect;
}

// One small line icon of the chat and list panels.
TRect InterfaceGraphics::GetSmallIconRect(int icon)
{
    TRect small_icon_rect;
    small_icon_rect.left = 0;
    small_icon_rect.top = 13 * icon;
    small_icon_rect.right = 13;
    small_icon_rect.bottom = 13 * icon + 13;
    return small_icon_rect;
}

// The interface corner button, plain or lit.
DEFINE_GET_CORNER_BUTTON_RECT(InterfaceGraphics)

// One minimap tile edge piece.
TRect InterfaceGraphics::GetMinimapTileRect(int edge)
{
    TRect minimap_edge_rect;
    minimap_edge_rect.left = 27 * edge;
    minimap_edge_rect.right = 27 * edge + 27;
    minimap_edge_rect.top = 0;
    minimap_edge_rect.bottom = 14;
    return minimap_edge_rect;
}

// One weapon effect frame, by effect row and direction column.
TRect InterfaceGraphics::GetCellRect(int effect, int direction)
{
    TRect weapon_effect_rect;
    weapon_effect_rect.left = 58 * direction;
    weapon_effect_rect.right = 58 * direction + 58;
    weapon_effect_rect.top = 58 * effect;
    weapon_effect_rect.bottom = 58 * effect + 58;
    return weapon_effect_rect;
}

// The player list filter icon for one mode, two to a row.
TRect InterfaceGraphics::GetListModeIconRect(int mode)
{
    TRect list_mode_icon_rect;
    int row = 0;
    if (mode > 1)
    {
        row = 1;
        mode = mode - 2;
    }
    list_mode_icon_rect.left = 12 * mode;
    list_mode_icon_rect.right = 12 * mode + 12;
    list_mode_icon_rect.top = 13 * row + 233;
    list_mode_icon_rect.bottom = 13 * row + 245;
    return list_mode_icon_rect;
}

TRect InterfaceGraphics::GetBackRect(int width, int height)
{
    TRect rect;
    rect.left = 0;
    rect.right = width;
    rect.top = 0;
    rect.bottom = height;
    return rect;
}

// One status icon, large from the first row or small from the second.
TRect InterfaceGraphics::GetStatusIconRect(int icon, bool small_icon)
{
    TRect status_icon_rect;
    if (!small_icon)
    {
        status_icon_rect.left = 20 * icon;
        status_icon_rect.right = 20 * icon + 20;
        status_icon_rect.top = 0;
        status_icon_rect.bottom = 20;
    }
    else
    {
        status_icon_rect.left = 20 * icon + 1;
        status_icon_rect.right = 20 * icon + 19;
        status_icon_rect.top = 21;
        status_icon_rect.bottom = 39;
    }
    return status_icon_rect;
}

// One frame of the tile cursor.
TRect InterfaceGraphics::GetWideCellRect(int frame)
{
    TRect tile_cursor_rect;
    tile_cursor_rect.left = 64 * frame;
    tile_cursor_rect.right = 64 * frame + 64;
    tile_cursor_rect.top = 0;
    tile_cursor_rect.bottom = 32;
    return tile_cursor_rect;
}

// One group member state icon.
TRect InterfaceGraphics::GetGroupIconRect(int icon)
{
    TRect group_icon_rect;
    group_icon_rect.left = 0;
    group_icon_rect.right = 56;
    group_icon_rect.top = 10 * icon;
    group_icon_rect.bottom = 10 * icon + 10;
    return group_icon_rect;
}

// The part of the progress bar a spell or skill slot shows for its progress out of a hundred.
TRect InterfaceGraphics::GetSpellProgressRect(int progress)
{
    TRect spell_progress_rect;
    double width = progress * HEALTH_BAR_SCALE;
    spell_progress_rect.left = 0;
    spell_progress_rect.right = width;
    spell_progress_rect.top = 405;
    spell_progress_rect.bottom = 411;
    return spell_progress_rect;
}

// One inventory slot highlight, by highlight state.
TRect InterfaceGraphics::GetSlotHighlightRect(int slot)
{
    TRect slot_highlight_rect;
    slot_highlight_rect.left = 0;
    slot_highlight_rect.right = 23;
    slot_highlight_rect.top = 23 * slot + 163;
    slot_highlight_rect.bottom = 23 * slot + 186;
    return slot_highlight_rect;
}

// The number label of one spell slot.
TRect InterfaceGraphics::GetSpellSlotLabelRect(int slot)
{
    TRect spell_slot_label_rect;
    spell_slot_label_rect.left = 18 * slot + 148;
    spell_slot_label_rect.right = 18 * slot + 166;
    spell_slot_label_rect.top = 51;
    spell_slot_label_rect.bottom = 64;
    return spell_slot_label_rect;
}

// The spell slot hover frame, by frame state.
TRect InterfaceGraphics::GetSpellSlotFrameRect(int slot)
{
    TRect spell_slot_frame_rect;
    spell_slot_frame_rect.left = SLOT_FRAME_PITCH * slot;
    spell_slot_frame_rect.right = SLOT_FRAME_PITCH * slot + 42;
    spell_slot_frame_rect.top = 442;
    spell_slot_frame_rect.bottom = 478;
    return spell_slot_frame_rect;
}

// The skill slot hover frame, by frame state.
TRect InterfaceGraphics::GetSkillSlotFrameRect(int slot)
{
    TRect skill_slot_frame_rect;
    skill_slot_frame_rect.left = SLOT_FRAME_PITCH * slot;
    skill_slot_frame_rect.right = SLOT_FRAME_PITCH * slot + 42;
    skill_slot_frame_rect.top = 442;
    skill_slot_frame_rect.bottom = 478;
    return skill_slot_frame_rect;
}

// One inventory arrow button, plain or lit.
TRect InterfaceGraphics::GetInventoryButtonRect(int button, int state)
{
    TRect inventory_button_rect;
    inventory_button_rect.left = 0;
    inventory_button_rect.right = 38;
    inventory_button_rect.top = 74 * button + 37 * state + 15;
    inventory_button_rect.bottom = 74 * button + 37 * state + 52;
    return inventory_button_rect;
}

// One small tab button of the player list (friends or ignore), plain or lit.
TRect InterfaceGraphics::GetSmallTabButtonRect(int state, int tab)
{
    TRect small_tab_button_rect;
    small_tab_button_rect.left = 17 * tab;
    small_tab_button_rect.right = 17 * tab + 17;
    small_tab_button_rect.top = 16 * state + 259;
    small_tab_button_rect.bottom = 16 * state + 275;
    return small_tab_button_rect;
}

// The inventory paperdoll button, plain or lit.
TRect InterfaceGraphics::GetInventoryWideButtonRect(ButtonState state)
{
    TRect inventory_wide_button_rect;
    inventory_wide_button_rect.left = 88 * state + 39;
    inventory_wide_button_rect.top = 385;
    inventory_wide_button_rect.right = 88 * state + 126;
    inventory_wide_button_rect.bottom = 405;
    return inventory_wide_button_rect;
}

InterfaceGraphics::~InterfaceGraphics()
{
    hud_background->Destroy();
    clock_background->Destroy();
    tooltip_background->Destroy();
    chat_panel_surface->Destroy();
    chat_input_background->Destroy();
    chat_mode_icons->Destroy();
    corner_button->Destroy();
    chat_tabs->Destroy();
    small_icons->Destroy();
    tab_buttons->Destroy();
    group_icons->Destroy();
    minimap_tiles->Destroy();
    status_icons->Destroy();
    hud_sheet->Destroy();
    delete hud_background;
    delete clock_background;
    delete tooltip_background;
    delete chat_panel_surface;
    delete chat_input_background;
    delete chat_mode_icons;
    delete corner_button;
    delete chat_tabs;
    delete small_icons;
    delete tab_buttons;
    delete group_icons;
    delete minimap_tiles;
    delete status_icons;
    delete hud_sheet;
    hud_frame->Destroy();
    hud_left_frame->Destroy();
    hud_top_frame->Destroy();
    tile_cursor->Destroy();
    panel_background->Destroy();
    loading_icon->Destroy();
    emote_sheet->Destroy();
    weapon_effects->Destroy();
    control_sheet->Destroy();
    scroll_icons->Destroy();
    list_icons->Destroy();
    delete hud_frame;
    delete hud_left_frame;
    delete hud_top_frame;
    delete tile_cursor;
    delete panel_background;
    delete loading_icon;
    delete emote_sheet;
    delete weapon_effects;
    delete control_sheet;
    delete scroll_icons;
    delete list_icons;
    surface_desc->Clear();
    surface_caps->Clear();
    color_key->Clear();
    delete surface_desc;
    delete surface_caps;
    delete color_key;
}

// Saves the game view as the first free numbered bitmap in the screen folder, then waits out a cooldown.
void InterfaceGraphics::SaveScreenshot()
{
    if (MainForm->config->GetScreenFolderExists() && MainForm->animation->GetScreenMode() == ScreenMode_Game &&
        !MainForm->animation->interface_controller->IsScreenshotCooldown())
    {
        try
        {
            String screen_folder = MainForm->config->GetAppPath() + "/screen/";
            String relative_folder = ".\\screen\\";
            String lp_file_name;
            // Numbers are tried in order; with every name taken nothing is saved.
            for (int file_number = 1; file_number <= 999; file_number++)
            {
                String number_text;
                if (file_number >= 0 && file_number <= 9)
                    number_text = "00" + IntToStr(file_number);
                if (file_number >= 10 && file_number <= 99)
                    number_text = "0" + IntToStr(file_number);
                if (file_number >= 100 && file_number <= 999)
                    number_text = IntToStr(file_number);
                lp_file_name = relative_folder + "ENDL" + number_text + ".BMP";
                if (access(lp_file_name.c_str(), 0))
                {
                    MainForm->OffscreenSurface->SaveToFile(MainForm->Dx_Draw1, lp_file_name);
                    MainForm->sfx->PlaySfx(SfxSound_ScreenCapture);
                    break;
                }
            }
        }
        catch (...)
        {
        }
        MainForm->animation->interface_controller->StartScreenshotCooldown();
    }
}

// Draws the interface emote sheet bitmap onto its surface.
void InterfaceGraphics::LoadEmoteSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (emote_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        emote_sheet->ReleaseDC(dc);
    }
}

// Draws the interface weapon effects bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadWeaponEffects(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (weapon_effects->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        weapon_effects->ReleaseDC(dc);
    }
    weapon_effects->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws a panel background bitmap onto the shared panel surface; every side panel loads into it.
void InterfaceGraphics::LoadPanelBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->interface_bitmap->Width = 0;
    MainForm->interface_bitmap->Height = 0;
    MainForm->interface_bitmap->Transparent = false;
    MainForm->interface_bitmap->Width = 0;
    MainForm->interface_bitmap->Height = 0;
    MainForm->interface_bitmap->Transparent = false;
    MainForm->interface_bitmap->LoadFromResourceID(library, resource_id);
    if (panel_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->interface_bitmap);
        }
        catch (Exception &)
        {
        }
        panel_background->ReleaseDC(dc);
    }
}

// Draws the map loading box bitmap onto the loading icon surface.
void InterfaceGraphics::LoadMapLoadingBox(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (loading_icon->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        loading_icon->ReleaseDC(dc);
    }
}

// Draws the interface clock background bitmap onto its surface.
void InterfaceGraphics::LoadClockBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (clock_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        clock_background->ReleaseDC(dc);
    }
}

// Draws the interface group icons bitmap onto its surface.
void InterfaceGraphics::LoadGroupIcons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (group_icons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        group_icons->ReleaseDC(dc);
    }
}

// Draws the interface tooltip background bitmap onto its surface.
void InterfaceGraphics::LoadTooltipBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (tooltip_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        tooltip_background->ReleaseDC(dc);
    }
}

// Fills the chat panel's surface with its dark background.
void InterfaceGraphics::ClearChatPanel()
{
    HDC dc;
    TRect rect;
    if (chat_panel_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Color = (TColor)0xC0C0C;
            rect.left = 0;
            rect.top = 0;
            rect.right = 442;
            rect.bottom = 99;
            MainForm->draw_canvas->FillRect(rect);
        }
        catch (Exception &)
        {
        }
        chat_panel_surface->ReleaseDC(dc);
    }
}

// Draws the interface chat input background bitmap onto its surface.
void InterfaceGraphics::LoadChatInputBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (chat_input_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        chat_input_background->ReleaseDC(dc);
    }
}

// Draws the interface small icons bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadSmallIcons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (small_icons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        small_icons->ReleaseDC(dc);
    }
    small_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface HUD sheet bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadHudSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (hud_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        hud_sheet->ReleaseDC(dc);
    }
    hud_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface status icons bitmap onto its surface.
void InterfaceGraphics::LoadStatusIcons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (status_icons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        status_icons->ReleaseDC(dc);
    }
}

// Draws the interface chat tabs bitmap onto its surface.
void InterfaceGraphics::LoadChatTabs(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (chat_tabs->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        chat_tabs->ReleaseDC(dc);
    }
}

// Draws the interface list icons bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadListIcons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (list_icons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        list_icons->ReleaseDC(dc);
    }
    list_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface corner button bitmap onto its surface, with black see-through.
DEFINE_LOAD_CORNER_BUTTON(InterfaceGraphics)

// Draws the interface minimap tiles bitmap onto its surface.
void InterfaceGraphics::LoadMinimapTiles(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (minimap_tiles->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        minimap_tiles->ReleaseDC(dc);
    }
}

// Draws the interface chat mode icons bitmap onto its surface.
void InterfaceGraphics::LoadChatModeIcons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (chat_mode_icons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        chat_mode_icons->ReleaseDC(dc);
    }
}

// Draws the interface HUD background bitmap onto its surface.
void InterfaceGraphics::LoadHudBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (hud_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        hud_background->ReleaseDC(dc);
    }
}

// Draws the interface tab buttons bitmap onto its surface.
void InterfaceGraphics::LoadTabButtons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (tab_buttons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        tab_buttons->ReleaseDC(dc);
    }
}

// Draws the interface HUD frame bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadHudFrame(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (hud_frame->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        hud_frame->ReleaseDC(dc);
    }
    hud_frame->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface HUD left frame bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadHudLeftFrame(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (hud_left_frame->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        hud_left_frame->ReleaseDC(dc);
    }
    hud_left_frame->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface HUD top frame bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadHudTopFrame(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (hud_top_frame->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        hud_top_frame->ReleaseDC(dc);
    }
    hud_top_frame->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface tile cursor bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadTileCursor(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (tile_cursor->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        tile_cursor->ReleaseDC(dc);
    }
    tile_cursor->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface control sheet bitmap onto its surface.
void InterfaceGraphics::LoadControlSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (control_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        control_sheet->ReleaseDC(dc);
    }
}

// Draws the interface scroll icons bitmap onto its surface.
DEFINE_LOAD_SCROLL_ICONS(InterfaceGraphics)

// One frame of the emote sheet.
TRect InterfaceGraphics::GetWideIconRect(int frame)
{
    TRect emote_frame_rect;
    emote_frame_rect.left = 50 * frame;
    emote_frame_rect.top = 0;
    emote_frame_rect.right = 50 * frame + 50;
    emote_frame_rect.bottom = 50;
    return emote_frame_rect;
}

// Draws the icon for the chat mode the chat input's first character picks; a locked chat shows its own icon.
void InterfaceGraphics::DrawChatModeIcon()
{
    int mode;
    String input_text = "";
    try
    {
        input_text = MainForm->edits->GetText(EditId_Chat);
    }
    catch (...)
    {
    }
    if (input_text.Length() <= 0)
        mode = 0;
    else if (input_text[1] == '!')
        mode = 2;
    else
    {
        bool prefix_matched = false;
        if (input_text[1] == '"' || String(input_text[1]) == "'")
        {
            mode = 4;
            prefix_matched = true;
        }
        // Admin prefixes count only for players with a high enough admin level.
        if (!prefix_matched && input_text[1] == '@' && MainForm->net_client->GetAdminLevel() > 1)
        {
            mode = 3;
            prefix_matched = true;
        }
        if (!prefix_matched && input_text[1] == '+' && MainForm->net_client->GetAdminLevel() > 0)
        {
            mode = 5;
            prefix_matched = true;
        }
        if (!prefix_matched && input_text[1] == '&' && MainForm->animation->interface_controller->status->IsInGuild())
        {
            mode = 7;
            prefix_matched = true;
        }
        if (!prefix_matched && input_text[1] == '~')
        {
            // A global message on the restricted map, or while global messages are not allowed, clears the input.
            if (MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_YouCannotUseGlobal));
                MainForm->edits->SetText(EditId_Chat, "");
                mode = 0;
                prefix_matched = true;
            }
            else if (MainForm->animation->IsGlobalMessageAllowed())
            {
                mode = 3;
                prefix_matched = true;
            }
            else
            {
                MainForm->edits->SetText(EditId_Chat, "");
                mode = 0;
                prefix_matched = true;
            }
        }
        if (!prefix_matched)
            mode = 1;
    }
    if (MainForm->edits->chat_locked)
    {
        MainForm->OffscreenSurface->BltFast(16, 309, chat_mode_icons, &GetChatModeIconRect(6), DDBLTFAST_WAIT);
        last_chat_mode = 10;
    }
    else
    {
        // The icon is drawn again only when the mode changes.
        if (last_chat_mode != mode)
            MainForm->OffscreenSurface->BltFast(16, 309, chat_mode_icons, &GetChatModeIconRect(mode),
                DDBLTFAST_NOCOLORKEY);
        last_chat_mode = mode;
    }
}

// Draws the interface corner button, lit under the mouse.
void InterfaceGraphics::DrawCornerButton()
{
    if (MainForm->animation->interface_controller->corner_button_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->interface_graphics->corner_button,
            &MainForm->gfx->interface_graphics->GetCornerButtonRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->interface_graphics->corner_button,
            &MainForm->gfx->interface_graphics->GetCornerButtonRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the player list scroll arrows, lit under the mouse.
void InterfaceGraphics::DrawPlayerListScrollButtons()
{
    if (MainForm->animation->interface_controller->player_list->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->player_list->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the spell panel scroll arrows, lit under the mouse.
void InterfaceGraphics::DrawSpellScrollButtons()
{
    if (MainForm->animation->interface_controller->spells->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(569, 332, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 332, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->spells->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the skill panel scroll arrows, lit under the mouse.
void InterfaceGraphics::DrawSkillScrollButtons()
{
    if (MainForm->animation->interface_controller->skills->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(569, 332, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 332, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->skills->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the chat panel scroll arrows, lit under the mouse.
void InterfaceGraphics::DrawChatScrollButtons()
{
    if (MainForm->animation->interface_controller->chat->scroll_up_hover)
        MainForm->OffscreenSurface->BltFast(569, 332, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 332, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->chat->scroll_down_hover)
        MainForm->OffscreenSurface->BltFast(569, 414, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 414, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the side panel tab buttons on both sides and the friends and ignore buttons, each lit under the mouse.
void InterfaceGraphics::DrawTabButtons()
{
    if (MainForm->animation->interface_controller->inventory_button_hover)
        MainForm->OffscreenSurface->BltFast(62, 330, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Inventory, ButtonState_Hover),
            DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(62, 330, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Inventory, ButtonState_Normal),
            DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->minimap_button_hover)
        MainForm->OffscreenSurface->BltFast(62, 350, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Minimap, ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(62, 350, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Minimap, ButtonState_Normal),
            DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->spells_button_hover)
        MainForm->OffscreenSurface->BltFast(62, 370, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Spells, ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(62, 370, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Spells, ButtonState_Normal), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->skills_button_hover)
        MainForm->OffscreenSurface->BltFast(62, 390, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Skills, ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(62, 390, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Skills, ButtonState_Normal), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->chat_button_hover)
        MainForm->OffscreenSurface->BltFast(62, 410, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Chat, ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(62, 410, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Chat, ButtonState_Normal), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->status_button_hover)
        MainForm->OffscreenSurface->BltFast(62, 430, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Status, ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(62, 430, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Status, ButtonState_Normal), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->players_button_hover)
        MainForm->OffscreenSurface->BltFast(590, 350, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Players, ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(590, 350, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Players, ButtonState_Normal),
            DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->group_button_hover)
        MainForm->OffscreenSurface->BltFast(590, 370, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Group, ButtonState_Hover),
                DDBLTFAST_NOCOLORKEY);
    else
        MainForm->OffscreenSurface->BltFast(590, 370, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Group, ButtonState_Normal),
                DDBLTFAST_NOCOLORKEY);
    if (MainForm->animation->interface_controller->third_right_button_hover)
        MainForm->OffscreenSurface->BltFast(590, 390, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Unused, ButtonState_Hover),
                DDBLTFAST_NOCOLORKEY);
    else
        MainForm->OffscreenSurface->BltFast(590, 390, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Unused, ButtonState_Normal),
                DDBLTFAST_NOCOLORKEY);
    if (MainForm->animation->interface_controller->settings_button_hover)
        MainForm->OffscreenSurface->BltFast(590, 410, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Settings, ButtonState_Hover),
                DDBLTFAST_NOCOLORKEY);
    else
        MainForm->OffscreenSurface->BltFast(590, 410, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Settings, ButtonState_Normal),
                DDBLTFAST_NOCOLORKEY);
    if (MainForm->animation->interface_controller->help_button_hover)
        MainForm->OffscreenSurface->BltFast(590, 430, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Help, ButtonState_Hover),
                DDBLTFAST_NOCOLORKEY);
    else
        MainForm->OffscreenSurface->BltFast(590, 430, MainForm->gfx->interface_graphics->tab_buttons,
            &MainForm->gfx->interface_graphics->GetTabButtonRect(HudButton_Help, ButtonState_Normal),
                DDBLTFAST_NOCOLORKEY);
    if (MainForm->animation->interface_controller->friends_button_hover)
        MainForm->OffscreenSurface->BltFast(592, 311, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetSmallTabButtonRect(ButtonState_Hover, SmallTabButton_Friends),
                DDBLTFAST_NOCOLORKEY);
    else
        MainForm->OffscreenSurface->BltFast(592, 311, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetSmallTabButtonRect(ButtonState_Normal, SmallTabButton_Friends),
                DDBLTFAST_NOCOLORKEY);
    if (MainForm->animation->interface_controller->ignore_button_hover)
        MainForm->OffscreenSurface->BltFast(609, 311, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetSmallTabButtonRect(ButtonState_Hover, SmallTabButton_Ignore),
                DDBLTFAST_NOCOLORKEY);
    else
        MainForm->OffscreenSurface->BltFast(609, 311, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetSmallTabButtonRect(ButtonState_Normal, SmallTabButton_Ignore),
                DDBLTFAST_NOCOLORKEY);
}

// Draws the player list's scroll thumb, filter icon and an icon per visible player by the player's icon type.
void InterfaceGraphics::DrawPlayerList()
{
    int i;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->player_list->GetScrollThumbOffset() + 366,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(106, 332, control_sheet,
        &GetListModeIconRect(MainForm->animation->interface_controller->player_list->GetFilter()), DDBLTFAST_WAIT);
    int top_line = MainForm->animation->interface_controller->player_list->GetTopLine();
    int line_count = MainForm->animation->interface_controller->player_list->shown_names->Count;
    int row = 0;
    for (i = top_line; i < line_count && i < top_line + 7; i++)
    {
        char icon_drawn = false;
        if (MainForm->animation->interface_controller->player_list->shown_icon_types->Strings[i] == "4")
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(12),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            icon_drawn = true;
        }
        if (MainForm->animation->interface_controller->player_list->shown_icon_types->Strings[i] == "9")
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(13),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            icon_drawn = true;
        }
        if (MainForm->animation->interface_controller->player_list->shown_icon_types->Strings[i] == "5")
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(14),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            icon_drawn = true;
        }
        if (MainForm->animation->interface_controller->player_list->shown_icon_types->Strings[i] == "10")
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(15),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            icon_drawn = true;
        }
        if (MainForm->animation->interface_controller->player_list->shown_icon_types->Strings[i] == "6")
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(10),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            icon_drawn = true;
        }
        if (!icon_drawn)
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(9),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        row++;
    }
}

// Draws the player count, and for each of the seven visible rows a player's name, second column, guild tag and class.
void InterfaceGraphics::DrawPlayerListText()
{
    int row;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int count_width =
        MainForm->text_canvas->TextWidth(IntToStr(MainForm->animation->interface_controller->player_list->shown_names->Count));
    MainForm->text_canvas->TextOut(580 - count_width, 332,
        IntToStr(MainForm->animation->interface_controller->player_list->shown_names->Count));
    int top_line = MainForm->animation->interface_controller->player_list->GetTopLine();
    int line_count = MainForm->animation->interface_controller->player_list->shown_names->Count;
    row = 0;
    MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
    for (int i = top_line; i < line_count && i < top_line + 7; i++)
    {
        MainForm->text_canvas->TextOut(124, 14 * row + 350,
            MainForm->animation->interface_controller->player_list->shown_names->Strings[i]);
        MainForm->text_canvas->TextOut(236, 14 * row + 350,
            MainForm->animation->interface_controller->player_list->shown_second_texts->Strings[i]);
        MainForm->text_canvas->TextOut(348, 14 * row + 350,
            MainForm->animation->interface_controller->player_list->shown_guild_tags->Strings[i]);
        MainForm->text_canvas->TextOut(460, 14 * row + 350,
            MainForm->animation->interface_controller->player_list->shown_class_names->Strings[i]);
        row++;
    }
}

// Draws the chat panel's scroll thumb, the line icons of the open tab and the tab strip with the open tab raised.
void InterfaceGraphics::DrawChatPanel()
{
    int first_private_top_line;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->chat->GetScrollbarPosition() + 348,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(122, 433, MainForm->gfx->interface_graphics->chat_tabs,
        &MainForm->gfx->interface_graphics->GetChatTabRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->chat->active_tab == 1)
    {
        first_private_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int first_private_line_count = MainForm->animation->interface_controller->chat->private_tab_lines[0]->Count;
        int first_private_row = 0;
        for (int i = first_private_top_line; i < first_private_line_count && first_private_top_line + 7 > i; i++)
        {
            if (!StrToInt(MainForm->animation->interface_controller->chat->private_indents[0]->Strings[i]))
                MainForm->OffscreenSurface->BltFast(108, 14 * first_private_row + 332, small_icons,
                    &GetSmallIconRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            first_private_row++;
        }
        if (MainForm->animation->interface_controller->chat->first_private_name != "")
            MainForm->OffscreenSurface->BltFast(125, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(2), DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->chat->second_private_name != "")
            MainForm->OffscreenSurface->BltFast(258, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(391, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(435, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(479, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(523, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 2)
    {
        int second_private_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int second_private_line_count =
            MainForm->animation->interface_controller->chat->last_private_tab_lines[0]->Count;
        int second_private_row = 0;
        for (int j = second_private_top_line; j < second_private_line_count && second_private_top_line + 7 > j; j++)
        {
            if (!StrToInt(MainForm->animation->interface_controller->chat->private_indents[1]->Strings[j]))
                MainForm->OffscreenSurface->BltFast(108, 14 * second_private_row + 332, small_icons,
                    &GetSmallIconRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            second_private_row++;
        }
        if (MainForm->animation->interface_controller->chat->first_private_name != "")
            MainForm->OffscreenSurface->BltFast(125, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->chat->second_private_name != "")
            MainForm->OffscreenSurface->BltFast(258, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(2), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(391, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(435, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(479, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(523, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 3)
    {
        int local_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int local_line_count = MainForm->animation->interface_controller->chat->local_lines->Count;
        int local_row = 0;
        for (int k = local_top_line; k < local_line_count && local_top_line + 7 > k; k++)
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * local_row + 332, small_icons,
                &GetSmallIconRect(StrToInt(MainForm->animation->interface_controller->chat->local_icons->Strings[k])),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            local_row++;
        }
        if (MainForm->animation->interface_controller->chat->first_private_name != "")
            MainForm->OffscreenSurface->BltFast(125, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->chat->second_private_name != "")
            MainForm->OffscreenSurface->BltFast(258, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(391, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(4), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(435, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(479, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(523, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 4)
    {
        int global_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int global_line_count = MainForm->animation->interface_controller->chat->global_lines->Count;
        int global_row = 0;
        for (int m = global_top_line; m < global_line_count && global_top_line + 7 > m; m++)
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * global_row + 332, small_icons,
                &GetSmallIconRect(StrToInt(MainForm->animation->interface_controller->chat->global_icons->Strings[m])),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            global_row++;
        }
        if (MainForm->animation->interface_controller->chat->first_private_name != "")
            MainForm->OffscreenSurface->BltFast(125, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->chat->second_private_name != "")
            MainForm->OffscreenSurface->BltFast(258, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(391, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(435, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(4), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(479, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(523, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 5)
    {
        int group_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int group_line_count = MainForm->animation->interface_controller->chat->group_lines->Count;
        int group_row = 0;
        for (int n = group_top_line; n < group_line_count && group_top_line + 7 > n; n++)
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * group_row + 332, small_icons,
                &GetSmallIconRect(StrToInt(MainForm->animation->interface_controller->chat->group_icons->Strings[n])),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            group_row++;
        }
        if (MainForm->animation->interface_controller->chat->first_private_name != "")
            MainForm->OffscreenSurface->BltFast(125, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->chat->second_private_name != "")
            MainForm->OffscreenSurface->BltFast(258, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(391, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(435, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(479, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(4), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(523, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 6)
    {
        int system_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int system_line_count = MainForm->animation->interface_controller->chat->system_lines->Count;
        int system_row = 0;
        for (int ii = system_top_line; ii < system_line_count && system_top_line + 7 > ii; ii++)
        {
            MainForm->OffscreenSurface->BltFast(108, 14 * system_row + 332, small_icons,
                &GetSmallIconRect(StrToInt(MainForm->animation->interface_controller->chat->system_icons->Strings[ii])),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            system_row++;
        }
        if (MainForm->animation->interface_controller->chat->first_private_name != "")
            MainForm->OffscreenSurface->BltFast(125, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->chat->second_private_name != "")
            MainForm->OffscreenSurface->BltFast(258, 433, MainForm->gfx->interface_graphics->chat_tabs,
                &MainForm->gfx->interface_graphics->GetChatTabRect(1), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(391, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(435, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(479, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(3), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(523, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(4), DDBLTFAST_WAIT);
    }
    // The last tab piece shows whether the chat scrolls to new lines.
    if (MainForm->animation->interface_controller->chat->auto_scroll)
        MainForm->OffscreenSurface->BltFast(568, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(5), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(568, 433, MainForm->gfx->interface_graphics->chat_tabs,
            &MainForm->gfx->interface_graphics->GetChatTabRect(6), DDBLTFAST_WAIT);
}

// Draws the chat tabs, lit when active or unread, and the seven visible lines of the open tab with each sender's
// name and the colour codes in the text.
void InterfaceGraphics::DrawChatText()
{
    int private_top_line;
    int private_line_count;
    int private_row;
    int i;
    int local_top_line;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    if (MainForm->animation->interface_controller->chat->active_tab == 1)
        MainForm->text_canvas->Font->Color = clWhite;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[0])
        MainForm->text_canvas->Font->Color = (TColor)0xAAC8DC;
    else
        MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->TextOut(141, 434,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->chat->first_private_name));
    if (MainForm->animation->interface_controller->chat->active_tab == 2)
        MainForm->text_canvas->Font->Color = clWhite;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[1])
        MainForm->text_canvas->Font->Color = (TColor)0xAAC8DC;
    else
        MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->TextOut(273, 434,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->chat->second_private_name));
    if (MainForm->animation->interface_controller->chat->active_tab == 3)
        MainForm->text_canvas->Font->Color = clWhite;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[2])
        MainForm->text_canvas->Font->Color = (TColor)0xAAC8DC;
    else
        MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->TextOut(406, 434, "scr");
    if (MainForm->animation->interface_controller->chat->active_tab == 4)
        MainForm->text_canvas->Font->Color = clWhite;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[3])
        MainForm->text_canvas->Font->Color = (TColor)0xAAC8DC;
    else
        MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->TextOut(450, 434, "glb");
    if (MainForm->animation->interface_controller->chat->active_tab == 5)
        MainForm->text_canvas->Font->Color = clWhite;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[4])
        MainForm->text_canvas->Font->Color = (TColor)0xAAC8DC;
    else
        MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->TextOut(494, 434, "grp");
    if (MainForm->animation->interface_controller->chat->active_tab == 6)
        MainForm->text_canvas->Font->Color = clWhite;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[5])
        MainForm->text_canvas->Font->Color = (TColor)0xAAC8DC;
    else
        MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->TextOut(538, 434, "sys");
    // Tabs 1 and 2 are the private ones; each tab draws its own lines from its scroll offset.
    if (MainForm->animation->interface_controller->chat->active_tab <= 2)
    {
        private_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        private_line_count =
            MainForm->animation->interface_controller->chat->private_lines[MainForm->animation->interface_controller->chat->active_tab]->Count;
        private_row = 0;
        i = private_top_line;
        while (i < private_line_count && private_top_line + 7 > i)
        {
            if (MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i] ==
                "0")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
            if (MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i] ==
                "1")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 1);
            if (MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i] ==
                "2")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 2);
            if (MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i] ==
                "3")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 3);
            if (MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i] ==
                "4")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 4);
            if (MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i] ==
                "5")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 5);
            MainForm->text_canvas->TextOut(StrToInt(MainForm->animation->interface_controller->chat->last_private_colors[MainForm->animation->interface_controller->chat->active_tab]->Strings[i]) +
                124, 14 * private_row + 332,
                MainForm->animation->interface_controller->chat->private_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[i]);
            private_row++;
            i++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 3)
    {
        local_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int local_line_count = MainForm->animation->interface_controller->chat->local_lines->Count;
        int local_row = 0;
        int j = local_top_line;
        while (j < local_line_count && local_top_line + 7 > j)
        {
            if (MainForm->animation->interface_controller->chat->local_colors->Strings[j] == "0")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
            if (MainForm->animation->interface_controller->chat->local_colors->Strings[j] == "1")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 1);
            if (MainForm->animation->interface_controller->chat->local_colors->Strings[j] == "2")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 2);
            if (MainForm->animation->interface_controller->chat->local_colors->Strings[j] == "3")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 3);
            if (MainForm->animation->interface_controller->chat->local_colors->Strings[j] == "4")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 4);
            if (MainForm->animation->interface_controller->chat->local_colors->Strings[j] == "5")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 5);
            MainForm->text_canvas->TextOut(StrToInt(MainForm->animation->interface_controller->chat->local_indents->Strings[j]) +
                124, 14 * local_row + 332, MainForm->animation->interface_controller->chat->local_lines->Strings[j]);
            local_row++;
            j++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 4)
    {
        int global_top_line;
        global_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int global_line_count = MainForm->animation->interface_controller->chat->global_lines->Count;
        int global_row = 0;
        int k = global_top_line;
        while (k < global_line_count && global_top_line + 7 > k)
        {
            if (MainForm->animation->interface_controller->chat->global_colors->Strings[k] == "0")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
            if (MainForm->animation->interface_controller->chat->global_colors->Strings[k] == "1")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 1);
            if (MainForm->animation->interface_controller->chat->global_colors->Strings[k] == "2")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 2);
            if (MainForm->animation->interface_controller->chat->global_colors->Strings[k] == "3")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 3);
            if (MainForm->animation->interface_controller->chat->global_colors->Strings[k] == "4")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 4);
            if (MainForm->animation->interface_controller->chat->global_colors->Strings[k] == "5")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 5);
            MainForm->text_canvas->TextOut(StrToInt(MainForm->animation->interface_controller->chat->global_indents->Strings[k]) +
                124, 14 * global_row + 332, MainForm->animation->interface_controller->chat->global_lines->Strings[k]);
            global_row++;
            k++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 5)
    {
        int group_top_line;
        group_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int group_line_count = MainForm->animation->interface_controller->chat->group_lines->Count;
        int group_row = 0;
        int m = group_top_line;
        while (m < group_line_count && group_top_line + 7 > m)
        {
            if (MainForm->animation->interface_controller->chat->group_colors->Strings[m] == "0")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
            if (MainForm->animation->interface_controller->chat->group_colors->Strings[m] == "1")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 1);
            if (MainForm->animation->interface_controller->chat->group_colors->Strings[m] == "2")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 2);
            if (MainForm->animation->interface_controller->chat->group_colors->Strings[m] == "3")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 3);
            if (MainForm->animation->interface_controller->chat->group_colors->Strings[m] == "4")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 4);
            if (MainForm->animation->interface_controller->chat->group_colors->Strings[m] == "5")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 5);
            MainForm->text_canvas->TextOut(StrToInt(MainForm->animation->interface_controller->chat->private_lines[0]->Strings[m]) +
                124, 14 * group_row + 332, MainForm->animation->interface_controller->chat->group_lines->Strings[m]);
            group_row++;
            m++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 6)
    {
        int system_top_line;
        system_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int system_line_count = MainForm->animation->interface_controller->chat->system_lines->Count;
        int system_row = 0;
        int n = system_top_line;
        while (n < system_line_count && system_top_line + 7 > n)
        {
            if (MainForm->animation->interface_controller->chat->system_colors->Strings[n] == "0")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
            if (MainForm->animation->interface_controller->chat->system_colors->Strings[n] == "1")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 1);
            if (MainForm->animation->interface_controller->chat->system_colors->Strings[n] == "2")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 2);
            if (MainForm->animation->interface_controller->chat->system_colors->Strings[n] == "3")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 3);
            if (MainForm->animation->interface_controller->chat->system_colors->Strings[n] == "4")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 4);
            if (MainForm->animation->interface_controller->chat->system_colors->Strings[n] == "5")
                MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 5);
            MainForm->text_canvas->TextOut(124, 14 * system_row + 332,
                MainForm->animation->interface_controller->chat->system_lines->Strings[n]);
            system_row++;
            n++;
        }
    }
}

// Draws the status panel: stats, health and mana, the damage range and other values, name, guild, weight, points,
// gold, experience, the alignment word and the level.
void InterfaceGraphics::DrawStatusValues()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    MainForm->text_canvas->TextOut(156, 338, IntToStr(MainForm->animation->interface_controller->status->stat_str));
    MainForm->text_canvas->TextOut(156, 356, IntToStr(MainForm->animation->interface_controller->status->stat_int));
    MainForm->text_canvas->TextOut(156, 374, IntToStr(MainForm->animation->interface_controller->status->stat_wis));
    MainForm->text_canvas->TextOut(156, 392, IntToStr(MainForm->animation->interface_controller->status->stat_agi));
    MainForm->text_canvas->TextOut(156, 410, IntToStr(MainForm->animation->interface_controller->status->stat_con));
    MainForm->text_canvas->TextOut(156, 428, IntToStr(MainForm->animation->interface_controller->status->stat_cha));
    MainForm->text_canvas->TextOut(264, 338, IntToStr(MainForm->animation->interface_controller->status->health));
    MainForm->text_canvas->TextOut(264, 356, IntToStr(MainForm->animation->interface_controller->status->mana));
    MainForm->text_canvas->TextOut(264, 374,
        IntToStr(MainForm->animation->interface_controller->status->damage_min) + "-" +
            IntToStr(MainForm->animation->interface_controller->status->damage_max));
    MainForm->text_canvas->TextOut(264, 392, IntToStr(MainForm->animation->interface_controller->status->hitrate));
    MainForm->text_canvas->TextOut(264, 410, IntToStr(MainForm->animation->interface_controller->status->armor));
    MainForm->text_canvas->TextOut(264, 428, IntToStr(MainForm->animation->interface_controller->status->dodge));
    MainForm->text_canvas->TextOut(385, 338,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->status->character_name));
    MainForm->text_canvas->TextOut(385, 356,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->status->guild_name));
    String weight_text = IntToStr(MainForm->animation->interface_controller->status->weight);
    weight_text = weight_text + " / ";
    weight_text = weight_text + IntToStr(MainForm->animation->interface_controller->status->max_weight);
    MainForm->text_canvas->TextOut(385, 374, weight_text);
    MainForm->text_canvas->TextOut(385, 392, IntToStr(MainForm->animation->interface_controller->status->stat_points));
    MainForm->text_canvas->TextOut(385, 410, IntToStr(MainForm->animation->interface_controller->status->skill_points));
    MainForm->text_canvas->TextOut(485, 374, IntToStr(MainForm->animation->interface_controller->status->gold));
    MainForm->text_canvas->TextOut(485, 392, IntToStr(MainForm->animation->interface_controller->status->experience));
    MainForm->text_canvas->TextOut(485, 410,
        IntToStr(MainForm->animation->interface_controller->status->experience_to_next_level));
    int karma = MainForm->animation->interface_controller->status->alignment;
    String karma_text = "";
    // The alignment value maps to a word in bands from Demonic to Pure.
    if (karma <= -900)
        karma_text = "Demonic";
    if (karma > -900 && karma <= -500)
        karma_text = "Doomed";
    if (karma > -500 && karma <= -250)
        karma_text = "Cursed";
    if (karma > -250 && karma <= -100)
        karma_text = "Evil";
    if (karma > -100 && karma < 100)
        karma_text = "Neutral";
    if (karma >= 100 && karma < 250)
        karma_text = "Good";
    if (karma >= 250 && karma < 500)
        karma_text = "Blessed";
    if (karma >= 500 && karma < 900)
        karma_text = "Saint";
    if (karma >= 900)
        karma_text = "Pure";
    MainForm->text_canvas->TextOut(485, 428, karma_text);
    MainForm->text_canvas->TextOut(557, 338, IntToStr(MainForm->animation->interface_controller->status->level));
}

// Draws the group panel scroll arrows, lit under the mouse.
void InterfaceGraphics::DrawGroupScrollButtons()
{
    if (MainForm->animation->interface_controller->group->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->group->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the news panel scroll arrows; the down arrow also lights while the scroll hint shows.
void InterfaceGraphics::DrawNewsScrollButtons()
{
    if (MainForm->animation->interface_controller->news->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->news->scroll_down_hovered ||
        MainForm->animation->interface_controller->news->IsScrollHintVisible())
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the help panel scroll arrows, lit under the mouse.
void InterfaceGraphics::DrawHelpScrollButtons()
{
    if (MainForm->animation->interface_controller->help->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(4), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 350, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(3), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->help->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(2), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(569, 432, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(1), DDBLTFAST_WAIT);
}

// Draws the group panel's scroll thumb and, per visible member, a line icon and a state icon.
void InterfaceGraphics::DrawGroupList()
{
    int i;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->group->GetScrollThumbOffset() + 366,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    int top_row = MainForm->animation->interface_controller->group->GetTopRow();
    int member_count = MainForm->animation->interface_controller->group->members.size();
    int row = 0;
    for (i = top_row; i < member_count && i < top_row + 7; i++)
    {
        if (MainForm->animation->interface_controller->group->members[i].is_leader >= 1)
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(5),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(9),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->group->members[i].removable)
        {
            if (MainForm->animation->interface_controller->group->members[i].is_hovered)
                MainForm->OffscreenSurface->BltFast(440, 14 * row + 351, group_icons, &GetGroupIconRect(2),
                    DDBLTFAST_WAIT);
            else
                MainForm->OffscreenSurface->BltFast(440, 14 * row + 351, group_icons, &GetGroupIconRect(0),
                    DDBLTFAST_WAIT);
        }
        else
            MainForm->OffscreenSurface->BltFast(440, 14 * row + 351, group_icons, &GetGroupIconRect(1), DDBLTFAST_WAIT);
        row++;
    }
}

// Draws the health bar of each visible group member, its colour changing at 40 and 20 percent.
void InterfaceGraphics::DrawGroupHealthBars()
{
    int j;
    int top_row;
    int member_count;
    TRect rc;
    MainForm->draw_canvas->Pen->Color = clGreen;
    MainForm->draw_canvas->Pen->Style = psSolid;
    top_row = MainForm->animation->interface_controller->group->GetTopRow();
    member_count = MainForm->animation->interface_controller->group->members.size();
    int row = 0;
    for (j = top_row; j < member_count && j < top_row + 7; j++)
    {
        if (MainForm->animation->interface_controller->group->members[j].health_percent > 40)
            MainForm->draw_canvas->Brush->Color = (TColor)33340;
        if (MainForm->animation->interface_controller->group->members[j].health_percent <= 40)
        {
            if (MainForm->animation->interface_controller->group->members[j].health_percent > 20)
                MainForm->draw_canvas->Brush->Color = (TColor)42186;
        }
        if (MainForm->animation->interface_controller->group->members[j].health_percent <= 20)
            MainForm->draw_canvas->Brush->Color = (TColor)0x3264FF;
        MainForm->draw_canvas->Brush->Style = bsSolid;
        rc.left = 331;
        rc.right = MainForm->animation->interface_controller->group->members[j].health_percent + 331;
        rc.top = 14 * row + 352;
        rc.bottom = 14 * row + 360;
        MainForm->draw_canvas->FillRect(rc);
        row++;
    }
    row = 0;
    MainForm->draw_canvas->Brush->Color = clBlack;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Pen->Color = clBlack;
    MainForm->draw_canvas->Pen->Style = psSolid;
    j = top_row;
    while (j < member_count && j < top_row + 7)
    {
        TPoint outline[9];
        outline[0].x = 330;
        outline[0].y = 14 * row + 353;
        outline[1].x = 332;
        outline[1].y = 14 * row + 351;
        outline[2].x = 429;
        outline[2].y = 14 * row + 351;
        outline[3].x = 431;
        outline[3].y = 14 * row + 353;
        outline[4].x = 431;
        outline[4].y = 14 * row + 358;
        outline[5].x = 429;
        outline[5].y = 14 * row + 360;
        outline[6].x = 332;
        outline[6].y = 14 * row + 360;
        outline[7].x = 330;
        outline[7].y = 14 * row + 358;
        outline[8].x = 330;
        outline[8].y = 14 * row + 353;
        MainForm->draw_canvas->Polyline(outline, 8);
        row++;
        j++;
    }
}

// Draws the member count, and each visible member's name, level and health label.
void InterfaceGraphics::DrawGroupText()
{
    int row;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int count_width =
        MainForm->text_canvas->TextWidth(IntToStr(MainForm->animation->interface_controller->group->members.size()));
    MainForm->text_canvas->TextOut(580 - count_width, 332,
        IntToStr(MainForm->animation->interface_controller->group->members.size()));
    int top_row = MainForm->animation->interface_controller->group->GetTopRow();
    int member_count = MainForm->animation->interface_controller->group->members.size();
    row = 0;
    MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
    for (int i = top_row; i < member_count && i < top_row + 7; i++)
    {
        MainForm->text_canvas->TextOut(124, 14 * row + 350,
            MainForm->animation->interface_controller->group->members[i].name);
        MainForm->text_canvas->TextOut(246, 14 * row + 350,
            IntToStr(MainForm->animation->interface_controller->group->members[i].level));
        MainForm->text_canvas->TextOut(305, 14 * row + 350, "HP");
        row++;
    }
}

// Draws the marker of every settings row, lit on the row under the mouse.
void InterfaceGraphics::DrawSettingMarkers()
{
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Sound)
        MainForm->OffscreenSurface->BltFast(317, 355, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(317, 355, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Music)
        MainForm->OffscreenSurface->BltFast(317, 373, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(317, 373, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Keyboard)
        MainForm->OffscreenSurface->BltFast(317, 391, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(317, 391, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Language)
        MainForm->OffscreenSurface->BltFast(317, 409, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(317, 409, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_PrivateMessages)
        MainForm->OffscreenSurface->BltFast(317, 427, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(317, 427, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Balloons)
        MainForm->OffscreenSurface->BltFast(557, 355, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(557, 355, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Shadows)
        MainForm->OffscreenSurface->BltFast(557, 373, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(557, 373, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Filter)
        MainForm->OffscreenSurface->BltFast(557, 391, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(557, 391, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_LogChat)
        MainForm->OffscreenSurface->BltFast(557, 409, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(557, 409, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_Requests)
        MainForm->OffscreenSurface->BltFast(557, 427, control_sheet, &GetSettingMarkerRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(557, 427, control_sheet, &GetSettingMarkerRect(0), DDBLTFAST_WAIT);
}

// Draws the value of each settings row: the sound and music volumes, the keyboard layout and language, and each
// switch's on or off.
void InterfaceGraphics::DrawSettingsText()
{
    int filter_text_id;
    int log_text_id;
    int requests_text_id;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    if (MainForm->config->GetSoundVolume() >= 100)
        MainForm->text_canvas->TextOut(222, 356, MainForm->strings->GetText(GameText_Enabled));
    else if (MainForm->config->GetSoundVolume() <= 0)
        MainForm->text_canvas->TextOut(222, 356, MainForm->strings->GetText(GameText_Disabled));
    else
        MainForm->text_canvas->TextOut(222, 356, IntToStr(MainForm->config->GetSoundVolume()) + "% vol.");
    if (MainForm->config->GetMusicVolume() >= 100)
        MainForm->text_canvas->TextOut(222, 374, MainForm->strings->GetText(GameText_Enabled));
    else if (MainForm->config->GetMusicVolume() <= 0)
        MainForm->text_canvas->TextOut(222, 374, MainForm->strings->GetText(GameText_Disabled));
    else
        MainForm->text_canvas->TextOut(222, 374, IntToStr(MainForm->config->GetMusicVolume()) + "% vol.");
    MainForm->text_canvas->TextOut(222, 392, MainForm->strings->GetText(MainForm->config->GetKeyboardLayout() + 254));
    int text_id = GameText_English;
    MainForm->text_canvas->TextOut(222, 410, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetAcceptPrivateMessages())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->text_canvas->TextOut(222, 428, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetShowBalloons())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->text_canvas->TextOut(462, 356, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetShowShadows())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->text_canvas->TextOut(462, 374, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetFilterEnabled())
    {
        if (MainForm->config->GetStrictFilter())
            filter_text_id = GameText_Exclusive;
        else
            filter_text_id = GameText_Normal;
    }
    else
        filter_text_id = GameText_Disabled;
    MainForm->text_canvas->TextOut(462, 392, MainForm->strings->GetText(filter_text_id));
    if (MainForm->config->GetLogChat())
        log_text_id = GameText_Enabled;
    else
        log_text_id = GameText_Disabled;
    MainForm->text_canvas->TextOut(462, 410, MainForm->strings->GetText(log_text_id));
    if (MainForm->config->GetAcceptRequests())
        requests_text_id = GameText_Enabled;
    else
        requests_text_id = GameText_Disabled;
    MainForm->text_canvas->TextOut(462, 428, MainForm->strings->GetText(requests_text_id));
}

// Draws the help panel's scroll thumb and an icon before each visible line that has one.
void InterfaceGraphics::DrawHelpList()
{
    int i;
    int row;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->help->GetScrollThumbOffset() + 366,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    int top_line = MainForm->animation->interface_controller->help->GetTopLine();
    int line_count = MainForm->animation->interface_controller->help->lines->Count;
    row = 0;
    for (i = top_line; i < line_count && i < top_line + 7; i++)
    {
        if (MainForm->animation->interface_controller->help->line_icons->Strings[i] != "0")
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        row++;
    }
}

// Draws the seven visible help lines, underlining titles and links.
void InterfaceGraphics::DrawHelpText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
    int top_line = MainForm->animation->interface_controller->help->GetTopLine();
    int line_count = MainForm->animation->interface_controller->help->lines->Count;
    int row = 0;
    MainForm->text_canvas->Font->Color = (TColor)GetThemeColor(MainForm->config->GetTextTheme(), 0);
    for (int i = top_line; i < line_count && i < top_line + 7; i++)
    {
        if (MainForm->animation->interface_controller->help->line_flags->Strings[i] == "1")
            MainForm->text_canvas->Font->Style = TFontStyles() << fsBold;
        else if (MainForm->animation->interface_controller->help->link_targets->Strings[i] != "")
            MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
        else
            MainForm->text_canvas->Font->Style = TFontStyles();
        MainForm->text_canvas->TextOut(124, 14 * row + 350,
            MainForm->animation->interface_controller->help->lines->Strings[i]);
        row++;
    }
    MainForm->text_canvas->Font->Style = TFontStyles();
}

// Draws the news panel's scroll thumb and a marker before each visible line that has one.
void InterfaceGraphics::DrawNewsList()
{
    int i;
    int row;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->news->GetScrollThumbOffset() + 366,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    int top_line = MainForm->animation->interface_controller->news->GetTopLine();
    int line_count = MainForm->animation->interface_controller->news->lines->Count;
    row = 0;
    for (i = top_line; i < line_count && i < top_line + 7; i++)
    {
        if (MainForm->animation->interface_controller->news->line_markers->Strings[i] != "0")
            MainForm->OffscreenSurface->BltFast(108, 14 * row + 350, small_icons, &GetSmallIconRect(1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        row++;
    }
}

// Draws the visible news lines.
void InterfaceGraphics::DrawNewsText()
{
    int row;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = clBlack;
    int top_line = MainForm->animation->interface_controller->news->GetTopLine();
    int line_count = MainForm->animation->interface_controller->news->lines->Count;
    row = 0;
    MainForm->text_canvas->Font->Color = clBlack;
    for (int i = top_line; i < line_count && i < top_line + 7; i++)
    {
        MainForm->text_canvas->TextOut(124, 14 * row + 350,
            MainForm->animation->interface_controller->news->lines->Strings[i]);
        row++;
    }
}

// Draws the inventory's paperdoll and arrow buttons, lit under the mouse.
void InterfaceGraphics::DrawInventoryButtons()
{
    if (MainForm->animation->interface_controller->inventory->paperdoll_button_hovered)
        MainForm->OffscreenSurface->BltFast(487, 339, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetInventoryWideButtonRect(ButtonState_Hover), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(487, 339, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetInventoryWideButtonRect(ButtonState_Normal), DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->inventory->left_button_hovered)
        MainForm->OffscreenSurface->BltFast(491, 398, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Left, ButtonState_Hover),
            DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(491, 398, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Left, ButtonState_Normal),
            DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->inventory->right_button_hovered)
        MainForm->OffscreenSurface->BltFast(533, 398, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Right, ButtonState_Hover),
            DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(533, 398, MainForm->gfx->interface_graphics->control_sheet,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Right, ButtonState_Normal),
            DDBLTFAST_WAIT);
}

// Draws the chosen spell's name, cut to fit with two dots, and its level, the spell tip while hovering, and the skill
// points left.
void InterfaceGraphics::DrawSpellInfoText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = (TColor)0xE1E1E1;
    if (MainForm->animation->interface_controller->spells->selected_spell_id >= 0 &&
        MainForm->animation->interface_controller->spells->selected_spell_icon >= 0)
    {
        String spell_name = MainForm->animation->interface_controller->spells->selected_spell_name;
        char truncated = false;
        while (MainForm->text_canvas->TextWidth(spell_name) > INFO_NAME_MAX_WIDTH)
        {
            spell_name.Delete(spell_name.Length(), 1);
            truncated = true;
        }
        if (truncated)
            spell_name = spell_name + "..";
        int value = 150 - MainForm->text_canvas->TextWidth(spell_name) / 2;
        MainForm->text_canvas->TextOut(value, 378, spell_name);
        if (MainForm->animation->interface_controller->spells->selected_spell_level >= 0 &&
            MainForm->animation->interface_controller->spells->selected_spell_level <= 100)
            MainForm->text_canvas->TextOut(134, 408,
                IntToStr(MainForm->animation->interface_controller->spells->selected_spell_level));
    }
    else
        MainForm->text_canvas->TextOut(134, 408, "0");
    if (MainForm->animation->interface_controller->spells->has_hover &&
        !MainForm->animation->interface_controller->spells->is_dragging)
        MainForm->animation->interface_controller->spells->tooltip->DrawString();
    MainForm->text_canvas->TextOut(134, 426, IntToStr(MainForm->animation->interface_controller->status->skill_points));
}

// Draws the selected skill's name, cut to fit, its level and the points left to spend.
void InterfaceGraphics::DrawSkillInfoText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    if (MainForm->animation->interface_controller->skills->selected_skill_id >= 0 &&
        MainForm->animation->interface_controller->skills->selected_skill_icon >= 0)
    {
        String skill_name = MainForm->animation->interface_controller->skills->selected_skill_name;
        char truncated = false;
        while (MainForm->text_canvas->TextWidth(skill_name) > INFO_NAME_MAX_WIDTH)
        {
            skill_name.Delete(skill_name.Length(), 1);
            truncated = true;
        }
        if (truncated)
            skill_name = skill_name + "..";
        int value = 150 - MainForm->text_canvas->TextWidth(skill_name) / 2;
        MainForm->text_canvas->TextOut(value, 378, skill_name);
        if (MainForm->animation->interface_controller->skills->selected_skill_level >= 0 &&
            MainForm->animation->interface_controller->skills->selected_skill_level <= 100)
            MainForm->text_canvas->TextOut(134, 408,
                IntToStr(MainForm->animation->interface_controller->skills->selected_skill_level));
    }
    else
        MainForm->text_canvas->TextOut(134, 408, "0");
    MainForm->text_canvas->TextOut(134, 426, IntToStr(MainForm->animation->interface_controller->status->skill_points));
}

// Draws the spell panel: scroll bar, each spell's icon in its cell with its level bar, the hovered and selected
// spells, the blinking raise button while skill points are left, and the spell tip.
void InterfaceGraphics::DrawSpellPanel()
{
    int blink_on;
    vector<SpellItem>::iterator it;
    int icon_id;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->spells->GetScrollThumbOffset() + 348,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->spells->has_hover)
    {
        int frame_state = 1;
        if (MainForm->animation->interface_controller->spells->hover_alt_frame)
            frame_state = 2;
        if (MainForm->animation->interface_controller->spells->hover_col >= 0 &&
            MainForm->animation->interface_controller->spells->hover_row >= 0 &&
            MainForm->animation->interface_controller->spells->hover_col <= 7 &&
            MainForm->animation->interface_controller->spells->hover_row <= 1)
            MainForm->OffscreenSurface->BltFast(45 * MainForm->animation->interface_controller->spells->hover_col + 203,
                52 * MainForm->animation->interface_controller->spells->hover_row + 339, control_sheet,
                &GetSpellSlotFrameRect(frame_state), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->spells->selected_spell_id >= 0 &&
        MainForm->animation->interface_controller->spells->selected_spell_icon >= 0)
    {
        int info_icon_id = MainForm->animation->interface_controller->spells->selected_spell_icon;
        MainForm->OffscreenSurface->BltFast(134, 344, MainForm->gfx->icons->GetGraphic(info_icon_id).resource->surface,
            &MainForm->gfx->icons->GetGraphic(info_icon_id).resource->GetIconRect(0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        // With skill points left, the raise button blinks by the selected spell.
        if (MainForm->animation->interface_controller->status->skill_points > 0)
        {
            blink_on = false;
            if (MainForm->animation->interface_controller->spells->GetBlinkPhase() > 5)
                blink_on = true;
            MainForm->OffscreenSurface->BltFast(173, 407, MainForm->gfx->interface_graphics->control_sheet,
                &MainForm->gfx->interface_graphics->GetRaiseButtonRect(blink_on), DDBLTFAST_WAIT);
            MainForm->OffscreenSurface->BltFast(173, 425, MainForm->gfx->interface_graphics->control_sheet,
                &MainForm->gfx->interface_graphics->GetRaiseButtonRect(blink_on), DDBLTFAST_WAIT);
        }
    }
    for (it = MainForm->animation->interface_controller->spells->spells.begin();
        it != MainForm->animation->interface_controller->spells->spells.end(); it++)
    {
        icon_id = MainForm->skill_values->GetIcon(it->spell_id);
        if (MainForm->gfx->icons->RequestGraphic(icon_id))
        {
            if (it->is_visible)
            {
                int slot_column = it->grid_x;
                int slot_row = it->grid_y - MainForm->animation->interface_controller->spells->GetTopRow();
                if (slot_row == 0 || slot_row == 1)
                {
                    if (MainForm->animation->interface_controller->spells->hover_col == slot_column &&
                        MainForm->animation->interface_controller->spells->hover_row == slot_row)
                        MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                            MainForm->gfx->icons->GetGraphic(icon_id).resource->surface,
                            &MainForm->gfx->icons->GetGraphic(icon_id).resource->GetIconRect(1),
                            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    else
                        MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                            MainForm->gfx->icons->GetGraphic(icon_id).resource->surface,
                            &MainForm->gfx->icons->GetGraphic(icon_id).resource->GetIconRect(0),
                            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    if (it->level > 0 && it->level <= 100)
                        MainForm->blitter->HalfBlendBlt(45 * slot_column + 206, 52 * slot_row + 379,
                            MainForm->OffscreenSurface, control_sheet, &GetSpellProgressRect(it->level));
                }
            }
            else
                MainForm->gfx->icons->GetGraphic(icon_id);
        }
        else
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
    }
    if (!MainForm->animation->interface_controller->spells->GetTopRow())
    {
        for (int j = 0; j < 8; j++)
            MainForm->OffscreenSurface->BltFast(45 * j + 202, 338, hud_sheet, &GetSpellSlotLabelRect(j),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        for (int k = 8; k < 16; k++)
            MainForm->OffscreenSurface->BltFast(45 * k - 158, 390, hud_sheet, &GetSpellSlotLabelRect(k),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->spells->GetTopRow() == 1)
    {
        for (int m = 8; m < 16; m++)
            MainForm->OffscreenSurface->BltFast(45 * m - 158, 338, hud_sheet, &GetSpellSlotLabelRect(m),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->spells->has_hover &&
        !MainForm->animation->interface_controller->spells->is_dragging)
        MainForm->animation->interface_controller->spells->tooltip->DrawFrame();
}

// Draws the skill panel: scroll thumb, hover frame, the selected skill's icon and raise buttons, and each visible
// skill's icon with its progress bar.
void InterfaceGraphics::DrawSkillPanel()
{
    int blink_on;
    vector<SkillItem>::iterator it;
    int icon_id;
    MainForm->OffscreenSurface->BltFast(569,
        MainForm->animation->interface_controller->skills->GetScrollThumbOffset() + 348,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(0),
        DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->skills->has_hover)
    {
        int frame_state = 1;
        if (MainForm->animation->interface_controller->skills->hover_alt_frame)
            frame_state = 2;
        if (MainForm->animation->interface_controller->skills->hover_col >= 0 &&
            MainForm->animation->interface_controller->skills->hover_row >= 0 &&
            MainForm->animation->interface_controller->skills->hover_col <= 7 &&
            MainForm->animation->interface_controller->skills->hover_row <= 1)
            MainForm->OffscreenSurface->BltFast(45 * MainForm->animation->interface_controller->skills->hover_col + 203,
                52 * MainForm->animation->interface_controller->skills->hover_row + 339, control_sheet,
                &GetSkillSlotFrameRect(frame_state), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->interface_controller->skills->selected_skill_id >= 0 &&
        MainForm->animation->interface_controller->skills->selected_skill_icon >= 0)
    {
        int info_icon_id = MainForm->animation->interface_controller->skills->selected_skill_icon;
        MainForm->OffscreenSurface->BltFast(134, 344, MainForm->gfx->icons->GetGraphic(info_icon_id).resource->surface,
            &MainForm->gfx->icons->GetGraphic(info_icon_id).resource->GetIconRect(0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->status->skill_points > 0)
        {
            blink_on = false;
            if (MainForm->animation->interface_controller->skills->GetBlinkPhase() > 5)
                blink_on = true;
            MainForm->OffscreenSurface->BltFast(173, 407, MainForm->gfx->interface_graphics->control_sheet,
                &MainForm->gfx->interface_graphics->GetRaiseButtonRect(blink_on), DDBLTFAST_WAIT);
            MainForm->OffscreenSurface->BltFast(173, 425, MainForm->gfx->interface_graphics->control_sheet,
                &MainForm->gfx->interface_graphics->GetRaiseButtonRect(blink_on), DDBLTFAST_WAIT);
        }
    }
    for (it = MainForm->animation->interface_controller->skills->skills.begin();
        it != MainForm->animation->interface_controller->skills->skills.end(); it++)
    {
        icon_id = MainForm->skill_values->GetIcon(it->skill_id);
        if (MainForm->gfx->icons->RequestGraphic(icon_id))
        {
            if (it->is_visible)
            {
                int slot_column = it->grid_x;
                int slot_row = it->grid_y - MainForm->animation->interface_controller->skills->GetTopRow();
                if (slot_row == 0 || slot_row == 1)
                {
                    if (MainForm->animation->interface_controller->skills->hover_col == slot_column &&
                        MainForm->animation->interface_controller->skills->hover_row == slot_row)
                        MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                            MainForm->gfx->icons->GetGraphic(icon_id).resource->surface,
                            &MainForm->gfx->icons->GetGraphic(icon_id).resource->GetIconRect(1),
                            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    else
                        MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                            MainForm->gfx->icons->GetGraphic(icon_id).resource->surface,
                            &MainForm->gfx->icons->GetGraphic(icon_id).resource->GetIconRect(0),
                            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    if (it->level > 0 && it->level <= 100)
                        MainForm->blitter->HalfBlendBlt(45 * slot_column + 206, 52 * slot_row + 379,
                            MainForm->OffscreenSurface, control_sheet, &GetSpellProgressRect(it->level));
                }
            }
            else
                MainForm->gfx->icons->GetGraphic(icon_id);
        }
        else
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
    }
}

// Draws the inventory grid: the hovered cells, marked when the drop is blocked, each item's picture at its place,
// and the item tip.
void InterfaceGraphics::DrawInventoryGrid()
{
    InventoryItem *it;
    int graphic_index;
    int last_col;
    int last_row;
    int highlight_state;
    int col;
    if (MainForm->animation->interface_controller->inventory->has_hover)
    {
        // Larger items take two columns and fewer rows.
        if (MainForm->animation->interface_controller->inventory->hover_item_size > 3)
        {
            last_col = 1;
            last_row = MainForm->animation->interface_controller->inventory->hover_item_size - 4;
        }
        else
        {
            last_col = 0;
            last_row = MainForm->animation->interface_controller->inventory->hover_item_size;
        }
        highlight_state = 1;
        if (MainForm->animation->interface_controller->inventory->drop_blocked)
            highlight_state = 2;
        if (MainForm->animation->interface_controller->inventory->hover_col >= 0 &&
            MainForm->animation->interface_controller->inventory->hover_row >= 0 &&
            MainForm->animation->interface_controller->inventory->hover_col + last_col <= 13 &&
            MainForm->animation->interface_controller->inventory->hover_row + last_row <= 3)
        {
            for (col = 0; col <= last_col; col++)
            {
                for (int j = 0; j <= last_row; j++)
                    MainForm->OffscreenSurface->BltFast(26 * col +
                        MainForm->animation->interface_controller->inventory->hover_col * 26 + 115,
                        26 * j + 26 * MainForm->animation->interface_controller->inventory->hover_row + 339,
                        control_sheet, &GetSlotHighlightRect(highlight_state), DDBLTFAST_WAIT);
            }
        }
    }
    it = MainForm->animation->interface_controller->inventory->items.begin();
    while (it != MainForm->animation->interface_controller->inventory->items.end())
    {
        graphic_index = MainForm->item_values->GetGraphic(it->item_id) - 1;
        if (MainForm->gfx->items->RequestSubItemGraphic(graphic_index))
        {
            if (it->is_visible)
                MainForm->OffscreenSurface->BltFast(26 * it->grid_x + 114, 26 * it->grid_y + 339,
                    MainForm->gfx->items->GetSubItemGraphic(graphic_index).resource->surface,
                    &MainForm->gfx->items->GetSubItemGraphic(graphic_index).resource->surface_rect,
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            else
                MainForm->gfx->items->GetSubItemGraphic(graphic_index);
        }
        else
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
        it++;
    }
    if (MainForm->animation->interface_controller->inventory->has_hover &&
        !MainForm->animation->interface_controller->inventory->is_dragging)
        MainForm->animation->interface_controller->inventory->tooltip->DrawFrame();
    MainForm->gfx->items->AgeSubItemGraphics();
}

// Draws the hovered item's tooltip, unless an item is being dragged.
void InterfaceGraphics::DrawInventoryText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->Font->Color = (TColor)0xE1E1E1;
    String weight_text = IntToStr(MainForm->animation->interface_controller->status->weight);
    weight_text = weight_text + " / ";
    weight_text = weight_text + IntToStr(MainForm->animation->interface_controller->status->max_weight);
    int spare_width = 86 - MainForm->text_canvas->TextWidth(weight_text);
    if (spare_width < 0)
        spare_width = 0;
    MainForm->text_canvas->TextOut(spare_width / 2 + 488, 369, weight_text);
    if (MainForm->animation->interface_controller->inventory->has_hover &&
        !MainForm->animation->interface_controller->inventory->is_dragging)
        MainForm->animation->interface_controller->inventory->tooltip->DrawString();
}

// Saves the screen under the dragged item, then draws its picture half blended at the mouse, cut to the screen.
void InterfaceGraphics::DrawDraggedItem()
{
    int icon_half_width;
    int icon_half_height;
    if (MainForm->animation->interface_controller->inventory->is_dragging)
    {
        icon_half_width = 12;
        if (MainForm->animation->interface_controller->inventory->drag_item_size > ITEM_SIZE_MAX_NARROW)
        {
            icon_half_width = 24;
            icon_half_height = 12 * MainForm->animation->interface_controller->inventory->drag_item_size - 36;
        }
        else
            icon_half_height = 12 * MainForm->animation->interface_controller->inventory->drag_item_size + 12;
        drag_rect.top = MainForm->animation->interface_controller->inventory->drag_y - 46;
        drag_rect.left = MainForm->animation->interface_controller->inventory->drag_x - 25;
        drag_rect.bottom = drag_rect.top + 102;
        drag_rect.right = drag_rect.left + 50;
        drag_height = 102;
        drag_width = DRAG_BACKUP_WIDTH;
        if (drag_rect.top < 0)
        {
            drag_height = 102 - (0 - drag_rect.top);
            drag_rect.top = 0;
        }
        if (drag_rect.left < 0)
        {
            drag_width = 50 - (0 - drag_rect.left);
            drag_rect.left = 0;
        }
        if (drag_rect.right > SCREEN_WIDTH)
        {
            drag_width -= drag_rect.right - 640;
            drag_rect.right = SCREEN_WIDTH;
        }
        if (drag_rect.bottom > SCREEN_HEIGHT)
        {
            drag_height -= drag_rect.bottom - 480;
            drag_rect.bottom = SCREEN_HEIGHT;
        }
        drag_save_surface->BltFast(0, 0, MainForm->OffscreenSurface, &drag_rect, DDBLTFAST_WAIT);
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->interface_controller->inventory->drag_graphic))
        {
            TRect source_rect =
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->interface_controller->inventory->drag_graphic).resource->surface_rect;
            int draw_x = MainForm->animation->interface_controller->inventory->drag_x - icon_half_width;
            int draw_y = MainForm->animation->interface_controller->inventory->drag_y - icon_half_height;
            if (draw_x < 0)
            {
                source_rect.left = 0 - draw_x;
                draw_x = 0;
            }
            if (draw_y < 0)
            {
                source_rect.top = 0 - draw_y;
                draw_y = 0;
            }
            if (source_rect.right + draw_x > 640)
                source_rect.right -= source_rect.right + draw_x - 640;
            if (source_rect.bottom + draw_y > 480)
                source_rect.bottom -= source_rect.bottom + draw_y - 480;
            MainForm->blitter->HalfBlendBlt(draw_x, draw_y, MainForm->OffscreenSurface,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->interface_controller->inventory->drag_graphic).resource->surface,
                &source_rect);
        }
    }
}

// Saves the screen under the dragged spell, then draws its icon half blended at the mouse, cut to the screen.
void InterfaceGraphics::DrawDraggedSpell()
{
    int draw_x;
    int draw_y;
    if (MainForm->animation->interface_controller->spells->is_dragging)
    {
        int icon_half_width = 16;
        int icon_half_height = 16;
        drag_rect.top = MainForm->animation->interface_controller->spells->drag_y - 25;
        drag_rect.left = MainForm->animation->interface_controller->spells->drag_x - 25;
        drag_rect.bottom = drag_rect.top + 50;
        drag_rect.right = drag_rect.left + 50;
        drag_height = DRAG_ICON_BACKUP_HEIGHT;
        drag_width = DRAG_BACKUP_WIDTH;
        if (drag_rect.top < 0)
        {
            drag_height = 50 - (0 - drag_rect.top);
            drag_rect.top = 0;
        }
        if (drag_rect.left < 0)
        {
            drag_width = 50 - (0 - drag_rect.left);
            drag_rect.left = 0;
        }
        if (drag_rect.right > SCREEN_WIDTH)
        {
            drag_width -= drag_rect.right - 640;
            drag_rect.right = SCREEN_WIDTH;
        }
        if (drag_rect.bottom > SCREEN_HEIGHT)
        {
            drag_height -= drag_rect.bottom - 480;
            drag_rect.bottom = SCREEN_HEIGHT;
        }
        drag_save_surface->BltFast(0, 0, MainForm->OffscreenSurface, &drag_rect, DDBLTFAST_WAIT);
        if (MainForm->gfx->icons->RequestGraphic(MainForm->animation->interface_controller->spells->drag_icon))
        {
            TRect source_rect =
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->spells->drag_icon).resource->GetIconRect(1);
            draw_x = MainForm->animation->interface_controller->spells->drag_x - icon_half_width;
            draw_y = MainForm->animation->interface_controller->spells->drag_y - icon_half_height;
            if (draw_x < 0)
            {
                source_rect.left = 0 - draw_x;
                draw_x = 0;
            }
            if (draw_y < 0)
            {
                source_rect.top = 0 - draw_y;
                draw_y = 0;
            }
            if (draw_x + source_rect.right > 640)
                source_rect.right -= draw_x + source_rect.right - 640;
            if (draw_y + source_rect.bottom > 480)
                source_rect.bottom -= draw_y + source_rect.bottom - 480;
            MainForm->blitter->HalfBlendBlt(draw_x, draw_y, MainForm->OffscreenSurface,
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->spells->drag_icon).resource->surface,
                &source_rect);
        }
    }
}

// Saves the screen under the dragged skill, then draws its icon half blended at the mouse, cut to the screen.
void InterfaceGraphics::DrawDraggedSkill()
{
    int draw_x;
    int draw_y;
    if (MainForm->animation->interface_controller->skills->is_dragging)
    {
        int icon_half_width = 16;
        int icon_half_height = 16;
        drag_rect.top = MainForm->animation->interface_controller->skills->drag_y - 25;
        drag_rect.left = MainForm->animation->interface_controller->skills->drag_x - 25;
        drag_rect.bottom = drag_rect.top + 50;
        drag_rect.right = drag_rect.left + 50;
        drag_height = DRAG_ICON_BACKUP_HEIGHT;
        drag_width = DRAG_BACKUP_WIDTH;
        if (drag_rect.top < 0)
        {
            drag_height = 50 - (0 - drag_rect.top);
            drag_rect.top = 0;
        }
        if (drag_rect.left < 0)
        {
            drag_width = 50 - (0 - drag_rect.left);
            drag_rect.left = 0;
        }
        if (drag_rect.right > SCREEN_WIDTH)
        {
            drag_width -= drag_rect.right - 640;
            drag_rect.right = SCREEN_WIDTH;
        }
        if (drag_rect.bottom > SCREEN_HEIGHT)
        {
            drag_height -= drag_rect.bottom - 480;
            drag_rect.bottom = SCREEN_HEIGHT;
        }
        drag_save_surface->BltFast(0, 0, MainForm->OffscreenSurface, &drag_rect, DDBLTFAST_WAIT);
        if (MainForm->gfx->icons->RequestGraphic(MainForm->animation->interface_controller->skills->drag_icon))
        {
            TRect source_rect =
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->skills->drag_icon).resource->GetIconRect(1);
            draw_x = MainForm->animation->interface_controller->skills->drag_x - icon_half_width;
            draw_y = MainForm->animation->interface_controller->skills->drag_y - icon_half_height;
            if (draw_x < 0)
            {
                source_rect.left = 0 - draw_x;
                draw_x = 0;
            }
            if (draw_y < 0)
            {
                source_rect.top = 0 - draw_y;
                draw_y = 0;
            }
            if (draw_x + source_rect.right > 640)
                source_rect.right -= draw_x + source_rect.right - 640;
            if (draw_y + source_rect.bottom > 480)
                source_rect.bottom -= draw_y + source_rect.bottom - 480;
            MainForm->blitter->HalfBlendBlt(draw_x, draw_y, MainForm->OffscreenSurface,
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->skills->drag_icon).resource->surface,
                &source_rect);
        }
    }
}

// Draws the HUD status bars for health, mana, stamina and experience, and a text box under each bar asked to show
// its numbers.
void InterfaceGraphics::DrawStatusBars()
{
    MainForm->OffscreenSurface->BltFast(55, 0, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->status_icon_left_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(77, 0, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->status_icon_right_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(101, 0, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->status_bars_frame_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(125, 4, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->GetStatusBarFillRect(0,
            MainForm->animation->interface_controller->game_bars->GetHealthBarSegments()),
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(234, 4, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->GetStatusBarFillRect(1,
            MainForm->animation->interface_controller->game_bars->GetManaBarSegments()),
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(345, 4, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->GetStatusBarFillRect(2,
            MainForm->animation->interface_controller->game_bars->GetStaminaBarSegments()),
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(454, 4, MainForm->gfx->interface_graphics->hud_sheet,
        &MainForm->gfx->interface_graphics->GetStatusBarFillRect(3,
            MainForm->animation->interface_controller->game_bars->GetExperienceBarSegments()),
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->game_bars->show_health_text ||
        MainForm->animation->interface_controller->game_bars->show_mana_text ||
        MainForm->animation->interface_controller->game_bars->show_stamina_text ||
        MainForm->animation->interface_controller->game_bars->show_experience_text)
    {
        if (MainForm->animation->interface_controller->game_bars->show_health_text)
            MainForm->OffscreenSurface->BltFast(101, 10, MainForm->gfx->interface_graphics->hud_sheet,
                &MainForm->gfx->interface_graphics->bar_text_box_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->game_bars->show_mana_text)
            MainForm->OffscreenSurface->BltFast(210, 10, MainForm->gfx->interface_graphics->hud_sheet,
                &MainForm->gfx->interface_graphics->bar_text_box_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->game_bars->show_stamina_text)
            MainForm->OffscreenSurface->BltFast(321, 10, MainForm->gfx->interface_graphics->hud_sheet,
                &MainForm->gfx->interface_graphics->bar_text_box_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->interface_controller->game_bars->show_experience_text)
            MainForm->OffscreenSurface->BltFast(430, 10, MainForm->gfx->interface_graphics->hud_sheet,
                &MainForm->gfx->interface_graphics->bar_text_box_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the numbers of the status bars asked to show them; experience shows what is left to the next level.
void InterfaceGraphics::DrawStatusBarText()
{
    if (MainForm->animation->interface_controller->game_bars->show_health_text ||
        MainForm->animation->interface_controller->game_bars->show_mana_text ||
        MainForm->animation->interface_controller->game_bars->show_stamina_text ||
        MainForm->animation->interface_controller->game_bars->show_experience_text)
    {
        MainForm->text_canvas->Brush->Color = clWhite;
        MainForm->text_canvas->Brush->Style = bsClear;
        MainForm->text_canvas->Font->Name = "MS Sans Serif";
        MainForm->text_canvas->Font->Size = 8;
        MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
        if (MainForm->animation->interface_controller->game_bars->show_health_text)
            MainForm->text_canvas->TextOut(104, 14,
                IntToStr(MainForm->animation->interface_controller->status->health) + "/" +
                    IntToStr(MainForm->animation->interface_controller->status->max_health));
        if (MainForm->animation->interface_controller->game_bars->show_mana_text)
            MainForm->text_canvas->TextOut(213, 14,
                IntToStr(MainForm->animation->interface_controller->status->mana) + "/" +
                    IntToStr(MainForm->animation->interface_controller->status->max_mana));
        if (MainForm->animation->interface_controller->game_bars->show_stamina_text)
            MainForm->text_canvas->TextOut(324, 14,
                IntToStr(MainForm->animation->interface_controller->status->stamina) + "/" +
                    IntToStr(MainForm->animation->interface_controller->status->max_stamina));
        if (MainForm->animation->interface_controller->game_bars->show_experience_text)
            MainForm->text_canvas->TextOut(433, 14,
                IntToStr(MainForm->animation->interface_controller->status->experience_to_next_level));
    }
}

// Puts back the screen saved under the dragged item.
void InterfaceGraphics::RestoreItemDragBackground()
{
    TRect source_rect;
    source_rect.top = 0;
    source_rect.left = 0;
    source_rect.right = drag_width;
    source_rect.bottom = drag_height;
    MainForm->OffscreenSurface->BltFast(drag_rect.left, drag_rect.top, drag_save_surface, &source_rect, DDBLTFAST_WAIT);
}

// Puts back the screen saved under the dragged spell.
void InterfaceGraphics::RestoreSpellDragBackground()
{
    TRect source_rect;
    source_rect.top = 0;
    source_rect.left = 0;
    source_rect.right = drag_width;
    source_rect.bottom = drag_height;
    MainForm->OffscreenSurface->BltFast(drag_rect.left, drag_rect.top, drag_save_surface, &source_rect, DDBLTFAST_WAIT);
}

// Puts back the screen saved under the dragged skill.
void InterfaceGraphics::RestoreSkillDragBackground()
{
    TRect source_rect;
    source_rect.top = 0;
    source_rect.left = 0;
    source_rect.right = drag_width;
    source_rect.bottom = drag_height;
    MainForm->OffscreenSurface->BltFast(drag_rect.left, drag_rect.top, drag_save_surface, &source_rect, DDBLTFAST_WAIT);
}
