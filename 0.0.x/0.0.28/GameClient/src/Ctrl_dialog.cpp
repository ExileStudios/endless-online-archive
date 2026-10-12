#include <vcl.h>
#pragma hdrstop

#include "Ctrl_dialog.h"
#include "Tmainform.h"
#include "Packetoperations.h"
#pragma package(smart_init)

#define DIALOG_BUTTON_BACK_FRAME 5
#define DIALOG_BUTTON_NEXT_FRAME 7
#define DIALOG_THUMB_TRAVEL 52.0

DialogController::DialogController()
{
    lines = new TStringList();
    line_right_texts = new TStringList();
    unused_lines = new TStringList();
    pages = new TStringList();
    link_texts = new TStringList();
    quest_names = new TStringList();
    window_x = 49;
    window_y = 115;
    unused_flag = 1;
    has_quest_list = true;
    unused_value = 0;
    request_cooldown = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = 0;
    prev_button_hovered = false;
    next_button_hovered = false;
    prev_button_kind = 0;
    next_button_kind = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
}

DialogController::~DialogController()
{
}

// Adds a text with its right-hand value, wrapped to the page width; a link line takes the next free link slot.
void DialogController::AddText(String text, String right_text, bool add_blank_line)
{
    if (text.Length() > 0)
    {
        // A line starting with an asterisk is a reply link
        if (text[1] == '*')
        {
            for (int slot = 0; slot < DIALOG_MAX_LINKS; slot++)
            {
                if (link_line_indices[slot] < 0)
                {
                    link_line_indices[slot] = lines->Count;
                    break;
                }
            }
        }
    }
    int max_width = 200;
    HDC dc;
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
                    line_right_texts->Add(right_text);
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
                            line_right_texts->Add(right_text);
                            text.Delete(1, fitted_text.Length());
                            split_done = 1;
                            break;
                        }
                        fitted_text = prefix;
                    }
                    if (!split_done)
                    {
                        lines->Add(text);
                        line_right_texts->Add(right_text);
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
        line_right_texts->Add("");
    }
    MainForm->redraw_needed = true;
}

// Shows the current page and sets the previous and next buttons; the last page also lists the reply links.
void DialogController::ShowPage()
{
    if (pages->Count > page_index)
    {
        ClearText();
        AddText(pages->Strings[page_index], "", false);
    }
    if (!page_index)
        prev_button_kind = 1;
    else
        prev_button_kind = DIALOG_BUTTON_BACK_FRAME;
    if (pages->Count - 1 <= page_index)
        next_button_kind = DIALOG_BUTTON_OK_FRAME;
    else
        next_button_kind = DIALOG_BUTTON_NEXT_FRAME;
    if (pages->Count - 1 <= page_index)
    {
        if (link_texts->Count > 0)
        {
            AddText(" ", "", false);
            for (int i = 0; i < link_texts->Count; i++)
                AddText("*" + link_texts->Strings[i], "", false);
        }
    }
}

// Goes back one page, if there is one, and shows it as ShowPage does.
void DialogController::PreviousPage()
{
    if (page_index > 0)
    {
        ClearText();
        page_index--;
        AddText(pages->Strings[page_index], "", false);
    }
    if (!page_index)
        prev_button_kind = 1;
    else
        prev_button_kind = DIALOG_BUTTON_BACK_FRAME;
    if (pages->Count - 1 <= page_index)
        next_button_kind = DIALOG_BUTTON_OK_FRAME;
    else
        next_button_kind = DIALOG_BUTTON_NEXT_FRAME;
    if (pages->Count - 1 <= page_index)
    {
        if (link_texts->Count > 0)
        {
            AddText(" ", "", false);
            for (int i = 0; i < link_texts->Count; i++)
                AddText("*" + link_texts->Strings[i], "", false);
        }
    }
}

// Goes on one page, if there is one, and shows it as ShowPage does.
void DialogController::NextPage()
{
    if (pages->Count - 1 > page_index)
    {
        ClearText();
        page_index++;
        AddText(pages->Strings[page_index], "", false);
    }
    if (!page_index)
        prev_button_kind = 1;
    else
        prev_button_kind = DIALOG_BUTTON_BACK_FRAME;
    if (pages->Count - 1 <= page_index)
        next_button_kind = DIALOG_BUTTON_OK_FRAME;
    else
        next_button_kind = DIALOG_BUTTON_NEXT_FRAME;
    if (pages->Count - 1 <= page_index)
    {
        if (link_texts->Count > 0)
        {
            AddText(" ", "", false);
            for (int i = 0; i < link_texts->Count; i++)
                AddText("*" + link_texts->Strings[i], "", false);
        }
    }
}

// Adds a page of dialog text.
void DialogController::AddPage(String page_text)
{
    pages->Add(page_text);
    page_count++;
}

// Adds a reply link with the id sent back when it is clicked.
void DialogController::AddLink(int link_id, String link_text)
{
    link_texts->Add(link_text);
    link_ids[link_texts->Count - 1] = link_id;
}

