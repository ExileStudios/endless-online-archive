#include <vcl.h>
#pragma hdrstop

#include "Ctrl_help.h"
#include "Tmainform.h"
#include "Ctrl_chat.h"
#include <io.h>
#pragma package(smart_init)

#define HELP_VISIBLE_LINES 7

HelpController::HelpController()
{
    int new_list;
    int new_list_2;
    int new_list_3;
    int new_list_4;
    int new_list_5;
    lines = new TStringList();
    line_flags = new TStringList();
    file_lines = new TStringList();
    link_targets = new TStringList();
    line_icons = new TStringList();
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_dirty = 0;
    is_dirty = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
}

// Empties the page's lists.
HelpController::~HelpController()
{
    lines->Clear();
    line_flags->Clear();
    line_flags->Clear();
    link_targets->Clear();
    line_icons->Clear();
    file_lines->Clear();
}

// Empties the page and scrolls back to the top.
void HelpController::Clear()
{
    scroll_up_held = 0;
    scroll_down_held = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    file_lines->Clear();
    line_flags->Clear();
    link_targets->Clear();
    lines->Clear();
    line_icons->Clear();
}

int HelpController::GetTopLine()
{
    return top_line;
}

int HelpController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

// A click on a line with a link opens the linked page, which keeps the clicked line's text for its title; also
// the scroll bar.
void HelpController::MouseDown(int x, int y)
{
    if (x >= 124)
    {
        if (x <= 568)
        {
            if (y >= 350)
            {
                if (y <= 447)
                {
                    int line = (y - 350) / 14;
                    if (line < 7)
                    {
                        line = top_line + line;
                        if (link_targets->Strings[line] != "")
                        {
                            MainForm->sfx->PlaySfx(4);
                            clicked_line_text = lines->Strings[line];
                            LoadPage(link_targets->Strings[line]);
                            return;
                        }
                    }
                }
            }
        }
    }
    if (x >= 569 && x <= 585)
    {
        if (y >= scroll_thumb_offset + 366 && y <= scroll_thumb_offset + 381)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset;
            DragScrollThumb(y);
        }
        if (y >= 350 && y <= 365)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (y >= 432 && y <= 447)
        {
            scroll_down_held = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
    return;
}

// Reads a help page from the help folder: each line carries one tag, for bold text, a link, a blank line or the
// title, and lines without a tag are skipped.
void HelpController::LoadPage(String page_name)
{
    Clear();
    try
    {
        String lpFileName = "./help/" + page_name + ".ehf";
        if (!access(lpFileName.c_str(), 0))
            file_lines->LoadFromFile(lpFileName.c_str());
        for (int i = 0; i < file_lines->Count; i++)
        {
            if (file_lines->Strings[i].Length() > 1)
            {
                char line_flag;
                String line = file_lines->Strings[i];
                String link_target = "";
                line_flag = 0;
                char hide_icon = 0;
                int open_pos = line.Pos("<");
                int close_pos = line.Pos(">");
                String tag = "";
                String tag_value = "";
                if (open_pos < 1 || close_pos < 1 || open_pos > close_pos)
                    continue;
                tag = line.SubString(open_pos + 1, close_pos - open_pos - 1);
                line.Delete(open_pos, close_pos - open_pos + 1);
                if (tag == "bold")
                    line_flag = 1;
                int separator_pos = tag.Pos(" ");
                if (separator_pos > 0)
                {
                    tag_value = tag.SubString(separator_pos + 1, tag.Length() - separator_pos);
                    tag = tag.SubString(1, separator_pos - 1);
                    if (tag == "link")
                        link_target = tag_value;
                    if (tag_value == "noicon")
                        hide_icon = 1;
                }
                if (tag == "newline")
                {
                    line_flags->Add("0");
                    link_targets->Add("");
                    lines->Add("");
                    line_icons->Add("0");
                    continue;
                }
                // The title line shows the text of the link that led here
                if (tag == "title")
                {
                    line = clicked_line_text;
                    link_target = "";
                    line_flag = 1;
                }
                AddText(line, link_target, line_flag, hide_icon);
            }
        }
        is_dirty = 1;
    }
    catch (...)
    {
        file_lines->Clear();
    }
}

// Adds a help line, wrapped to the panel width with a hyphen where a word is cut; the first piece of a plain line
// gets the icon.
void HelpController::AddText(String text, String link_target, char line_flag, char hide_icon)
{
    HDC dc;
    int piece_start = 0;
    int j = 0;
    if (text.Length() > 1)
    {
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
                    link_targets->Add(link_target);
                    if (line_flag)
                        line_flags->Add("1");
                    else
                        line_flags->Add("0");
                    if (!piece_index && link_target == "" && !hide_icon)
                        line_icons->Add("1");
                    else
                        line_icons->Add("0");
                }
                MainForm->draw_canvas->Handle = 0;
            }
            catch (Exception &)
            {
            }
            MainForm->OffscreenSurface->ReleaseDC(dc);
        }
        is_dirty = 1;
    }
}

// Repeats a held scroll arrow.
DEFINE_TICK(HelpController)

// Scrolls the page up one line and moves the scroll thumb to match.
void HelpController::ScrollUp()
{
    if (lines->Count > HELP_VISIBLE_LINES && top_line > 0)
    {
        top_line--;
        double scroll_fraction = (double)top_line / (double)(lines->Count - 7);
        double thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        is_dirty = 1;
    }
}

// Scrolls the page down one line and moves the scroll thumb to match.
void HelpController::ScrollDown()
{
    if (lines->Count - 7 > top_line)
    {
        top_line++;
        double scroll_fraction = (double)top_line / (double)(lines->Count - 7);
        double thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        is_dirty = 1;
    }
}

// Follows a grabbed scroll thumb, kept on its track, and picks the top line from its position.
void HelpController::DragScrollThumb(int y)
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
        if (lines->Count > HELP_VISIBLE_LINES)
        {
            thumb_fraction = thumb_offset / PANEL_THUMB_TRAVEL;
            scaled_line = (lines->Count - 7.0) * thumb_fraction;
            int new_top_line = scaled_line;
            if (new_top_line != top_line)
                top_line = new_top_line;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_dirty = 1;
        is_dirty = 1;
    }
}

// Lets go of the held scroll arrows and the grabbed thumb.
void HelpController::MouseUp()
{
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

void HelpController::MouseMove(int x, int y)
{
    UpdateScrollHover(x, y);
}

// Lights the scroll arrow under the mouse, lets go of an arrow the mouse has left, and drags a grabbed thumb.
DEFINE_HELP_UPDATE_SCROLL_HOVER(HelpController)

DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(HelpController)

bool HelpController::TestAndClearTextDirty()
{
    bool was_dirty = 0;
    if (is_dirty)
    {
        is_dirty = 0;
        was_dirty = 1;
    }
    return was_dirty;
}

void HelpController::MarkTextDirty()
{
    is_dirty = 1;
}
