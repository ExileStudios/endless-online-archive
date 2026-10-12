#include <vcl.h>
#pragma hdrstop

#include "Ctrl_memo.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_law.h"
#pragma package(smart_init)

MemoController::MemoController()
{
    int new_list;
    int new_list_2;
    int new_list_3;
    lines = new TStringList();
    line_values = new TStringList();
    item_entries = new TStringList();
    window_x = 49;
    window_y = 15;
    unused_flag = 1;
    unused_value = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_line = -1;
    is_visible = 0;
    close_hovered = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
}

MemoController::~MemoController()
{
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
        String amount_text = item_entries->Strings[j].SubString(hash_pos + 1,
            item_entries->Strings[j].Length() - hash_pos);
        AddText(item_name, amount_text, false);
    }
}

// Adds a character's stats block from the server: level, map and position, the current and maximum values, the
// base stats and the other numbers, in groups split by blank lines.
void MemoController::AddCharacterStats(String stats_data)
{
    int level = MainForm->net_client->DecodeNumber(stats_data.SubString(1, 4));
    int label_340_bracket = MainForm->net_client->DecodeNumber(stats_data.SubString(5, 1));
    int map_id = MainForm->net_client->DecodeNumber(stats_data.SubString(6, 2));
    int map_x = MainForm->net_client->DecodeNumber(stats_data.SubString(8, 2));
    int map_y = MainForm->net_client->DecodeNumber(stats_data.SubString(10, 2));
    int label_356_current = MainForm->net_client->DecodeNumber(stats_data.SubString(12, 2));
    int label_356_max = MainForm->net_client->DecodeNumber(stats_data.SubString(14, 2));
    int label_357_current = MainForm->net_client->DecodeNumber(stats_data.SubString(16, 2));
    int label_357_max = MainForm->net_client->DecodeNumber(stats_data.SubString(18, 2));
    int str_value = MainForm->net_client->DecodeNumber(stats_data.SubString(20, 2));
    int int_value = MainForm->net_client->DecodeNumber(stats_data.SubString(22, 2));
    int wis_value = MainForm->net_client->DecodeNumber(stats_data.SubString(24, 2));
    int agi_value = MainForm->net_client->DecodeNumber(stats_data.SubString(26, 2));
    int con_value = MainForm->net_client->DecodeNumber(stats_data.SubString(28, 2));
    int cha_value = MainForm->net_client->DecodeNumber(stats_data.SubString(30, 2));
    int label_413_to = MainForm->net_client->DecodeNumber(stats_data.SubString(32, 2));
    int label_413_from = MainForm->net_client->DecodeNumber(stats_data.SubString(34, 2));
    int label_414_value = MainForm->net_client->DecodeNumber(stats_data.SubString(36, 2));
    int label_416_value = MainForm->net_client->DecodeNumber(stats_data.SubString(38, 2));
    int label_415_value = MainForm->net_client->DecodeNumber(stats_data.SubString(40, 2));
    int label_418_value = MainForm->net_client->DecodeNumber(stats_data.SubString(42, 2));
    int label_419_value = MainForm->net_client->DecodeNumber(stats_data.SubString(44, 2));
    int label_420_value = MainForm->net_client->DecodeNumber(stats_data.SubString(46, 2));
    int label_421_value = MainForm->net_client->DecodeNumber(stats_data.SubString(48, 2));
    int label_422_value = MainForm->net_client->DecodeNumber(stats_data.SubString(50, 2));
    int label_423_value = MainForm->net_client->DecodeNumber(stats_data.SubString(52, 2));
    int label_417_current = MainForm->net_client->DecodeNumber(stats_data.SubString(54, 1));
    int label_417_max = MainForm->net_client->DecodeNumber(stats_data.SubString(55, 1));
    AddText(MainForm->strings->GetText(GameText_Level), IntToStr(level) + " [" + IntToStr(label_340_bracket) + "]",
        false);
    AddText(MainForm->strings->GetText(GameText_Location), MapController::ReadMapName(MainForm->map, map_id), false);
    AddText(MainForm->strings->GetText(GameText_Coordinates), IntToStr(map_x) + "," + IntToStr(map_y), false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_Hitpoints), IntToStr(label_356_current) + "/" + IntToStr(label_356_max),
        false);
    AddText(MainForm->strings->GetText(GameText_Techpoints), IntToStr(label_357_current) + "/" +
        IntToStr(label_357_max), false);
    AddText(MainForm->strings->GetText(GameText_Weight), IntToStr(label_417_current) + "/" + IntToStr(label_417_max),
        false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_Damage), IntToStr(label_413_from) + "-" + IntToStr(label_413_to),
        false);
    AddText(MainForm->strings->GetText(GameText_Hitrate), IntToStr(label_414_value), false);
    AddText(MainForm->strings->GetText(GameText_Armor), IntToStr(label_415_value), false);
    AddText(MainForm->strings->GetText(GameText_Dodge), IntToStr(label_416_value), false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_Strength), IntToStr(str_value), false);
    AddText(MainForm->strings->GetText(GameText_Intelligence), IntToStr(int_value), false);
    AddText(MainForm->strings->GetText(GameText_Wisdom), IntToStr(wis_value), false);
    AddText(MainForm->strings->GetText(GameText_Agility), IntToStr(agi_value), false);
    AddText(MainForm->strings->GetText(GameText_Constitution), IntToStr(con_value), false);
    AddText(MainForm->strings->GetText(GameText_Charisma), IntToStr(cha_value), false);
    AddText(" ", "", false);
    AddText(MainForm->strings->GetText(GameText_LightEle), IntToStr(label_418_value), false);
    AddText(MainForm->strings->GetText(GameText_DarkEle), IntToStr(label_419_value), false);
    AddText(MainForm->strings->GetText(GameText_FireEle), IntToStr(label_420_value), false);
    AddText(MainForm->strings->GetText(GameText_WaterEle), IntToStr(label_421_value), false);
    AddText(MainForm->strings->GetText(GameText_EarthEle), IntToStr(label_422_value), false);
    AddText(MainForm->strings->GetText(GameText_WindEle), IntToStr(label_423_value), false);
}

