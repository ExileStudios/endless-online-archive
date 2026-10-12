#include <vcl.h>
#pragma hdrstop

#include "Ctrl_chat.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Packetoperations.h"
#pragma package(smart_init)

#define MAX_CHAT_LINES 300
#define MAX_SYSTEM_LINES 100

#define MAX_WRAPPED_CHAT_LINES 3
#define LOG_CHANNEL_GLOBAL 2

ChatController::ChatController()
{
    active_tab = 3;
    auto_scroll = true;
    global_chat_open = false;
    private_name_1 = "";
    private_name_2 = "";
    scroll_up_hover = 0;
    scroll_down_hover = 0;
    scrollbar_dirty = false;
    text_dirty = 0;
    drunk_ticks = 0;
    system_lines = new TStringList();
    system_colors = new TStringList();
    system_icons = new TStringList();
    spare_line_list = new TStringList();
    local_lines = new TStringList();
    local_colors = new TStringList();
    local_icons = new TStringList();
    local_senders = new TStringList();
    local_indents = new TStringList();
    global_lines = new TStringList();
    global_colors = new TStringList();
    global_icons = new TStringList();
    global_senders = new TStringList();
    global_indents = new TStringList();
    group_lines = new TStringList();
    group_colors = new TStringList();
    group_icons = new TStringList();
    group_senders = new TStringList();
    private_lines[0] = new TStringList();
    private_lines[1] = new TStringList();
    private_lines[2] = new TStringList();
    private_colors[0] = new TStringList();
    private_colors[1] = new TStringList();
    private_indents[0] = new TStringList();
    private_indents[1] = new TStringList();
    for (int i = 0; i <= 5; i++)
    {
        scroll_offsets[i] = 0; scrollbar_positions[i] = 50; tab_has_unread[i] = 0;
    }
    scrolling_up = 0;
    scrolling_down = 0;
    is_dragging_thumb = 0;
}

// 0 or 1 for the private tab talking with the name (in any case), or -1.
int ChatController::FindPrivateTab(String name)
{
    int tab_index = -1;
    if (LowerCase(private_name_1) == LowerCase(name))
        tab_index = 0;
    if (LowerCase(private_name_2) == LowerCase(name))
        tab_index = 1;
    return tab_index;
}

// Opens a private tab for the name in the first free one of the two, unless one is open for it already.
void ChatController::OpenPrivateTab(String name)
{
    if (private_name_1 != name && private_name_2 != name)
    {
        if (private_name_1 == "")
        {
            private_name_1 = name;
            scrollbar_dirty = true;
            text_dirty = 1;
        }
        else
        {
            if (private_name_2 == "")
            {
                private_name_2 = name;
                scrollbar_dirty = true;
                text_dirty = 1;
            }
        }
    }
}

// Closes private tab 1 or 2 and shows the other one, or local chat when that one is closed too.
void ChatController::ClosePrivateTab(int tab)
{
    if (tab == 1)
    {
        private_name_1 = "";
        private_lines[1]->Clear();
        private_colors[0]->Clear();
        private_indents[0]->Clear();
        scroll_offsets[0] = 0; scrollbar_positions[0] = 50;
        if (private_name_2 != "")
            active_tab = 2;
        else
            active_tab = 3;
        scrollbar_dirty = true;
        text_dirty = 1;
    }
    if (tab == 2)
    {
        private_name_2 = "";
        private_lines[2]->Clear();
        private_colors[1]->Clear();
        private_indents[1]->Clear();
        scroll_offsets[1] = 0; scrollbar_positions[1] = 50;
        if (private_name_1 != "")
            active_tab = 1;
        else
            active_tab = 3;
        scrollbar_dirty = true;
        text_dirty = 1;
    }
}

