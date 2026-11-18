#include <vcl.h>
#pragma hdrstop

#include "Ctrl_locker.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#include "Ctrl_inventory.h"
#include "Ctrl_shop.h"
#include "Ctrl_tooltip.h"

#pragma package(smart_init)

// A closed locker with no tile yet, its window at its default spot.
LockerController::LockerController()
{
    window_x = 49;
    window_y = 13;
    is_visible = false;
    hovered_slot = -1;
    reserved_value = -1;
    locker_x = -1;
    locker_y = -1;
    unused_flag = true;
    unused_value = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    scroll_changed = false;
    close_hovered = false;
    scroll_up_held = false;
    scroll_down_held = false;
    is_dragging = false;
    tooltip = new Tooltip(76, 50, 2);
    tooltip->SetFlipEdge(630, 63);
}

LockerController::~LockerController()
{
}

// Adds an item with its amount from the server's locker list.
void LockerController::AddItem(int item_id, int amount)
{
    InventoryItem handle(item_id);
    handle.amount = amount;
    items.insert(items.end(), handle);
}

// Shows the locker at the map tile with an empty list, closing other npc windows.
void LockerController::Open(String window_title, int reserved, int x, int y)
{
    MainForm->animation->shop->is_visible = false;
    MainForm->animation->guild->is_visible = false;
    MainForm->animation->memo->is_visible = false;
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->inn->is_visible = false;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    items.clear();
    top_row = 0;
    scroll_thumb_offset = 0;
    title = window_title;
    reserved_value = reserved;
    locker_x = x;
    locker_y = y;
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void LockerController::Close()
{
    tooltip->Hide();
    is_visible = false;
    reserved_value = -1;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Repeats a held scroll arrow.
DEFINE_TICK(LockerController)

int LockerController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int LockerController::GetTopRow()
{
    return top_row;
}

// Empties the list and scrolls back to the top.
void LockerController::ClearItems()
{
    items.clear();
    top_row = 0;
    scroll_thumb_offset = 0;
}

void LockerController::ClearList()
{
    items.clear();
}

void LockerController::AdjustScroll()
{
    int row_count = items.size() >> 2;
    if ((items.size() & 3) != 0)
        row_count++;
    if (row_count - top_row < 5)
        ScrollDown();
}

// A right-click on an item asks to take it out when it fits the inventory and the weight limit. Otherwise the
// scroll bar and moving the window.
void LockerController::MouseDown(int x, int y, int button)
{
    bool handled = false;
    if (button == 1)
    {
        if (y > window_y + 43 && y < window_y + 243 && x > window_x + 15 && x < window_x + 250)
        {
            is_dragging = false;
            TPoint locker_pos;
            locker_pos.x = locker_x;
            locker_pos.y = locker_y;
            // Four slots to a row; the item must fit the inventory and the weight limit.
            int row = (x - window_x - 17) / 57;
            if (row > 3)
                row = 3;
            row = row + 4 * ((y - window_y - 50) / NPC_LIST_ROW_HEIGHT) + 4 * top_row;
            if (row >= 0 && (int)items.size() > row)
            {
                if (MainForm->animation->interface_controller->inventory->HasRoomForItem(items[row].item_id))
                {
                    if (MainForm->animation->interface_controller->status->weight +
                        MainForm->item_values->GetWeight(items[row].item_id) <=
                        MainForm->animation->interface_controller->status->max_weight)
                        MainForm->net_client->sender->SendLockerTakeItem(locker_pos, items[row].item_id);
                    else
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetText(GameText_Warning),
                            MainForm->strings->GetText(GameText_ItsTooHeavyYou));
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_ItsTooHeavyYou));
                    }
                }
                else
                {
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetText(GameText_Warning),
                        MainForm->strings->GetText(GameText_YouCouldNotPick));
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouCouldNotPick));
                }
            }
        }
    }
    if (!handled)
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
}

// Moves a dragged window, kept on screen; shows the tip of the item under the mouse with its amount and weight, and
// tracks the close button.
void LockerController::MouseMove(int x, int y)
{
    int slot;
    UpdateScrollHover(x, y);
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 346)
            window_x = 346;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    // Over a slot, the item's tip shows with its amount.
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 15 && x < window_x + 250 && !is_dragging &&
        !thumb_grabbed)
    {
        int col = (x - window_x - 17) / 57;
        int row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if (col < 0)
            col = 0;
        if (row < 0)
            row = 0;
        if (col > 3)
            col = 3;
        if (row > 4)
            row = 4;
        slot = 4 * GetTopRow() + 4 * row + col;
        if (slot >= (int)items.size() || slot < 0)
        {
            hovered_slot = -1;
            if (tooltip->Hide())
                MainForm->redraw_needed = true;
        }
        else if (slot != hovered_slot)
        {
            hovered_slot = slot;
            hovered_col = col;
            hovered_row = row;
            MainForm->redraw_needed = true;
            int item_id = items[slot].item_id;
            unsigned int amount = items[slot].amount;
            tooltip->ShowItemCount(item_id, amount);
            tooltip->SetAnchorX(57 * col + window_x);
            tooltip->SetAnchorY(37 * row + window_y);
        }
    }
    else if (hovered_slot >= 0)
    {
        hovered_slot = -1;
        tooltip->Hide();
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 92 && x <= window_x + 182 && y >= window_y + 253 && y <= window_y + 279)
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

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_SHOP_UPDATE_SCROLL_HOVER(LockerController)

// Follows a grabbed scroll thumb, kept on its track, and picks the top row from its position.
void LockerController::DragScrollThumb(int y)
{
    int thumb_offset;
    int new_top_row;
    if (thumb_grabbed)
    {
        int row_count = items.size() >> 2;
        if ((items.size() & 3) != 0)
            row_count++;
        thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (items.size() > 20)
        {
            double thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            double scaled_row = (row_count - 5.0) * thumb_fraction;
            new_top_row = scaled_row;
            if (new_top_row != top_row)
                top_row = new_top_row;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list up a row when it is longer than five rows, moving the thumb and hiding the tip.
void LockerController::ScrollUp()
{
    int row_count = items.size() >> 2;
    if ((items.size() & 3) != 0)
        row_count++;
    if (row_count > LOCKER_VISIBLE_ROWS && top_row > 0)
    {
        top_row--;
        double scroll_fraction = (double)top_row / (row_count - 5);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        tooltip->Hide();
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list down a row while rows remain below, moving the thumb and hiding the tip.
void LockerController::ScrollDown()
{
    int row_count = items.size() >> 2;
    if ((items.size() & 3) != 0)
        row_count++;
    if (row_count - 5 > top_row)
    {
        top_row++;
        double scroll_fraction = (double)top_row / (row_count - 5);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        tooltip->Hide();
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void LockerController::MouseUp()
{
    is_dragging = false;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
}

// Every item may be stored.
char LockerController::CanStoreItem(int item_id)
{
    return 1;
}

// The amount of an item in the locker, or 0 when it holds none.
DEFINE_GET_ITEM_AMOUNT(LockerController)
