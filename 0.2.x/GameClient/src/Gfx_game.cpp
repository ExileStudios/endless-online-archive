#include <vcl.h>
#pragma hdrstop

#include "Gfx_game.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Gfx_bank.h"
#include "Gfx_icon.h"
#include "Gfx_interface.h"
#include "Gfx_msgboard.h"
#include "Gfx_musician.h"
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Gfx_barber.h"
#include "Ctrl_book.h"
#include "Gfx_chest.h"
#include "Ctrl_dialog.h"
#include "Gfx_dialog.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Gfx_item.h"
#include "Ctrl_jukebox.h"
#include "Gfx_jukebox.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_musician.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_quests.h"
#include "Ctrl_report.h"
#include "Gfx_shop.h"
#include "Ctrl_social.h"
#include "Gfx_social.h"
#include "Gfx_sprites.h"
#include "Ctrl_stats.h"
#include "Gfx_stats.h"
#include "Gfx_trade.h"
#include "Ctrl_status.h"
#include "Gfx_subitem.h"
#include <vector.h>
#include "Ctrl_chest.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_msgboard.h"
#include "Ctrl_shop.h"
#include "Ctrl_skills.h"
#include "Ctrl_spells.h"
#include "Ctrl_tooltip.h"
#include "Ctrl_trade.h"
#include "Gfx_mapicons.h"
#include "Gfx_mapitems.h"

#pragma package(smart_init)

// Creates the box, button, popup menu, paperdoll, captcha and document surfaces at their fixed sizes and sets the
// view rects and the source rects of the amount box, its title strips and the row highlights.
GameGraphics::GameGraphics()
{
    button_sheet = new TDx_DrawSurface(NULL);
    extra_surface = new TDx_DrawSurface(NULL);
    message_box_surface = new TDx_DrawSurface(NULL);
    text_box_surface = new TDx_DrawSurface(NULL);
    input_box_surface = new TDx_DrawSurface(NULL);
    list_box_surface = new TDx_DrawSurface(NULL);
    paperdoll_surface = new TDx_DrawSurface(NULL);
    book_surface = new TDx_DrawSurface(NULL);
    black_fill_fx = new TDDBltFX(NULL);
    view_fill_fx = new TDDBltFX(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    black_fill_fx->FillColor = 0;
    view_fill_fx->FillColor = MainForm->blitter->MakeColor(8, 8, 8);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 290;
    surface_desc->Height = 125;
    message_box_surface->GetSurfaceDesc(surface_desc);
    message_box_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 290;
    surface_desc->Height = 189;
    text_box_surface->GetSurfaceDesc(surface_desc);
    text_box_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 182;
    surface_desc->Height = 290;
    button_sheet->GetSurfaceDesc(surface_desc);
    button_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 284;
    surface_desc->Height = 137;
    extra_surface->GetSurfaceDesc(surface_desc);
    extra_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 380;
    surface_desc->Height = 580;
    paperdoll_surface->GetSurfaceDesc(surface_desc);
    paperdoll_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 380;
    surface_desc->Height = 290;
    book_surface->GetSurfaceDesc(surface_desc);
    book_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 265;
    surface_desc->Height = 148;
    input_box_surface->GetSurfaceDesc(surface_desc);
    input_box_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 385;
    surface_desc->Height = 420;
    list_box_surface->GetSurfaceDesc(surface_desc);
    list_box_surface->Create(surface_desc, MainForm->Dx_Draw1);
    view_clip_rect.top = 8;
    view_clip_rect.left = 9;
    view_clip_rect.right = 629;
    view_clip_rect.bottom = 308;
    view_rect.top = 0;
    view_rect.left = 0;
    view_rect.right = 620;
    view_rect.bottom = 300;
    message_box_rect.top = 0;
    message_box_rect.left = 0;
    message_box_rect.bottom = 125;
    message_box_rect.right = 290;
    text_box_rect.top = 0;
    text_box_rect.left = 0;
    text_box_rect.bottom = 189;
    text_box_rect.right = 290;
    player_menu_shadow_rect.top = 0;
    player_menu_shadow_rect.left = 192;
    player_menu_shadow_rect.right = 280;
    player_menu_shadow_rect.bottom = 129;
    player_menu_rect.top = 0;
    player_menu_rect.left = 0;
    player_menu_rect.right = 96;
    player_menu_rect.bottom = 137;
    amount_box_rect.top = 0;
    amount_box_rect.left = 38;
    amount_box_rect.right = 303;
    amount_box_rect.bottom = 170;
    junk_title_rect.top = 172;
    junk_title_rect.left = 39;
    junk_title_rect.right = 281;
    junk_title_rect.bottom = 193;
    amount_title_rect.top = 196;
    amount_title_rect.left = 39;
    amount_title_rect.right = 281;
    amount_title_rect.bottom = 217;
    trade_title_rect.top = 220;
    trade_title_rect.left = 39;
    trade_title_rect.right = 281;
    trade_title_rect.bottom = 241;
    shop_title_rect.top = 244;
    shop_title_rect.left = 39;
    shop_title_rect.right = 281;
    shop_title_rect.bottom = 265;
    bank_title_rect.top = 268;
    bank_title_rect.left = 39;
    bank_title_rect.right = 281;
    bank_title_rect.bottom = 289;
    cell_highlight_rect.top = 405;
    cell_highlight_rect.left = 0;
    cell_highlight_rect.right = 58;
    cell_highlight_rect.bottom = 441;
    tall_row_highlight_rect.top = 405;
    tall_row_highlight_rect.left = 0;
    tall_row_highlight_rect.right = 232;
    tall_row_highlight_rect.bottom = 441;
    narrow_row_highlight_rect.top = 405;
    narrow_row_highlight_rect.left = 0;
    narrow_row_highlight_rect.right = 215;
    narrow_row_highlight_rect.bottom = 421;
    item_row_highlight_rect.top = 405;
    item_row_highlight_rect.left = 0;
    item_row_highlight_rect.right = 232;
    item_row_highlight_rect.bottom = 441;
    short_highlight_rect.top = 405;
    short_highlight_rect.left = 0;
    short_highlight_rect.right = 173;
    short_highlight_rect.bottom = 441;
    row_highlight_rect.top = 405;
    row_highlight_rect.left = 0;
    row_highlight_rect.right = 232;
    row_highlight_rect.bottom = 421;
    input_box_rect.top = 0;
    input_box_rect.left = 0;
    input_box_rect.right = 265;
    input_box_rect.bottom = 148;
    list_scrollbar_rect.top = 0;
    list_scrollbar_rect.left = 330;
    list_scrollbar_rect.right = 385;
    list_scrollbar_rect.bottom = 240;
    book_rect.top = 0;
    book_rect.left = 0;
    book_rect.right = 380;
    book_rect.bottom = 290;
}

// The input form body for a row count: with fewer than five rows the body is cut shorter by one row height each.
TRect GameGraphics::GetListBoxBodyRect(int item_count)
{
    TRect list_box_body_rect;
    if (item_count < 5)
        item_count = INPUT_FORM_ROW_HEIGHT * (5 - item_count);
    else
        item_count = 0;
    list_box_body_rect.top = 0;
    list_box_body_rect.left = 0;
    list_box_body_rect.right = 330;
    list_box_body_rect.bottom = 240 - item_count;
    return list_box_body_rect;
}

// One of the input form's three footer pieces, for forms of four, three or two rows.
TRect GameGraphics::GetListBoxFooterRect(int footer)
{
    TRect list_box_footer_rect;
    list_box_footer_rect.left = 0;
    list_box_footer_rect.right = 330;
    list_box_footer_rect.top = 60 * footer + 240;
    list_box_footer_rect.bottom = 60 * footer + 300;
    return list_box_footer_rect;
}

// One window menu icon of the control sheet, nine to a row.
TRect GameGraphics::GetIconRect(int icon)
{
    TRect icon_rect;
    int row = 0;
    if (icon > 8)
    {
        row++;
        icon = icon - 9;
    }
    if (icon > 8)
    {
        row++;
        icon = icon - 9;
    }
    icon_rect.left = 31 * icon;
    icon_rect.right = 31 * icon + 32;
    icon_rect.top = 31 * row + 291;
    icon_rect.bottom = 31 * row + 322;
    return icon_rect;
}

GameGraphics::~GameGraphics()
{
    button_sheet->Destroy();
    extra_surface->Destroy();
    message_box_surface->Destroy();
    text_box_surface->Destroy();
    paperdoll_surface->Destroy();
    book_surface->Destroy();
    input_box_surface->Destroy();
    list_box_surface->Destroy();
    delete button_sheet;
    delete extra_surface;
    delete message_box_surface;
    delete text_box_surface;
    delete paperdoll_surface;
    delete book_surface;
    delete input_box_surface;
    delete list_box_surface;
    black_fill_fx->Clear();
    view_fill_fx->Clear();
    color_key->Clear();
    delete black_fill_fx;
    delete view_fill_fx;
    delete color_key;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
}

// The offset that centres an item of a given size in a paperdoll equipment box: sizes from four on are two
// columns wide.
TPoint GameGraphics::GetPaperdollSlotOffset(int item_size)
{
    TPoint result;
    int slot_width = 23;
    int slot_height = 23;
    if (item_size < 4)
        slot_height += 26 * item_size;
    else
    {
        slot_width = 49;
        slot_height = slot_height + 26 * item_size - 104;
    }
    result.x = (55 - slot_width) / 2;
    result.y = (101 - slot_height) / 2;
    return result;
}

// Blends a source rect into the map view at a view position with an alpha level, clipped to the view.
void GameGraphics::BlitClippedAlpha(int x, int y, TDx_DrawSurface *source, TRect source_rect, int alpha)
{
    int clipped_x;
    char visible = true;
    int width = source_rect.right - source_rect.left;
    int height = source_rect.bottom - source_rect.top;
    if (x + width < view_rect.left)
        visible = false;
    if (y + height < view_rect.top)
        visible = false;
    if (x > view_clip_rect.right)
        visible = false;
    if (y > view_clip_rect.bottom)
        visible = false;
    if (visible)
    {
        clipped_x = x;
        int clipped_y = y;
        if (clipped_x <= view_rect.left)
            clipped_x = view_rect.left;
        if (view_rect.top >= clipped_y)
            clipped_y = view_rect.top;
        source_rect = ClipSourceRect(x, y, source_rect);
        if (source_rect.bottom - source_rect.top > 0)
            MainForm->blitter->AlphaBlt(view_clip_rect.left + clipped_x, clipped_y + view_clip_rect.top,
                MainForm->OffscreenSurface, source, &source_rect, alpha);
    }
}

// Half-blends a source rect into the map view at a view position, clipped to the view; used for shadows.
void GameGraphics::BlitClippedHalfBlend(int x, int y, TDx_DrawSurface *source, TRect source_rect)
{
    int clipped_x;
    char result = 1;
    int width = source_rect.right - source_rect.left;
    int height = source_rect.bottom - source_rect.top;
    if (x + width < view_rect.left)
        result = 0;
    if (y + height < view_rect.top)
        result = 0;
    if (x > view_clip_rect.right)
        result = 0;
    if (y > view_clip_rect.bottom)
        result = 0;
    if (result)
    {
        clipped_x = x;
        int clipped_y = y;
        if (clipped_x <= view_rect.left)
            clipped_x = view_rect.left;
        if (view_rect.top >= clipped_y)
            clipped_y = view_rect.top;
        source_rect = ClipSourceRect(x, y, source_rect);
        if (source_rect.bottom - source_rect.top > 0)
            MainForm->blitter->HalfBlendBlt(view_clip_rect.left + clipped_x, clipped_y + view_clip_rect.top,
                MainForm->OffscreenSurface, source, &source_rect);
    }
}

// Blits a source rect, black see-through, into the map view at a view position, clipped to the view; returns whether
// any of it was in view.
bool GameGraphics::BlitClipped(int x, int y, TDx_DrawSurface *source, TRect source_rect)
{
    bool visible = true;
    int width = source_rect.right - source_rect.left;
    int height = source_rect.bottom - source_rect.top;
    if (x + width < view_rect.left)
        visible = false;
    if (y + height < view_rect.top)
        visible = false;
    if (x > view_clip_rect.right)
        visible = false;
    if (y > view_clip_rect.bottom)
        visible = false;
    if (visible)
    {
        int clipped_x = x;
        int clipped_y = y;
        if (view_rect.left >= clipped_x)
            clipped_x = view_rect.left;
        if (view_rect.top >= clipped_y)
            clipped_y = view_rect.top;
        MainForm->OffscreenSurface->BltFast(clipped_x + view_clip_rect.left, clipped_y + view_clip_rect.top, source,
            &ClipSourceRect(x, y, source_rect), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        // A lost surface on either side makes DirectDraw restore every surface.
        if (MainForm->OffscreenSurface->ErrorValue == DDERR_SURFACELOST || source->ErrorValue == DDERR_SURFACELOST)
            MainForm->Dx_Draw1->RestoreAllSurfaces();
    }
    return visible;
}

// Draws the message box title and its two message lines, first wrapping a long first line into both lines.
void GameGraphics::DrawMessageBoxText()
{
    int i;
    MainForm->draw_canvas->Brush->Color = clWhite;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Font->Color = (TColor)0xC8F0F0;
    MainForm->draw_canvas->Font->Name = "MS Sans Serif";
    MainForm->draw_canvas->Font->Size = 10;
    // The wrapped lines replace the stored ones, so a message is wrapped once; text past two lines is lost.
    if (MainForm->draw_canvas->TextWidth(MainForm->animation->game->message_lines[0]) > 250)
    {
        String message = MainForm->animation->game->message_lines[0];
        int line_start = 1;
        int line_end = 1;
        int line_index = 0;
        do
        {
            if (MainForm->draw_canvas->TextWidth(message.SubString(line_start, line_end)) >= 250 ||
                line_end == message.Length())
            {
                char seeking_break = true;
                for (i = line_end; i >= 1 && seeking_break; i--)
                {
                    if (message[i] == ' ' || i == message.Length())
                    {
                        if (line_index <= MESSAGE_BOX_LINES && message.SubString(line_start, i).Length() >= 1)
                        {
                            if (message.SubString(line_start, i)[1] == ' ')
                                MainForm->animation->game->message_lines[line_index] =
                                    message.SubString(line_start + 1, i);
                            else
                                MainForm->animation->game->message_lines[line_index] = message.SubString(line_start, i);
                        }
                        seeking_break = false;
                        line_start = i;
                        line_end = i;
                        line_end++;
                        line_index++;
                    }
                }
            }
            else
                line_end++;
        } while (message.Length() >= line_end && line_index <= 1);
    }
    MainForm->draw_canvas->TextOut(MainForm->animation->game->message_x + 20, MainForm->animation->game->message_y + 12,
        MainForm->animation->game->message_title);
    MainForm->draw_canvas->Font->Color = (TColor)0xD6E6E6;
    MainForm->draw_canvas->TextOut(MainForm->animation->game->message_x + 20, MainForm->animation->game->message_y + 40,
        MainForm->animation->game->message_lines[0]);
    MainForm->draw_canvas->TextOut(MainForm->animation->game->message_x + 20, MainForm->animation->game->message_y + 56,
        MainForm->animation->game->message_lines[1]);
}

// Draws the text box title and its six lines, first wrapping newly set text into the lines at spaces.
void GameGraphics::DrawTextBoxText()
{
    int width_limit;
    int line_index;
    char line_full;
    MainForm->draw_canvas->Brush->Color = clWhite;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Font->Color = (TColor)0xC8F0F0;
    MainForm->draw_canvas->Font->Name = "MS Sans Serif";
    MainForm->draw_canvas->Font->Size = 10;
    if (MainForm->animation->game->large_box_text.Length() > 0)
    {
        width_limit = 254;
        line_index = 0;
        String remaining = MainForm->animation->game->large_box_text;
        do
        {
            String fitting_text;
            if (MainForm->draw_canvas->TextWidth(remaining) <= width_limit)
            {
                MainForm->animation->game->large_box_lines[line_index] = remaining;
                remaining = "";
            }
            else
            {
                line_full = false;
                for (int i = 1; i <= remaining.Length(); i++)
                {
                    if (remaining[i] == ' ' || i == remaining.Length())
                    {
                        String prefix = remaining.SubString(1, i);
                        if (MainForm->draw_canvas->TextWidth(prefix) >= width_limit)
                        {
                            MainForm->animation->game->large_box_lines[line_index] = fitting_text;
                            remaining.Delete(1, fitting_text.Length());
                            line_index++;
                            line_full = true;
                            break;
                        }
                        fitting_text = prefix;
                    }
                }
                if (!line_full)
                {
                    MainForm->animation->game->large_box_lines[line_index] = remaining;
                    remaining = "";
                }
            }
        } while (remaining.Length() > 0);
        // Clearing the text marks it wrapped; lines set by the caller and not reached by the wrap stay.
        MainForm->animation->game->large_box_text = "";
    }
    MainForm->draw_canvas->TextOut(MainForm->animation->game->large_box_x + 20,
        MainForm->animation->game->large_box_y + 12, MainForm->animation->game->large_box_title);
    MainForm->draw_canvas->Font->Color = (TColor)0xD6E6E6;
    for (int j = 0; j < 6; j++)
        MainForm->draw_canvas->TextOut(MainForm->animation->game->large_box_x + 20,
            MainForm->animation->game->large_box_y + 16 * j + 40, MainForm->animation->game->large_box_lines[j]);
}

// Draws the input form: its body cut to the row count, a matching footer under fewer than five rows, and with more
// than five rows the scroll bar, arrows and thumb.
void GameGraphics::DrawListBox()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x, MainForm->animation->game->form_y,
        list_box_surface, &GetListBoxBodyRect(MainForm->animation->game->form_row_count), DDBLTFAST_WAIT);
    if (MainForm->animation->game->form_row_count > 5)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 275, MainForm->animation->game->form_y,
            list_box_surface, &list_scrollbar_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->game->form_row_count < 5)
    {
        int footer = 2;
        int missing_rows = 0;
        if (MainForm->animation->game->form_row_count == 3)
        {
            missing_rows = 1;
            footer = 1;
        }
        if (MainForm->animation->game->form_row_count == 2)
        {
            missing_rows = 2;
            footer = 0;
        }
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x,
            MainForm->animation->game->form_y + 157 - 23 * missing_rows, list_box_surface,
            &GetListBoxFooterRect(footer), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->game->form_row_count > 5)
    {
        if (MainForm->animation->game->form_scroll_up_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 284,
                MainForm->animation->game->form_y + 71, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(16), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 284,
                MainForm->animation->game->form_y + 71, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(15), DDBLTFAST_WAIT);
        if (MainForm->animation->game->form_scroll_down_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 284,
                MainForm->animation->game->form_y + 163, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(14), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 284,
                MainForm->animation->game->form_y + 163, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(13), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 284,
            MainForm->animation->game->form_y + MainForm->animation->game->GetInputFormThumbOffset() + 87,
            MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(12),
            DDBLTFAST_WAIT);
    }
}

