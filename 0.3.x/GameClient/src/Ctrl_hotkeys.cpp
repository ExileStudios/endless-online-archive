#include <vcl.h>
#pragma hdrstop

#include "Ctrl_hotkeys.h"
#include "Tmainform.h"
#include "Ctrl_interface.h"
#include "Ctrl_inventory.h"
#include "Ctrl_spells.h"
#include "Ctrl_tooltip.h"

#pragma package(smart_init)

HotkeysController::HotkeysController()
{
    window_x = 97;
    window_y = 135;
    is_visible = false;
    close_hovered = false;
    left_grid_x = 18;
    right_grid_x = 232;
    grid_y = 25;
    slot_width = 51;
    slot_height = 46;
    tooltip = new Tooltip(6, 18, 2);
    tooltip->SetFlipEdge(600, 52);
    tooltip->SetBottomLimit(310, 23);
    tooltip->SetShowCursed(false);
}

HotkeysController::~HotkeysController()
{
}

// Puts an item dropped on the hotkey bar into the slot under it and saves the bar; an item with no hotkey use is
// refused with a message.
void HotkeysController::DropItem(int item_id, int x, int y)
{
    if (item_id <= 0)
        return;
    if ((unsigned int)MainForm->item_values->Count() < item_id)
        return;
    if (!IsHotkeyItem(item_id))
    {
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            "The dragged item does not have a valid hotkey usage");
        return;
    }
    HitTest(x, y);
    int slot = 8 * hovered_row + hovered_column;
    if (slot < 0)
        slot = 0;
    if (slot > 15)
        slot = 15;
    MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    slots[slot].kind = 1;
    slots[slot].id = item_id;
    int tip_x = window_x + left_grid_x;
    tip_x = tip_x + 45 * hovered_column + 45;
    int tip_y = window_y + grid_y + 45 * hovered_row;
    if (hovered_column > 3)
        tip_x += 8;
    tooltip->Hide();
    tooltip->ShowItem(item_id);
    tooltip->SetAnchorX(tip_x);
    tooltip->SetAnchorY(tip_y);
    MainForm->redraw_needed = true;
    SaveHotkeys();
}

// Puts the dropped spell in the hotkey slot under the mouse, shows its tip beside the slot and saves the hotkeys.
void HotkeysController::DropSpell(int spell_id, int x, int y)
{
    HitTest(x, y);
    int slot = 8 * hovered_row + hovered_column;
    if (slot < 0)
        slot = 0;
    if (slot > 15)
        slot = 15;
    MainForm->sfx->PlaySfx(SfxSound_Select);
    slots[slot].kind = 2;
    slots[slot].id = spell_id;
    int tip_x = window_x + left_grid_x;
    tip_x = tip_x + 45 * hovered_column + 45;
    int tip_y = window_y + grid_y + 45 * hovered_row;
    // The right grid sits 8 pixels further over.
    if (hovered_column > 3)
        tip_x += 8;
    tooltip->Hide();
    tooltip->ShowSpell(spell_id);
    tooltip->SetAnchorX(tip_x);
    tooltip->SetAnchorY(tip_y);
    MainForm->redraw_needed = true;
    SaveHotkeys();
}

void HotkeysController::Open()
{
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    is_visible = true;
    is_dragging = false;
    MainForm->redraw_needed = true;
}

DEFINE_HOTKEYS_CLOSE(HotkeysController)

// A click on the hotkey bar: a left click on a filled slot tells how to delete it, a right click empties the slot and
// saves the bar.
void HotkeysController::MouseDown(int x, int y, int button)
{
    int hit_slot;
    if (!button)
    {
        if (x >= window_x && x <= window_x + 455 && y >= window_y && y <= window_y + 170)
        {
            HitTest(x, y);
            if (hovered_column >= 0)
            {
                int slot = 8 * hovered_row + hovered_column;
                if (slot < 0)
                    slot = 0;
                if (slot > 15)
                    slot = 15;
                if (slots[slot].id > 0)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                        "Right click on a hotkey to delete it.");
                    return;
                }
            }
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
    if (button == 1)
    {
        HitTest(x, y);
        if (hovered_column >= 0)
        {
            hit_slot = 8 * hovered_row + hovered_column;
            if (hit_slot < 0)
                hit_slot = 0;
            if (hit_slot > 15)
                hit_slot = 15;
            if (slots[hit_slot].id > 0)
            {
                MainForm->sfx->PlaySfx(SfxSound_Send);
                slots[hit_slot].id = -1;
                tooltip->Hide();
                MainForm->redraw_needed = true;
                SaveHotkeys();
            }
        }
    }
}

