#include <vcl.h>
#pragma hdrstop

#include "Ctrl_inventory.h"
#include "Tmainform.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#include "Ctrl_locker.h"
#include "Ctrl_tooltip.h"
#include "Ctrl_trade.h"
#include "Gfx_mapitems.h"
#include "Ctrl_notification.h"
#include "Ctrl_spells.h"

#pragma package(smart_init)

InventoryController::InventoryController()
{
    left_button_hovered = false;
    right_button_hovered = false;
    hover_col_end = -1;
    hover_row_end = -1;
    hover_col = -1;
    hover_row = -1;
    hover_message_delay = -1;
    drop_blocked = false;
    has_hover = false;
    is_dragging = false;
    dragging_from_paperdoll = false;
    input_enabled = true;
    equip_pending = false;
    hotswap_supported = false;
    tooltip = new Tooltip(145, 342, 2);
    tooltip->SetFlipEdge(549, 36);
    tooltip->SetBottomLimit(446, 10);
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
            drag_started = true;
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
    items.clear();
    trial_backup.clear();
}

// True when one more of the item would fit; the items are put back as they were.
char InventoryController::TestAddItem(int item_id)
{
    char fits = true;
    input_enabled = false;
    trial_backup.clear();
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
        trial_backup.insert(trial_backup.end(), *it);
    if (!AddItemQuietly(item_id, 1))
        fits = false;
    if (trial_backup.size() > 0)
    {
        items.clear();
        for (vector<InventoryItem>::iterator saved = trial_backup.begin(); saved != trial_backup.end(); saved++)
            items.insert(items.end(), *saved);
        trial_backup.clear();
    }
    input_enabled = true;
    return fits;
}

// True when the inventory would hold the trade's result: the own offer taken out, the partner's offer put in.
// The items are put back as they were.
char InventoryController::CanFitTradeItems()
{
    int j;
    int k;
    char fits = true;
    input_enabled = false;
    // A trial run: the own offer goes out and the partner's comes in, then the inventory is put back.
    trial_backup.clear();
    for (vector<InventoryItem>::iterator i = items.begin(); i != items.end(); i++)
        trial_backup.insert(trial_backup.end(), *i);
    for (j = 0; j < MainForm->animation->trade->own_items.size(); j++)
        RemoveItem(MainForm->animation->trade->own_items[j].item_id, MainForm->animation->trade->own_items[j].amount);
    for (k = 0; k < MainForm->animation->trade->partner_items.size(); k++)
    {
        if (!AddItemQuietly(MainForm->animation->trade->partner_items[k].item_id,
            MainForm->animation->trade->partner_items[k].amount))
        {
            fits = false;
            break;
        }
    }
    if (trial_backup.size() > 0)
    {
        vector<InventoryItem> *item_list = &items;
        item_list->clear();
        for (vector<InventoryItem>::iterator m = trial_backup.begin(); m != trial_backup.end(); m++)
            items.insert(items.end(), *m);
        vector<InventoryItem> *backup_list = &trial_backup;
        backup_list->clear();
    }
    input_enabled = true;
    return fits;
}

bool InventoryController::TestAndClearHoverDirty()
{
    bool was_dirty = false;
    if (hover_dirty)
    {
        hover_dirty = false;
        was_dirty = true;
    }
    return was_dirty;
}

