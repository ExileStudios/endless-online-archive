#include <vcl.h>
#pragma hdrstop

#include "Ctrl_bank.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#pragma package(smart_init)

BankController::BankController()
{
    window_x = 185;
    window_y = 35;
    is_visible = 0;
    npc_session_id = -1;
}

BankController::~BankController()
{
}

// Shows the bank window with the bank's gold and the locker upgrade level from the server.
void BankController::Open(int session_id, int gold_in_bank, int upgrade_level, String unused_text)
{
    MainForm->gfx->CreateBankGraphics();
    MainForm->gfx->LoadBankGraphics();
    npc_session_id = session_id;
    reserved_text = unused_text;
    bank_gold = gold_in_bank;
    locker_upgrade_level = upgrade_level;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void BankController::Close()
{
    is_visible = 0;
    npc_session_id = -1;
    MainForm->gfx->FreeBankGraphics();
    MainForm->redraw_needed = true;
}

// A click on the bank menu: deposit gold, withdraw gold, or upgrade the locker. A single coin moves at once;
// more opens the amount box. Otherwise starts moving the window.
void BankController::MouseDown(int x, int y, int button)
{
    char handled;
    int row;
    int upgrade_price;
    handled = 0;
    if (button == 0 || button == 1)
    {
        if (window_y + 50 < y && window_y + 170 > y && window_x + 17 < x)
        {
            if (window_x + 252 > x)
            {
                row = (y - window_y - 52) / NPC_LIST_ROW_HEIGHT;
                int result = MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                if (!row)
                {
                    handled = 1;
                    if (result > 0)
                    {
                        if (result == 1)
                            MainForm->net_client->sender->SendBankDeposit(npc_session_id, 1);
                        else
                        {
                            MainForm->animation->game->OpenAmountBox(AmountBoxKind_BankDeposit, result, 1, 0);
                            MainForm->animation->interface_controller->inventory->input_enabled = true;
                        }
                    }
                    else
                    {
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, false),
                            MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                    }
                }
                if (row == 1)
                {
                    handled = 1;
                    if (bank_gold > 0)
                    {
                        if (bank_gold == 1)
                            MainForm->net_client->sender->SendBankWithdraw(npc_session_id, 1);
                        else
                        {
                            MainForm->animation->game->OpenAmountBox(AmountBoxKind_BankWithdraw, bank_gold, 1, 0);
                            MainForm->animation->interface_controller->inventory->input_enabled = true;
                        }
                    }
                    else
                    {
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_CouldNotWithdraw, false),
                            MainForm->strings->GetDialogText(DialogText_CouldNotWithdraw, true));
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetDialogText(DialogText_CouldNotWithdraw, true));
                    }
                }
                if (row == 2)
                {
                    // Each upgrade costs more than the one before
                    upgrade_price = 1000 * locker_upgrade_level + 1000;
                    int gold = MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                    if (locker_upgrade_level > 6)
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LockerUpgradeImpossible, false),
                            MainForm->strings->GetDialogText(DialogText_LockerUpgradeImpossible, true));
                    else
                    {
                        if (gold < upgrade_price)
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                                MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                                LowerCase(MainForm->item_values->GetName(1)));
                        else
                        {
                            String price_text = IntToStr(upgrade_price);
                            price_text = price_text + " " + LowerCase(MainForm->item_values->GetName(1)) + "?";
                            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_BuyLockerUpgrade,
                                MainForm->strings->GetDialogText(DialogText_LockerUpgradeLike, false),
                                MainForm->strings->GetDialogText(DialogText_LockerUpgradeLike, true) + price_text);
                        }
                    }
                }
            }
        }
    }
    if (!handled)
    {
        if (window_x <= x && window_x + 270 >= x && y >= window_y && window_y + 235 >= y)
        {
            is_dragging = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered menu row and the close button.
void BankController::MouseMove(int x, int y)
{
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 358)
            window_x = 358;
        if (window_y >= 74)
            window_y = 74;
        MainForm->redraw_needed = true;
    }
    if (!(y <= window_y + 50 || y >= window_y + 170 || x <= window_x + 17 || x >= window_x + 252 || is_dragging))
    {
        int row = (y - window_y - 52) / NPC_LIST_ROW_HEIGHT;
        if (row > 2)
            row = -1;
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (hovered_row >= 0)
        {
            hovered_row = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < window_x + 91 || x > window_x + 181 || y < window_y + 192 || y > window_y + 218))
    {
        if (!close_hovered)
        {
            close_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (close_hovered)
        {
            close_hovered = 0;
            MainForm->redraw_needed = true;
        }
    }
}

// Ends moving the window.
void BankController::MouseUp()
{
    is_dragging = 0;
}
