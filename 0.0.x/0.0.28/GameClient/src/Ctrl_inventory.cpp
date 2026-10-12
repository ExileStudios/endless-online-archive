#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

#define DRAG_DELAY_IDLE 1000
#define INVENTORY_CELL_SIZE 26

InventoryController::InventoryController()
{
    left_button_hovered = false;
    right_button_hovered = false;
    paperdoll_button_hovered = false;
    hover_col_end = -1;
    hover_row_end = -1;
    hover_col = -1;
    hover_row = -1;
    hover_message_delay = -1;
    drop_blocked = false;
    has_hover = 0;
    is_dragging = 0;
    dragging_from_paperdoll = false;
    input_enabled = true;
}

InventoryController::~InventoryController()
{
}

// Counts the ticks since the last click and since a drag began; a drag counts as started after its first ticks.
void InventoryController::Tick()
{
    if (click_ticks < 10)
        click_ticks++;
    if (hover_message_delay > 0)
        hover_message_delay--;
    if (drag_delay_ticks < 10 && is_dragging)
    {
        drag_delay_ticks++;
        if (drag_delay_ticks > 1)
            drag_started = 1;
    }
}

// Holds back the hover text for a few ticks, so the message just shown stays in the status bar.
void InventoryController::SuppressHoverMessage()
{
    hover_message_delay = 2;
}

// Empties the inventory.
void InventoryController::Clear()
{
    left_button_hovered = false;
    right_button_hovered = false;
    hover_text = "";
    items.clear();
    trial_backup.clear();
}

// True when one more of the item would fit; the items are put back as they were.
char InventoryController::TestAddItem(int item_id)
{
    char fits = 1;
    input_enabled = false;
    trial_backup.clear();
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
        trial_backup.insert(trial_backup.end(), *it);
    if (!AddItem(item_id, 1))
        fits = 0;
    if (trial_backup.size() > 0)
    {
        items.clear();
        for (vector<InventoryItem>::iterator it = trial_backup.begin(); it != trial_backup.end(); it++)
            items.insert(items.end(), *it);
        trial_backup.clear();
    }
    input_enabled = true;
    return fits;
}

// True when the inventory would hold the trade's result: the own offer taken out, the partner's offer put in.
// The items are put back as they were.
char InventoryController::CanFitTradeItems()
{
    char fits;
    volatile char __hole1;
    volatile char __hole2;
    volatile char __hole3;
    int j = 0;
    int k = 0;
    fits = 1;
    input_enabled = false;
    trial_backup.clear();
    for (vector<InventoryItem>::iterator i = items.begin(); i != items.end(); i++)
        trial_backup.insert(trial_backup.end(), *i);
    for (j = 0; (unsigned int)j < MainForm->animation->trade->own_items.size(); j++)
        RemoveItem(MainForm->animation->trade->own_items[j].item_id, MainForm->animation->trade->own_items[j].amount);
    for (k = 0; (unsigned int)k < MainForm->animation->trade->partner_items.size(); k++)
    {
        if (!AddItem(MainForm->animation->trade->partner_items[k].item_id,
            MainForm->animation->trade->partner_items[k].amount))
        {
            fits = 0;
            break;
        }
    }
    if (trial_backup.size() > 0)
    {
        items.clear();
        for (vector<InventoryItem>::iterator m = trial_backup.begin(); m != trial_backup.end(); m++)
            items.insert(items.end(), *m);
        trial_backup.clear();
    }
    input_enabled = true;
    return fits;
}

bool InventoryController::TestAndClearHoverDirty()
{
    bool was_dirty = 0;
    if (hover_dirty)
    {
        hover_dirty = false;
        was_dirty = 1;
    }
    return was_dirty;
}

bool InventoryController::TestAndClearItemsDirty()
{
    bool was_dirty = 0;
    if (items_dirty)
    {
        items_dirty = false;
        was_dirty = 1;
    }
    return was_dirty;
}

DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(InventoryController)

void InventoryController::MarkHoverDirty()
{
    hover_dirty = true;
}

void InventoryController::MarkItemsDirty()
{
    items_dirty = true;
}

void InventoryController::MarkDragDirty()
{
    drag_dirty = 1;
}

