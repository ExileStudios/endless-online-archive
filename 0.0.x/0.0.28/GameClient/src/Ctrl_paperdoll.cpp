#include <vcl.h>
#pragma hdrstop

#include "Ctrl_paperdoll.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

PaperdollController::PaperdollController()
{
    window_x = 133;
    window_y = 13;
    frame_index = 0;
    extra_value = 0;
    is_visible = 0;
    close_hovered = 0;
    hovered_slot = PaperdollSlot_None;
    unused_flag = 1;
    equipped_head = -1;
    equipped_necklace = -1;
    equipped_armor = -1;
    equipped_weapon = -1;
    equipped_shield = -1;
    equipped_belt = -1;
    equipped_gloves = -1;
    equipped_boots = -1;
    equipped_type19_left = -1;
    equipped_type19_right = -1;
    equipped_type20_left = -1;
    equipped_type20_right = -1;
    equipped_type21_left = -1;
    equipped_type21_right = -1;
    equipped_type16 = -1;
}

PaperdollController::~PaperdollController()
{
}

// An inventory item dropped on the own paperdoll: sends the equip request when its class fits and its box is free.
// The item's type picks the box, wherever it is dropped; a drop on a right-hand box asks for the second of a pair.
void PaperdollController::EquipDroppedItem(int item_id, int x, int y)
{
    char can_equip;
    int sub_slot;
    int item_type;
    int required_class;
    bool dropped_on_slot = 0;
    can_equip = 0;
    sub_slot = 0;
    if (is_own_character)
    {
        if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72)
            dropped_on_slot = 1;
        if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72)
            dropped_on_slot = 1;
        if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178)
            dropped_on_slot = 1;
        if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178)
            dropped_on_slot = 1;
        if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178)
            dropped_on_slot = 1;
        if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240)
            dropped_on_slot = 1;
        if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209)
            dropped_on_slot = 1;
        if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209)
            dropped_on_slot = 1;
        if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209)
        {
            sub_slot = 1;
            dropped_on_slot = 1;
        }
        if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271)
            dropped_on_slot = 1;
        if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240)
            dropped_on_slot = 1;
        if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240)
        {
            sub_slot = 1;
            dropped_on_slot = 1;
        }
        if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271)
            dropped_on_slot = 1;
        if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271)
        {
            sub_slot = 1;
            dropped_on_slot = 1;
        }
        if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271)
            dropped_on_slot = 1;
        if (dropped_on_slot)
        {
            item_type = MainForm->item_values->GetType(item_id);
            required_class = MainForm->item_values->GetClassRequirement(item_id);
            if (!MainForm->class_values->IsDescendantOf(MainForm->animation->interface_controller->status->class_id,
                required_class))
            {
                String required_class_name = MainForm->class_values->GetName(required_class);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_ThisItemCanOnly) + required_class_name);
                return;
            }
            if (item_type == EifItemType_Weapon)
            {
                if (weapon_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Shield)
            {
                if (shield_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Armor)
            {
                if (armor_graphic <= 0)
                {
                    required_class = MainForm->item_values->GetGender(item_id);
                    if (required_class == MainForm->players->GetGender(MainForm->net_client->GetPlayerId()))
                        can_equip = 1;
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
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Boots)
            {
                if (boots_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Gloves)
            {
                if (gloves_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Accessory)
            {
                if (type16_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Belt)
            {
                if (belt_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            if (item_type == EifItemType_Necklace)
            {
                if (necklace_graphic <= 0)
                    can_equip = 1;
                else
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            }
            // A pair fills its left box first, then the right one
            if (item_type == EifItemType_Ring)
            {
                if (type19_left_graphic <= 0 && !sub_slot)
                    can_equip = 1;
                else
                    sub_slot = 1;
                if (type19_right_graphic <= 0 && sub_slot == 1)
                    can_equip = 1;
            }
            if (item_type == EifItemType_Ring && type19_left_graphic > 0 && type19_right_graphic > 0)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            if (item_type == EifItemType_Armlet)
            {
                if (type20_left_graphic <= 0 && !sub_slot)
                    can_equip = 1;
                else
                    sub_slot = 1;
                if (type20_right_graphic <= 0 && sub_slot == 1)
                    can_equip = 1;
            }
            if (item_type == EifItemType_Armlet && type20_left_graphic > 0 && type20_right_graphic > 0)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            if (item_type == EifItemType_Bracer)
            {
                if (type21_left_graphic <= 0 && !sub_slot)
                    can_equip = 1;
                else
                    sub_slot = 1;
                if (type21_right_graphic <= 0 && sub_slot == 1)
                    can_equip = 1;
            }
            if (item_type == EifItemType_Bracer && type21_left_graphic > 0 && type21_right_graphic > 0)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouAlreadyHaveItem));
            if (can_equip)
                MainForm->net_client->sender->SendEquipItem(item_id, sub_slot);
            // Items that are not equipment get their own message
            else if (item_type < EifItemType_Weapon || item_type > EifItemType_Bracer)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouCannotEquipThis));
        }
    }
}

// Shows the paperdoll from the paperdoll reply: the info lines, and every box empty until the slot setters fill it.
void PaperdollController::Open(int frame_value, String line_1_text, String line_2_text, String line_4_text,
    String line_5_text, String player_class_name, String line_7_text, String line_8_text, int new_icon_type,
    int unused_value, char own_character_flag)
{
    is_visible = 1;
    info_line_1 = line_1_text;
    info_line_2 = line_2_text;
    info_line_4 = line_4_text;
    info_line_5 = line_5_text;
    class_name = player_class_name;
    info_line_7 = line_7_text;
    info_line_8 = line_8_text;
    frame_index = frame_value;
    icon_type = new_icon_type;
    extra_value = unused_value;
    // A text shorter than four characters is not shown
    if (info_line_4.Length() < 4)
        info_line_4 = "";
    is_own_character = own_character_flag;
    head_graphic = -1;
    necklace_graphic = -1;
    armor_graphic = -1;
    weapon_graphic = -1;
    shield_graphic = -1;
    belt_graphic = -1;
    gloves_graphic = -1;
    boots_graphic = -1;
    type19_left_graphic = -1;
    type19_right_graphic = -1;
    type20_left_graphic = -1;
    type20_right_graphic = -1;
    type21_left_graphic = -1;
    type21_right_graphic = -1;
    type16_graphic = -1;
}

// A click on the paperdoll: the left button picks up an equipped item to drag, or starts moving the window.
// Another button asks to unequip the item under the mouse. Only the own character's items, from the inventory panel.
void PaperdollController::MouseDown(int x, int y, int button)
{
    char slot_clicked = 0;
    char no_room = 0;
    char cursed = 0;
    int item_id = 0;
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
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(head_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = head_item_id;
                    slot_graphic = head_graphic;
                    head_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72 &&
                necklace_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(necklace_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(necklace_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = necklace_item_id;
                    slot_graphic = necklace_graphic;
                    necklace_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178 &&
                weapon_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(weapon_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(weapon_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = weapon_item_id;
                    slot_graphic = weapon_graphic;
                    weapon_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178 &&
                armor_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(armor_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(armor_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = armor_item_id;
                    slot_graphic = armor_graphic;
                    armor_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178 &&
                shield_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(shield_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(shield_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = shield_item_id;
                    slot_graphic = shield_graphic;
                    shield_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240 &&
                gloves_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(gloves_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(gloves_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = gloves_item_id;
                    slot_graphic = gloves_graphic;
                    gloves_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209 &&
                belt_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(belt_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(belt_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = belt_item_id;
                    slot_graphic = belt_graphic;
                    belt_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209 &&
                type19_left_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type19_left_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type19_left_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type19_left_item_id;
                    slot_graphic = type19_left_graphic;
                    type19_left_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209 &&
                type19_right_graphic > 0)
            {
                sub_slot = 1;
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type19_right_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type19_right_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type19_right_item_id;
                    slot_graphic = type19_right_graphic;
                    type19_right_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271 &&
                boots_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(boots_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(boots_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = boots_item_id;
                    slot_graphic = boots_graphic;
                    boots_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240 &&
                type20_left_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type20_left_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type20_left_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type20_left_item_id;
                    slot_graphic = type20_left_graphic;
                    type20_left_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240 &&
                type20_right_graphic > 0)
            {
                sub_slot = 1;
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type20_right_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type20_right_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type20_right_item_id;
                    slot_graphic = type20_right_graphic;
                    type20_right_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271 &&
                type21_left_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type21_left_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type21_left_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type21_left_item_id;
                    slot_graphic = type21_left_graphic;
                    type21_left_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271 &&
                type21_right_graphic > 0)
            {
                sub_slot = 1;
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type21_right_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type21_right_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type21_right_item_id;
                    slot_graphic = type21_right_graphic;
                    type21_right_graphic = 0;
                    slot_clicked = 1;
                }
            }
            if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271 &&
                type16_graphic > 0)
            {
                if (!MainForm->animation->interface_controller->inventory->HasRoomForItem(type16_item_id))
                {
                    slot_clicked = 1;
                    no_room = 1;
                }
                else if (MainForm->item_values->GetSpecial(type16_item_id) == ItemSpecial_Cursed)
                {
                    slot_clicked = 1;
                    cursed = 1;
                }
                else
                {
                    item_id = type16_item_id;
                    slot_graphic = type16_graphic;
                    type16_graphic = 0;
                    slot_clicked = 1;
                }
            }
        }
        else if (is_own_character)
        {
            if (MainForm->animation->GetSubState() != GamePanel_Inventory)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_OpenYourInventoryFirst));
        }
        // An item that does not fit the inventory, or one that cannot be taken off, stays
        if (slot_clicked)
        {
            if (no_room || cursed)
            {
                if (no_room)
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_CouldNotRemoveItem));
                else
                {
                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_CursedItem, false),
                        MainForm->strings->GetDialogText(DialogText_CursedItem, true));
                    MainForm->edits->caret_enabled = false;
                }
            }
            else
            {
                MainForm->sfx->PlaySfx(20);
                MainForm->animation->interface_controller->inventory->hover_item_size = MainForm->item_values->GetSize(item_id);
                MainForm->animation->interface_controller->inventory->hover_item_id = item_id;
                MainForm->animation->interface_controller->inventory->is_dragging = 1;
                MainForm->animation->interface_controller->inventory->dragging_from_paperdoll = true;
                MainForm->animation->interface_controller->inventory->paperdoll_slot_graphic = slot_graphic;
                MainForm->animation->interface_controller->inventory->paperdoll_sub_slot = sub_slot;
                MainForm->animation->interface_controller->inventory->drag_started = 0;
                MainForm->animation->interface_controller->inventory->drag_delay_ticks = 0;
                MainForm->animation->interface_controller->inventory->drag_x = x;
                MainForm->animation->interface_controller->inventory->drag_y = y;
                MainForm->animation->interface_controller->inventory->drag_item_size = MainForm->item_values->GetSize(item_id);
                MainForm->animation->interface_controller->inventory->drag_item_id = item_id;
                MainForm->animation->interface_controller->inventory->drag_graphic = MainForm->item_values->GetGraphic(item_id) -
                    1;
                MainForm->redraw_needed = true;
                MainForm->dragging = true;
            }
        }
        if (!slot_clicked && x >= window_x)
        {
            if (x <= window_x + 360 && y >= window_y && y <= window_y + 280)
            {
                is_dragging = 1;
                drag_mode = 1;
                drag_offset_x = x - window_x;
                drag_offset_y = y - window_y;
            }
        }
    }
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
            type19_left_graphic > 0)
            item_id = type19_left_item_id;
        if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209 &&
            type19_right_graphic > 0)
        {
            item_id = type19_right_item_id;
            sub_slot = 1;
        }
        if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271 &&
            boots_graphic > 0)
            item_id = boots_item_id;
        if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240 &&
            type20_left_graphic > 0)
            item_id = type20_left_item_id;
        if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240 &&
            type20_right_graphic > 0)
        {
            item_id = type20_right_item_id;
            sub_slot = 1;
        }
        if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271 &&
            type21_left_graphic > 0)
            item_id = type21_left_item_id;
        if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271 &&
            type21_right_graphic > 0)
        {
            item_id = type21_right_item_id;
            sub_slot = 1;
        }
        if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271 &&
            type16_graphic > 0)
            item_id = type16_item_id;
        if (MainForm->animation->interface_controller->inventory->HasRoomForItem(item_id))
        {
            if (MainForm->item_values->GetSpecial(item_id) == ItemSpecial_Cursed)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CursedItem, false),
                    MainForm->strings->GetDialogText(DialogText_CursedItem, true));
                MainForm->edits->caret_enabled = false;
            }
            else
                MainForm->net_client->sender->SendUnequipItem(item_id, sub_slot);
        }
        else
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_CouldNotRemoveItem));
    }
}