// Empties every tab and closes the private ones.
void ChatController::ResetChat()
{
    active_tab = 3;
    auto_scroll = true;
    private_name_1 = "";
    private_name_2 = "";
    scroll_up_hover = 0;
    scroll_down_hover = 0;
    scrollbar_dirty = false;
    text_dirty = 0;
    system_lines->Clear();
    system_colors->Clear();
    system_icons->Clear();
    spare_line_list->Clear();
    local_lines->Clear();
    local_colors->Clear();
    local_icons->Clear();
    local_senders->Clear();
    local_indents->Clear();
    global_lines->Clear();
    global_colors->Clear();
    global_icons->Clear();
    global_senders->Clear();
    global_indents->Clear();
    group_lines->Clear();
    group_colors->Clear();
    group_icons->Clear();
    group_senders->Clear();
    private_lines[0]->Clear();
    private_lines[1]->Clear();
    private_lines[2]->Clear();
    private_colors[0]->Clear();
    private_colors[1]->Clear();
    private_indents[0]->Clear();
    private_indents[1]->Clear();
    for (int tab = 0; tab <= 5; tab++)
    {
        scroll_offsets[tab] = 0; scrollbar_positions[tab] = 50;
    }
    scrolling_up = 0;
    scrolling_down = 0;
}

ChatController::~ChatController()
{
}

int ChatController::GetScrollOffset()
{
    return scroll_offsets[active_tab - 1];
}

int ChatController::GetScrollbarPosition()
{
    return scrollbar_positions[active_tab - 1];
}

// While drunk, every fifth tick plays emote 11 four times in ten; also repeats a held scroll arrow.
void ChatController::OnTimer()
{
    if (drunk_ticks > 0 && !(--drunk_ticks % 5))
    {
        int r = random(10);
        if (r <= 3)
            MainForm->animation->game->PlayEmote(11);
    }
    if (scroll_repeat_delay > 0)
        scroll_repeat_delay--;
    else
    {
        if (scrolling_up)
            ScrollUp();
        if (scrolling_down)
            ScrollDown();
    }
}

// One line up in the open tab; the tab shows 7 lines and the thumb moves over 50 pixels.
void ChatController::ScrollUp()
{
    double offset_ratio;
    double thumb_position;
    int tab;
    int line_count;
    tab = active_tab;
    if (tab <= 2)
        line_count = private_lines[tab]->Count;
    if (active_tab == 3)
        line_count = local_lines->Count;
    if (active_tab == 4)
        line_count = global_lines->Count;
    if (active_tab == 5)
        line_count = group_lines->Count;
    if (active_tab == 6)
        line_count = system_lines->Count;
    if (line_count > 7 && scroll_offsets[active_tab - 1] > 0)
    {
        scroll_offsets[active_tab - 1]--;
        offset_ratio = (double)scroll_offsets[active_tab - 1] / (double)(line_count - 7);
        thumb_position = PANEL_THUMB_TRAVEL * offset_ratio;
        scrollbar_positions[active_tab - 1] = thumb_position;
        scrollbar_dirty = true;
        text_dirty = 1;
    }
}

// One line down in the open tab.
void ChatController::ScrollDown()
{
    double offset_ratio;
    double thumb_position;
    int tab;
    int line_count;
    tab = active_tab;
    if (tab <= 2)
        line_count = private_lines[tab]->Count;
    if (active_tab == 3)
        line_count = local_lines->Count;
    if (active_tab == 4)
        line_count = global_lines->Count;
    if (active_tab == 5)
        line_count = group_lines->Count;
    if (active_tab == 6)
        line_count = system_lines->Count;
    if (line_count - 7 > scroll_offsets[active_tab - 1])
    {
        scroll_offsets[active_tab - 1]++;
        offset_ratio = (double)scroll_offsets[active_tab - 1] / (double)(line_count - 7);
        thumb_position = PANEL_THUMB_TRAVEL * offset_ratio;
        scrollbar_positions[active_tab - 1] = thumb_position;
        scrollbar_dirty = true;
        text_dirty = 1;
    }
}

// Scrolls the open tab to where the thumb is dragged, 0 to 50 pixels down the bar.
void ChatController::DragScrollbar(int mouse_y)
{
    double thumb_ratio;
    double scaled_offset;
    int __slot1;
    int thumb_position;
    int line_count;
    if (is_dragging_thumb)
    {
        thumb_position = mouse_y - drag_grab_offset;
        if (thumb_position < 0)
            thumb_position = 0;
        if (thumb_position > 50)
            thumb_position = 50;
        if (active_tab <= 2)
            line_count = private_lines[active_tab]->Count;
        if (active_tab == 3)
            line_count = local_lines->Count;
        if (active_tab == 4)
            line_count = global_lines->Count;
        if (active_tab == 5)
            line_count = group_lines->Count;
        if (active_tab == 6)
            line_count = system_lines->Count;
        if (line_count > 7)
        {
            thumb_ratio = thumb_position / PANEL_THUMB_TRAVEL;
            scaled_offset = (line_count - 7.0) * thumb_ratio;
            line_count = scaled_offset;
            if (line_count != scroll_offsets[active_tab - 1])
                scroll_offsets[active_tab - 1] = line_count;
        }
        scrollbar_positions[active_tab - 1] = thumb_position;
        scrollbar_dirty = true;
        text_dirty = 1;
    }
}