// Uses an item from the inventory: the use request for the usable types, the equip request for equipment.
// Equipment must suit the class, level and stats; a paired type goes to its second slot when the first is taken.
void InventoryController::UseItem(int item_id)
{
    char blocked;
    int item_type = 0;
    item_type = MainForm->item_values->GetType(item_id);
    // This type asks for confirmation before anything is sent
    if (item_type == EifItemType_CureCurse)
    {
        pending_use_item_id = item_id;
        MainForm->animation->game->ShowMessageBox(3, ConfirmAction_UseItem,
            MainForm->strings->GetDialogText(DialogText_RemoveCurse, false),
            MainForm->strings->GetDialogText(DialogText_RemoveCurse, true));
        MainForm->edits->caret_enabled = false;
    }
    if (item_type == EifItemType_EffectPotion)
    {
        if (!MainForm->players->IsEffectPlaying(MainForm->net_client->GetPlayerId()))
            MainForm->net_client->sender->SendUseItem(item_id);
    }
    if (item_type == EifItemType_Heal || item_type == EifItemType_Teleport || item_type == EifItemType_ExpReward || item_type == EifItemType_Alcohol || item_type == EifItemType_HairDye)
    {
        blocked = 0;
        if (item_type == EifItemType_Teleport && MainForm->map->item_use_restricted)
        {
            blocked = 1;
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                MainForm->strings->GetText(GameText_NothingHappened));
            MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
        }
        if (item_type == EifItemType_Teleport && MainForm->map->IsActionBlocked())
        {
            blocked = 1;
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                MainForm->strings->GetText(GameText_NothingHappened));
            MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
        }
        if (!blocked)
            MainForm->net_client->sender->SendUseItem(item_id);
    }
    if (item_type >= EifItemType_Weapon && item_type <= EifItemType_Bracer)
    {
        ItemValue handle = MainForm->item_values->GetItem(item_id);
        if (!MainForm->class_values->IsDescendantOf(MainForm->animation->interface_controller->status->class_id,
            handle.required_class))
        {
            String required_class_name = MainForm->class_values->GetName(handle.required_class);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemCanOnly) + required_class_name);
            SuppressHoverMessage();
            CancelDrag();
            return;
        }
        if (handle.required_level > MainForm->animation->interface_controller->status->level)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemRequires) + "LVL " + IntToStr(handle.required_level));
            SuppressHoverMessage();
            CancelDrag();
            return;
        }
        // The stats the item needs, listed in the refusal
        String stat_requirements = "";
        if (handle.required_str > 0)
            stat_requirements = stat_requirements + IntToStr(handle.required_str) + " STR ";
        if (handle.required_int > 0)
            stat_requirements = stat_requirements + IntToStr(handle.required_int) + " INT ";
        if (handle.required_wis > 0)
            stat_requirements = stat_requirements + IntToStr(handle.required_wis) + " WIS ";
        if (handle.required_agi > 0)
            stat_requirements = stat_requirements + IntToStr(handle.required_agi) + " AGI ";
        if (handle.required_con > 0)
            stat_requirements = stat_requirements + IntToStr(handle.required_con) + " CON ";
        if (handle.required_cha > 0)
            stat_requirements = stat_requirements + IntToStr(handle.required_cha) + " CHA ";
        if (handle.required_str > MainForm->animation->interface_controller->status->stat_str ||
            handle.required_int > MainForm->animation->interface_controller->status->stat_int ||
            handle.required_wis > MainForm->animation->interface_controller->status->stat_wis ||
            handle.required_agi > MainForm->animation->interface_controller->status->stat_agi ||
            handle.required_con > MainForm->animation->interface_controller->status->stat_con ||
            handle.required_cha > MainForm->animation->interface_controller->status->stat_cha)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemRequires) + stat_requirements);
            SuppressHoverMessage();
            CancelDrag();
            return;
        }
        if (item_type == EifItemType_Weapon)
        {
            if (MainForm->animation->paperdoll->equipped_weapon <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Shield)
        {
            if (MainForm->animation->paperdoll->equipped_shield <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Armor)
        {
            if (MainForm->animation->paperdoll->equipped_armor <= 0)
            {
                if (MainForm->item_values->GetGender(item_id) ==
                    MainForm->players->GetGender(MainForm->net_client->GetPlayerId()))
                    MainForm->net_client->sender->SendEquipItem(item_id, 0);
                else
                {
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_ThisArmorDoesNot));
                    SuppressHoverMessage();
                }
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Hat)
        {
            if (MainForm->animation->paperdoll->equipped_head <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Boots)
        {
            if (MainForm->animation->paperdoll->equipped_boots <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Gloves)
        {
            if (MainForm->animation->paperdoll->equipped_gloves <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Accessory)
        {
            if (MainForm->animation->paperdoll->equipped_type16 <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Belt)
        {
            if (MainForm->animation->paperdoll->equipped_belt <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Necklace)
        {
            if (MainForm->animation->paperdoll->equipped_necklace <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Ring)
        {
            if (MainForm->animation->paperdoll->equipped_type19_left <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else if (MainForm->animation->paperdoll->equipped_type19_right <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 1);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Armlet)
        {
            if (MainForm->animation->paperdoll->equipped_type20_left <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else if (MainForm->animation->paperdoll->equipped_type20_right <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 1);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
        if (item_type == EifItemType_Bracer)
        {
            if (MainForm->animation->paperdoll->equipped_type21_left <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 0);
            else if (MainForm->animation->paperdoll->equipped_type21_right <= 0)
                MainForm->net_client->sender->SendEquipItem(item_id, 1);
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
            }
        }
    }
    CancelDrag();
}

// Moves the dragged item and updates the button and cell hovers.
void InventoryController::MouseMove(int x, int y)
{
    if (is_dragging && (x != drag_x || y != drag_y))
    {
        drag_x = x;
        drag_y = y;
        MarkDragDirty();
    }
    UpdateButtonHover(x, y);
    UpdateItemHover(x, y);
}

// Works out the cells under the mouse: the drop target of a dragged item, or the item to name in the status bar.
void InventoryController::UpdateItemHover(int x, int y)
{
    int row;
    int drag_extra_cols;
    int drag_extra_rows;
    int overlap_count;
    int item_extra_cols;
    int item_extra_rows;
    char uncounted;
    char found;
    int extra_rows;
    HDC dc;
    int col = 0;
    int drag_size;
    int item_size;
    int j;
    int k;
    int result;
    int extra_cols;
    // Cells are squares from the grid's top-left corner
    col = (x - 114) / INVENTORY_CELL_SIZE;
    row = (y - 339) / INVENTORY_CELL_SIZE;
    last_mouse_x = x;
    last_mouse_y = y;
    if (row >= 0 && row <= 3 && col >= 0 && col <= 13 && x > 109 && y > 333)
    {
        if (is_dragging)
        {
            drag_size = drag_item_size;
            // A size above 3 is two columns wide; the rest of the size is the extra rows
            if (drag_size > 3)
            {
                drag_extra_cols = 1;
                drag_extra_rows = drag_size - 4;
            }
            else
            {
                drag_extra_cols = 0;
                drag_extra_rows = drag_size;
            }
            if (drag_extra_cols >= 1)
                col = (x - 126) / INVENTORY_CELL_SIZE;
            if (drag_extra_rows >= 1)
                row = (y - 339 - 12 * drag_extra_rows) / INVENTORY_CELL_SIZE;
            if (col + drag_extra_cols > 13)
                col = 13 - drag_extra_cols;
            if (row + drag_extra_rows > 3)
                row = 3 - drag_extra_rows;
            if (col < 0)
                col = 0;
            if (row < 0)
                row = 0;
            overlap_count = 0;
            for (vector<InventoryItem>::iterator i = items.begin(); i != items.end(); i++)
            {
                if (i->item_id == drag_item_id)
                    continue;
                item_size = MainForm->item_values->GetSize(i->item_id);
                if (item_size > 3)
                {
                    item_extra_cols = 1;
                    item_extra_rows = item_size - 4;
                }
                else
                {
                    item_extra_cols = 0;
                    item_extra_rows = item_size;
                }
                uncounted = 1;
                for (j = 0; j <= drag_extra_cols; j++)
                {
                    for (k = 0; k <= drag_extra_rows; k++)
                    {
                        if (col + j >= i->grid_x && col + j <= item_extra_cols + i->grid_x && k + row >= i->grid_y &&
                            k + row <= i->grid_y + item_extra_rows && i->is_visible && uncounted)
                        {
                            overlap_count++;
                            uncounted = 0;
                        }
                    }
                }
            }
            // Dropping on one item swaps it; on two or more the drop is refused
            if (overlap_count < 2)
                drop_blocked = false;
            else
                drop_blocked = true;
            hover_col = col;
            hover_row = row;
            hover_col_end = col + drag_extra_cols;
            hover_row_end = row + drag_extra_rows;
            hover_text_width = 0;
            has_hover = 1;
            hover_dirty = true;
            items_dirty = true;
        }
        else
        {
            found = 0;
            for (vector<InventoryItem>::iterator m = items.begin(); m != items.end(); m++)
            {
                result = MainForm->item_values->GetSize(m->item_id);
                if (result > 3)
                {
                    extra_cols = 1;
                    extra_rows = result - 4;
                }
                else
                {
                    extra_cols = 0;
                    extra_rows = result;
                }
                if (col >= m->grid_x && col <= extra_cols + m->grid_x && m->grid_y <= row &&
                    m->grid_y + extra_rows >= row && m->is_visible)
                {
                    found = 1;
                    if (hover_col != m->grid_x || hover_row != m->grid_y)
                    {
                        hover_item_size = result;
                        hover_item_id = m->item_id;
                        hover_col = m->grid_x;
                        hover_row = m->grid_y;
                        hover_col_end = m->grid_x + extra_cols;
                        hover_row_end = m->grid_y + extra_rows;
                        if (m->amount > 1)
                        {
                            // Gold shows its amount first, other stacks show it after the name
                            if (m->item_id == 1)
                                hover_text = IntToStr(m->amount) + " " + MainForm->item_values->GetName(m->item_id);
                            else
                                hover_text = MainForm->item_values->GetName(m->item_id) + " x" + IntToStr(m->amount);
                        }
                        else if (m->item_id == 1)
                            hover_text = IntToStr(m->amount) + " " + MainForm->item_values->GetName(m->item_id);
                        else
                            hover_text = MainForm->item_values->GetName(m->item_id);
                        if (hover_message_delay < 1)
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Item,
                                hover_text);
                        hover_item_special = MainForm->item_values->GetSpecial(m->item_id);
                        drop_blocked = false;
                        has_hover = 1;
                        hover_dirty = true;
                        items_dirty = true;
                        hover_text_width = 0;
                        if (MainForm->OffscreenSurface->GetDC(&dc))
                        {
                            try
                            {
                                MainForm->text_canvas->Handle = dc;
                                hover_text_width = MainForm->text_canvas->TextWidth(hover_text);
                                MainForm->text_canvas->Handle = 0;
                            }
                            catch (Exception &)
                            {
                            }
                            result = MainForm->OffscreenSurface->ReleaseDC(dc);
                        }
                    }
                    break;
                }
            }
            if (!found)
            {
                if (has_hover)
                {
                    hover_dirty = true;
                    items_dirty = true;
                    hover_col = -1;
                    hover_row = -1;
                    hover_item_id = -1;
                    hover_text = "";
                }
                drop_blocked = false;
                has_hover = 0;
            }
        }
    }
    else
    {
        if (has_hover)
        {
            hover_dirty = true;
            items_dirty = true;
            hover_col = -1;
            hover_row = -1;
            hover_item_id = -1;
            hover_text = "";
        }
        drop_blocked = false;
        has_hover = 0;
    }
}

// A click in the inventory: the paperdoll button, the drop and junk buttons, picking up, placing or swapping an item.
// While an item from the paperdoll is dragged, a click in the grid asks to unequip it, another button puts it back.
void InventoryController::MouseDown(int x, int y, int button)
{
    if (!dragging_from_paperdoll)
    {
        char can_click = 1;
        if (y >= 338 && y <= 358 && x >= 486 && x <= 575 && !MainForm->animation->paperdoll->is_visible)
        {
            if (MainForm->players->HasPlayer(MainForm->net_client->GetPlayerId()))
            {
                can_click = 0;
                MainForm->sfx->PlaySfx(2);
                MainForm->net_client->sender->RequestPaperdoll(MainForm->net_client->GetPlayerId());
            }
        }
        else
        {
            // The drop button: a stack asks for the amount first
            if (is_dragging && x >= 492 && x <= 530 && y >= 398 && y <= 436)
            {
                char can_drop = 1;
                if (MainForm->item_values->GetSpecial(drag_item_id) == ItemSpecial_Bound)
                {
                    can_drop = 0;
                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                    MainForm->edits->caret_enabled = false;
                }
                if (can_drop && MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
                {
                    can_drop = 0;
                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                        MainForm->strings->GetText(GameText_Warning),
                        MainForm->strings->GetText(GameText_YouCannotDropItems));
                    MainForm->edits->caret_enabled = false;
                }
                if (can_drop)
                {
                    if (GetItemAmount(drag_item_id) < 2)
                    {
                        if (drag_item_id == 1)
                        {
                            if (!GetItemAmount(drag_item_id))
                            {
                                String gold_name = MainForm->item_values->GetName(1);
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                    MainForm->strings->GetText(GameText_YouHaveNoMore) + " " + gold_name);
                            }
                            else
                            {
                                MainForm->net_client->sender->SendDropItem(drag_item_id, 1, -1, -1);
                            }
                        }
                        else
                        {
                            MainForm->net_client->sender->SendDropItem(drag_item_id, 1, -1, -1);
                        }
                    }
                    else
                    {
                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_DropItem, GetItemAmount(drag_item_id), drag_item_id, 0);
                        input_enabled = false;
                    }
                }
                CancelDrag();
                UpdateItemHover(x, y);
            }
            // The junk button: a stack asks for the amount first
            if (x >= 533 && x <= 571 && y >= 398 && y <= 436)
            {
                if (GetItemAmount(drag_item_id) < 2)
                {
                    if (drag_item_id == 1)
                    {
                        if (!GetItemAmount(drag_item_id))
                        {
                            String junk_gold_name = MainForm->item_values->GetName(1);
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                MainForm->strings->GetText(GameText_YouHaveNoMore) + " " + junk_gold_name);
                        }
                        else
                        {
                            MainForm->net_client->sender->SendJunkItem(drag_item_id, 1);
                        }
                    }
                    else
                    {
                        MainForm->net_client->sender->SendJunkItem(drag_item_id, 1);
                    }
                }
                else
                {
                    MainForm->animation->game->OpenAmountBox(AmountBoxKind_JunkItem, GetItemAmount(drag_item_id), drag_item_id, 0);
                    input_enabled = false;
                }
                CancelDrag();
                UpdateItemHover(x, y);
            }
        }
        if (input_enabled)
        {
            if (can_click)
            {
                char cancel_drag = 0;
                int col = (x - 114) / INVENTORY_CELL_SIZE;
                int row = (y - 339) / INVENTORY_CELL_SIZE;
                if (!button)
                {
                    if (!(row < 0 || row > 3 || col < 0 || col > 13 || x <= 109 || y <= 333))
                    {
                        if (MainForm->animation->game->amount_box_visible)
                        {
                            MainForm->animation->game->amount_box_visible = false;
                            MainForm->edits->RemoveEdit(13);
                            MainForm->edits->FocusEdit(EditId_Chat);
                        }
                        if (this->is_dragging)
                        {
                            // A second click on the same cell soon after the first uses the item
                            if (this->click_ticks < 3 && col == this->click_col && this->click_row == row)
                                this->UseItem(this->drag_item_id);
                            else
                            {
                                int drag_extra_cols;
                                int drag_extra_rows;
                                if (this->drag_item_size > ITEM_SIZE_MAX_NARROW)
                                {
                                    drag_extra_cols = 1;
                                    drag_extra_rows = this->drag_item_size - 4;
                                }
                                else
                                {
                                    drag_extra_cols = 0;
                                    drag_extra_rows = this->drag_item_size;
                                }
                                if (drag_extra_cols >= 1)
                                    col = (x - 126) / INVENTORY_CELL_SIZE;
                                if (drag_extra_rows >= 1)
                                    row = (y - 339 - 12 * drag_extra_rows) / INVENTORY_CELL_SIZE;
                                if (col + drag_extra_cols > 13)
                                    col = 13 - drag_extra_cols;
                                if (row + drag_extra_rows > 3)
                                    row = 3 - drag_extra_rows;
                                if (col < 0)
                                    col = 0;
                                if (row < 0)
                                    row = 0;
                                vector<InventoryItem>::iterator it;
                                int overlap_count = 0;
                                int overlapped_item_id = -1;
                                for (it = this->items.begin(); it != this->items.end(); it++)
                                {
                                    if (this->drag_item_id == it->item_id)
                                        continue;
                                    int item_extra_cols;
                                    int item_extra_rows;
                                    int placed_item_size = MainForm->item_values->GetSize(it->item_id);
                                    if (placed_item_size > 3)
                                    {
                                        item_extra_cols = 1;
                                        item_extra_rows = placed_item_size - 4;
                                    }
                                    else
                                    {
                                        item_extra_cols = 0;
                                        item_extra_rows = placed_item_size;
                                    }
                                    char not_counted = 1;
                                    for (int j = 0; j <= drag_extra_cols; j++)
                                    {
                                        for (int k = 0; k <= drag_extra_rows; k++)
                                        {
                                            if (col + j >= it->grid_x && col + j <= it->grid_x + item_extra_cols &&
                                                k + row >= it->grid_y && k + row <= it->grid_y + item_extra_rows &&
                                                it->is_visible && not_counted)
                                            {
                                                overlap_count++;
                                                overlapped_item_id = it->item_id;
                                                not_counted = 0;
                                            }
                                        }
                                    }
                                }
                                if (!overlap_count)
                                {
                                    vector<InventoryItem>::iterator it;
                                    MainForm->sfx->PlaySfx(21);
                                    this->layout_backup.clear();
                                    for (it = this->items.begin(); it != this->items.end(); it++)
                                    {
                                        if (it->item_id == this->drag_item_id)
                                        {
                                            it->grid_x = col;
                                            it->grid_y = row;
                                            it->is_placed = 1;
                                            it->is_visible = 1;
                                            this->RecordLayoutChange(this->drag_item_id, col, row);
                                            break;
                                        }
                                    }
                                    this->SaveLayoutChanges();
                                    this->is_dragging = 0;
                                    this->drag_started = 0;
                                    this->drag_delay_ticks = DRAG_DELAY_IDLE;
                                    this->hover_col = -1;
                                    this->hover_row = -1;
                                    this->items_dirty = true;
                                    this->hover_dirty = true;
                                    MainForm->dragging = false;
                                    if (MainForm->animation->trade->is_visible)
                                    {
                                        if (MainForm->animation->trade->trade_locked)
                                        {
                                            if (!this->CanFitTradeItems())
                                            {
                                                MainForm->animation->trade->own_agreed = false;
                                                MainForm->animation->trade->own_status_text = MainForm->strings->GetText(GameText_Trading);
                                                MainForm->net_client->sender->SendTradeDisagree();
                                                MainForm->animation->trade->trade_locked = false;
                                                MainForm->redraw_needed = true;
                                            }
                                        }
                                    }
                                }
                                // The item under the drop is picked up in turn, so the two swap
                                if (overlap_count == 1)
                                {
                                    int held_item_id;
                                    int dropped_item_id;
                                    int dropped_col;
                                    int dropped_row;
                                    int picked_item_id;
                                    int picked_col;
                                    int picked_row;
                                    vector<InventoryItem>::iterator it;
                                    MainForm->sfx->PlaySfx(20);
                                    held_item_id = this->drag_item_id;
                                    for (it = this->items.begin(); it != this->items.end(); it++)
                                    {
                                        if (it->item_id == overlapped_item_id)
                                        {
                                            this->hover_item_id = it->item_id;
                                            this->hover_item_size = MainForm->item_values->GetSize(it->item_id);
                                            this->drag_x = x;
                                            this->drag_y = y;
                                            this->drag_item_id = it->item_id;
                                            this->drag_item_size = MainForm->item_values->GetSize(it->item_id);
                                            this->drag_graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
                                            picked_item_id = overlapped_item_id;
                                            picked_col = it->grid_x;
                                            picked_row = it->grid_y;
                                            it->is_visible = 0;
                                        }
                                        if (it->item_id == held_item_id)
                                        {
                                            it->grid_x = col;
                                            it->grid_y = row;
                                            dropped_item_id = held_item_id;
                                            dropped_col = col;
                                            dropped_row = row;
                                            it->is_placed = 1;
                                            it->is_visible = 1;
                                        }
                                    }
                                    this->RecordLayoutChange(dropped_item_id, dropped_col, dropped_row);
                                    this->RecordLayoutChange(picked_item_id, picked_col, picked_row);
                                    this->drag_started = 0;
                                    this->drag_delay_ticks = DRAG_DELAY_IDLE;
                                    this->items_dirty = true;
                                    this->hover_dirty = true;
                                }
                            }
                        }
                        else
                        {
                            vector<InventoryItem>::iterator it;
                            this->click_ticks = 0;
                            this->click_col = col;
                            this->click_row = row;
                            this->BackupLayout();
                            this->ResetLayoutChanges();
                            for (it = this->items.begin(); it != this->items.end(); it++)
                            {
                                int picked_extra_cols;
                                int picked_extra_rows;
                                int picked_item_size = MainForm->item_values->GetSize(it->item_id);
                                if (picked_item_size > 3)
                                {
                                    picked_extra_cols = 1;
                                    picked_extra_rows = picked_item_size - 4;
                                }
                                else
                                {
                                    picked_extra_cols = 0;
                                    picked_extra_rows = picked_item_size;
                                }
                                if (col >= it->grid_x && col <= it->grid_x + picked_extra_cols && it->grid_y <= row &&
                                    it->grid_y + picked_extra_rows >= row)
                                {
                                    MainForm->sfx->PlaySfx(20);
                                    this->RecordLayoutChange(it->item_id, it->grid_x, it->grid_y);
                                    this->hover_item_size = picked_item_size;
                                    this->hover_item_id = it->item_id;
                                    this->is_dragging = 1;
                                    this->drag_started = 0;
                                    this->drag_delay_ticks = 0;
                                    this->drag_x = x;
                                    this->drag_y = y;
                                    this->drag_item_size = picked_item_size;
                                    this->drag_item_id = it->item_id;
                                    this->drag_graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
                                    it->is_visible = 0;
                                    this->items_dirty = true;
                                    this->hover_dirty = true;
                                    MainForm->dragging = true;
                                }
                            }
                        }
                    }
                    else
                        cancel_drag = 1;
                }
                else
                    cancel_drag = 1;
                if (cancel_drag && is_dragging)
                {
                    CancelDrag();
                    UpdateItemHover(x, y);
                }
            }
        }
    }
    else
    {
        if (!button || drag_started)
        {
            int cell_col = (x - 114) / INVENTORY_CELL_SIZE;
            int cell_row = (y - 339) / INVENTORY_CELL_SIZE;
            if (cell_row >= 0 && cell_row <= 3 && cell_col >= 0 && cell_col <= 13 && x > 109 && y > 333)
            {
                if (MainForm->animation->interface_controller->inventory->HasRoomForItem(drag_item_id))
                {
                    if (MainForm->item_values->GetSpecial(drag_item_id) == ItemSpecial_Cursed)
                    {
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_CursedItem, false),
                            MainForm->strings->GetDialogText(DialogText_CursedItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    else
                        MainForm->net_client->sender->SendUnequipItem(drag_item_id, paperdoll_sub_slot);
                }
                else
                {
                    dragging_from_paperdoll = false;
                    CancelDrag();
                    MainForm->redraw_needed = true;
                    UpdateItemHover(x, y);
                }
            }
        }
        // Any other button puts the dragged paperdoll item's picture back in its box
        if (button == 1)
        {
            int item_type = MainForm->item_values->GetType(this->drag_item_id);
            if (item_type == EifItemType_Weapon)
                MainForm->animation->paperdoll->weapon_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Shield)
                MainForm->animation->paperdoll->shield_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Armor)
                MainForm->animation->paperdoll->armor_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Hat)
                MainForm->animation->paperdoll->head_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Boots)
                MainForm->animation->paperdoll->boots_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Gloves)
                MainForm->animation->paperdoll->gloves_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Accessory)
                MainForm->animation->paperdoll->type16_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Belt)
                MainForm->animation->paperdoll->belt_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Necklace)
                MainForm->animation->paperdoll->necklace_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Ring)
            {
                if (!MainForm->animation->paperdoll->type19_right_graphic)
                    MainForm->animation->paperdoll->type19_right_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->type19_left_graphic = paperdoll_slot_graphic;
            }
            if (item_type == EifItemType_Armlet)
            {
                if (!MainForm->animation->paperdoll->type20_right_graphic)
                    MainForm->animation->paperdoll->type20_right_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->type20_left_graphic = paperdoll_slot_graphic;
            }
            if (item_type == EifItemType_Bracer)
                if (!MainForm->animation->paperdoll->type21_right_graphic)
                    MainForm->animation->paperdoll->type21_right_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->type21_right_graphic = paperdoll_slot_graphic;
            MainForm->redraw_needed = true;
            dragging_from_paperdoll = false;
            CancelDrag();
            UpdateItemHover(x, y);
        }
    }
}

