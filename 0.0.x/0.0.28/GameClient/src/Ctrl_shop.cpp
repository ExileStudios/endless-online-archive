#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

#define SHOP_VISIBLE_ROWS 5

ShopController::ShopController()
{
    window_x = 49;
    window_y = 13;
    unused_flag = 1;
    unused_value = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    npc_session_id = -1;
    top_row = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = 0;
    close_hovered = 0;
    back_hovered = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
}

ShopController::~ShopController()
{
}

// Shows the shop for the npc on its menu page with empty lists, closing the other npc windows.
void ShopController::Open(String npc, int session_id)
{
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->guild->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->learn->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    npc_name = npc;
    npc_session_id = session_id;
    top_row = 0;
    scroll_thumb_offset = 0;
    page = ShopPage_Menu;
    shown_items.clear();
    buy_items.clear();
    sell_items.clear();
    craft_items.clear();
    is_visible = 1;
    unused_cleared_flag = 0;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void ShopController::Close()
{
    is_visible = 0;
    npc_session_id = -1;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Adds a recipe with up to four ingredients and their amounts, once per item.
void ShopController::AddCraftItem(int item_id, int first_ingredient_id, int first_ingredient_amount,
    int second_ingredient_id, int second_ingredient_amount, int third_ingredient_id, int third_ingredient_amount,
    int fourth_ingredient_id, int fourth_ingredient_amount)
{
    char known = 0;
    for (vector<CraftItem>::iterator it = craft_items.begin(); it != craft_items.end(); it++)
    {
        if (item_id == it->item_id)
        {
            known = 1;
            break;
        }
    }
    if (!known)
    {
        CraftItem handle(item_id);
        handle.ingredient_ids[0] = first_ingredient_id; handle.ingredient_amounts[0] = first_ingredient_amount;
        handle.ingredient_ids[1] = second_ingredient_id; handle.ingredient_amounts[1] = second_ingredient_amount;
        handle.ingredient_ids[2] = third_ingredient_id; handle.ingredient_amounts[2] = third_ingredient_amount;
        handle.ingredient_ids[3] = fourth_ingredient_id; handle.ingredient_amounts[3] = fourth_ingredient_amount;
        craft_items.insert(craft_items.end(), handle);
    }
}

// Adds an item to the buy list or the sell list; an item listed again adds to its amount.
void ShopController::AddTradeItem(int list_type, int item_id, int item_id_copy, int price, int amount)
{
    char buy_found;
    char sell_found;
    if (list_type == 1)
    {
        vector<ShopItem>::iterator it;
        buy_found = 0;
        for (it = buy_items.begin(); it != buy_items.end(); it++)
        {
            if (item_id == it->item_id)
            {
                buy_found = 1;
                it->max_amount += amount;
                break;
            }
        }
        if (!buy_found)
        {
            ShopItem handle(item_id);
            handle.price = price;
            handle.max_amount = amount;
            buy_items.insert(buy_items.end(), handle);
        }
    }
    else
    {
        vector<ShopItem>::iterator it;
        sell_found = 0;
        for (it = sell_items.begin(); it != sell_items.end(); it++)
        {
            if (item_id == it->item_id)
            {
                sell_found = 1;
                it->max_amount += amount;
                break;
            }
        }
        if (!sell_found)
        {
            ShopItem sell_item(item_id);
            sell_item.price = price;
            sell_item.max_amount = amount;
            sell_items.insert(sell_items.end(), sell_item);
        }
    }
}

// Repeats a held scroll arrow.
DEFINE_TICK(ShopController)

int ShopController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int ShopController::GetTopRow()
{
    return top_row;
}

void ShopController::ResetScroll()
{
    top_row = 0;
    scroll_thumb_offset = 0;
}

// The number of sell-list items the inventory holds.
int ShopController::CountSellableItems()
{
    int count = 0;
    vector<ShopItem>::iterator it;
    for (it = sell_items.begin(); it != sell_items.end(); it++)
    {
        if (MainForm->animation->interface_controller->inventory->GetItemAmount(it->item_id) > 0)
            count++;
    }
    return count;
}

// A click in the window. On the menu: open the buy, sell or craft list. On the buy list: checks weight, room and
// gold, then asks to confirm one item or opens the amount box. On the sell list: the same without the checks.
// On the craft list: shows the recipe. Otherwise the scroll bar and moving the window.
void ShopController::MouseDown(int x, int y, int button)
{
    char handled = 0;
    if ((button == 0 || button == 1) && this->window_y + 43 < y && this->window_y + 243 > y)
    {
        if (window_x + 17 < x && window_x + 252 > x)
        {
            is_dragging = 0;
            int row = top_row + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (page == ShopPage_Menu)
            {
                if (!row)
                {
                    handled = 1;
                    if (this->buy_items.size() <= 0)
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_StoreEmptyThere, false),
                            MainForm->strings->GetDialogText(DialogText_StoreEmptyThere, true));
                    else
                    {
                        shown_items.clear();
                        for (vector<ShopItem>::iterator j = buy_items.begin(); j != buy_items.end(); j++)
                            shown_items.insert(shown_items.end(), *j);
                        page = ShopPage_Buy;
                    }
                }
                if (row == 1)
                {
                    handled = 1;
                    if (this->sell_items.size() <= 0)
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_RefusedBuying, false),
                            MainForm->strings->GetDialogText(DialogText_RefusedBuying, true));
                    else
                    {
                        shown_items.clear();
                        for (vector<ShopItem>::iterator k = sell_items.begin(); k != sell_items.end(); k++)
                        {
                            int carried_amount = MainForm->animation->interface_controller->inventory->GetItemAmount(k->item_id);
                            // Only items the inventory holds are offered for sale, with the amount carried
                            if (carried_amount > 0)
                            {
                                k->max_amount = carried_amount;
                                shown_items.insert(shown_items.end(), *k);
                            }
                        }
                        if (!this->shown_items.size())
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_RefusedBuying, false),
                                MainForm->strings->GetDialogText(DialogText_RefusedBuying, true));
                        else
                            page = ShopPage_Sell;
                    }
                }
                if (row == 2)
                {
                    handled = 1;
                    if (craft_items.size() > 0)
                    {
                        shown_items.clear();
                        for (vector<CraftItem>::iterator m = craft_items.begin(); m != craft_items.end(); m++)
                        {
                            int ingredient_count = 0;
                            for (int ingredient = 0; ingredient < MAX_CRAFT_INGREDIENTS; ingredient++)
                            {
                                if (m->ingredient_ids[ingredient] > 0 && m->ingredient_amounts[ingredient] > 0)
                                    ingredient_count++;
                            }
                            ShopItem handle(m->item_id);
                            handle.price = ingredient_count;
                            shown_items.insert(shown_items.end(), handle);
                        }
                        page = ShopPage_Craft;
                    }
                }
            }
            else
            {
                if (page == ShopPage_Buy && row >= 0 && (int)shown_items.size() > row)
                {
                    handled = 1;
                    int buy_item_id = MainForm->animation->shop->shown_items[row].item_id;
                    int buy_amount = MainForm->animation->shop->shown_items[row].max_amount;
                    selected_price = MainForm->animation->shop->shown_items[row].price;
                    selected_item_id = buy_item_id;
                    selected_amount = 1;
                    if (MainForm->animation->interface_controller->status->weight <
                        MainForm->animation->interface_controller->status->max_weight)
                    {
                        if (MainForm->animation->interface_controller->inventory->TestAddItem(buy_item_id))
                        {
                            if (MainForm->animation->interface_controller->status->gold >= selected_price)
                            {
                                if (buy_amount == 1)
                                {
                                    String buy_confirm_text = MainForm->strings->GetText(GameText_Buy);
                                    buy_confirm_text = buy_confirm_text + " 1 ";
                                    buy_confirm_text = buy_confirm_text +
                                        MainForm->item_values->GetName(selected_item_id);
                                    buy_confirm_text = buy_confirm_text + " ";
                                    buy_confirm_text = buy_confirm_text + MainForm->strings->GetText(GameText_For);
                                    buy_confirm_text = buy_confirm_text + " ";
                                    buy_confirm_text = buy_confirm_text + IntToStr(selected_price);
                                    buy_confirm_text = buy_confirm_text + " ";
                                    buy_confirm_text = buy_confirm_text + LowerCase(MainForm->item_values->GetName(1));
                                    buy_confirm_text = buy_confirm_text + " ?";
                                    MainForm->animation->game->ShowMessageBox(3, ConfirmAction_BuyItem,
                                        MainForm->strings->GetText(GameText_BuyItemS), buy_confirm_text);
                                    MainForm->edits->caret_enabled = false;
                                }
                                if (buy_amount > 1)
                                {
                                    MainForm->animation->game->OpenAmountBox(AmountBoxKind_ShopBuy, buy_amount, buy_item_id, 0);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                            }
                            else
                            {
                                String no_gold_text = MainForm->strings->GetDialogText(DialogText_WarningEnough, true) +
                                    " " +
                                    LowerCase(MainForm->item_values->GetName(1));
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_WarningEnough, false), no_gold_text);
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                        else
                        {
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetText(GameText_YouHaveNotEnough));
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_Warning),
                                MainForm->strings->GetText(GameText_YouHaveNotEnough));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    else
                    {
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetText(GameText_Warning),
                            MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                        MainForm->edits->caret_enabled = false;
                    }
                }
                if (page == ShopPage_Sell && row >= 0 && (int)shown_items.size() > row)
                {
                    handled = 1;
                    int sell_item_id = MainForm->animation->shop->shown_items[row].item_id;
                    int sell_amount = MainForm->animation->shop->shown_items[row].max_amount;
                    selected_price = MainForm->animation->shop->shown_items[row].price;
                    selected_item_id = sell_item_id;
                    selected_amount = 1;
                    if (sell_amount == 1)
                    {
                        String sell_confirm_text = MainForm->strings->GetText(GameText_Sell);
                        sell_confirm_text = sell_confirm_text + " 1 ";
                        sell_confirm_text = sell_confirm_text + MainForm->item_values->GetName(selected_item_id);
                        sell_confirm_text = sell_confirm_text + " ";
                        sell_confirm_text = sell_confirm_text + MainForm->strings->GetText(GameText_For);
                        sell_confirm_text = sell_confirm_text + " ";
                        sell_confirm_text = sell_confirm_text + IntToStr(selected_price);
                        sell_confirm_text = sell_confirm_text + " ";
                        sell_confirm_text = sell_confirm_text + LowerCase(MainForm->item_values->GetName(1));
                        sell_confirm_text = sell_confirm_text + " ?";
                        MainForm->animation->game->ShowMessageBox(3, ConfirmAction_SellItem,
                            MainForm->strings->GetText(GameText_SellItemsS), sell_confirm_text);
                        MainForm->edits->caret_enabled = false;
                    }
                    if (sell_amount > 1)
                    {
                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_ShopSell, sell_amount, sell_item_id, 0);
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                    }
                }
                if (page == ShopPage_Craft && row >= 0 && (int)shown_items.size() > row)
                {
                    handled = 1;
                    int craft_item_id = MainForm->animation->shop->shown_items[row].item_id;
                    int craft_max_amount = MainForm->animation->shop->shown_items[row].max_amount;
                    String craft_item_name = MainForm->item_values->GetName(craft_item_id);
                    MainForm->animation->shop->selected_item_id = craft_item_id;
                    MainForm->animation->shop->selected_amount = 1;
                    String craft_text;
                    int buttons = 2;
                    char recipe_found = 0;
                    MainForm->animation->game->large_box_lines[1] = "";
                    MainForm->animation->game->large_box_lines[2] = "";
                    MainForm->animation->game->large_box_lines[3] = "";
                    MainForm->animation->game->large_box_lines[4] = "";
                    MainForm->animation->game->large_box_lines[5] = "";
                    vector<CraftItem>::iterator i;
                    for (i = this->craft_items.begin(); i != this->craft_items.end(); i++)
                    {
                        if (craft_item_id == i->item_id)
                        {
                            recipe_found = 1;
                            for (int craft_ingredient = 0; craft_ingredient < MAX_CRAFT_INGREDIENTS; craft_ingredient++)
                            {
                                if (i->ingredient_ids[craft_ingredient] <= 0)
                                    continue;
                                if (i->ingredient_amounts[craft_ingredient] <= 0)
                                    continue;
                                int carried_ingredient_amount = MainForm->animation->interface_controller->inventory->GetItemAmount(i->ingredient_ids[craft_ingredient]);
                                int needed_ingredient_amount = i->ingredient_amounts[craft_ingredient];
                                String ingredient_name = MainForm->item_values->GetName(i->ingredient_ids[craft_ingredient]);
                                if (carried_ingredient_amount < needed_ingredient_amount)
                                    recipe_found = 0;
                                MainForm->animation->game->large_box_lines[craft_ingredient + 2] = "+  " +
                                    IntToStr(needed_ingredient_amount) + " " + ingredient_name;
                            }
                            break;
                        }
                    }
                    // The box offers crafting only when every ingredient is carried in full
                    if (recipe_found)
                    {
                        craft_text = MainForm->strings->GetText(GameText_PutIngredientsTogether);
                        buttons = 3;
                    }
                    else
                        craft_text = MainForm->strings->GetText(GameText_YouDontHaveAll);
                    MainForm->animation->game->ShowLargeMessageBox(buttons, LargeBoxKind_CraftItem,
                        MainForm->strings->GetText(GameText_Ingredients) + " " + craft_item_name, craft_text);
                    MainForm->edits->caret_enabled = false;
                }
                if (page == ShopPage_Unknown4 && row >= 0 && (int)shown_items.size() > row)
                    handled = 1;
            }
        }
    }
    if (!handled)
    {
        if (this->window_x + 252 <= x && this->window_x + 267 >= x &&
            this->window_y + this->scroll_thumb_offset + 58 <= y &&
            this->window_y + this->scroll_thumb_offset + 73 >= y)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 44 <= y)
        {
            if (window_y + 59 >= y)
            {
                scroll_up_held = 1;
                scroll_repeat_delay = 4;
                ScrollUp();
            }
        }
        if (window_x + 252 <= x && window_x + 267 >= x)
        {
            if (window_y + 228 <= y && window_y + 243 >= y)
            {
                scroll_down_held = 1;
                scroll_repeat_delay = 4;
                ScrollDown();
            }
        }
        if (window_x <= x)
        {
            if (window_x + 284 >= x && y >= window_y && window_y + 290 >= y && !thumb_grabbed)
            {
                is_dragging = 1;
                drag_offset_x = x - window_x;
                drag_offset_y = y - window_y;
            }
        }
    }
}

