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
#include "Gfx_game.h"
#include "Ctrl_bossbars.h"

#pragma package(smart_init)

// Creates every interface surface at its fixed size and sets the source rects of the HUD sheet's pieces.
InterfaceGraphics::InterfaceGraphics()
{
    hud_background = new TDx_DrawSurface(NULL);
    clock_background = new TDx_DrawSurface(NULL);
    tooltip_background = new TDx_DrawSurface(NULL);
    chat_panel_surface = new TDx_DrawSurface(NULL);
    chat_mode_icons = new TDx_DrawSurface(NULL);
    chat_input_background = new TDx_DrawSurface(NULL);
    small_icons = new TDx_DrawSurface(NULL);
    chat_tabs = new TDx_DrawSurface(NULL);
    corner_button = new TDx_DrawSurface(NULL);
    small_buttons = new TDx_DrawSurface(NULL);
    inventory_buttons = new TDx_DrawSurface(NULL);
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
    tile_cursor = new TDx_DrawSurface(NULL);
    emote_sheet = new TDx_DrawSurface(NULL);
    weapon_effects = new TDx_DrawSurface(NULL);
    drag_save_surface = new TDx_DrawSurface(NULL);
    element_icons = new TDx_DrawSurface(NULL);
    popup_icons = new TDx_DrawSurface(NULL);
    hud_icons = new TDx_DrawSurface(NULL);
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
    surface_desc->Width = 39;
    surface_desc->Height = 44;
    corner_button->GetSurfaceDesc(surface_desc);
    corner_button->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 31;
    surface_desc->Height = 32;
    small_buttons->GetSurfaceDesc(surface_desc);
    small_buttons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 46;
    surface_desc->Height = 107;
    inventory_buttons->GetSurfaceDesc(surface_desc);
    inventory_buttons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 297;
    surface_desc->Height = 80;
    minimap_tiles->GetSurfaceDesc(surface_desc);
    minimap_tiles->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 83;
    surface_desc->Height = 189;
    chat_mode_icons->GetSurfaceDesc(surface_desc);
    chat_mode_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 72;
    surface_desc->Height = 209;
    tab_buttons->GetSurfaceDesc(surface_desc);
    tab_buttons->Create(surface_desc, MainForm->Dx_Draw1);
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
    surface_desc->Width = 120;
    surface_desc->Height = 40;
    status_icons->GetSurfaceDesc(surface_desc);
    status_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 439;
    surface_desc->Height = 104;
    hud_sheet->GetSurfaceDesc(surface_desc);
    hud_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 50;
    surface_desc->Height = 102;
    drag_save_surface->GetSurfaceDesc(surface_desc);
    drag_save_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 184;
    surface_desc->Height = 56;
    element_icons->GetSurfaceDesc(surface_desc);
    element_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 225;
    surface_desc->Height = 18;
    popup_icons->GetSurfaceDesc(surface_desc);
    popup_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 68;
    surface_desc->Height = 50;
    hud_icons->GetSurfaceDesc(surface_desc);
    hud_icons->Create(surface_desc, MainForm->Dx_Draw1);
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
    panel_rect.top = 0;
    panel_rect.left = 0;
    panel_rect.bottom = 119;
    panel_rect.right = 485;
    status_bars_frame_rect.top = 0;
    status_bars_frame_rect.left = 0;
    status_bars_frame_rect.bottom = 14;
    status_bars_frame_rect.right = 439;
    health_bar_frame_rect.top = 28;
    health_bar_frame_rect.left = 0;
    health_bar_frame_rect.bottom = 35;
    health_bar_frame_rect.right = 40;
    health_bar_shadow_rect.top = 77;
    health_bar_shadow_rect.left = 0;
    health_bar_shadow_rect.bottom = 84;
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
    scroll_up_arrow_rect.top = 47;
    scroll_up_arrow_rect.left = 177;
    scroll_up_arrow_rect.bottom = 51;
    scroll_up_arrow_rect.right = 183;
    scroll_down_arrow_rect.top = 52;
    scroll_down_arrow_rect.left = 177;
    scroll_down_arrow_rect.bottom = 56;
    scroll_down_arrow_rect.right = 183;
    notification_rect.top = 0;
    notification_rect.left = 0;
    notification_rect.bottom = 50;
    notification_rect.right = 50;
    notification_top_rect.top = 0;
    notification_top_rect.left = 0;
    notification_top_rect.bottom = 45;
    notification_top_rect.right = 50;
    notification_bottom_rect.top = 34;
    notification_bottom_rect.left = 0;
    notification_bottom_rect.bottom = 50;
    notification_bottom_rect.right = 50;
    minimap_visible = 0;
}