// Ends a drag and puts the items back where they were when it began.
void InventoryController::CancelDrag()
{
    is_dragging = 0;
    drag_started = 0;
    drag_delay_ticks = 0;
    RevertDragLayout();
    hover_col = -1;
    hover_row = -1;
    items_dirty = true;
    hover_dirty = true;
    MainForm->dragging = false;
}

// Keeps a copy of the items, to put back if the drag is cancelled.
void InventoryController::BackupLayout()
{
    layout_backup.clear();
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
        layout_backup.insert(layout_backup.end(), *it);
}

// Puts the items back from the copy, if there is one.
void InventoryController::RestoreLayoutBackup()
{
    if (layout_backup.size() > 0)
    {
        items.clear();
        for (vector<InventoryItem>::iterator it = layout_backup.begin(); it != layout_backup.end(); it++)
            items.insert(items.end(), *it);
        layout_backup.clear();
        input_enabled = true;
    }
}

// Puts the items back from the copy, if there is one, and plays the drop sound.
void InventoryController::RevertDragLayout()
{
    if (layout_backup.size() > 0)
    {
        items.clear();
        for (vector<InventoryItem>::iterator it = layout_backup.begin(); it != layout_backup.end(); it++)
            items.insert(items.end(), *it);
        layout_backup.clear();
        input_enabled = true;
    }
    MainForm->sfx->PlaySfx(21);
}

