#include <vcl.h>
#pragma hdrstop

#include "Gfx_edits.h"
#include "Tmainform.h"
#pragma package(smart_init)

EditCanvas::EditCanvas()
{
    lines = new TStringList;
}

EditCanvas::~EditCanvas()
{
}

// Draws every visible edit box of a screen, or of every screen: its rounded frame, its text and the caret.
void EditCanvas::DrawEditBoxes(int screen_id)
{
    int i;
    int m;
    MainForm->edits->MoveFirst();
    do
    {
        Edit handle = MainForm->edits->GetCurrentEdit();
        if (handle.visible)
        {
            if ((int)(handle.screen_group == screen_id || screen_id == 240))
            {
                MainForm->text_canvas->Brush->Color = (TColor)(handle.background_color);
                if (handle.background_visible)
                    MainForm->text_canvas->Brush->Style = bsSolid;
                else
                {
                    MainForm->text_canvas->Brush->Color = clWhite;
                    MainForm->text_canvas->Brush->Style = bsClear;
                }
                MainForm->text_canvas->Pen->Color = (TColor)(handle.border_color);
                if (handle.border_visible)
                    MainForm->text_canvas->Pen->Style = psSolid;
                else
                    MainForm->text_canvas->Pen->Style = psClear;
                if (handle.multi_line)
                {
                    MainForm->text_canvas->Font->Color = (TColor)(handle.text_color);
                    MainForm->text_canvas->Font->Name = "MS Sans Serif";
                    MainForm->text_canvas->Font->Size = 7;
                }
                else
                {
                    MainForm->text_canvas->Font->Color = (TColor)(handle.text_color);
                    MainForm->text_canvas->Font->Name = "MS Sans Serif";
                    MainForm->text_canvas->Font->Size = 8;
                }
                MainForm->text_canvas->RoundRect(handle.x, handle.y, handle.x + handle.width, handle.y + handle.height,
                    4, 4);
                MainForm->text_canvas->PenPos = TPoint(handle.x + 1, handle.y + 1);
                MainForm->text_canvas->Pen->Color = (TColor)(handle.caret_color);
                if (handle.background_visible)
                {
                    MainForm->text_canvas->LineTo(handle.x + handle.width - 1, handle.y + 1);
                    MainForm->text_canvas->PenPos = TPoint(handle.x + 1, handle.y + 1);
                    MainForm->text_canvas->LineTo(handle.x + 1, handle.y + handle.height + 1);
                }
                MainForm->text_canvas->Brush->Color = clWhite;
                MainForm->text_canvas->Brush->Style = bsClear;
                String display_text = "";
                // A password box shows one star per character.
                if (!handle.password)
                    display_text = handle.text;
                else
                {
                    for (i = 1; i <= handle.text.Length(); i++)
                        display_text = display_text + "*";
                }
                // In this case mode a short text is shown in capitals, a longer one as typed.
                if (handle.case_mode == 4)
                {
                    if (display_text.Length() < 4)
                    {
                        String upper_text = "";
                        for (i = 1; i <= display_text.Length() && i < 4; i++)
                            upper_text = upper_text + UpperCase(String(display_text[i]));
                        display_text = upper_text;
                    }
                }
                int text_width = MainForm->text_canvas->TextWidth(display_text);
                int line_count = 0;
                int visible_lines;
                char ends_on_new_line = 0;
                int first_line = 0;
                int row = 0;
                // A multi-line box wraps at a space or line break to the box width.
                // It shows the last lines that fit, moved up by the scroll offset.
                if (handle.multi_line)
                {
                    if (display_text.Length() > 0)
                    {
                        if (display_text[display_text.Length()] == '\r')
                            ends_on_new_line = 1;
                    }
                    lines->Clear();
                    int line_height = MainForm->text_canvas->TextHeight("T");
                    visible_lines = (handle.height - line_height) / (line_height + 3);
                    do
                    {
                        text_width = MainForm->text_canvas->TextWidth(display_text);
                        if (text_width < handle.width - 18)
                        {
                            char new_line_found = 0;
                            if (display_text.Length() > 0)
                            {
                                for (i = 1; i <= display_text.Length() && !new_line_found; i++)
                                {
                                    if (display_text[i] != '\r')
                                        continue;
                                    if (i == 1)
                                        handle.line_buffer = "";
                                    else
                                        handle.line_buffer = display_text.SubString(1, i - 1);
                                    display_text.Delete(1, i);
                                    new_line_found = 1;
                                }
                            }
                            if (!new_line_found)
                            {
                                handle.line_buffer = display_text;
                                display_text = "";
                            }
                        }
                        else
                        {
                            int prefix_width;
                            for (int k = 1; k <= display_text.Length(); k++)
                            {
                                prefix_width = MainForm->text_canvas->TextWidth(display_text.SubString(1, k));
                                if (handle.width - 18 > prefix_width)
                                {
                                    if (display_text[k] != '\r')
                                        continue;
                                }
                                if (display_text[k] != ' ')
                                {
                                    char break_found = 0;
                                    for (m = k; m > 0 && !break_found; m--)
                                    {
                                        if (display_text[m] != ' ')
                                        {
                                            if (display_text[m] != '\r')
                                                continue;
                                        }
                                        if (display_text[m] == '\r')
                                        {
                                            prefix_width = MainForm->text_canvas->TextWidth(display_text.SubString(1,
                                                m - 1));
                                            if (handle.width - 18 > prefix_width)
                                            {
                                                handle.line_buffer = display_text.SubString(1, m - 1);
                                                display_text.Delete(1, m);
                                                break_found = 1;
                                            }
                                        }
                                        else
                                        {
                                            handle.line_buffer = display_text.SubString(1, m);
                                            display_text.Delete(1, m);
                                            break_found = 1;
                                        }
                                    }
                                    if (!break_found)
                                    {
                                        handle.line_buffer = display_text.SubString(1, k);
                                        display_text.Delete(1, k);
                                    }
                                }
                                else
                                {
                                    handle.line_buffer = display_text.SubString(1, k);
                                    display_text.Delete(1, k);
                                }
                                break;
                            }
                        }
                        lines->Add(handle.line_buffer);
                        line_count++;
                    } while (display_text.Length() > 0);
                    if (!ends_on_new_line)
                        line_count--;
                    if (visible_lines < line_count)
                        first_line = line_count - visible_lines;
                    first_line -= handle.scroll_offset;
                    int lines_below = handle.scroll_offset;
                    if (first_line < 0)
                    {
                        lines_below -= 0 - first_line;
                        first_line = 0;
                    }
                    for (i = first_line; i < lines->Count - lines_below; i++)
                    {
                        MainForm->text_canvas->TextOut(handle.x + 5, handle.y + row * (line_height + 3) + 3,
                            lines->Strings[i]);
                        row++;
                    }
                    MainForm->edits->SetFirstVisibleLine(handle.id, first_line);
                    MainForm->edits->SetLineCount(handle.id, lines->Count);
                    MainForm->edits->SetVisibleLineCount(handle.id, visible_lines);
                }
                else
                {
                    // A single-line text wider than the box shows only its end, so the caret end stays in view.
                    if (text_width < handle.width - 18)
                        handle.line_buffer = display_text;
                    else
                    {
                        int overflow_width = text_width - (handle.width - 18);
                        int trimmed_width = 0;
                        for (int jj = 1; jj <= display_text.Length(); jj++)
                        {
                            trimmed_width += MainForm->text_canvas->TextWidth(String(display_text[jj]));
                            if (trimmed_width >= overflow_width)
                            {
                                handle.line_buffer = display_text.SubString(jj, display_text.Length());
                                break;
                            }
                        }
                    }
                    MainForm->text_canvas->TextOut(handle.x + 5, handle.y + 3, handle.line_buffer);
                }
                // The caret blinks after the last shown text of a focused, editable box.
                if (handle.focused)
                {
                    if (handle.editable)
                    {
                        text_width = MainForm->text_canvas->TextWidth(handle.line_buffer);
                        if (MainForm->edits->caret_blink_on)
                        {
                            if (MainForm->edits->caret_enabled)
                            {
                                MainForm->text_canvas->Brush->Color = (TColor)(handle.caret_color);
                                MainForm->text_canvas->Pen->Color = (TColor)(handle.caret_color);
                                if (handle.multi_line)
                                {
                                    if (!handle.scroll_offset)
                                    {
                                        int caret_line_height = MainForm->text_canvas->TextHeight("T");
                                        if (ends_on_new_line)
                                            text_width = 0;
                                        MainForm->text_canvas->Rectangle(text_width + handle.x + 6,
                                            handle.y + (line_count - first_line) * (caret_line_height + 3) + 4,
                                            text_width + handle.x + 8,
                                            handle.y + caret_line_height +
                                            (line_count - first_line) * (caret_line_height + 3) + 2);
                                    }
                                }
                                else
                                    MainForm->text_canvas->Rectangle(text_width + handle.x + 6, handle.y + 4,
                                        text_width + handle.x + 8, handle.y + handle.height - 4);
                            }
                        }
                    }
                }
            }
        }
        MainForm->edits->MoveNext();
    } while (!MainForm->edits->IsAtEnd());
}