// Moves a dragged hotkey bar, kept in the game area, lights the close button, and updates the slot tip.
void HotkeysController::MouseMove(int x, int y)
{
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 174)
            window_x = 174;
        if (window_y >= 139)
            window_y = 139;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 182 && x <= window_x + 272 && y >= window_y + 132 && y <= window_y + 165 &&
        !MainForm->animation->interface_controller->inventory->is_dragging &&
        !MainForm->animation->interface_controller->spells->is_dragging)
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
    UpdateTooltip(x, y);
}

// When the mouse moves to another slot, shows the tip of the item or spell in it, or hides the tip for an empty slot.
void HotkeysController::UpdateTooltip(int x, int y)
{
    HitTest(x, y);
    if (hovered_column != last_column || last_row != hovered_row)
    {
        if (hovered_column >= 0)
        {
            int slot = 8 * hovered_row + hovered_column;
            if (slot > 15)
                slot = 15;
            if (slots[slot].id > 0)
            {
                int tip_x = window_x + left_grid_x + hovered_column * slot_width + slot_width;
                int tip_y = window_y + grid_y + hovered_row * slot_height;
                if (hovered_column > 3)
                    tip_x += 8;
                if (slots[slot].kind == 1)
                {
                    tooltip->ShowItem(slots[slot].id);
                    tooltip->SetAnchorX(tip_x);
                    tooltip->SetAnchorY(tip_y);
                }
                if (slots[slot].kind == 2)
                {
                    tooltip->ShowSpell(slots[slot].id);
                    tooltip->SetAnchorX(tip_x);
                    tooltip->SetAnchorY(tip_y);
                }
            }
            else
                tooltip->Hide();
        }
        else
            tooltip->Hide();
        last_column = hovered_column;
        last_row = hovered_row;
        MainForm->redraw_needed = true;
    }
}

// Finds the column and row of the slot under the point, in the left or the right grid of four columns; -1 for none.
void HotkeysController::HitTest(int x, int y)
{
    int grid_x;
    int cell_y;
    int local_x = x - window_x;
    int local_y = y - window_y;
    hovered_column = -1;
    hovered_row = -1;
    if (local_x >= left_grid_x && local_x < 4 * slot_width + left_grid_x)
    {
        if (grid_y <= local_y && local_y < 2 * slot_height + grid_y)
        {
            grid_x = local_x - left_grid_x;
            cell_y = local_y - grid_y;
            hovered_column = grid_x / slot_width;
            hovered_row = cell_y / slot_height;
        }
    }
    // The right grid's columns follow the left's, as 4 to 7.
    if (right_grid_x <= local_x && local_x < 4 * slot_width + right_grid_x)
    {
        if (grid_y <= local_y && local_y < 2 * slot_height + grid_y)
        {
            grid_x = local_x - right_grid_x;
            cell_y = local_y - grid_y;
            hovered_column = grid_x / slot_width;
            hovered_row = cell_y / slot_height;
            hovered_column += 4;
        }
    }
    if (hovered_row < 0 || hovered_row < 0 || hovered_column > 7 || hovered_row > 1)
    {
        hovered_column = -1;
        hovered_row = -1;
    }
}

