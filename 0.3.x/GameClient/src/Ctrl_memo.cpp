#include <vcl.h>
#pragma hdrstop

#include "Ctrl_memo.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_law.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_shop.h"
#include "Ctrl_trade.h"
#include "Ctrl_crafter.h"
#include "Ctrl_tooltip.h"
#include "Gfx_interface.h"
#include "Gfx_mapitems.h"
#include "Gfx_mapnpcs.h"
#include "Gfx_npc.h"
#include "Gfx_subitem.h"

#pragma package(smart_init)

// Creates the memo's preview surface and empty lists for its lines and values.
MemoController::MemoController()
{
    preview_surface = new TDx_DrawSurface(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 223;
    surface_desc->Height = 104;
    preview_surface->GetSurfaceDesc(surface_desc);
    preview_surface->Create(surface_desc, MainForm->Dx_Draw1);
    preview_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    preview_rect.top = 0;
    preview_rect.left = 0;
    preview_rect.right = 223;
    preview_rect.bottom = 104;
    lines = new TStringList();
    line_values = new TStringList();
    item_entries = new TStringList();
    window_x = 49;
    window_y = 15;
    unused_flag = true;
    unused_value = 0;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    scroll_changed = false;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_line = -1;
    is_visible = false;
    close_hovered = false;
    scroll_up_held = false;
    scroll_down_held = false;
    is_dragging = false;
    tooltip = new Tooltip(76, 50, 2);
    tooltip->SetFlipEdge(630, 63);
}

MemoController::~MemoController()
{
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
}

// Adds a packed item list as name and amount lines, sorted by name.
void MemoController::AddItemList(String item_data, int record_size, int amount_size)
{
    item_entries->Clear();
    int record_count = item_data.Length() / record_size;
    int id_size = record_size - amount_size;
    for (int i = 0; i < record_count; i++)
    {
        int item_id = MainForm->net_client->DecodeNumber(item_data.SubString(i * record_size + 1, id_size));
        int amount = MainForm->net_client->DecodeNumber(item_data.SubString(i * record_size + 3, amount_size));
        String listed_item_name = MainForm->item_values->GetName(item_id);
        item_entries->Add(listed_item_name + "#" + IntToStr(amount));
    }
    // Each entry is the name, a hash sign and the amount, so the sort goes by name
    item_entries->Sort();
    for (int j = 0; j < item_entries->Count; j++)
    {
        int hash_pos = item_entries->Strings[j].Pos("#");
        String item_name = item_entries->Strings[j].SubString(1, hash_pos - 1);
        String amount_text =
            item_entries->Strings[j].SubString(hash_pos + 1, item_entries->Strings[j].Length() - hash_pos);
        AddText(item_name, amount_text, false);
    }
}

// Adds a character's stats block from the server: level, map and position, the current and maximum values, the
// base stats and the other numbers, in groups split by blank lines.
void MemoController::AddCharacterStats(String stats_data)
{
    int experience = MainForm->net_client->DecodeNumber(stats_data.SubString(1, 4));
    int character_level = MainForm->net_client->DecodeNumber(stats_data.SubString(5, 1));
    int map_id = MainForm->net_client->DecodeNumber(stats_data.SubString(6, 2));
    int map_x = MainForm->net_client->DecodeNumber(stats_data.SubString(8, 2));
    int map_y = MainForm->net_client->DecodeNumber(stats_data.SubString(10, 2));
    int hitpoints = MainForm->net_client->DecodeNumber(stats_data.SubString(12, 2));
    int max_hitpoints = MainForm->net_client->DecodeNumber(stats_data.SubString(14, 2));
    int techpoints = MainForm->net_client->DecodeNumber(stats_data.SubString(16, 2));
    int max_techpoints = MainForm->net_client->DecodeNumber(stats_data.SubString(18, 2));
    int str_value = MainForm->net_client->DecodeNumber(stats_data.SubString(20, 2));
    int int_value = MainForm->net_client->DecodeNumber(stats_data.SubString(22, 2));
    int wis_value = MainForm->net_client->DecodeNumber(stats_data.SubString(24, 2));
    int agi_value = MainForm->net_client->DecodeNumber(stats_data.SubString(26, 2));
    int con_value = MainForm->net_client->DecodeNumber(stats_data.SubString(28, 2));
    int cha_value = MainForm->net_client->DecodeNumber(stats_data.SubString(30, 2));
    int damage_max = MainForm->net_client->DecodeNumber(stats_data.SubString(32, 2));
    int damage_min = MainForm->net_client->DecodeNumber(stats_data.SubString(34, 2));
    int hitrate = MainForm->net_client->DecodeNumber(stats_data.SubString(36, 2));
    int dodge = MainForm->net_client->DecodeNumber(stats_data.SubString(38, 2));
    int armor = MainForm->net_client->DecodeNumber(stats_data.SubString(40, 2));
    int light_element = MainForm->net_client->DecodeNumber(stats_data.SubString(42, 2));
    int dark_element = MainForm->net_client->DecodeNumber(stats_data.SubString(44, 2));
    int fire_element = MainForm->net_client->DecodeNumber(stats_data.SubString(46, 2));
    int water_element = MainForm->net_client->DecodeNumber(stats_data.SubString(48, 2));
    int earth_element = MainForm->net_client->DecodeNumber(stats_data.SubString(50, 2));
    int wind_element = MainForm->net_client->DecodeNumber(stats_data.SubString(52, 2));
    int weight = MainForm->net_client->DecodeNumber(stats_data.SubString(54, 1));
    int max_weight = MainForm->net_client->DecodeNumber(stats_data.SubString(55, 1));
    int spent_points = 0;
    int total_points = 0;
    AddText(MainForm->strings->GetText(GameText_Level), IntToStr(experience) + " [" + IntToStr(character_level) + "]",
        false);
    AddText(MainForm->strings->GetText(GameText_Location), IntToStr(map_id), false);
    AddText(MainForm->strings->GetText(GameText_Coordinates), IntToStr(map_x) + "," + IntToStr(map_y), false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_Hitpoints), IntToStr(hitpoints) + "/" + IntToStr(max_hitpoints),
        false);
    AddText(MainForm->strings->GetText(GameText_Techpoints), IntToStr(techpoints) + "/" + IntToStr(max_techpoints),
        false);
    AddText("specpoints", IntToStr(spent_points) + "/" + IntToStr(total_points), false);
    AddText(MainForm->strings->GetText(GameText_Weight), IntToStr(weight) + "/" + IntToStr(max_weight),
        false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_Damage), IntToStr(damage_min) + "-" + IntToStr(damage_max),
        false);
    AddText(MainForm->strings->GetText(GameText_Hitrate), IntToStr(hitrate), false);
    AddText(MainForm->strings->GetText(GameText_Armor), IntToStr(armor), false);
    AddText(MainForm->strings->GetText(GameText_Dodge), IntToStr(dodge), false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_Strength), IntToStr(str_value), false);
    AddText(MainForm->strings->GetText(GameText_Intelligence), IntToStr(int_value), false);
    AddText(MainForm->strings->GetText(GameText_Wisdom), IntToStr(wis_value), false);
    AddText(MainForm->strings->GetText(GameText_Agility), IntToStr(agi_value), false);
    AddText(MainForm->strings->GetText(GameText_Constitution), IntToStr(con_value), false);
    AddText(MainForm->strings->GetText(GameText_Charisma), IntToStr(cha_value), false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_LightEle), IntToStr(light_element), false);
    AddText(MainForm->strings->GetText(GameText_DarkEle), IntToStr(dark_element), false);
    AddText(MainForm->strings->GetText(GameText_FireEle), IntToStr(fire_element), false);
    AddText(MainForm->strings->GetText(GameText_WaterEle), IntToStr(water_element), false);
    AddText(MainForm->strings->GetText(GameText_EarthEle), IntToStr(earth_element), false);
    AddText(MainForm->strings->GetText(GameText_WindEle), IntToStr(wind_element), false);
    AddText("devine ele.", "-", false);
}