// The theme colour of a chat colour code, 1 to 5, or the default chat colour.
int InterfaceGraphics::GetCodeColor(String code)
{
    if (code == "1")
        return MainForm->themes->color_chat_private;
    if (code == "2")
        return MainForm->themes->color_chat_system;
    if (code == "3")
        return MainForm->themes->color_chat_warning;
    if (code == "4")
        return MainForm->themes->color_chat_global;
    if (code == "5")
        return MainForm->themes->color_chat_admin;
    return MainForm->themes->color_chat_default;
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

// One damage digit sprite of the interface sheet; the kind picks the row, 0 past 4.
TRect InterfaceGraphics::GetDamageDigitRect(unsigned char style, int digit)
{
    TRect damage_digit_rect;
    if (style > 4)
        style = 0;
    damage_digit_rect.left = 9 * digit + 40;
    damage_digit_rect.right = 9 * digit + 49;
    damage_digit_rect.top = 12 * style + 28;
    damage_digit_rect.bottom = 12 * style + 38;
    return damage_digit_rect;
}

// The miss sprite of the interface sheet; the kind picks the row, 0 past 4.
TRect InterfaceGraphics::GetMissRect(unsigned char kind)
{
    TRect rect;
    if (kind > 4)
        kind = 0;
    rect.left = 132;
    rect.right = 161;
    rect.top = 12 * kind + 28;
    rect.bottom = 12 * kind + 38;
    return rect;
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
TRect InterfaceGraphics::GetCornerButtonRect(int tab)
{
    TRect corner_button_rect;
    corner_button_rect.left = 0;
    corner_button_rect.right = 39;
    corner_button_rect.top = 22 * tab;
    corner_button_rect.bottom = 22 * tab + 22;
    return corner_button_rect;
}

TRect InterfaceGraphics::GetSmallButtonRect(int button)
{
    TRect rect;
    rect.left = 0;
    rect.right = 31;
    rect.top = 16 * button;
    rect.bottom = 16 * button + 16;
    return rect;
}

TRect InterfaceGraphics::GetMinimapTileRect(int row, int edge)
{
    TRect minimap_edge_rect;
    minimap_edge_rect.left = 27 * edge;
    minimap_edge_rect.right = 27 * edge + 26;
    minimap_edge_rect.top = 16 * row;
    minimap_edge_rect.bottom = 16 * row + 14;
    return minimap_edge_rect;
}

TRect InterfaceGraphics::GetListIconInnerRect(int row, int column)
{
    TRect rect;
    rect.left = 27 * column + 2;
    rect.right = 27 * column + 25;
    rect.top = 16 * row + 2;
    rect.bottom = 16 * row + 13;
    return rect;
}

TRect InterfaceGraphics::GetListIconMarkRect(int row, int column)
{
    TRect rect;
    rect.left = 27 * column + 8;
    rect.right = 27 * column + 18;
    rect.top = 16 * row;
    rect.bottom = 16 * row + 14;
    return rect;
}

TRect InterfaceGraphics::GetMinimapMarkRect(int row, int column)
{
    TRect rect;
    rect.left = 7 * column + 216;
    rect.right = 7 * column + 222;
    rect.top = 7 * row + 48;
    rect.bottom = 7 * row + 53;
    return rect;
}

TRect InterfaceGraphics::GetStatusIconLowRect(int row, int column)
{
    TRect rect;
    rect.left = 7 * column + 216;
    rect.right = 7 * column + 222;
    rect.top = 7 * row + 49;
    rect.bottom = 7 * row + 53;
    return rect;
}

TRect InterfaceGraphics::GetMarkerRect(int unused, int index)
{
    TRect rect;
    rect.left = 11 * index + 252;
    rect.right = 11 * index + 262;
    rect.top = 69;
    rect.bottom = 77;
    return rect;
}

TRect InterfaceGraphics::GetElementIconRect(int element)
{
    TRect rect;
    rect.left = 8 * element + 120;
    rect.right = 8 * element + 127;
    rect.top = 38;
    rect.bottom = 47;
    return rect;
}

TRect InterfaceGraphics::GetWeaknessIconRect(int weakness)
{
    TRect rect;
    rect.left = 15 * weakness;
    rect.right = 15 * weakness + 14;
    rect.top = 39;
    rect.bottom = 56;
    return rect;
}

TRect InterfaceGraphics::GetPopupFrameRect()
{
    TRect rect;
    rect.left = 0;
    rect.right = 184;
    rect.top = 0;
    rect.bottom = 38;
    return rect;
}

TRect InterfaceGraphics::GetBarRowRect(int row)
{
    TRect rect;
    rect.left = 0;
    rect.right = 206;
    rect.top = 6 * row;
    rect.bottom = 6 * row + 5;
    return rect;
}

// The bar of the row filled to the percent, 2 pixels a percent, kept within 0 to 100.
TRect InterfaceGraphics::GetPercentBarRect(int percent, int row)
{
    TRect rect;
    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;
    rect.left = 0;
    rect.right = 2 * percent;
    rect.top = 3 * row + 7;
    rect.bottom = 3 * row + 9;
    return rect;
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

// The place of an icon button on the sheet: its plain picture in the first row, its pressed one in the second.
TRect InterfaceGraphics::GetStatusIconRect(int icon, bool small_icon)
{
    TRect status_icon_rect;
    if (!small_icon)
    {
        status_icon_rect.left = 20 * icon + 2;
        status_icon_rect.right = 20 * icon + 18;
        status_icon_rect.top = 2;
        status_icon_rect.bottom = 18;
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
    spell_slot_label_rect.top = 91;
    spell_slot_label_rect.bottom = 104;
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
    inventory_button_rect.left = 23 * state;
    inventory_button_rect.right = 23 * state + 23;
    inventory_button_rect.top = 27 * button;
    inventory_button_rect.bottom = 27 * button + 26;
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

// A notification's status mark on the hud icons: row 0 the translucent backing, rows 1 to 3 the marks.
TRect InterfaceGraphics::GetInventoryWideButtonRect(int mark)
{
    TRect inventory_wide_button_rect;
    inventory_wide_button_rect.left = 52;
    inventory_wide_button_rect.top = 12 * mark;
    inventory_wide_button_rect.right = 68;
    inventory_wide_button_rect.bottom = 12 * mark + 11;
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
    small_buttons->Destroy();
    inventory_buttons->Destroy();
    chat_tabs->Destroy();
    small_icons->Destroy();
    tab_buttons->Destroy();
    group_icons->Destroy();
    minimap_tiles->Destroy();
    status_icons->Destroy();
    hud_sheet->Destroy();
    element_icons->Destroy();
    popup_icons->Destroy();
    hud_icons->Destroy();
    delete hud_background;
    delete clock_background;
    delete tooltip_background;
    delete chat_panel_surface;
    delete chat_input_background;
    delete chat_mode_icons;
    delete corner_button;
    delete small_buttons;
    delete inventory_buttons;
    delete chat_tabs;
    delete small_icons;
    delete tab_buttons;
    delete group_icons;
    delete minimap_tiles;
    delete status_icons;
    delete hud_sheet;
    delete element_icons;
    delete popup_icons;
    delete hud_icons;
    tile_cursor->Destroy();
    panel_background->Destroy();
    loading_icon->Destroy();
    emote_sheet->Destroy();
    weapon_effects->Destroy();
    control_sheet->Destroy();
    scroll_icons->Destroy();
    list_icons->Destroy();
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

void InterfaceGraphics::LoadSurface(TDx_DrawSurface *surface, int library, int resource_id)
{
    LoadSurfaceAt(surface, library, resource_id, 0, 0);
}

// Loads the library's bitmap onto the surface; false when it cannot.
bool InterfaceGraphics::LoadSurfaceAt(TDx_DrawSurface *surface, int library, int resource_id, int x, int y)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return false;
    if (MainForm->gfx_transfers->BeginDraw(surface))
    {
        MainForm->gfx_transfers->Draw(surface);
        MainForm->gfx_transfers->EndDraw(surface);
        return true;
    }
    return false;
}

// Draws the interface emote sheet bitmap onto its surface.
void InterfaceGraphics::LoadEmoteSheet(int library, int resource_id)
{
    LoadSurface(emote_sheet, library, resource_id);
}

// Draws the interface weapon effects bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadWeaponEffects(int library, int resource_id)
{
    LoadSurface(weapon_effects, library, resource_id);
    weapon_effects->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws a panel background bitmap onto the shared panel surface; every side panel loads into it.
void InterfaceGraphics::LoadPanelBackground(int library, int resource_id)
{
    LoadSurface(panel_background, library, resource_id);
}

// Draws the map loading box bitmap onto the loading icon surface.
void InterfaceGraphics::LoadMapLoadingBox(int library, int resource_id)
{
    LoadSurface(loading_icon, library, resource_id);
}

// Draws the interface clock background bitmap onto its surface.
void InterfaceGraphics::LoadClockBackground(int library, int resource_id)
{
    LoadSurface(clock_background, library, resource_id);
}

// Draws the interface group icons bitmap onto its surface.
void InterfaceGraphics::LoadGroupIcons(int library, int resource_id)
{
    LoadSurface(group_icons, library, resource_id);
}

// Draws the interface tooltip background bitmap onto its surface.
void InterfaceGraphics::LoadTooltipBackground(int library, int resource_id)
{
    LoadSurface(tooltip_background, library, resource_id);
}

// Draws the interface chat input background bitmap onto its surface.
void InterfaceGraphics::LoadChatInputBackground(int library, int resource_id)
{
    LoadSurface(chat_input_background, library, resource_id);
}

// Draws the interface small icons bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadSmallIcons(int library, int resource_id)
{
    LoadSurface(small_icons, library, resource_id);
    small_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface HUD sheet bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadHudSheet(int library, int resource_id)
{
    LoadSurface(hud_sheet, library, resource_id);
    hud_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface status icons bitmap onto its surface.
void InterfaceGraphics::LoadStatusIcons(int library, int resource_id)
{
    LoadSurface(status_icons, library, resource_id);
}

// Draws the interface chat tabs bitmap onto its surface.
void InterfaceGraphics::LoadChatTabs(int library, int resource_id)
{
    LoadSurface(chat_tabs, library, resource_id);
}

// Draws the interface list icons bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadListIcons(int library, int resource_id)
{
    LoadSurface(list_icons, library, resource_id);
    list_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the interface corner button bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadCornerButton(int library, int resource_id)
{
    LoadSurface(corner_button, library, resource_id);
    corner_button->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void InterfaceGraphics::LoadSmallButtons(int library, int resource_id)
{
    LoadSurface(small_buttons, library, resource_id);
}

void InterfaceGraphics::LoadInventoryButtons(int library, int resource_id)
{
    LoadSurface(inventory_buttons, library, resource_id);
}

// Draws the interface minimap tiles bitmap onto its surface.
void InterfaceGraphics::LoadMinimapTiles(int library, int resource_id)
{
    LoadSurface(minimap_tiles, library, resource_id);
}

// Draws the interface chat mode icons bitmap onto its surface.
void InterfaceGraphics::LoadChatModeIcons(int library, int resource_id)
{
    LoadSurface(chat_mode_icons, library, resource_id);
}

// Draws the interface HUD background bitmap onto its surface.
void InterfaceGraphics::LoadHudBackground(int library, int resource_id)
{
    LoadSurface(hud_background, library, resource_id);
}

// Draws the interface tab buttons bitmap onto its surface.
void InterfaceGraphics::LoadTabButtons(int library, int resource_id)
{
    LoadSurface(tab_buttons, library, resource_id);
}

// Draws the interface tile cursor bitmap onto its surface, with black see-through.
void InterfaceGraphics::LoadTileCursor(int library, int resource_id)
{
    LoadSurface(tile_cursor, library, resource_id);
    tile_cursor->SetColorKey(DDCKEY_SRCBLT, color_key);
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
            TCanvas *canvas = new TCanvas();
            canvas->Handle = dc;
            canvas->Pen->Style = psClear;
            canvas->Brush->Color = (TColor)0xC0C0C;
            rect.left = 0;
            rect.top = 0;
            rect.right = 442;
            rect.bottom = 99;
            canvas->FillRect(rect);
        }
        catch (Exception &)
        {
        }
        chat_panel_surface->ReleaseDC(dc);
    }
}

// Draws the interface control sheet bitmap onto its surface.
void InterfaceGraphics::LoadControlSheet(int library, int resource_id)
{
    LoadSurface(control_sheet, library, resource_id);
}

// Draws the interface scroll icons bitmap onto its surface.
void InterfaceGraphics::LoadScrollIcons(int library, int resource_id)
{
    LoadSurface(scroll_icons, library, resource_id);
}

void InterfaceGraphics::LoadElementIcons(int library, int resource_id)
{
    LoadSurfaceAt(element_icons, library, resource_id, 17, 47);
    element_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void InterfaceGraphics::LoadPopupIcons(int library, int resource_id)
{
    LoadSurface(popup_icons, library, resource_id);
    popup_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

void InterfaceGraphics::LoadHudIcons(int library, int resource_id)
{
    LoadSurface(hud_icons, library, resource_id);
    hud_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

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
// A mode the character may not use, such as admin, guild or global chat, is refused with a message.
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
    // A global message on the restricted map, or while global messages are not allowed, clears the input.
    else if (input_text[1] == '!')
        mode = 2;
    else if (input_text[1] == '"' || String(input_text[1]) == "'")
        mode = 4;
    else if (input_text[1] == '$')
    {
        int admin_level = MainForm->net_client->GetAdminLevel();
        if (MainForm->net_client->GetAdminLevel() > 1)
            mode = 8;
    }
    else if (input_text[1] == '@')
    {
        int admin_level_check = MainForm->net_client->GetAdminLevel();
        if (MainForm->net_client->GetAdminLevel() > 1)
            mode = 3;
    }
    else if (input_text[1] == '+')
    {
        if (MainForm->net_client->GetAdminLevel() > 0)
            mode = 5;
    }
    else if (input_text[1] == '&')
    {
        if (MainForm->animation->interface_controller->status->IsInGuild())
            mode = 7;
    }
    else if (input_text[1] == '~')
    {
        if (MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_YouCannotUseGlobal));
            MainForm->edits->SetText(EditId_Chat, "");
            mode = 0;
        }
        else if (MainForm->animation->IsGlobalMessageAllowed())
            mode = 3;
        else
        {
            MainForm->edits->SetText(EditId_Chat, "");
            mode = 0;
        }
    }
    else
        mode = 1;
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
        MainForm->OffscreenSurface->BltFast(590, 328, MainForm->gfx->interface_graphics->corner_button,
            &MainForm->gfx->interface_graphics->GetCornerButtonRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(590, 328, MainForm->gfx->interface_graphics->corner_button,
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
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel;
    int title_width =
        (short)MainForm->fonts->TextWidth(IntToStr(MainForm->animation->interface_controller->player_list->shown_names->Count));
    MainForm->fonts->DrawString(580 - (short)title_width, 332,
        IntToStr(MainForm->animation->interface_controller->player_list->shown_names->Count));
    int top_line = MainForm->animation->interface_controller->player_list->GetTopLine();
    int line_count = MainForm->animation->interface_controller->player_list->shown_names->Count;
    row = 0;
    MainForm->fonts->color = MainForm->themes->color_panel_list;
    for (int i = top_line; i < line_count && i < top_line + 7; i++)
    {
        MainForm->fonts->DrawString(124, (short)(14 * row) + 350,
            MainForm->animation->interface_controller->player_list->shown_names->Strings[i]);
        MainForm->fonts->DrawString(261, (short)(14 * row) + 350,
            MainForm->animation->interface_controller->player_list->shown_second_texts->Strings[i]);
        MainForm->fonts->DrawString(405, (short)(14 * row) + 350,
            MainForm->animation->interface_controller->player_list->shown_guild_tags->Strings[i]);
        MainForm->fonts->DrawString(460, (short)(14 * row) + 350,
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
    int private_index;
    int local_top;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    // A tab's label is lit while open, and marked while it holds unread lines.
    if (MainForm->animation->interface_controller->chat->active_tab == 1)
        MainForm->fonts->color = MainForm->themes->color_chat_tab_active;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[0])
        MainForm->fonts->color = MainForm->themes->color_chat_tab_highlight;
    else
        MainForm->fonts->color = MainForm->themes->color_chat_tab;
    MainForm->fonts->DrawString(141, 434,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->chat->first_private_name));
    if (MainForm->animation->interface_controller->chat->active_tab == 2)
        MainForm->fonts->color = MainForm->themes->color_chat_tab_active;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[1])
        MainForm->fonts->color = MainForm->themes->color_chat_tab_highlight;
    else
        MainForm->fonts->color = MainForm->themes->color_chat_tab;
    MainForm->fonts->DrawString(273, 434,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->chat->second_private_name));
    if (MainForm->animation->interface_controller->chat->active_tab == 3)
        MainForm->fonts->color = MainForm->themes->color_chat_tab_active;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[2])
        MainForm->fonts->color = MainForm->themes->color_chat_tab_highlight;
    else
        MainForm->fonts->color = MainForm->themes->color_chat_tab;
    MainForm->fonts->DrawString(406, 434, "scr");
    if (MainForm->animation->interface_controller->chat->active_tab == 4)
        MainForm->fonts->color = MainForm->themes->color_chat_tab_active;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[3])
        MainForm->fonts->color = MainForm->themes->color_chat_tab_highlight;
    else
        MainForm->fonts->color = MainForm->themes->color_chat_tab;
    MainForm->fonts->DrawString(450, 434, "glb");
    if (MainForm->animation->interface_controller->chat->active_tab == 5)
        MainForm->fonts->color = MainForm->themes->color_chat_tab_active;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[4])
        MainForm->fonts->color = MainForm->themes->color_chat_tab_highlight;
    else
        MainForm->fonts->color = MainForm->themes->color_chat_tab;
    MainForm->fonts->DrawString(494, 434, "grp");
    if (MainForm->animation->interface_controller->chat->active_tab == 6)
        MainForm->fonts->color = MainForm->themes->color_chat_tab_active;
    else if (MainForm->animation->interface_controller->chat->tab_has_unread[5])
        MainForm->fonts->color = MainForm->themes->color_chat_tab_highlight;
    else
        MainForm->fonts->color = MainForm->themes->color_chat_tab;
    MainForm->fonts->DrawString(538, 434, "sys");
    // Tabs 1 and 2 are the private ones; each tab draws its own lines from its scroll offset.
    if (MainForm->animation->interface_controller->chat->active_tab <= 2)
    {
        private_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        private_line_count =
            MainForm->animation->interface_controller->chat->private_lines[MainForm->animation->interface_controller->chat->active_tab]->Count;
        private_row = 0;
        for (private_index = private_top_line;
            private_index < private_line_count && private_top_line + 7 > private_index; private_index++)
        {
            String private_color =
                MainForm->animation->interface_controller->chat->last_private_tab_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[private_index];
            MainForm->fonts->color = GetCodeColor(private_color);
            MainForm->fonts->DrawString((short)StrToInt(MainForm->animation->interface_controller->chat->last_private_colors[MainForm->animation->interface_controller->chat->active_tab]->Strings[private_index]) +
                124, (short)(14 * private_row) + 332,
                MainForm->animation->interface_controller->chat->private_lines[MainForm->animation->interface_controller->chat->active_tab]->Strings[private_index]);
            private_row++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 3)
    {
        local_top = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int local_line_count = MainForm->animation->interface_controller->chat->local_lines->Count;
        int local_row = 0;
        int j;
        for (j = local_top; j < local_line_count && local_top + 7 > j; j++)
        {
            String local_color = MainForm->animation->interface_controller->chat->local_colors->Strings[j];
            MainForm->fonts->color = GetCodeColor(local_color);
            MainForm->fonts->DrawString((short)StrToInt(MainForm->animation->interface_controller->chat->local_indents->Strings[j]) +
                124, (short)(14 * local_row) + 332,
                MainForm->animation->interface_controller->chat->local_lines->Strings[j]);
            local_row++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 4)
    {
        int global_top_line;
        global_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int global_line_count = MainForm->animation->interface_controller->chat->global_lines->Count;
        int global_row = 0;
        int k;
        for (k = global_top_line; k < global_line_count && global_top_line + 7 > k; k++)
        {
            String global_color = MainForm->animation->interface_controller->chat->global_colors->Strings[k];
            MainForm->fonts->color = GetCodeColor(global_color);
            MainForm->fonts->DrawString((short)StrToInt(MainForm->animation->interface_controller->chat->global_indents->Strings[k]) +
                124, (short)(14 * global_row) + 332,
                MainForm->animation->interface_controller->chat->global_lines->Strings[k]);
            global_row++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 5)
    {
        int group_top_line;
        group_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int group_line_count = MainForm->animation->interface_controller->chat->group_lines->Count;
        int group_row = 0;
        int m;
        for (m = group_top_line; m < group_line_count && group_top_line + 7 > m; m++)
        {
            String group_color = MainForm->animation->interface_controller->chat->group_colors->Strings[m];
            MainForm->fonts->color = GetCodeColor(group_color);
            MainForm->fonts->DrawString((short)StrToInt(MainForm->animation->interface_controller->chat->private_lines[0]->Strings[m]) +
                124, (short)(14 * group_row) + 332,
                MainForm->animation->interface_controller->chat->group_lines->Strings[m]);
            group_row++;
        }
    }
    if (MainForm->animation->interface_controller->chat->active_tab == 6)
    {
        int system_top_line;
        system_top_line = MainForm->animation->interface_controller->chat->GetScrollOffset();
        int system_line_count = MainForm->animation->interface_controller->chat->system_lines->Count;
        int system_row = 0;
        int n;
        for (n = system_top_line; n < system_line_count && system_top_line + 7 > n; n++)
        {
            String system_color = MainForm->animation->interface_controller->chat->system_colors->Strings[n];
            MainForm->fonts->color = GetCodeColor(system_color);
            MainForm->fonts->DrawString(124, (short)(14 * system_row) + 332,
                MainForm->animation->interface_controller->chat->system_lines->Strings[n]);
            system_row++;
        }
    }
}

// Draws the status panel's values: the character's name and numbers, and the karma as a word from Demonic to Pure.
void InterfaceGraphics::DrawStatusValues()
{
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel;
    MainForm->fonts->DrawString(153, 338, IntToStr(MainForm->animation->interface_controller->status->stat_str));
    MainForm->fonts->DrawString(153, 356, IntToStr(MainForm->animation->interface_controller->status->stat_int));
    MainForm->fonts->DrawString(153, 374, IntToStr(MainForm->animation->interface_controller->status->stat_wis));
    MainForm->fonts->DrawString(153, 392, IntToStr(MainForm->animation->interface_controller->status->stat_agi));
    MainForm->fonts->DrawString(153, 410, IntToStr(MainForm->animation->interface_controller->status->stat_con));
    MainForm->fonts->DrawString(153, 428, IntToStr(MainForm->animation->interface_controller->status->stat_cha));
    MainForm->fonts->DrawString(261, 338,
        IntToStr(MainForm->animation->interface_controller->status->health) + " / " +
            IntToStr(MainForm->animation->interface_controller->status->max_health));
    MainForm->fonts->DrawString(261, 356,
        IntToStr(MainForm->animation->interface_controller->status->mana) + " / " +
            IntToStr(MainForm->animation->interface_controller->status->max_mana));
    MainForm->fonts->DrawString(261, 374,
        IntToStr(MainForm->animation->interface_controller->status->damage_min) + "-" +
            IntToStr(MainForm->animation->interface_controller->status->damage_max));
    MainForm->fonts->DrawString(261, 392, IntToStr(MainForm->animation->interface_controller->status->hitrate));
    MainForm->fonts->DrawString(261, 410, IntToStr(MainForm->animation->interface_controller->status->armor));
    MainForm->fonts->DrawString(261, 428, IntToStr(MainForm->animation->interface_controller->status->dodge));
    MainForm->fonts->DrawString(382, 338,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->status->character_name));
    MainForm->fonts->DrawString(382, 356,
        MainForm->net_client->Capitalize(MainForm->animation->interface_controller->status->guild_name));
    String weight_text = IntToStr(MainForm->animation->interface_controller->status->weight);
    weight_text = weight_text + " / ";
    weight_text = weight_text + IntToStr(MainForm->animation->interface_controller->status->max_weight);
    MainForm->fonts->DrawString(382, 374, weight_text);
    MainForm->fonts->DrawString(382, 392, IntToStr(MainForm->animation->interface_controller->status->stat_points));
    MainForm->fonts->DrawString(382, 410, IntToStr(MainForm->animation->interface_controller->status->skill_points));
    MainForm->fonts->DrawString(482, 374, IntToStr(MainForm->animation->interface_controller->status->gold));
    MainForm->fonts->DrawString(482, 392, IntToStr(MainForm->animation->interface_controller->status->experience));
    MainForm->fonts->DrawString(482, 410,
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
    MainForm->fonts->DrawString(482, 428, karma_text);
    MainForm->fonts->DrawString(554, 338, IntToStr(MainForm->animation->interface_controller->status->level));
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
                MainForm->OffscreenSurface->BltFast(460, 14 * row + 351, group_icons, &GetGroupIconRect(2),
                    DDBLTFAST_WAIT);
            else
                MainForm->OffscreenSurface->BltFast(460, 14 * row + 351, group_icons, &GetGroupIconRect(0),
                    DDBLTFAST_WAIT);
        }
        else
            MainForm->OffscreenSurface->BltFast(460, 14 * row + 351, group_icons, &GetGroupIconRect(1), DDBLTFAST_WAIT);
        row++;
    }
}

// Draws the health bar of each visible group member, its colour changing at 40 and 20 percent.
void InterfaceGraphics::DrawGroupHealthBars(int dc)
{
    int j;
    int top_row;
    int member_count;
    TRect rc;
    TCanvas *canvas = new TCanvas();
    canvas->Handle = (void *)dc;
    canvas->Pen->Color = clGreen;
    canvas->Pen->Style = psSolid;
    top_row = MainForm->animation->interface_controller->group->GetTopRow();
    member_count = MainForm->animation->interface_controller->group->members.size();
    int row = 0;
    for (j = top_row; j < member_count && j < top_row + 7; j++)
    {
        if (MainForm->animation->interface_controller->group->members[j].health_percent > 40)
            canvas->Brush->Color = (TColor)33340;
        if (MainForm->animation->interface_controller->group->members[j].health_percent <= 40)
        {
            if (MainForm->animation->interface_controller->group->members[j].health_percent > 20)
                canvas->Brush->Color = (TColor)42186;
        }
        if (MainForm->animation->interface_controller->group->members[j].health_percent <= 20)
            canvas->Brush->Color = (TColor)0x3264FF;
        canvas->Brush->Style = bsSolid;
        rc.left = 351;
        rc.right = MainForm->animation->interface_controller->group->members[j].health_percent + 351;
        rc.top = 14 * row + 352;
        rc.bottom = 14 * row + 360;
        canvas->FillRect(rc);
        row++;
    }
    row = 0;
    canvas->Brush->Color = clBlack;
    canvas->Brush->Style = bsClear;
    canvas->Pen->Color = clBlack;
    canvas->Pen->Style = psSolid;
    j = top_row;
    while (j < member_count && j < top_row + 7)
    {
        TPoint outline[9];
        outline[0].x = 350;
        outline[0].y = 14 * row + 353;
        outline[1].x = 352;
        outline[1].y = 14 * row + 351;
        outline[2].x = 449;
        outline[2].y = 14 * row + 351;
        outline[3].x = 451;
        outline[3].y = 14 * row + 353;
        outline[4].x = 451;
        outline[4].y = 14 * row + 358;
        outline[5].x = 449;
        outline[5].y = 14 * row + 360;
        outline[6].x = 352;
        outline[6].y = 14 * row + 360;
        outline[7].x = 350;
        outline[7].y = 14 * row + 358;
        outline[8].x = 350;
        outline[8].y = 14 * row + 353;
        canvas->Polyline(outline, 8);
        row++;
        j++;
    }
}

// Draws the member count, and each visible member's name, level and health label.
void InterfaceGraphics::DrawGroupText()
{
    int row;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel;
    int count_width =
        (short)MainForm->fonts->TextWidth(IntToStr(MainForm->animation->interface_controller->group->members.size()));
    MainForm->fonts->DrawString(580 - (short)count_width, 332,
        IntToStr(MainForm->animation->interface_controller->group->members.size()));
    int top_row = MainForm->animation->interface_controller->group->GetTopRow();
    int member_count = MainForm->animation->interface_controller->group->members.size();
    row = 0;
    MainForm->fonts->color = MainForm->themes->color_panel_list;
    for (int i = top_row; i < member_count && i < top_row + 7; i++)
    {
        MainForm->fonts->DrawString(124, (short)(14 * row) + 350,
            MainForm->animation->interface_controller->group->members[i].name);
        MainForm->fonts->DrawString(266, (short)(14 * row) + 350,
            IntToStr(MainForm->animation->interface_controller->group->members[i].level));
        MainForm->fonts->DrawString(325, (short)(14 * row) + 350, "HP");
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
    if (MainForm->animation->interface_controller->settings->hovered_option == SettingOption_GameHelp)
        MainForm->OffscreenSurface->BltFast(556, 330, small_buttons, &GetSmallButtonRect(1), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(556, 330, small_buttons, &GetSmallButtonRect(0), DDBLTFAST_WAIT);
}

// Draws the value of each settings row: the sound and music volumes, the keyboard layout and language, and each
// switch's on or off.
void InterfaceGraphics::DrawSettingsText()
{
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel;
    if (MainForm->config->GetSoundVolume() >= 100)
        MainForm->fonts->DrawString(222, 356, MainForm->strings->GetText(GameText_Enabled));
    else if (MainForm->config->GetSoundVolume() <= 0)
        MainForm->fonts->DrawString(222, 356, MainForm->strings->GetText(GameText_Disabled));
    else
        MainForm->fonts->DrawString(222, 356, IntToStr(MainForm->config->GetSoundVolume()) + "% vol.");
    if (MainForm->config->GetMusicVolume() >= 100)
        MainForm->fonts->DrawString(222, 374, MainForm->strings->GetText(GameText_Enabled));
    else if (MainForm->config->GetMusicVolume() <= 0)
        MainForm->fonts->DrawString(222, 374, MainForm->strings->GetText(GameText_Disabled));
    else
        MainForm->fonts->DrawString(222, 374, IntToStr(MainForm->config->GetMusicVolume()) + "% vol.");
    MainForm->fonts->DrawString(222, 392, MainForm->strings->GetText(MainForm->config->GetKeyboardLayout() + 254));
    int text_id = GameText_English;
    MainForm->fonts->DrawString(222, 410, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetAcceptPrivateMessages())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->fonts->DrawString(222, 428, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetShowBalloons())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->fonts->DrawString(462, 356, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetShowShadows())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->fonts->DrawString(462, 374, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetFilterEnabled())
    {
        if (MainForm->config->GetStrictFilter())
            text_id = GameText_Exclusive;
        else
            text_id = GameText_Normal;
    }
    else
        text_id = GameText_Disabled;
    MainForm->fonts->DrawString(462, 392, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetLogChat())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->fonts->DrawString(462, 410, MainForm->strings->GetText(text_id));
    if (MainForm->config->GetAcceptRequests())
        text_id = GameText_Enabled;
    else
        text_id = GameText_Disabled;
    MainForm->fonts->DrawString(462, 428, MainForm->strings->GetText(text_id));
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
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel_list;
    int top_line = MainForm->animation->interface_controller->help->GetTopLine();
    int line_count = MainForm->animation->interface_controller->help->lines->Count;
    int row = 0;
    for (int i = top_line; i < line_count && i < top_line + 7; i++)
    {
        if (MainForm->animation->interface_controller->help->line_flags->Strings[i] == "1")
            MainForm->fonts->is_underlined = true;
        else if (MainForm->animation->interface_controller->help->link_targets->Strings[i] != "")
            MainForm->fonts->is_underlined = true;
        else
            MainForm->fonts->is_underlined = false;
        MainForm->fonts->DrawString(124, (short)(14 * row) + 350,
            MainForm->animation->interface_controller->help->lines->Strings[i]);
        row++;
    }
    MainForm->fonts->is_underlined = false;
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

// Draws the seven visible news lines.
void InterfaceGraphics::DrawNewsText()
{
    int row;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel_list;
    int top_line = MainForm->animation->interface_controller->news->GetTopLine();
    int line_count = MainForm->animation->interface_controller->news->lines->Count;
    row = 0;
    for (int i = top_line; i < line_count && i < top_line + 7; i++)
    {
        MainForm->fonts->DrawString(124, (short)(14 * row) + 350,
            MainForm->animation->interface_controller->news->lines->Strings[i]);
        row++;
    }
}

// Draws the inventory's two buttons, each lit while the mouse is on it.
void InterfaceGraphics::DrawInventoryButtons()
{
    if (MainForm->animation->interface_controller->inventory->left_button_hovered)
        MainForm->OffscreenSurface->BltFast(558, 391, MainForm->gfx->interface_graphics->inventory_buttons,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Left, ButtonState_Hover),
            DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(558, 391, MainForm->gfx->interface_graphics->inventory_buttons,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Left, ButtonState_Normal),
            DDBLTFAST_WAIT);
    if (MainForm->animation->interface_controller->inventory->right_button_hovered)
        MainForm->OffscreenSurface->BltFast(558, 418, MainForm->gfx->interface_graphics->inventory_buttons,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Right, ButtonState_Hover),
            DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(558, 418, MainForm->gfx->interface_graphics->inventory_buttons,
            &MainForm->gfx->interface_graphics->GetInventoryButtonRect(InventoryButton_Right, ButtonState_Normal),
            DDBLTFAST_WAIT);
}

// Draws the chosen spell's name, cut to fit with two dots, and its level, the spell tip while hovering, and the skill
// points left.
void InterfaceGraphics::DrawSpellInfoText()
{
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel;
    if (MainForm->animation->interface_controller->spells->selected_spell_id >= 0 &&
        MainForm->animation->interface_controller->spells->selected_spell_icon >= 0)
    {
        String spell_name = MainForm->animation->interface_controller->spells->selected_spell_name;
        char truncated = false;
        while ((short)MainForm->fonts->TextWidth(spell_name) > INFO_NAME_MAX_WIDTH)
        {
            spell_name.Delete(spell_name.Length(), 1);
            truncated = true;
        }
        if (truncated)
            spell_name = spell_name + "..";
        int value = 150 - (short)MainForm->fonts->TextWidth(spell_name) / 2;
        MainForm->fonts->DrawString(value, 378, spell_name);
        if (MainForm->animation->interface_controller->spells->selected_spell_level >= 0 &&
            MainForm->animation->interface_controller->spells->selected_spell_level <= 100)
            MainForm->fonts->DrawString(134, 408,
                IntToStr(MainForm->animation->interface_controller->spells->selected_spell_level));
    }
    else
        MainForm->fonts->DrawString(134, 408, "0");
    if (MainForm->animation->interface_controller->spells->has_hover &&
        !MainForm->animation->interface_controller->spells->is_dragging)
        MainForm->animation->interface_controller->spells->tooltip->DrawString();
    MainForm->fonts->DrawString(134, 426, IntToStr(MainForm->animation->interface_controller->status->skill_points));
}

// Draws the chosen skill's name, cut to fit with two dots, and its level, and the skill points left.
void InterfaceGraphics::DrawSkillInfoText()
{
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = MainForm->themes->color_panel;
    if (MainForm->animation->interface_controller->skills->selected_skill_id >= 0 &&
        MainForm->animation->interface_controller->skills->selected_skill_icon >= 0)
    {
        String skill_name = MainForm->animation->interface_controller->skills->selected_skill_name;
        char truncated = false;
        while ((short)MainForm->fonts->TextWidth(skill_name) > INFO_NAME_MAX_WIDTH)
        {
            skill_name.Delete(skill_name.Length(), 1);
            truncated = true;
        }
        if (truncated)
            skill_name = skill_name + "..";
        int value = 150 - (short)MainForm->fonts->TextWidth(skill_name) / 2;
        MainForm->fonts->DrawString(value, 378, skill_name);
        if (MainForm->animation->interface_controller->skills->selected_skill_level >= 0 &&
            MainForm->animation->interface_controller->skills->selected_skill_level <= 100)
            MainForm->fonts->DrawString(134, 408,
                IntToStr(MainForm->animation->interface_controller->skills->selected_skill_level));
    }
    else
        MainForm->fonts->DrawString(134, 408, "0");
    MainForm->fonts->DrawString(134, 426, IntToStr(MainForm->animation->interface_controller->status->skill_points));
}

// Draws the spell panel: scroll bar, each spell's icon in its cell with its level bar, the hovered and selected
// spells, the blinking raise button while skill points are left, and the spell tip.
void InterfaceGraphics::DrawSpellPanel()
{
    IconWrapper *selected_icon;
    int blink_on;
    vector<SpellItem>::iterator it;
    SkillValue *skill;
    IconWrapper *hover_icon;
    IconWrapper *icon;
    int info_icon_id;
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
        info_icon_id = MainForm->animation->interface_controller->spells->selected_spell_icon;
        selected_icon = MainForm->gfx->icons->GetGraphic(info_icon_id);
        if (selected_icon)
            MainForm->OffscreenSurface->BltFast(134, 344, selected_icon->resource->surface,
                &selected_icon->resource->GetIconRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
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
        skill = MainForm->skill_values->GetSkill(it->spell_id - 1);
        if (MainForm->gfx->icons->RequestGraphic(skill->icon))
        {
            if (it->is_visible)
            {
                int slot_column = it->grid_x;
                int slot_row = it->grid_y - MainForm->animation->interface_controller->spells->GetTopRow();
                if (slot_row == 0 || slot_row == 1)
                {
                    if (MainForm->animation->interface_controller->spells->hover_col == slot_column &&
                        MainForm->animation->interface_controller->spells->hover_row == slot_row)
                    {
                        hover_icon = MainForm->gfx->icons->GetGraphic(skill->icon);
                        if (hover_icon)
                            MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                                hover_icon->resource->surface, &hover_icon->resource->GetIconRect(1),
                                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    }
                    else
                    {
                        icon = MainForm->gfx->icons->GetGraphic(skill->icon);
                        if (icon)
                            MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                                icon->resource->surface, &icon->resource->GetIconRect(0),
                                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    }
                    if (it->level > 0 && it->level <= 100)
                        MainForm->blitter->HalfBlendBlt(45 * slot_column + 206, 52 * slot_row + 379,
                            MainForm->OffscreenSurface, control_sheet, &GetSpellProgressRect(it->level));
                }
            }
            else
                MainForm->gfx->icons->GetGraphic(skill->icon);
        }
        else
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
    }
    if (MainForm->animation->interface_controller->spells->has_hover &&
        !MainForm->animation->interface_controller->spells->is_dragging)
        MainForm->animation->interface_controller->spells->tooltip->DrawFrame();
}

// Draws the skill panel: scroll bar, each skill's icon in its cell with its level bar, the hovered and selected
// skills, and the blinking raise button while skill points are left.
void InterfaceGraphics::DrawSkillPanel()
{
    IconWrapper *selected_icon;
    int blink_on;
    vector<SkillItem>::iterator it;
    SkillValue *skill;
    IconWrapper *hover_icon;
    IconWrapper *icon;
    int info_icon_id;
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
        info_icon_id = MainForm->animation->interface_controller->skills->selected_skill_icon;
        selected_icon = MainForm->gfx->icons->GetGraphic(info_icon_id);
        if (selected_icon)
            MainForm->OffscreenSurface->BltFast(134, 344, selected_icon->resource->surface,
                &selected_icon->resource->GetIconRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        // With skill points left, the raise button blinks by the selected skill.
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
        skill = MainForm->skill_values->GetSkill(it->skill_id - 1);
        if (MainForm->gfx->icons->RequestGraphic(skill->icon))
        {
            if (it->is_visible)
            {
                int slot_column = it->grid_x;
                int slot_row = it->grid_y - MainForm->animation->interface_controller->skills->GetTopRow();
                if (slot_row == 0 || slot_row == 1)
                {
                    if (MainForm->animation->interface_controller->skills->hover_col == slot_column &&
                        MainForm->animation->interface_controller->skills->hover_row == slot_row)
                    {
                        hover_icon = MainForm->gfx->icons->GetGraphic(skill->icon);
                        if (hover_icon)
                            MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                                hover_icon->resource->surface, &hover_icon->resource->GetIconRect(1),
                                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    }
                    else
                    {
                        icon = MainForm->gfx->icons->GetGraphic(skill->icon);
                        if (icon)
                            MainForm->OffscreenSurface->BltFast(45 * slot_column + 207, 52 * slot_row + 341,
                                icon->resource->surface, &icon->resource->GetIconRect(0),
                                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    }
                    if (it->level > 0 && it->level <= 100)
                        MainForm->blitter->HalfBlendBlt(45 * slot_column + 206, 52 * slot_row + 379,
                            MainForm->OffscreenSurface, control_sheet, &GetSpellProgressRect(it->level));
                }
            }
            else
                MainForm->gfx->icons->GetGraphic(skill->icon);
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
    SubItemWrapper *graphic_item;
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
            MainForm->animation->interface_controller->inventory->hover_col + last_col <= 17 &&
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
            {
                graphic_item = MainForm->gfx->items->GetSubItemGraphic(graphic_index);
                if (graphic_item)
                    MainForm->OffscreenSurface->BltFast(26 * it->grid_x + 114, 26 * it->grid_y + 339,
                        graphic_item->resource->surface, &graphic_item->resource->surface_rect,
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            }
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
        // The dragged picture is kept on the screen.
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
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->interface_controller->inventory->drag_graphic)->resource->surface_rect;
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
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->interface_controller->inventory->drag_graphic)->resource->surface,
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
        // What lies under the icon is saved first, to be put back when it moves.
        drag_save_surface->BltFast(0, 0, MainForm->OffscreenSurface, &drag_rect, DDBLTFAST_WAIT);
        if (MainForm->gfx->icons->RequestGraphic(MainForm->animation->interface_controller->spells->drag_icon))
        {
            TRect source_rect =
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->spells->drag_icon)->resource->GetIconRect(1);
            draw_x = MainForm->animation->interface_controller->spells->drag_x - icon_half_width;
            draw_y = MainForm->animation->interface_controller->spells->drag_y - icon_half_height;
            if (draw_x < 0)
            {
                source_rect.left += 0 - draw_x;
                draw_x = 0;
            }
            if (draw_y < 0)
            {
                source_rect.top = 0 - draw_y;
                draw_y = 0;
            }
            if (source_rect.right - source_rect.left + draw_x > 640)
                source_rect.right -= source_rect.right - source_rect.left + draw_x - 640;
            if (draw_y + source_rect.bottom > 480)
                source_rect.bottom -= draw_y + source_rect.bottom - 480;
            MainForm->blitter->HalfBlendBlt(draw_x, draw_y, MainForm->OffscreenSurface,
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->spells->drag_icon)->resource->surface,
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
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->skills->drag_icon)->resource->GetIconRect(1);
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
                MainForm->gfx->icons->GetGraphic(MainForm->animation->interface_controller->skills->drag_icon)->resource->surface,
                &source_rect);
        }
    }
}

// Draws the radar in the corner the settings pick: the tiles around the character with walls, doors and other
// features, the other players and the npcs by kind, and the coordinates when asked.
void InterfaceGraphics::DrawMinimap()
{
    GfxController *gfx;
    NpcValue *npc_value;
    NpcValue *mark_npc_value;
    MapController *map = MainForm->map;
    gfx = MainForm->gfx;
    if (!gfx->interface_graphics->minimap_visible)
        return;
    if (map->IsMinimapDisabled())
        return;
    int player_index = 0;
    int npc_index = 0;
    int origin_x = 104;
    int origin_y = 192;
    // 0 = bottom left, 1 = bottom right, 2 = top left, 3 = top right, or top left while a boss bar shows
    if (MainForm->config->GetRadarPosition() == 1)
        origin_x = 508;
    if (MainForm->config->GetRadarPosition() == 2)
        origin_y = 10;
    if (MainForm->config->GetRadarPosition() == 3)
    {
        if (MainForm->animation->boss_bars->IsAnyVisible())
            origin_y = 10;
        else
        {
            origin_x = 508;
            origin_y = 10;
        }
    }
    short radius = 11;
    Player self = MainForm->players->GetPlayerById(MainForm->net_client->GetPlayerId());
    int first_x = self.x - radius;
    int first_y = self.y - radius;
    // The tiles within the radius around the player, half blended over the view.
    for (int row = 0; 2 * radius >= row; row++)
    {
        for (int col = 0; 2 * radius >= col; col++)
        {
            int x = col + first_x;
            int y = row + first_y;
            int screen_x = 4 * col - 4 * row + origin_x;
            int screen_y = 2 * col + 2 * row + origin_y;
            int dot_x = screen_x + 1;
            int dot_y = screen_y + 1;
            if (2 * radius == row)
                gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetStatusIconLowRect(1, 0));
            if (2 * radius == col)
                gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetStatusIconLowRect(1, 1));
            char off_map = false;
            if (x < 0)
                off_map = true;
            if (y < 0)
                off_map = true;
            if (x > map->GetWidth())
                off_map = true;
            if (map->GetHeight() < y)
                off_map = true;
            if (map->spec_grid[x][y] == 18)
                off_map = true;
            if (off_map)
                gfx->game->BlitClippedAlpha(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetStatusIconLowRect(3, 0), 60);
            else
            {
                if ((int)MainForm->players->GetCount() > player_index)
                {
                    char player_searching;
                    do
                    {
                        player_searching = false;
                        Player player = MainForm->players->GetPlayer(player_index);
                        if (x == player.x && player.y == y)
                        {
                            if (MainForm->net_client->GetPlayerId() != player.player_id && !player.hidden)
                                gfx->game->BlitClippedHalfBlend(screen_x, screen_y,
                                    gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetStatusIconLowRect(0, 5));
                            player_searching = true;
                            player_index++;
                        }
                    } while ((int)MainForm->players->GetCount() > player_index && player_searching);
                }
                if (map->edge_bottom[x][y])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(1, 0));
                if (map->edge_top[x][y])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(0, 0));
                if (map->edge_left[x][y])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(0, 1));
                if (map->edge_right[x][y])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(1, 1));
                unsigned char spec = map->spec_grid[x][y];
                if (map->blocked[x][y])
                {
                    if (spec > MapTileSpec_FakeWall && spec < MapTileSpec_Jump)
                        gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetStatusIconLowRect(0, 8));
                    else if (spec != 18)
                        gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetStatusIconLowRect(0, 3));
                }
                else if (spec == MapTileSpec_FakeWall)
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(0, 3));
                else if (spec == MapTileSpec_Water)
                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(1, 10));
                else if (spec > MapTileSpec_AmbientSource && spec < MapTileSpec_HiddenSpikes)
                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(1, 9));
                if (map->has_tile_spec[x][y])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(0, 3));
                if (map->minimap_mask[x][y] >= 1 && map->minimap_mask[x][y] <= 2 &&
                    (!x || !y || x == map->GetWidth() || map->GetHeight() == y))
                {
                    int icon = map->minimap_mask[x][y] - 1;
                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetStatusIconLowRect(1, icon + 2));
                }
                if ((int)MainForm->npcs->GetCount() > npc_index)
                {
                    char npc_searching;
                    do
                    {
                        npc_searching = false;
                        Npc npc = MainForm->npcs->GetNpc(npc_index);
                        if (x == npc.x && npc.y == y)
                        {
                            if (npc.graphic != NpcGraphic_Wraith)
                            {
                                npc_value = MainForm->npc_values->GetNpc(npc.npc_id - 1);
                                if (npc_value->npc_type > NpcType_Friendly && npc_value->npc_type < NpcType_Shop)
                                {
                                    if (npc_value->child)
                                        gfx->game->BlitClippedAlpha(dot_x, dot_y,
                                            gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetStatusIconLowRect(0, 6), 160);
                                    else if (npc_value->boss >= 2)
                                        gfx->game->BlitClippedAlpha(dot_x, dot_y,
                                            gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetStatusIconLowRect(0, 10), 160);
                                    else if (npc_value->boss == 1)
                                        gfx->game->BlitClippedAlpha(dot_x, dot_y,
                                            gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetStatusIconLowRect(0, 6), 160);
                                    else if (npc_value->npc_type == NpcType_Untargetable)
                                        gfx->game->BlitClippedAlpha(dot_x, dot_y,
                                            gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetStatusIconLowRect(0, 8), 160);
                                    else
                                        gfx->game->BlitClippedAlpha(dot_x, dot_y,
                                            gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetStatusIconLowRect(0, 6), 160);
                                }
                                else if (npc_value->npc_type == NpcType_Shop)
                                    gfx->game->BlitClippedAlpha(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetStatusIconLowRect(0, 9), 160);
                                else if (npc_value->npc_type == NpcType_Bank)
                                    gfx->game->BlitClippedAlpha(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetStatusIconLowRect(0, 5), 160);
                                else if (npc_value->npc_type == NpcType_Quest)
                                    gfx->game->BlitClippedAlpha(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetStatusIconLowRect(0, 4), 160);
                                else
                                    gfx->game->BlitClippedAlpha(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetStatusIconLowRect(0, 7), 160);
                            }
                            npc_searching = true;
                            npc_index++;
                        }
                    } while ((int)MainForm->npcs->GetCount() > npc_index && npc_searching);
                }
            }
        }
    }
    gfx->game->BlitClippedHalfBlend(origin_x - 1, 4 * radius + origin_y - 2, gfx->interface_graphics->minimap_tiles,
        GetMarkerRect(2, self.direction));
    // Without the icons setting the radar shows only the tiles and the player's own marker.
    if (!MainForm->config->GetMinimapIcons())
        return;
    player_index = 0;
    npc_index = 0;
    for (int mark_row = 0; 2 * radius >= mark_row; mark_row++)
    {
        for (int mark_col = 0; 2 * radius >= mark_col; mark_col++)
        {
            int mark_tile_x = mark_col + first_x;
            int mark_tile_y = mark_row + first_y;
            int x = 4 * mark_col - 4 * mark_row + origin_x;
            int mark_y = 2 * mark_col + 2 * mark_row + origin_y - 2;
            if (mark_tile_x >= 0 && mark_tile_y >= 0 && map->GetWidth() >= mark_tile_x &&
                map->GetHeight() >= mark_tile_y)
            {
                if ((int)MainForm->players->GetCount() > player_index)
                {
                    char mark_player_searching;
                    do
                    {
                        mark_player_searching = false;
                        Player mark_player = MainForm->players->GetPlayer(player_index);
                        if (mark_player.x == mark_tile_x && mark_player.y == mark_tile_y)
                        {
                            if (MainForm->net_client->GetPlayerId() != mark_player.player_id && !mark_player.hidden)
                            {
                                if (mark_player.badge_level > 0)
                                    gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetMinimapMarkRect(2, 10), 150);
                                else
                                    gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetMinimapMarkRect(2, 4), 150);
                            }
                            mark_player_searching = true;
                            player_index++;
                        }
                    } while ((int)MainForm->players->GetCount() > player_index && mark_player_searching);
                }
                unsigned char mark_spec = map->spec_grid[mark_tile_x][mark_tile_y];
                if (map->has_tile_spec[mark_tile_x][mark_tile_y])
                {
                    if (!mark_spec)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 5), 220);
                    else if (mark_spec == 1 || mark_spec == 2)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 6), 220);
                    else if (mark_spec == 2 || mark_spec == 3)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 7), 220);
                    else if (mark_spec == 5 || mark_spec == 7)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 6), 220);
                    else if (mark_spec == 6)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 7), 220);
                    else if (mark_spec == 9)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 4), 220);
                    else if (mark_spec == MapTileSpec_BankVault)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(1, 8), 220);
                    else if (mark_spec == MapTileSpec_Jukebox)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(2, 0), 220);
                    else if (mark_spec > MapTileSpec_FakeWall && mark_spec < MapTileSpec_Jukebox)
                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapMarkRect(2, 2), 220);
                }
                else if (mark_spec == MapTileSpec_Arena)
                    gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapMarkRect(2, 1), 220);
                if (map->minimap_mask[mark_tile_x][mark_tile_y] == 3)
                    gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapMarkRect(2, 3), 220);
                if ((int)MainForm->npcs->GetCount() > npc_index)
                {
                    char mark_npc_searching;
                    do
                    {
                        mark_npc_searching = false;
                        Npc mark_npc = MainForm->npcs->GetNpc(npc_index);
                        if (mark_npc.x == mark_tile_x && mark_npc.y == mark_tile_y)
                        {
                            if (mark_npc.graphic != NpcGraphic_Wraith)
                            {
                                mark_npc_value = MainForm->npc_values->GetNpc(mark_npc.npc_id - 1);
                                if (mark_npc_value->npc_type > NpcType_Friendly &&
                                    mark_npc_value->npc_type < NpcType_Shop)
                                {
                                    if (mark_npc_value->child)
                                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetMinimapMarkRect(2, 7), 150);
                                    else if (mark_npc_value->boss >= 2)
                                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetMinimapMarkRect(2, 9), 150);
                                    else if (mark_npc_value->boss == 1)
                                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetMinimapMarkRect(2, 8), 150);
                                    else if (mark_npc_value->npc_type == NpcType_Untargetable)
                                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetMinimapMarkRect(3, 8), 150);
                                    else
                                        gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                            gfx->interface_graphics->GetMinimapMarkRect(2, 5), 150);
                                }
                                else
                                    gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetMinimapMarkRect(2, 6), 150);
                            }
                            mark_npc_searching = true;
                            npc_index++;
                        }
                    } while ((int)MainForm->npcs->GetCount() > npc_index && mark_npc_searching);
                }
                if (radius == mark_col && radius == mark_row)
                    gfx->game->BlitClippedAlpha(x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapMarkRect(3, 1), 210);
            }
        }
    }
    HDC dc;
    // With the coordinates setting on, the player's position is written by the radar.
    if (MainForm->config->GetRadarCoords() && MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            TCanvas *canvas = new TCanvas();
            canvas->Handle = dc;
            String coords = IntToStr(self.x) + "," + IntToStr(self.y);
            canvas->Brush->Color = clWhite;
            canvas->Brush->Style = bsClear;
            canvas->Font->Name = "Arial";
            canvas->Font->Size = 6;
            canvas->Font->Color = (TColor)0xC8C8C8;
            canvas->TextOut(origin_x + 42, origin_y + 86, coords);
            canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
}