// True for the item types a hotkey can use: 3 to 6, 8, 10, 11 and 23 to 26.
bool HotkeysController::IsHotkeyItem(int item_id)
{
    int item_type = MainForm->item_values->GetType(item_id);
    if (item_type >= EifItemType_Heal && item_type <= EifItemType_ExpReward)
        return true;
    if (item_type >= EifItemType_Weapon && item_type <= EifItemType_Shield)
        return true;
    if (item_type >= EifItemType_EffectPotion && item_type <= EifItemType_Buff)
        return true;
    if (item_type == EifItemType_VisualDocument)
        return true;
    return false;
}

// Uses the item or casts the spell in the hotkey slot, or says the item is not held or the spell not learned.
void HotkeysController::UseHotkey(int column, int row)
{
    SkillValue *spell;
    ItemValue *item[1];
    if (column < 0)
        return;
    if (column > 7)
        return;
    if (row < 0)
        return;
    if (row > 1)
        return;
    int slot = column + 8 * row;
    if (slot > 15)
        slot = 15;
    if (slots[slot].id < 0)
        return;
    if (slots[slot].kind == 1)
    {
        if (slots[slot].id <= 0)
            return;
        if ((unsigned int)MainForm->item_values->Count() < slots[slot].id)
            return;
        item[0] = MainForm->item_values->GetItem(slots[slot].id - 1);
        if (!MainForm->animation->interface_controller->inventory->HasItem(slots[slot].id))
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Hotkey, "You do not have " +
                item[0]->name);
        else
            MainForm->animation->interface_controller->inventory->UseItem(slots[slot].id, 9);
    }
    if (slots[slot].kind == 2)
    {
        if (!MainForm->animation->interface_controller->spells->HasSpell(slots[slot].id))
        {
            spell = MainForm->skill_values->GetSkill(slots[slot].id - 1);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Hotkey, "You did not learned " +
                spell->name);
        }
        else
            MainForm->animation->interface_controller->spells->UseSpell(slots[slot].id, 9);
    }
}

void HotkeysController::MouseUp()
{
    is_dragging = false;
}

// Reads the 16 hotkey slots from the registry, each saved as its kind, 1 for an item or 2 for a spell, a colon and
// the id.
void HotkeysController::LoadHotkeys()
{
    int slot;
    int row;
    MainForm->registry->OpenSlotKey(LayoutKey_Hotkeys);
    for (row = 0; row < 2; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            slot = 8 * row + column;
            if (slot > 15)
                slot = 15;
            String entry = MainForm->registry->ReadSlotText(slot);
            if (entry.Length() < 3 && entry.Pos(":") <= 0)
            {
                slots[slot].id = -1;
                slots[slot].kind = -1;
            }
            else
            {
                // A saved entry starts with 1 for an item or 2 for a spell, then its id.
                if (entry[1] == '1')
                    slots[slot].kind = 1;
                if (entry[1] == '2')
                    slots[slot].kind = 2;
                if (slots[slot].kind > 0)
                {
                    entry.Delete(1, 2);
                    slots[slot].id = ParseNumber(entry);
                }
            }
        }
    }
    MainForm->registry->CloseSlotKey();
}

// The number made of the digits in the text, or -1 when it has none.
int HotkeysController::ParseNumber(String text)
{
    String digits = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] >= '0' && text[i] <= '9')
            digits = digits + text[i];
    }
    if (!digits.Length())
        return -1;
    return StrToInt(digits);
}

// Writes each hotkey slot to the registry as its kind and id, or 0 when empty, where the saved text differs.
void HotkeysController::SaveHotkeys()
{
    int slot;
    int row;
    MainForm->registry->OpenSlotKey(LayoutKey_Hotkeys);
    for (row = 0; row < 2; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            slot = 8 * row + column;
            if (slot > 15)
                slot = 15;
            int id = slots[slot].id;
            int kind = slots[slot].kind;
            String entry = "0";
            if (id > 0)
                entry = IntToStr(kind) + ":" + IntToStr(id);
            String stored = MainForm->registry->ReadSlotText(slot);
            String entry_copy = entry;
            if (MainForm->registry->ReadSlotText(slot) != entry)
                MainForm->registry->WriteSlotText(slot, entry);
        }
    }
    MainForm->registry->CloseSlotKey();
}
