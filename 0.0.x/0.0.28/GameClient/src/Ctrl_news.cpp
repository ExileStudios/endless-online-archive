#include <vcl.h>
#pragma hdrstop

#include "Ctrl_news.h"
#include "Tmainform.h"
#include "Ctrl_chat.h"
#pragma package(smart_init)

#define NEWS_VISIBLE_LINES 7

NewslistController::NewslistController()
{
    int new_list;
    int new_list_2;
    lines = new TStringList();
    line_markers = new TStringList();
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_dirty = 0;
    text_dirty = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    scroll_hint_active = false;
}

NewslistController::~NewslistController()
{
    lines->Clear();
    line_markers->Clear();
}

int NewslistController::GetLineCount()
{
    return lines->Count;
}

// Empties the news and resets the scroll bar.
void NewslistController::Clear()
{
    scroll_up_held = 0;
    scroll_down_held = 0;
    scroll_hint_active = false;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    lines->Clear();
    line_markers->Clear();
}

int NewslistController::GetTopLine()
{
    return top_line;
}

int NewslistController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

// A click on the scroll thumb grabs it; one on an arrow scrolls a line and keeps scrolling while held.
void NewslistController::MouseDown(int x, int y)
{
    if (x >= 569 && x <= 585)
    {
        if (y >= scroll_thumb_offset + 366 && y <= scroll_thumb_offset + 381)
        {
            thumb_grabbed = 1;
            scroll_hint_active = false;
            scroll_hint_frame = 0;
            thumb_grab_offset = y - scroll_thumb_offset;
            DragScrollThumb(y);
        }
        if (y >= 350 && y <= 365)
        {
            scroll_up_held = 1;
            scroll_hint_active = false;
            scroll_hint_frame = 0;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (y >= 432 && y <= 447)
        {
            scroll_down_held = 1;
            scroll_hint_active = false;
            scroll_hint_frame = 0;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
}

// Adds one news item wrapped to the panel's width, with a blank line before each item after the first.
void NewslistController::AddNewsItem(String text)
{
    HDC dc;
    int piece_start = 0;
    int j = 0;
    if (text.Length() > 1)
    {
        if (lines->Count > 0)
        {
            lines->Add("");
            line_markers->Add("0");
        }
        int max_width = 420;
        int piece_count = 0;
        int next_start = 1;
        int continuation_indent = 0;
        String wrapped_pieces[MAX_WRAP_PIECES];
        if (MainForm->OffscreenSurface->GetDC(&dc))
        {
            try
            {
                MainForm->draw_canvas->Handle = dc;
                MainForm->draw_canvas->Brush->Style = bsClear;
                MainForm->draw_canvas->Font->Color = clBlack;
                MainForm->draw_canvas->Font->Name = "MS Sans Serif";
                MainForm->draw_canvas->Font->Size = 8;
                if (MainForm->draw_canvas->TextWidth(text) >= max_width)
                {
                    for (int i = 0; i <= 12; i++)
                    {
                        if (text.Length() < next_start)
                            continue;
                        piece_count++;
                        piece_start = next_start;
                        char searching = 1;
                        for (j = next_start; j <= text.Length() && searching; j++)
                        {
                            int piece_indent = 0;
                            if (i > 0)
                                piece_indent = continuation_indent;
                            if ((text[j] == ' ' &&
                                MainForm->draw_canvas->TextWidth(text.SubString(piece_start, j - piece_start)) >=
                                max_width - 40 - piece_indent) ||
                                // A line breaks at a space once nearly full, or inside a word with a hyphen once full
                                MainForm->draw_canvas->TextWidth(text.SubString(piece_start, j - piece_start)) >=
                                max_width - piece_indent || j == text.Length())
                            {
                                if (MainForm->draw_canvas->TextWidth(text.SubString(piece_start, j - piece_start)) <=
                                    max_width || j == text.Length())
                                {
                                    wrapped_pieces[i] = text.SubString(piece_start, j - piece_start + 1);
                                    next_start = j + 1;
                                    searching = 0;
                                }
                                else
                                {
                                    wrapped_pieces[i] = text.SubString(piece_start, j - piece_start + 1) + "-";
                                    next_start = j + 1;
                                    searching = 0;
                                }
                            }
                        }
                    }
                }
                else
                {
                    piece_count = 1;
                    wrapped_pieces[0] = text;
                }
                for (int piece_index = 0; piece_index < piece_count; piece_index++)
                {
                    lines->Add(wrapped_pieces[piece_index]);
                    if (!piece_index)
                        // The first line of an item is drawn with a bullet
                        line_markers->Add("1");
                    else
                        line_markers->Add("0");
                }
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            MainForm->OffscreenSurface->ReleaseDC(dc);
        }
        text_dirty = 1;
    }
}

// Steps the blinking hint and repeats a held arrow after a short delay.
void NewslistController::Tick()
{
    if (scroll_hint_active)
    {
        scroll_hint_frame++;
        if (scroll_hint_frame > 3)
            scroll_hint_frame = -1;
        scroll_dirty = 1;
    }
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

// Scrolls the news up one line and moves the scroll thumb to match.
void NewslistController::ScrollUp()
{
    if (lines->Count > NEWS_VISIBLE_LINES && top_line > 0)
    {
        top_line--;
        double scroll_fraction = (double)top_line / (double)(lines->Count - 7);
        double thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        text_dirty = 1;
    }
}

// Scrolls the news down one line and moves the scroll thumb to match.
void NewslistController::ScrollDown()
{
    if (lines->Count - 7 > top_line)
    {
        top_line++;
        double scroll_fraction = (double)top_line / (double)(lines->Count - 7);
        double thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        text_dirty = 1;
    }
}

// Follows a grabbed scroll thumb, kept on its track, and picks the top line from its position.
void NewslistController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_line;
    int __slot1;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 50)
            thumb_offset = 50;
        if (lines->Count > NEWS_VISIBLE_LINES)
        {
            thumb_fraction = thumb_offset / PANEL_THUMB_TRAVEL;
            scaled_line = (lines->Count - 7.0) * thumb_fraction;
            int new_top_line = scaled_line;
            if (new_top_line != top_line)
                top_line = new_top_line;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_dirty = 1;
        text_dirty = 1;
    }
}

// Lets go of the thumb and the held arrows.
void NewslistController::MouseUp()
{
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

void NewslistController::MouseMove(int x, int y)
{
    UpdateScrollHover(x, y);
}

// Drags a grabbed thumb and tracks the hovered arrows; moving off an arrow stops its scrolling.
DEFINE_HELP_UPDATE_SCROLL_HOVER(NewslistController)

// Tells whether the scroll bar needs drawing again, and clears the flag.
DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(NewslistController)

// Tells whether the news text needs drawing again, and clears the flag.
bool NewslistController::TestAndClearTextDirty()
{
    bool was_dirty = 0;
    if (text_dirty)
    {
        text_dirty = 0;
        was_dirty = 1;
    }
    return was_dirty;
}

void NewslistController::MarkTextDirty()
{
    text_dirty = 1;
}

// Starts blinking the down arrow when the news is longer than the panel.
void NewslistController::StartScrollHint()
{
    if (lines->Count > NEWS_VISIBLE_LINES)
    {
        scroll_hint_active = true;
        scroll_hint_frame = 0;
    }
    else
        scroll_hint_active = false;
}

// Whether the blinking hint is lit in this frame.
bool NewslistController::IsScrollHintVisible()
{
    bool visible = 0;
    if (scroll_hint_frame > 1)
        visible = 1;
    return visible;
}