// Draws the whole map over the view, centered on the character: walls, doors and other features, the grid when set,
// the other players and the npcs by kind.
void InterfaceGraphics::DrawFullMinimap()
{
    GfxController *gfx;
    int mark_x;
    int mark_y;
    NpcValue *npc_value;
    unsigned short npc_type;
    unsigned char mark_spec;
    NpcValue *mark_npc_value;
    MapController *map;
    int player_index;
    int npc_index;
    int origin_x;
    int origin_y;
    map = MainForm->map;
    gfx = MainForm->gfx;
    if (!gfx->interface_graphics->minimap_visible)
        return;
    if (map->IsMinimapDisabled())
        return;
    player_index = 0;
    npc_index = 0;
    // Each tile is a diamond of 28 by 14 pixels, placed so the view centre sits mid-screen.
    origin_x = 0 - 14 * map->GetCenterX() + 14 * map->GetCenterY();
    origin_y = 0 - 7 * map->GetCenterX() - 7 * map->GetCenterY();
    origin_x -= 24;
    origin_y -= 8;
    for (int row = 0; map->GetHeight() >= row; row++)
    {
        int col = 0;
        while (map->GetWidth() >= col)
        {
            int screen_x = 14 * col - 14 * row + origin_x + 320;
            int screen_y = 7 * col + 7 * row + origin_y + 148;
            screen_x = screen_x + 2 * map->GetScrollShiftX();
            screen_y += 2 * map->GetScrollShiftY();
            int dot_x = screen_x + 1;
            int dot_y = screen_y + 1;
            if ((int)MainForm->players->GetCount() > player_index)
            {
                char player_searching;
                do
                {
                    player_searching = false;
                    Player player = MainForm->players->GetPlayer(player_index);
                    if (player.x == col && player.y == row)
                    {
                        if (MainForm->net_client->GetPlayerId() != player.player_id)
                        {
                            if (!player.hidden)
                                gfx->game->BlitClippedHalfBlend(screen_x, screen_y,
                                    gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconInnerRect(3, player.direction + 4));
                        }
                        else if (MainForm->config->GetMinimapGrid())
                            gfx->game->BlitClippedHalfBlend(screen_x + 2, screen_y + 1,
                                gfx->interface_graphics->minimap_tiles,
                                gfx->interface_graphics->GetListIconInnerRect(3, player.direction));
                        else
                            gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                                gfx->interface_graphics->GetListIconInnerRect(3, player.direction));
                        player_searching = true;
                        player_index++;
                    }
                } while ((int)MainForm->players->GetCount() > player_index && player_searching);
            }
            unsigned char spec = map->spec_grid[col][row];
            if (MainForm->config->GetMinimapGrid())
            {
                if (spec != MapTileSpec_Edge)
                {
                    if (map->edge_top[col][row])
                        gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapTileRect(0, 1));
                    else if (map->edge_left[col][row])
                        gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapTileRect(0, 0));
                    else
                        gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                            gfx->interface_graphics->GetMinimapTileRect(0, 2));
                }
            }
            else
            {
                if (map->edge_bottom[col][row])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapTileRect(1, 0));
                if (map->edge_top[col][row])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapTileRect(0, 0));
                if (map->edge_left[col][row])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapTileRect(0, 1));
                if (map->edge_right[col][row])
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapTileRect(1, 1));
            }
            if (map->blocked[col][row])
            {
                if (spec > MapTileSpec_FakeWall && spec < MapTileSpec_Jump)
                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconInnerRect(0, 8));
                else if (spec != MapTileSpec_Edge)
                    gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetMinimapTileRect(0, 3));
            }
            else if (spec == MapTileSpec_FakeWall)
                gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetMinimapTileRect(0, 3));
            else if (spec == MapTileSpec_Water)
                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetListIconInnerRect(1, 10));
            else if (spec > MapTileSpec_AmbientSource && spec < MapTileSpec_HiddenSpikes)
                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetListIconInnerRect(1, 9));
            if (map->has_tile_spec[col][row])
                gfx->game->BlitClippedHalfBlend(screen_x, screen_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetMinimapTileRect(0, 3));
            if (map->minimap_mask[col][row] >= 1 && map->minimap_mask[col][row] <= 2 &&
                (!col || !row || map->GetWidth() == col || map->GetHeight() == row))
            {
                int icon = map->minimap_mask[col][row] - 1;
                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetListIconInnerRect(1, icon + 2));
            }
            if ((int)MainForm->npcs->GetCount() > npc_index)
            {
                char npc_searching;
                do
                {
                    npc_searching = false;
                    Npc npc = MainForm->npcs->GetNpc(npc_index);
                    if (npc.x == col && npc.y == row)
                    {
                        if (npc.graphic != NpcGraphic_Wraith)
                        {
                            npc_value = MainForm->npc_values->GetNpc(npc.npc_id - 1);
                            npc_type = npc_value->npc_type;
                            if (npc_type > NpcType_Friendly && npc_type < NpcType_Shop)
                            {
                                if (npc_value->child)
                                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y,
                                        gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconInnerRect(4, npc.direction));
                                else if (npc_value->boss >= 2)
                                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y,
                                        gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconInnerRect(4, npc.direction + 4));
                                else if (npc_value->boss == 1)
                                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y,
                                        gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconInnerRect(4, npc.direction));
                                else if (npc_value->npc_type == NpcType_Untargetable)
                                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y,
                                        gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconInnerRect(0, 8));
                                else
                                    gfx->game->BlitClippedHalfBlend(dot_x, dot_y,
                                        gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconInnerRect(4, npc.direction));
                            }
                            else if (npc_value->npc_type == NpcType_Shop)
                                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconInnerRect(0, 9));
                            else if (npc_value->npc_type == NpcType_Bank)
                                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconInnerRect(0, 5));
                            else if (npc_value->npc_type == NpcType_Quest)
                                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconInnerRect(0, 4));
                            else
                                gfx->game->BlitClippedHalfBlend(dot_x, dot_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconInnerRect(0, 7));
                        }
                        npc_searching = true;
                        npc_index++;
                    }
                } while ((int)MainForm->npcs->GetCount() > npc_index && npc_searching);
            }
            col++;
        }
    }
    // The marks of players and npcs follow, when the minimap icons setting is on.
    if (!MainForm->config->GetMinimapIcons())
        return;
    player_index = 0;
    npc_index = 0;
    for (int mark_row = 0; map->GetHeight() >= mark_row; mark_row++)
    {
        int mark_col = 0;
        while (map->GetWidth() >= mark_col)
        {
            mark_x = 14 * mark_col - 14 * mark_row + origin_x + 328;
            mark_y = 7 * mark_col + 7 * mark_row + origin_y + 142;
            mark_x = mark_x + 2 * map->GetScrollShiftX();
            mark_y = mark_y + 2 * map->GetScrollShiftY();
            if ((int)MainForm->players->GetCount() > player_index)
            {
                char mark_player_searching;
                do
                {
                    mark_player_searching = false;
                    Player mark_player = MainForm->players->GetPlayer(player_index);
                    if (mark_player.x == mark_col && mark_player.y == mark_row)
                    {
                        if (MainForm->net_client->GetPlayerId() != mark_player.player_id && !mark_player.hidden)
                        {
                            if (mark_player.badge_level > 0)
                                gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconMarkRect(2, 10), 150);
                            else
                                gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconMarkRect(2, 4), 150);
                        }
                        mark_player_searching = true;
                        player_index++;
                    }
                } while ((int)MainForm->players->GetCount() > player_index && mark_player_searching);
            }
            mark_spec = (char)map->spec_grid[mark_col][mark_row];
            if (map->has_tile_spec[mark_col][mark_row])
            {
                if (!mark_spec)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 5), 220);
                else if (mark_spec == 1 || mark_spec == 2)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 6), 220);
                else if (mark_spec == 2 || mark_spec == 3)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 7), 220);
                else if (mark_spec == 5 || mark_spec == 7)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 6), 220);
                else if (mark_spec == 6)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 7), 220);
                else if (mark_spec == 9)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 4), 220);
                else if (mark_spec == MapTileSpec_BankVault)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(1, 8), 220);
                else if (mark_spec == MapTileSpec_Jukebox)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(2, 0), 220);
                else if (mark_spec > MapTileSpec_FakeWall && mark_spec < MapTileSpec_Jukebox)
                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                        gfx->interface_graphics->GetListIconMarkRect(2, 2), 220);
            }
            else if (mark_spec == MapTileSpec_Arena)
                gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetListIconMarkRect(2, 1), 220);
            if (map->minimap_mask[mark_col][mark_row] == 3)
                gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                    gfx->interface_graphics->GetListIconMarkRect(2, 3), 220);
            if ((int)MainForm->npcs->GetCount() > npc_index)
            {
                char mark_npc_searching;
                do
                {
                    mark_npc_searching = false;
                    Npc mark_npc = MainForm->npcs->GetNpc(npc_index);
                    if (mark_npc.x == mark_col && mark_npc.y == mark_row)
                    {
                        if (mark_npc.graphic != NpcGraphic_Wraith)
                        {
                            mark_npc_value = MainForm->npc_values->GetNpc(mark_npc.npc_id - 1);
                            if (mark_npc_value->npc_type > NpcType_Friendly && mark_npc_value->npc_type < NpcType_Shop)
                            {
                                if (mark_npc_value->child)
                                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconMarkRect(2, 7), 150);
                                else if (mark_npc_value->boss >= 2)
                                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconMarkRect(2, 9), 150);
                                else if (mark_npc_value->boss == 1)
                                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconMarkRect(2, 8), 150);
                                else if (mark_npc_value->npc_type == NpcType_Untargetable)
                                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconMarkRect(3, 8), 150);
                                else
                                    gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                        gfx->interface_graphics->GetListIconMarkRect(2, 5), 150);
                            }
                            else
                                gfx->game->BlitClippedAlpha(mark_x, mark_y, gfx->interface_graphics->minimap_tiles,
                                    gfx->interface_graphics->GetListIconMarkRect(2, 6), 150);
                        }
                        mark_npc_searching = true;
                        npc_index++;
                    }
                } while ((int)MainForm->npcs->GetCount() > npc_index && mark_npc_searching);
            }
            mark_col++;
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
        MainForm->fonts->face = 1;
        MainForm->fonts->size = 8;
        MainForm->fonts->color = MainForm->themes->color_top_bar;
        if (MainForm->animation->interface_controller->game_bars->show_health_text)
            MainForm->fonts->DrawString(104, 14,
                IntToStr(MainForm->animation->interface_controller->status->health) + "/" +
                    IntToStr(MainForm->animation->interface_controller->status->max_health));
        if (MainForm->animation->interface_controller->game_bars->show_mana_text)
            MainForm->fonts->DrawString(213, 14,
                IntToStr(MainForm->animation->interface_controller->status->mana) + "/" +
                    IntToStr(MainForm->animation->interface_controller->status->max_mana));
        if (MainForm->animation->interface_controller->game_bars->show_stamina_text)
            MainForm->fonts->DrawString(324, 14,
                IntToStr(MainForm->animation->interface_controller->status->stamina) + "/" +
                    IntToStr(MainForm->animation->interface_controller->status->max_stamina));
        if (MainForm->animation->interface_controller->game_bars->show_experience_text)
            MainForm->fonts->DrawString(433, 14,
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