// Releasing the button after a drag has started places the item where the mouse is.
void InventoryController::MouseUp()
{
    if (drag_started && is_dragging)
    {
        MouseDown(last_mouse_x, last_mouse_y, 0);
        drag_delay_ticks = DRAG_DELAY_IDLE;
        drag_started = 0;
    }
    else
    {
        drag_delay_ticks = DRAG_DELAY_IDLE;
        drag_started = 0;
    }
}

// Tracks the hover over the paperdoll, drop and junk buttons, naming each in the status bar; true on a change.
bool InventoryController::UpdateButtonHover(int x, int y)
{
    bool result = 0;
    if (y >= 338 && y <= 358 && !MainForm->animation->paperdoll->is_visible && !dragging_from_paperdoll)
    {
        if (x >= 486 && x <= 575)
        {
            if (!paperdoll_button_hovered)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickHereToShow));
                paperdoll_button_hovered = true;
                result = 1;
            }
        }
        else if (paperdoll_button_hovered)
        {
            paperdoll_button_hovered = false;
            result = 1;
        }
    }
    else if (paperdoll_button_hovered)
    {
        paperdoll_button_hovered = false;
        result = 1;
    }
    if (y >= 398 && y <= 438 && !dragging_from_paperdoll)
    {
        if (x >= 491 && x <= 529)
        {
            if (!left_button_hovered)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_DragItemToThis));
                left_button_hovered = true;
                result = 1;
            }
        }
        else if (left_button_hovered)
        {
            left_button_hovered = false;
            result = 1;
        }
        if (x >= 533 && x <= 571)
        {
            if (!right_button_hovered)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_DragItemToThisButton));
                right_button_hovered = true;
                result = 1;
            }
        }
        else if (right_button_hovered)
        {
            right_button_hovered = false;
            result = 1;
        }
    }
    else if (left_button_hovered || right_button_hovered)
    {
        left_button_hovered = false;
        result = 1;
        right_button_hovered = false;
    }
    if (result)
        hover_dirty = true;
    return result;
}

