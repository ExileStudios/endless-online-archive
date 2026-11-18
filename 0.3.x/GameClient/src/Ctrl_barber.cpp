#include <vcl.h>
#pragma hdrstop

#include "Ctrl_barber.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Ctrl_status.h"
#include "Gfx_barber.h"

#pragma package(smart_init)

BarberController::BarberController()
{
    window_x = 107;
    window_y = 53;
    base_price = 0;
    price_per_level = 0;
    hovered_column = -1;
    is_visible = false;
    npc_session_id = -1;
}

BarberController::~BarberController()
{
}

// Opens the barber window with a preview copy of the player and the first page of hair styles; the price is the base
// cost plus the cost per level times the character's level.
void BarberController::Open(int session_id, int styles, int base_cost, int level_cost)
{
    short count = styles;
    style_count = count;
    if (count < 20)
        style_count = 20;
    hair_style = MainForm->players->GetHairStyle(MainForm->net_client->GetPlayerId());
    hair_color = MainForm->players->GetHairColor(MainForm->net_client->GetPlayerId());
    preview_direction = FacingDirection_Down;
    MainForm->players->AddPreviewPlayer(0, 0);
    MainForm->players->SetGender(PlayerId_Preview, MainForm->players->GetGender(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetSkin(PlayerId_Preview, MainForm->players->GetSkin(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetHairStyle(PlayerId_Preview,
        MainForm->players->GetHairStyle(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetHairColor(PlayerId_Preview,
        MainForm->players->GetHairColor(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetArmor(PlayerId_Preview, MainForm->players->GetArmor(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetWeapon(PlayerId_Preview, MainForm->players->GetWeapon(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetShield(PlayerId_Preview, MainForm->players->GetShield(MainForm->net_client->GetPlayerId()));
    MainForm->players->SetBoots(PlayerId_Preview, MainForm->players->GetBoots(MainForm->net_client->GetPlayerId()));
    MainForm->players->ReloadGraphics(PlayerId_Preview);
    MainForm->gfx->CreateBarberGraphics();
    MainForm->gfx->LoadBarberGraphics();
    base_price = base_cost;
    price_per_level = level_cost;
    if (base_price <= 0)
        base_price = 0;
    if (price_per_level <= 0)
        price_per_level = 0;
    price = base_price;
    if (price_per_level > 0)
        price += MainForm->animation->interface_controller->status->level * price_per_level;
    style_page = 0;
    MainForm->gfx->barber->RenderHairStyles(MainForm->players->GetGender(MainForm->net_client->GetPlayerId()),
        MainForm->players->GetSkin(MainForm->net_client->GetPlayerId()), hair_color, style_page);
    npc_session_id = session_id;
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Closes the window and removes the preview player.
void BarberController::Close()
{
    is_visible = false;
    npc_session_id = -1;
    MainForm->gfx->FreeBarberGraphics();
    MainForm->players->RemovePreviewPlayer();
    MainForm->redraw_needed = true;
}

// A click in the barber window: picks a hair colour or style for the preview, turns the preview, pages the styles,
// or asks to buy the look when the gold covers its price. Otherwise the close button and moving the window.
void BarberController::MouseDown(int x, int y, int button)
{
    int color;
    int column;
    char handled;
    int row;
    handled = false;
    // A click in the colour row picks that hair colour for the preview.
    if (window_y + 121 < y && window_y + 144 > y)
    {
        for (color = 0; color < 10; color++)
        {
            if (x > 22 * color + window_x + 46 && x < 22 * color + window_x + 65)
            {
                hair_color = color;
                MainForm->sfx->PlaySfx(SfxSound_Select);
                MainForm->players->SetPreviewHairColor(hair_color);
                MainForm->players->ReloadGraphics(PlayerId_Preview);
                MainForm->gfx->barber->RenderHairStyles(MainForm->players->GetGender(MainForm->net_client->GetPlayerId()),
                    MainForm->players->GetSkin(MainForm->net_client->GetPlayerId()), hair_color, style_page);
                MainForm->redraw_needed = true;
                handled = true;
            }
        }
    }
    if (x >= window_x + 12 && x <= window_x + 28 && window_y + 20 <= y && window_y + 127 >= y)
    {
        int page = style_page - 1;
        if (page <= 0)
            page = 0;
        if (page != style_page)
        {
            MainForm->sfx->PlaySfx(SfxSound_Select);
            style_page = page;
            MainForm->gfx->barber->RenderHairStyles(MainForm->players->GetGender(MainForm->net_client->GetPlayerId()),
                MainForm->players->GetSkin(MainForm->net_client->GetPlayerId()), hair_color, style_page);
            MainForm->redraw_needed = true;
        }
        handled = true;
    }
    if (x >= window_x + 278 && x <= window_x + 298 && window_y + 20 <= y && window_y + 127 >= y)
    {
        int new_page = style_page + 1;
        if (new_page > style_count / 16)
            new_page = style_count / 16;
        if (new_page != style_page)
        {
            MainForm->sfx->PlaySfx(SfxSound_Select);
            style_page = new_page;
            MainForm->gfx->barber->RenderHairStyles(MainForm->players->GetGender(MainForm->net_client->GetPlayerId()),
                MainForm->players->GetSkin(MainForm->net_client->GetPlayerId()), hair_color, style_page);
            MainForm->redraw_needed = true;
        }
        handled = true;
    }
    if (x >= window_x + 390 && x <= window_x + 409 && window_y + 27 <= y && window_y + 47 >= y)
    {
        MainForm->sfx->PlaySfx(SfxSound_ArrowButton);
        if (hair_style)
        {
            hair_style = 0;
            MainForm->players->SetPreviewHairStyle(hair_style);
            MainForm->players->ReloadGraphics(PlayerId_Preview);
            MainForm->redraw_needed = true;
        }
        handled = true;
    }
    else if (window_y + 14 < y && window_y + 137 > y && x > window_x + 308 && x < window_x + 412)
    {
        MainForm->sfx->PlaySfx(SfxSound_Select);
        preview_direction++;
        if (preview_direction > FacingDirection_Right)
            preview_direction = FacingDirection_Down;
        MainForm->players->SetDirection(PlayerId_Preview, preview_direction);
        MainForm->players->ReloadGraphics(PlayerId_Preview);
        MainForm->redraw_needed = true;
        handled = true;
    }
    // The two rows of styles: a click previews that style.
    for (row = 0; row < 2; row++)
    {
        for (column = 0; column < 8; column++)
        {
            if (x >= 31 * column + window_x + 30 && x <= 31 * column + window_x + 60 && 36 * row + window_y + 32 <= y &&
                36 * row + window_y + 64 >= y)
            {
                hair_style = 8 * row + column + 16 * style_page + 1;
                if (hair_style > style_count)
                    break;
                MainForm->sfx->PlaySfx(SfxSound_Select);
                MainForm->players->SetPreviewHairStyle(hair_style);
                MainForm->players->ReloadGraphics(PlayerId_Preview);
                MainForm->redraw_needed = true;
                handled = true;
            }
        }
    }
    if (x >= MainForm->animation->barber->window_x + 215 && x <= MainForm->animation->barber->window_x + 304 &&
        MainForm->animation->barber->window_y + 151 <= y && MainForm->animation->barber->window_y + 178 >= y)
    {
        if (MainForm->animation->interface_controller->status->gold >= price)
        {
            String confirm_text = MainForm->strings->GetText(GameText_DoYouWantTo);
            confirm_text = confirm_text + " ";
            confirm_text = confirm_text + IntToStr(price);
            confirm_text = confirm_text + " ";
            confirm_text = confirm_text + LowerCase(MainForm->item_values->GetName(ItemId_Gold));
            confirm_text = confirm_text + "?";
            MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_BuyHairstyle,
                MainForm->strings->GetText(GameText_BuyHairstyle), confirm_text);
            MainForm->edits->caret_enabled = false;
        }
        else
        {
            String no_gold_text =
                MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                    LowerCase(MainForm->item_values->GetName(ItemId_Gold));
            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_WarningEnough, false), no_gold_text);
            MainForm->edits->caret_enabled = false;
        }
    }
    if (x >= MainForm->animation->barber->window_x + 308 && x <= MainForm->animation->barber->window_x + 398 &&
        MainForm->animation->barber->window_y + 151 <= y && MainForm->animation->barber->window_y + 178 >= y)
    {
        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
        MainForm->animation->barber->Close();
        MainForm->redraw_needed = true;
    }
    if (!handled && x >= window_x && x <= window_x + 425 && y >= window_y && window_y + 270 >= y)
    {
        is_dragging = true;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered style cell and the reset, page, accept and close
// buttons.
void BarberController::MouseMove(int x, int y)
{
    int column;
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 204)
            window_x = 204;
        if (window_y >= 116)
            window_y = 116;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 390 && x <= window_x + 409 && window_y + 27 <= y && window_y + 47 >= y)
    {
        if (!reset_hovered)
        {
            reset_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (reset_hovered)
    {
        reset_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 12 && x <= window_x + 28 && window_y + 20 <= y && window_y + 127 >= y)
    {
        if (!previous_hovered)
        {
            previous_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (previous_hovered)
    {
        previous_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 278 && x <= window_x + 298 && window_y + 20 <= y && window_y + 127 >= y)
    {
        if (!next_hovered)
        {
            next_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (next_hovered)
    {
        next_hovered = false;
        MainForm->redraw_needed = true;
    }
    char over_cell = false;
    // The two rows of eight styles: the one under the mouse is marked.
    for (int row = 0; row < 2; row++)
    {
        for (column = 0; column < 8; column++)
        {
            if (column + 8 * row + 16 * style_page + 1 > style_count)
                break;
            if (x >= 31 * column + window_x + 30 && x <= 31 * column + window_x + 60 && 36 * row + window_y + 32 <= y &&
                36 * row + window_y + 64 >= y)
            {
                if (column != hovered_column)
                    MainForm->redraw_needed = true;
                if (row != hovered_row)
                    MainForm->redraw_needed = true;
                hovered_column = column;
                hovered_row = row;
                over_cell = true;
                break;
            }
        }
    }
    if (!over_cell && (hovered_column >= 0 || hovered_row >= 0))
    {
        hovered_column = -1;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 215 && x <= window_x + 304 && y >= window_y + 151 && y <= window_y + 178)
    {
        if (!accept_hovered)
        {
            accept_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (accept_hovered)
    {
        accept_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 308 && x <= window_x + 401 && window_y + 147 <= y && y <= window_y + 178)
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
}

void BarberController::MouseUp()
{
    is_dragging = false;
}
