#include <vcl.h>
#pragma hdrstop

#include "Ctrl_chat.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Packetoperations.h"
#include <stdlib.h>

#pragma package(smart_init)

ChatController::ChatController()
{
    active_tab = 3;
    auto_scroll = true;
    global_chat_open = false;
    first_private_name = "";
    second_private_name = "";
    scroll_up_hover = false;
    scroll_down_hover = false;
    scrollbar_dirty = false;
    text_dirty = false;
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
    private_tab_lines[0] = new TStringList();
    last_private_tab_lines[0] = new TStringList();
    private_colors[0] = new TStringList();
    last_private_colors[0] = new TStringList();
    private_indents[0] = new TStringList();
    private_indents[1] = new TStringList();
    for (int i = 0; i <= 5; i++)
    {
        scroll_offsets[i] = 0;
        scrollbar_positions[i] = 50;
        tab_has_unread[i] = false;
    }
    scrolling_up = false;
    scrolling_down = false;
    is_dragging_thumb = false;
}

// 0 or 1 for the private tab talking with the name (in any case), or -1.
int ChatController::FindPrivateTab(String name)
{
    int tab_index = -1;
    if (LowerCase(first_private_name) == LowerCase(name))
        tab_index = 0;
    if (LowerCase(second_private_name) == LowerCase(name))
        tab_index = 1;
    return tab_index;
}

// Opens a private tab for the name in the first free one of the two, unless one is open for it already.
void ChatController::OpenPrivateTab(String name)
{
    if (first_private_name != name && second_private_name != name)
    {
        if (first_private_name == "")
        {
            first_private_name = name;
            scrollbar_dirty = true;
            text_dirty = true;
        }
        else if (second_private_name == "")
        {
            second_private_name = name;
            scrollbar_dirty = true;
            text_dirty = true;
        }
    }
}

// Closes private tab 1 or 2 and shows the other one, or local chat when that one is closed too.
void ChatController::ClosePrivateTab(int tab)
{
    if (tab == 1)
    {
        first_private_name = "";
        private_tab_lines[0]->Clear();
        private_colors[0]->Clear();
        private_indents[0]->Clear();
        scroll_offsets[0] = 0;
        scrollbar_positions[0] = 50;
        if (second_private_name != "")
            active_tab = 2;
        else
            active_tab = 3;
        scrollbar_dirty = true;
        text_dirty = true;
    }
    if (tab == 2)
    {
        second_private_name = "";
        last_private_tab_lines[0]->Clear();
        last_private_colors[0]->Clear();
        private_indents[1]->Clear();
        scroll_offsets[1] = 0;
        scrollbar_positions[1] = 50;
        if (first_private_name != "")
            active_tab = 1;
        else
            active_tab = 3;
        scrollbar_dirty = true;
        text_dirty = true;
    }
}

// Empties every tab and closes the private ones.
void ChatController::ResetChat()
{
    active_tab = 3;
    auto_scroll = true;
    first_private_name = "";
    second_private_name = "";
    scroll_up_hover = false;
    scroll_down_hover = false;
    scrollbar_dirty = false;
    text_dirty = false;
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
    private_tab_lines[0]->Clear();
    last_private_tab_lines[0]->Clear();
    private_colors[0]->Clear();
    last_private_colors[0]->Clear();
    private_indents[0]->Clear();
    private_indents[1]->Clear();
    for (int tab = 0; tab <= 5; tab++)
    {
        scroll_offsets[tab] = 0;
        scrollbar_positions[tab] = 50;
    }
    scrolling_up = false;
    scrolling_down = false;
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
    return scroll_offsets[active_tab + 5];
}

// While drunk, every fifth tick plays emote 11 four times in ten; also repeats a held scroll arrow.
void ChatController::OnTimer()
{
    if (drunk_ticks > 0)
    {
        drunk_ticks--;
        if (drunk_ticks % 5 == 0)
        {
            int roll = random(10);
            if (roll <= 3)
                MainForm->animation->game->PlayEmote(Emote_Drunk);
        }
    }
    if (scroll_repeat_delay > 0)
    {
        scroll_repeat_delay--;
        return;
    }
    if (scrolling_up)
        ScrollUp();
    if (scrolling_down)
        ScrollDown();
}

// One line up in the open tab; the tab shows 7 lines and the thumb moves over 50 pixels.
void ChatController::ScrollUp()
{
    double offset_ratio;
    double thumb_position;
    int tab;
    int line_count;
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
    if (line_count > 7 && scroll_offsets[active_tab - 1] > 0)
    {
        scroll_offsets[active_tab - 1]--;
        double offset_ratio = (double)scroll_offsets[active_tab - 1] / (line_count - 7);
        double thumb_position = PANEL_THUMB_TRAVEL * offset_ratio;
        scroll_offsets[active_tab + 5] = thumb_position;
        scrollbar_dirty = true;
        text_dirty = true;
    }
}