void InventoryController::ResetLayoutChanges()
{
    change_count = 0;
    changes_overflowed = false;
}

// Notes an item's cell for SaveLayoutChanges; past the list's size only the overflow is noted.
void InventoryController::RecordLayoutChange(int item_id, int col, int row)
{
    if (change_count > 17)
        changes_overflowed = true;
    else
    {
        change_item_ids[change_count] = item_id; change_cols[change_count] = col; change_rows[change_count] = row;
        change_count++;
    }
}

// Writes every cell of the grid to the registry, with the item in it or 0.
void InventoryController::SaveLayout()
{
    int j;
    int slot_item_id;
    for (int i = 0; i < 4; i++)
    {
        for (j = 0; j < 14; j++)
        {
            slot_item_id = 0;
            for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                {
                    slot_item_id = it->item_id;
                    break;
                }
            }
            MainForm->registry->WriteSlotValue(1, 14 * i + j, slot_item_id);
        }
    }
}

// Writes the moved items to the registry: the even changes clear the cells left, the odd ones store the new cells.
// After an overflow the whole grid is written instead.
void InventoryController::SaveLayoutChanges()
{
    char first;
    volatile char __hole1;
    volatile char __hole2;
    volatile char __hole3;
    int slot;
    int change = 0;
    if (!changes_overflowed)
    {
        first = 1;
        change = 0;
        while (change < change_count)
        {
            int slot_value = 0;
            char should_write = 1;
            slot = change_cols[change] + 14 * change_rows[change];
            if (!first)
            {
                if (!(change % 2))
                {
                    if (change_cols[change] == change_cols[change - 1] &&
                        change_rows[change] == change_rows[change - 1])
                        should_write = 0;
                }
                else
                    slot_value = change_item_ids[change];
            }
            first = 0;
            if (should_write)
                MainForm->registry->WriteSlotValue(1, slot, slot_value);
            change++;
        }
    }
    else
        SaveLayout();
    ResetLayoutChanges();
}

