#include <vcl.h>
#pragma hdrstop

#include "Ctrl_book.h"
#include "Tmainform.h"
#pragma package(smart_init)
#define BOOK_THUMB_TRAVEL 188.0

BookController::BookController()
{
    int new_list;
    lines = new TStringList();
    window_x = 133;
    window_y = 13;
    frame_index = 0;
    extra_value = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    is_visible = 0;
    close_hovered = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    unused_index = -1;
    unused_flag = 1;
    scroll_up_held = 0;
    scroll_down_held = 0;
}

BookController::~BookController()
{
}

// Shows the book with the player's info lines from the book reply; the reply adds the entries after this.
void BookController::Open(int frame_byte, String line_1_text, String line_2_text, String line_4_text, String line_5_text, String player_class_name, String line_7_text, String line_8_text,
    int new_icon_type, int unused_value)
{
    is_visible = 1;
    lines->Clear();
    info_line_1 = line_1_text;
    info_line_2 = line_2_text;
    info_line_4 = line_4_text;
    info_line_5 = line_5_text;
    class_name = player_class_name;
    info_line_7 = line_7_text;
    info_line_8 = line_8_text;
    frame_index = frame_byte;
    icon_type = new_icon_type;
    extra_value = unused_value;
    // A text shorter than four characters is not shown
    if (info_line_4.Length() < 4)
        info_line_4 = "";
}

// A click on the scroll bar, or anywhere else in the window to start moving it.
void BookController::MouseDown(int x, int y, int button)
{
    char thumb_clicked = 0;
    if (x >= window_x + 190 && x <= window_x + 204)
    {
        if (y >= window_y + scroll_thumb_offset + 51 && y <= window_y + scroll_thumb_offset + 66)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
            thumb_clicked = 1;
        }
    }
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 35 && y <= window_y + 50)
    {
        scroll_up_held = 1;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 255 && y <= window_y + 270)
    {
        scroll_down_held = 1;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 360 && y >= window_y && y <= window_y + 280 && !thumb_clicked)
    {
        is_dragging = 1;
        drag_mode = 1;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Drags a grabbed thumb, moves a dragged window, kept on screen, and tracks the close and scroll buttons.
void BookController::MouseMove(int x, int y)
{
    if (x >= this->window_x + 190 && x <= this->window_x + 204)
    {
        if (y >= this->window_y + 51 && y <= this->window_y + 270)
            this->DragScrollThumb(y);
    }
    if (is_dragging && drag_mode == 1)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 250)
            window_x = 250;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 277 && x <= window_x + 367 && y >= window_y + 254 && y <= window_y + 281)
    {
        if (!close_hovered)
        {
            close_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else if (close_hovered)
    {
        close_hovered = 0;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 35 && y <= window_y + 50)
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
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 255 && y <= window_y + 270)
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

// Follows a grabbed scroll thumb, kept on its track, and picks the top line from its position.
void BookController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_line;
    int __slot1;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 187)
            thumb_offset = 188;
        int line_count = lines->Count;
        if (line_count > 14)
        {
            thumb_fraction = thumb_offset / 188.0;
            scaled_line = (line_count - 14.0) * thumb_fraction;
            int new_top_line = scaled_line;
            if (new_top_line != top_line)
                top_line = new_top_line;
        }
        scroll_thumb_offset = thumb_offset;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the entries up one line and moves the scroll thumb to match.
void BookController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
    int line_count = lines->Count;
    if (line_count > 14 && top_line > 0)
    {
        top_line--;
        scroll_fraction = (double)top_line / (double)(line_count - 14);
        thumb_pos = BOOK_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the entries down one line and moves the scroll thumb to match.
void BookController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    int result = lines->Count;
    if (result - 14 > top_line)
    {
        top_line++;
        scroll_fraction = (double)top_line / (double)(result - 14);
        thumb_pos = BOOK_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        MainForm->redraw_needed = true;
    }
}

int BookController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int BookController::GetTopLine()
{
    return top_line;
}

// Ends moving the window, the grabbed thumb and the held scroll arrows.
void BookController::MouseUp()
{
    is_dragging = 0;
    thumb_grabbed = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
}