// Draws the input box prompt in two lines, first wrapping a long first line into both lines.
void GameGraphics::DrawInputBoxText()
{
    int i;
    int line_start;
    MainForm->draw_canvas->Brush->Color = clWhite;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Font->Color = (TColor)0xC8F0F0;
    MainForm->draw_canvas->Font->Name = "MS Sans Serif";
    MainForm->draw_canvas->Font->Size = 10;
    if (MainForm->draw_canvas->TextWidth(MainForm->animation->game->input_box_lines[0]) > 210)
    {
        String prompt = MainForm->animation->game->input_box_lines[0];
        int line_end = 1;
        line_start = 1;
        int line_index = 0;
        do
        {
            if (MainForm->draw_canvas->TextWidth(prompt.SubString(line_start, line_end)) >= 210 ||
                prompt.Length() == line_end)
            {
                char seeking_break = true;
                for (i = line_end; i >= 1 && seeking_break; i--)
                {
                    if (prompt[i] == ' ' || i == prompt.Length())
                    {
                        if (line_index <= INPUT_BOX_LINES && prompt.SubString(line_start, i).Length() >= 1)
                        {
                            if (prompt.SubString(line_start, i)[1] == ' ')
                                MainForm->animation->game->input_box_lines[line_index] =
                                    prompt.SubString(line_start + 1, i);
                            else
                                MainForm->animation->game->input_box_lines[line_index] =
                                    prompt.SubString(line_start, i);
                        }
                        seeking_break = false;
                        line_start = i;
                        line_end = i;
                        line_end++;
                        line_index++;
                    }
                }
            }
            else
                line_end++;
        } while (prompt.Length() >= line_end && line_index <= 1);
    }
    MainForm->draw_canvas->Font->Color = (TColor)0xD6E6E6;
    MainForm->draw_canvas->TextOut(MainForm->animation->game->input_box_x + 20,
        MainForm->animation->game->input_box_y + 19, MainForm->animation->game->input_box_lines[0]);
    MainForm->draw_canvas->TextOut(MainForm->animation->game->input_box_x + 20,
        MainForm->animation->game->input_box_y + 35, MainForm->animation->game->input_box_lines[1]);
    // While a message box is open the input edit's text is drawn here in the small font.
    if (MainForm->animation->game->message_visible)
    {
        MainForm->draw_canvas->Font->Color = (TColor)0xB4C8DC;
        MainForm->draw_canvas->Font->Name = "MS Sans Serif";
        MainForm->draw_canvas->Font->Size = 8;
        MainForm->draw_canvas->TextOut(MainForm->animation->game->input_box_x + 46,
            MainForm->animation->game->input_box_y + 77, MainForm->edits->GetText(EditId_InputBox));
    }
}

// Draws the input form's OK and Cancel buttons, raised by the rows a short form lacks, each lit under the mouse.
void GameGraphics::DrawListBoxButtons()
{
    int shrink = MainForm->animation->game->form_row_count;
    if (shrink < 5)
        shrink = INPUT_FORM_ROW_HEIGHT * (5 - shrink);
    else
        shrink = 0;
    if (MainForm->animation->game->form_ok_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 72,
            MainForm->animation->game->form_y + 193 - shrink, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 72,
            MainForm->animation->game->form_y + 193 - shrink, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->game->form_cancel_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 165,
            MainForm->animation->game->form_y + 193 - shrink, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->form_x + 165,
            MainForm->animation->game->form_y + 193 - shrink, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the input box OK and Cancel buttons, each lit under the mouse.
void GameGraphics::DrawInputBoxButtons()
{
    if (MainForm->animation->game->input_box_ok_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->input_box_x + 41,
            MainForm->animation->game->input_box_y + 103, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->input_box_x + 41,
            MainForm->animation->game->input_box_y + 103, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->game->input_box_cancel_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->input_box_x + 134,
            MainForm->animation->game->input_box_y + 103, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->input_box_x + 134,
            MainForm->animation->game->input_box_y + 103, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the message box buttons: the right one is OK, or Cancel for kinds 2 and 3, and kind 3 adds OK on the left.
void GameGraphics::DrawMessageBoxButtons()
{
    int right_button = 4;
    if (MainForm->animation->game->message_buttons == DialogButtons_OkCancel ||
        MainForm->animation->game->message_buttons == DialogButtons_Cancel)
        right_button = 1;
    if (MainForm->animation->game->message_ok_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->message_x + 183,
            MainForm->animation->game->message_y + 83, button_sheet, &GetButtonRect(right_button, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->message_x + 183,
            MainForm->animation->game->message_y + 83, button_sheet, &GetButtonRect(right_button, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->game->message_buttons == DialogButtons_OkCancel)
    {
        if (MainForm->animation->game->message_cancel_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->message_x + 90,
                MainForm->animation->game->message_y + 83, button_sheet, &GetButtonRect(4, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->message_x + 90,
                MainForm->animation->game->message_y + 83, button_sheet, &GetButtonRect(4, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the text box buttons: the right one is OK, or Cancel for kinds 2 and 3, and kind 3 adds OK on the left.
void GameGraphics::DrawTextBoxButtons()
{
    int right_button = 4;
    if (MainForm->animation->game->large_box_buttons == DialogButtons_OkCancel ||
        MainForm->animation->game->large_box_buttons == DialogButtons_Cancel)
        right_button = 1;
    if (MainForm->animation->game->large_box_ok_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->large_box_x + 183,
            MainForm->animation->game->large_box_y + 147, button_sheet, &GetButtonRect(right_button, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->large_box_x + 183,
            MainForm->animation->game->large_box_y + 147, button_sheet, &GetButtonRect(right_button, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->game->large_box_buttons == DialogButtons_OkCancel)
    {
        if (MainForm->animation->game->large_box_cancel_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->large_box_x + 90,
                MainForm->animation->game->large_box_y + 147, button_sheet, &GetButtonRect(4, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->game->large_box_x + 90,
                MainForm->animation->game->large_box_y + 147, button_sheet, &GetButtonRect(4, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the amount box text: a title line with the item name and the prompt for the box kind.
void GameGraphics::DrawItemAmountText()
{
    MainForm->draw_canvas->Brush->Color = clWhite;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Font->Color = (TColor)0xD6E6E6;
    MainForm->draw_canvas->Font->Name = "MS Sans Serif";
    MainForm->draw_canvas->Font->Size = 10;
    int prompt_text_id = 57;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_JunkItem ||
        MainForm->animation->game->amount_box_kind == AmountBoxKind_Amount)
        prompt_text_id = prompt_text_id + MainForm->animation->game->amount_box_kind - 1;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_TradeAddItem)
        prompt_text_id = 107;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_BankDeposit ||
        MainForm->animation->game->amount_box_kind == AmountBoxKind_GuildDeposit)
        prompt_text_id = 134;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_BankWithdraw)
        prompt_text_id = 133;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ShopBuy)
        prompt_text_id = 131;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ShopSell)
        prompt_text_id = 132;
    if (MainForm->animation->game->amount_box_kind == AmountBoxKind_LockerAddItem)
        prompt_text_id = 177;
    String title = MainForm->strings->GetText(GameText_HowMuch);
    title.Insert(" ", title.Length() + 1);
    title.Insert(MainForm->item_values->GetName(MainForm->animation->game->amount_item_id), title.Length() + 1);
    MainForm->draw_canvas->TextOut(MainForm->animation->game->amount_box_x + 20,
        MainForm->animation->game->amount_box_y + 40, title);
    MainForm->draw_canvas->TextOut(MainForm->animation->game->amount_box_x + 20,
        MainForm->animation->game->amount_box_y + 56, MainForm->strings->GetText(prompt_text_id));
    // While a message box or the social window is open, the amount edit's text is drawn here.
    if (MainForm->animation->game->message_visible || MainForm->animation->social->is_open)
    {
        MainForm->draw_canvas->Font->Color = (TColor)0xB4C8DC;
        MainForm->draw_canvas->Font->Name = "MS Sans Serif";
        MainForm->draw_canvas->Font->Size = 8;
        MainForm->draw_canvas->TextOut(MainForm->animation->game->amount_box_x + 163,
            MainForm->animation->game->amount_box_y + 97, MainForm->edits->GetText(EditId_Amount));
    }
}

// Draws the chest window: background, hovered row highlight, and a slot with the item graphic for each of up to five
// items.
void GameGraphics::DrawChestWindow()
{
    int graphic;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->chest->window_x, MainForm->animation->chest->window_y,
        MainForm->gfx->chest->background_surface, &MainForm->gfx->chest->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->chest->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->chest->window_x + 18,
            37 * MainForm->animation->chest->hovered_row + MainForm->animation->chest->window_y + 23,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &item_row_highlight_rect, 12);
    int item_count = MainForm->animation->chest->items.size();
    int row = 0;
    for (int i = 0; i < item_count && i < 5; i++)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->chest->window_x + 18,
            MainForm->animation->chest->window_y + 37 * row + 27, MainForm->gfx->chest->item_slot_surface,
            &MainForm->gfx->chest->item_slot_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        int item_id = MainForm->animation->chest->items[i].item_id;
        int amount = MainForm->animation->chest->items[i].amount;
        graphic = MainForm->item_values->GetGraphic(item_id) - 1;
        if (graphic >= 0)
        {
            // Gold uses a pile graphic that grows with the amount.
            if (item_id == ItemId_Gold)
            {
                graphic = 134;
                if (amount > 1)
                    graphic = 135;
                if (amount > 99)
                    graphic = 136;
                if (amount > 9999)
                    graphic = 137;
                if (amount > 99999)
                    graphic = 138;
            }
            if (MainForm->gfx->items->RequestGraphic(graphic))
            {
                GraphicSize size = MainForm->gfx->items->GetGraphicSize(graphic);
                int item_x = MainForm->animation->chest->window_x + 40 - size.width / 2;
                int item_y = MainForm->animation->chest->window_y + 37 * row + 35 - size.height / 2;
                MainForm->gfx->game->BlitClipped(item_x, item_y,
                    MainForm->gfx->items->GetGraphic(graphic).resource->surface,
                    MainForm->gfx->items->GetGraphic(graphic).resource->surface_rect);
            }
            else
                // While an item graphic is still loading, another redraw is requested.
                MainForm->redraw_needed = true;
        }
        row++;
    }
}

// Draws the barber window: background, hovered row highlight, the three row icons, and a preview of the player with
// the chosen hair style and colour.
void GameGraphics::DrawBarberWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->barber->window_x, MainForm->animation->barber->window_y,
        MainForm->gfx->barber->surface, &MainForm->gfx->barber->surface_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->barber->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->barber->window_x + 19,
            37 * MainForm->animation->barber->hovered_row + MainForm->animation->barber->window_y + 23,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &short_highlight_rect, 12);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->barber->window_x + 24,
        MainForm->animation->barber->window_y + 23, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(6),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->barber->window_x + 24,
        MainForm->animation->barber->window_y + 60, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(7),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->barber->window_x + 24,
        MainForm->animation->barber->window_y + 97, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(8),
        DDBLTFAST_WAIT);
    MainForm->gfx->sprites->DrawPlayerAt(MainForm->players->GetPlayerById(PlayerId_Preview),
        MainForm->animation->barber->hair_style, MainForm->animation->barber->hair_color,
        MainForm->animation->barber->window_x + 250, MainForm->animation->barber->window_y + 50);
}

// Draws the musician window background and lights the note cell under the mouse.
void GameGraphics::DrawMusicianWindow()
{
    int grid_x;
    int grid_y;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->musician->window_x,
        MainForm->animation->musician->window_y, MainForm->gfx->musician->background_surface,
        &MainForm->gfx->musician->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->musician->hovered_note_col >= 0 && MainForm->animation->musician->hovered_note_row >= 0)
    {
        int cell_column = MainForm->animation->musician->hovered_note_col;
        int cell_row = MainForm->animation->musician->hovered_note_row;
        int grid_x = MainForm->animation->musician->window_x + 15;
        int grid_y = MainForm->animation->musician->window_y + 15;
        MainForm->OffscreenSurface->BltFast(20 * cell_column + grid_x, 20 * cell_row + grid_y,
            MainForm->gfx->musician->cell_sheet_surface, &MainForm->gfx->musician->GetCellRect(cell_column, cell_row),
            DDBLTFAST_WAIT);
    }
}

// Draws the jukebox window: background, hovered row highlight and its two row icons.
void GameGraphics::DrawJukeboxWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->jukebox->window_x, MainForm->animation->jukebox->window_y,
        MainForm->gfx->jukebox->surface, &MainForm->gfx->jukebox->surface_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->jukebox->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->jukebox->window_x + 19,
            37 * MainForm->animation->jukebox->hovered_row + MainForm->animation->jukebox->window_y + 58,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &tall_row_highlight_rect, 12);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->jukebox->window_x + 26,
        MainForm->animation->jukebox->window_y + 61, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(1),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->jukebox->window_x + 26,
        MainForm->animation->jukebox->window_y + 98, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(8),
        DDBLTFAST_WAIT);
}

// Draws the experience window background and the bullet icons of its eight lines.
void GameGraphics::DrawStatWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->stat->window_x, MainForm->animation->stat->window_y,
        MainForm->gfx->stat->surface, &MainForm->gfx->stat->surface_rect, DDBLTFAST_WAIT);
    for (int row = 0; row < 8; row++)
    {
        int icon_x = MainForm->animation->stat->window_x + 22;
        int icon_y = MainForm->animation->stat->window_y + LIST_ROW_HEIGHT * row + 48;
        MainForm->OffscreenSurface->BltFast(icon_x, icon_y, MainForm->gfx->interface_graphics->list_icons,
            &MainForm->gfx->interface_graphics->GetListIconRect(0, 0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (row < 3)
            MainForm->OffscreenSurface->BltFast(icon_x + 120, icon_y, MainForm->gfx->interface_graphics->list_icons,
                &MainForm->gfx->interface_graphics->GetListIconRect(1, 0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the bank window: background, hovered row highlight and its three row icons.
void GameGraphics::DrawBankWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->bank->window_x, MainForm->animation->bank->window_y,
        MainForm->gfx->bank->surface, &MainForm->gfx->bank->surface_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->bank->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->bank->window_x + 19,
            37 * MainForm->animation->bank->hovered_row + MainForm->animation->bank->window_y + 55,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &tall_row_highlight_rect, 12);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->bank->window_x + 26,
        MainForm->animation->bank->window_y + 57, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(2),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->bank->window_x + 26,
        MainForm->animation->bank->window_y + 94, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(3),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->bank->window_x + 26,
        MainForm->animation->bank->window_y + 131, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(5),
        DDBLTFAST_WAIT);
}

// Draws the locker window: background, the hovered item highlight, each visible item's graphic in the four-wide grid,
// and the scroll arrows and thumb.
void GameGraphics::DrawLockerWindow()
{
    bool loop_entered = false;
    int graphic;
    int item_id;
    int index;
    int count;
    int drawn;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x, MainForm->animation->locker->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->locker->hovered_slot >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->locker->window_x +
            57 * MainForm->animation->locker->hovered_col + 16,
            MainForm->animation->locker->window_y + 37 * MainForm->animation->locker->hovered_row + 49,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &cell_highlight_rect, 12);
    index = 4 * MainForm->animation->locker->GetTopRow();
    count = MainForm->animation->locker->items.size();
    drawn = 0;
    // Five rows show, from the top row.
    for (int row = 0; row < 5 && index < count; row++)
    {
        loop_entered = true;
        int col = 0;
        while (col < 4 && index < count)
        {
            loop_entered = true;
            int slot_x = MainForm->animation->locker->window_x + 57 * col + 16;
            int slot_y = MainForm->animation->locker->window_y + 37 * row;
            MainForm->OffscreenSurface->BltFast(slot_x, slot_y + 50, MainForm->gfx->shop->item_slot_surface,
                &MainForm->gfx->shop->item_slot_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            item_id = MainForm->animation->locker->items[index].item_id;
            graphic = MainForm->item_values->GetGraphic(item_id) - 1;
            index++;
            if (graphic >= 0)
            {
                if (MainForm->gfx->items->RequestGraphic(graphic))
                {
                    GraphicSize graphic_size = MainForm->gfx->items->GetGraphicSize(graphic);
                    item_id = slot_x + 21;
                    item_id -= graphic_size.width / 2;
                    int item_x = slot_y + 58 - graphic_size.height / 2;
                    MainForm->gfx->game->BlitClipped(item_id, item_x,
                        MainForm->gfx->items->GetGraphic(graphic).resource->surface,
                        MainForm->gfx->items->GetGraphic(graphic).resource->surface_rect);
                }
                else
                    MainForm->redraw_needed = true;
            }
            col++;
        }
    }
    if (MainForm->animation->locker->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 252,
            MainForm->animation->locker->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 252,
            MainForm->animation->locker->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->locker->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 252,
            MainForm->animation->locker->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 252,
            MainForm->animation->locker->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 252,
        MainForm->animation->locker->window_y + MainForm->animation->locker->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the social window: background, hovered line highlight, and the scroll arrows and thumb.
void GameGraphics::DrawSocialWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x, MainForm->animation->social->window_y,
        MainForm->gfx->social->surface, &MainForm->gfx->social->surface_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->social->hover_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->social->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->social->hover_row + MainForm->animation->social->window_y +
            47, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->social->scroll_up_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 252,
            MainForm->animation->social->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 252,
            MainForm->animation->social->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->social->scroll_down_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 252,
            MainForm->animation->social->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 252,
            MainForm->animation->social->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 252,
        MainForm->animation->social->window_y + MainForm->animation->social->GetScrollbarPosition() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the guild window: background, the highlight of the hovered menu row or list line, the scroll arrows, the menu
// icons of the first four pages, and the scroll thumb.
void GameGraphics::DrawGuildWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x, MainForm->animation->guild->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    // The first four pages are menus of tall rows; later pages are text lists.
    if (MainForm->animation->guild->page <= GuildPage_Lookup)
    {
        if (MainForm->animation->guild->hovered_row >= 0 && MainForm->animation->guild->hovered_row <= 4)
            MainForm->blitter->AlphaBlt(MainForm->animation->guild->window_x + 16,
                MENU_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->guild->hovered_row +
                    MainForm->animation->guild->window_y + 49, MainForm->OffscreenSurface,
                MainForm->gfx->interface_graphics->control_sheet, &tall_row_highlight_rect, 12);
    }
    else if (MainForm->animation->guild->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->guild->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->guild->hovered_row + MainForm->animation->guild->window_y +
                47, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect,
            12);
    if (MainForm->animation->guild->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 252,
            MainForm->animation->guild->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 252,
            MainForm->animation->guild->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->guild->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 252,
            MainForm->animation->guild->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 252,
            MainForm->animation->guild->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    if (MainForm->animation->guild->page == GuildPage_Main)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 50, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(9), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 87, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(10), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 124, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(11), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 161, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(12), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->guild->page == GuildPage_Membership)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 50, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(13), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 87, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(14), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 124, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(15), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->guild->page == GuildPage_Manage)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 50, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(16), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 87, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(17), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 124, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(17), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 161, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(18), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 198, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(19), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->guild->page == GuildPage_Lookup)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 50, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(15), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 22,
            MainForm->animation->guild->window_y + 87, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(15), DDBLTFAST_WAIT);
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 252,
        MainForm->animation->guild->window_y + MainForm->animation->guild->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the help report window: background, hovered line highlight, and the scroll arrows and thumb.
void GameGraphics::DrawReportWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x, MainForm->animation->report->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->report->hovered_line >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->report->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->report->hovered_line +
                MainForm->animation->report->window_y + 47, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->report->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 252,
            MainForm->animation->report->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 252,
            MainForm->animation->report->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->report->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 252,
            MainForm->animation->report->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 252,
            MainForm->animation->report->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 252,
        MainForm->animation->report->window_y + MainForm->animation->report->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the quest dialog window: background, a close mark when the npc has a quest list, hovered line highlight,
// and the scroll arrows and thumb.
void GameGraphics::DrawDialogWindow()
{
    TRect close_rect;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x, MainForm->animation->dialog->window_y,
        MainForm->gfx->dialog->surface, &MainForm->gfx->dialog->surface_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->dialog->has_quest_list)
    {
        close_rect.left = 303;
        close_rect.top = 242;
        close_rect.right = 319;
        close_rect.bottom = 257;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 251,
            MainForm->animation->dialog->window_y + 16, MainForm->gfx->interface_graphics->control_sheet, &close_rect,
            DDBLTFAST_WAIT);
    }
    if (MainForm->animation->dialog->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->dialog->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->dialog->hovered_row +
                MainForm->animation->dialog->window_y + 47, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->dialog->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 252,
            MainForm->animation->dialog->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 252,
            MainForm->animation->dialog->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->dialog->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 252,
            MainForm->animation->dialog->window_y + 128, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 252,
            MainForm->animation->dialog->window_y + 128, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 252,
        MainForm->animation->dialog->window_y + MainForm->animation->dialog->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the memo window: background, hovered line highlight, and the scroll arrows and thumb.
void GameGraphics::DrawMemoWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x, MainForm->animation->memo->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->memo->hovered_line >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->memo->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->memo->hovered_line + MainForm->animation->memo->window_y +
            47, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->memo->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 252,
            MainForm->animation->memo->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 252,
            MainForm->animation->memo->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->memo->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 252,
            MainForm->animation->memo->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 252,
            MainForm->animation->memo->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 252,
        MainForm->animation->memo->window_y + MainForm->animation->memo->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the inn window: background, the highlight of the hovered menu row or list line, the scroll arrows, the menu
// icons of its first two pages, and the scroll thumb.
void GameGraphics::DrawInnWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x, MainForm->animation->inn->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->inn->page <= 3)
    {
        if (MainForm->animation->inn->hovered_row >= 0 && MainForm->animation->inn->hovered_row <= 4)
            MainForm->blitter->AlphaBlt(MainForm->animation->inn->window_x + 16,
                MENU_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->inn->hovered_row + MainForm->animation->inn->window_y +
                    49, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet,
                &tall_row_highlight_rect, 12);
    }
    else if (MainForm->animation->inn->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->inn->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->inn->hovered_row + MainForm->animation->inn->window_y + 47,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->inn->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 252,
            MainForm->animation->inn->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 252,
            MainForm->animation->inn->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->inn->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 252,
            MainForm->animation->inn->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 252,
            MainForm->animation->inn->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    if (!MainForm->animation->inn->page)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 22,
            MainForm->animation->inn->window_y + 50, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(9),
            DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 22,
            MainForm->animation->inn->window_y + 87, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(22),
            DDBLTFAST_WAIT);
    }
    if (MainForm->animation->inn->page == 1)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 22,
            MainForm->animation->inn->window_y + 50, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(23),
            DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 22,
            MainForm->animation->inn->window_y + 87, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(24),
            DDBLTFAST_WAIT);
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 252,
        MainForm->animation->inn->window_y + MainForm->animation->inn->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the law window: background, the highlight of the hovered menu row or list line, the scroll arrows, the menu