// One line down in the open tab.
void ChatController::ScrollDown()
{
    double offset_ratio;
    double thumb_position;
    int tab;
    int line_count;
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
    if (line_count - 7 > scroll_offsets[active_tab - 1])
    {
        scroll_offsets[active_tab - 1]++;
        double offset_ratio = (double)scroll_offsets[active_tab - 1] / (line_count - 7);
        double thumb_position = PANEL_THUMB_TRAVEL * offset_ratio;
        scroll_offsets[active_tab + 5] = thumb_position;
        scrollbar_dirty = true;
        text_dirty = true;
    }
}

// Scrolls the open tab to where the thumb is dragged, 0 to 50 pixels down the bar.
void ChatController::DragScrollbar(int mouse_y)
{
    int line_count;
    int lines;
    if (is_dragging_thumb)
    {
        int thumb_position = mouse_y - drag_grab_offset;
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
            double thumb_ratio = thumb_position / PANEL_THUMB_TRAVEL;
            double scaled_offset = (line_count - 7.0) * thumb_ratio;
            lines = scaled_offset;
            if (lines != scroll_offsets[active_tab - 1])
                scroll_offsets[active_tab - 1] = lines;
        }
        scroll_offsets[active_tab + 5] = thumb_position;
        scrollbar_dirty = true;
        text_dirty = true;
    }
}

// Adds a line to the system tab (the last 100 are kept) and writes it to the chat log.
void ChatController::AddSystemMessage(int icon, int color, String text, bool silent)
{
    // The tab keeps its last 100 lines.
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
    if (auto_scroll || (!auto_scroll && scrollbar_positions[5] == 50))
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
        if (!scroll_offsets[5])
            scrollbar_positions[5] = 0;
    }
    if (active_tab == 6)
        text_dirty = true;
    else if (!tab_has_unread[5] && !silent)
    {
        tab_has_unread[5] = true;
        text_dirty = true;
    }
    MainForm->chat_log->WriteLine(3, text);
}

// Adds a message to a private tab, wrapped with the later lines set in under the name, kept to 100 lines; it follows
// the scroll, marks the tab unread when another tab is open and writes the line to the chat log.
void ChatController::AddPrivateMessage(int tab_index, int color, String sender, String text, bool silent)
{
    int j;
    int line_start;
    text = sender + "  " + text;
    if (private_tab_lines[tab_index]->Count > 100)
    {
        do
        {
            private_tab_lines[tab_index]->Delete(0);
            private_colors[tab_index]->Delete(0);
            private_indents[tab_index]->Delete(0);
        } while (private_tab_lines[tab_index]->Count > 100);
    }
    int wrap_width = 420;
    int line_count = 0;
    int next_start = 1;
    String wrapped_lines[5];
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            int name_width;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            name_width = (short)MainForm->fonts->TextWidth(MainForm->net_client->Capitalize(sender) + "  ");
            if ((short)MainForm->fonts->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() >= next_start)
                    {
                        line_count++;
                        line_start = next_start;
                        char searching = true;
                        for (j = next_start; j <= text.Length() && searching; j++)
                        {
                            int indent = 0;
                            if (i > 0)
                                indent = name_width;
                            // A line ends at a space 40 pixels before the width, or at the width, with a hyphen past it.
                            if ((text[j] == ' ' &&
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - 40 - indent) ||
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - indent || j == text.Length())
                            {
                                if ((short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) <=
                                    wrap_width || j == text.Length())
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                    next_start = j + 1;
                                    searching = false;
                                }
                                else
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                    next_start = j + 1;
                                    searching = false;
                                }
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
                private_tab_lines[tab_index]->Add(wrapped_lines[line_index]);
                if (line_index)
                    private_indents[tab_index]->Add(IntToStr(name_width));
                else
                    private_indents[tab_index]->Add("0");
            }
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    // Stays at the bottom while auto scroll is on or the thumb is down.
    if (auto_scroll || (!auto_scroll && scrollbar_positions[tab_index] == 50))
    {
        if (private_tab_lines[tab_index]->Count > 7)
            scroll_offsets[tab_index] = private_tab_lines[tab_index]->Count - 7;
        else
            scroll_offsets[tab_index] = 0;
        scrollbar_positions[tab_index] = 50;
    }
    else if (scroll_offsets[tab_index] > 0 && private_tab_lines[tab_index]->Count >= 100)
    {
        scroll_offsets[tab_index]--;
        if (!scroll_offsets[tab_index])
            scrollbar_positions[tab_index] = 0;
    }
    if ((unsigned int)active_tab == tab_index + 1)
        text_dirty = true;
    else if (!tab_has_unread[tab_index] && !silent)
    {
        tab_has_unread[tab_index] = true;
        text_dirty = true;
    }
    MainForm->chat_log->WriteLine(1, text);
}