bool InventoryController::TestAndClearItemsDirty()
{
    bool was_dirty = false;
    if (items_dirty)
    {
        items_dirty = false;
        was_dirty = true;
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
    drag_dirty = true;
}

// Uses or equips an inventory item after the checks its type needs: a full health or mana bar, a known skill,
// a map that forbids it, or the class, level and stats the item asks for. A refusal says why.
int InventoryController::UseItem(int item_id, int category)
{
    ItemValue *item;
    char use_not_needed = false;
    char heals_nothing;
    ItemValue *equipment;
    if (item_id <= 0)
        return -400;
    if ((unsigned int)MainForm->item_values->Count() < item_id)
        return -400;
    item = MainForm->item_values->GetItem(item_id - 1);
    if (!HasItem(item_id))
    {
        MainForm->animation->notification->NotifyWithStatus(NotifyKind_Item, item->item_id, NotifyStatus_NotHeld);
        MainForm->sfx->PlaySfx(SfxSound_Denied);
        return -404;
    }
    // A title certificate asks for the title in an input box.
    if (item->type == EifItemType_TitleCertificate)
    {
        pending_use_item_id = item_id;
        MainForm->sfx->PlaySfx(SfxSound_Send);
        MainForm->animation->game->OpenInputBox(InputBoxKind_ChooseTitle,
            "Congratulations, by which title do you want to be known?", EditCharFilter_Printable, 12);
    }
    if (item->type == EifItemType_CureCurse)
    {
        pending_use_item_id = item_id;
        MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_UseItem,
            MainForm->strings->GetDialogText(DialogText_RemoveCurse, false),
            MainForm->strings->GetDialogText(DialogText_RemoveCurse, true));
        MainForm->edits->caret_enabled = false;
        MainForm->animation->notification->NotifyWithStatus(NotifyKind_Item, item->item_id, NotifyStatus_Ready);
        return FinishUse(item->type);
    }
    // An effect item waits until the player's own effect ends.
    if (item->type == EifItemType_EffectPotion)
    {
        if (!MainForm->players->IsEffectPlaying(MainForm->net_client->GetPlayerId()))
            MainForm->net_client->sender->SendUseItem(item_id);
        MainForm->animation->notification->NotifyWithStatus(NotifyKind_Item, item->item_id, NotifyStatus_Ready);
        return FinishUse(item->type);
    }
    // A visual document opens in its window.
    if (item->type == EifItemType_VisualDocument)
        return FinishUse(item->type);
    if ((item->type >= EifItemType_Heal && item->type <= EifItemType_SkillBook) || item->type == EifItemType_Alcohol ||
        item->type == EifItemType_HairDye)
    {
        use_not_needed = false;
        heals_nothing = false;
        if (item->type == EifItemType_Heal)
        {
            if (item->hp <= 0 && item->tp <= 0)
                heals_nothing = true;
            else if (item->hp > 0 && item->tp <= 0)
            {
                if (MainForm->animation->interface_controller->status->health >=
                    MainForm->animation->interface_controller->status->max_health)
                    use_not_needed = true;
            }
            else if (item->tp > 0 && item->hp <= 0)
            {
                if (MainForm->animation->interface_controller->status->mana >=
                    MainForm->animation->interface_controller->status->max_mana)
                    use_not_needed = true;
            }
            else if (MainForm->animation->interface_controller->status->mana >=
                MainForm->animation->interface_controller->status->max_mana &&
                MainForm->animation->interface_controller->status->health >=
                    MainForm->animation->interface_controller->status->max_health)
                use_not_needed = true;
            if (use_not_needed)
            {
                MainForm->animation->notification->NotifyWithStatus(NotifyKind_Item, item->item_id,
                    NotifyStatus_NoEffect);
                MainForm->animation->interface_controller->ShowStatusMessage(category,
                    "There was no need to use " + item->name);
                MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
                return FinishUse(-101);
            }
        }
        if (item->type == EifItemType_Transformation &&
            MainForm->players->GetSkin(MainForm->net_client->GetPlayerId()) == item->spec_value)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                "The transformation aborted.");
            SuppressHoverMessage();
            return RefuseUse(-10, 55);
        }
        if (item->type == EifItemType_SkillBook &&
            MainForm->animation->interface_controller->spells->HasSpell(item->spec_value))
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                "You already learned this skill.");
            SuppressHoverMessage();
            return RefuseUse(-10, 55);
        }
        // A teleport is refused on a map that forbids it, or while actions are blocked.
        if (item->type == EifItemType_Teleport && MainForm->map->item_use_restricted)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(category,
                MainForm->strings->GetText(GameText_NothingHappened));
            MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
            return FinishUse(-20);
        }
        if (item->type == EifItemType_Teleport && MainForm->map->IsActionBlocked())
        {
            MainForm->animation->interface_controller->ShowStatusMessage(category,
                MainForm->strings->GetText(GameText_NothingHappened));
            MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
            return FinishUse(-20);
        }
        MainForm->animation->notification->NotifyWithStatus(NotifyKind_Item, item->item_id, NotifyStatus_Ready);
        MainForm->net_client->sender->SendUseItem(item_id);
        if (heals_nothing)
            return FinishUse(5);
        return FinishUse(item->type);
    }
    // Worn items: the class, the level and every needed stat are checked first.
    if (item->type >= EifItemType_Weapon && item->type <= EifItemType_Bracer)
    {
        equipment = MainForm->item_values->GetItem(item_id - 1);
        if (!MainForm->class_values->IsDescendantOf(MainForm->animation->interface_controller->status->class_id,
            equipment->required_class))
        {
            ClassValue *class_record = MainForm->class_values->GetRecord(equipment->required_class - 1);
            String class_name = class_record->name;
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemCanOnly) + class_name);
            MainForm->animation->interface_controller->HoldStatusMessages();
            SuppressHoverMessage();
            return RefuseUse(-7, 54);
        }
        if ((int)equipment->required_level > (int)MainForm->animation->interface_controller->status->level)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemRequires) + "LVL " + IntToStr(equipment->required_level));
            SuppressHoverMessage();
            return RefuseUse(-8, 55);
        }
        // The stats the item needs, listed in the refusal
        String stat_requirements = "";
        if (equipment->required_str > 0)
            stat_requirements = stat_requirements + IntToStr(equipment->required_str) + " STR ";
        if (equipment->required_int > 0)
            stat_requirements = stat_requirements + IntToStr(equipment->required_int) + " INT ";
        if (equipment->required_wis > 0)
            stat_requirements = stat_requirements + IntToStr(equipment->required_wis) + " WIS ";
        if (equipment->required_agi > 0)
            stat_requirements = stat_requirements + IntToStr(equipment->required_agi) + " AGI ";
        if (equipment->required_con > 0)
            stat_requirements = stat_requirements + IntToStr(equipment->required_con) + " CON ";
        if (equipment->required_cha > 0)
            stat_requirements = stat_requirements + IntToStr(equipment->required_cha) + " CHA ";
        if ((int)equipment->required_str > (int)MainForm->animation->interface_controller->status->stat_str ||
            (int)equipment->required_int > (int)MainForm->animation->interface_controller->status->stat_int ||
            (int)equipment->required_wis > (int)MainForm->animation->interface_controller->status->stat_wis ||
            (int)equipment->required_agi > (int)MainForm->animation->interface_controller->status->stat_agi ||
            (int)equipment->required_con > (int)MainForm->animation->interface_controller->status->stat_con ||
            (int)equipment->required_cha > (int)MainForm->animation->interface_controller->status->stat_cha)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemRequires) + stat_requirements);
            SuppressHoverMessage();
            return RefuseUse(-9, 55);
        }
        if (item->type == EifItemType_Weapon)
        {
            if (MainForm->animation->paperdoll->equipped_weapon <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(MainForm->animation->paperdoll->equipped_weapon))
            {
                MainForm->sfx->PlaySfx(SfxSound_Blocked);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_CouldNotRemoveItem));
            }
            else
            {
                if (!hotswap_supported && equip_pending)
                {
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        "This server not (yet) support item-hotswap, please unequip the equiped weapon first.");
                    MainForm->animation->interface_controller->HoldStatusMessages();
                    return RefuseUse(-10, 55);
                }
                equip_pending = true;
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
        }
        if (item->type == EifItemType_Shield)
        {
            if (MainForm->animation->paperdoll->equipped_shield <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(MainForm->animation->paperdoll->equipped_shield))
            {
                MainForm->sfx->PlaySfx(SfxSound_Blocked);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_CouldNotRemoveItem));
            }
            else
            {
                if (!hotswap_supported && equip_pending)
                {
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        "This server not (yet) support item-hotswap, please unequip the equiped shield first.");
                    MainForm->animation->interface_controller->HoldStatusMessages();
                    return RefuseUse(-10, 55);
                }
                equip_pending = true;
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
        }
        if (item->type == EifItemType_Armor)
        {
            if (MainForm->animation->paperdoll->equipped_armor <= 0)
            {
                if (MainForm->item_values->GetGender(item_id) ==
                    MainForm->players->GetGender(MainForm->net_client->GetPlayerId()))
                {
                    MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                    return FinishUse(item->type);
                }
                else
                {
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_ThisArmorDoesNot));
                    SuppressHoverMessage();
                    return RefuseUse(-11, 54);
                }
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Hat)
        {
            if (MainForm->animation->paperdoll->equipped_head <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Boots)
        {
            if (MainForm->animation->paperdoll->equipped_boots <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Gloves)
        {
            if (MainForm->animation->paperdoll->equipped_gloves <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Accessory)
        {
            if (MainForm->animation->paperdoll->equipped_accessory <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Belt)
        {
            if (MainForm->animation->paperdoll->equipped_belt <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Necklace)
        {
            if (MainForm->animation->paperdoll->equipped_necklace <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Ring)
        {
            if (MainForm->animation->paperdoll->equipped_ring1 <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else if (MainForm->animation->paperdoll->equipped_ring2 <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_Second);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Armlet)
        {
            if (MainForm->animation->paperdoll->equipped_armlet1 <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else if (MainForm->animation->paperdoll->equipped_armlet2 <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_Second);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
        if (item->type == EifItemType_Bracer)
        {
            if (MainForm->animation->paperdoll->equipped_bracer1 <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_First);
                return FinishUse(item->type);
            }
            else if (MainForm->animation->paperdoll->equipped_bracer2 <= 0)
            {
                MainForm->net_client->sender->SendEquipItem(item_id, PaperdollSubSlot_Second);
                return FinishUse(item->type);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
                SuppressHoverMessage();
                return RefuseUse(-10, 55);
            }
        }
    }
    FinishUse(-1);
}

int InventoryController::FinishUse(int outcome)
{
    CancelDrag();
    PlayUseSound(outcome);
    return outcome;
}

int InventoryController::RefuseUse(int outcome, int sound)
{
    CancelDrag();
    MainForm->sfx->PlaySfx(sound);
    return outcome;
}

// Plays the sound that goes with using or equipping an item of the type.
void InventoryController::PlayUseSound(int item_type)
{
    if (item_type < 0)
        return;
    if (!item_type)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemStatic);
        return;
    }
    if (item_type == EifItemType_Currency)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemCoins);
        return;
    }
    if (item_type == EifItemType_Heal || item_type == EifItemType_Alcohol || item_type == EifItemType_CureCurse)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemPotion);
        return;
    }
    if (item_type == EifItemType_HairDye)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemHairDye);
        return;
    }
    if (item_type == EifItemType_Key)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemKey);
        return;
    }
    if (item_type == EifItemType_Teleport)
    {
        MainForm->sfx->PlaySfx(SfxSound_Send);
        return;
    }
    if (item_type == EifItemType_ExpReward)
    {
        MainForm->sfx->PlaySfx(SfxSound_Send);
        return;
    }
    if (item_type == EifItemType_SkillBook)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemBook);
        return;
    }
    if (item_type == EifItemType_VisualDocument)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemDocument);
        return;
    }
    if (item_type == EifItemType_Weapon)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemWeapon);
        return;
    }
    if (item_type == EifItemType_Shield)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemWeapon);
        return;
    }
    if (item_type >= EifItemType_Armor && item_type <= EifItemType_Gloves)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemArmor);
        return;
    }
    if (item_type == EifItemType_Accessory)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemAccessory);
        return;
    }
    if (item_type == EifItemType_Belt)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemBelt);
        return;
    }
    if (item_type == EifItemType_Necklace || item_type == EifItemType_Ring)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemJewelry);
        return;
    }
    if (item_type == EifItemType_Armlet || item_type == EifItemType_Bracer)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemArmlet);
        return;
    }
    if (item_type == EifItemType_TitleCertificate)
    {
        MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
        return;
    }
    MainForm->sfx->PlaySfx(SfxSound_ItemPlace);
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
    mouse_x = x;
    mouse_y = y;
}

