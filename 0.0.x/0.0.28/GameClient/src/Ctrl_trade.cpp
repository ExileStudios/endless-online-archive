#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Packetoperations.h"
#pragma package(smart_init)

#define TRADE_VISIBLE_ROWS 5

TradeController::TradeController()
{
    window_x = 49;
    window_y = 13;
    unused_flag = 1;
    accept_delay = 0;
    partner_scroll_up_hovered = false;
    partner_scroll_down_hovered = false;
    own_scroll_up_hovered = false;
    own_scroll_down_hovered = false;
    scroll_changed = 0;
    unused_index = -1;
    hovered_row = -1;
    own_player_id = -1;
    partner_player_id = -1;
    partner_removal_count = 0;
    partner_top_row = 0;
    own_top_row = 0;
    partner_scroll_thumb_offset = 0;
    own_scroll_thumb_offset = 0;
    own_status_text = "ok";
    partner_status_text = "ok";
    is_visible = 0;
    agree_hovered = false;
    cancel_hovered = false;
    partner_scroll_up_held = false;
    partner_scroll_down_held = false;
    own_scroll_up_held = false;
    own_scroll_down_held = false;
    is_dragging = 0;
}

TradeController::~TradeController()
{
}

// Adds an item with its amount to the partner's offer or to the own offer.
void TradeController::AddItem(int side, int item_id, int amount)
{
    InventoryItem handle(item_id);
    handle.amount = amount;
    if (side == 1)
        partner_items.insert(partner_items.end(), handle);
    else
        own_items.insert(own_items.end(), handle);
}

void TradeController::SetAcceptDelay(int delay_ticks)
{
    accept_delay = delay_ticks;
}

bool TradeController::IsAcceptDelayed()
{
    bool is_delayed = 0;
    if (accept_delay > 0)
        is_delayed = 1;
    return is_delayed;
}

// Counts down the agree wait and repeats a held scroll arrow on either list.
void TradeController::Tick()
{
    if (accept_delay > 0)
        accept_delay--;
    if (partner_scroll_repeat_delay > 0)
        partner_scroll_repeat_delay--;
    else
    {
        if (partner_scroll_up_held)
            ScrollPartnerUp();
        if (partner_scroll_down_held)
            ScrollPartnerDown();
    }
    if (own_scroll_repeat_delay > 0)
        own_scroll_repeat_delay--;
    else
    {
        if (own_scroll_up_held)
            ScrollOwnUp();
        if (own_scroll_down_held)
            ScrollOwnDown();
    }
}

