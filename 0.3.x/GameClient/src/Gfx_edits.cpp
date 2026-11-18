#include <vcl.h>
#pragma hdrstop

#include "Gfx_edits.h"
#include "Tmainform.h"

#pragma package(smart_init)

EditCanvas::EditCanvas()
{
    lines = new TStringList();
}

EditCanvas::~EditCanvas()
{
}

// Draws every visible edit box of a screen, or of every screen: its rounded frame, its text and the caret.
void EditCanvas::DrawEditBoxes(int screen_id)
{
    int i;
    int m;
    int caret_offset;
    int bevel_top_y;
    int bevel_left_x;
    TPoint bevel_start;
    int bevel_side_y;
    int bevel_side_x;
    TPoint bevel_side_start;
    int line_count;
    int visible_lines;
    char ends_on_new_line;
    int first_line;
    int row;
    MainForm->edits->MoveFirst();
    do
    {
        Edit handle = MainForm->edits->GetCurrentEdit();
        if (handle.visible)
        {
            if ((int)(handle.screen_group == screen_id || screen_id == EditGroup_All))
            {
                MainForm->text_canvas->Brush->Color = (TColor)handle.background_color;
                if (handle.background_visible)
                    MainForm->text_canvas->Brush->Style = bsSolid;
                else
                {
                    MainForm->text_canvas->Brush->Color = clWhite;
                    MainForm->text_canvas->Brush->Style = bsClear;
                }
                MainForm->text_canvas->Pen->Color = (TColor)handle.border_color;
                if (handle.border_visible)
                    MainForm->text_canvas->Pen->Style = psSolid;
                else
                    MainForm->text_canvas->Pen->Style = psClear;
                if (handle.multi_line)
                {
                    MainForm->text_canvas->Font->Color = (TColor)handle.text_color;
                    MainForm->text_canvas->Font->Name = "MS Sans Serif";
                    MainForm->text_canvas->Font->Size = 7;
                }
                else
                {
                    MainForm->text_canvas->Font->Color = (TColor)handle.text_color;
                    MainForm->text_canvas->Font->Name = "MS Sans Serif";
                    MainForm->text_canvas->Font->Size = 8;
                }
                MainForm->text_canvas->RoundRect(handle.x, handle.y, handle.x + handle.width, handle.y + handle.height,
                    4, 4);
                bevel_top_y = handle.y + 1;
                bevel_left_x = handle.x + 1;
                bevel_start.x = bevel_left_x;
                bevel_start.y = bevel_top_y;
                MainForm->text_canvas->PenPos = bevel_start;
                MainForm->text_canvas->Pen->Color = (TColor)handle.caret_color;
                if (handle.background_visible)
                {
                    MainForm->text_canvas->LineTo(handle.x + handle.width - 1, handle.y + 1);
                    bevel_side_y = handle.y + 1;
                    bevel_side_x = handle.x + 1;
                    bevel_side_start.x = bevel_side_x;
                    bevel_side_start.y = bevel_side_y;
                    MainForm->text_canvas->PenPos = bevel_side_start;
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
                    i = 1;
                    while (i <= handle.text.Length())
                    {
                        display_text = display_text + "*";
                        i++;
                    }
                }
                if (handle.case_mode == 4)
                {
                    if (display_text.Length() < 4)
                    {
                        String upper_text = "";
                        for (int letter_index = 1; letter_index <= display_text.Length() && letter_index < 4;
                            letter_index++)
                            upper_text = upper_text + UpperCase((char)display_text[letter_index]);
                        display_text = upper_text;
                    }
                }
                int text_width = MainForm->text_canvas->TextWidth(display_text);
                line_count = 0;
                ends_on_new_line = false;
                first_line = 0;
                row = 0;
                if (handle.multi_line)
                {
                    // A multi-line box wraps at a space or line break to the box width.
                    // It shows the last lines that fit, moved up by the scroll offset.
                    if (display_text.Length() > 0)
                    {
                        if (display_text[display_text.Length()] == '\r')
                            ends_on_new_line = true;
                    }
                    lines->Clear();
                    int line_height = MainForm->text_canvas->TextHeight("T");
                    visible_lines = (handle.height - line_height) / (line_height + 3);
                    do
                    {
                        text_width = MainForm->text_canvas->TextWidth(display_text);
                        // A single-line text wider than the box shows only its end, so the caret end stays in view.
                        if (text_width < handle.width - 18)
                        {
                            char new_line_found = false;
                            if (display_text.Length() > 0)
                            {
                                text_width = 1;
                                while (text_width <= display_text.Length() && !new_line_found)
                                {
                                    if (display_text[text_width] == '\r')
                                    {
                                        if (text_width == 1)
                                            handle.line_buffer = "";
                                        else
                                            handle.line_buffer = display_text.SubString(1, text_width - 1);
                                        display_text.Delete(1, text_width);
                                        new_line_found = true;
                                    }
                                    text_width++;
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
                            for (int j = 1; j <= display_text.Length(); j++)
                            {
                                int prefix_width;
                                prefix_width = MainForm->text_canvas->TextWidth(display_text.SubString(1, j));
                                if (handle.width - 18 <= prefix_width || display_text[j] == '\r')
                                {
                                    if (display_text[j] != ' ')
                                    {
                                        char break_found = false;
                                        m = j;
                                        while (m > 0 && !break_found)
                                        {
                                            if (display_text[m] == ' ' || display_text[m] == '\r')
                                            {
                                                if (display_text[m] == '\r')
                                                {
                                                    prefix_width =
                                                        MainForm->text_canvas->TextWidth(display_text.SubString(1,
                                                            m - 1));
                                                    if (handle.width - 18 > prefix_width)
                                                    {
                                                        handle.line_buffer = display_text.SubString(1, m - 1);
                                                        display_text.Delete(1, m);
                                                        break_found = true;
                                                    }
                                                }
                                                else
                                                {
                                                    handle.line_buffer = display_text.SubString(1, m);
                                                    display_text.Delete(1, m);
                                                    break_found = true;
                                                }
                                            }
                                            m--;
                                        }
                                        if (!break_found)
                                        {
                                            handle.line_buffer = display_text.SubString(1, j);
                                            display_text.Delete(1, j);
                                        }
                                    }
                                    else
                                    {
                                        handle.line_buffer = display_text.SubString(1, j);
                                        display_text.Delete(1, j);
                                    }
                                    break;
                                }
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
                    for (int line_index = first_line; line_index < lines->Count - lines_below; line_index++)
                    {
                        MainForm->text_canvas->TextOut(handle.x + 5, (line_height + 3) * row + handle.y + 3,
                            lines->Strings[line_index]);
                        row++;
                    }
                    MainForm->edits->SetFirstVisibleLine(handle.id, first_line);
                    MainForm->edits->SetLineCount(handle.id, lines->Count);
                    MainForm->edits->SetVisibleLineCount(handle.id, visible_lines);
                }
                else
                {
                    if (text_width < handle.width - 18)
                        handle.line_buffer = display_text;
                    else
                    {
                        int overflow_width = text_width - (handle.width - 18);
                        int trimmed_width = 0;
                        for (int jj = 1; jj <= display_text.Length(); jj++)
                        {
                            trimmed_width += MainForm->text_canvas->TextWidth((char)display_text[jj]);
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
                if (handle.focused && handle.editable)
                {
                    caret_offset = MainForm->text_canvas->TextWidth(handle.line_buffer);
                    if (MainForm->edits->caret_blink_on && MainForm->edits->caret_enabled)
                    {
                        MainForm->text_canvas->Brush->Color = (TColor)handle.caret_color;
                        MainForm->text_canvas->Pen->Color = (TColor)handle.caret_color;
                        if (handle.multi_line)
                        {
                            if (!handle.scroll_offset)
                            {
                                int caret_line_height = MainForm->text_canvas->TextHeight("T");
                                if (ends_on_new_line)
                                    caret_offset = 0;
                                MainForm->text_canvas->Rectangle(handle.x + caret_offset + 6,
                                    (line_count - first_line) * (caret_line_height + 3) + handle.y + 4,
                                    handle.x + caret_offset + 8,
                                    (line_count - first_line) * (caret_line_height + 3) +
                                        (handle.y + caret_line_height) + 2);
                            }
                        }
                        else
                            MainForm->text_canvas->Rectangle(handle.x + caret_offset + 6, handle.y + 4,
                                handle.x + caret_offset + 8, handle.y + handle.height - 4);
                    }
                }
            }
        }
        MainForm->edits->MoveNext();
    } while (!MainForm->edits->IsAtEnd());
}
