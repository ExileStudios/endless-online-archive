#include <vcl.h>
#pragma hdrstop

#include "Ctrl_report.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_law.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_shop.h"
#include "Ctrl_trade.h"

#pragma package(smart_init)

ReportController::ReportController()
{
    lines = new TStringList();
    line_values = new TStringList();
    unused_lines = new TStringList();
    link_line_indices[0] = -1;
    report_link_line = -1;
    question_link_line = -1;
    window_x = 269;
    window_y = 16;
    unused_flag = true;
    unused_value = 0;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    scroll_changed = false;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_line = -1;
    is_visible = false;
    close_hovered = false;
    scroll_up_held = false;
    scroll_down_held = false;
    is_dragging = false;
}

ReportController::~ReportController()
{
}

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

// Adds a text wrapped to the window's width, with a value shown at the right end of each of its lines; a link line,
// marked by a leading star, takes the next free link slot.
void ReportController::AddText(String text, String right_text, bool add_blank_line)
{
    if (text.Length() > 0 && text[1] == '*')
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
    int max_width = 200;
    if (MainForm->fonts->BeginOffscreenIf(200))
    {
        try
        {
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            do
            {
                String fitted_text;
                if ((short)MainForm->fonts->TextWidth(text) <= max_width)
                {
                    lines->Add(text);
                    line_values->Add(right_text);
                    text = "";
                }
                else
                {
                    char split_done = false;
                    for (int i = 1; i <= text.Length(); i++)
                    {
                        if (text[i] == ' ' || i == text.Length())
                        {
                            String prefix = text.SubString(1, i);
                            if ((short)MainForm->fonts->TextWidth(prefix) >= max_width)
                            {
                                lines->Add(fitted_text);
                                line_values->Add(right_text);
                                text.Delete(1, fitted_text.Length());
                                split_done = true;
                                break;
                            }
                            fitted_text = prefix;
                        }
                    }
                    if (!split_done)
                    {
                        lines->Add(text);
                        line_values->Add(right_text);
                        text = "";
                    }
                }
            } while (text.Length() > 0);
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
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
    MainForm->animation->shop->is_visible = false;
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->learn->is_visible = false;
    MainForm->animation->report->is_visible = false;
    MainForm->animation->inn->is_visible = false;
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
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void ReportController::Close()
{
    is_visible = false;
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

// A click on the thumb grabs it, and one on an arrow scrolls. The links open the web page, the report form or the
// question box. Any click in the window while the thumb is not grabbed also starts moving it.
void ReportController::MouseDown(int x, int y, int button)
{
    if (x >= window_x + 252 && x <= window_x + 267)
    {
        if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
        {
            thumb_grabbed = true;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
    }
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 252)
    {
        int line = (y - window_y - 47) / 16 + top_line;
        // The first link opens a web page, the second the player report form, the third the question box
        if (line == link_line_indices[0])
        {
            String url = "jvvr<11yyy0gpfnguu/qpnkpg0eqo1rytgeqxgt{0jvon";
            for (int i = 1; i <= url.Length(); i++)
            {
                url[i] = url[i] - 1;
                url[i] = url[i] - 1;
            }
            MainForm->sfx->PlaySfx(SfxSound_Select);
            ShellExecuteA(Application->Handle, "open", url.c_str(), 0, 0, SW_SHOWMAXIMIZED);
            return;
        }
        if (line == report_link_line)
        {
            MainForm->sfx->PlaySfx(SfxSound_Select);
            MainForm->animation->game->OpenInputForm(FormKind_ReportPlayer, 2, EditCharFilter_Email, 32,
                "Report Player", "Who do you want to report, and why?");
            MainForm->animation->game->AddInputFormRow("Name", "", EditCharFilter_Email, 16, EditCaseMode_None, false);
            MainForm->animation->game->AddInputFormRow("Reason", "", EditCharFilter_Printable, 48, EditCaseMode_None,
                false);
        }
        if (line == link_line_indices[2])
        {
            MainForm->sfx->PlaySfx(SfxSound_Select);
            MainForm->animation->game->OpenInputBox(InputBoxKind_HelpQuestion,
                "Please enter your question, and we will try to get back to you.", EditCharFilter_Printable, 48);
        }
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
    {
        scroll_up_held = true;
        scroll_repeat_delay = 4;
        ScrollUp();
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
    {
        scroll_down_held = true;
        scroll_repeat_delay = 4;
        ScrollDown();
    }
    if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 290 && !thumb_grabbed)
    {
        is_dragging = true;
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
    is_dragging = false;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
}