// Shows the trade between the two players with empty offers and both lists at the top.
void TradeController::Open(String my_name, String their_name, int my_player_id, int their_player_id)
{
    MainForm->gfx->CreateTradeGraphics();
    MainForm->gfx->LoadTradeGraphics();
    own_name = my_name;
    partner_name = their_name;
    own_player_id = my_player_id;
    partner_player_id = their_player_id;
    removal_warning_armed = true;
    partner_removal_count = 0;
    partner_top_row = 0;
    own_top_row = 0;
    partner_scroll_thumb_offset = 0;
    own_scroll_thumb_offset = 0;
    own_status_text = MainForm->strings->GetText(GameText_Trading);
    partner_status_text = MainForm->strings->GetText(GameText_Trading);
    partner_items.clear();
    own_items.clear();
    is_visible = 1;
    own_agreed = false;
    trade_locked = false;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void TradeController::Close()
{
    is_visible = 0;
    own_player_id = -1;
    partner_player_id = -1;
    MainForm->gfx->FreeTradeGraphics();
    MainForm->redraw_needed = true;
}

int TradeController::GetPartnerScrollThumbOffset()
{
    return partner_scroll_thumb_offset;
}

int TradeController::GetOwnScrollThumbOffset()
{
    return own_scroll_thumb_offset;
}

int TradeController::GetPartnerTopRow()
{
    return partner_top_row;
}

int TradeController::GetOwnTopRow()
{
    return own_top_row;
}

// Scrolls both lists back to the top.
void TradeController::ResetScroll()
{
    partner_top_row = 0;
    own_top_row = 0;
    partner_scroll_thumb_offset = 0;
    own_scroll_thumb_offset = 0;
}

// A right-click on an own item asks to take it out of the offer and starts the agree wait. Otherwise the scroll bars
// and moving the window.
void TradeController::MouseDown(int x, int y, int button)
{
    char handled = 0;
    if (button == 1)
    {
        if (y > window_y + 43 && y < window_y + 243 && x > window_x + 280 && x < window_x + 515)
        {
            handled = 1;
            is_dragging = 0;
            // Removing works only while the own offer fits on one page
            if (own_items.size() < 6)
            {
                int row = own_top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
                if (row >= 0)
                {
                    if ((int)own_items.size() > row)
                    {
                        SetAcceptDelay(30);
                        MainForm->net_client->sender->SendTradeRemoveItem(own_items[row].item_id);
                    }
                }
            }
            else
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CannotRemove, false),
                    MainForm->strings->GetDialogText(DialogText_CannotRemove, true));
                MainForm->edits->caret_enabled = false;
            }
        }
    }
    if (!handled)
    {
        if (x >= window_x + 252 && x <= window_x + 267)
        {
            if (y >= window_y + partner_scroll_thumb_offset + 58 && y <= window_y + partner_scroll_thumb_offset + 73)
            {
                partner_thumb_grabbed = true;
                partner_thumb_grab_offset = y - partner_scroll_thumb_offset - window_y;
                DragPartnerScrollThumb(y);
            }
        }
        if (x >= window_x + 518 && x <= window_x + 533)
        {
            if (y >= window_y + own_scroll_thumb_offset + 58 && y <= window_y + own_scroll_thumb_offset + 73)
            {
                own_thumb_grabbed = true;
                own_thumb_grab_offset = y - own_scroll_thumb_offset - window_y;
                DragOwnScrollThumb(y);
            }
        }
        if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
        {
            partner_scroll_up_held = true;
            partner_scroll_repeat_delay = 4;
            ScrollPartnerUp();
        }
        if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
        {
            partner_scroll_down_held = true;
            partner_scroll_repeat_delay = 4;
            ScrollPartnerDown();
        }
        if (x >= window_x + 518 && x <= window_x + 533 && y >= window_y + 44 && y <= window_y + 59)
        {
            own_scroll_up_held = true;
            own_scroll_repeat_delay = 4;
            ScrollOwnUp();
        }
        if (x >= window_x + 518 && x <= window_x + 533 && y >= window_y + 228 && y <= window_y + 243)
        {
            own_scroll_down_held = true;
            own_scroll_repeat_delay = 4;
            ScrollOwnDown();
        }
        if (x >= window_x && x <= window_x + 500 && y >= window_y && y <= window_y + 280 && !partner_thumb_grabbed &&
            !own_thumb_grabbed)
        {
            is_dragging = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered own row and the agree and cancel buttons.
void TradeController::MouseMove(int x, int y)
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
        if (window_x >= 80)
            window_x = 80;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (!(y <= window_y + 43 || y >= window_y + 243 || x <= window_x + 280 || x >= window_x + 515 || is_dragging))
    {
        int row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if (row >= (int)own_items.size() || row > 4)
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
    if (!(x < window_x + 357 || x > window_x + 445 || y < window_y + 254 || y > window_y + 281))
    {
        if (!agree_hovered)
        {
            agree_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (agree_hovered)
        {
            agree_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
    if (!(x < window_x + 450 || x > window_x + 538 || y < window_y + 254 || y > window_y + 281))
    {
        if (!cancel_hovered)
        {
            cancel_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (cancel_hovered)
        {
            cancel_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
}

// Lights the scroll arrows under the mouse on both lists and drags a grabbed thumb.
void TradeController::UpdateScrollHover(int x, int y)
{
    if (x >= this->window_x + 252 && x <= this->window_x + 267)
    {
        if (y >= this->window_y + 58 && y <= this->window_y + 229)
            this->DragPartnerScrollThumb(y);
    }
    if (x >= window_x + 518 && x <= window_x + 533 && (y >= window_y + 58 && y <= window_y + 229))
        DragOwnScrollThumb(y);
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 44 && y <= window_y + 59)
    {
        if (!partner_scroll_up_hovered)
        {
            partner_scroll_up_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (partner_scroll_up_hovered)
    {
        partner_scroll_up_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 252 && x <= window_x + 267 && y >= window_y + 228 && y <= window_y + 243)
    {
        if (!partner_scroll_down_hovered)
        {
            partner_scroll_down_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (partner_scroll_down_hovered)
    {
        partner_scroll_down_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 518 && x <= window_x + 533 && y >= window_y + 44 && y <= window_y + 59)
    {
        if (!own_scroll_up_hovered)
        {
            own_scroll_up_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (own_scroll_up_hovered)
    {
        own_scroll_up_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 518 && x <= window_x + 533 && y >= window_y + 228 && y <= window_y + 243)
    {
        if (!own_scroll_down_hovered)
        {
            own_scroll_down_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (own_scroll_down_hovered)
    {
        own_scroll_down_hovered = false;
        MainForm->redraw_needed = true;
    }
}

// Follows the grabbed thumb of the partner's list, kept on its track, and picks that list's top row from it.
void TradeController::DragPartnerScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_row;
    int __slot1;
    int __slot2;
    if (partner_thumb_grabbed)
    {
        int thumb_offset = y - partner_thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (partner_items.size() > TRADE_VISIBLE_ROWS)
        {
            thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            scaled_row = (partner_items.size() - 5.0) * thumb_fraction;
            int new_top_row = scaled_row;
            if (new_top_row != partner_top_row)
                partner_top_row = new_top_row;
        }
        partner_scroll_thumb_offset = thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Follows the grabbed thumb of the own list, kept on its track, and picks that list's top row from it.
void TradeController::DragOwnScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_row;
    int __slot1;
    int __slot2;
    if (own_thumb_grabbed)
    {
        int thumb_offset = y - own_thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (own_items.size() > TRADE_VISIBLE_ROWS)
        {
            thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            scaled_row = (own_items.size() - 5.0) * thumb_fraction;
            int new_top_row = scaled_row;
            if (new_top_row != own_top_row)
                own_top_row = new_top_row;
        }
        own_scroll_thumb_offset = thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the partner's list up one row and moves its thumb to match.
void TradeController::ScrollPartnerUp()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)partner_items.size() > TRADE_VISIBLE_ROWS && partner_top_row > 0)
    {
        partner_top_row--;
        scroll_fraction = (double)partner_top_row / (double)(partner_items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        partner_scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the partner's list down one row and moves its thumb to match.
void TradeController::ScrollPartnerDown()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)partner_items.size() - 5 > partner_top_row)
    {
        partner_top_row++;
        scroll_fraction = (double)partner_top_row / (double)(partner_items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        partner_scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the own list up one row and moves its thumb to match.
void TradeController::ScrollOwnUp()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)own_items.size() > TRADE_VISIBLE_ROWS && own_top_row > 0)
    {
        own_top_row--;
        scroll_fraction = (double)own_top_row / (double)(own_items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        own_scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the own list down one row and moves its thumb to match.
void TradeController::ScrollOwnDown()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)own_items.size() - 5 > own_top_row)
    {
        own_top_row++;
        scroll_fraction = (double)own_top_row / (double)(own_items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        own_scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumbs.
void TradeController::MouseUp()
{
    is_dragging = 0;
    partner_scroll_up_held = false;
    partner_scroll_down_held = false;
    own_scroll_up_held = false;
    own_scroll_down_held = false;
    partner_thumb_grabbed = false;
    own_thumb_grabbed = false;
}
