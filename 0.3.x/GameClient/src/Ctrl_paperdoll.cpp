#include <vcl.h>
#pragma hdrstop

#include "Ctrl_paperdoll.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#include "Ctrl_inventory.h"
#include "Ctrl_tooltip.h"

#pragma package(smart_init)

// A paperdoll with no item in any slot, its window at its default spot.
PaperdollController::PaperdollController()
{
    window_x = 120;
    window_y = 16;
    frame_index = 0;
    admin_level = 0;
    is_visible = false;
    close_hovered = false;
    hovered_slot = PaperdollSlot_None;
    unused_flag = true;
    equipped_head = -1;
    equipped_necklace = -1;
    equipped_armor = -1;
    equipped_weapon = -1;
    equipped_shield = -1;
    equipped_belt = -1;
    equipped_gloves = -1;
    equipped_boots = -1;
    equipped_ring1 = -1;
    equipped_ring2 = -1;
    equipped_armlet1 = -1;
    equipped_armlet2 = -1;
    equipped_bracer1 = -1;
    equipped_bracer2 = -1;
    equipped_accessory = -1;
    tooltip = new Tooltip(6, 0, 2);
    tooltip->SetFlipEdge(630, 63);
    tooltip->SetVerticalAlign(TooltipAlign_Centered);
    tooltip->SetShowCursed(true);
}

PaperdollController::~PaperdollController()
{
}