void InventoryController::RefreshHover()
{
    hover_col = -1;
    hover_row = -1;
    MouseMove(mouse_x, mouse_y);
}

// Tracks the inventory cell under the mouse: while dragging it checks where the item would fit, otherwise it shows
// the hovered item's tip with its amount and weight.
void InventoryController::UpdateItemHover(int x, int y)
{
    InventoryItem *i;
    int extra_rows;
    int col;
    int result;
    int other_size;
    int j;
    int k;
    int row;
    int wide;
    int drag_extra_rows;
    int overlap_count;
    int item_extra_cols;
    int item_extra_rows;
    char uncounted;
    char found;
    InventoryItem *m;
    int dc;
    // Cells are squares from the grid's top-left corner
    col = (x - 114) / INVENTORY_CELL_SIZE;
    row = (y - 339) / INVENTORY_CELL_SIZE;
    last_mouse_x = x;
    last_mouse_y = y;
    if (row >= 0 && row <= 3 && col >= 0 && col <= 16 && x > 109 && y > 333)
    {
        if (is_dragging)
        {
            // A size above 3 is two columns wide; the rest of the size is the extra rows
            if (drag_item_size > ITEM_SIZE_MAX_NARROW)
            {
                wide = 1;
                drag_extra_rows = drag_item_size - 4;
            }
            else
            {
                wide = 0;
                drag_extra_rows = drag_item_size;
            }
            if (wide >= 1)
                col = (x - 126) / INVENTORY_CELL_SIZE;
            if (drag_extra_rows >= 1)
                row = (y - 339 - 12 * drag_extra_rows) / INVENTORY_CELL_SIZE;
            if (wide + col > 16)
                col = 16 - wide;
            if (row + drag_extra_rows > 3)
                row = 3 - drag_extra_rows;
            if (col < 0)
                col = 0;
            if (row < 0)
                row = 0;
            overlap_count = 0;
            i = items.begin();
            while (i != items.end())
            {
                if (drag_item_id != i->item_id)
                {
                    other_size = MainForm->item_values->GetSize(i->item_id);
                    if (other_size > 3)
                    {
                        item_extra_cols = 1;
                        item_extra_rows = other_size - 4;
                    }
                    else
                    {
                        item_extra_cols = 0;
                        item_extra_rows = other_size;
                    }
                    uncounted = true;
                    for (j = 0; j <= wide; j++)
                    {
                        for (k = 0; k <= drag_extra_rows; k++)
                        {
                            if (col + j >= i->grid_x && col + j <= i->grid_x + item_extra_cols &&
                                row + k >= i->grid_y && row + k <= i->grid_y + item_extra_rows && i->is_visible &&
                                uncounted)
                            {
                                overlap_count++;
                                uncounted = false;
                            }
                        }
                    }
                }
                i++;
            }
            // Dropping on one item swaps it; on two or more the drop is refused
            if (overlap_count < 2)
                drop_blocked = false;
            else
                drop_blocked = true;
            hover_col = col;
            hover_row = row;
            hover_col_end = col + wide;
            hover_row_end = row + drag_extra_rows;
            hover_text_width = 0;
            has_hover = true;
            hover_dirty = true;
            items_dirty = true;
        }
        else
        {
            found = false;
            m = items.begin();
            while (items.end() != m)
            {
                result = MainForm->item_values->GetSize(m->item_id);
                if (result > 3)
                {
                    extra_rows = 1;
                    dc = result - 4;
                    tooltip->SetFlipExtra(26);
                }
                else
                {
                    extra_rows = 0;
                    dc = result;
                    tooltip->SetFlipExtra(0);
                }
                if (col >= m->grid_x && col <= m->grid_x + extra_rows && m->grid_y <= row && m->grid_y + dc >= row &&
                    m->is_visible)
                {
                    found = true;
                    if (hover_col != m->grid_x || hover_row != m->grid_y)
                    {
                        hover_item_size = result;
                        hover_item_id = m->item_id;
                        hover_col = m->grid_x;
                        hover_row = m->grid_y;
                        hover_col_end = extra_rows + m->grid_x;
                        hover_row_end = m->grid_y;
                        tooltip->ShowItemCount(m->item_id, m->amount);
                        tooltip->SetAnchorX(26 * hover_col_end);
                        tooltip->SetAnchorY(26 * hover_row_end);
                        if (hover_row_end <= 1)
                            tooltip->SetVerticalAlign(TooltipAlign_Below);
                        if (hover_row_end >= 2)
                            tooltip->SetVerticalAlign(TooltipAlign_Above);
                        if (hover_message_delay < 1)
                        {
                            ItemValue *item_value = MainForm->item_values->GetItem(m->item_id - 1);
                            String weight_text = MainForm->animation->interface_controller->status->GetWeightText();
                            int item_weight = item_value->weight;
                            String status_text = item_value->name + ", " + IntToStr(item_weight) + " weight, ";
                            if (item_weight > 0 && m->amount > 1)
                                status_text =
                                    status_text + IntToStr((unsigned int)(m->amount * item_weight)) + " total, ";
                            status_text = status_text + weight_text;
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Item,
                                status_text);
                        }
                        drop_blocked = false;
                        has_hover = true;
                        hover_dirty = true;
                        items_dirty = true;
                    }
                    break;
                }
                m++;
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
                }
                tooltip->Hide();
                drop_blocked = false;
                has_hover = false;
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
        }
        tooltip->Hide();
        drop_blocked = false;
        has_hover = false;
    }
}