// Adds a line to the system tab (the last 100 are kept) and writes it to the chat log.
void ChatController::AddSystemMessage(int icon, int color, String text, bool silent)
{
    if (system_lines->Count > MAX_SYSTEM_LINES)
    {
        do
        {
            system_lines->Delete(0);
            system_colors->Delete(0);
            system_icons->Delete(0);
        } while (system_lines->Count > MAX_SYSTEM_LINES);
    }
    system_lines->Add(text);
    system_colors->Add(IntToStr(color));
    system_icons->Add(IntToStr(icon));
    if (auto_scroll || !auto_scroll && scrollbar_positions[5] == 50)
    {
        if (system_lines->Count > 7)
            scroll_offsets[5] = system_lines->Count - 7;
        else
            scroll_offsets[5] = 0;
        scrollbar_positions[5] = 50;
    }
    else if (scroll_offsets[5] > 0 && system_lines->Count >= MAX_SYSTEM_LINES)
    {
        scroll_offsets[5]--;
        if (scroll_offsets[5] == 0)
            scrollbar_positions[5] = 0;
    }
    if (active_tab == 6)
        text_dirty = 1;
    else if (!tab_has_unread[5] && !silent)
    {
        tab_has_unread[5] = 1;
        text_dirty = 1;
    }
    MainForm->chat_log->WriteLine(3, text);
}

// Adds a message to a private tab, wrapped to 420 pixels with the later lines set in under the name.
void ChatController::AddPrivateMessage(int tab_index, int color, String sender, String text, bool silent)
{
    HDC dc;
    int line_start = 0;
    int j = 0;
    text = sender + "  " + text;
    if (private_lines[tab_index + 1]->Count > 100)
    {
        do
        {
            private_lines[tab_index + 1]->Delete(0);
            private_colors[tab_index]->Delete(0);
            private_indents[tab_index]->Delete(0);
        } while (private_lines[tab_index + 1]->Count > 100);
    }
    int wrap_width = 420;
    int line_count;
    int next_start;
    int name_width;
    line_count = 0;
    next_start = 1;
    String wrapped_lines[MAX_WRAPPED_CHAT_LINES];
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Style = bsClear;
            MainForm->draw_canvas->Font->Color = clBlack;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            name_width = MainForm->draw_canvas->TextWidth(Client::Capitalize(MainForm->net_client, sender) + "  ");
            if (MainForm->draw_canvas->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() < next_start)
                        continue;
                    line_count++;
                    line_start = next_start;
                    char searching = 1;
                    for (j = next_start; j <= text.Length() && searching; j++)
                    {
                        int indent = 0;
                        if (i > 0)
                            indent = name_width;
                        // A line ends at a space 40 pixels before the width, or at the width, with a hyphen past it.
                        if ((text[j] == ' ' &&
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - 40 - indent) ||
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - indent || j == text.Length())
                        {
                            if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) <=
                                wrap_width || j == text.Length())
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                next_start = j + 1;
                                searching = 0;
                            }
                            else
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                next_start = j + 1;
                                searching = 0;
                            }
                        }
                    }
                }
            }
            else
            {
                line_count = 1;
                wrapped_lines[0] = text;
            }
            for (int line_index = 0; line_index < line_count; line_index++)
            {
                private_colors[tab_index]->Add(IntToStr(color));
                private_lines[tab_index + 1]->Add(wrapped_lines[line_index]);
                if (line_index)
                    private_indents[tab_index]->Add(IntToStr(name_width));
                else
                    private_indents[tab_index]->Add("0");
            }
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    // Stays at the bottom while auto scroll is on or the thumb is down.
    if (auto_scroll || !auto_scroll && scrollbar_positions[tab_index] == 50)
    {
        if (private_lines[tab_index + 1]->Count > 7)
            scroll_offsets[tab_index] = private_lines[tab_index + 1]->Count - 7;
        else
            scroll_offsets[tab_index] = 0;
        scrollbar_positions[tab_index] = 50;
    }
    else if (scroll_offsets[tab_index] > 0 && private_lines[tab_index + 1]->Count >= 100)
    {
        scroll_offsets[tab_index]--;
        if (scroll_offsets[tab_index] == 0)
            scrollbar_positions[tab_index] = 0;
    }
    if (active_tab == tab_index + 1)
        text_dirty = 1;
    else if (!tab_has_unread[tab_index] && !silent)
    {
        tab_has_unread[tab_index] = 1;
        text_dirty = 1;
    }
    MainForm->chat_log->WriteLine(1, text);
}