// An inventory item dropped on the own paperdoll: sends the equip request when its class fits and its box is free,
// and armor only for the wearer's gender. The item's type picks the box, wherever it is dropped; a drop on a right-hand
// box asks for the second of a pair.
void PaperdollController::EquipDroppedItem(int item_id, int x, int y)
{
    int required_class;
    bool dropped_on_slot = false;
    char can_equip = false;
    int sub_slot = 0;
    if (is_own_character)
    {
        if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72)
            dropped_on_slot = true;
        if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72)
            dropped_on_slot = true;
        if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178)
            dropped_on_slot = true;
        if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178)
            dropped_on_slot = true;
        if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178)
            dropped_on_slot = true;
        if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240)
            dropped_on_slot = true;
        if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209)
            dropped_on_slot = true;
        if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209)
            dropped_on_slot = true;
        if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209)
        {
            sub_slot = 1;
            dropped_on_slot = true;
        }
        if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271)
            dropped_on_slot = true;
        if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240)
            dropped_on_slot = true;
        if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240)
        {
            sub_slot = 1;
            dropped_on_slot = true;
        }
        if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271)
            dropped_on_slot = true;
        if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271)
        {
            sub_slot = 1;
            dropped_on_slot = true;
        }
        if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271)
            dropped_on_slot = true;
        if (dropped_on_slot)
        {
            int item_type = MainForm->item_values->GetType(item_id);
            required_class = MainForm->item_values->GetClassRequirement(item_id);
            // The player's class must be the one the item asks for, or one that grows out of it.
            if (!MainForm->class_values->IsDescendantOf(MainForm->animation->interface_controller->status->class_id,
                required_class))
            {
                ClassValue *class_record = MainForm->class_values->GetRecord(required_class - 1);
                String required_class_name = class_record->name;
                MainForm->sfx->PlaySfx(SfxSound_Denied);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_ThisItemCanOnly) + required_class_name);
                return;
            }
            if (item_type == EifItemType_Weapon)
            {
                if (weapon_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Shield)
            {
                if (shield_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Armor)
            {
                if (armor_graphic <= 0)
                {
                    if (MainForm->item_values->GetGender(item_id) ==
                        MainForm->players->GetGender(MainForm->net_client->GetPlayerId()))
                        can_equip = true;
                    else
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                            MainForm->strings->GetText(GameText_ThisArmorDoesNot));
                }
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Hat)
            {
                if (head_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Boots)
            {
                if (boots_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Gloves)
            {
                if (gloves_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Accessory)
            {
                if (accessory_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Belt)
            {
                if (belt_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Necklace)
            {
                if (necklace_graphic <= 0)
                    can_equip = true;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            // A pair fills its left box first, then the right one
            if (item_type == EifItemType_Ring)
            {
                if (ring1_graphic <= 0 && !sub_slot)
                    can_equip = true;
                else
                    sub_slot = 1;
                if (ring2_graphic <= 0 && sub_slot == 1)
                    can_equip = true;
            }
            if (item_type == EifItemType_Ring && ring1_graphic > 0 && ring2_graphic > 0)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            if (item_type == EifItemType_Armlet)
            {
                if (armlet1_graphic <= 0 && !sub_slot)
                    can_equip = true;
                else
                    sub_slot = 1;
                if (armlet2_graphic <= 0 && sub_slot == 1)
                    can_equip = true;
            }
            if (item_type == EifItemType_Armlet && armlet1_graphic > 0 && armlet2_graphic > 0)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            if (item_type == EifItemType_Bracer)
            {
                if (bracer1_graphic <= 0 && !sub_slot)
                    can_equip = true;
                else
                    sub_slot = 1;
                if (bracer2_graphic <= 0 && sub_slot == 1)
                    can_equip = true;
            }
            if (item_type == EifItemType_Bracer && bracer1_graphic > 0 && bracer2_graphic > 0)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            if (can_equip)
            {
                MainForm->animation->interface_controller->inventory->PlayUseSound(item_type);
                MainForm->net_client->sender->SendEquipItem(item_id, sub_slot);
            }
            else
            {
                MainForm->sfx->PlaySfx(SfxSound_CommandError);
                MainForm->animation->interface_controller->HoldStatusMessages();
                if (item_type < EifItemType_Weapon || item_type > EifItemType_Bracer)
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouCannotEquipThis));
            }
        }
    }
}

// Shows the paperdoll from the paperdoll reply: the info lines, and every box empty until the slot setters fill it.
void PaperdollController::Open(int frame_row, String name_text, String home_text, String partner_text,
    String title_text, String player_class_name, String guild_text, String guild_rank_text, int new_icon_type,
    int new_admin_level, char own_character_flag)
{
    is_visible = true;
    is_dragging = false;
    player_name = name_text;
    home_name = home_text;
    partner_name = partner_text;
    title = title_text;
    class_name = player_class_name;
    guild_name = guild_text;
    guild_rank_name = guild_rank_text;
    frame_index = frame_row;
    icon_type = new_icon_type;
    admin_level = new_admin_level;
    // A text shorter than four characters is not shown
    if (partner_name.Length() < 4)
        partner_name = "";
    is_own_character = own_character_flag;
    head_graphic = -1;
    necklace_graphic = -1;
    armor_graphic = -1;
    weapon_graphic = -1;
    shield_graphic = -1;
    belt_graphic = -1;
    gloves_graphic = -1;
    boots_graphic = -1;
    ring1_graphic = -1;
    ring2_graphic = -1;
    armlet1_graphic = -1;
    armlet2_graphic = -1;
    bracer1_graphic = -1;
    bracer2_graphic = -1;
    accessory_graphic = -1;
}

void PaperdollController::Close()
{
    if (is_visible)
    {
        is_visible = false;
        MainForm->redraw_needed = true;
    }
}

// A click on the paperdoll: the left button picks up an equipped item to drag, or starts moving the window.
// Another button asks to unequip the item under the mouse. Only the own character's items, from the inventory panel,
// when the inventory has room for them; a cursed item stays on.
void PaperdollController::MouseDown(int x, int y, int button)
{
    char slot_clicked = false;
    char no_room = false;
    char cursed = false;
    int item_id = -1;
    int sub_slot = 0;
    int slot_graphic = 0;
    if (!button)
    {
        if (is_own_character && MainForm->animation->GetSubState() == GamePanel_Inventory)
        {
            if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72 &&
                head_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(head_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(head_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = head_item_id;
                    slot_graphic = head_graphic;
                    head_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72 &&
                necklace_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(necklace_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(necklace_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = necklace_item_id;
                    slot_graphic = necklace_graphic;
                    necklace_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178 &&
                weapon_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(weapon_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(weapon_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = weapon_item_id;
                    slot_graphic = weapon_graphic;
                    weapon_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178 &&
                armor_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(armor_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(armor_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = armor_item_id;
                    slot_graphic = armor_graphic;
                    armor_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178 &&
                shield_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(shield_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(shield_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = shield_item_id;
                    slot_graphic = shield_graphic;
                    shield_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240 &&
                gloves_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(gloves_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(gloves_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = gloves_item_id;
                    slot_graphic = gloves_graphic;
                    gloves_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209 &&
                belt_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(belt_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(belt_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = belt_item_id;
                    slot_graphic = belt_graphic;
                    belt_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209 &&
                ring1_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(ring1_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(ring1_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = ring1_item_id;
                    slot_graphic = ring1_graphic;
                    ring1_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209 &&
                ring2_graphic > 0)
            {
                sub_slot = 1;
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(ring2_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(ring2_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = ring2_item_id;
                    slot_graphic = ring2_graphic;
                    ring2_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271 &&
                boots_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(boots_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(boots_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = boots_item_id;
                    slot_graphic = boots_graphic;
                    boots_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240 &&
                armlet1_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(armlet1_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(armlet1_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = armlet1_item_id;
                    slot_graphic = armlet1_graphic;
                    armlet1_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240 &&
                armlet2_graphic > 0)
            {
                sub_slot = 1;
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(armlet2_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(armlet2_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = armlet2_item_id;
                    slot_graphic = armlet2_graphic;
                    armlet2_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271 &&
                bracer1_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(bracer1_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(bracer1_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = bracer1_item_id;
                    slot_graphic = bracer1_graphic;
                    bracer1_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271 &&
                bracer2_graphic > 0)
            {
                sub_slot = 1;
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(bracer2_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(bracer2_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = bracer2_item_id;
                    slot_graphic = bracer2_graphic;
                    bracer2_graphic = 0;
                    slot_clicked = true;
                }
            }
            if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271 &&
                accessory_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(accessory_item_id))
                {
                    slot_clicked = true;
                    no_room = true;
                }
                else if (MainForm->item_values->GetSpecial(accessory_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = true;
                    cursed = true;
                }
                else
                {
                    item_id = accessory_item_id;
                    slot_graphic = accessory_graphic;
                    accessory_graphic = 0;
                    slot_clicked = true;
                }
            }
        }
        else if (is_own_character && MainForm->animation->GetSubState() != GamePanel_Inventory)
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_OpenYourInventoryFirst));
        // An item that does not fit the inventory, or one that cannot be taken off, stays
        if (slot_clicked)
        {
            if (no_room || cursed)
            {
                if (no_room)
                {
                    MainForm->sfx->PlaySfx(SfxSound_Blocked);
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_CouldNotRemoveItem));
                }
                else
                {
                    MainForm->sfx->PlaySfx(SfxSound_Failed);
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_CursedItem, false),
                        MainForm->strings->GetDialogText(DialogText_CursedItem, true));
                    MainForm->edits->caret_enabled = false;
                }
            }
            else
            {
                MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                MainForm->animation->interface_controller->inventory->hover_item_size =
                    MainForm->item_values->GetSize(item_id);
                MainForm->animation->interface_controller->inventory->hover_item_id = item_id;
                MainForm->animation->interface_controller->inventory->is_dragging = true;
                MainForm->animation->interface_controller->inventory->dragging_from_paperdoll = true;
                MainForm->animation->interface_controller->inventory->paperdoll_slot_graphic = slot_graphic;
                MainForm->animation->interface_controller->inventory->paperdoll_sub_slot = sub_slot;
                MainForm->animation->interface_controller->inventory->drag_started = false;
                MainForm->animation->interface_controller->inventory->drag_delay_ticks = 0;
                MainForm->animation->interface_controller->inventory->drag_x = x;
                MainForm->animation->interface_controller->inventory->drag_y = y;
                MainForm->animation->interface_controller->inventory->drag_item_size =
                    MainForm->item_values->GetSize(item_id);
                MainForm->animation->interface_controller->inventory->drag_item_id = item_id;
                MainForm->animation->interface_controller->inventory->drag_graphic =
                    MainForm->item_values->GetGraphic(item_id) - 1;
                MainForm->redraw_needed = true;
                MainForm->dragging = true;
            }
        }
        // A click on no slot starts dragging the window.
        if (!slot_clicked && x >= window_x && x <= window_x + 360 && y >= window_y && y <= window_y + 280)
        {
            is_dragging = true;
            drag_mode = 1;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
    // Another button takes the item under the mouse off, when the inventory has room; a cursed one stays.
    else if (is_own_character)
    {
        if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72 && head_graphic > 0)
            item_id = head_item_id;
        if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72 &&
            necklace_graphic > 0)
            item_id = necklace_item_id;
        if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178 && weapon_graphic > 0)
            item_id = weapon_item_id;
        if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178 && armor_graphic > 0)
            item_id = armor_item_id;
        if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178 &&
            shield_graphic > 0)
            item_id = shield_item_id;
        if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240 &&
            gloves_graphic > 0)
            item_id = gloves_item_id;
        if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209 && belt_graphic > 0)
            item_id = belt_item_id;
        if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209 &&
            ring1_graphic > 0)
            item_id = ring1_item_id;
        if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209 &&
            ring2_graphic > 0)
        {
            item_id = ring2_item_id;
            sub_slot = 1;
        }
        if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271 &&
            boots_graphic > 0)
            item_id = boots_item_id;
        if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240 &&
            armlet1_graphic > 0)
            item_id = armlet1_item_id;
        if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240 &&
            armlet2_graphic > 0)
        {
            item_id = armlet2_item_id;
            sub_slot = 1;
        }
        if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271 &&
            bracer1_graphic > 0)
        {
            item_id = bracer1_item_id;
            sub_slot = 0;
        }
        if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271 &&
            bracer2_graphic > 0)
        {
            item_id = bracer2_item_id;
            sub_slot = 1;
        }
        if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271 &&
            accessory_graphic > 0)
            item_id = accessory_item_id;
        if (MainForm->animation->interface_controller->inventory->HasRoomForItem(item_id))
        {
            if (MainForm->item_values->GetSpecial(item_id) == ItemSpecial_Cursed)
            {
                MainForm->sfx->PlaySfx(SfxSound_Failed);
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CursedItem, false),
                    MainForm->strings->GetDialogText(DialogText_CursedItem, true));
                MainForm->edits->caret_enabled = false;
            }
            else if (item_id >= 0)
            {
                MainForm->sfx->PlaySfx(SfxSound_Unequip);
                MainForm->net_client->sender->SendUnequipItem(item_id, sub_slot);
            }
        }
        else
        {
            MainForm->sfx->PlaySfx(SfxSound_Blocked);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_CouldNotRemoveItem));
        }
    }
}

// Moves a dragged window, kept on screen; tracks the close button and the equipment box under the mouse, showing the
// equipped item's tip there.
void PaperdollController::MouseMove(int x, int y)
{
    if (is_dragging && drag_mode == 1)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 223)
            window_x = 223;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 277 && x <= window_x + 367 && y >= window_y + 254 && y <= window_y + 281 &&
        !MainForm->animation->interface_controller->inventory->dragging_from_paperdoll)
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
    if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72 &&
        !MainForm->animation->interface_controller->inventory->dragging_from_paperdoll)
    {
        if (SetHoveredSlot(PaperdollSlot_Head))
            ShowSlotInfo(GameText_HatEquipment, head_item_id, 141, 44);
        if (head_item_id > 0)
            return;
    }
    if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72)
    {
        if (SetHoveredSlot(PaperdollSlot_Necklace))
            ShowSlotInfo(GameText_NecklaceEquipment, necklace_item_id, 206, 50);
        if (necklace_item_id > 0)
            return;
    }
    if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178)
    {
        if (SetHoveredSlot(PaperdollSlot_Weapon))
            ShowSlotInfo(GameText_WeaponEquipment, weapon_item_id, 76, 131);
        if (weapon_item_id > 0)
            return;
    }
    if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178)
    {
        if (SetHoveredSlot(PaperdollSlot_Armor))
            ShowSlotInfo(GameText_ArmorEquipment, armor_item_id, 141, 131);
        if (armor_item_id > 0)
            return;
    }
    if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178)
    {
        if (SetHoveredSlot(PaperdollSlot_Shield))
            ShowSlotInfo(GameText_ShieldEquipment, shield_item_id, 206, 131);
        if (shield_item_id > 0)
            return;
    }
    if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240)
    {
        if (SetHoveredSlot(PaperdollSlot_Gloves))
            ShowSlotInfo(GameText_GlovesEquipment, gloves_item_id, 76, 212);
        if (gloves_item_id > 0)
            return;
    }
    if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209)
    {
        if (SetHoveredSlot(PaperdollSlot_Belt))
            ShowSlotInfo(GameText_BeltEquipment, belt_item_id, 141, 187);
        if (belt_item_id > 0)
            return;
    }
    if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209)
    {
        if (SetHoveredSlot(PaperdollSlot_Text71Left))
            ShowSlotInfo(GameText_RingEquipment, ring1_item_id, 173, 187);
        if (ring1_item_id > 0)
            return;
    }
    if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209)
    {
        if (SetHoveredSlot(PaperdollSlot_Text71Right))
            ShowSlotInfo(GameText_RingEquipment, ring2_item_id, 206, 187);
        if (ring2_item_id > 0)
            return;
    }
    if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271)
    {
        if (SetHoveredSlot(PaperdollSlot_Boots))
            ShowSlotInfo(GameText_BootsEquipment, boots_item_id, 141, 243);
        if (boots_item_id > 0)
            return;
    }
    if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240)
    {
        if (SetHoveredSlot(PaperdollSlot_Text72Left))
            ShowSlotInfo(GameText_ArmletEquipment, armlet1_item_id, 173, 218);
        if (armlet1_item_id > 0)
            return;
    }
    if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240)
    {
        if (SetHoveredSlot(PaperdollSlot_Text72Right))
            ShowSlotInfo(GameText_ArmletEquipment, armlet2_item_id, 206, 218);
        if (armlet2_item_id > 0)
            return;
    }
    if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271)
    {
        if (SetHoveredSlot(PaperdollSlot_Text73Left))
            ShowSlotInfo(GameText_BracerEquipment, bracer1_item_id, 173, 249);
        if (bracer1_item_id > 0)
            return;
    }
    if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271)
    {
        if (SetHoveredSlot(PaperdollSlot_Text73Right))
            ShowSlotInfo(GameText_BracerEquipment, bracer2_item_id, 206, 249);
        if (bracer2_item_id > 0)
            return;
    }
    if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271)
    {
        if (SetHoveredSlot(PaperdollSlot_Text76))
            ShowSlotInfo(76, accessory_item_id, 76, 249);
        if (accessory_item_id > 0)
            return;
    }
    if (tooltip->Hide())
        MainForm->redraw_needed = true;
    hovered_slot = PaperdollSlot_None;
}