// True when a key item for the lock is in the inventory.
char InventoryController::HasKey(int key_id)
{
    char found;
    int item_id;
    found = 0;
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        item_id = it->item_id;
        if (MainForm->item_values->GetType(item_id) == 9 && MainForm->item_values->GetSpecValue(item_id) == key_id)
        {
            found = 1;
            break;
        }
    }
    return found;
}

// True when the item is already held or a free place in the grid fits its size.
char InventoryController::HasRoomForItem(int item_id)
{
    char has_room;
    int extra_cols;
    int extra_rows;
    int j;
    int overlap_count;
    int other_extra_cols;
    int other_extra_rows;
    char uncounted;
    int item_size;
    int k;
    int other_size;
    int n;
    int ii;
    vector<InventoryItem>::iterator it;
    has_room = 0;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (item_id == it->item_id)
        {
            has_room = 1;
            break;
        }
    }
    if (!has_room)
    {
        item_size = MainForm->item_values->GetSize(item_id);
        if (item_size > 3)
        {
            extra_cols = 1;
            extra_rows = item_size - 4;
        }
        else
        {
            extra_cols = 0;
            extra_rows = item_size;
        }
        for (j = 0; 4 - extra_rows > j && !has_room; j++)
        {
            for (k = 0; k < 14 - extra_cols && !has_room; k++)
            {
                vector<InventoryItem>::iterator it;
                overlap_count = 0;
                for (it = items.begin(); it != items.end(); it++)
                {
                    other_size = MainForm->item_values->GetSize(it->item_id);
                    if (other_size > 3)
                    {
                        other_extra_cols = 1;
                        other_extra_rows = other_size - 4;
                    }
                    else
                    {
                        other_extra_cols = 0;
                        other_extra_rows = other_size;
                    }
                    uncounted = 1;
                    for (n = 0; n <= extra_cols; n++)
                    {
                        for (ii = 0; ii <= extra_rows; ii++)
                        {
                            if (k + n >= it->grid_x && k + n <= it->grid_x + other_extra_cols && ii + j >= it->grid_y &&
                                ii + j <= it->grid_y + other_extra_rows && uncounted)
                            {
                                overlap_count++;
                                uncounted = 0;
                            }
                        }
                    }
                }
                if (!overlap_count)
                {
                    has_room = 1;
                    break;
                }
            }
        }
    }
    return has_room;
}