// Adds a message to the local chat tab, wrapped at spaces to the tab's width, dropping the oldest lines past 300;
// it follows the scroll, marks the tab unread when another tab is open and writes the line to the chat log.
void ChatController::AddLocalMessage(int icon, int color, String sender, String text, bool silent)
{
    int j;
    int line_start;
    text = sender + "  " + text;
    // The tab keeps its last 300 lines.
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
    int line_count = 0;
    int next_start = 1;
    String wrapped_lines[5];
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            int name_width;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            name_width = (short)MainForm->fonts->TextWidth(MainForm->net_client->Capitalize(sender) + "  ");
            if ((short)MainForm->fonts->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() >= next_start)
                    {
                        line_count++;
                        line_start = next_start;
                        char searching = true;
                        for (j = next_start; j <= text.Length() && searching; j++)
                        {
                            int indent = 0;
                            if (i > 0)
                                indent = name_width;
                            if ((text[j] == ' ' &&
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - 40 - indent) ||
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - indent || j == text.Length())
                            {
                                if ((short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) <=
                                    wrap_width || j == text.Length())
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                    next_start = j + 1;
                                    searching = false;
                                }
                                else
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                    next_start = j + 1;
                                    searching = false;
                                }
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
                    local_icons->Add(-1);
                    local_indents->Add(IntToStr(name_width));
                }
                else
                {
                    local_icons->Add(IntToStr(icon));
                    local_indents->Add("0");
                }
            }
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    // With auto scroll on, or the bar at the bottom, the view follows the new line.
    if (auto_scroll || (!auto_scroll && scrollbar_positions[2] == 50))
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
        if (!scroll_offsets[2])
            scrollbar_positions[2] = 0;
    }
    if (active_tab == 3)
        text_dirty = true;
    // Another tab being open, this one is marked as holding unread lines, unless the message is silent.
    else if (!tab_has_unread[2] && !silent)
    {
        tab_has_unread[2] = true;
        text_dirty = true;
    }
    if (icon == ChatIcon_Announce)
        MainForm->chat_log->WriteLine(LOG_CHANNEL_GLOBAL, text);
    else
        MainForm->chat_log->WriteLine(0, text);
}

// Adds a message to the group chat tab as the local tab does: wrapped, kept to 300 lines, followed by the scroll
// and marked unread when another tab is open.
void ChatController::AddGroupMessage(int icon, int color, String sender, String text, bool silent)
{
    int j;
    int line_start;
    text = sender + "  " + text;
    // The tab keeps its last 300 lines.
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
    int line_count = 0;
    int next_start = 1;
    String wrapped_lines[5];
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            int name_width;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            name_width = (short)MainForm->fonts->TextWidth(MainForm->net_client->Capitalize(sender) + "  ");
            if ((short)MainForm->fonts->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() >= next_start)
                    {
                        line_count++;
                        line_start = next_start;
                        char searching = true;
                        for (j = next_start; j <= text.Length() && searching; j++)
                        {
                            int indent = 0;
                            if (i > 0)
                                indent = name_width;
                            if ((text[j] == ' ' &&
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - 40 - indent) ||
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - indent || j == text.Length())
                            {
                                if ((short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) <=
                                    wrap_width || j == text.Length())
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                    next_start = j + 1;
                                    searching = false;
                                }
                                else
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                    next_start = j + 1;
                                    searching = false;
                                }
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
                    group_icons->Add(-1);
                    private_lines[0]->Add(IntToStr(name_width));
                }
                else
                {
                    group_icons->Add(IntToStr(icon));
                    private_lines[0]->Add("0");
                }
            }
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    if (auto_scroll || (!auto_scroll && scrollbar_positions[4] == 50))
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
        if (!scroll_offsets[4])
            scrollbar_positions[4] = 0;
    }
    if (active_tab == 5)
        text_dirty = true;
    // Another tab being open, this one is marked as holding unread lines, unless the message is silent.
    else if (!tab_has_unread[4] && !silent)
    {
        tab_has_unread[4] = true;
        text_dirty = true;
    }
    MainForm->chat_log->WriteLine(LOG_CHANNEL_GLOBAL, text);
}

