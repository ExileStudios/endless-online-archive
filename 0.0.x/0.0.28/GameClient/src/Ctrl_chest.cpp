#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

ChestController::ChestController()
{
    window_x = 35;
    window_y = 23;
    is_visible = 0;
    hovered_row = -1;
    reserved_value = -1;
    chest_x = -1;
    chest_y = -1;
}

ChestController::~ChestController()
{
}

// Adds an item with its amount from the server's chest list.
void ChestController::AddItem(int item_id, int amount)
{
    InventoryItem handle(item_id);
    handle.amount = amount;
    items.insert(items.end(), handle);
}

// Empties the item list.
void ChestController::ClearItems()
{
    int __slot1;
    items.clear();
}

// Shows the chest at the map tile with an empty list.
void ChestController::Open(int reserved, int x, int y)
{
    int __slot1;
    MainForm->gfx->CreateChestGraphics();
    MainForm->gfx->LoadChestGraphics();
    items.clear();
    reserved_value = reserved;
    chest_x = x;
    chest_y = y;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window, forgets the chest's tile and frees its graphics.
void ChestController::Close()
{
    is_visible = 0;
    reserved_value = -1;
    chest_x = -1;
    chest_y = -1;
    MainForm->gfx->FreeChestGraphics();
    MainForm->redraw_needed = true;
}

// A right-click on an item asks to take it when it fits the inventory and the weight limit; any other click in the
// window starts moving it.
void ChestController::MouseDown(int x, int y, int button)
{
    char handled;
    TPoint chest_pos;
    handled = 0;
    if (button == 1)
    {
        if (y > window_y + 24 && y < window_y + 243 && window_x + 17 < x)
        {
            if (window_x + 252 > x)
            {
                handled = 1;
                chest_pos.x = chest_x;
                chest_pos.y = chest_y;
                int result = (y - window_y - 18) / NPC_LIST_ROW_HEIGHT;
                if (result >= 0)
                {
                    if (result < (int)items.size())
                    {
                        if (MainForm->animation->interface_controller->inventory->HasRoomForItem(items[result].item_id))
                        {
                            if (MainForm->animation->interface_controller->status->weight +
                                MainForm->item_values->GetWeight(items[result].item_id) <=
                                MainForm->animation->interface_controller->status->max_weight)
                            {
                                MainForm->net_client->sender->SendChestTakeItem(chest_pos, items[result].item_id);
                            }
                            else
                            {
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetText(GameText_Warning),
                                    MainForm->strings->GetText(GameText_ItsTooHeavyYou));
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_ItsTooHeavyYou));
                            }
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_Warning),
                                MainForm->strings->GetText(GameText_YouCouldNotPick));
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                MainForm->strings->GetText(GameText_YouCouldNotPick));
                        }
                    }
                }
            }
        }
    }
    if (!handled)
    {
        if (window_x <= x && window_x + 270 >= x && y >= window_y && y <= window_y + 270)
        {
            is_dragging = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered row and the close button.
void ChestController::MouseMove(int x, int y)
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
        if (window_y >= 39)
            window_y = 39;
        MainForm->redraw_needed = true;
    }
    if (!(y <= window_y + 24 || y >= window_y + 243 || x <= window_x + 17 || x >= window_x + 252 || is_dragging))
    {
        int row = (y - window_y - 18) / NPC_LIST_ROW_HEIGHT;
        if (row >= (int)items.size())
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
    if (!(x < window_x + 92 || x > window_x + 182 || y < window_y + 231 || y > window_y + 254))
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
void ChestController::MouseUp()
{
    is_dragging = 0;
}
