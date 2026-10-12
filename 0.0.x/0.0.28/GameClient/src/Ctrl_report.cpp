#include <vcl.h>
#pragma hdrstop

#include "Ctrl_report.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_law.h"
#pragma package(smart_init)

void Ctrl_report_UnusedLiterals1()
{
    "#";
    "#";
    " [";
    "]";
    ",";
    " ";
    "";
    "/";
    "/";
    " ";
    "";
}

ReportController::ReportController()
{
    int new_list;
    int new_list_2;
    int new_list_3;
    lines = new TStringList();
    line_values = new TStringList();
    unused_lines = new TStringList();
    link_line_indices[0] = -1;
    link_line_indices[1] = -1;
    link_line_indices[2] = -1;
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

ReportController::~ReportController()
{
}

// Adds a text wrapped to the window's width, with a value shown at the right end of each of its lines.
void ReportController::AddText(String text, String right_text, bool add_blank_line)
{
    if (text.Length() > 0)
    {
        // A text starting with a star becomes the next free link
        if (text[1] == '*')
        {
            for (int slot = 0; slot < REPORT_MAX_LINKS; slot++)
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

// Shows the window with a title and no lines, closing other npc windows.
void ReportController::Open(String window_title)
{
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->learn->is_visible = 0;
    MainForm->animation->report->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadReportGraphics();
    lines->Clear();
    line_values->Clear();
    link_line_indices[0] = -1;
    link_line_indices[1] = -1;
    link_line_indices[2] = -1;
    top_line = 0;
    scroll_thumb_offset = 0;
    title = window_title;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void ReportController::Close()
{
    is_visible = 0;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Repeats a held scroll arrow after a short delay.
DEFINE_TICK(ReportController)

int ReportController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int ReportController::GetTopLine()
{
    return top_line;
}

// A click on the thumb grabs it, one on a link follows it, and one on an arrow scrolls; any click in the window
// while the thumb is not grabbed also starts moving it.
void ReportController::MouseDown(int x, int y, int button)
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
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 252)
    {
        int line = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
        // The first link opens a web page, the second the player report form, the third the question box
        if (line == link_line_indices[0])
        {
            MainForm->sfx->PlaySfx(4);
            ShellExecuteA(Application->Handle, "open", "http://www.endless-online.com/pwrecovery.html", 0, 0, 3);
        }
        if (line == link_line_indices[1])
        {
            MainForm->sfx->PlaySfx(4);
            MainForm->animation->game->OpenInputForm(FormKind_ReportPlayer, 2, 2, 32, "Report Player",
                "Who do you want to report, and why?");
            MainForm->animation->game->AddInputFormRow("Name", "", 2, 16, 0, 0);
            MainForm->animation->game->AddInputFormRow("Reason", "", 1, 48, 0, 0);
        }
        if (line == link_line_indices[2])
        {
            MainForm->sfx->PlaySfx(4);
            MainForm->animation->game->OpenInputBox(InputBoxKind_HelpQuestion,
                "Please enter your question, and we will try to get back to you.", 1, 48);
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

DEFINE_MOUSE_MOVE(ReportController)

// Drags a grabbed thumb and tracks the hovered arrows.
DEFINE_UPDATE_SCROLL_HOVER(ReportController)

DEFINE_MEMO_DRAG_SCROLL_THUMB(ReportController)

// Scrolls the text up one line and moves the scroll thumb to match.
DEFINE_MEMO_SCROLL_UP(ReportController)

// Scrolls the text down one line and moves the scroll thumb to match.
DEFINE_MEMO_SCROLL_DOWN(ReportController)

// Ends moving the window, the grabbed thumb and the held scroll arrows.
void ReportController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}