// icons of its first two pages, and the scroll thumb.
void GameGraphics::DrawLawWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x, MainForm->animation->law->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->law->page <= 3)
    {
        if (MainForm->animation->law->hovered_row >= 0 && MainForm->animation->law->hovered_row <= 4)
            MainForm->blitter->AlphaBlt(MainForm->animation->law->window_x + 16,
                MENU_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->law->hovered_row + MainForm->animation->law->window_y +
                    49, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet,
                &tall_row_highlight_rect, 12);
    }
    else if (MainForm->animation->law->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->law->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->law->hovered_row + MainForm->animation->law->window_y + 47,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->law->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 252,
            MainForm->animation->law->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 252,
            MainForm->animation->law->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->law->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 252,
            MainForm->animation->law->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 252,
            MainForm->animation->law->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    if (!MainForm->animation->law->page)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 22,
            MainForm->animation->law->window_y + 50, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(9),
            DDBLTFAST_WAIT);
    if (MainForm->animation->law->page == 1)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 22,
            MainForm->animation->law->window_y + 50, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(23),
            DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 22,
            MainForm->animation->law->window_y + 87, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(24),
            DDBLTFAST_WAIT);
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 252,
        MainForm->animation->law->window_y + MainForm->animation->law->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the quest book window: background, hovered line highlight across both halves, the icons of each visible
