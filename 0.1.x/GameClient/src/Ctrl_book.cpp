#include <vcl.h>
#pragma hdrstop

#include "Ctrl_book.h"
#include "Tmainform.h"

#pragma package(smart_init)

BookController::BookController()
{
    lines = new TStringList();
    window_x = 133;
    window_y = 13;
    frame_index = 0;
    admin_level = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    is_visible = false;
    close_hovered = false;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    unused_index = -1;
    unused_flag = true;
    scroll_up_held = false;
    scroll_down_held = false;
}

BookController::~BookController()
{
}

// Shows the book with the player's info lines from the book reply; the reply adds the entries after this.
void BookController::Open(int frame_row, String name_text, String home_text, String partner_text,
    String title_text, String player_class_name, String guild_text, String guild_rank_text, int new_icon_type,
    int new_admin_level)
{
    is_visible = true;
    lines->Clear();
    player_name = name_text;
    home_name = home_text;
    partner_name = partner_text;
    title = title_text;
    class_name = player_class_name;
    guild_name = guild_text;
    guild_rank_name = guild_rank_text;
    frame_index = frame_row;
    icon_type = new_icon_type;
    admin_level = new_admin_level;
    // A text shorter than four characters is not shown
    if (partner_name.Length() < 4)
        partner_name = "";
}

void BookController::Close()
{
    is_visible = false;
    MainForm->redraw_needed = true;
}

// A click on the scroll bar, or anywhere else in the window to start moving it.
void BookController::MouseDown(int x, int y, int button)
{
    char thumb_clicked = false;
    if (x >= window_x + 190 && x <= window_x + 204)
    {
        if (y >= window_y + scroll_thumb_offset + 51 && y <= window_y + scroll_thumb_offset + 66)
        {
            thumb_grabbed = true;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
            thumb_clicked = true;
        }
    }
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 35 && y <= window_y + 50)
    {
        scroll_up_held = true;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 255 && y <= window_y + 270)
    {
        scroll_down_held = true;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 360 && y >= window_y && y <= window_y + 280 && !thumb_clicked)
    {
        is_dragging = true;
        drag_mode = 1;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Drags a grabbed thumb, moves a dragged window, kept on screen, and tracks the close and scroll buttons.
void BookController::MouseMove(int x, int y)
{
    if (x >= window_x + 190 && x <= window_x + 204)
    {
        if (y >= window_y + 51 && y <= window_y + 270)
            DragScrollThumb(y);
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
            close_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (close_hovered)
    {
        close_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 35 && y <= window_y + 50)
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
    if (x >= window_x + 190 && x <= window_x + 205 && y >= window_y + 255 && y <= window_y + 270)
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

// Follows a grabbed scroll thumb, kept on its track, and picks the top line from its position.
void BookController::DragScrollThumb(int y)
{
    int new_top_line;
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
            double thumb_fraction = thumb_offset / 188.0;
            double scaled_line = (line_count - 14.0) * thumb_fraction;
            new_top_line = scaled_line;
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
    int line_count = lines->Count;
    if (line_count > 14 && top_line > 0)
    {
        top_line--;
        double scroll_fraction = (double)top_line / (line_count - 14);
        double thumb_pos = BOOK_THUMB_TRAVEL * scroll_fraction;
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
        double scroll_fraction = (double)top_line / (result - 14);
        double thumb_pos = BOOK_THUMB_TRAVEL * scroll_fraction;
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
    is_dragging = false;
    thumb_grabbed = false;
    scroll_up_held = false;
    scroll_down_held = false;
}
