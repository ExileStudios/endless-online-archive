#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

#define LOCKER_VISIBLE_ROWS 5

LockerController::LockerController()
{
    window_x = 49;
    window_y = 13;
    is_visible = 0;
    hovered_row = -1;
    reserved_value = -1;
    locker_x = -1;
    locker_y = -1;
    unused_flag = 1;
    unused_value = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    close_hovered = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
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
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->guild->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    items.clear();
    top_row = 0;
    scroll_thumb_offset = 0;
    title = window_title;
    reserved_value = reserved;
    locker_x = x;
    locker_y = y;
    is_visible = 1;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void LockerController::Close()
{
    is_visible = 0;
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
    int __slot1;
    items.clear();
    top_row = 0;
    scroll_thumb_offset = 0;
}

// A right-click on an item asks to take it out when it fits the inventory and the weight limit. Otherwise the
// scroll bar and moving the window.
void LockerController::MouseDown(int x, int y, int button)
{
    bool flag;
    TPoint locker_pos;
    flag = false;
    if (button == 1)
    {
        if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 252)
        {
            is_dragging = 0;
            locker_pos.x = locker_x;
            locker_pos.y = locker_y;
            int row = top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row >= 0 && (int)items.size() > row)
            {
                if (MainForm->animation->interface_controller->inventory->HasRoomForItem(items[row].item_id))
                {
                    if (MainForm->animation->interface_controller->status->weight +
                        MainForm->item_values->GetWeight(items[row].item_id) <=
                        MainForm->animation->interface_controller->status->max_weight)
                    {
                        MainForm->net_client->sender->SendLockerTakeItem(locker_pos, items[row].item_id);
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
    if (!flag)
    {
        if (x >= window_x + 252 && x <= window_x + 267)
        {
            if (y >= window_y + scroll_thumb_offset + 58 && y <= window_y + scroll_thumb_offset + 73)
            {
                thumb_grabbed = 1;
                thumb_grab_offset = y - scroll_thumb_offset - window_y;
                DragScrollThumb(y);
            }
        }
        if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
        {
            scroll_down_held = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 290 && !thumb_grabbed)
        {
            is_dragging = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered row and the close button.
void LockerController::MouseMove(int x, int y)
{
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
    if (!(y <= window_y + 43 || y >= window_y + 243 || x <= window_x + 17 || x >= window_x + 252 || is_dragging))
    {
        int row = top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if (row >= (int)items.size() || row > 4)
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
    if (!(x < window_x + 92 || x > window_x + 182 || y < window_y + 253 || y > window_y + 279))
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

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(LockerController)

// Follows a grabbed scroll thumb, kept on its track, and picks the top row from its position.
void LockerController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_row;
    int __slot1;
    int __slot2;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (items.size() > LOCKER_VISIBLE_ROWS)
        {
            thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            scaled_row = (items.size() - 5.0) * thumb_fraction;
            int new_top_row = scaled_row;
            if (new_top_row != top_row)
                top_row = new_top_row;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list up one row and moves the scroll thumb to match.
void LockerController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)items.size() > LOCKER_VISIBLE_ROWS && top_row > 0)
    {
        top_row--;
        scroll_fraction = (double)top_row / (double)(items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list down one row and moves the scroll thumb to match.
void LockerController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)items.size() - 5 > top_row)
    {
        top_row++;
        scroll_fraction = (double)top_row / (double)(items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void LockerController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

// Every item may be stored.
char LockerController::CanStoreItem(int item_id)
{
    return 1;
}

// The amount of an item in the locker, or 0 when it holds none.
DEFINE_GET_ITEM_AMOUNT(LockerController)
