#include <vcl.h>
#pragma hdrstop

#include "Ctrl_barber.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Ctrl_status.h"

#pragma package(smart_init)

BarberController::BarberController()
{
    window_x = 165;
    window_y = 53;
    is_visible = false;
    npc_session_id = -1;
}

BarberController::~BarberController()
{
}

// Opens the barber window with a preview copy of the player and the first page of hair styles; the price is the base
// cost plus the cost per level times the character's level.
void BarberController::Open(int session_id, int base_cost, int level_cost)
{
    Sleep(1);
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
        price_per_level = 20;
    price += base_price;
    price += MainForm->animation->interface_controller->status->level * price_per_level;
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

// A click on the preview turns it; the three rows, 37 pixels apart, are:
// row 0 = the next hair style, 0 to 20
// row 1 = the next hair color, 0 to 9
// row 2 = buy, after a yes in the message box, or a message when the gold is short
void BarberController::MouseDown(int x, int y, int button)
{
    bool handled = false;
    if (button == 0 || button == 1)
    {
        if (y > window_y + 14 && y < window_y + 137 && window_x + 209 < x && window_x + 308 > x)
        {
            MainForm->sfx->PlaySfx(SfxSound_Select);
            preview_direction++;
            if (preview_direction > FacingDirection_Right)
                preview_direction = FacingDirection_Down;
            MainForm->players->SetDirection(PlayerId_Preview, preview_direction);
            MainForm->players->ReloadGraphics(PlayerId_Preview);
            MainForm->redraw_needed = true;
        }
        if (y > window_y + 20 && y < window_y + 134 && window_x + 17 < x && window_x + 190 > x)
        {
            int row = (y - window_y - 23) / NPC_LIST_ROW_HEIGHT;
            if (!row)
            {
                MainForm->sfx->PlaySfx(SfxSound_Select);
                hair_style++;
                if (hair_style > 50)
                    hair_style = 0;
                MainForm->players->SetPreviewHairStyle(hair_style);
                MainForm->players->ReloadGraphics(PlayerId_Preview);
                MainForm->redraw_needed = true;
            }
            if (row == 1)
            {
                MainForm->sfx->PlaySfx(SfxSound_Select);
                hair_color++;
                if (hair_color > 9)
                    hair_color = 0;
                MainForm->players->SetPreviewHairColor(hair_color);
                MainForm->players->ReloadGraphics(PlayerId_Preview);
                MainForm->redraw_needed = true;
            }
            if (row == 2)
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
                    String no_gold_text = MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                        LowerCase(MainForm->item_values->GetName(ItemId_Gold));
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_WarningEnough, false), no_gold_text);
                    MainForm->edits->caret_enabled = false;
                }
            }
        }
    }
    if (!handled)
    {
        if (window_x <= x && window_x + 270 >= x && y >= window_y && y <= window_y + 270)
        {
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves the window while it is dragged and lights the row and the close button under the mouse.
void BarberController::MouseMove(int x, int y)
{
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 304)
            window_x = 304;
        if (window_y >= 116)
            window_y = 116;
        MainForm->redraw_needed = true;
    }
    if (y > window_y + 20 && y < window_y + 134 && x > window_x + 17 && x < window_x + 190 && !is_dragging)
    {
        int row = (y - window_y - 23) / NPC_LIST_ROW_HEIGHT;
        if (row > 2)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else if (hovered_row >= 0)
    {
        hovered_row = -1;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 215 && x <= window_x + 304 && y >= window_y + 151 && y <= window_y + 178)
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