// A click in the inventory: the drop and junk buttons drop or destroy the dragged item, asking an amount for a stack;
// in the grid a click picks up or places an item, a double click uses it, and a dragged item goes to the open window.
void InventoryController::MouseDown(int x, int y, int button)
{
    vector<InventoryItem>::iterator other;
    vector<InventoryItem>::iterator entry;
    vector<InventoryItem>::iterator it;
    vector<InventoryItem>::iterator slot;
    int picked_item_size;
    int picked_extra_cols;
    int picked_extra_rows;
    int slot_wide;
    int placed_item_size;
    int j;
    int dy;
    vector<InventoryItem>::iterator dragged;
    int slot_size;
    int slot_rows;
    int item_type;
    if (!dragging_from_paperdoll)
    {
        char can_click = true;
        if (x >= 561 && x <= 584 && y >= 395 && y <= 418)
        {
            if (is_dragging)
            {
                char drop_allowed = true;
                if (MainForm->item_values->GetSpecial(drag_item_id) == ItemSpecial_Bound)
                {
                    drop_allowed = false;
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                    MainForm->edits->caret_enabled = false;
                }
                if (drop_allowed && MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
                {
                    drop_allowed = false;
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetText(GameText_Warning),
                        MainForm->strings->GetText(GameText_YouCannotDropItems));
                    MainForm->edits->caret_enabled = false;
                }
                if (drop_allowed)
                {
                    if (GetItemAmount(drag_item_id) < 2)
                    {
                        if (drag_item_id == ItemId_Gold)
                        {
                            if (!GetItemAmount(drag_item_id))
                            {
                                String gold_name = MainForm->item_values->GetName(ItemId_Gold);
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                    MainForm->strings->GetText(GameText_YouHaveNoMore) + " " + gold_name);
                            }
                            else
                                MainForm->net_client->sender->SendDropItem(drag_item_id, 1, -1, -1);
                        }
                        else
                            MainForm->net_client->sender->SendDropItem(drag_item_id, 1, -1, -1);
                    }
                    else
                    {
                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_DropItem, GetItemAmount(drag_item_id),
                            drag_item_id, 0);
                        input_enabled = false;
                    }
                }
                CancelDrag();
                UpdateItemHover(x, y);
            }
            else
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                    MainForm->strings->GetText(GameText_DragItemToThis));
        }
        if (x >= 561 && x <= 584 && y >= 423 && y <= 446)
        {
            if (is_dragging)
            {
                if (GetItemAmount(drag_item_id) < 2)
                {
                    if (drag_item_id == ItemId_Gold)
                    {
                        if (!GetItemAmount(drag_item_id))
                        {
                            String drop_gold_name = MainForm->item_values->GetName(ItemId_Gold);
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                MainForm->strings->GetText(GameText_YouHaveNoMore) + " " + drop_gold_name);
                        }
                        else
                        {
                            String junk_name = MainForm->item_values->GetName(drag_item_id);
                            MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_DestroyItem,
                                "Destroy Item", "Destroy the " + junk_name + " ?");
                        }
                    }
                    else
                    {
                        String destroy_name = MainForm->item_values->GetName(drag_item_id);
                        MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_DestroyItem,
                            "Destroy Item", "Destroy the " + destroy_name + " ?");
                    }
                }
                else
                {
                    MainForm->animation->game->OpenAmountBox(AmountBoxKind_JunkItem, GetItemAmount(drag_item_id),
                        drag_item_id, 0);
                    input_enabled = false;
                }
                CancelDrag();
                UpdateItemHover(x, y);
            }
            else
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                    MainForm->strings->GetText(GameText_DragItemToThisButton));
        }
        if (input_enabled && can_click)
        {
            char cancel_drag = false;
            int grid_col = (x - 114) / INVENTORY_CELL_SIZE;
            int row = (y - 339) / INVENTORY_CELL_SIZE;
            if (!button)
            {
                if (row >= 0 && row <= 3 && grid_col >= 0 && grid_col <= 16 && x > 109 && y > 333)
                {
                    if (MainForm->animation->game->amount_box_visible)
                    {
                        MainForm->animation->game->amount_box_visible = false;
                        MainForm->edits->RemoveEdit(EditId_Amount);
                        MainForm->edits->FocusEdit(EditId_Chat);
                    }
                    if (is_dragging)
                    {
                        // A second click on the same cell soon after the first uses the item
                        if (click_ticks < 3 && grid_col == click_col && click_row == row)
                            UseItem(drag_item_id, 2);
                        else
                        {
                            int drag_extra_cols;
                            int drag_extra_rows;
                            if (drag_item_size > ITEM_SIZE_MAX_NARROW)
                            {
                                drag_extra_cols = 1;
                                drag_extra_rows = drag_item_size - 4;
                            }
                            else
                            {
                                drag_extra_cols = 0;
                                drag_extra_rows = drag_item_size;
                            }
                            if (drag_extra_cols >= 1)
                                grid_col = (x - 126) / INVENTORY_CELL_SIZE;
                            if (drag_extra_rows >= 1)
                                row = (y - 339 - 12 * drag_extra_rows) / INVENTORY_CELL_SIZE;
                            if (drag_extra_cols + grid_col > 16)
                                grid_col = 16 - drag_extra_cols;
                            if (row + drag_extra_rows > 3)
                                row = 3 - drag_extra_rows;
                            if (grid_col < 0)
                                grid_col = 0;
                            if (row < 0)
                                row = 0;
                            int overlap_count = 0;
                            int overlapped_item_id = -1;
                            for (other = items.begin(); other != items.end(); other++)
                            {
                                if (drag_item_id != other->item_id)
                                {
                                    placed_item_size = MainForm->item_values->GetSize(other->item_id);
                                    int item_extra_cols;
                                    int item_extra_rows;
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
                                    char not_counted = true;
                                    for (j = 0; j <= drag_extra_cols; j++)
                                    {
                                        for (dy = 0; dy <= drag_extra_rows; dy++)
                                        {
                                            if (grid_col + j >= other->grid_x &&
                                                grid_col + j <= other->grid_x + item_extra_cols &&
                                                row + dy >= other->grid_y &&
                                                row + dy <= other->grid_y + item_extra_rows && other->is_visible &&
                                                not_counted)
                                            {
                                                overlap_count++;
                                                overlapped_item_id = other->item_id;
                                                not_counted = false;
                                            }
                                        }
                                    }
                                }
                            }
                            if (!overlap_count)
                            {
                                int use_type = MainForm->item_values->GetType(drag_item_id);
                                PlayUseSound(use_type);
                                layout_backup.clear();
                                for (dragged = items.begin(); dragged != items.end(); dragged++)
                                {
                                    if (dragged->item_id == drag_item_id)
                                    {
                                        dragged->grid_x = grid_col;
                                        dragged->grid_y = row;
                                        dragged->is_placed = true;
                                        dragged->is_visible = true;
                                        RecordLayoutChange(drag_item_id, grid_col, row);
                                        break;
                                    }
                                }
                                SaveLayoutChanges();
                                is_dragging = false;
                                drag_started = false;
                                drag_delay_ticks = DRAG_DELAY_IDLE;
                                hover_col = -1;
                                hover_row = -1;
                                items_dirty = true;
                                hover_dirty = true;
                                MainForm->dragging = false;
                                if (MainForm->animation->trade->is_visible &&
                                    MainForm->animation->trade->trade_locked && !CanFitTradeItems())
                                {
                                    MainForm->animation->trade->own_agreed = false;
                                    MainForm->animation->trade->own_status_text =
                                        MainForm->strings->GetText(GameText_Trading);
                                    MainForm->net_client->sender->SendTradeDisagree();
                                    MainForm->animation->trade->trade_locked = false;
                                    MainForm->redraw_needed = true;
                                }
                            }
                            // The item under the drop is picked up in turn, so the two swap
                            if (overlap_count == 1)
                            {
                                MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                                int held_item_id = drag_item_id;
                                entry = items.begin();
                                int moved_id;
                                int moved_col;
                                int moved_row;
                                int swapped_id;
                                int swapped_col;
                                int swapped_row;
                                while (entry != items.end())
                                {
                                    if (entry->item_id == overlapped_item_id)
                                    {
                                        hover_item_id = entry->item_id;
                                        hover_item_size = MainForm->item_values->GetSize(entry->item_id);
                                        drag_x = x;
                                        drag_y = y;
                                        drag_item_id = entry->item_id;
                                        drag_item_size = MainForm->item_values->GetSize(entry->item_id);
                                        drag_graphic = MainForm->item_values->GetGraphic(entry->item_id) - 1;
                                        swapped_id = overlapped_item_id;
                                        swapped_col = entry->grid_x;
                                        swapped_row = entry->grid_y;
                                        entry->is_visible = false;
                                    }
                                    if (entry->item_id == held_item_id)
                                    {
                                        entry->grid_x = grid_col;
                                        entry->grid_y = row;
                                        moved_id = held_item_id;
                                        moved_col = grid_col;
                                        moved_row = row;
                                        entry->is_placed = true;
                                        entry->is_visible = true;
                                    }
                                    entry++;
                                }
                                RecordLayoutChange(moved_id, moved_col, moved_row);
                                RecordLayoutChange(swapped_id, swapped_col, swapped_row);
                                drag_started = false;
                                drag_delay_ticks = DRAG_DELAY_IDLE;
                                items_dirty = true;
                                hover_dirty = true;
                            }
                        }
                    }
                    else
                    {
                        click_ticks = 0;
                        click_col = grid_col;
                        click_row = row;
                        BackupLayout();
                        ResetLayoutChanges();
                        for (it = items.begin(); it != items.end(); it++)
                        {
                            picked_item_size = MainForm->item_values->GetSize(it->item_id);
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
                            if (grid_col >= it->grid_x && grid_col <= it->grid_x + picked_extra_cols &&
                                it->grid_y <= row && it->grid_y + picked_extra_rows >= row)
                            {
                                MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                                RecordLayoutChange(it->item_id, it->grid_x, it->grid_y);
                                hover_item_size = picked_item_size;
                                hover_item_id = it->item_id;
                                is_dragging = true;
                                drag_started = false;
                                drag_delay_ticks = 0;
                                drag_x = x;
                                drag_y = y;
                                drag_item_size = picked_item_size;
                                drag_item_id = it->item_id;
                                drag_graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
                                it->is_visible = false;
                                items_dirty = true;
                                hover_dirty = true;
                                MainForm->dragging = true;
                            }
                        }
                    }
                }
                else
                    cancel_drag = true;
            }
            else
            {
                cancel_drag = true;
                if (MainForm->animation->locker->is_visible)
                {
                    int locker_item_id = -1;
                    for (slot = items.begin(); slot != items.end(); slot++)
                    {
                        slot_size = MainForm->item_values->GetSize(slot->item_id);
                        if (slot_size > 3)
                        {
                            slot_wide = 1;
                            slot_rows = slot_size - 4;
                        }
                        else
                        {
                            slot_wide = 0;
                            slot_rows = slot_size;
                        }
                        if (grid_col >= slot->grid_x && grid_col <= slot_wide + slot->grid_x && slot->grid_y <= row &&
                            slot_rows + slot->grid_y >= row)
                            locker_item_id = slot->item_id;
                    }
                    if (locker_item_id > 0)
                    {
                        TPoint locker_pos;
                        locker_pos.x = MainForm->animation->locker->locker_x;
                        locker_pos.y = MainForm->animation->locker->locker_y;
                        if (MainForm->animation->locker->GetItemAmount(locker_item_id) < 200)
                        {
                            MainForm->net_client->sender->SendLockerAddItem(locker_pos, locker_item_id, 1);
                            MainForm->animation->interface_controller->inventory->tooltip->Hide();
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_LockerFullStore, false),
                                MainForm->strings->GetDialogText(DialogText_LockerFullStore, true));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                }
                else if (!is_dragging)
                {
                    InventoryItem *clicked_item = ItemAt(grid_col, row);
                    if (clicked_item)
                        UseItem(clicked_item->item_id, 2);
                }
            }
            if (cancel_drag && is_dragging)
            {
                CancelDrag();
                UpdateItemHover(x, y);
            }
        }
    }
    else
    {
        if (!button || drag_started)
        {
            int cell_col = (x - 114) / INVENTORY_CELL_SIZE;
            int cell_row = (y - 339) / INVENTORY_CELL_SIZE;
            if (cell_row >= 0 && cell_row <= 3 && cell_col >= 0 && cell_col <= 16 && x > 109 && y > 333)
            {
                if (MainForm->animation->interface_controller->inventory->HasRoomForItem(drag_item_id))
                {
                    if (MainForm->item_values->GetSpecial(drag_item_id) == ItemSpecial_Cursed)
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_CursedItem, false),
                            MainForm->strings->GetDialogText(DialogText_CursedItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    else
                    {
                        int sound_type = MainForm->item_values->GetType(drag_item_id);
                        MainForm->animation->interface_controller->inventory->PlayUseSound(sound_type);
                        MainForm->net_client->sender->SendUnequipItem(drag_item_id, paperdoll_sub_slot);
                    }
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
            item_type = MainForm->item_values->GetType(drag_item_id);
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
                MainForm->animation->paperdoll->accessory_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Belt)
                MainForm->animation->paperdoll->belt_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Necklace)
                MainForm->animation->paperdoll->necklace_graphic = paperdoll_slot_graphic;
            if (item_type == EifItemType_Ring)
            {
                if (!MainForm->animation->paperdoll->ring2_graphic)
                    MainForm->animation->paperdoll->ring2_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->ring1_graphic = paperdoll_slot_graphic;
            }
            if (item_type == EifItemType_Armlet)
            {
                if (!MainForm->animation->paperdoll->armlet2_graphic)
                    MainForm->animation->paperdoll->armlet2_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->armlet1_graphic = paperdoll_slot_graphic;
            }
            if (item_type == EifItemType_Bracer)
            {
                if (!MainForm->animation->paperdoll->bracer2_graphic)
                    MainForm->animation->paperdoll->bracer2_graphic = paperdoll_slot_graphic;
                else
                    MainForm->animation->paperdoll->bracer1_graphic = paperdoll_slot_graphic;
            }
            PlayUseSound(item_type);
            MainForm->redraw_needed = true;
            dragging_from_paperdoll = false;
            CancelDrag();
            UpdateItemHover(x, y);
        }
    }
}

