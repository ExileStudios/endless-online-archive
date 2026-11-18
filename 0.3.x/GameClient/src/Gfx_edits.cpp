#include <vcl.h>
#pragma hdrstop

#include "Gfx_edits.h"
#include "Tmainform.h"
#include "Gfx_game.h"

#pragma package(smart_init)

// Creates the surface the edit boxes' texts are drawn on, and its empty list of lines.
EditCanvas::EditCanvas()
{
    lines = new TStringList();
    surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 410;
    surface_desc->Height = 20;
    surface->GetSurfaceDesc(surface_desc);
    surface->Create(surface_desc, MainForm->Dx_Draw1);
    end_rect.left = 297;
    end_rect.right = 300;
    end_rect.top = 0;
    end_rect.bottom = 20;
    caret_rect.left = 302;
    caret_rect.right = 304;
    caret_rect.top = 2;
    caret_rect.bottom = 12;
    meter_rect.left = 313;
    meter_rect.right = 362;
    meter_rect.top = 0;
    meter_rect.bottom = 19;
    meter_back_rect.left = 321;
    meter_back_rect.right = 361;
    meter_back_rect.top = 1;
    meter_back_rect.bottom = 2;
}

EditCanvas::~EditCanvas()
{
    surface->Destroy();
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// The rectangle of an edit box's text surface: the width less 3, at most 295, and 20 high.
TRect EditCanvas::GetBoxRect(short width)
{
    TRect rect;
    if (width > 298)
        width = 298;
    width -= 3;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = 20;
    return rect;
}

// The place of the strength meter's fill for the level, 0 to 10, on the edit sheet.
TRect EditCanvas::GetFillRect(int level)
{
    TRect rect;
    if (level < 0)
        level = 0;
    if (level > 10)
        level = 10;
    rect.left = 370;
    rect.right = 4 * level + 370;
    int row = level / 2;
    if (row > 5)
        row = 5;
    rect.top = 2 * row;
    rect.bottom = 2 * row + 1;
    return rect;
}

void EditCanvas::LoadResource()
{
    int file = MainForm->gfx_vault->OpenLibrary(GfxLibrary_GameInterface);
    if (file)
    {
        int file_id = file;
        if (!MainForm->gfx_transfers->LoadResourceBitmap(file_id, InterfaceBitmap_ResourceBitmap))
            return;
        if (MainForm->gfx_transfers->BeginDraw(surface))
        {
            MainForm->gfx_transfers->Draw(surface);
            MainForm->gfx_transfers->EndDraw(surface);
        }
    }
    surface->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the boxes of the screen, their texts and the caret; the message board also draws its own text.
void EditCanvas::DrawEditBoxes(int screen_id)
{
    DrawBoxes(screen_id);
    if (MainForm->fonts->BeginOffscreenIf(2))
    {
        try
        {
            if (screen_id == EditGroup_MsgBoard)
                MainForm->gfx->game->DrawMsgBoardText();
            DrawTexts(screen_id);
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
    if (caret_visible)
        DrawCaret();
}

void EditCanvas::DrawCaret()
{
    MainForm->OffscreenSurface->BltFast(caret_x, caret_y, surface, &caret_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the border of every visible edit box on the screen, or on all screens for 240, and a password box's strength
// meter.
void EditCanvas::DrawBoxes(int screen_id)
{
    MainForm->edits->MoveFirst();
    do
    {
        Edit edit = MainForm->edits->GetCurrentEdit();
        if (edit.visible)
        {
            if ((int)(edit.screen_group == screen_id || screen_id == EditGroup_All))
            {
                if (edit.border_visible)
                {
                    MainForm->OffscreenSurface->BltFast(edit.x, edit.y, surface, &GetBoxRect(edit.width),
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(edit.x + edit.width - 3, edit.y, surface, &end_rect,
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                }
                if (edit.background_visible)
                {
                }
                if (edit.show_strength)
                {
                    MainForm->blitter->AlphaBlt(edit.x + edit.width - 51, edit.y + 1, MainForm->OffscreenSurface,
                        surface, &meter_rect, 25);
                    int length_level = MainForm->edits->GetPasswordStrength(edit.text);
                    MainForm->OffscreenSurface->BltFast(edit.x + edit.width - 46, edit.y + edit.height - 3, surface,
                        &meter_back_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    MainForm->OffscreenSurface->BltFast(edit.x + edit.width - 46, edit.y + edit.height - 3, surface,
                        &GetFillRect(length_level), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                }
            }
        }
        MainForm->edits->MoveNext();
    } while (!MainForm->edits->IsAtEnd());
}

// Draws the text of every visible edit box of the screen: passwords as stars, multi-line boxes wrapped at spaces with
// their scroll, the password strength where asked, and the blinking caret in the focused box.
void EditCanvas::DrawTexts(int screen_id)
{
    int i;
    int line;
    int max_lines;
    char ends_in_return;
    int first_line;
    int drawn;
    caret_visible = false;
    MainForm->edits->MoveFirst();
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    do
    {
        Edit edit = MainForm->edits->GetCurrentEdit();
        if (edit.visible)
        {
            if ((int)(edit.screen_group == screen_id || screen_id == EditGroup_All))
            {
                MainForm->fonts->color = edit.text_color;
                String text = "";
                // A password box shows one star per character.
                if (!edit.password)
                    text = edit.text;
                else
                {
                    for (i = 1; i <= edit.text.Length(); i++)
                        text = text + "*";
                }
                int text_width = (short)MainForm->fonts->TextWidth(text);
                line = 0;
                ends_in_return = false;
                first_line = 0;
                drawn = 0;
                if (edit.multi_line)
                {
                    if (text.Length() > 0)
                    {
                        if (text[text.Length()] == '\r')
                            ends_in_return = true;
                    }
                    lines->Clear();
                    int line_height = (short)MainForm->fonts->TextHeight("T");
                    max_lines = (edit.height - line_height) / (line_height + 3);
                    do
                    {
                        if ((short)MainForm->fonts->TextWidth(text) < edit.width - 18)
                        {
                            char fits = false;
                            if (text.Length() > 0)
                            {
                                for (i = 1; i <= text.Length() && fits == 0; i++)
                                {
                                    if (text[i] == '\r')
                                    {
                                        if (i == 1)
                                            edit.line_buffer = "";
                                        else
                                            edit.line_buffer = text.SubString(1, i - 1);
                                        text.Delete(1, i);
                                        fits = true;
                                    }
                                }
                            }
                            if (!fits)
                            {
                                edit.line_buffer = text;
                                text = "";
                            }
                        }
                        else
                        {
                            for (int j = 1; j <= text.Length(); j++)
                            {
                                int piece_width;
                                piece_width = (short)MainForm->fonts->TextWidth(text.SubString(1, j));
                                if (edit.width - 18 <= piece_width || text[j] == '\r')
                                {
                                    if (text[j] != ' ')
                                    {
                                        char break_found = false;
                                        for (i = j; i > 0 && break_found == 0; i--)
                                        {
                                            if (text[i] == ' ' || text[i] == '\r')
                                            {
                                                if (text[i] == '\r')
                                                {
                                                    piece_width =
                                                        (short)MainForm->fonts->TextWidth(text.SubString(1, i - 1));
                                                    if (edit.width - 18 > piece_width)
                                                    {
                                                        edit.line_buffer = text.SubString(1, i - 1);
                                                        text.Delete(1, i);
                                                        break_found = true;
                                                    }
                                                }
                                                else
                                                {
                                                    edit.line_buffer = text.SubString(1, i);
                                                    text.Delete(1, i);
                                                    break_found = true;
                                                }
                                            }
                                        }
                                        if (!break_found)
                                        {
                                            edit.line_buffer = text.SubString(1, j);
                                            text.Delete(1, j);
                                        }
                                    }
                                    else
                                    {
                                        edit.line_buffer = text.SubString(1, j);
                                        text.Delete(1, j);
                                    }
                                    break;
                                }
                            }
                        }
                        lines->Add(edit.line_buffer);
                        line++;
                    } while (text.Length() > 0);
                    if (!ends_in_return)
                        line--;
                    if (max_lines < line)
                        first_line = line - max_lines;
                    first_line -= edit.scroll_offset;
                    int scroll = edit.scroll_offset;
                    if (first_line < 0)
                    {
                        scroll -= 0 - first_line;
                        first_line = 0;
                    }
                    for (i = first_line; i < lines->Count - scroll; i++)
                    {
                        MainForm->fonts->DrawString(edit.x + 5, (short)((line_height + 3) * drawn) + (short)edit.y + 3,
                            lines->Strings[i]);
                        drawn++;
                    }
                    MainForm->edits->SetFirstVisibleLine(edit.id, first_line);
                    MainForm->edits->SetLineCount(edit.id, lines->Count);
                    MainForm->edits->SetVisibleLineCount(edit.id, max_lines);
                }
                else
                {
                    // A single-line text wider than the box shows only its end, so the caret end stays in view.
                    if (text_width < edit.width - 18)
                        edit.line_buffer = text;
                    else
                    {
                        int overflow = text_width - (edit.width - 18);
                        int cut_width = 0;
                        for (int c = 1; c <= text.Length(); c++)
                        {
                            cut_width += (short)MainForm->fonts->TextWidth(text[c]);
                            if (cut_width >= overflow)
                            {
                                edit.line_buffer = text.SubString(c, text.Length());
                                break;
                            }
                        }
                    }
                    MainForm->fonts->DrawString(edit.x + 5, (short)edit.y + 3, edit.line_buffer);
                    if (edit.show_strength)
                    {
                        int text_x = edit.width - 55;
                        int strength[1];
                        strength[0] = MainForm->edits->GetPasswordStrength(edit.text);
                        if (edit.redraw_ticks > 0)
                        {
                            String length_text = IntToStr(edit.text.Length());
                            length_text += "/";
                            length_text += IntToStr(edit.max_length);
                            text_x -= (short)MainForm->fonts->TextWidth(length_text);
                            MainForm->fonts->DrawString(edit.x + (short)text_x, (short)edit.y + 3, length_text);
                        }
                        String strength_text = "weak";
                        if (strength[0] > 4)
                            strength_text = "okay";
                        if (strength[0] > 6)
                            strength_text = "good";
                        if (strength[0] > 9)
                            strength_text = "strong";
                        text_x = edit.width - 27;
                        text_x -= (short)MainForm->fonts->TextWidth(strength_text) / 2;
                        MainForm->fonts->DrawString(edit.x + (short)text_x, (short)edit.y + 2, strength_text);
                    }
                }
                if (edit.focused && edit.editable)
                {
                    text_width = (short)MainForm->fonts->TextWidth(edit.line_buffer);
                    if (MainForm->edits->caret_blink_on && MainForm->edits->caret_enabled)
                    {
                        caret_visible = true;
                        caret_x = text_width + edit.x + 7;
                        caret_y = edit.y + 5;
                        if (edit.multi_line && !edit.scroll_offset)
                        {
                            int line_height_t = (short)MainForm->fonts->TextHeight("T");
                            if (ends_in_return)
                            {
                            }
                            caret_y += (line - first_line) * (line_height_t + 3);
                        }
                    }
                }
            }
        }
        MainForm->edits->MoveNext();
    } while (!MainForm->edits->IsAtEnd());
}