// Adds a message to the local tab, wrapped like a private one; the last 300 lines are kept.
void ChatController::AddLocalMessage(int icon, int color, String sender, String text, bool silent)
{
    HDC dc;
    int line_start = 0;
    int j = 0;
    text = sender + "  " + text;
    if (local_lines->Count > MAX_CHAT_LINES)
    {
        do
        {
            local_lines->Delete(0);
            local_colors->Delete(0);
            local_icons->Delete(0);
            local_senders->Delete(0);
            local_indents->Delete(0);
        } while (local_lines->Count > MAX_CHAT_LINES);
    }
    int wrap_width = 420;
    int line_count;
    int next_start;
    int name_width;
    line_count = 0;
    next_start = 1;
    String wrapped_lines[MAX_WRAPPED_CHAT_LINES];
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Style = bsClear;
            MainForm->draw_canvas->Font->Color = clBlack;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            name_width = MainForm->draw_canvas->TextWidth(Client::Capitalize(MainForm->net_client, sender) + "  ");
            if (MainForm->draw_canvas->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() < next_start)
                        continue;
                    line_count++;
                    line_start = next_start;
                    char searching = 1;
                    for (j = next_start; j <= text.Length() && searching; j++)
                    {
                        int indent = 0;
                        if (i > 0)
                            indent = name_width;
                        if ((text[j] == ' ' &&
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - 40 - indent) ||
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - indent || j == text.Length())
                        {
                            if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) <=
                                wrap_width || j == text.Length())
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                next_start = j + 1;
                                searching = 0;
                            }
                            else
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                next_start = j + 1;
                                searching = 0;
                            }
                        }
                    }
                }
            }
            else
            {
                line_count = 1;
                wrapped_lines[0] = text;
            }
            for (int line_index = 0; line_index < line_count; line_index++)
            {
                local_colors->Add(IntToStr(color));
                local_lines->Add(wrapped_lines[line_index]);
                local_senders->Add(sender);
                if (line_index)
                {
                    local_icons->Add(String(-1));
                    local_indents->Add(IntToStr(name_width));
                }
                else
                {
                    local_icons->Add(IntToStr(icon));
                    local_indents->Add("0");
                }
            }
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    if (auto_scroll || !auto_scroll && scrollbar_positions[2] == 50)
    {
        if (local_lines->Count > 7)
            scroll_offsets[2] = local_lines->Count - 7;
        else
            scroll_offsets[2] = 0;
        scrollbar_positions[2] = 50;
    }
    else if (scroll_offsets[2] > 0 && local_lines->Count >= MAX_CHAT_LINES)
    {
        scroll_offsets[2]--;
        if (scroll_offsets[2] == 0)
            scrollbar_positions[2] = 0;
    }
    if (active_tab == 3)
        text_dirty = 1;
    else if (!tab_has_unread[2] && !silent)
    {
        tab_has_unread[2] = 1;
        text_dirty = 1;
    }
    if (icon == ChatIcon_Announce)
        MainForm->chat_log->WriteLine(LOG_CHANNEL_GLOBAL, text);
    else
        MainForm->chat_log->WriteLine(0, text);
}