// The item that covers the grid cell, by its place and size; none for an empty cell.
InventoryItem *InventoryController::ItemAt(int col, int row)
{
    int wide;
    int rows;
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        int size = MainForm->item_values->GetSize(it->item_id);
        if (size > 3)
        {
            wide = 1;
            rows = size - 4;
        }
        else
        {
            wide = 0;
            rows = size;
        }
        if (col >= it->grid_x && col <= wide + it->grid_x && row >= it->grid_y && row <= rows + it->grid_y)
            return it;
    }
    return 0;
}

// Ends a drag and puts the items back where they were when it began.
void InventoryController::CancelDrag()
{
    is_dragging = false;
    drag_started = false;
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

// Puts back the items kept by BackupLayout and lets input in again.
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

// Puts back the items as they were before the drag and lets input in again.
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
}

// Releasing the button after a drag has started places the item where the mouse is.
void InventoryController::MouseUp()
{
    if (drag_started && is_dragging)
    {
        MouseDown(last_mouse_x, last_mouse_y, 0);
        drag_delay_ticks = DRAG_DELAY_IDLE;
        drag_started = false;
        return;
    }
    else
    {
        drag_delay_ticks = DRAG_DELAY_IDLE;
        drag_started = false;
    }
}

// Lights the two inventory buttons under the mouse and puts their tips on the status line; not while an item is
// dragged from the paperdoll.
void InventoryController::UpdateButtonHover(int x, int y)
{
    bool result = false;
    if (x < 561 || x > 584)
    {
        if (left_button_hovered || right_button_hovered)
        {
            left_button_hovered = false;
            right_button_hovered = false;
            hover_dirty = true;
        }
        return;
    }
    if (y >= 395 && y <= 418 && !dragging_from_paperdoll)
    {
        if (!left_button_hovered)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                MainForm->strings->GetText(GameText_DragItemToThis));
            left_button_hovered = true;
            result = true;
        }
    }
    else if (left_button_hovered)
    {
        left_button_hovered = false;
        result = true;
    }
    if (y >= 423 && y <= 446 && !dragging_from_paperdoll)
    {
        if (!right_button_hovered)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                MainForm->strings->GetText(GameText_DragItemToThisButton));
            right_button_hovered = true;
            result = true;
        }
    }
    else if (right_button_hovered)
    {
        right_button_hovered = false;
        result = true;
    }
    if (result)
        hover_dirty = true;
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
    {
        changes_overflowed = true;
        return;
    }
    change_item_ids[change_count] = item_id;
    change_cols[change_count] = col;
    change_rows[change_count] = row;
    change_count++;
}