// Moves a dragged window, kept on screen, tracks the close button, and names the hovered box in the status bar.
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
        if (window_x >= 250)
            window_x = 250;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 277 && x <= window_x + 367 && y >= window_y + 254 && y <= window_y + 281 &&
        !MainForm->animation->interface_controller->inventory->dragging_from_paperdoll)
    {
        if (!close_hovered)
        {
            close_hovered = 1;
            MainForm->redraw_needed = true;
        }
    }
    else if (close_hovered)
    {
        close_hovered = 0;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 86 && y >= window_y + 19 && x <= window_x + 141 && y <= window_y + 72 &&
        !MainForm->animation->interface_controller->inventory->dragging_from_paperdoll && hovered_slot != PaperdollSlot_Head)
    {
        String head_text = MainForm->strings->GetText(GameText_HatEquipment);
        if (head_item_id > 0)
            head_text = head_text + ", " + MainForm->item_values->GetName(head_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, head_text);
        hovered_slot = PaperdollSlot_Head;
    }
    if (x >= window_x + 151 && y >= window_y + 50 && x <= window_x + 206 && y <= window_y + 72 && hovered_slot != PaperdollSlot_Necklace)
    {
        String necklace_text = MainForm->strings->GetText(GameText_NecklaceEquipment);
        if (necklace_item_id > 0)
            necklace_text = necklace_text + ", " + MainForm->item_values->GetName(necklace_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, necklace_text);
        hovered_slot = PaperdollSlot_Necklace;
    }
    if (x >= window_x + 21 && y >= window_y + 81 && x <= window_x + 76 && y <= window_y + 178 && hovered_slot != PaperdollSlot_Weapon)
    {
        String weapon_text = MainForm->strings->GetText(GameText_WeaponEquipment);
        if (weapon_item_id > 0)
            weapon_text = weapon_text + ", " + MainForm->item_values->GetName(weapon_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, weapon_text);
        hovered_slot = PaperdollSlot_Weapon;
    }
    if (x >= window_x + 86 && y >= window_y + 81 && x <= window_x + 141 && y <= window_y + 178 && hovered_slot != PaperdollSlot_Armor)
    {
        String armor_text = MainForm->strings->GetText(GameText_ArmorEquipment);
        if (armor_item_id > 0)
            armor_text = armor_text + ", " + MainForm->item_values->GetName(armor_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, armor_text);
        hovered_slot = PaperdollSlot_Armor;
    }
    if (x >= window_x + 151 && y >= window_y + 81 && x <= window_x + 206 && y <= window_y + 178 && hovered_slot != PaperdollSlot_Shield)
    {
        String shield_text = MainForm->strings->GetText(GameText_ShieldEquipment);
        if (shield_item_id > 0)
            shield_text = shield_text + ", " + MainForm->item_values->GetName(shield_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, shield_text);
        hovered_slot = PaperdollSlot_Shield;
    }
    if (x >= window_x + 21 && y >= window_y + 187 && x <= window_x + 76 && y <= window_y + 240 && hovered_slot != PaperdollSlot_Gloves)
    {
        String gloves_text = MainForm->strings->GetText(GameText_GlovesEquipment);
        if (gloves_item_id > 0)
            gloves_text = gloves_text + ", " + MainForm->item_values->GetName(gloves_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, gloves_text);
        hovered_slot = PaperdollSlot_Gloves;
    }
    if (x >= window_x + 86 && y >= window_y + 187 && x <= window_x + 141 && y <= window_y + 209 && hovered_slot != PaperdollSlot_Belt)
    {
        String belt_text = MainForm->strings->GetText(GameText_BeltEquipment);
        if (belt_item_id > 0)
            belt_text = belt_text + ", " + MainForm->item_values->GetName(belt_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, belt_text);
        hovered_slot = PaperdollSlot_Belt;
    }
    if (x >= window_x + 151 && y >= window_y + 187 && x <= window_x + 173 && y <= window_y + 209 && hovered_slot != PaperdollSlot_Text71Left)
    {
        String type19_left_text = MainForm->strings->GetText(GameText_RingEquipment);
        if (type19_left_item_id > 0)
            type19_left_text = type19_left_text + ", " + MainForm->item_values->GetName(type19_left_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type19_left_text);
        hovered_slot = PaperdollSlot_Text71Left;
    }
    if (x >= window_x + 184 && y >= window_y + 187 && x <= window_x + 206 && y <= window_y + 209 && hovered_slot != PaperdollSlot_Text71Right)
    {
        String type19_right_text = MainForm->strings->GetText(GameText_RingEquipment);
        if (type19_right_item_id > 0)
            type19_right_text = type19_right_text + ", " + MainForm->item_values->GetName(type19_right_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type19_right_text);
        hovered_slot = PaperdollSlot_Text71Right;
    }
    if (x >= window_x + 86 && y >= window_y + 218 && x <= window_x + 141 && y <= window_y + 271 && hovered_slot != PaperdollSlot_Boots)
    {
        String boots_text = MainForm->strings->GetText(GameText_BootsEquipment);
        if (boots_item_id > 0)
            boots_text = boots_text + ", " + MainForm->item_values->GetName(boots_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, boots_text);
        hovered_slot = PaperdollSlot_Boots;
    }
    if (x >= window_x + 151 && y >= window_y + 218 && x <= window_x + 173 && y <= window_y + 240 && hovered_slot != PaperdollSlot_Text72Left)
    {
        String type20_left_text = MainForm->strings->GetText(GameText_ArmletEquipment);
        if (type20_left_item_id > 0)
            type20_left_text = type20_left_text + ", " + MainForm->item_values->GetName(type20_left_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type20_left_text);
        hovered_slot = PaperdollSlot_Text72Left;
    }
    if (x >= window_x + 184 && y >= window_y + 218 && x <= window_x + 206 && y <= window_y + 240 && hovered_slot != PaperdollSlot_Text72Right)
    {
        String type20_right_text = MainForm->strings->GetText(GameText_ArmletEquipment);
        if (type20_right_item_id > 0)
            type20_right_text = type20_right_text + ", " + MainForm->item_values->GetName(type20_right_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type20_right_text);
        hovered_slot = PaperdollSlot_Text72Right;
    }
    if (x >= window_x + 151 && y >= window_y + 249 && x <= window_x + 173 && y <= window_y + 271 && hovered_slot != PaperdollSlot_Text73Left)
    {
        String type21_left_text = MainForm->strings->GetText(GameText_BracerEquipment);
        if (type21_left_item_id > 0)
            type21_left_text = type21_left_text + ", " + MainForm->item_values->GetName(type21_left_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type21_left_text);
        hovered_slot = PaperdollSlot_Text73Left;
    }
    if (x >= window_x + 184 && y >= window_y + 249 && x <= window_x + 206 && y <= window_y + 271 && hovered_slot != PaperdollSlot_Text73Right)
    {
        String type21_right_text = MainForm->strings->GetText(GameText_BracerEquipment);
        if (type21_right_item_id > 0)
            type21_right_text = type21_right_text + ", " + MainForm->item_values->GetName(type21_right_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type21_right_text);
        hovered_slot = PaperdollSlot_Text73Right;
    }
    if (x >= window_x + 54 && y >= window_y + 249 && x <= window_x + 76 && y <= window_y + 271 && hovered_slot != PaperdollSlot_Text76)
    {
        String type16_text = MainForm->strings->GetText(GameText_MiscellaneousEquipment);
        if (type16_item_id > 0)
            type16_text = type16_text + ", " + MainForm->item_values->GetName(type16_item_id);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, type16_text);
        hovered_slot = PaperdollSlot_Text76;
    }
}