// Adds a message to the group tab (party, guild and admin chat), wrapped like a private one.
void ChatController::AddGroupMessage(int icon, int color, String sender, String text, bool silent)
{
    HDC dc;
    int line_start = 0;
    int j = 0;
    text = sender + "  " + text;
    if (group_lines->Count > MAX_CHAT_LINES)
    {
        do
        {
            group_lines->Delete(0);
            group_colors->Delete(0);
            group_icons->Delete(0);
            group_senders->Delete(0);
            private_lines[0]->Delete(0);
        } while (group_lines->Count > MAX_CHAT_LINES);
    }
    int wrap_width = 420;
    int line_count;
    int next_start;
    int name_width;
    line_count = 0;
    next_start = 1;
    String wrapped_lines[4];
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Style = bsClear;
            MainForm->draw_canvas->Font->Color = clBlack;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            name_width = MainForm->draw_canvas->TextWidth(Client::Capitalize(MainForm->net_client, sender) + "  ");
            if (MainForm->draw_canvas->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() < next_start)
                        continue;
                    line_count++;
                    line_start = next_start;
                    char searching = 1;
                    for (j = next_start; j <= text.Length() && searching; j++)
                    {
                        int indent = 0;
                        if (i > 0)
                            indent = name_width;
                        if ((text[j] == ' ' &&
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - 40 - indent) ||
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - indent || j == text.Length())
                        {
                            if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) <=
                                wrap_width || j == text.Length())
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                next_start = j + 1;
                                searching = 0;
                            }
                            else
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                next_start = j + 1;
                                searching = 0;
                            }
                        }
                    }
                }
            }
            else
            {
                line_count = 1;
                wrapped_lines[0] = text;
            }
            for (int line_index = 0; line_index < line_count; line_index++)
            {
                group_colors->Add(IntToStr(color));
                group_lines->Add(wrapped_lines[line_index]);
                group_senders->Add(sender);
                if (line_index)
                {
                    group_icons->Add(String(-1));
                    private_lines[0]->Add(IntToStr(name_width));
                }
                else
                {
                    group_icons->Add(IntToStr(icon));
                    private_lines[0]->Add("0");
                }
            }
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    if (auto_scroll || !auto_scroll && scrollbar_positions[4] == 50)
    {
        if (group_lines->Count > 7)
            scroll_offsets[4] = group_lines->Count - 7;
        else
            scroll_offsets[4] = 0;
        scrollbar_positions[4] = 50;
    }
    else if (scroll_offsets[4] > 0 && group_lines->Count >= MAX_CHAT_LINES)
    {
        scroll_offsets[4]--;
        if (scroll_offsets[4] == 0)
            scrollbar_positions[4] = 0;
    }
    if (active_tab == 5)
        text_dirty = 1;
    else if (!tab_has_unread[4] && !silent)
    {
        tab_has_unread[4] = 1;
        text_dirty = 1;
    }
    MainForm->chat_log->WriteLine(LOG_CHANNEL_GLOBAL, text);
}

// Adds a message to the global tab; with skip_duplicates, lines the tab holds already are left out.
void ChatController::AddGlobalMessage(int icon, int color, String sender, String text, bool silent,
    bool skip_duplicates)
{
    HDC dc;
    int line_start = 0;
    int j = 0;
    int m = 0;
    text = sender + "  " + text;
    if (global_lines->Count > MAX_CHAT_LINES)
    {
        do
        {
            global_lines->Delete(0);
            global_colors->Delete(0);
            global_icons->Delete(0);
            global_senders->Delete(0);
            global_indents->Delete(0);
        } while (global_lines->Count > MAX_CHAT_LINES);
    }
    int wrap_width = 420;
    int line_count;
    int next_start;
    int name_width;
    line_count = 0;
    next_start = 1;
    String wrapped_lines[MAX_WRAPPED_CHAT_LINES];
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Brush->Style = bsClear;
            MainForm->draw_canvas->Font->Color = clBlack;
            MainForm->draw_canvas->Font->Name = "MS Sans Serif";
            MainForm->draw_canvas->Font->Size = 8;
            name_width = MainForm->draw_canvas->TextWidth(Client::Capitalize(MainForm->net_client, sender) + "  ");
            if (MainForm->draw_canvas->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() < next_start)
                        continue;
                    line_count++;
                    line_start = next_start;
                    char searching = 1;
                    for (j = next_start; j <= text.Length() && searching; j++)
                    {
                        int indent = 0;
                        if (i > 0)
                            indent = name_width;
                        if ((text[j] == ' ' &&
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - 40 - indent) ||
                            MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) >=
                            wrap_width - indent || j == text.Length())
                        {
                            if (MainForm->draw_canvas->TextWidth(text.SubString(line_start, j - line_start)) <=
                                wrap_width || j == text.Length())
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                next_start = j + 1;
                                searching = 0;
                            }
                            else
                            {
                                wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                next_start = j + 1;
                                searching = 0;
                            }
                        }
                    }
                }
            }
            else
            {
                line_count = 1;
                wrapped_lines[0] = text;
            }
            for (int line_index = 0; line_index < line_count; line_index++)
            {
                char already_listed = 0;
                if (skip_duplicates)
                {
                    for (m = 0; m < global_lines->Count; m++)
                    {
                        if (global_lines->Strings[m] == wrapped_lines[line_index])
                            already_listed = 1;
                    }
                }
                if (!already_listed)
                {
                    global_colors->Add(IntToStr(color));
                    global_lines->Add(wrapped_lines[line_index]);
                    global_senders->Add(sender);
                    if (line_index)
                    {
                        global_icons->Add(String(-1));
                        global_indents->Add(IntToStr(name_width));
                    }
                    else
                    {
                        global_icons->Add(IntToStr(icon));
                        global_indents->Add("0");
                    }
                }
            }
            MainForm->draw_canvas->Handle = 0;
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    if (auto_scroll || !auto_scroll && scrollbar_positions[3] == 50)
    {
        if (global_lines->Count > 7)
            scroll_offsets[3] = global_lines->Count - 7;
        else
            scroll_offsets[3] = 0;
        scrollbar_positions[3] = 50;
    }
    else if (scroll_offsets[3] > 0 && global_lines->Count >= MAX_CHAT_LINES)
    {
        scroll_offsets[3]--;
        if (scroll_offsets[3] == 0)
            scrollbar_positions[3] = 0;
    }
    if (active_tab == 4)
        text_dirty = 1;
    else if (!tab_has_unread[3] && !silent)
    {
        tab_has_unread[3] = 1;
        text_dirty = 1;
    }
    MainForm->chat_log->WriteLine(LOG_CHANNEL_GLOBAL, text);
}