// Adds to a held stack, or places the item at the first free place for its size; false when nothing fits.
// A newly placed item's cell is saved in the registry.
bool InventoryController::AddItem(int item_id, int amount)
{
    bool placed;
    int extra_cols;
    int extra_rows;
    int free_col;
    int free_row;
    int j;
    int overlap_count;
    int other_extra_cols;
    int other_extra_rows;
    char uncounted;
    char stacked = 0;
    int item_size;
    int k = 0;
    int other_size;
    int n;
    int ii;
    placed = 1;
    stacked = 0;
    for (vector<InventoryItem>::iterator i = items.begin(); i != items.end(); i++)
    {
        if (i->item_id == item_id)
        {
            i->amount += amount;
            items_dirty = true;
            hover_dirty = true;
            stacked = 1;
            break;
        }
    }
    if (!stacked)
    {
        placed = 0;
        item_size = MainForm->item_values->GetSize(item_id);
        if (item_size > 3)
        {
            extra_cols = 1;
            extra_rows = item_size - 4;
        }
        else
        {
            extra_cols = 0;
            extra_rows = item_size;
        }
        for (j = 0; 4 - extra_rows > j && !placed; j++)
        {
            for (k = 0; k < 14 - extra_cols && !placed; k++)
            {
                overlap_count = 0;
                for (vector<InventoryItem>::iterator m = items.begin(); m != items.end(); m++)
                {
                    other_size = MainForm->item_values->GetSize(m->item_id);
                    if (other_size > 3)
                    {
                        other_extra_cols = 1;
                        other_extra_rows = other_size - 4;
                    }
                    else
                    {
                        other_extra_cols = 0;
                        other_extra_rows = other_size;
                    }
                    uncounted = 1;
                    for (n = 0; n <= extra_cols; n++)
                    {
                        for (ii = 0; ii <= extra_rows; ii++)
                        {
                            if (k + n >= m->grid_x && k + n <= m->grid_x + other_extra_cols && ii + j >= m->grid_y &&
                                ii + j <= m->grid_y + other_extra_rows && uncounted)
                            {
                                overlap_count++;
                                uncounted = 0;
                            }
                        }
                    }
                }
                if (!overlap_count)
                {
                    free_col = k;
                    free_row = j;
                    placed = 1;
                }
            }
        }
        if (placed)
        {
            InventoryItem handle(item_id);
            handle.grid_x = free_col;
            handle.grid_y = free_row;
            handle.amount = amount;
            items.insert(items.end(), handle);
            int value = 14 * free_row + free_col;
            MainForm->registry->WriteSlotValue(1, value, item_id);
            hover_dirty = true;
            items_dirty = true;
        }
    }
    return placed;
}