// Ends moving the window.
void PaperdollController::MouseUp()
{
    is_dragging = 0;
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

void PaperdollController::SetType19LeftSlot(int item_id)
{
    type19_left_item_id = item_id;
    type19_left_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type19_left_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetType19RightSlot(int item_id)
{
    type19_right_item_id = item_id;
    type19_right_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type19_right_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetType20LeftSlot(int item_id)
{
    type20_left_item_id = item_id;
    type20_left_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type20_left_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetType20RightSlot(int item_id)
{
    type20_right_item_id = item_id;
    type20_right_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type20_right_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetType21LeftSlot(int item_id)
{
    type21_left_item_id = item_id;
    type21_left_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type21_left_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetType21RightSlot(int item_id)
{
    type21_right_item_id = item_id;
    type21_right_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type21_right_item_size = MainForm->item_values->GetSize(item_id);
}

void PaperdollController::SetType16Slot(int item_id)
{
    type16_item_id = item_id;
    type16_graphic = MainForm->item_values->GetGraphic(item_id) - 1;
    type16_item_size = MainForm->item_values->GetSize(item_id);
}

// Empties every box whose item cannot be taken off; true when the weapon, shield, armor, head or boots box changed.
bool PaperdollController::RemoveCursedItems()
{
    bool removed = 0;
    if (MainForm->item_values->GetSpecial(weapon_item_id) == ItemSpecial_Cursed)
    {
        SetWeaponSlot(0);
        equipped_weapon = 0;
        removed = 1;
    }
    if (MainForm->item_values->GetSpecial(shield_item_id) == ItemSpecial_Cursed)
    {
        SetShieldSlot(0);
        equipped_shield = 0;
        removed = 1;
    }
    if (MainForm->item_values->GetSpecial(armor_item_id) == ItemSpecial_Cursed)
    {
        SetArmorSlot(0);
        equipped_armor = 0;
        removed = 1;
    }
    if (MainForm->item_values->GetSpecial(head_item_id) == ItemSpecial_Cursed)
    {
        SetHeadSlot(0);
        equipped_head = 0;
        removed = 1;
    }
    if (MainForm->item_values->GetSpecial(boots_item_id) == ItemSpecial_Cursed)
    {
        SetBootsSlot(0);
        equipped_boots = 0;
        removed = 1;
    }
    if (MainForm->item_values->GetSpecial(gloves_item_id) == ItemSpecial_Cursed)
    {
        SetGlovesSlot(0);
        equipped_gloves = 0;
    }
    if (MainForm->item_values->GetSpecial(necklace_item_id) == ItemSpecial_Cursed)
    {
        SetNecklaceSlot(0);
        equipped_necklace = 0;
    }
    if (MainForm->item_values->GetSpecial(belt_item_id) == ItemSpecial_Cursed)
    {
        SetBeltSlot(0);
        equipped_belt = 0;
    }
    if (MainForm->item_values->GetSpecial(type19_left_item_id) == ItemSpecial_Cursed)
    {
        SetType19LeftSlot(0);
        equipped_type19_left = 0;
    }
    if (MainForm->item_values->GetSpecial(type19_right_item_id) == ItemSpecial_Cursed)
    {
        SetType19RightSlot(0);
        equipped_type19_right = 0;
    }
    if (MainForm->item_values->GetSpecial(type20_left_item_id) == ItemSpecial_Cursed)
    {
        SetType20LeftSlot(0);
        equipped_type20_left = 0;
    }
    if (MainForm->item_values->GetSpecial(type20_right_item_id) == ItemSpecial_Cursed)
    {
        SetType20RightSlot(0);
        equipped_type20_right = 0;
    }
    if (MainForm->item_values->GetSpecial(type21_left_item_id) == ItemSpecial_Cursed)
    {
        SetType21LeftSlot(0);
        equipped_type21_left = 0;
    }
    if (MainForm->item_values->GetSpecial(type21_right_item_id) == ItemSpecial_Cursed)
    {
        SetType21RightSlot(0);
        equipped_type21_right = 0;
    }
    if (MainForm->item_values->GetSpecial(type16_item_id) == ItemSpecial_Cursed)
    {
        SetType16Slot(0);
        equipped_type16 = 0;
    }
    MainForm->redraw_needed = true;
    return removed;
}
