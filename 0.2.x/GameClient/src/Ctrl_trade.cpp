#include <vcl.h>
#pragma hdrstop

#include "Ctrl_trade.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Packetoperations.h"
#include "Ctrl_tooltip.h"

#pragma package(smart_init)

// A trade with no partner yet, its window at its default spot.
TradeController::TradeController()
{
    window_x = 49;
    window_y = 13;
    unused_flag = true;
    accept_delay = 0;
    partner_scroll_up_hovered = false;
    partner_scroll_down_hovered = false;
    own_scroll_up_hovered = false;
    own_scroll_down_hovered = false;
    scroll_changed = false;
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
    is_visible = false;
    agree_hovered = false;
    cancel_hovered = false;
    partner_scroll_up_held = false;
    partner_scroll_down_held = false;
    own_scroll_up_held = false;
    own_scroll_down_held = false;
    is_dragging = false;
    tooltip = new Tooltip(76, 49);
    tooltip->SetFlipEdge(630, 63);
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
    bool is_delayed = false;
    if (accept_delay > 0)
        is_delayed = true;
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
    {
        own_scroll_repeat_delay--;
        return;
    }
    if (own_scroll_up_held)
        ScrollOwnUp();
    if (own_scroll_down_held)
        ScrollOwnDown();
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
    is_visible = true;
    own_agreed = false;
    trade_locked = false;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void TradeController::Close()
{
    tooltip->Hide();
    is_visible = false;
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
    char handled = false;
    if (button == 1)
    {
        if (y > window_y + 43 && y < window_y + 243 && x > window_x + 280 && x < window_x + 515)
        {
            handled = true;
            is_dragging = false;
            // Removing works only while the own offer fits on one page
            if (own_items.size() < 6)
            {
                int row = own_top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
                if (row >= 0 && (int)own_items.size() > row)
                {
                    SetAcceptDelay(10);
                    MainForm->net_client->sender->SendTradeRemoveItem(own_items[row].item_id);
                }
            }
            else
            {
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
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
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen; shows the tip of the item under the mouse on either side, and tracks the
// agree and cancel buttons.
void TradeController::MouseMove(int x, int y)
{
    int amount;
    int row;
    int partner_row;
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
    // Over the partner's items, the tip of the item under the mouse shows.
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 10 && x < window_x + 74 && !is_dragging)
    {
        partner_row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if ((int)partner_items.size() <= partner_row || partner_row > 4)
            partner_row = -1;
        if (partner_row >= 0)
        {
            int partner_index = GetPartnerTopRow() + partner_row;
            int item_id = MainForm->animation->trade->partner_items[partner_index].item_id;
            amount = MainForm->animation->trade->partner_items[partner_index].amount;
            tooltip->ShowItemCount(item_id, amount);
            tooltip->SetAnchorX(window_x);
            tooltip->SetAnchorY(37 * partner_row + window_y);
            MainForm->redraw_needed = true;
        }
        else if (tooltip->Hide())
            MainForm->redraw_needed = true;
    }
    else if (tooltip->Hide())
        MainForm->redraw_needed = true;
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 280 && x < window_x + 515 && !is_dragging)
    {
        row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if (row >= (int)own_items.size() || row > 4)
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
    if (x >= window_x + 357 && x <= window_x + 445 && y >= window_y + 254 && y <= window_y + 281)
    {
        if (!agree_hovered)
        {
            agree_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (agree_hovered)
    {
        agree_hovered = false;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 450 && x <= window_x + 538 && y >= window_y + 254 && y <= window_y + 281)
    {
        if (!cancel_hovered)
        {
            cancel_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (cancel_hovered)
    {
        cancel_hovered = false;
        MainForm->redraw_needed = true;
    }
}

// Lights the scroll arrows under the mouse on both lists and drags a grabbed thumb.
void TradeController::UpdateScrollHover(int x, int y)
{
    if (x >= window_x + 252 && x <= window_x + 267)
    {
        if (y >= window_y + 58 && y <= window_y + 229)
            DragPartnerScrollThumb(y);
    }
    if (x >= window_x + 518 && x <= window_x + 533)
    {
        if (y >= window_y + 58 && y <= window_y + 229)
            DragOwnScrollThumb(y);
    }
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
    int new_top_row;
    if (partner_thumb_grabbed)
    {
        int thumb_offset = y - partner_thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (partner_items.size() > TRADE_VISIBLE_ROWS)
        {
            double thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            double scaled_row = (partner_items.size() - 5.0) * thumb_fraction;
            new_top_row = scaled_row;
            if (new_top_row != partner_top_row)
                partner_top_row = new_top_row;
        }
        partner_scroll_thumb_offset = thumb_offset;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Follows the grabbed thumb of the own list, kept on its track, and picks that list's top row from it.
void TradeController::DragOwnScrollThumb(int y)
{
    int new_top_row;
    if (own_thumb_grabbed)
    {
        int thumb_offset = y - own_thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (own_items.size() > TRADE_VISIBLE_ROWS)
        {
            double thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            double scaled_row = (own_items.size() - 5.0) * thumb_fraction;
            new_top_row = scaled_row;
            if (new_top_row != own_top_row)
                own_top_row = new_top_row;
        }
        own_scroll_thumb_offset = thumb_offset;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the partner's list up one row and moves its thumb to match.
void TradeController::ScrollPartnerUp()
{
    if ((int)partner_items.size() > TRADE_VISIBLE_ROWS && partner_top_row > 0)
    {
        partner_top_row--;
        double scroll_fraction = (double)partner_top_row / (partner_items.size() - 5);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        partner_scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the partner's list down one row and moves its thumb to match.
void TradeController::ScrollPartnerDown()
{
    if ((int)partner_items.size() - 5 > partner_top_row)
    {
        partner_top_row++;
        double scroll_fraction = (double)partner_top_row / (partner_items.size() - 5);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        partner_scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the own list up one row and moves its thumb to match.
void TradeController::ScrollOwnUp()
{
    if ((int)own_items.size() > TRADE_VISIBLE_ROWS && own_top_row > 0)
    {
        own_top_row--;
        double scroll_fraction = (double)own_top_row / (own_items.size() - 5);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        own_scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the own list down one row and moves its thumb to match.
void TradeController::ScrollOwnDown()
{
    if ((int)own_items.size() - 5 > own_top_row)
    {
        own_top_row++;
        double scroll_fraction = (double)own_top_row / (own_items.size() - 5);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        own_scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumbs.
void TradeController::MouseUp()
{
    is_dragging = false;
    partner_scroll_up_held = false;
    partner_scroll_down_held = false;
    own_scroll_up_held = false;
    own_scroll_down_held = false;
    partner_thumb_grabbed = false;
    own_thumb_grabbed = false;
}