// Takes an amount off the item; an item other than gold that runs out leaves the grid and its saved cell.
char InventoryController::RemoveItem(int item_id, int amount)
{
    vector<InventoryItem>::iterator it;
    char result = 1;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (item_id == it->item_id)
        {
            it->amount -= amount;
            if (it->item_id == 1)
            {
                if (it->amount <= 0)
                    it->amount = 0;
            }
            else if (it->amount <= 0)
            {
                int slot = 14 * it->grid_y + it->grid_x;
                MainForm->registry->WriteSlotValue(1, slot, 0);
                items.erase(it);
            }
            hover_dirty = true;
            items_dirty = true;
            break;
        }
    }
    return result;
}

// The weight of everything held and equipped: each stack counts its amount, each equipped item once.
int InventoryController::GetTotalWeight()
{
    unsigned int items_weight = 0;
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
        items_weight += it->amount * MainForm->item_values->GetWeight(it->item_id);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_head);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_necklace);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_armor);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_weapon);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_shield);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_belt);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_gloves);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_boots);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type19_left);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type19_right);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type20_left);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type20_right);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type21_left);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type21_right);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_type16);
    return items_weight;
}

// Sets the item's amount; an item other than gold at 0 leaves the grid and its saved cell.
void InventoryController::SetItemAmount(int item_id, int amount)
{
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        if (item_id == it->item_id)
        {
            it->amount = amount;
            if (it->item_id == 1)
            {
                if (it->amount <= 0)
                    it->amount = 0;
            }
            else if (it->amount <= 0)
            {
                int slot = 14 * it->grid_y + it->grid_x;
                MainForm->registry->WriteSlotValue(1, slot, 0);
                items.erase(it);
            }
            hover_dirty = true;
            items_dirty = true;
            break;
        }
    }
}

// The amount held of the item, or 0.
DEFINE_GET_ITEM_AMOUNT(InventoryController)

// Keeps an item of the character's inventory until PlaceLoadedItems places it.
void InventoryController::AddLoadedItem(int item_id, int amount)
{
    InventoryItem handle(item_id);
    handle.amount = amount;
    handle.is_placed = 0;
    loaded_items.insert(loaded_items.end(), handle);
}

// Places the loaded items at the cells saved in the registry; the rest go to the first free places.
void InventoryController::PlaceLoadedItems()
{
    saved_slots.clear();
    for (int i = 0; i < 56; i++)
    {
        int slot_item_id = MainForm->registry->ReadSlotValue(1, i);
        if (slot_item_id > 0)
        {
            int cell_col = i % 14;
            int cell_row = i / 14;
            InventoryItem handle(slot_item_id);
            handle.grid_y = cell_row;
            handle.grid_x = cell_col;
            saved_slots.insert(saved_slots.end(), handle);
        }
    }
    vector<InventoryItem>::iterator it;
    for (it = loaded_items.begin(); it != loaded_items.end(); it++)
    {
        char matched = 0;
        for (vector<InventoryItem>::iterator it2 = saved_slots.begin(); it2 != saved_slots.end() && !matched; it2++)
        {
            if (it2->item_id != it->item_id)
                continue;
            if (it2->item_id == 1)
                MainForm->animation->interface_controller->status->gold = it->amount;
            it->is_placed = 1;
            matched = 1;
            InventoryItem placed_item(it->item_id);
            placed_item.grid_x = it2->grid_x;
            placed_item.grid_y = it2->grid_y;
            placed_item.amount = it->amount;
            items.insert(items.end(), placed_item);
            int restored_graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
            MainForm->gfx->items->RequestSubItemGraphic(restored_graphic);
        }
    }
    for (it = loaded_items.begin(); it != loaded_items.end(); it++)
    {
        if (it->is_placed)
            continue;
        if (!AddItem(it->item_id, it->amount))
            continue;
        it->is_placed = 1;
        int pending_graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
        MainForm->gfx->items->RequestSubItemGraphic(pending_graphic);
    }
    input_enabled = true;
    layout_backup.clear();
}

// Places the loaded items that found no place before, where they fit now.
void InventoryController::PlaceRemainingItems()
{
    for (vector<InventoryItem>::iterator it = loaded_items.begin(); it != loaded_items.end(); it++)
    {
        if (it->is_placed)
            continue;
        if (AddItem(it->item_id, it->amount))
        {
            it->is_placed = 1;
            int graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
            MainForm->gfx->items->RequestSubItemGraphic(graphic);
        }
    }
}