// Writes the item in each of the 4 rows of 17 cells to the registry, 0 for an empty cell.
void InventoryController::SaveLayout()
{
    int slot_item_id;
    vector<InventoryItem>::iterator it;
    MainForm->registry->OpenSlotKey(LayoutKey_Inventory);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j <= 16; j++)
        {
            slot_item_id = 0;
            for (it = items.begin(); it != items.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                {
                    slot_item_id = it->item_id;
                    break;
                }
            }
            MainForm->registry->WriteSlotValue(17 * i + j, slot_item_id);
        }
    }
    MainForm->registry->CloseSlotKey();
}

// Writes the inventory cells changed since the last save to the registry, or the whole layout when too many changed,
// then clears the record of changes.
void InventoryController::SaveLayoutChanges()
{
    int *change;
    int i;
    char should_write;
    if (!changes_overflowed)
    {
        char first = true;
        char opened = false;
        i = 0;
        change = change_cols;
        while (i < change_count)
        {
            int slot_value = 0;
            should_write = true;
            int cell = 17 * change[20] + *change;
            if (!first)
            {
                if (i % 2 == 0)
                {
                    if (*change == change[-1] && change[20] == change[19])
                        should_write = false;
                }
                else
                    slot_value = change[-20];
            }
            first = false;
            if (should_write)
            {
                if (!opened)
                {
                    opened = true;
                    MainForm->registry->OpenSlotKey(LayoutKey_Inventory);
                }
                MainForm->registry->WriteSlotValue(cell, slot_value);
            }
            i++;
            change++;
        }
        if (opened)
            MainForm->registry->CloseSlotKey();
    }
    else
        SaveLayout();
    ResetLayoutChanges();
}