// quest line (or of each history line), and the scroll arrows and thumb.
void GameGraphics::DrawQuestWindow()
{
    int history_end_x;
    int top_line;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x, MainForm->animation->quests->window_y,
        MainForm->gfx->message_board->surface, &MainForm->gfx->message_board->GetFrameRect(0), DDBLTFAST_WAIT);
    if (MainForm->animation->quests->hovered_line >= 0)
    {
        MainForm->blitter->AlphaBlt(MainForm->animation->quests->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->quests->hovered_line +
                MainForm->animation->quests->window_y + 47, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->control_sheet, &narrow_row_highlight_rect, 12);
        MainForm->blitter->AlphaBlt(MainForm->animation->quests->window_x + 231,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->quests->hovered_line +
                MainForm->animation->quests->window_y + 47, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->control_sheet, &narrow_row_highlight_rect, 12);
    }
    if (!MainForm->animation->quests->showing_history)
    {
        top_line = MainForm->animation->quests->GetTopLine();
        int line_count = MainForm->animation->quests->lines->Count;
        int row = 0;
        for (int i = top_line; i < line_count && i < top_line + 12; i++)
        {
            String quest_line = MainForm->animation->quests->lines->Strings[i];
            if (quest_line.Length() > 0)
            {
                MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
                int icon_x = MainForm->animation->quests->window_x + 22;
                int icon_y = MainForm->animation->quests->window_y + 16 * row + 47;
                MainForm->OffscreenSurface->BltFast(icon_x, icon_y, MainForm->gfx->interface_graphics->list_icons,
                    &MainForm->gfx->interface_graphics->GetListIconRect(0, 0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                int column_x = icon_x + 145;
                // Each quest line's step column gets the icon of its step type.
                int step_icon = StrToInt(MainForm->animation->quests->line_icon_types->Strings[i]) + 1;
                MainForm->OffscreenSurface->BltFast(column_x, icon_y, MainForm->gfx->interface_graphics->list_icons,
                    &MainForm->gfx->interface_graphics->GetListIconRect(0, step_icon),
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                column_x = icon_x + 330;
                MainForm->OffscreenSurface->BltFast(column_x, icon_y, MainForm->gfx->interface_graphics->list_icons,
                    &MainForm->gfx->interface_graphics->GetListIconRect(1, 0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            }
            row++;
        }
    }
    if (MainForm->animation->quests->showing_history == 1)
    {
        int history_top_line = MainForm->animation->quests->GetTopLine();
        int history_line_count = MainForm->animation->quests->history_lines->Count;
        int history_row = 0;
        for (int j = history_top_line; j < history_line_count && j < history_top_line + 12; j++)
        {
            String history_line = MainForm->animation->quests->history_lines->Strings[j];
            if (history_line.Length() > 0)
            {
                MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
                int history_icon_x = MainForm->animation->quests->window_x + 22;
                int history_icon_y = MainForm->animation->quests->window_y + 16 * history_row + 47;
                MainForm->OffscreenSurface->BltFast(history_icon_x, history_icon_y,
                    MainForm->gfx->interface_graphics->list_icons,
                    &MainForm->gfx->interface_graphics->GetListIconRect(0, 5), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                history_end_x = history_icon_x + 268;
                MainForm->OffscreenSurface->BltFast(history_end_x, history_icon_y,
                    MainForm->gfx->interface_graphics->list_icons,
                    &MainForm->gfx->interface_graphics->GetListIconRect(1, 0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            }
            history_row++;
        }
    }
    if (MainForm->animation->quests->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 449,
            MainForm->animation->quests->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 449,
            MainForm->animation->quests->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->quests->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 449,
            MainForm->animation->quests->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 449,
            MainForm->animation->quests->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 449,
        MainForm->animation->quests->window_y + MainForm->animation->quests->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the message board window in the current view (post list or a post): background, hovered row highlight, and
// the scroll arrows and thumb at the view's positions.
void GameGraphics::DrawMsgBoardWindow()
{
    MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x,
        MainForm->animation->message_board->window_y, MainForm->gfx->message_board->surface,
        &MainForm->gfx->message_board->GetFrameRect(MainForm->animation->message_board->view_mode), DDBLTFAST_WAIT);
    if (MainForm->animation->message_board->hovered_row >= 0)
    {
        MainForm->blitter->AlphaBlt(MainForm->animation->message_board->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->message_board->hovered_row +
                MainForm->animation->message_board->window_y + 47, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->control_sheet, &narrow_row_highlight_rect, 12);
        MainForm->blitter->AlphaBlt(MainForm->animation->message_board->window_x + 231,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->message_board->hovered_row +
                MainForm->animation->message_board->window_y + 47, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->control_sheet, &narrow_row_highlight_rect, 12);
    }
    if (!MainForm->animation->message_board->view_mode)
    {
        if (MainForm->animation->message_board->scroll_up_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
        if (MainForm->animation->message_board->scroll_down_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    }
    else
    {
        if (MainForm->animation->message_board->scroll_up_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 74, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 74, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
        if (MainForm->animation->message_board->scroll_down_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
                MainForm->animation->message_board->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
                &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    }
    if (!MainForm->animation->message_board->view_mode)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
            MainForm->animation->message_board->window_y + MainForm->animation->message_board->GetScrollThumbOffset() +
                60, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(7), DDBLTFAST_WAIT);
    if (MainForm->animation->message_board->view_mode)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 449,
            MainForm->animation->message_board->window_y + MainForm->animation->message_board->GetScrollThumbOffset() +
                90, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(7), DDBLTFAST_WAIT);
}

// Draws the shop window: background, hovered row highlight, a slot with the item graphic for each visible item, the
// scroll arrows, the menu icons on the first page, and the scroll thumb.
void GameGraphics::DrawShopWindow()
{
    int graphic;
    int item_id;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x, MainForm->animation->shop->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->shop->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->shop->window_x + 16,
            MENU_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->shop->hovered_row + MainForm->animation->shop->window_y +
                49, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet,
            &tall_row_highlight_rect, 12);
    int top_row = MainForm->animation->shop->GetTopRow();
    int item_count = MainForm->animation->shop->shown_items.size();
    int row = 0;
    int i = top_row;
    while (i < item_count && top_row + 5 > i)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 18,
            MainForm->animation->shop->window_y + 37 * row + 50, MainForm->gfx->shop->item_slot_surface,
            &MainForm->gfx->shop->item_slot_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        item_id = MainForm->animation->shop->shown_items[i].item_id;
        graphic = MainForm->item_values->GetGraphic(item_id) - 1;
        if (graphic >= 0)
        {
            if (MainForm->gfx->items->RequestGraphic(graphic))
            {
                GraphicSize size = MainForm->gfx->items->GetGraphicSize(graphic);
                int item_x = MainForm->animation->shop->window_x + 40 - size.width / 2;
                int item_y = MainForm->animation->shop->window_y + 37 * row + 58 - size.height / 2;
                MainForm->gfx->game->BlitClipped(item_x, item_y,
                    MainForm->gfx->items->GetGraphic(graphic).resource->surface,
                    MainForm->gfx->items->GetGraphic(graphic).resource->surface_rect);
            }
            else
                MainForm->redraw_needed = true;
        }
        row++;
        i++;
    }
    if (MainForm->animation->shop->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 252,
            MainForm->animation->shop->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 252,
            MainForm->animation->shop->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->shop->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 252,
            MainForm->animation->shop->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 252,
            MainForm->animation->shop->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    if (MainForm->animation->shop->page == ShopPage_Menu)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 22,
            MainForm->animation->shop->window_y + 50, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(0),
            DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 22,
            MainForm->animation->shop->window_y + 87, MainForm->gfx->interface_graphics->control_sheet, &GetIconRect(1),
            DDBLTFAST_WAIT);
        // The craft row is shown only when the shop has craft items.
        if (MainForm->animation->shop->craft_items.size() > 0)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 22,
                MainForm->animation->shop->window_y + 124, MainForm->gfx->interface_graphics->control_sheet,
                &GetIconRect(4), DDBLTFAST_WAIT);
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 252,
        MainForm->animation->shop->window_y + MainForm->animation->shop->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the skill master window: background, the hovered row or line highlight, each offered skill's icon on the
// skill page, the scroll arrows, the menu icons on the first page, and the scroll thumb.
void GameGraphics::DrawLearnWindow()
{
    int icon_id;
    int top_line;
    int skill_count;
    int row;
    TPoint icon_width;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x, MainForm->animation->learn->window_y,
        MainForm->gfx->shop->background_surface, &MainForm->gfx->shop->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->learn->page <= LearnPage_SkillList)
    {
        if (MainForm->animation->learn->hovered_row >= 0)
            MainForm->blitter->AlphaBlt(MainForm->animation->learn->window_x + 16,
                MENU_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->learn->hovered_row +
                    MainForm->animation->learn->window_y + 49, MainForm->OffscreenSurface,
                MainForm->gfx->interface_graphics->control_sheet, &tall_row_highlight_rect, 12);
    }
    else if (MainForm->animation->learn->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->learn->window_x + 16,
            LIST_HIGHLIGHT_ROW_HEIGHT * MainForm->animation->learn->hovered_row + MainForm->animation->learn->window_y +
            47, MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &row_highlight_rect, 12);
    if (MainForm->animation->learn->page == LearnPage_SkillList)
    {
        top_line = MainForm->animation->learn->GetTopLine();
        skill_count = MainForm->animation->learn->offered_skills.size();
        row = 0;
        int i = top_line;
        while (i < skill_count && i < top_line + 5)
        {
            int arg = MainForm->animation->learn->shown_skills[i].skill_id;
            icon_id = MainForm->skill_values->GetIcon(arg);
            if (icon_id >= 0)
            {
                if (MainForm->gfx->icons->RequestGraphic(icon_id))
                {
                    icon_width.x = 34;
                    icon_width.y = 32;
                    int icon_x = MainForm->animation->learn->window_x + 30 - icon_width.x / 2;
                    int icon_y = MainForm->animation->learn->window_y + 37 * row + 58 - icon_width.y / 2;
                    MainForm->gfx->game->BlitClipped(icon_x, icon_y,
                        MainForm->gfx->icons->GetGraphic(icon_id).resource->surface,
                        MainForm->gfx->icons->GetGraphic(icon_id).resource->GetIconRect(0));
                }
                else
                    MainForm->redraw_needed = true;
            }
            row++;
            i++;
        }
    }
    if (MainForm->animation->learn->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 252,
            MainForm->animation->learn->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 252,
            MainForm->animation->learn->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->learn->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 252,
            MainForm->animation->learn->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 252,
            MainForm->animation->learn->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    if (MainForm->animation->learn->page == LearnPage_Menu)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 22,
            MainForm->animation->learn->window_y + 50, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(20), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 22,
            MainForm->animation->learn->window_y + 87, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(21), DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 22,
            MainForm->animation->learn->window_y + 124, MainForm->gfx->interface_graphics->control_sheet,
            &GetIconRect(21), DDBLTFAST_WAIT);
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 252,
        MainForm->animation->learn->window_y + MainForm->animation->learn->GetScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the trade window: background, hovered row highlight, both scroll bars, and a slot with the item graphic for
// each visible item of the partner's offer (left) and the player's own offer (right).
void GameGraphics::DrawTradeWindow()
{
    int graphic;
    int own_graphic;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x, MainForm->animation->trade->window_y,
        MainForm->gfx->trade->background_surface, &MainForm->gfx->trade->background_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->trade->hovered_row >= 0)
        MainForm->blitter->AlphaBlt(MainForm->animation->trade->window_x + 282,
            37 * MainForm->animation->trade->hovered_row + MainForm->animation->trade->window_y + 47,
            MainForm->OffscreenSurface, MainForm->gfx->interface_graphics->control_sheet, &item_row_highlight_rect, 12);
    if (MainForm->animation->trade->partner_scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 252,
            MainForm->animation->trade->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 252,
            MainForm->animation->trade->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->trade->partner_scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 252,
            MainForm->animation->trade->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 252,
            MainForm->animation->trade->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    if (MainForm->animation->trade->own_scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 518,
            MainForm->animation->trade->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 518,
            MainForm->animation->trade->window_y + 44, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->trade->own_scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 518,
            MainForm->animation->trade->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 518,
            MainForm->animation->trade->window_y + 228, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(8), DDBLTFAST_WAIT);
    int top_row = MainForm->animation->trade->GetPartnerTopRow();
    int item_count = MainForm->animation->trade->partner_items.size();
    int row = 0;
    for (int i = top_row; i < item_count && top_row + 5 > i; i++)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 18,
            MainForm->animation->trade->window_y + 37 * row + 50, MainForm->gfx->trade->item_slot_surface,
            &MainForm->gfx->trade->item_slot_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        int partner_item_id = MainForm->animation->trade->partner_items[i].item_id;
        int partner_amount = MainForm->animation->trade->partner_items[i].amount;
        graphic = MainForm->item_values->GetGraphic(partner_item_id) - 1;
        if (graphic >= 0)
        {
            if (partner_item_id == ItemId_Gold)
            {
                graphic = 134;
                if (partner_amount > 1)
                    graphic = 135;
                if (partner_amount > 99)
                    graphic = 136;
                if (partner_amount > 9999)
                    graphic = 137;
                if (partner_amount > 99999)
                    graphic = 138;
            }
            if (MainForm->gfx->items->RequestGraphic(graphic))
            {
                GraphicSize partner_size = MainForm->gfx->items->GetGraphicSize(graphic);
                int partner_item_x = MainForm->animation->trade->window_x + 40 - partner_size.width / 2;
                int partner_item_y = MainForm->animation->trade->window_y + 37 * row + 58 - partner_size.height / 2;
                MainForm->gfx->game->BlitClipped(partner_item_x, partner_item_y,
                    MainForm->gfx->items->GetGraphic(graphic).resource->surface,
                    MainForm->gfx->items->GetGraphic(graphic).resource->surface_rect);
            }
            else
                MainForm->redraw_needed = true;
        }
        row++;
    }
    top_row = MainForm->animation->trade->GetOwnTopRow();
    item_count = MainForm->animation->trade->own_items.size();
    row = 0;
    for (int j = top_row; j < item_count && top_row + 5 > j; j++)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 284,
            MainForm->animation->trade->window_y + 37 * row + 50, MainForm->gfx->trade->item_slot_surface,
            &MainForm->gfx->trade->item_slot_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        int own_item_id = MainForm->animation->trade->own_items[j].item_id;
        int own_amount = MainForm->animation->trade->own_items[j].amount;
        own_graphic = MainForm->item_values->GetGraphic(own_item_id) - 1;
        if (own_graphic >= 0)
        {
            if (own_item_id == ItemId_Gold)
            {
                own_graphic = 134;
                if (own_amount > 1)
                    own_graphic = 135;
                if (own_amount > 99)
                    own_graphic = 136;
                if (own_amount > 9999)
                    own_graphic = 137;
                if (own_amount > 99999)
                    own_graphic = 138;
            }
            if (MainForm->gfx->items->RequestGraphic(own_graphic))
            {
                GraphicSize own_size = MainForm->gfx->items->GetGraphicSize(own_graphic);
                int own_item_x = MainForm->animation->trade->window_x + 306 - own_size.width / 2;
                int own_item_y = MainForm->animation->trade->window_y + 37 * row + 58 - own_size.height / 2;
                MainForm->gfx->game->BlitClipped(own_item_x, own_item_y,
                    MainForm->gfx->items->GetGraphic(own_graphic).resource->surface,
                    MainForm->gfx->items->GetGraphic(own_graphic).resource->surface_rect);
            }
            else
                MainForm->redraw_needed = true;
        }
        row++;
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 252,
        MainForm->animation->trade->window_y + MainForm->animation->trade->GetPartnerScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 518,
        MainForm->animation->trade->window_y + MainForm->animation->trade->GetOwnScrollThumbOffset() + 60,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(7),
        DDBLTFAST_WAIT);
}

// Draws the book window: background, the status icon for the book's icon type, a bullet per visible line, and the
// scroll bar with its arrows and thumb.
void GameGraphics::DrawBookWindow()
{
    int status_icon;
    int window_x;
    int window_y;
    int top_line;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x, MainForm->animation->book->window_y,
        book_surface, &book_rect, DDBLTFAST_WAIT);
    window_x = MainForm->animation->book->window_x;
    window_y = MainForm->animation->book->window_y;
    status_icon = 9;
    if (MainForm->animation->book->icon_type == 4)
        status_icon = 12;
    if (MainForm->animation->book->icon_type == 9)
        status_icon = 13;
    if (MainForm->animation->book->icon_type == 5)
        status_icon = 14;
    if (MainForm->animation->book->icon_type == 10)
        status_icon = 15;
    if (MainForm->animation->book->icon_type == 6)
        status_icon = 10;
    MainForm->OffscreenSurface->BltFast(window_x + 241, window_y + 262, MainForm->gfx->interface_graphics->small_icons,
        &MainForm->gfx->interface_graphics->GetSmallIconRect(status_icon), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    top_line = MainForm->animation->book->GetTopLine();
    int line_count = MainForm->animation->book->lines->Count;
    int row = 0;
    for (int i = top_line; i < line_count && i < top_line + 14; i++)
    {
        String line = MainForm->animation->book->lines->Strings[i];
        if (line.Length() > 0)
        {
            MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            int icon_x = MainForm->animation->book->window_x + 26;
            int icon_y = MainForm->animation->book->window_y + BOOK_ROW_HEIGHT * row + 41;
            MainForm->OffscreenSurface->BltFast(icon_x, icon_y, MainForm->gfx->interface_graphics->list_icons,
                &MainForm->gfx->interface_graphics->GetListIconRect(0, 0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            // Only non-empty lines take a row, so blank lines are skipped.
            row++;
        }
    }
    MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 188,
        MainForm->animation->book->window_y + 33, MainForm->gfx->interface_graphics->control_sheet,
        &MainForm->gfx->interface_graphics->book_scrollbar_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->book->scroll_up_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 190,
            MainForm->animation->book->window_y + 35, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(21), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 190,
            MainForm->animation->book->window_y + 35, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(20), DDBLTFAST_WAIT);
    if (MainForm->animation->book->scroll_down_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 190,
            MainForm->animation->book->window_y + 255, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(19), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 190,
            MainForm->animation->book->window_y + 255, MainForm->gfx->interface_graphics->scroll_icons,
            &MainForm->gfx->interface_graphics->GetScrollIconRect(18), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 190,
        MainForm->animation->book->window_y + MainForm->animation->book->GetScrollThumbOffset() + 51,
        MainForm->gfx->interface_graphics->scroll_icons, &MainForm->gfx->interface_graphics->GetScrollIconRect(17),
        DDBLTFAST_WAIT);
}

// Draws the paperdoll window: the background frame, the status icon, and the graphic of each equipped item in its
// box; the weapon, armor, shield and hat are centred by item size.
void GameGraphics::DrawPaperdollWindow()
{
    int status_icon;
    int window_x;
    int origin_y;
    TPoint slot_offset;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x,
        MainForm->animation->paperdoll->window_y, paperdoll_surface,
        &GetPaperdollFrameRect(MainForm->animation->paperdoll->frame_index), DDBLTFAST_WAIT);
    window_x = MainForm->animation->paperdoll->window_x;
    origin_y = MainForm->animation->paperdoll->window_y;
    status_icon = 9;
    if (MainForm->animation->paperdoll->icon_type == 4)
        status_icon = 12;
    if (MainForm->animation->paperdoll->icon_type == 9)
        status_icon = 13;
    if (MainForm->animation->paperdoll->icon_type == 5)
        status_icon = 14;
    if (MainForm->animation->paperdoll->icon_type == 10)
        status_icon = 15;
    if (MainForm->animation->paperdoll->icon_type == 6)
        status_icon = 10;
    MainForm->OffscreenSurface->BltFast(window_x + 241, origin_y + 262, MainForm->gfx->interface_graphics->small_icons,
        &MainForm->gfx->interface_graphics->GetSmallIconRect(status_icon), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->paperdoll->weapon_graphic > 0)
    {
        slot_offset = GetPaperdollSlotOffset(MainForm->animation->paperdoll->weapon_item_size);
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->weapon_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + slot_offset.x + 21,
                MainForm->animation->paperdoll->window_y + slot_offset.y + 80,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->weapon_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->weapon_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->armor_graphic > 0)
    {
        slot_offset = GetPaperdollSlotOffset(MainForm->animation->paperdoll->armor_item_size);
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->armor_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + slot_offset.x + 86,
                MainForm->animation->paperdoll->window_y + slot_offset.y + 80,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->armor_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->armor_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->shield_graphic > 0)
    {
        slot_offset = GetPaperdollSlotOffset(MainForm->animation->paperdoll->shield_item_size);
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->shield_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + slot_offset.x + 151,
                MainForm->animation->paperdoll->window_y + slot_offset.y + 80,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->shield_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->shield_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->head_graphic > 0)
    {
        slot_offset = GetPaperdollSlotOffset(MainForm->animation->paperdoll->head_item_size);
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->head_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 91,
                MainForm->animation->paperdoll->window_y + slot_offset.y - 2,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->head_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->head_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->necklace_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->necklace_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 156,
                MainForm->animation->paperdoll->window_y + 50,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->necklace_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->necklace_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->gloves_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->gloves_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 26,
                MainForm->animation->paperdoll->window_y + 187,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->gloves_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->gloves_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->boots_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->boots_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 91,
                MainForm->animation->paperdoll->window_y + 218,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->boots_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->boots_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->belt_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->belt_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 91,
                MainForm->animation->paperdoll->window_y + 187,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->belt_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->belt_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->ring1_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->ring1_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 151,
                MainForm->animation->paperdoll->window_y + 187,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->ring1_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->ring1_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->ring2_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->ring2_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 184,
                MainForm->animation->paperdoll->window_y + 187,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->ring2_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->ring2_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->armlet1_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->armlet1_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 151,
                MainForm->animation->paperdoll->window_y + 218,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->armlet1_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->armlet1_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->armlet2_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->armlet2_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 184,
                MainForm->animation->paperdoll->window_y + 218,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->armlet2_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->armlet2_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->bracer1_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->bracer1_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 151,
                MainForm->animation->paperdoll->window_y + 249,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->bracer1_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->bracer1_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->bracer2_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->bracer2_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 184,
                MainForm->animation->paperdoll->window_y + 249,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->bracer2_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->bracer2_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
    if (MainForm->animation->paperdoll->accessory_graphic > 0)
    {
        if (MainForm->gfx->items->RequestSubItemGraphic(MainForm->animation->paperdoll->accessory_graphic))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 54,
                MainForm->animation->paperdoll->window_y + 249,
                MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->accessory_graphic).resource->surface,
                &MainForm->gfx->items->GetSubItemGraphic(MainForm->animation->paperdoll->accessory_graphic).resource->surface_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->redraw_needed = true;
    }
}

// Draws the chest window's close button, lit under the mouse.
void GameGraphics::DrawChestButtons()
{
    if (MainForm->animation->chest->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->chest->window_x + 92,
            MainForm->animation->chest->window_y + 227, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->chest->window_x + 92,
            MainForm->animation->chest->window_y + 227, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the barber window's close button, lit under the mouse.
void GameGraphics::DrawBarberButtons()
{
    if (MainForm->animation->barber->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->barber->window_x + 215,
            MainForm->animation->barber->window_y + 151, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->barber->window_x + 215,
            MainForm->animation->barber->window_y + 151, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the musician window's close button, lit under the mouse.
void GameGraphics::DrawMusicianButtons()
{
    if (MainForm->animation->musician->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->musician->window_x + 92,
            MainForm->animation->musician->window_y + 82, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->musician->window_x + 92,
            MainForm->animation->musician->window_y + 82, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the jukebox window's close button, lit under the mouse.
void GameGraphics::DrawJukeboxButtons()
{
    if (MainForm->animation->jukebox->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->jukebox->window_x + 92,
            MainForm->animation->jukebox->window_y + 158, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->jukebox->window_x + 92,
            MainForm->animation->jukebox->window_y + 158, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the experience window's close button, lit under the mouse.
void GameGraphics::DrawStatButtons()
{
    if (MainForm->animation->stat->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->stat->window_x + 96,
            MainForm->animation->stat->window_y + 212, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->stat->window_x + 96,
            MainForm->animation->stat->window_y + 212, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the bank window's close button, lit under the mouse.
void GameGraphics::DrawBankButtons()
{
    if (MainForm->animation->bank->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->bank->window_x + 92,
            MainForm->animation->bank->window_y + 191, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->bank->window_x + 92,
            MainForm->animation->bank->window_y + 191, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the locker window's close button, lit under the mouse.
void GameGraphics::DrawLockerButtons()
{
    if (MainForm->animation->locker->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 96,
            MainForm->animation->locker->window_y + 252, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->locker->window_x + 96,
            MainForm->animation->locker->window_y + 252, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the quest window's history button, whose face changes while history is shown, and its close button, lit
// under the mouse.
void GameGraphics::DrawQuestButtons()
{
    int left_button = 8;
    if (MainForm->animation->quests->showing_history == 1)
        left_button = 9;
    if (MainForm->animation->quests->history_button_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 288,
            MainForm->animation->quests->window_y + 252, button_sheet, &GetButtonRect(left_button, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 288,
            MainForm->animation->quests->window_y + 252, button_sheet, &GetButtonRect(left_button, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->quests->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 380,
            MainForm->animation->quests->window_y + 252, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->quests->window_x + 380,
            MainForm->animation->quests->window_y + 252, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the message board's action button for the current view and its close button, lit under the mouse.
void GameGraphics::DrawMsgBoardButtons()
{
    if (!MainForm->animation->message_board->view_mode)
    {
        if (MainForm->animation->message_board->action_button_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 284,
                MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(6, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 284,
                MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(6, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    // Reading a post shows the action button only to admins.
    if (MainForm->animation->message_board->view_mode == 1 && MainForm->net_client->GetAdminLevel() > 0)
    {
        if (MainForm->animation->message_board->action_button_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 284,
                MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(3, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 284,
                MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(3, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->message_board->view_mode == MSGBOARD_MODE_WRITE)
    {
        if (MainForm->animation->message_board->action_button_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 284,
                MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(4, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 284,
                MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(4, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->message_board->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 380,
            MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->message_board->window_x + 380,
            MainForm->animation->message_board->window_y + 252, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the social window's close and add buttons, lit under the mouse.
void GameGraphics::DrawSocialButtons()
{
    if (MainForm->animation->social->close_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 144,
            MainForm->animation->social->window_y + 252, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 144,
            MainForm->animation->social->window_y + 252, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->social->add_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 48,
            MainForm->animation->social->window_y + 252, button_sheet, &GetButtonRect(6, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->social->window_x + 48,
            MainForm->animation->social->window_y + 252, button_sheet, &GetButtonRect(6, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the guild window's close button on the first page, or close and back on the others, lit under the mouse.
void GameGraphics::DrawGuildButtons()
{
    if (MainForm->animation->guild->page == GuildPage_Main)
    {
        if (MainForm->animation->guild->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 96,
                MainForm->animation->guild->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 96,
                MainForm->animation->guild->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        if (MainForm->animation->guild->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 144,
                MainForm->animation->guild->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 144,
                MainForm->animation->guild->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->guild->back_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 48,
                MainForm->animation->guild->window_y + 252, button_sheet, &GetButtonRect(5, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->guild->window_x + 48,
                MainForm->animation->guild->window_y + 252, button_sheet, &GetButtonRect(5, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the help report window's close button, lit under the mouse.
void GameGraphics::DrawReportButtons()
{
    if (MainForm->animation->report->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 96,
            MainForm->animation->report->window_y + 252, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->report->window_x + 96,
            MainForm->animation->report->window_y + 252, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the quest dialog's two buttons of the kinds the dialog set, or only the right one, as Cancel, on the quest
// list.
void GameGraphics::DrawDialogButtons()
{
    int left_button = MainForm->animation->dialog->prev_button_kind;
    int right_button = MainForm->animation->dialog->next_button_kind;
    if (MainForm->animation->dialog->showing_quest_list)
        right_button = 1;
    if (!MainForm->animation->dialog->showing_quest_list)
    {
        if (MainForm->animation->dialog->prev_button_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 89,
                MainForm->animation->dialog->window_y + 152, button_sheet, &GetButtonRect(left_button, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 89,
                MainForm->animation->dialog->window_y + 152, button_sheet, &GetButtonRect(left_button, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->dialog->next_button_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 183,
            MainForm->animation->dialog->window_y + 152, button_sheet, &GetButtonRect(right_button, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->dialog->window_x + 183,
            MainForm->animation->dialog->window_y + 152, button_sheet, &GetButtonRect(right_button, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the memo window's close button, lit under the mouse.
void GameGraphics::DrawMemoButtons()
{
    if (MainForm->animation->memo->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 96,
            MainForm->animation->memo->window_y + 252, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->memo->window_x + 96,
            MainForm->animation->memo->window_y + 252, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the inn window's close button on the first page, or close and back on the others, lit under the mouse.
void GameGraphics::DrawInnButtons()
{
    if (!MainForm->animation->inn->page)
    {
        if (MainForm->animation->inn->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 96,
                MainForm->animation->inn->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 96,
                MainForm->animation->inn->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        if (MainForm->animation->inn->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 144,
                MainForm->animation->inn->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 144,
                MainForm->animation->inn->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->inn->back_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 48,
                MainForm->animation->inn->window_y + 252, button_sheet, &GetButtonRect(5, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->inn->window_x + 48,
                MainForm->animation->inn->window_y + 252, button_sheet, &GetButtonRect(5, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the law window's close button on the first page, or close and back on the others, lit under the mouse.
void GameGraphics::DrawLawButtons()
{
    if (!MainForm->animation->law->page)
    {
        if (MainForm->animation->law->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 96,
                MainForm->animation->law->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 96,
                MainForm->animation->law->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        if (MainForm->animation->law->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 144,
                MainForm->animation->law->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 144,
                MainForm->animation->law->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->law->back_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 48,
                MainForm->animation->law->window_y + 252, button_sheet, &GetButtonRect(5, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->law->window_x + 48,
                MainForm->animation->law->window_y + 252, button_sheet, &GetButtonRect(5, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the skill master window's close button on the first page, or close and back on the others.
void GameGraphics::DrawLearnButtons()
{
    if (MainForm->animation->learn->page == LearnPage_Menu)
    {
        if (MainForm->animation->learn->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 96,
                MainForm->animation->learn->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 96,
                MainForm->animation->learn->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        if (MainForm->animation->learn->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 144,
                MainForm->animation->learn->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 144,
                MainForm->animation->learn->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->learn->back_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 48,
                MainForm->animation->learn->window_y + 252, button_sheet, &GetButtonRect(5, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->learn->window_x + 48,
                MainForm->animation->learn->window_y + 252, button_sheet, &GetButtonRect(5, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the shop window's close button on the first page, or close and back on the others, lit under the mouse.
void GameGraphics::DrawShopButtons()
{
    if (MainForm->animation->shop->page == ShopPage_Menu)
    {
        if (MainForm->animation->shop->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 96,
                MainForm->animation->shop->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 96,
                MainForm->animation->shop->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    else
    {
        if (MainForm->animation->shop->close_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 144,
                MainForm->animation->shop->window_y + 252, button_sheet, &GetButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 144,
                MainForm->animation->shop->window_y + 252, button_sheet, &GetButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->shop->back_hovered)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 48,
                MainForm->animation->shop->window_y + 252, button_sheet, &GetButtonRect(5, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->shop->window_x + 48,
                MainForm->animation->shop->window_y + 252, button_sheet, &GetButtonRect(5, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the trade window's agree and cancel buttons, lit under the mouse.
void GameGraphics::DrawTradeButtons()
{
    if (MainForm->animation->trade->agree_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 356,
            MainForm->animation->trade->window_y + 252, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 356,
            MainForm->animation->trade->window_y + 252, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->trade->cancel_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 449,
            MainForm->animation->trade->window_y + 252, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->trade->window_x + 449,
            MainForm->animation->trade->window_y + 252, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the book window's close button, lit under the mouse.
void GameGraphics::DrawBookButtons()
{
    if (MainForm->animation->book->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 276,
            MainForm->animation->book->window_y + 253, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->book->window_x + 276,
            MainForm->animation->book->window_y + 253, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the paperdoll window's close button, lit under the mouse.
void GameGraphics::DrawPaperdollButtons()
{
    if (MainForm->animation->paperdoll->close_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 276,
            MainForm->animation->paperdoll->window_y + 253, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->paperdoll->window_x + 276,
            MainForm->animation->paperdoll->window_y + 253, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the amount box OK and Cancel buttons, lit under the mouse.
void GameGraphics::DrawItemAmountButtons()
{
    if (MainForm->animation->game->amount_ok_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->amount_box_x + 60,
            MainForm->animation->game->amount_box_y + 125, button_sheet, &GetButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->amount_box_x + 60,
            MainForm->animation->game->amount_box_y + 125, button_sheet, &GetButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->game->amount_cancel_hovered)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->amount_box_x + 153,
            MainForm->animation->game->amount_box_y + 125, button_sheet, &GetButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->game->amount_box_x + 153,
            MainForm->animation->game->amount_box_y + 125, button_sheet, &GetButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// One paperdoll background frame on the paperdoll sheet.
TRect GameGraphics::GetPaperdollFrameRect(int frame)
{
    TRect paperdoll_frame_rect;
    paperdoll_frame_rect.left = 0;
    paperdoll_frame_rect.top = 290 * frame;
    paperdoll_frame_rect.right = 380;
    paperdoll_frame_rect.bottom = 290 * frame + 290;
    return paperdoll_frame_rect;
}

// The highlight of one row of the player popup menu on the popup menu sheet.
TRect GameGraphics::GetPopupItemRect(int row)
{
    TRect menu_row_rect;
    menu_row_rect.left = 109;
    menu_row_rect.top = 15 * row + 9;
    menu_row_rect.right = 169;
    menu_row_rect.bottom = 15 * row + 24;
    return menu_row_rect;
}

// One button of the button sheet, plain or lit.
TRect GameGraphics::GetButtonRect(int button, int state)
{
    TRect button_rect;
    button_rect.left = BUTTON_COLUMN_PITCH * state;
    button_rect.top = BUTTON_ROW_PITCH * button;
    button_rect.right = BUTTON_COLUMN_PITCH * state + 90;
    button_rect.bottom = BUTTON_ROW_PITCH * button + 28;
    return button_rect;
}

// Trims a source rect by how far a blit at a view position would run past each side of the view.
TRect GameGraphics::ClipSourceRect(int x, int y, TRect source_rect)
{
    TRect clipped;
    clipped = source_rect;
    int width = source_rect.right - source_rect.left;
    int height = source_rect.bottom - source_rect.top;
    if (x < view_rect.left)
        clipped.left = view_rect.left - x + source_rect.left;
    if (y < view_rect.top)
        clipped.top = view_rect.top - y + source_rect.top;
    if (x + width > view_rect.right)
    {
        clipped.right -= x + width - view_rect.right;
        if (clipped.right < 0)
            clipped.right = 0;
    }
    if (y + height > view_rect.bottom)
    {
        clipped.bottom -= y + height - view_rect.bottom;
        if (clipped.bottom < 0)
            clipped.bottom = 0;
    }
    return clipped;
}

// Draws the button sheet bitmap onto its surface, with black see-through.
void GameGraphics::LoadButtonSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (button_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        button_sheet->ReleaseDC(dc);
    }
    button_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the player popup menu bitmap onto its surface, with black see-through.
void GameGraphics::LoadPopupMenu(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (extra_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        extra_surface->ReleaseDC(dc);
    }
    extra_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the input form background bitmap onto its surface.
void GameGraphics::LoadListBoxBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (list_box_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        list_box_surface->ReleaseDC(dc);
    }
}

// Draws the input box background bitmap onto its surface.
void GameGraphics::LoadInputBoxBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (input_box_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        input_box_surface->ReleaseDC(dc);
    }
}

// Draws the message box background bitmap onto its surface.
void GameGraphics::LoadMessageBoxBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (message_box_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        message_box_surface->ReleaseDC(dc);
    }
}

// Draws the text box background bitmap onto its surface.
void GameGraphics::LoadTextBoxBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (text_box_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        text_box_surface->ReleaseDC(dc);
    }
}

// Draws the input form title, caption and the labels of up to five rows from the top row.
void GameGraphics::DrawFormText()
{
    int drawn;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = (TColor)0xC8F0F0;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 10;
    MainForm->text_canvas->TextOut(MainForm->animation->game->form_x + 20, MainForm->animation->game->form_y + 12,
        MainForm->animation->game->form_title);
    MainForm->text_canvas->Font->Color = (TColor)0xD6E6E6;
    MainForm->text_canvas->TextOut(MainForm->animation->game->form_x + 20, MainForm->animation->game->form_y + 40,
        MainForm->animation->game->form_caption);
    int top_row = MainForm->animation->game->GetInputFormTopRow();
    int end_row = top_row + 5;
    drawn = 0;
    for (int i = top_row; i < end_row && drawn < MainForm->animation->game->form_row_count; i++)
    {
        MainForm->text_canvas->TextOut(MainForm->animation->game->form_x + 26,
            MainForm->animation->game->form_y + 23 * drawn + 69, MainForm->animation->game->form_row_labels[i]);
        drawn++;
    }
    // While a message or text box is open, the row edits' text is drawn here in the small font.
    if (MainForm->animation->game->message_visible || MainForm->animation->game->large_box_visible)
    {
        MainForm->text_canvas->Font->Color = (TColor)0xB4C8DC;
        MainForm->text_canvas->Font->Name = "MS Sans Serif";
        MainForm->text_canvas->Font->Size = 8;
        for (int j = 14; j < 19; j++)
            MainForm->text_canvas->TextOut(MainForm->animation->game->form_x + 131,
                MainForm->animation->game->form_y + 23 * j - 249, MainForm->edits->GetText(j));
    }
}

// Draws each chest item's name and amount; an item of type 12 also shows its gender word.
void GameGraphics::DrawChestText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int item_count = MainForm->animation->chest->items.size();
    int row = 0;
    for (int i = 0; i < item_count && i < 5; i++)
    {
        int item_id = MainForm->animation->chest->items[i].item_id;
        int amount = MainForm->animation->chest->items[i].amount;
        String item_name = MainForm->item_values->GetName(item_id);
        int item_type = MainForm->item_values->GetType(item_id);
        int text_x = MainForm->animation->chest->window_x + 76;
        int text_y = MainForm->animation->chest->window_y + 37 * row + 30;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->net_client->Capitalize(item_name));
        text_y = MainForm->animation->chest->window_y + 37 * row + 45;
        if (item_type == ITEM_TYPE_GENDERED)
        {
            if (!MainForm->item_values->GetGender(item_id))
                MainForm->text_canvas->TextOut(text_x, text_y,
                    "x " + IntToStr(amount) + "  (" + MainForm->strings->GetText(GameText_Female) + ")");
            else
                MainForm->text_canvas->TextOut(text_x, text_y,
                    "x " + IntToStr(amount) + "  (" + MainForm->strings->GetText(GameText_Male) + ")");
        }
        else
            MainForm->text_canvas->TextOut(text_x, text_y, "x " + IntToStr(amount));
        row++;
    }
}

// Draws the barber rows: the chosen hair style, the chosen colour's name and the price in gold.
void GameGraphics::DrawBarberText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int text_y = MainForm->animation->barber->window_y + 26;
    int text_x = MainForm->animation->barber->window_x + 76;
    String gold_name = LowerCase(MainForm->item_values->GetName(ItemId_Gold));
    MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_ChangeModal));
    MainForm->text_canvas->TextOut(text_x, text_y + 14,
        MainForm->strings->GetText(GameText_Current) + ": " + IntToStr(MainForm->animation->barber->hair_style));
    MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_ChangeColor));
    MainForm->text_canvas->TextOut(text_x, text_y + 51,
        MainForm->strings->GetText(GameText_Current) + ": " +
            MainForm->strings->GetText(MainForm->animation->barber->hair_color + 167));
    MainForm->text_canvas->TextOut(text_x, text_y + 74, MainForm->strings->GetText(GameText_BuyHairstyle));
    MainForm->text_canvas->TextOut(text_x, text_y + 88,
        MainForm->strings->GetText(GameText_Current) + ": " + IntToStr(MainForm->animation->barber->price) + " " +
            gold_name);
}

// Draws whether a song is playing, with the requester's name if known, and the rows for the chosen song and the
// price.
void GameGraphics::DrawJukeboxText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_y = MainForm->animation->jukebox->window_y;
    int window_x = MainForm->animation->jukebox->window_x;
    String status_text = "";
    if (MainForm->animation->jukebox->is_playing)
    {
        // A name of three characters or fewer is left out.
        if (MainForm->animation->jukebox->requester_name.Length() > 3)
            status_text =
                MainForm->strings->GetText(GameText_PlayingRequest) + " (" +
                    MainForm->animation->jukebox->requester_name + ")";
        else
            status_text = MainForm->strings->GetText(GameText_PlayingRequest);
    }
    else
        status_text = MainForm->strings->GetText(GameText_JukeboxIsReady);
    MainForm->text_canvas->TextOut(window_x + 24, window_y + 22, status_text);
    int text_y = MainForm->animation->jukebox->window_y + 64;
    int text_x = MainForm->animation->jukebox->window_x + 76;
    String gold_name = LowerCase(MainForm->item_values->GetName(ItemId_Gold));
    MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_BrowseThroughSongs));
    MainForm->text_canvas->TextOut(text_x, text_y + 14,
        MainForm->strings->GetText(GameText_Current) + " : " +
            MainForm->strings->GetSongName(MainForm->animation->jukebox->selected_song + 1));
    MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_PlaySelectedSong));
    MainForm->text_canvas->TextOut(text_x, text_y + 51,
        MainForm->strings->GetText(GameText_Current) + " : 25 " + gold_name);
}

// Draws the experience window: the player's experience, the next level's, what is left to it, the session gain,
// the per-hour rates over all play and over the session, and the best and last gains.
void GameGraphics::DrawStatText()
{
    int elapsed_minutes;
    int elapsed_ms;
    int elapsed_days;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->stat->window_x;
    int window_y = MainForm->animation->stat->window_y;
    TDateTime now = Now();
    TTimeStamp start_stamp;
    // The session length in minutes comes from the day and millisecond parts of the two timestamps.
    start_stamp = DateTimeToTimeStamp(MainForm->animation->interface_controller->status->usage_start_time);
    TTimeStamp now_stamp;
    now_stamp = DateTimeToTimeStamp(now);
    int session_rate;
    double session_rate_scaled;
    double session_rate_hourly;
    int total_rate;
    double total_rate_scaled;
    double total_rate_hourly;
    int text_y;
    elapsed_ms = now_stamp.Time - start_stamp.Time;
    elapsed_days = now_stamp.Date - start_stamp.Date;
    elapsed_minutes = elapsed_ms / 60000 + 1440 * elapsed_days;
    session_rate = elapsed_minutes;
    if (!session_rate)
        session_rate = 1;
    if (session_rate < 60)
    {
        if (session_rate > 0)
        {
            session_rate_scaled =
                (double)(60 / session_rate) * MainForm->animation->interface_controller->status->total_exp_gained;
            session_rate = session_rate_scaled;
        }
    }
    else
    {
        session_rate_hourly =
            (double)MainForm->animation->interface_controller->status->total_exp_gained / (session_rate / 60);
        session_rate = session_rate_hourly;
    }
    total_rate = MainForm->animation->interface_controller->status->usage_minutes + elapsed_minutes;
    if (!total_rate)
        total_rate = 1;
    if (total_rate < 60)
    {
        if (total_rate > 0)
        {
            total_rate_scaled =
                (double)(60 / total_rate) * MainForm->animation->interface_controller->status->total_exp_gained;
            total_rate = total_rate_scaled;
        }
    }
    else
    {
        total_rate_hourly = (double)MainForm->animation->interface_controller->status->experience / (total_rate / 60);
        total_rate = total_rate_hourly;
    }
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->strings->GetText(GameText_Performance));
    for (int i = 0; i < 8; i++)
    {
        int text_x = MainForm->animation->stat->window_x + 40;
        text_y = MainForm->animation->stat->window_y + LIST_ROW_HEIGHT * i + 48;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(i + 288));
        if (!i)
            MainForm->text_canvas->TextOut(text_x + 120, text_y,
                IntToStr(MainForm->animation->interface_controller->status->experience));
        if (i == 1)
            MainForm->text_canvas->TextOut(text_x + 120, text_y,
                IntToStr(MainForm->animation->interface_controller->status->next_level_experience));
        if (i == 2)
            MainForm->text_canvas->TextOut(text_x + 120, text_y,
                IntToStr(MainForm->animation->interface_controller->status->experience_to_next_level));
        if (i == 3)
            MainForm->text_canvas->TextOut(text_x + 120, text_y,
                IntToStr(MainForm->animation->interface_controller->status->total_exp_gained));
        if (i == 4)
            MainForm->text_canvas->TextOut(text_x + 120, text_y, IntToStr(total_rate));
        if (i == 5)
            MainForm->text_canvas->TextOut(text_x + 120, text_y, IntToStr(session_rate));
        if (i == 6)
            MainForm->text_canvas->TextOut(text_x + 120, text_y,
                IntToStr(MainForm->animation->interface_controller->status->best_exp_gain));
        if (i == 7)
            MainForm->text_canvas->TextOut(text_x + 120, text_y,
                IntToStr(MainForm->animation->interface_controller->status->last_exp_gain));
    }
}

// Draws the bank balance right-aligned and the two text lines of each of its three rows.
void GameGraphics::DrawBankText()
{
    int text_y;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->bank->window_x;
    int window_y = MainForm->animation->bank->window_y;
    String balance_text = IntToStr(MainForm->animation->bank->bank_gold);
    int value = MainForm->text_canvas->TextWidth(balance_text);
    MainForm->text_canvas->TextOut(window_x + 240 - value, window_y + 22, balance_text);
    text_y = MainForm->animation->bank->window_y + 60;
    int text_x = MainForm->animation->bank->window_x + 76;
    String gold_name = LowerCase(MainForm->item_values->GetName(ItemId_Gold));
    MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_Deposit));
    MainForm->text_canvas->TextOut(text_x, text_y + 14,
        MainForm->strings->GetText(GameText_Transfer) + " " + gold_name + " " +
            MainForm->strings->GetText(GameText_ToBankAccount));
    MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_Withdraw));
    MainForm->text_canvas->TextOut(text_x, text_y + 51,
        MainForm->strings->GetText(GameText_Take) + " " + gold_name + " " +
            MainForm->strings->GetText(GameText_FromBackAccount));
    MainForm->text_canvas->TextOut(text_x, text_y + 74, MainForm->strings->GetText(GameText_LockerUpgrade));
    MainForm->text_canvas->TextOut(text_x, text_y + 88, MainForm->strings->GetText(GameText_BuyMoreLockerSpace));
}

// Draws the quest book title and, per visible line, the quest name, its step and its progress, or the history lines;
// an empty book shows one fixed line instead.
void GameGraphics::DrawQuestText()
{
    int state_x;
    int top_line;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->quests->window_x;
    int window_y = MainForm->animation->quests->window_y;
    MainForm->text_canvas->TextOut(window_x + 20, window_y + 15, MainForm->animation->quests->title);
    if (!MainForm->animation->quests->showing_history)
    {
        top_line = MainForm->animation->quests->GetTopLine();
        int line_count = MainForm->animation->quests->lines->Count;
        int row = 0;
        for (int i = top_line; i < line_count && i < top_line + 12; i++)
        {
            String quest_line = MainForm->animation->quests->lines->Strings[i];
            if (quest_line.Length() > 0)
            {
                MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
                int text_x = MainForm->animation->quests->window_x + 42;
                int text_y = MainForm->animation->quests->window_y + LIST_ROW_HEIGHT * row + 48;
                MainForm->text_canvas->TextOut(text_x, text_y, quest_line);
                MainForm->text_canvas->Font->Style = TFontStyles();
                int column_x = text_x + 145;
                MainForm->text_canvas->TextOut(column_x, text_y,
                    MainForm->animation->quests->line_detail_texts->Strings[i]);
                column_x = text_x + 330;
                // A progress of "0" shows as "n/a".
                if (MainForm->animation->quests->line_progress_texts->Strings[i] != "0")
                    MainForm->text_canvas->TextOut(column_x, text_y,
                        MainForm->animation->quests->line_progress_texts->Strings[i]);
                else
                    MainForm->text_canvas->TextOut(column_x, text_y, "n/a");
            }
            row++;
        }
        if (MainForm->animation->quests->lines->Count == 0)
        {
            MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            int empty_text_x = MainForm->animation->quests->window_x + 24;
            int empty_text_y = MainForm->animation->quests->window_y + 48;
            MainForm->text_canvas->TextOut(empty_text_x, empty_text_y,
                MainForm->strings->GetText(GameText_YouDidNotStart));
        }
    }
    if (MainForm->animation->quests->showing_history == 1)
    {
        int history_top_line = MainForm->animation->quests->GetTopLine();
        int history_line_count = MainForm->animation->quests->history_lines->Count;
        int history_row = 0;
        for (int j = history_top_line; j < history_line_count && j < history_top_line + 12; j++)
        {
            String history_line = MainForm->animation->quests->history_lines->Strings[j];
            if (history_line.Length() > 0)
            {
                MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
                int history_text_x = MainForm->animation->quests->window_x + 42;
                int history_text_y = MainForm->animation->quests->window_y + 16 * history_row + 48;
                MainForm->text_canvas->TextOut(history_text_x, history_text_y, history_line);
                MainForm->text_canvas->Font->Style = TFontStyles();
                state_x = history_text_x + 268;
                MainForm->text_canvas->TextOut(state_x, history_text_y,
                    MainForm->strings->GetText(GameText_QuestCompleted));
            }
            history_row++;
        }
    }
}

// Draws the post author in a post view, and in the list view the board title and each visible post's author and
// subject.
void GameGraphics::DrawMsgBoardText()
{
    int window_x;
    int window_y;
    int top_line;
    int post_count;
    int row;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    window_x = MainForm->animation->message_board->window_x;
    window_y = MainForm->animation->message_board->window_y;
    if (MainForm->animation->message_board->view_mode == 1 ||
        MainForm->animation->message_board->view_mode == MSGBOARD_MODE_WRITE)
        MainForm->text_canvas->TextOut(window_x + 149, window_y + 15,
            MainForm->net_client->Capitalize(MainForm->animation->message_board->current_post_author));
    if (!MainForm->animation->message_board->view_mode)
    {
        MainForm->text_canvas->TextOut(window_x + 18, window_y + 15, MainForm->strings->GetText(GameText_TownBoard));
        top_line = MainForm->animation->message_board->GetTopLine();
        post_count = MainForm->animation->message_board->posts.size();
        row = 0;
        for (int i = top_line; i < post_count && i < top_line + 12; i++)
        {
            String author = MainForm->animation->message_board->posts[i].author;
            String subject = MainForm->animation->message_board->posts[i].subject;
            int value = window_x + 22;
            int text_y = 16 * row + window_y + 48;
            MainForm->text_canvas->TextOut(value, text_y, MainForm->net_client->Capitalize(author));
            MainForm->text_canvas->TextOut(value + 116, text_y, subject);
            row++;
        }
    }
}

// Draws the friend or ignore list title with its count and the visible names; online friends are brighter.
void GameGraphics::DrawSocialText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->social->window_x;
    int window_y = MainForm->animation->social->window_y;
    String title = MainForm->animation->social->title;
    if (!MainForm->animation->social->list_kind)
        title = title + " [" + IntToStr(MainForm->animation->social->friend_names->Count) + "]";
    if (MainForm->animation->social->list_kind == 1)
        title = title + " [" + IntToStr(MainForm->animation->social->ignored_names->Count) + "]";
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, title);
    if (!MainForm->animation->social->list_kind)
    {
        int friend_top_line = MainForm->animation->social->GetScrollOffset();
        int friend_line_count = MainForm->animation->social->friend_names->Count;
        int friend_row = 0;
        for (int i = friend_top_line; i < friend_line_count && friend_top_line + 12 > i; i++)
        {
            if (MainForm->animation->social->IsOnline(MainForm->animation->social->friend_names->Strings[i]))
                MainForm->text_canvas->Font->Color = (TColor)0xF0F0F0;
            else
                MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
            String friend_name = MainForm->animation->social->friend_names->Strings[i];
            int friend_text_x = MainForm->animation->social->window_x + 22;
            int friend_text_y = MainForm->animation->social->window_y + LIST_ROW_HEIGHT * friend_row + 48;
            MainForm->text_canvas->TextOut(friend_text_x, friend_text_y, MainForm->net_client->Capitalize(friend_name));
            friend_row++;
        }
    }
    if (MainForm->animation->social->list_kind == 1)
    {
        int ignore_top_line = MainForm->animation->social->GetScrollOffset();
        int ignore_line_count = MainForm->animation->social->ignored_names->Count;
        int ignore_row = 0;
        for (int j = ignore_top_line; j < ignore_line_count && ignore_top_line + 12 > j; j++)
        {
            String ignore_name = MainForm->animation->social->ignored_names->Strings[j];
            int ignore_text_x = MainForm->animation->social->window_x + 22;
            int ignore_text_y = MainForm->animation->social->window_y + LIST_ROW_HEIGHT * ignore_row + 48;
            MainForm->text_canvas->TextOut(ignore_text_x, ignore_text_y, MainForm->net_client->Capitalize(ignore_name));
            ignore_row++;
        }
    }
}

// Draws the locker's item count and the amount of each visible item held more than once.
void GameGraphics::DrawLockerText()
{
    int amount;
    int index;
    int window_x;
    int window_y;
    int count;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xE1E1E1;
    window_x = MainForm->animation->locker->window_x;
    window_y = MainForm->animation->locker->window_y;
    String title =
        MainForm->animation->locker->title + " [" + IntToStr(MainForm->animation->locker->items.size()) + "]";
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, title);
    MainForm->text_canvas->Font->Color = (TColor)0x646482;
    index = 4 * MainForm->animation->locker->GetTopRow();
    count = MainForm->animation->locker->items.size();
    int row = 0;
    while (row < 5 && index < count)
    {
        for (int col = 0; col < 4 && index < count; col++)
        {
            int slot_x = MainForm->animation->locker->window_x + 57 * col + 61;
            int slot_y = MainForm->animation->locker->window_y + 37 * row + 48;
            amount = MainForm->animation->locker->items[index].amount;
            if (amount > 999)
                amount = 999;
            index++;
            if (amount > 1)
            {
                if (amount < 10)
                    MainForm->text_canvas->TextOut(slot_x + 2, slot_y + 23, IntToStr(amount));
                else
                    MainForm->text_canvas->TextOut(slot_x, slot_y + 23, IntToStr(amount));
            }
        }
        row++;
    }
}

// Draws the guild window title and the text of the current page: the four menu pages, or the visible list lines.
void GameGraphics::DrawGuildText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->guild->window_x;
    int window_y = MainForm->animation->guild->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->guild->title);
    if (MainForm->animation->guild->page == GuildPage_Main)
    {
        int text_y = MainForm->animation->guild->window_y + 54;
        int text_x = MainForm->animation->guild->window_x + 78;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_Information2));
        MainForm->text_canvas->TextOut(text_x, text_y + 14, MainForm->strings->GetText(GameText_LearnMoreAboutGuilds));
        MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_Administration));
        MainForm->text_canvas->TextOut(text_x, text_y + 51, MainForm->strings->GetText(GameText_JoinLeaveRegister));
        MainForm->text_canvas->TextOut(text_x, text_y + 74, MainForm->strings->GetText(GameText_Managment));
        MainForm->text_canvas->TextOut(text_x, text_y + 88, MainForm->strings->GetText(GameText_ModifyRankingDisband));
        MainForm->text_canvas->TextOut(text_x, text_y + 111, MainForm->strings->GetText(GameText_BankAccount));
        MainForm->text_canvas->TextOut(text_x, text_y + 125,
            MainForm->strings->GetText(GameText_DespositToGuildAccount));
    }
    if (MainForm->animation->guild->page == GuildPage_Membership)
    {
        int page1_text_y = MainForm->animation->guild->window_y + 54;
        int page1_text_x = MainForm->animation->guild->window_x + 78;
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y, MainForm->strings->GetText(GameText_JoinGuild));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 14,
            MainForm->strings->GetText(GameText_JoinExistingGuild));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 37,
            MainForm->strings->GetText(GameText_LeaveGuild));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 51,
            MainForm->strings->GetText(GameText_LeaveYourCurrentGuild));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 74,
            MainForm->strings->GetText(GameText_RegisterGuild));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 88,
            MainForm->strings->GetText(GameText_CreateYourOwnGuild));
    }
    if (MainForm->animation->guild->page == GuildPage_Manage)
    {
        int page2_text_y = MainForm->animation->guild->window_y + 54;
        int page2_text_x = MainForm->animation->guild->window_x + 78;
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y, MainForm->strings->GetText(GameText_ModifyGuild));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 14,
            MainForm->strings->GetText(GameText_ChangeYourGuildDetails));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 37, MainForm->strings->GetText(GameText_Ranking));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 51,
            MainForm->strings->GetText(GameText_ManageMemberRankings));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 74, MainForm->strings->GetText(GameText_Ranking));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 88,
            MainForm->strings->GetText(GameText_AssignRankToMember));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 111,
            MainForm->strings->GetText(GameText_RemoveMember));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 125,
            MainForm->strings->GetText(GameText_RemoveMemberFromGuild));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 148, MainForm->strings->GetText(GameText_Disband));
        MainForm->text_canvas->TextOut(page2_text_x, page2_text_y + 162,
            MainForm->strings->GetText(GameText_DisbandYourGuild));
    }
    if (MainForm->animation->guild->page == GuildPage_Lookup)
    {
        int page3_text_y = MainForm->animation->guild->window_y + 54;
        int page3_text_x = MainForm->animation->guild->window_x + 78;
        MainForm->text_canvas->TextOut(page3_text_x, page3_text_y, MainForm->strings->GetText(GameText_GuildLookup));
        MainForm->text_canvas->TextOut(page3_text_x, page3_text_y + 14,
            MainForm->strings->GetText(GameText_ViewGuildDetails));
        MainForm->text_canvas->TextOut(page3_text_x, page3_text_y + 37,
            MainForm->strings->GetText(GameText_Memberlist));
        MainForm->text_canvas->TextOut(page3_text_x, page3_text_y + 51,
            MainForm->strings->GetText(GameText_ViewGuildMembers));
    }
    if (MainForm->animation->guild->page >= GuildPage_Join)
    {
        int top_line = MainForm->animation->guild->GetTopLine();
        int line_count = MainForm->animation->guild->lines->Count;
        int row = 0;
        int i;
        for (i = top_line; i < line_count && top_line + 12 > i; i++)
        {
            String list_line = MainForm->animation->guild->lines->Strings[i];
            if (list_line.Length() > 0)
            {
                // An asterisk marks a link line: drawn underlined without it, brighter under the mouse.
                if (list_line[1] == '*')
                {
                    list_line.Delete(1, 1);
                    if (MainForm->animation->guild->hovered_row == row)
                        MainForm->text_canvas->Font->Color = (TColor)0xDCDCDC;
                    else
                        MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
                    MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
                }
                else
                    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            }
            int list_text_x = MainForm->animation->guild->window_x + 22;
            int list_text_y = MainForm->animation->guild->window_y + LIST_ROW_HEIGHT * row + 48;
            MainForm->text_canvas->TextOut(list_text_x, list_text_y, list_line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            // The member list page adds each member's rank title.
            if (MainForm->animation->guild->page == GuildPage_MemberList)
                MainForm->text_canvas->TextOut(list_text_x + 120, list_text_y,
                    MainForm->animation->guild->member_rank_titles->Strings[i]);
            row++;
        }
    }
}

// Draws the help report title and each visible line with its right-aligned value; link lines are underlined.
void GameGraphics::DrawReportText()
{
    int top_line;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->report->window_x;
    int window_y = MainForm->animation->report->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->report->title);
    top_line = MainForm->animation->report->GetTopLine();
    int line_count = MainForm->animation->report->lines->Count;
    int row = 0;
    for (int i = top_line; i < line_count && i < top_line + 12; i++)
    {
        String line = MainForm->animation->report->lines->Strings[i];
        if (line.Length() > 0)
        {
            if (line[1] == '*')
            {
                line.Delete(1, 1);
                if (MainForm->animation->guild->hovered_row == row)
                    MainForm->text_canvas->Font->Color = (TColor)0xDCDCDC;
                else
                    MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
                MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
            }
            else
                MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            int text_x = MainForm->animation->report->window_x + 22;
            int text_y = MainForm->animation->report->window_y + LIST_ROW_HEIGHT * row + 48;
            MainForm->text_canvas->TextOut(text_x, text_y, line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            if (MainForm->animation->report->line_values->Strings[i].Length() > 0)
            {
                int value_x = text_x + 220;
                value_x -= MainForm->text_canvas->TextWidth(MainForm->animation->report->line_values->Strings[i]);
                MainForm->text_canvas->TextOut(value_x, text_y, MainForm->animation->report->line_values->Strings[i]);
            }
        }
        row++;
    }
}

// Draws the quest dialog: the npc and quest name with the visible dialog lines and their right-aligned values, or
// the list of the npc's quests.
void GameGraphics::DrawDialogText()
{
    int window_x;
    int window_y;
    int top_line;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    window_x = MainForm->animation->dialog->window_x;
    window_y = MainForm->animation->dialog->window_y;
    if (!MainForm->animation->dialog->showing_quest_list)
    {
        String title = MainForm->animation->dialog->npc_name + " - " + MainForm->animation->dialog->quest_name;
        MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, title);
        top_line = MainForm->animation->dialog->GetTopLine();
        int line_count = MainForm->animation->dialog->lines->Count;
        int row = 0;
        for (int i = top_line; i < line_count && i < top_line + 6; i++)
        {
            String line = MainForm->animation->dialog->lines->Strings[i];
            if (line.Length() > 0)
            {
                if (line[1] == '*')
                {
                    line.Delete(1, 1);
                    MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
                    MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
                }
                else
                    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
                int text_x = MainForm->animation->dialog->window_x + 22;
                int text_y = MainForm->animation->dialog->window_y + LIST_ROW_HEIGHT * row + 48;
                MainForm->text_canvas->TextOut(text_x, text_y, line);
                MainForm->text_canvas->Font->Style = TFontStyles();
                if (MainForm->animation->dialog->line_right_texts->Strings[i].Length() > 0)
                {
                    int value_x = text_x + 220;
                    value_x -=
                        MainForm->text_canvas->TextWidth(MainForm->animation->dialog->line_right_texts->Strings[i]);
                    MainForm->text_canvas->TextOut(value_x, text_y,
                        MainForm->animation->dialog->line_right_texts->Strings[i]);
                }
            }
            row++;
        }
    }
    if (MainForm->animation->dialog->showing_quest_list)
    {
        MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->strings->GetText(GameText_SelectQuest));
        int other_top_line = MainForm->animation->dialog->GetTopLine();
        int other_line_count = MainForm->animation->dialog->quest_names->Count;
        int other_row = 0;
        for (int j = other_top_line; j < other_line_count && j < other_top_line + 6; j++)
        {
            String other_line = MainForm->animation->dialog->quest_names->Strings[j];
            if (other_line.Length() > 0)
            {
                MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
                int other_text_x = MainForm->animation->dialog->window_x + 22;
                int other_text_y = MainForm->animation->dialog->window_y + LIST_ROW_HEIGHT * other_row + 48;
                MainForm->text_canvas->TextOut(other_text_x, other_text_y, other_line);
                MainForm->text_canvas->Font->Style = TFontStyles();
            }
            other_row++;
        }
    }
}

// Draws the memo title and each visible line with its right-aligned value.
void GameGraphics::DrawMemoText()
{
    int top_line;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->memo->window_x;
    int window_y = MainForm->animation->memo->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->memo->title);
    top_line = MainForm->animation->memo->GetTopLine();
    int line_count = MainForm->animation->memo->lines->Count;
    int row = 0;
    for (int i = top_line; i < line_count && i < top_line + 12; i++)
    {
        String line = MainForm->animation->memo->lines->Strings[i];
        if (line.Length() > 0)
        {
            MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            int text_x = MainForm->animation->memo->window_x + 22;
            int text_y = MainForm->animation->memo->window_y + LIST_ROW_HEIGHT * row + 48;
            MainForm->text_canvas->TextOut(text_x, text_y, line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            if (MainForm->animation->memo->line_values->Strings[i].Length() > 0)
            {
                int value_x = text_x + 220;
                value_x -= MainForm->text_canvas->TextWidth(MainForm->animation->memo->line_values->Strings[i]);
                MainForm->text_canvas->TextOut(value_x, text_y, MainForm->animation->memo->line_values->Strings[i]);
            }
        }
        row++;
    }
}

// Draws the inn's npc name and the text of its two menu pages, or the visible list lines with their links.
void GameGraphics::DrawInnText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->inn->window_x;
    int window_y = MainForm->animation->inn->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->inn->npc_name);
    if (!MainForm->animation->inn->page)
    {
        int text_y = MainForm->animation->inn->window_y + 54;
        int text_x = MainForm->animation->inn->window_x + 78;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_RegistrationService2));
        MainForm->text_canvas->TextOut(text_x, text_y + 14,
            MainForm->strings->GetText(GameText_CitizenRegistrationService));
        MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_Sleep));
        MainForm->text_canvas->TextOut(text_x, text_y + 51, MainForm->strings->GetText(GameText_FullHpRecovery));
    }
    if (MainForm->animation->inn->page == 1)
    {
        int page1_text_y = MainForm->animation->inn->window_y + 54;
        int page1_text_x = MainForm->animation->inn->window_x + 78;
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y, MainForm->strings->GetText(GameText_SignUp));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 14,
            MainForm->strings->GetText(GameText_BecomeCitizen));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 37,
            MainForm->strings->GetText(GameText_Unsubscribe));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 51,
            MainForm->strings->GetText(GameText_GiveUpCitizenship));
    }
    if (MainForm->animation->inn->page >= 6)
    {
        int top_line = MainForm->animation->inn->GetTopLine();
        int line_count = MainForm->animation->inn->lines->Count;
        int row = 0;
        int i;
        for (i = top_line; i < line_count && i < top_line + 12; i++)
        {
            String line = MainForm->animation->inn->lines->Strings[i];
            if (line.Length() > 0)
            {
                if (line[1] == '*')
                {
                    line.Delete(1, 1);
                    if (MainForm->animation->inn->hovered_row == row)
                        MainForm->text_canvas->Font->Color = (TColor)0xDCDCDC;
                    else
                        MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
                    MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
                }
                else
                    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            }
            int list_text_x = MainForm->animation->inn->window_x + 22;
            int list_text_y = MainForm->animation->inn->window_y + LIST_ROW_HEIGHT * row + 48;
            MainForm->text_canvas->TextOut(list_text_x, list_text_y, line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            row++;
        }
    }
}

// Draws the law npc's name and the text of its two menu pages, or the visible list lines with their links.
void GameGraphics::DrawLawText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->law->window_x;
    int window_y = MainForm->animation->law->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->law->npc_name);
    if (!MainForm->animation->law->page)
    {
        int text_y = MainForm->animation->law->window_y + 54;
        int text_x = MainForm->animation->law->window_x + 78;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_RegistrationService));
        MainForm->text_canvas->TextOut(text_x, text_y + 14,
            MainForm->strings->GetText(GameText_RequestMarriageOrDivorce));
    }
    if (MainForm->animation->law->page == 1)
    {
        int page1_text_y = MainForm->animation->law->window_y + 54;
        int page1_text_x = MainForm->animation->law->window_x + 78;
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y, MainForm->strings->GetText(GameText_Marriage));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 14,
            MainForm->strings->GetText(GameText_RequestWeddingApproval));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 37, MainForm->strings->GetText(GameText_Divorce));
        MainForm->text_canvas->TextOut(page1_text_x, page1_text_y + 51,
            MainForm->strings->GetText(GameText_BreakUpWithPartner));
    }
    if (MainForm->animation->law->page >= 6)
    {
        int top_line = MainForm->animation->law->GetTopLine();
        int line_count = MainForm->animation->law->lines->Count;
        int row = 0;
        int i;
        for (i = top_line; i < line_count && i < top_line + 12; i++)
        {
            String line = MainForm->animation->law->lines->Strings[i];
            if (line.Length() > 0)
            {
                if (line[1] == '*')
                {
                    line.Delete(1, 1);
                    if (MainForm->animation->law->hovered_row == row)
                        MainForm->text_canvas->Font->Color = (TColor)0xDCDCDC;
                    else
                        MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
                    MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
                }
                else
                    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            }
            int list_text_x = MainForm->animation->law->window_x + 22;
            int list_text_y = MainForm->animation->law->window_y + LIST_ROW_HEIGHT * row + 48;
            MainForm->text_canvas->TextOut(list_text_x, list_text_y, line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            row++;
        }
    }
}

// Draws the skill master's text for its page: the menu with the known skill and spell counts, the offered skills, or
// the visible lines of a text page.
void GameGraphics::DrawLearnText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->learn->window_x;
    int window_y = MainForm->animation->learn->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->learn->npc_name);
    if (MainForm->animation->learn->tooltip->IsVisible())
        MainForm->text_canvas->Font->Color = (TColor)0x46484B;
    if (MainForm->animation->learn->page == LearnPage_Menu)
    {
        int text_y = MainForm->animation->learn->window_y + 54;
        int text_x = MainForm->animation->learn->window_x + 78;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_Learn));
        MainForm->text_canvas->TextOut(text_x, text_y + 14,
            IntToStr(MainForm->animation->learn->offered_skills.size()) +
                MainForm->strings->GetText(GameText_ItemsToLearn));
        unsigned int known_count = MainForm->animation->interface_controller->skills->skills.size();
        // Known skills and spells are counted together.
        known_count += MainForm->animation->interface_controller->spells->spells.size();
        MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_Forget));
        MainForm->text_canvas->TextOut(text_x, text_y + 51,
            IntToStr((int)known_count) + MainForm->strings->GetText(GameText_ItemsLearned));
        MainForm->text_canvas->TextOut(text_x, text_y + 74, MainForm->strings->GetText(GameText_ForgetAll));
        MainForm->text_canvas->TextOut(text_x, text_y + 88, MainForm->strings->GetText(GameText_ResetYourCharacter));
    }
    if (MainForm->animation->learn->page == LearnPage_SkillList)
    {
        int top_line = MainForm->animation->learn->GetTopLine();
        int skill_count = MainForm->animation->learn->offered_skills.size();
        int row = 0;
        for (int i = top_line; i < skill_count && i < top_line + 5; i++)
        {
            int skill_id = MainForm->animation->learn->offered_skills[i].skill_id;
            String skill_name = MainForm->skill_values->GetName(skill_id);
            int name_x = MainForm->animation->learn->window_x + 78;
            int name_y = MainForm->animation->learn->window_y + ITEM_NAME_ROW_HEIGHT * row + 54;
            MainForm->text_canvas->TextOut(name_x, name_y, MainForm->net_client->Capitalize(skill_name));
            name_y = MainForm->animation->learn->window_y + ITEM_AMOUNT_ROW_HEIGHT * row + 68;
            MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
            MainForm->text_canvas->TextOut(name_x, name_y, MainForm->strings->GetText(GameText_Requirements));
            MainForm->text_canvas->Font->Style = TFontStyles();
            row++;
        }
    }
    if (MainForm->animation->learn->page >= LearnPage_SkillInfo)
    {
        int list_top_line = MainForm->animation->learn->GetTopLine();
        int list_line_count = MainForm->animation->learn->lines->Count;
        int list_row = 0;
        for (int j = list_top_line; j < list_line_count && list_top_line + 12 > j; j++)
        {
            String list_line = MainForm->animation->learn->lines->Strings[j];
            if (list_line.Length() > 0)
            {
                if (list_line[1] == '*')
                {
                    list_line.Delete(1, 1);
                    if (MainForm->animation->learn->hovered_row == list_row)
                        MainForm->text_canvas->Font->Color = (TColor)0xDCDCDC;
                    else
                        MainForm->text_canvas->Font->Color = (TColor)0xB4B4B4;
                    MainForm->text_canvas->Font->Style = TFontStyles() << fsUnderline;
                }
                else
                    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            }
            int list_text_x = MainForm->animation->learn->window_x + 22;
            int list_text_y = MainForm->animation->learn->window_y + LIST_ROW_HEIGHT * list_row + 48;
            MainForm->text_canvas->TextOut(list_text_x, list_text_y, list_line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            if (MainForm->animation->learn->page == LearnPage_Unknown15)
                MainForm->text_canvas->TextOut(list_text_x + 120, list_text_y,
                    MainForm->animation->learn->line_right_texts->Strings[j]);
            list_row++;
        }
    }
}

// Draws the shop npc's name, on the first page the buy, sell and craft counts, and each visible item's name and
// price.
void GameGraphics::DrawShopText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->shop->window_x;
    int window_y = MainForm->animation->shop->window_y;
    MainForm->text_canvas->TextOut(window_x + 22, window_y + 17, MainForm->animation->shop->npc_name);
    if (MainForm->animation->shop->page == ShopPage_Menu)
    {
        int text_y = MainForm->animation->shop->window_y + 54;
        int text_x = MainForm->animation->shop->window_x + 78;
        MainForm->text_canvas->TextOut(text_x, text_y, MainForm->strings->GetText(GameText_BuyItemS));
        MainForm->text_canvas->TextOut(text_x, text_y + 14, IntToStr(MainForm->animation->shop->buy_items.size()) +
            " " + MainForm->strings->GetText(GameText_ItemsInStore));
        MainForm->text_canvas->TextOut(text_x, text_y + 37, MainForm->strings->GetText(GameText_SellItemsS));
        MainForm->text_canvas->TextOut(text_x, text_y + 51, IntToStr(MainForm->animation->shop->CountSellableItems()) +
            " " + MainForm->strings->GetText(GameText_ItemsAccepted));
        if (MainForm->animation->shop->craft_items.size() > 0)
        {
            MainForm->text_canvas->TextOut(text_x, text_y + 74, MainForm->strings->GetText(GameText_CraftItemS));
            MainForm->text_canvas->TextOut(text_x, text_y + 88,
                IntToStr(MainForm->animation->shop->craft_items.size()) + " " +
                    MainForm->strings->GetText(GameText_ItemsAccepted));
        }
    }
    if (MainForm->animation->shop->tooltip->IsVisible())
        MainForm->text_canvas->Font->Color = (TColor)0x46484B;
    int top_row = MainForm->animation->shop->GetTopRow();
    int item_count = MainForm->animation->shop->shown_items.size();
    int row = 0;
    for (int i = top_row; i < item_count && i < top_row + 5; i++)
    {
        int item_id = MainForm->animation->shop->shown_items[i].item_id;
        ShopItem &shop_item = MainForm->animation->shop->shown_items[i];
        int price = MainForm->animation->shop->shown_items[i].price;
        String item_name = MainForm->item_values->GetName(item_id);
        int item_type = MainForm->item_values->GetType(item_id);
        int item_text_x = MainForm->animation->shop->window_x + 86;
        int item_text_y = MainForm->animation->shop->window_y + ITEM_NAME_ROW_HEIGHT * row + 54;
        MainForm->text_canvas->TextOut(item_text_x, item_text_y, MainForm->net_client->Capitalize(item_name));
        item_text_y = MainForm->animation->shop->window_y + ITEM_AMOUNT_ROW_HEIGHT * row + 68;
        // The craft page labels the price with its own word.
        if (MainForm->animation->shop->page == ShopPage_Craft)
        {
            if (item_type == ITEM_TYPE_GENDERED)
            {
                if (!MainForm->item_values->GetGender(item_id))
                    MainForm->text_canvas->TextOut(item_text_x, item_text_y,
                        MainForm->strings->GetText(GameText_Ingredients) + ": " + IntToStr(price) + "  (" +
                            MainForm->strings->GetText(GameText_Female) + ")");
                else
                    MainForm->text_canvas->TextOut(item_text_x, item_text_y,
                        MainForm->strings->GetText(GameText_Ingredients) + ": " + IntToStr(price) + "  (" +
                            MainForm->strings->GetText(GameText_Male) + ")");
            }
            else
                MainForm->text_canvas->TextOut(item_text_x, item_text_y,
                    MainForm->strings->GetText(GameText_Ingredients) + ": " + IntToStr(price));
        }
        else if (item_type == ITEM_TYPE_GENDERED && MainForm->animation->shop->page < ShopPage_Sell)
        {
            if (!MainForm->item_values->GetGender(item_id))
                MainForm->text_canvas->TextOut(item_text_x, item_text_y, MainForm->strings->GetText(GameText_Price) +
                    ": " + IntToStr(price) + "  (" + MainForm->strings->GetText(GameText_Female) + ")");
            else
                MainForm->text_canvas->TextOut(item_text_x, item_text_y, MainForm->strings->GetText(GameText_Price) +
                    ": " + IntToStr(price) + "  (" + MainForm->strings->GetText(GameText_Male) + ")");
        }
        else
            MainForm->text_canvas->TextOut(item_text_x, item_text_y,
                MainForm->strings->GetText(GameText_Price) + ": " + IntToStr(price));
        row++;
    }
}

// Draws both trade sides' names with their item counts and status texts, and each visible item's name and amount.
void GameGraphics::DrawTradeText()
{
    int partner_row;
    int window_x;
    int window_y;
    int partner_count;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    window_x = MainForm->animation->trade->window_x;
    window_y = MainForm->animation->trade->window_y;
    partner_count = MainForm->animation->trade->partner_items.size();
    int own_count = MainForm->animation->trade->own_items.size();
    if (partner_count <= 0)
        MainForm->text_canvas->TextOut(window_x + 20, window_y + 17,
            MainForm->net_client->Capitalize(MainForm->animation->trade->partner_name));
    else
        MainForm->text_canvas->TextOut(window_x + 20, window_y + 17,
            MainForm->net_client->Capitalize(MainForm->animation->trade->partner_name) + " [" +
                IntToStr(partner_count) + "]");
    MainForm->text_canvas->TextOut(window_x + 197, window_y + 17,
        MainForm->net_client->Capitalize(MainForm->animation->trade->partner_status_text));
    if (own_count <= 0)
        MainForm->text_canvas->TextOut(window_x + 287, window_y + 17,
            MainForm->net_client->Capitalize(MainForm->animation->trade->own_name));
    else
        MainForm->text_canvas->TextOut(window_x + 287, window_y + 17,
            MainForm->net_client->Capitalize(MainForm->animation->trade->own_name) + " [" + IntToStr(own_count) + "]");
    MainForm->text_canvas->TextOut(window_x + 464, window_y + 17,
        MainForm->net_client->Capitalize(MainForm->animation->trade->own_status_text));
    int top_row = MainForm->animation->trade->GetPartnerTopRow();
    int item_count = MainForm->animation->trade->partner_items.size();
    partner_row = 0;
    if (MainForm->animation->trade->tooltip->IsVisible())
        MainForm->text_canvas->Font->Color = (TColor)0x46484B;
    for (int i = top_row; i < item_count && top_row + 5 > i; i++)
    {
        int partner_item_id = MainForm->animation->trade->partner_items[i].item_id;
        int partner_amount = MainForm->animation->trade->partner_items[i].amount;
        String partner_item_name = MainForm->item_values->GetName(partner_item_id);
        int partner_item_type = MainForm->item_values->GetType(partner_item_id);
        int partner_text_x = MainForm->animation->trade->window_x + 86;
        int partner_text_y = MainForm->animation->trade->window_y + ITEM_AMOUNT_ROW_HEIGHT * partner_row + 53;
        MainForm->text_canvas->TextOut(partner_text_x, partner_text_y,
            MainForm->net_client->Capitalize(partner_item_name));
        partner_text_y = MainForm->animation->trade->window_y + ITEM_AMOUNT_ROW_HEIGHT * partner_row + 68;
        // Items of type 12 show the gender they are for beside the amount.
        if (partner_item_type == EifItemType_Armor)
        {
            if (!MainForm->item_values->GetGender(partner_item_id))
                MainForm->text_canvas->TextOut(partner_text_x, partner_text_y,
                    "x " + IntToStr(partner_amount) + "  (" + MainForm->strings->GetText(GameText_Female) + ")");
            else
                MainForm->text_canvas->TextOut(partner_text_x, partner_text_y,
                    "x " + IntToStr(partner_amount) + "  (" + MainForm->strings->GetText(GameText_Male) + ")");
        }
        else
            MainForm->text_canvas->TextOut(partner_text_x, partner_text_y, "x " + IntToStr(partner_amount));
        partner_row++;
    }
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    top_row = MainForm->animation->trade->GetOwnTopRow();
    item_count = MainForm->animation->trade->own_items.size();
    partner_row = 0;
    int j;
    for (j = top_row; j < item_count && top_row + 5 > j; j++)
    {
        int own_item_id = MainForm->animation->trade->own_items[j].item_id;
        int own_amount = MainForm->animation->trade->own_items[j].amount;
        String own_item_name = MainForm->item_values->GetName(own_item_id);
        int own_item_type = MainForm->item_values->GetType(own_item_id);
        int own_text_x = MainForm->animation->trade->window_x + 352;
        int own_text_y = MainForm->animation->trade->window_y + ITEM_AMOUNT_ROW_HEIGHT * partner_row + 53;
        MainForm->text_canvas->TextOut(own_text_x, own_text_y, MainForm->net_client->Capitalize(own_item_name));
        own_text_y = MainForm->animation->trade->window_y + ITEM_AMOUNT_ROW_HEIGHT * partner_row + 68;
        if (own_item_type == EifItemType_Armor)
        {
            if (!MainForm->item_values->GetGender(own_item_id))
                MainForm->text_canvas->TextOut(own_text_x, own_text_y,
                    "x " + IntToStr(own_amount) + "  (" + MainForm->strings->GetText(GameText_Female) + ")");
            else
                MainForm->text_canvas->TextOut(own_text_x, own_text_y,
                    "x " + IntToStr(own_amount) + "  (" + MainForm->strings->GetText(GameText_Male) + ")");
        }
        else
            MainForm->text_canvas->TextOut(own_text_x, own_text_y, "x " + IntToStr(own_amount));
        partner_row++;
    }
}

// Draws the book's info lines and its visible non-empty text lines.
void GameGraphics::DrawBookText()
{
    int top_line;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    int window_x = MainForm->animation->book->window_x;
    int window_y = MainForm->animation->book->window_y;
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 23, MainForm->animation->book->player_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 53, MainForm->animation->book->home_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 83, MainForm->animation->book->class_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 113, MainForm->animation->book->partner_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 143, MainForm->animation->book->title);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 203, MainForm->animation->book->guild_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 233, MainForm->animation->book->guild_rank_name);
    top_line = MainForm->animation->book->GetTopLine();
    int line_count = MainForm->animation->book->lines->Count;
    int row = 0;
    for (int i = top_line; i < line_count && i < top_line + 14; i++)
    {
        String line = MainForm->animation->book->lines->Strings[i];
        if (line.Length() > 0)
        {
            MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
            int text_x = MainForm->animation->book->window_x + 49;
            int text_y = MainForm->animation->book->window_y + BOOK_ROW_HEIGHT * row + 41;
            MainForm->text_canvas->TextOut(text_x, text_y, line);
            MainForm->text_canvas->Font->Style = TFontStyles();
            row++;
        }
    }
}

// Draws the paperdoll's character details, dimmed while a slot is hovered.
void GameGraphics::DrawPaperdollText()
{
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = clBlack;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 7;
    MainForm->text_canvas->Font->Color = (TColor)0xC8C8C8;
    if (MainForm->animation->paperdoll->HasHoveredSlot())
        MainForm->text_canvas->Font->Color = (TColor)0x5A5A5A;
    int window_x = MainForm->animation->paperdoll->window_x;
    int window_y = MainForm->animation->paperdoll->window_y;
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 23, MainForm->animation->paperdoll->player_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 53, MainForm->animation->paperdoll->home_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 83, MainForm->animation->paperdoll->class_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 113, MainForm->animation->paperdoll->partner_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 143, MainForm->animation->paperdoll->title);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 203, MainForm->animation->paperdoll->guild_name);
    MainForm->text_canvas->TextOut(window_x + 231, window_y + 233, MainForm->animation->paperdoll->guild_rank_name);
}

// Draws the paperdoll background bitmap onto its surface.
void GameGraphics::LoadPaperdollBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (paperdoll_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        paperdoll_surface->ReleaseDC(dc);
    }
}

// Draws the book background bitmap onto its surface.
void GameGraphics::LoadBookBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (book_surface->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        book_surface->ReleaseDC(dc);
    }
}
