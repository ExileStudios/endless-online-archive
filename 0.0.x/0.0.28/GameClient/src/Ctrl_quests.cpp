#include <vcl.h>
#pragma hdrstop

#include "Ctrl_quests.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_trade.h"
#pragma package(smart_init)

QuestController::QuestController()
{
    lines = new TStringList();
    line_icon_types = new TStringList();
    line_detail_texts = new TStringList();
    line_progress_texts = new TStringList();
    quest_entries = new TStringList();
    history_lines = new TStringList();
    window_x = 77;
    window_y = 16;
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
    history_button_hovered = false;
    unused_cleared_flag = 0;
    showing_history = 0;
    history_requested = false;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
}

QuestController::~QuestController()
{
}

// Adds the finished quests from the server's reply, one line for each piece between separators.
void QuestController::LoadQuestHistory(String reply_data, String separator)
{
    while (reply_data.Length() > 0)
    {
        int sep_pos = reply_data.Pos(separator);
        String history_line = "";
        if (sep_pos > 0)
        {
            history_line = reply_data.SubString(1, sep_pos - 1);
            reply_data.Delete(1, sep_pos);
        }
        else
        {
            history_line = reply_data;
            reply_data = "";
        }
        history_lines->Add(history_line);
    }
}

// Fills the current quests from the server's reply: each quest's name, step and numbers, sorted by name, with the
// step kind turned into one of the icons and the progress shown as done and needed.
void QuestController::LoadQuestList(String reply_data, String separator)
{
    int token_end = 0;
    int third_sep_pos = 0;
    quest_entries->Clear();
    do
    {
        String quest_token;
        String detail_token;
        String numbers_token;
        token_end = reply_data.Pos(separator);
        if (token_end > 0)
        {
            quest_token = reply_data.SubString(1, token_end - 1);
            reply_data.Delete(1, token_end);
        }
        else
        {
            quest_token = reply_data;
            reply_data = "";
        }
        token_end = reply_data.Pos(separator);
        if (token_end > 0)
        {
            detail_token = reply_data.SubString(1, token_end - 1);
            reply_data.Delete(1, token_end);
        }
        else
        {
            detail_token = reply_data;
            reply_data = "";
        }
        third_sep_pos = reply_data.Pos(separator);
        if (third_sep_pos > 0)
        {
            numbers_token = reply_data.SubString(1, third_sep_pos - 1);
            reply_data.Delete(1, third_sep_pos);
        }
        else
        {
            numbers_token = "n/a";
            reply_data = "";
        }
        quest_entries->Add(quest_token + '\xFF' + detail_token + '\xFF' + numbers_token);
    } while (third_sep_pos > 0 && reply_data.Length() > 2);
    quest_entries->Sort();
    for (int i = 0; i < quest_entries->Count; i++)
    {
        String entry = quest_entries->Strings[i];
        int first_sep_pos = entry.Pos('\xFF');
        String quest_text = entry.SubString(1, first_sep_pos - 1);
        entry.Delete(1, first_sep_pos);
        first_sep_pos = entry.Pos('\xFF');
        String detail_text = entry.SubString(1, first_sep_pos - 1);
        entry.Delete(1, first_sep_pos);
        int icon_type = MainForm->net_client->DecodeNumber(entry.SubString(1, 2));
        int v5b = MainForm->net_client->DecodeNumber(entry.SubString(3, 2));
        int progress_target = MainForm->net_client->DecodeNumber(entry.SubString(5, 2));
        String progress_text = "n/a";
        if (v5b > 0 || progress_target > 0)
            progress_text = IntToStr(v5b) + " / " + IntToStr(progress_target);
        // The step kinds fold into four icons; unknown kinds get the first
        if (icon_type == 3)
            icon_type = 1;
        if (icon_type == 8)
            icon_type = 2;
        if (icon_type == 9)
            icon_type = 2;
        if (icon_type == 10)
            icon_type = 3;
        if (icon_type == 11)
            icon_type = 3;
        if (icon_type > 3)
            icon_type = 0;
        first_sep_pos = entry.Pos('\xFF');
        entry.Delete(1, first_sep_pos);
        AddText(IntToStr(icon_type), quest_text, detail_text, progress_text, false);
    }
}

// Adds a quest line with its icon, step and progress, wrapped to the page width; each wrapped line repeats them.
void QuestController::AddText(String icon_type, String text, String detail_text, String progress_text,
    bool add_blank_line)
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
                    line_icon_types->Add(icon_type);
                    line_detail_texts->Add(detail_text);
                    line_progress_texts->Add(progress_text);
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
                            line_icon_types->Add(icon_type);
                            line_detail_texts->Add(detail_text);
                            line_progress_texts->Add(progress_text);
                            text.Delete(1, fitted_text.Length());
                            split_done = 1;
                            break;
                        }
                        fitted_text = prefix;
                    }
                    if (!split_done)
                    {
                        lines->Add(text);
                        line_icon_types->Add(icon_type);
                        line_detail_texts->Add(detail_text);
                        line_progress_texts->Add(progress_text);
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
        line_icon_types->Add(icon_type);
        line_detail_texts->Add("");
        line_progress_texts->Add("");
    }
    MainForm->redraw_needed = true;
}