bool PaperdollController::SetHoveredSlot(unsigned char slot)
{
    if (slot != hovered_slot)
    {
        hovered_slot = slot;
        return true;
    }
    return false;
}

bool PaperdollController::HasHoveredSlot()
{
    if (hovered_slot > PaperdollSlot_NoSlot)
        return true;
    return false;
}

// Puts the paperdoll slot's name on the status line, with the worn item's name and weight and the weight carried,
// and shows the item's tip.
void PaperdollController::ShowSlotInfo(unsigned char text_id, int item_id, int x_offset, int y_offset)
{
    String message = MainForm->strings->GetText(text_id);
    if (item_id > 0)
    {
        String carried_text = MainForm->animation->interface_controller->status->GetWeightText();
        tooltip->ShowItem(item_id);
        tooltip->SetAnchorX(window_x + x_offset);
        tooltip->SetAnchorY(window_y + y_offset);
        message += ", " + MainForm->item_values->GetName(item_id);
        message += ", " + IntToStr(MainForm->item_values->GetWeight(item_id)) + " weight, ";
        message += carried_text;
    }
    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, message);
    MainForm->redraw_needed = true;
}

// Ends moving the window.
void PaperdollController::MouseUp()
{
    is_dragging = false;
}

void PaperdollController::SetHeadSlot(int item_id)
{
    head_item_id = item_id;
    head_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    head_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetNecklaceSlot(int item_id)
{
    necklace_item_id = item_id;
    necklace_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    necklace_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetArmorSlot(int item_id)
{
    armor_item_id = item_id;
    armor_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    armor_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetWeaponSlot(int item_id)
{
    weapon_item_id = item_id;
    weapon_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    weapon_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetShieldSlot(int item_id)
{
    shield_item_id = item_id;
    shield_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    shield_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetBeltSlot(int item_id)
{
    belt_item_id = item_id;
    belt_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    belt_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetGlovesSlot(int item_id)
{
    gloves_item_id = item_id;
    gloves_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    gloves_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetBootsSlot(int item_id)
{
    boots_item_id = item_id;
    boots_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    boots_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetRing1Slot(int item_id)
{
    ring1_item_id = item_id;
    ring1_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    ring1_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetRing2Slot(int item_id)
{
    ring2_item_id = item_id;
    ring2_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    ring2_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetArmlet1Slot(int item_id)
{
    armlet1_item_id = item_id;
    armlet1_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    armlet1_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetArmlet2Slot(int item_id)
{
    armlet2_item_id = item_id;
    armlet2_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    armlet2_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetBracer1Slot(int item_id)
{
    bracer1_item_id = item_id;
    bracer1_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    bracer1_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetBracer2Slot(int item_id)
{
    bracer2_item_id = item_id;
    bracer2_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    bracer2_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetAccessorySlot(int item_id)
{
    accessory_item_id = item_id;
    accessory_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    accessory_item_size = MainForm->item_values->GetSize(item_id);
}

// Empties every box whose item cannot be taken off; true when the weapon, shield, armor, head or boots box changed.
bool PaperdollController::RemoveCursedItems()
{
    bool removed = false;
    // Special 5 marks a cursed item.
    if (MainForm->item_values->GetSpecial(weapon_item_id) == ItemSpecial_Cursed)
    {
        SetWeaponSlot(ItemId_None);
        equipped_weapon = 0;
        removed = true;
    }
    if (MainForm->item_values->GetSpecial(shield_item_id) == ItemSpecial_Cursed)
    {
        SetShieldSlot(ItemId_None);
        equipped_shield = 0;
        removed = true;
    }
    if (MainForm->item_values->GetSpecial(armor_item_id) == ItemSpecial_Cursed)
    {
        SetArmorSlot(ItemId_None);
        equipped_armor = 0;
        removed = true;
    }
    if (MainForm->item_values->GetSpecial(head_item_id) == ItemSpecial_Cursed)
    {
        SetHeadSlot(ItemId_None);
        equipped_head = 0;
        removed = true;
    }
    if (MainForm->item_values->GetSpecial(boots_item_id) == ItemSpecial_Cursed)
    {
        SetBootsSlot(ItemId_None);
        equipped_boots = 0;
        removed = true;
    }
    if (MainForm->item_values->GetSpecial(gloves_item_id) == ItemSpecial_Cursed)
    {
        SetGlovesSlot(ItemId_None);
        equipped_gloves = 0;
    }
    if (MainForm->item_values->GetSpecial(necklace_item_id) == ItemSpecial_Cursed)
    {
        SetNecklaceSlot(ItemId_None);
        equipped_necklace = 0;
    }
    if (MainForm->item_values->GetSpecial(belt_item_id) == ItemSpecial_Cursed)
    {
        SetBeltSlot(ItemId_None);
        equipped_belt = 0;
    }
    if (MainForm->item_values->GetSpecial(ring1_item_id) == ItemSpecial_Cursed)
    {
        SetRing1Slot(ItemId_None);
        equipped_ring1 = 0;
    }
    if (MainForm->item_values->GetSpecial(ring2_item_id) == ItemSpecial_Cursed)
    {
        SetRing2Slot(ItemId_None);
        equipped_ring2 = 0;
    }
    if (MainForm->item_values->GetSpecial(armlet1_item_id) == ItemSpecial_Cursed)
    {
        SetArmlet1Slot(ItemId_None);
        equipped_armlet1 = 0;
    }
    if (MainForm->item_values->GetSpecial(armlet2_item_id) == ItemSpecial_Cursed)
    {
        SetArmlet2Slot(ItemId_None);
        equipped_armlet2 = 0;
    }
    if (MainForm->item_values->GetSpecial(bracer1_item_id) == ItemSpecial_Cursed)
    {
        SetBracer1Slot(ItemId_None);
        equipped_bracer1 = 0;
    }
    if (MainForm->item_values->GetSpecial(bracer2_item_id) == ItemSpecial_Cursed)
    {
        SetBracer2Slot(ItemId_None);
        equipped_bracer2 = 0;
    }
    if (MainForm->item_values->GetSpecial(accessory_item_id) == ItemSpecial_Cursed)
    {
        SetAccessorySlot(ItemId_None);
        equipped_accessory = 0;
    }
    MainForm->redraw_needed = true;
    return removed;
}