// Whether the inventory holds the item.
char InventoryController::HasItem(int item_id)
{
    char found = false;
    vector<InventoryItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (item_id == it->item_id)
            return 1;
    }
    return found;
}

// True when a key item for the lock is in the inventory.
char InventoryController::HasKey(int key_id)
{
    char found;
    int item_id;
    found = false;
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        int item_id = it->item_id;
        if (MainForm->item_values->GetType(item_id) == 9 && MainForm->item_values->GetSpecValue(item_id) == key_id)
        {
            found = true;
            break;
        }
    }
    return found;
}

// True when the item is already held or a free place in the grid fits its size.
bool InventoryController::HasRoomForItem(int item_id)
{
    InventoryItem *it;
    int k;
    InventoryItem *walk;
    int item_size;
    int other_size;
    int n;
    int ii;
    bool has_room;
    int extra_cols;
    int extra_rows;
    int j;
    int overlap_count;
    int other_extra_cols;
    int other_extra_rows;
    has_room = false;
    walk = items.begin();
    while (walk != items.end())
    {
        // An item already held always has room, as it adds to the stack.
        if (item_id == walk->item_id)
        {
            has_room = true;
            break;
        }
        walk++;
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
        j = 0;
        while (4 - extra_rows > j && !has_room)
        {
            k = 0;
            while (k <= 16 - extra_cols && !has_room)
            {
                overlap_count = 0;
                it = items.begin();
                while (it != items.end())
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
                    char uncounted = true;
                    for (n = 0; n <= extra_cols; n++)
                    {
                        for (ii = 0; ii <= extra_rows; ii++)
                        {
                            if (k + n >= it->grid_x && k + n <= it->grid_x + other_extra_cols && j + ii >= it->grid_y &&
                                j + ii <= it->grid_y + other_extra_rows && uncounted)
                            {
                                overlap_count++;
                                uncounted = false;
                            }
                        }
                    }
                    it++;
                }
                if (!overlap_count)
                {
                    has_room = true;
                    break;
                }
                k++;
            }
            j++;
        }
    }
    return has_room;
}

bool InventoryController::AddItemQuietly(int item_id, int amount)
{
    AddItem(item_id, amount, false);
}

// Adds to a held stack, or places the item at the first free place for its size; false when nothing fits.
// A newly placed item's cell is saved in the registry.
bool InventoryController::AddItem(int item_id, int amount, char set_amount)
{
    int try_col;
    InventoryItem *m;
    char stacked;
    InventoryItem *i;
    int item_size;
    int other_size;
    int n;
    int row_offset;
    bool placed;
    int extra_cols;
    int extra_rows;
    int free_col;
    int free_row;
    int try_row;
    int overlap_count;
    int other_extra_cols;
    int other_extra_rows;
    placed = true;
    stacked = false;
    i = items.begin();
    while (i != items.end())
    {
        // An item already held adds to its stack, or takes the new amount.
        if (i->item_id == item_id)
        {
            if (!set_amount)
                i->amount += amount;
            else
                i->amount = amount;
            items_dirty = true;
            hover_dirty = true;
            stacked = true;
            break;
        }
        i++;
    }
    if (!stacked)
    {
        placed = false;
        // Sizes 4 and up are two slots wide, and the rest of the size gives the height.
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
        try_row = 0;
        while (4 - extra_rows > try_row && !placed)
        {
            try_col = 0;
            while (try_col <= 16 - extra_cols && !placed)
            {
                overlap_count = 0;
                m = items.begin();
                while (m != items.end())
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
                    char uncounted = true;
                    for (n = 0; n <= extra_cols; n++)
                    {
                        for (row_offset = 0; row_offset <= extra_rows; row_offset++)
                        {
                            if (try_col + n >= m->grid_x && try_col + n <= m->grid_x + other_extra_cols &&
                                try_row + row_offset >= m->grid_y &&
                                try_row + row_offset <= m->grid_y + other_extra_rows && uncounted)
                            {
                                overlap_count++;
                                uncounted = false;
                            }
                        }
                    }
                    m++;
                }
                if (!overlap_count)
                {
                    free_col = try_col;
                    free_row = try_row;
                    placed = true;
                }
                try_col++;
            }
            try_row++;
        }
        if (placed)
        {
            InventoryItem handle(item_id);
            handle.grid_x = free_col;
            handle.grid_y = free_row;
            handle.amount = amount;
            items.insert(items.end(), handle);
            int slot = 17 * free_row + free_col;
            MainForm->registry->OpenSlotKey(LayoutKey_Inventory);
            MainForm->registry->WriteSlotValue(slot, item_id);
            MainForm->registry->CloseSlotKey();
            hover_dirty = true;
            items_dirty = true;
        }
    }
    return placed;
}

