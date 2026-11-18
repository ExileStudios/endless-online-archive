#include <vcl.h>
#pragma hdrstop

#include "Sys_edit.h"

#pragma package(smart_init)

void Sys_edit_UnusedLiterals1()
{
    "";
    "";
    "";
}

// A new box starts focused, visible and editable, with the default colours and length limit.
Edit::Edit(int edit_id, int group, int left, int top, int box_width, int box_height)
{
    focused = true;
    id = edit_id;
    screen_group = group;
    x = left;
    y = top;
    width = box_width;
    height = box_height;
    case_mode = 0;
    multi_line = false;
    pending_scroll = -1;
    scroll_offset = 0;
    numeric = false;
    char_filter = 0;
    allow_spaces = true;
    no_double_spaces = false;
    no_leading_space = false;
    border_visible = true;
    background_visible = true;
    editable = true;
    dirty = true;
    change_notify = false;
    password = 0;
    show_strength = false;
    redraw_ticks = 0;
    visible = true;
    text_color = 0;
    max_length = 64;
    extra_mode = 0;
    label = "";
    text = "";
    line_buffer = "";
}

Edit::~Edit()
{
}

void Edit::AppendChar(char ch)
{
    text = text + ch;
}

void Edit::ClearText()
{
    text = "";
}

void Edit::RemoveLastChar()
{
    if (text.Length() >= 1)
    {
        String shortened_text;
        for (int i = 1; i < text.Length(); i++)
            shortened_text = shortened_text + text[i];
        text = shortened_text;
    }
}