// Tabs, auto scroll and the scroll bar; a right click on a chat line puts "!" and its sender in the chat box.
void ChatController::OnMouseClick(int mouse_x, int mouse_y, int button)
{
    int local_row;
    int global_row;
    int group_row;
    int dirty_edit_result;
    if (mouse_y >= 435 && mouse_y <= 451)
    {
        if (mouse_x >= 125 && mouse_x <= 256 && private_name_1 != "")
        {
            UnsubscribeGlobalChat();
            if (mouse_x <= 138)
                ClosePrivateTab(1);
            else
            {
                active_tab = 1;
                tab_has_unread[0] = 0;
                text_dirty = 1;
            }
        }
        if (mouse_x >= 258 && mouse_x <= 389 && private_name_2 != "")
        {
            UnsubscribeGlobalChat();
            if (mouse_x <= 271)
                ClosePrivateTab(2);
            else
            {
                active_tab = 2;
                tab_has_unread[1] = 0;
                text_dirty = 1;
            }
        }
        if (mouse_x >= 391 && mouse_x <= 433)
        {
            UnsubscribeGlobalChat();
            active_tab = 3;
            tab_has_unread[2] = 0;
            text_dirty = 1;
        }
        if (mouse_x >= 435 && mouse_x <= 476)
        {
            SubscribeGlobalChat();
            active_tab = 4;
            tab_has_unread[3] = 0;
            text_dirty = 1;
        }
        if (mouse_x >= 479 && mouse_x <= 521)
        {
            UnsubscribeGlobalChat();
            active_tab = 5;
            tab_has_unread[4] = 0;
            text_dirty = 1;
        }
        if (mouse_x >= 523 && mouse_x <= 565)
        {
            UnsubscribeGlobalChat();
            active_tab = 6;
            tab_has_unread[5] = 0;
            text_dirty = 1;
        }
        if (mouse_x >= 573 && mouse_x <= 587)
        {
            if (auto_scroll)
                auto_scroll = false;
            else
                auto_scroll = true;
            text_dirty = 1;
        }
    }
    if (active_tab == 3 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        local_row = scroll_offsets[2] + (mouse_y - 333) / 14;
        if (local_senders->Count > local_row)
        {
            if (local_senders->Strings[local_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + local_senders->Strings[local_row] + " ");
                dirty_edit_result = MainForm->edits->HasDirtyEdit();
            }
        }
    }
    if (active_tab == 4 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        global_row = scroll_offsets[3] + (mouse_y - 333) / 14;
        if (global_senders->Count > global_row)
        {
            if (global_senders->Strings[global_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + global_senders->Strings[global_row] + " ");
                dirty_edit_result = MainForm->edits->HasDirtyEdit();
            }
        }
    }
    if (active_tab == 5 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        group_row = scroll_offsets[3] + (mouse_y - 333) / 14;
        if (group_senders->Count > group_row)
        {
            if (group_senders->Strings[group_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + group_senders->Strings[group_row] + " ");
                dirty_edit_result = MainForm->edits->HasDirtyEdit();
            }
        }
    }
    if (mouse_x >= 569 && mouse_x <= 585)
    {
        if (scrollbar_positions[active_tab - 1] + 348 <= mouse_y &&
            scrollbar_positions[active_tab - 1] + 363 >= mouse_y)
        {
            is_dragging_thumb = 1;
            drag_grab_offset = mouse_y - scrollbar_positions[active_tab - 1];
            DragScrollbar(mouse_y);
        }
        if (mouse_y >= 332 && mouse_y <= 347)
        {
            scrolling_up = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (mouse_y >= 414 && mouse_y <= 429)
        {
            scrolling_down = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
}

void ChatController::StopScrolling()
{
    scrolling_up = 0;
    scrolling_down = 0;
    is_dragging_thumb = 0;
}

void ChatController::OnMouseMove(int mouse_x, int mouse_y)
{
    UpdateScrollbarHover(mouse_x, mouse_y);
}

// Lights the scroll arrows under the mouse; leaving an arrow stops its scrolling.
void ChatController::UpdateScrollbarHover(int mouse_x, int mouse_y)
{
    char changed = 0;
    if (mouse_x >= 569 && mouse_x <= 585)
    {
        if (mouse_y >= 347 && mouse_y <= 414)
            DragScrollbar(mouse_y);
        if (mouse_y >= 332 && mouse_y <= 347)
        {
            if (!scroll_up_hover)
            {
                scroll_up_hover = 1;
                changed = 1;
            }
        }
        else if (scroll_up_hover)
        {
            scroll_up_hover = 0;
            scrolling_up = 0;
            changed = 1;
        }
        if (mouse_y >= 414 && mouse_y <= 429)
        {
            if (!scroll_down_hover)
            {
                scroll_down_hover = 1;
                changed = 1;
            }
        }
        else if (scroll_down_hover)
        {
            scroll_down_hover = 0;
            scrolling_down = 0;
            changed = 1;
        }
    }
    else if (scroll_up_hover || scroll_down_hover)
    {
        scroll_up_hover = 0;
        scroll_down_hover = 0;
        scrolling_up = 0;
        scrolling_down = 0;
        changed = 1;
    }
    if (changed)
        scrollbar_dirty = true;
}

// True once when the scroll bar has to be drawn again.
bool ChatController::TakeScrollbarDirty()
{
    bool was_dirty = 0;
    if (scrollbar_dirty)
    {
        scrollbar_dirty = false;
        was_dirty = 1;
    }
    return was_dirty;
}

// True once when the chat text has to be drawn again.
bool ChatController::TakeTextDirty()
{
    bool was_dirty = 0;
    if (text_dirty)
    {
        text_dirty = 0;
        was_dirty = 1;
    }
    return was_dirty;
}

void ChatController::Invalidate()
{
    text_dirty = 1;
}

// Drunk for 10 ticks per unit of the item's value, plus 100.
void ChatController::SetDrunkTime(int duration)
{
    drunk_ticks = 10 * duration + 100;
}

// Tells the server to send global chat only while the global tab is open.
void ChatController::SyncGlobalSubscription()
{
    if (!global_chat_open)
    {
        if (active_tab == 4)
        {
            global_chat_open = true;
            MainForm->net_client->sender->SendGlobalChatOpen();
        }
    }
    else if (active_tab != 4)
    {
        global_chat_open = false;
        MainForm->net_client->sender->SendGlobalChatClose();
    }
}

void ChatController::SubscribeGlobalChat()
{
    if (!global_chat_open)
    {
        global_chat_open = true;
        MainForm->net_client->sender->SendGlobalChatOpen();
    }
}

void ChatController::UnsubscribeGlobalChat()
{
    if (global_chat_open)
    {
        global_chat_open = false;
        MainForm->net_client->sender->SendGlobalChatClose();
    }
}

bool ChatController::IsDrunk()
{
    bool is_drunk = 0;
    if (drunk_ticks > 0)
        is_drunk = 1;
    return is_drunk;
}