// Moves a dragged window, kept on screen; tracks the hovered row, names a hovered buy or craft item in the status
// bar, and tracks the close and back buttons.
void ShopController::MouseMove(int x, int y)
{
    int row;
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
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 252 && !is_dragging)
    {
        int last_row = 1;
        if (page == ShopPage_Menu)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (craft_items.size() > 0)
                last_row = 2;
            if (row > last_row)
                row = -1;
        }
        else
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if ((int)shown_items.size() <= row || row > 4)
                row = -1;
        }
        if (row != hovered_row)
        {
            if (row != -1)
            {
                if (page == ShopPage_Buy || page == ShopPage_Craft)
                {
                    String tooltip_text = Client::Capitalize(MainForm->net_client,
                        MainForm->item_values->GetName(shown_items[row].item_id));
                    if (page == ShopPage_Buy)
                    {
                        tooltip_text = tooltip_text + ", ";
                        tooltip_text = tooltip_text + IntToStr((unsigned int)shown_items[row].price) + " " +
                            MainForm->item_values->GetName(1);
                    }
                    int required_class = MainForm->item_values->GetClassRequirement(shown_items[row].item_id);
                    if (required_class > 0)
                        tooltip_text = tooltip_text + ", " + MainForm->class_values->GetName(required_class);
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        tooltip_text);
                }
            }
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else if (hovered_row >= 0)
    {
        hovered_row = -1;
        MainForm->redraw_needed = true;
    }
    if (page == ShopPage_Menu)
    {
        if (!(x < this->window_x + 92 || x > this->window_x + 182 || y < this->window_y + 253 ||
            y > this->window_y + 279))
        {
            if (!this->close_hovered)
            {
                this->close_hovered = 1;
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
    else
    {
        if (!(x < this->window_x + 144 || x > this->window_x + 234 || y < this->window_y + 253 ||
            y > this->window_y + 279))
        {
            if (!this->close_hovered)
            {
                this->close_hovered = 1;
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
        if (!(x < this->window_x + 48 || x > this->window_x + 138 || y < this->window_y + 253 ||
            y > this->window_y + 279))
        {
            if (!this->back_hovered)
            {
                this->back_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else
        {
            if (back_hovered)
            {
                back_hovered = 0;
                MainForm->redraw_needed = true;
            }
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(ShopController)

// Follows a grabbed scroll thumb, kept on its track, and picks the top row from its position.
void ShopController::DragScrollThumb(int y)
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
        if (shown_items.size() > SHOP_VISIBLE_ROWS)
        {
            thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
            scaled_row = (shown_items.size() - 5.0) * thumb_fraction;
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
void ShopController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)shown_items.size() > SHOP_VISIBLE_ROWS && top_row > 0)
    {
        top_row--;
        scroll_fraction = (double)top_row / (double)(shown_items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list down one row and moves the scroll thumb to match.
void ShopController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    int __slot1;
    int __slot2;
    if ((int)shown_items.size() - 5 > top_row)
    {
        top_row++;
        scroll_fraction = (double)top_row / (double)(shown_items.size() - 5);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void ShopController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}