// Adds a text with its right-hand value, wrapped to the page width; each wrapped line repeats the value.
void MemoController::AddText(String text, String right_text, bool add_blank_line)
{
    int max_width;
    HDC dc;
    max_width = 200;
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->text_canvas->Handle = dc;
            MainForm->text_canvas->Font->Name = "MS Sans Serif";
            MainForm->text_canvas->Font->Size = 7;
            do
            {
                String fitted_text;
                if (MainForm->text_canvas->TextWidth(text) <= max_width)
                {
                    lines->Add(text);
                    line_values->Add(right_text);
                    text = "";
                }
                else
                {
                    char split_done = 0;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] != ' ' && i != text.Length())
                            continue;
                        String prefix = text.SubString(1, i);
                        if (MainForm->text_canvas->TextWidth(prefix) >= max_width)
                        {
                            lines->Add(fitted_text);
                            line_values->Add(right_text);
                            text.Delete(1, fitted_text.Length());
                            split_done = 1;
                            break;
                        }
                        fitted_text = prefix;
                    }
                    if (!split_done)
                    {
                        lines->Add(text);
                        line_values->Add(right_text);
                        text = "";
                    }
                }
            } while (text.Length() > 0);
            MainForm->text_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    if (add_blank_line)
    {
        lines->Add("");
        line_values->Add("");
    }
    MainForm->redraw_needed = true;
}

// Shows the memo window with the title and an empty page, closing other npc windows.
void MemoController::Open(String window_title)
{
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->learn->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    lines->Clear();
    line_values->Clear();
    top_line = 0;
    scroll_thumb_offset = 0;
    title = window_title;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void MemoController::Close()
{
    is_visible = 0;
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

// A click on the scroll bar, or anywhere else in the window to start moving it.
void MemoController::MouseDown(int x, int y, int button)
{
    if (x >= window_x + 252 && x <= window_x + 267)
    {
        if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
    {
        scroll_up_held = 1;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
    {
        scroll_down_held = 1;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 290 && !thumb_grabbed)
    {
        is_dragging = 1;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

DEFINE_MOUSE_MOVE(MemoController)

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(MemoController)

DEFINE_MEMO_DRAG_SCROLL_THUMB(MemoController)

// Scrolls the page up one line and moves the scroll thumb to match.
DEFINE_MEMO_SCROLL_UP(MemoController)

// Scrolls the page down one line and moves the scroll thumb to match.
DEFINE_MEMO_SCROLL_DOWN(MemoController)

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void MemoController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}