// Shows the dialog window for the quest with no pages or links yet; the dialog reply fills them.
void DialogController::Open(String first_quest_name)
{
    MainForm->gfx->CreateDialogGraphics();
    MainForm->gfx->LoadDialogGraphics();
    lines->Clear();
    line_right_texts->Clear();
    pages->Clear();
    link_texts->Clear();
    top_line = 0;
    scroll_thumb_offset = 0;
    page_index = 0;
    page_count = 0;
    quest_name = first_quest_name;
    showing_quest_list = false;
    if (pages->Count > 0)
        AddText(pages->Strings[0], "", false);
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void DialogController::Close()
{
    is_visible = 0;
    MainForm->gfx->FreeDialogGraphics();
    MainForm->redraw_needed = true;
}

// Counts down the wait between quest requests and repeats a held scroll arrow.
void DialogController::Tick()
{
    if (request_cooldown > 0)
        request_cooldown--;
    if (scroll_repeat_delay > 0)
        scroll_repeat_delay--;
    else
    {
        if (scroll_up_held)
            ScrollUp();
        if (scroll_down_held)
            ScrollDown();
    }
}

int DialogController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int DialogController::GetTopLine()
{
    return top_line;
}

// A click in the window: the quest list button switches between the dialog and the quest list; a reply link sends
// the reply and closes the window; a quest in the list asks for its dialog. Also the scroll bar and moving the
// window.
void DialogController::MouseDown(int x, int y, int button)
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
    if (y > window_y + 13 && y < window_y + 29 && x > window_x + 251 && x < window_x + 270)
    {
        MainForm->sfx->PlaySfx(2);
        if (!showing_quest_list)
            showing_quest_list = true;
        else
            showing_quest_list = false;
        hovered_row = 0;
        top_line = 0;
        scroll_thumb_offset = 0;
        MainForm->redraw_needed = true;
    }
    if (y > window_y + 43 && y < window_y + 143 && x > window_x + 17 && x < window_x + 252)
    {
        int line = top_line + (y - window_y - 47) / 16;
        if (!showing_quest_list)
        {
            for (int slot = 0; slot < DIALOG_MAX_LINKS; slot++)
            {
                if (link_line_indices[slot] != line)
                    continue;
                if (link_line_indices[slot] == -1)
                    continue;
                MainForm->sfx->PlaySfx(2);
                MainForm->net_client->sender->SendQuestDialogLink(dialog_echo_a, quest_id, dialog_echo_b,
                    link_ids[slot]);
                Close();
                MainForm->redraw_needed = true;
            }
        }
        else if (line >= 0 && quest_names->Count > line && request_cooldown <= 0)
        {
            // Picking the quest already shown goes back to its dialog
            if (quest_ids[line] == quest_id)
            {
                showing_quest_list = false;
                hovered_row = 0;
                top_line = 0;
                scroll_thumb_offset = 0;
                MainForm->redraw_needed = true;
            }
            else
            {
                MainForm->sfx->PlaySfx(2);
                MainForm->net_client->sender->RequestQuestDialog(npc_index, quest_ids[line]);
                MainForm->redraw_needed = true;
                request_cooldown = QUEST_REQUEST_COOLDOWN_TICKS;
            }
        }
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
    {
        scroll_up_held = 1;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 128 && y <= window_y + 143)
    {
        scroll_down_held = 1;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 190 && !thumb_grabbed)
    {
        is_dragging = 1;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered line and the previous and next buttons.
void DialogController::MouseMove(int x, int y)
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
        if (window_x >= 346)
            window_x = 346;
        if (window_y >= 119)
            window_y = 119;
        MainForm->redraw_needed = true;
    }
    if (!(y <= window_y + 43 || y >= window_y + 143 || x <= window_x + 17 || x >= window_x + 252 || is_dragging))
    {
        int row = (y - window_y - 47) / 16;
        if (row > 6)
            row = -1;
        int line_count = lines->Count;
        if (showing_quest_list)
            line_count = quest_names->Count;
        if (line_count <= row)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (hovered_row >= 0)
        {
            hovered_row = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < window_x + 89 || x > window_x + 169 || y < window_y + 153 || y > window_y + 179))
    {
        if (!prev_button_hovered)
        {
            prev_button_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (prev_button_hovered)
        {
            prev_button_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < window_x + 183 || x > window_x + 273 || y < window_y + 153 || y > window_y + 179))
    {
        if (!next_button_hovered)
        {
            next_button_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (next_button_hovered)
        {
            next_button_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
void DialogController::UpdateScrollHover(int x, int y)
{
    if (x >= window_x + 252 && x <= window_x + 267)
    {
        if (y >= window_y + 58 && y <= window_y + 129)
            DragScrollThumb(y);
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
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
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 128 && y <= window_y + 143)
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

// Follows a grabbed scroll thumb, kept on its track, and picks the top line of the dialog or the quest list.
void DialogController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_line;
    int __slot1;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 52)
            thumb_offset = 52;
        int line_count = lines->Count;
        if (showing_quest_list)
            line_count = quest_names->Count;
        if (line_count > 6)
        {
            thumb_fraction = thumb_offset / 52.0;
            scaled_line = (line_count - 6.0) * thumb_fraction;
            int new_top_line = scaled_line;
            if (new_top_line != top_line)
                top_line = new_top_line;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the dialog or the quest list up one line and moves the scroll thumb to match.
void DialogController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
    int line_count = lines->Count;
    if (showing_quest_list)
        line_count = quest_names->Count;
    if (line_count > 6 && top_line > 0)
    {
        top_line--;
        scroll_fraction = (double)top_line / (double)(line_count - 6);
        thumb_pos = DIALOG_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the dialog or the quest list down one line and moves the scroll thumb to match.
void DialogController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    int result = lines->Count;
    if (showing_quest_list)
        result = quest_names->Count;
    if (result - 6 > top_line)
    {
        top_line++;
        scroll_fraction = (double)top_line / (double)(result - 6);
        thumb_pos = DIALOG_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void DialogController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

// Empties the page and its link slots and scrolls back to the top.
void DialogController::ClearText()
{
    for (int slot = 0; slot < DIALOG_MAX_LINKS; slot++)
        link_line_indices[slot] = -1;
    lines->Clear();
    line_right_texts->Clear();
    hovered_row = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
}