// Adds a message to the global tab; with skip_duplicates, lines the tab holds already are left out.
void ChatController::AddGlobalMessage(int icon, int color, String sender, String text, bool silent,
    bool skip_duplicates)
{
    int j;
    int m;
    int line_start;
    int unused_value;
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
    int line_count = 0;
    int next_start = 1;
    String wrapped_lines[5];
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            int name_width;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            name_width = (short)MainForm->fonts->TextWidth(MainForm->net_client->Capitalize(sender) + "  ");
            if ((short)MainForm->fonts->TextWidth(text) >= wrap_width)
            {
                for (int i = 0; i <= 5; i++)
                {
                    if (text.Length() >= next_start)
                    {
                        line_count++;
                        line_start = next_start;
                        char searching = true;
                        for (j = next_start; j <= text.Length() && searching; j++)
                        {
                            int indent = 0;
                            if (i > 0)
                                indent = name_width;
                            if ((text[j] == ' ' &&
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - 40 - indent) ||
                                (short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) >=
                                    wrap_width - indent || j == text.Length())
                            {
                                if ((short)MainForm->fonts->TextWidth(text.SubString(line_start, j - line_start)) <=
                                    wrap_width || j == text.Length())
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1);
                                    next_start = j + 1;
                                    searching = false;
                                }
                                else
                                {
                                    wrapped_lines[i] = text.SubString(line_start, j - line_start + 1) + "-";
                                    next_start = j + 1;
                                    searching = false;
                                }
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
                char already_listed = false;
                if (skip_duplicates)
                {
                    for (m = 0; m < global_lines->Count; m++)
                    {
                        if (global_lines->Strings[m] == wrapped_lines[line_index])
                            already_listed = true;
                    }
                }
                if (!already_listed)
                {
                    global_colors->Add(IntToStr(color));
                    global_lines->Add(wrapped_lines[line_index]);
                    global_senders->Add(sender);
                    if (line_index)
                    {
                        global_icons->Add(-1);
                        global_indents->Add(IntToStr(name_width));
                    }
                    else
                    {
                        global_icons->Add(IntToStr(icon));
                        global_indents->Add("0");
                    }
                }
            }
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    if (auto_scroll || (!auto_scroll && scrollbar_positions[3] == 50))
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
        if (!scroll_offsets[3])
            scrollbar_positions[3] = 0;
    }
    if (active_tab == 4)
        text_dirty = true;
    else if (!tab_has_unread[3] && !silent)
    {
        tab_has_unread[3] = true;
        text_dirty = true;
    }
    MainForm->chat_log->WriteLine(LOG_CHANNEL_GLOBAL, text);
}

// Tabs, auto scroll and the scroll bar; a right click on a chat line puts "!" and its sender in the chat box.
void ChatController::OnMouseClick(int mouse_x, int mouse_y, int button)
{
    // The tab strip: the two private tabs, then local, global, group and system.
    if (mouse_y >= 435 && mouse_y <= 451)
    {
        if (mouse_x >= 125 && mouse_x <= 256 && first_private_name != "")
        {
            UnsubscribeGlobalChat();
            if (mouse_x <= 138)
                ClosePrivateTab(1);
            else
            {
                active_tab = 1;
                tab_has_unread[0] = false;
                text_dirty = true;
            }
        }
        if (mouse_x >= 258 && mouse_x <= 389 && second_private_name != "")
        {
            UnsubscribeGlobalChat();
            if (mouse_x <= 271)
                ClosePrivateTab(2);
            else
            {
                active_tab = 2;
                tab_has_unread[1] = false;
                text_dirty = true;
            }
        }
        if (mouse_x >= 391 && mouse_x <= 433)
            ShowLocalTab();
        if (mouse_x >= 435 && mouse_x <= 476)
            ShowGlobalTab();
        if (mouse_x >= 479 && mouse_x <= 521)
            ShowGroupTab();
        if (mouse_x >= 523 && mouse_x <= 565)
            ShowSystemTab();
        if (mouse_x >= 573 && mouse_x <= 587)
        {
            if (auto_scroll)
                auto_scroll = false;
            else
                auto_scroll = true;
            text_dirty = true;
        }
    }
    // A click on a line of the local, global or group tab starts a private message to its sender.
    if (active_tab == 3 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        int local_row = scroll_offsets[2] + (mouse_y - 333) / 14;
        if (local_senders->Count > local_row)
        {
            if (local_senders->Strings[local_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + local_senders->Strings[local_row] + " ");
                MainForm->edits->HasDirtyEdit();
            }
        }
    }
    if (active_tab == 4 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        int global_row = scroll_offsets[3] + (mouse_y - 333) / 14;
        if (global_senders->Count > global_row)
        {
            if (global_senders->Strings[global_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + global_senders->Strings[global_row] + " ");
                MainForm->edits->HasDirtyEdit();
            }
        }
    }
    if (active_tab == 5 && button == 1 && mouse_x >= 127 && mouse_x <= 560 && mouse_y >= 333 && mouse_y <= 430)
    {
        int group_row = scroll_offsets[3] + (mouse_y - 333) / 14;
        if (group_senders->Count > group_row)
        {
            if (group_senders->Strings[group_row] != "")
            {
                MainForm->edits->SetText(EditId_Chat, "!" + group_senders->Strings[group_row] + " ");
                MainForm->edits->HasDirtyEdit();
            }
        }
    }
    if (mouse_x >= 569 && mouse_x <= 585)
    {
        if (scroll_offsets[active_tab + 5] + 348 <= mouse_y && scroll_offsets[active_tab + 5] + 363 >= mouse_y)
        {
            is_dragging_thumb = true;
            drag_grab_offset = mouse_y - scroll_offsets[active_tab + 5];
            DragScrollbar(mouse_y);
        }
        if (mouse_y >= 332 && mouse_y <= 347)
        {
            scrolling_up = true;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (mouse_y >= 414 && mouse_y <= 429)
        {
            scrolling_down = true;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
}

// Shows the local chat tab, leaving global chat.
void ChatController::ShowLocalTab()
{
    UnsubscribeGlobalChat();
    active_tab = 3;
    tab_has_unread[2] = false;
    text_dirty = true;
}

// Shows the global chat tab, joining global chat.
void ChatController::ShowGlobalTab()
{
    SubscribeGlobalChat();
    active_tab = 4;
    tab_has_unread[3] = false;
    text_dirty = true;
}

// Shows the group chat tab, leaving global chat.
void ChatController::ShowGroupTab()
{
    UnsubscribeGlobalChat();
    active_tab = 5;
    tab_has_unread[4] = false;
    text_dirty = true;
}

// Shows the system tab, leaving global chat.
void ChatController::ShowSystemTab()
{
    UnsubscribeGlobalChat();
    active_tab = 6;
    tab_has_unread[5] = false;
    text_dirty = true;
}

void ChatController::StopScrolling()
{
    scrolling_up = false;
    scrolling_down = false;
    is_dragging_thumb = false;
}

void ChatController::OnMouseMove(int mouse_x, int mouse_y)
{
    UpdateScrollbarHover(mouse_x, mouse_y);
}

// Lights the scroll arrows under the mouse; leaving an arrow stops its scrolling.
void ChatController::UpdateScrollbarHover(int mouse_x, int mouse_y)
{
    char changed = false;
    DragScrollbar(mouse_y);
    if (mouse_x >= 569 && mouse_x <= 585)
    {
        if (mouse_y >= 332 && mouse_y <= 347)
        {
            if (!scroll_up_hover)
            {
                scroll_up_hover = true;
                changed = true;
            }
        }
        else if (scroll_up_hover)
        {
            scroll_up_hover = false;
            scrolling_up = false;
            changed = true;
        }
        if (mouse_y >= 414 && mouse_y <= 429)
        {
            if (!scroll_down_hover)
            {
                scroll_down_hover = true;
                changed = true;
            }
        }
        else if (scroll_down_hover)
        {
            scroll_down_hover = false;
            scrolling_down = false;
            changed = true;
        }
    }
    else if (scroll_up_hover || scroll_down_hover)
    {
        scroll_up_hover = false;
        scroll_down_hover = false;
        scrolling_up = false;
        scrolling_down = false;
        changed = 1;
    }
    if (changed)
        scrollbar_dirty = true;
}

// True once when the scroll bar has to be drawn again.
bool ChatController::TakeScrollbarDirty()
{
    bool was_dirty = false;
    if (scrollbar_dirty)
    {
        scrollbar_dirty = false;
        was_dirty = true;
    }
    return was_dirty;
}

// True once when the chat text has to be drawn again.
bool ChatController::TakeTextDirty()
{
    bool was_dirty = false;
    if (text_dirty)
    {
        text_dirty = false;
        was_dirty = true;
    }
    return was_dirty;
}

void ChatController::Invalidate()
{
    text_dirty = true;
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
    bool is_drunk = false;
    if (drunk_ticks > 0)
        is_drunk = true;
    return is_drunk;
}