// Shows the quest book titled with the character's name, empty and on the current quests, closing the board.
void QuestController::Open()
{
    MainForm->animation->message_board->is_visible = 0;
    title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
    title = Client::Capitalize(MainForm->net_client, title);
    title = title + "'s " + MainForm->strings->GetText(GameText_QuestProgress);
    MainForm->gfx->CreateMsgBoardGraphics();
    MainForm->gfx->LoadMsgBoardGraphics();
    lines->Clear();
    line_icon_types->Clear();
    line_detail_texts->Clear();
    line_progress_texts->Clear();
    history_lines->Clear();
    top_line = 0;
    scroll_thumb_offset = 0;
    showing_history = 0;
    history_requested = false;
    close_hovered = 0;
    history_button_hovered = false;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void QuestController::Close()
{
    is_visible = 0;
    MainForm->gfx->FreeMsgBoardGraphics();
    MainForm->redraw_needed = true;
}

int QuestController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int QuestController::GetTopLine()
{
    return top_line;
}

// A click on the scroll bar, or anywhere else in the window to start moving it.
void QuestController::MouseDown(int x, int y, int button)
{
    if (x >= window_x + 449 && x <= window_x + 464)
    {
        if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
    }
    if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 44 && y <= window_y + 59)
    {
        scroll_up_held = 1;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 228 && y <= window_y + 243)
    {
        scroll_down_held = 1;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 484 && y >= window_y && y <= window_y + 290 && !thumb_grabbed)
    {
        is_dragging = 1;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered line and the history and close buttons.
void QuestController::MouseMove(int x, int y)
{
    UpdateScrollHover(x, y);
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 146)
            window_x = 146;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (!(y <= this->window_y + 43 || y >= this->window_y + 243 || x <= this->window_x + 10 ||
        x >= this->window_x + 449 || this->is_dragging))
    {
        int line = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
        if (line > 11)
            line = -1;
        if (lines->Count <= line)
            line = -1;
        if (line != hovered_line)
        {
            hovered_line = line;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (this->hovered_line >= 0)
        {
            this->hovered_line = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < this->window_x + 288 || x > this->window_x + 378 || y < this->window_y + 253 || y > this->window_y + 279))
    {
        if (!this->history_button_hovered)
        {
            this->history_button_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (history_button_hovered)
        {
            history_button_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < this->window_x + 380 || x > this->window_x + 470 || y < this->window_y + 253 || y > this->window_y + 279))
    {
        if (!this->close_hovered)
        {
            this->close_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (close_hovered)
        {
            close_hovered = 0;
            MainForm->redraw_needed = true;
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
void QuestController::UpdateScrollHover(int x, int y)
{
    if (x >= this->window_x + 449 && x <= this->window_x + 464)
    {
        if (y >= this->window_y + 58 && y <= this->window_y + 229)
            this->DragScrollThumb(y);
    }
    if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 44 && y <= window_y + 59)
    {
        if (!scroll_up_hovered)
        {
            scroll_up_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else if (scroll_up_hovered)
    {
        scroll_up_hovered = 0;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 449 && x <= window_x + 464 && y >= window_y + 228 && y <= window_y + 243)
    {
        if (!scroll_down_hovered)
        {
            scroll_down_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else if (scroll_down_hovered)
    {
        scroll_down_hovered = 0;
        MainForm->redraw_needed = true;
    }
}

// Follows a grabbed scroll thumb, kept on its track, and picks the top line of the quests or the history.
void QuestController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_line;
    int __slot1;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        int line_count = lines->Count;
        if (showing_history == 1)
            line_count = history_lines->Count;
        if (line_count > 12)
        {
            thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            scaled_line = (line_count - 12.0) * thumb_fraction;
            int new_top_line = scaled_line;
            if (new_top_line != top_line)
                top_line = new_top_line;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the quests or the history up one line and moves the scroll thumb to match.
void QuestController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
    int line_count = lines->Count;
    if (showing_history == 1)
        line_count = history_lines->Count;
    if (line_count > 12 && top_line > 0)
    {
        top_line--;
        scroll_fraction = (double)top_line / (double)(line_count - 12);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the quests or the history down one line and moves the scroll thumb to match.
void QuestController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    int result = lines->Count;
    if (showing_history == 1)
        result = history_lines->Count;
    if (result - 12 > top_line)
    {
        top_line++;
        scroll_fraction = (double)top_line / (double)(result - 12);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void QuestController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

// Scrolls back to the top, for a switch between the quests and the history.
void QuestController::ResetScroll()
{
    hovered_line = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
}