// Takes an amount off the item; an item other than gold that runs out leaves the grid and its saved cell.
char InventoryController::RemoveItem(int item_id, int amount)
{
    char result = 1;
    char removed = false;
    vector<InventoryItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (item_id == it->item_id)
        {
            it->amount -= amount;
            if (it->item_id == ItemId_Gold)
            {
                if (it->amount <= 0)
                    // Item 1 stays in the list at 0; any other item at 0 leaves it, and its saved slot is cleared.
                    it->amount = 0;
            }
            else if (it->amount <= 0)
            {
                if (!removed)
                {
                    removed = true;
                    MainForm->registry->OpenSlotKey(LayoutKey_Inventory);
                }
                int slot = 17 * it->grid_y + it->grid_x;
                MainForm->registry->WriteSlotValue(slot, 0);
                items.erase(it);
            }
            hover_dirty = true;
            items_dirty = true;
            break;
        }
    }
    if (removed)
        MainForm->registry->CloseSlotKey();
    return result;
}

// The weight of everything held and equipped: each stack counts its amount, each equipped item once.
int InventoryController::GetTotalWeight()
{
    int items_weight = 0;
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
        items_weight += MainForm->item_values->GetWeight(it->item_id) * (int)it->amount;
    // Then every worn item.
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_head);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_necklace);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_armor);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_weapon);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_shield);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_belt);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_gloves);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_boots);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_ring1);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_ring2);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_armlet1);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_armlet2);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_bracer1);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_bracer2);
    items_weight += MainForm->item_values->GetWeight(MainForm->animation->paperdoll->equipped_accessory);
    return items_weight;
}

// Sets the amount of the item and of its tip; an item other than gold set to none leaves the grid and its saved cell.
void InventoryController::SetItemAmount(int item_id, int amount)
{
    char removed = false;
    for (vector<InventoryItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        if (it->item_id == item_id)
        {
            if (amount != (int)it->amount)
                items_dirty = true;
            it->amount = amount;
            if (it->item_id == ItemId_Gold)
            {
                if (it->amount <= 0)
                    it->amount = 0;
            }
            else if (it->amount <= 0)
            {
                if (!removed)
                {
                    removed = true;
                    MainForm->registry->OpenSlotKey(LayoutKey_Inventory);
                }
                int slot = 17 * it->grid_y + it->grid_x;
                MainForm->registry->WriteSlotValue(slot, 0);
                items.erase(it);
            }
            hover_dirty = true;
            items_dirty = true;
            break;
        }
    }
    if (removed)
        MainForm->registry->CloseSlotKey();
    tooltip->UpdateItemAmount(item_id, amount);
}

// The amount held of the item, or 0.
DEFINE_GET_ITEM_AMOUNT(InventoryController)

// Keeps an item of the character's inventory until PlaceLoadedItems places it.
void InventoryController::AddLoadedItem(int item_id, int amount)
{
    InventoryItem handle(item_id);
    handle.amount = amount;
    handle.is_placed = false;
    loaded_items.insert(loaded_items.end(), handle);
}

bool InventoryController::IsInSavedSlots(int item_id)
{
    for (vector<InventoryItem>::iterator it = saved_slots.begin(); it != saved_slots.end(); it++)
    {
        if (item_id == it->item_id)
            return true;
    }
    return false;
}

// Places the loaded items at the cells saved in the registry; the rest go to the first free places.
void InventoryController::PlaceLoadedItems()
{
    vector<InventoryItem>::iterator loaded_item;
    int slot_item_id;
    vector<InventoryItem>::iterator saved_slot;
    MainForm->registry->OpenSlotKey(LayoutKey_Inventory);
    saved_slots.clear();
    // The saved slots of the 17 by 4 grid; a second slot saved for the same item is cleared.
    for (int i = 0; i < 68; i++)
    {
        slot_item_id = MainForm->registry->ReadSlotValue(i);
        if (slot_item_id > 0)
        {
            int cell_col = i % 17;
            int cell_row = i / 17;
            InventoryItem handle(slot_item_id);
            handle.grid_y = cell_row;
            handle.grid_x = cell_col;
            if (!IsInSavedSlots(slot_item_id))
                saved_slots.insert(saved_slots.end(), handle);
            else
                MainForm->registry->WriteSlotValue(i, 0);
        }
    }
    MainForm->registry->CloseSlotKey();
    loaded_item = loaded_items.begin();
    while (loaded_item != loaded_items.end())
    {
        char matched = false;
        saved_slot = saved_slots.begin();
        while (saved_slot != saved_slots.end() && !matched)
        {
            if (saved_slot->item_id == loaded_item->item_id)
            {
                if (saved_slot->item_id == ItemId_Gold)
                    MainForm->animation->interface_controller->status->gold = loaded_item->amount;
                loaded_item->is_placed = true;
                matched = true;
                InventoryItem placed_item(loaded_item->item_id);
                placed_item.grid_x = saved_slot->grid_x;
                placed_item.grid_y = saved_slot->grid_y;
                placed_item.amount = (int)loaded_item->amount;
                items.insert(items.end(), placed_item);
                int restored_graphic = MainForm->item_values->GetGraphic(loaded_item->item_id) - 1;
                MainForm->gfx->items->RequestSubItemGraphic(restored_graphic);
            }
            saved_slot++;
        }
        loaded_item++;
    }
    for (loaded_item = loaded_items.begin(); loaded_item != loaded_items.end(); loaded_item++)
    {
        if (!loaded_item->is_placed)
        {
            if (AddItemQuietly(loaded_item->item_id, loaded_item->amount))
            {
                loaded_item->is_placed = true;
                int pending_graphic = MainForm->item_values->GetGraphic(loaded_item->item_id) - 1;
                MainForm->gfx->items->RequestSubItemGraphic(pending_graphic);
            }
        }
    }
    input_enabled = true;
    layout_backup.clear();
}

// Places the loaded items that found no place before, where they fit now.
void InventoryController::PlaceRemainingItems()
{
    for (vector<InventoryItem>::iterator it = loaded_items.begin(); it != loaded_items.end(); it++)
    {
        if (!it->is_placed && AddItemQuietly(it->item_id, it->amount))
        {
            it->is_placed = true;
            int graphic = MainForm->item_values->GetGraphic(it->item_id) - 1;
            MainForm->gfx->items->RequestSubItemGraphic(graphic);
        }
    }
}