// Adds a text with its right-hand value, wrapped to the page width; each wrapped line repeats the value.
void MemoController::AddText(String text, String right_text, bool add_blank_line)
{
    int max_width = 200;
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            do
            {
                String fitted_text;
                if ((short)MainForm->fonts->TextWidth(text) <= max_width)
                {
                    lines->Add(text);
                    line_values->Add(right_text);
                    text = "";
                }
                else
                {
                    char split_done = false;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] == ' ' || i == text.Length())
                        {
                            String prefix = text.SubString(1, i);
                            if ((short)MainForm->fonts->TextWidth(prefix) >= max_width)
                            {
                                lines->Add(fitted_text);
                                line_values->Add(right_text);
                                text.Delete(1, fitted_text.Length());
                                split_done = true;
                                break;
                            }
                            fitted_text = prefix;
                        }
                    }
                    if (!split_done)
                    {
                        lines->Add(text);
                        line_values->Add(right_text);
                        text = "";
                    }
                }
            } while (text.Length() > 0);
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    if (add_blank_line)
    {
        lines->Add("");
        line_values->Add("");
    }
    MainForm->redraw_needed = true;
}

// Shows the memo window empty and at the top, closing the other npc windows.
void MemoController::Show()
{
    MainForm->animation->crafter->is_visible = false;
    MainForm->animation->shop->is_visible = false;
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->learn->is_visible = false;
    MainForm->animation->memo->is_visible = false;
    MainForm->animation->inn->is_visible = false;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    lines->Clear();
    line_values->Clear();
    top_line = 0;
    scroll_thumb_offset = 0;
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Shows an item in the memo window: its picture, and its name, type and effect lines from its tip.
void MemoController::ShowItem(ItemValue *item)
{
    int i;
    SubItemWrapper *picture;
    int x;
    int y;
    TRect *source;
    TRect rect;
    if (!item)
        return;
    preview_surface->BltFast(0, 0, MainForm->gfx->interface_graphics->chat_panel_surface,
        &MainForm->gfx->interface_graphics->memo_preview_rect, DDBLTFAST_WAIT);
    int graphic = item->graphic - 1;
    if (MainForm->gfx->items->LoadSubItemGraphicNow(graphic))
    {
        picture = MainForm->gfx->items->GetSubItemGraphic(graphic);
        source = &picture->resource->surface_rect;
        rect = *source;
        x = 108 - picture->width / 2;
        y = 50 - picture->height / 2;
        preview_surface->BltFast(x, y, picture->resource->surface, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (item->name.Length() < 3)
        return;
    Show();
    tooltip->ShowItem(item->item_id);
    memo_kind = 1;
    preview_graphic = item->graphic;
    title = "Item : " + item->name;
    for (int row = 0; row < 7; row++)
        AddText("", "", false);
    for (i = 0; i < tooltip->line_count; i++)
    {
        if (!i)
            AddText("Name:", tooltip->lines[i], false);
        else if (i == 1)
            AddText("Type:", tooltip->lines[i], false);
        else
            AddText("Effect", tooltip->lines[i], false);
    }
}

// Shows an npc in the memo window: its picture, cut to fit, and its name, level, behaviour, experience, hit and tech
// points, damage, hit rate, evade and armor.
void MemoController::ShowNpc(NpcValue *npc)
{
    NpcWrapper *picture;
    NpcResource *resource;
    int y;
    int excess_height;
    int x;
    int trim[1];
    if (!npc)
        return;
    preview_surface->BltFast(0, 0, MainForm->gfx->interface_graphics->chat_panel_surface,
        &MainForm->gfx->interface_graphics->memo_preview_rect, DDBLTFAST_WAIT);
    int graphic = npc->graphic;
    if (MainForm->gfx->npc_cache->LoadGraphicNow(npc->npc_id, graphic))
    {
        picture = MainForm->gfx->npc_cache->FindGraphic(graphic);
        TRect rect = picture->resource->GetFrameRect(0);
        resource = picture->resource;
        x = 108 - resource->frame_width / 2;
        y = 55 - resource->frame_height / 2;
        if (y < 0)
            y = 0;
        // A picture taller than 88 shows 88 rows, lower down the taller it is; one wider than 200
        // is cut at both sides.
        if (picture->resource->frame_height > 88)
        {
            excess_height = picture->resource->frame_height - 88;
            rect.top = 0;
            rect.bottom = 88;
            if (excess_height > 50)
            {
                rect.top += 20;
                rect.bottom += 20;
            }
            if (excess_height > 100)
            {
                rect.top += 20;
                rect.bottom += 20;
            }
            if (excess_height > 150)
            {
                rect.top += 20;
                rect.bottom += 20;
            }
        }
        if (picture->resource->frame_width > 200)
        {
            trim[0] = (picture->resource->frame_width - 200) / 2;
            rect.left += trim[0];
            rect.right -= trim[0];
            x = 0;
        }
        preview_surface->BltFast(x, y, resource->front_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    // An npc whose name has fewer than three letters shows no memo.
    if (npc->name.Length() < 3)
        return;
    Show();
    memo_kind = 2;
    preview_graphic = npc->graphic;
    title = "Npc : " + npc->name + " - lvl " + String(npc->level);
    for (int row = 0; row < 7; row++)
        AddText("", "", false);
    AddText("Name:", npc->name, false);
    AddText("Behaviour:", BehaviourName(npc->npc_type), false);
    AddText("Experience", String(npc->experience) + " exp", false);
    AddText("", "", false);
    AddText("Hitpoints", String(npc->hp) + " hp", false);
    AddText("Techpoints", String((unsigned short)npc->tp) + " tp", false);
    AddText("", "", false);
    AddText("Damage",
        String((unsigned short)npc->min_damage) + " - " + String(String((unsigned short)npc->max_damage)) + " dmg",
        false);
    AddText("Hitrate", String((unsigned short)npc->accuracy) + " hit", false);
    AddText("Evade", String((unsigned short)npc->evade) + " ev", false);
    AddText("Armor", String((unsigned short)npc->armor) + " ac", false);
}

// The word the memo shows for an npc's behaviour number.
String MemoController::BehaviourName(int behaviour)
{
    if (!behaviour)
        return "friendly";
    if (behaviour == 1)
        return "passive";
    if (behaviour == 2)
        return "aggresive";
    if (behaviour == 6)
        return "shopkeeper";
    if (behaviour == 15)
        return "quest npc";
    return "other";
}

void MemoController::Open(String heading)
{
    Show();
    memo_kind = 0;
    title = heading;
}

// Hides the window and frees its graphics.
void MemoController::Close()
{
    is_visible = false;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Repeats a held scroll arrow.
DEFINE_TICK(MemoController)

int MemoController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int MemoController::GetTopLine()
{
    return top_line;
}

// A click on the scroll bar grabs its thumb or starts scrolling; any other click in the window starts moving it.
void MemoController::MouseDown(int x, int y, int button)
{
    if (x >= window_x + 252 && x <= window_x + 267)
    {
        if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
        {
            thumb_grabbed = true;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
    {
        scroll_up_held = true;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
    {
        scroll_down_held = true;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 290 && !thumb_grabbed)
    {
        is_dragging = true;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

DEFINE_MOUSE_MOVE(MemoController)

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
void MemoController::UpdateScrollHover(int x, int y)
{
    if (y >= window_y + 58 && y <= window_y + 229)
        DragScrollThumb(y);
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
    {
        if (!scroll_up_hovered)
        {
            scroll_up_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (scroll_up_hovered)
    {
        scroll_up_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
    {
        if (!scroll_down_hovered)
        {
            scroll_down_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (scroll_down_hovered)
    {
        scroll_down_hovered = false;
        MainForm->redraw_needed = true;
    }
}

DEFINE_MEMO_DRAG_SCROLL_THUMB(MemoController)

// Scrolls the page up one line and moves the scroll thumb to match.
DEFINE_MEMO_SCROLL_UP(MemoController)

// Scrolls the page down one line and moves the scroll thumb to match.
DEFINE_MEMO_SCROLL_DOWN(MemoController)

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void MemoController::MouseUp()
{
    is_dragging = false;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
}
