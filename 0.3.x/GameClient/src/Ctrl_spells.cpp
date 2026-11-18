#include <vcl.h>
#pragma hdrstop

#include "Ctrl_spells.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Ctrl_musician.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#include "Ctrl_group.h"
#include "Ctrl_skills.h"
#include "Ctrl_tooltip.h"
#include "Gfx_mapicons.h"
#include "Ctrl_notification.h"

#pragma package(smart_init)

// No spell selected, hovered, dragged or being cast.
SpellController::SpellController()
{
    last_hover_col = -1;
    last_hover_row = -1;
    is_casting = false;
    awaiting_target = false;
    selected_spell_id = -1;
    selected_spell_icon = -1;
    hover_col = -1;
    hover_row = -1;
    hover_message_delay = -1;
    hover_alt_frame = 0;
    has_hover = false;
    is_dragging = false;
    top_row = 0;
    scroll_thumb_offset = 0;
    use_cooldown = 0;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
    unused_reset_flag = true;
    train_confirm_ticks = 0;
    tooltip = new Tooltip(252, 350, 2);
    tooltip->SetFlipEdge(566, 55);
    tooltip->SetBottomLimit(446, 10);
    tooltip->SetRaise(-1);
}

SpellController::~SpellController()
{
}

int SpellController::GetTopRow()
{
    return top_row;
}

int SpellController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

// Counts down the cast in progress and sends it when due; drops it when its npc or player target has gone.
// Also blinks the training buttons while skill points are left, and repeats a held scroll arrow.
void SpellController::Tick()
{
    if (is_casting)
    {
        cast_ticks_left--;
        if (cast_ticks_left < 1)
        {
            is_casting = false;
            int player_id = MainForm->net_client->GetPlayerId();
            // Kind 0 is cast toward the facing direction, 1 at an npc, 2 at a player, 3 at the group
            if (cast_target_kind == CastTarget_None)
                MainForm->animation->game->CastUntargetedSpell(cast_spell_id);
            if (cast_target_kind == CastTarget_Npc)
            {
                MainForm->players->StartSpellCast(player_id, MainForm->players->GetDirection(player_id));
                MainForm->net_client->sender->SendSpellCastOnNpc(cast_spell_id, cast_target_id);
            }
            if (cast_target_kind == CastTarget_Player)
            {
                MainForm->players->StartSpellCast(player_id, MainForm->players->GetDirection(player_id));
                MainForm->net_client->sender->SendSpellCastOnPlayer(cast_spell_id, cast_target_id);
            }
            if (cast_target_kind == CastTarget_Group)
                MainForm->net_client->sender->SendSpellCastGroup(cast_spell_id);
            is_casting = false;
        }
        // The target left before the cast was sent
        if (cast_target_kind == CastTarget_Npc && !MainForm->npcs->HasNpc(cast_target_id))
        {
            MainForm->animation->interface_controller->spells->CancelCast();
            MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
        }
        if (cast_target_kind == CastTarget_Player && !MainForm->players->HasPlayer(cast_target_id))
        {
            MainForm->animation->interface_controller->spells->CancelCast();
            MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
        }
    }
    if (train_confirm_ticks > 0)
        train_confirm_ticks--;
    if (MainForm->animation->interface_controller->status->skill_points > 0 && selected_spell_id > 0)
    {
        if (blink_phase < 10)
        {
            blink_phase++;
            if (blink_phase == 6)
            {
                buttons_dirty = true;
                is_dirty = true;
            }
        }
        else
        {
            blink_phase = 0;
            buttons_dirty = true;
            is_dirty = true;
        }
    }
    if (use_cooldown > 0)
        use_cooldown--;
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
    if (scroll_repeat_delay > 0)
        scroll_repeat_delay--;
    else
    {
        if (scroll_up_held)
            ScrollUp();
        if (scroll_down_held)
            ScrollDown();
    }
}

DEFINE_SKILL_SCROLL_UP(SpellController)

DEFINE_SKILL_SCROLL_DOWN(SpellController)

DEFINE_SKILL_DRAG_SCROLL_THUMB(SpellController)

// Moves a dragged spell icon and updates the scroll arrow and spell hover states.
DEFINE_SKILL_MOUSE_MOVE(SpellController)

DEFINE_SKILL_UPDATE_SCROLL_HOVER(SpellController)

// Tracks the spell under the mouse and shows its tip and its name in the status bar. While a spell is dragged, any
// cell counts.
void SpellController::UpdateHover(int x, int y)
{
    int grid_row;
    int screen_row;
    vector<SpellItem>::iterator it;
    SkillValue *skill;
    int status_kind;
    int col = (x - 201) / SLOT_GRID_CELL_WIDTH;
    screen_row = (y - 339) / SLOT_GRID_CELL_HEIGHT;
    last_mouse_x = x;
    int row = screen_row;
    last_mouse_y = y;
    // Only the panel's grid of two rows and eight columns counts.
    if (row >= 0 && row <= 1 && col >= 0 && col <= 7 && x > 201 && y > 333)
    {
        if (is_dragging)
        {
            hover_col = col;
            hover_row = row;
            last_hover_col = col;
            last_hover_row = row;
            hover_text_width = 0;
            has_hover = true;
            selection_dirty = true;
            is_dirty = true;
        }
        else
        {
            char found = false;
            for (it = spells.begin(); it != spells.end(); it++)
            {
                grid_row = top_row + row;
                if (col == it->grid_x && grid_row == it->grid_y && it->is_visible)
                {
                    found = true;
                    if (hover_col != it->grid_x || hover_row != it->grid_y)
                    {
                        hover_spell_id = it->spell_id;
                        hover_col = it->grid_x;
                        hover_row = it->grid_y - top_row;
                        last_hover_col = it->grid_x;
                        last_hover_row = it->grid_y - top_row;
                        tooltip->ShowSpell(it->spell_id);
                        tooltip->SetAnchorX(45 * last_hover_col);
                        tooltip->SetAnchorY(52 * last_hover_row);
                        if (hover_message_delay < 1)
                        {
                            skill = MainForm->skill_values->GetSkill(hover_spell_id - 1);
                            status_kind = skill->category + 7;
                            MainForm->animation->interface_controller->ShowStatusMessage(status_kind, skill->name);
                        }
                        hover_alt_frame = 0;
                        has_hover = true;
                        selection_dirty = true;
                        is_dirty = true;
                    }
                    break;
                }
            }
            if (!found)
            {
                if (has_hover)
                {
                    selection_dirty = true;
                    is_dirty = true;
                    hover_col = -1;
                    hover_row = -1;
                    hover_spell_id = -1;
                }
                hover_alt_frame = 0;
                has_hover = false;
            }
        }
    }
    else
    {
        if (has_hover)
        {
            selection_dirty = true;
            is_dirty = true;
            hover_col = -1;
            hover_row = -1;
            hover_spell_id = -1;
        }
        hover_alt_frame = 0;
        has_hover = false;
    }
}

// A click on the spell panel. The left button picks up and selects a spell, or drops a dragged one on a cell,
// swapping it with the spell there; a double click casts it. Also handles the scroll bar and the training buttons.
void SpellController::MouseDown(int x, int y, int button)
{
    vector<SpellItem>::iterator spell_item;
    int row;
    char used;
    char double_click_used;
    vector<SpellItem>::iterator clicked;
    int grid_row;
    int drag_col;
    vector<SpellItem>::iterator dragged;
    vector<SpellItem>::iterator other;
    vector<SpellItem>::iterator moved;
    vector<SpellItem>::iterator placed;
    SkillValue *drag_skill;
    vector<SpellItem>::iterator target;
    char cancel_drag = false;
    if (x >= 201 && x <= 560)
    {
        if (y >= 339 && y <= 440)
        {
            int col = (x - 201) / SLOT_GRID_CELL_WIDTH;
            row = (y - 339) / SLOT_GRID_CELL_HEIGHT;
            if (row >= 0 && row <= 1 && col >= 0 && col <= 7)
            {
                if (!button)
                {
                    if (is_dragging)
                    {
                        // A second click soon after the first uses the spell
                        if (click_ticks < 3)
                        {
                            double_click_used = false;
                            for (clicked = spells.begin(); clicked != spells.end(); clicked++)
                            {
                                grid_row = row + top_row;
                                if (col == clicked->grid_x && grid_row == clicked->grid_y)
                                {
                                    UseSpell(clicked->spell_id, 3);
                                    double_click_used = true;
                                    break;
                                }
                            }
                            if (!double_click_used && (selected_spell_id >= 0 || selected_spell_icon >= 0))
                            {
                                awaiting_target = false;
                                selected_spell_id = -1;
                                selected_spell_icon = -1;
                                selection_dirty = true;
                                is_dirty = true;
                                selected_text_width = 0;
                            }
                        }
                        char swapped = false;
                        drag_col = -1;
                        int drag_row = -1;
                        row += top_row;
                        for (dragged = spells.begin(); dragged != spells.end(); dragged++)
                        {
                            if (drag_spell_id == dragged->spell_id)
                            {
                                drag_col = dragged->grid_x;
                                drag_row = dragged->grid_y;
                            }
                        }
                        for (other = spells.begin(); other != spells.end(); other++)
                        {
                            if (col == other->grid_x && row == other->grid_y && drag_spell_id != other->spell_id)
                            {
                                other->grid_x = drag_col;
                                other->grid_y = drag_row;
                                other->is_visible = true;
                                swapped = true;
                                break;
                            }
                        }
                        if (!swapped)
                        {
                            for (moved = spells.begin(); moved != spells.end(); moved++)
                            {
                                if (drag_spell_id == moved->spell_id)
                                {
                                    moved->grid_x = col;
                                    moved->grid_y = row;
                                    moved->is_visible = true;
                                    cancel_drag = true;
                                }
                            }
                        }
                        else
                        {
                            for (placed = spells.begin(); placed != spells.end(); placed++)
                            {
                                if (drag_spell_id == placed->spell_id)
                                {
                                    placed->grid_x = col;
                                    placed->grid_y = row;
                                    placed->is_visible = true;
                                    cancel_drag = true;
                                }
                            }
                        }
                        SaveLayout();
                    }
                    else if (!is_casting)
                    {
                        click_ticks = 0;
                        click_col = col;
                        click_row = row;
                        row += top_row;
                        char found = false;
                        for (spell_item = spells.begin(); spell_item != spells.end(); spell_item++)
                        {
                            if (col == spell_item->grid_x && row == spell_item->grid_y)
                            {
                                MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                                found = true;
                                drag_skill = MainForm->skill_values->GetSkill(spell_item->spell_id - 1);
                                if (drag_skill)
                                {
                                    if (selected_spell_id == spell_item->spell_id)
                                    {
                                        is_dragging = true;
                                        drag_started = false;
                                        drag_delay_ticks = 0;
                                        drag_x = x;
                                        drag_y = y;
                                        drag_spell_id = spell_item->spell_id;
                                        drag_icon = drag_skill->icon;
                                        spell_item->is_visible = false;
                                        MainForm->dragging = true;
                                    }
                                    selection_dirty = true;
                                    is_dirty = true;
                                    clicked_spell_id = spell_item->spell_id;
                                    clicked_col = spell_item->grid_x;
                                    clicked_row = spell_item->grid_y;
                                    awaiting_target = false;
                                    selected_spell_id = spell_item->spell_id;
                                    selected_spell_level = spell_item->level;
                                    selected_spell_icon = drag_skill->icon;
                                    selected_spell_category = drag_skill->category;
                                    selected_spell_type = drag_skill->type;
                                    selected_target_kind = drag_skill->target_restriction;
                                    selected_spell_name = drag_skill->name;
                                    selected_text_width = 0;
                                    if (MainForm->fonts->BeginOffscreenIf(240))
                                    {
                                        try
                                        {
                                            selected_text_width =
                                                (short)MainForm->fonts->TextWidth(selected_spell_name);
                                        }
                                        catch (Exception &)
                                        {
                                        }
                                        MainForm->fonts->End();
                                    }
                                    break;
                                }
                            }
                        }
                        if (!found && (selected_spell_id >= 0 || selected_spell_icon >= 0))
                        {
                            awaiting_target = false;
                            selected_spell_id = -1;
                            selected_spell_icon = -1;
                            selection_dirty = true;
                            is_dirty = true;
                            selected_text_width = 0;
                        }
                    }
                }
                else if (is_dragging)
                    cancel_drag = true;
                else
                {
                    used = false;
                    for (target = spells.begin(); target != spells.end(); target++)
                    {
                        if (col == target->grid_x && row == target->grid_y)
                        {
                            UseSpell(target->spell_id, 3);
                            used = true;
                            break;
                        }
                    }
                    if (!used && (selected_spell_id >= 0 || selected_spell_icon >= 0))
                    {
                        awaiting_target = false;
                        selected_spell_id = -1;
                        selected_spell_icon = -1;
                        selection_dirty = true;
                        is_dirty = true;
                        selected_text_width = 0;
                    }
                }
            }
        }
        else
            cancel_drag = true;
    }
    else
    {
        if (x < 190 || x > 585)
            cancel_drag = true;
        if (y < 339 || y > 440)
            cancel_drag = true;
        if (x >= 569 && x <= 585 && y >= 332 && y <= 447)
            cancel_drag = false;
    }
    if (cancel_drag && is_dragging)
    {
        CancelDrag();
        UpdateHover(x, y);
    }
    if (x >= 569 && x <= 585)
    {
        if (scroll_thumb_offset + 348 <= y && scroll_thumb_offset + 363 >= y)
        {
            thumb_grabbed = true;
            thumb_grab_offset = y - scroll_thumb_offset;
            DragScrollThumb(y);
        }
        if (y >= 332 && y <= 347)
        {
            scroll_up_held = true;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (y >= 432 && y <= 447)
        {
            scroll_down_held = true;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
    if (MainForm->animation->interface_controller->status->skill_points > 0 && selected_spell_id >= 0)
    {
        train_spell_id = 0;
        if (x > 173 && x < 190 && y > 407 && y < 424)
            train_spell_id = selected_spell_id;
        if (x > 173 && x < 190 && y > 425 && y < 442)
            train_spell_id = selected_spell_id;
        // A click just after a training is ignored
        if (train_spell_id > 0 && train_confirm_ticks < 195)
        {
            // Asks first, unless a training was sent recently
            if (train_confirm_ticks <= 0)
            {
                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_TrainSpell,
                    "Skill training", "Do you want to train?");
                MainForm->edits->caret_enabled = false;
            }
            else
            {
                MainForm->net_client->sender->SendTrainSkill(train_spell_id);
                train_confirm_ticks = TRAIN_CONFIRM_TICKS;
            }
        }
    }
}

// Drops a dragged spell where the mouse is, once the drag has started, and lets go of the scroll arrows.
DEFINE_MOUSE_UP(SpellController)

DEFINE_TEST_AND_CLEAR_BUTTONS_DIRTY(SpellController)

DEFINE_TEST_AND_CLEAR_DIRTY(SpellController)

DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(SpellController)

void SpellController::MarkSelectionDirty()
{
    selection_dirty = true;
}

void SpellController::MarkDirty()
{
    is_dirty = true;
}

void SpellController::MarkDragDirty()
{
    drag_dirty = true;
}

// Ends a drag and marks every spell as back in its cell.
void SpellController::CancelDrag()
{
    MainForm->sfx->PlaySfx(SfxSound_ItemPlace);
    is_dragging = false;
    drag_started = false;
    drag_delay_ticks = 0;
    hover_col = -1;
    hover_row = -1;
    is_dirty = true;
    selection_dirty = true;
    MainForm->dragging = false;
    for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
        it->is_visible = true;
}

// Writes the spell in each of the 4 rows of 8 cells to the registry where it changed, 0 for an empty cell.
void SpellController::SaveLayout()
{
    int j;
    vector<SpellItem>::iterator it;
    MainForm->registry->OpenSlotKey(LayoutKey_Spells);
    for (int i = 0; i < 4; i++)
    {
        for (j = 0; j < 8; j++)
        {
            int spell_id = 0;
            for (it = spells.begin(); it != spells.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                {
                    spell_id = it->spell_id;
                    break;
                }
            }
            int slot = j + 8 * i;
            if (MainForm->registry->ReadSlotValue(slot) != spell_id)
                MainForm->registry->WriteSlotValue(slot, spell_id);
        }
    }
    MainForm->registry->CloseSlotKey();
}

// Records a spell from the server's list; RestoreLayout places it on the grid.
void SpellController::AddKnownSpell(int spell_id, int level)
{
    SpellItem handle(spell_id);
    handle.level = level;
    handle.is_placed = false;
    known_spells.insert(known_spells.end(), handle);
}

// Sets a raised spell's level; true when the spell is on the grid.
bool SpellController::SetSpellLevel(int spell_id, int level)
{
    if (spell_id == selected_spell_id)
        selected_spell_level = level;
    for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
    {
        if (spell_id == it->spell_id)
        {
            selection_dirty = true;
            is_dirty = true;
            it->level = level;
            return true;
        }
    }
    return false;
}

// Takes a forgotten spell off the grid; true when it was there.
bool SpellController::RemoveSpell(int spell_id)
{
    for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
    {
        if (spell_id == it->spell_id)
        {
            spells.erase(it);
            return true;
        }
    }
    return false;
}

char SpellController::HasSpell(int spell_id)
{
    for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
    {
        if (spell_id == it->spell_id)
            return true;
    }
    return false;
}

// Adds a spell at the first free cell of the grid and saves its cell in the registry; nothing when it is known
// already.
char SpellController::AddSpell(int spell_id, int level)
{
    int col;
    vector<SpellItem>::iterator it;
    char occupied;
    if (HasSpell(spell_id))
        return false;
    char placed = false;
    int free_col = -1;
    int free_row = -1;
    for (int i = 0; i < 4 && placed == 0; i++)
    {
        for (col = 0; col < 8 && placed == 0; col++)
        {
            occupied = false;
            for (it = spells.begin(); it != spells.end(); it++)
            {
                if (col == it->grid_x && i == it->grid_y)
                    occupied = true;
            }
            if (!occupied)
            {
                placed = true;
                free_col = col;
                free_row = i;
            }
        }
    }
    if (free_col != -1)
    {
        SpellItem handle(spell_id);
        handle.grid_x = free_col;
        handle.grid_y = free_row;
        handle.level = level;
        spells.insert(spells.end(), handle);
        int slot = 8 * free_row + free_col;
        MainForm->registry->OpenSlotKey(LayoutKey_Spells);
        MainForm->registry->WriteSlotValue(slot, spell_id);
        MainForm->registry->CloseSlotKey();
        selection_dirty = true;
        is_dirty = true;
        return true;
    }
    return false;
}

bool SpellController::HasSavedSlot(int spell_id)
{
    for (vector<SpellItem>::iterator it = saved_slots.begin(); it != saved_slots.end(); it++)
    {
        if (spell_id == it->spell_id)
            return true;
    }
    return false;
}

// Places the known spells on the grid: first in the cells saved in the registry, then the rest in free cells.
// The icon of each placed spell is requested.
void SpellController::RestoreLayout()
{
    vector<SpellItem>::iterator it;
    int slot_spell_id;
    vector<SpellItem>::iterator saved_slot;
    SkillValue *added_icon;
    MainForm->registry->OpenSlotKey(LayoutKey_Spells);
    saved_slots.clear();
    for (int i = 0; i < 32; i++)
    {
        slot_spell_id = MainForm->registry->ReadSlotValue(i);
        if (slot_spell_id > 0 && !HasSavedSlot(slot_spell_id))
        {
            int cell_col = i % 8;
            int cell_row = i / 8;
            SpellItem handle(slot_spell_id);
            handle.grid_y = cell_row;
            handle.grid_x = cell_col;
            saved_slots.insert(saved_slots.end(), handle);
        }
    }
    MainForm->registry->CloseSlotKey();
    it = known_spells.begin();
    while (it != known_spells.end())
    {
        char matched = false;
        saved_slot = saved_slots.begin();
        while (saved_slot != saved_slots.end() && !matched)
        {
            if (saved_slot->spell_id == it->spell_id)
            {
                it->is_placed = true;
                matched = true;
                SpellItem placed_spell(it->spell_id);
                placed_spell.grid_x = saved_slot->grid_x;
                placed_spell.grid_y = saved_slot->grid_y;
                placed_spell.level = (int)it->level;
                spells.insert(spells.end(), placed_spell);
                SkillValue *restored_icon = MainForm->skill_values->GetSkill(it->spell_id - 1);
                MainForm->gfx->icons->RequestGraphic(restored_icon->icon);
            }
            saved_slot++;
        }
        it++;
    }
    for (it = known_spells.begin(); it != known_spells.end(); it++)
    {
        if (!it->is_placed && AddSpell(it->spell_id, it->level))
        {
            it->is_placed = true;
            added_icon = MainForm->skill_values->GetSkill(it->spell_id - 1);
            MainForm->gfx->icons->RequestGraphic(added_icon->icon);
        }
    }
    unused_reset_flag = true;
}

// Starts casting a spell when it has cooled down, the mana covers it and the character is free to act.
// A bard spell opens the musician window when an instrument is held; the rest show the chant and begin the cast.
int SpellController::UseSpell(int spell_id, int unused)
{
    vector<SpellItem>::iterator it;
    int tp_cost;
    SkillValue *spell = MainForm->skill_values->GetSkill(spell_id - 1);
    if (!spell)
        return -1;
    clicked_col = -1;
    for (it = spells.begin(); it != spells.end(); it++)
    {
        if (it->spell_id == spell_id)
        {
            clicked_col = it->grid_x;
            clicked_row = it->grid_y;
            break;
        }
    }
    if (clicked_col < 0)
    {
        MainForm->animation->notification->NotifyWithStatus(NotifyKind_Spell, spell_id, NotifyStatus_NotHeld);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_NothingWasSelectedNo));
        MainForm->sfx->PlaySfx(SfxSound_Denied);
        return -1;
    }
    if (use_cooldown > 0)
        return -2;
    use_cooldown = 2;
    tp_cost = spell->tp_cost;
    if (tp_cost > MainForm->animation->interface_controller->status->mana)
    {
        MainForm->animation->notification->NotifyWithStatus(NotifyKind_Spell, spell_id, NotifyStatus_NoMana);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_YouAreExhaustedNo));
        MainForm->sfx->PlaySfx(SfxSound_Exhausted);
        return -4;
    }
    // Using a spell during a cast only clears the selection
    if (MainForm->animation->interface_controller->spells->IsCasting())
    {
        MainForm->animation->interface_controller->spells->ClearSelection();
        return -3;
    }
    if (!MainForm->map->IsActionBlocked() && !MainForm->animation->IsSitDelayed() &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
    {
        int target_type = spell->target_type;
        int spell_type = spell->type;
        MainForm->animation->notification->Notify(NotifyKind_Spell, spell_id);
        if (spell_type == 2)
        {
            awaiting_target = false;
            selected_spell_id = -1;
            selected_spell_icon = -1;
            use_cooldown = 10;
            int check_result = MainForm->players->GetWeapon(MainForm->net_client->GetPlayerId());
            if (check_result == 49 || check_result == 50)
                MainForm->animation->musician->Open();
            return 1;
        }
        if (spell_type == 3)
            return 1;
        if (target_type == 0 || target_type == 2)
        {
            awaiting_target = true;
            selected_spell_id = spell_id;
            selected_spell_icon = spell->icon;
            selected_spell_category = spell->category;
            selected_spell_type = spell->type;
            selected_target_kind = spell->target_restriction;
            selected_spell_name = spell->name;
            clicked_spell_id = spell_id;
            selected_text_width = 0;
            if (MainForm->fonts->BeginOffscreenIf(240))
            {
                try
                {
                    selected_text_width = (short)MainForm->fonts->TextWidth(selected_spell_name);
                }
                catch (Exception &)
                {
                }
                MainForm->fonts->End();
            }
            MainForm->redraw_needed = true;
            MainForm->sfx->PlaySfx(SfxSound_Select);
            MainForm->animation->interface_controller->ShowStatusMessage(selected_spell_category + 7,
                selected_spell_name + " " + MainForm->strings->GetText(GameText_WasSelected));
        }
        if (target_type == 1)
        {
            awaiting_target = false;
            selected_spell_id = -1;
            selected_spell_icon = -1;
            use_cooldown = 10;
            MainForm->players->ShowTextLabel(MainForm->net_client->GetPlayerId(), spell->shout, LabelColor_Silver, true,
                4 * spell->cast_time);
            MainForm->net_client->sender->SendSpellChant(spell_id);
            BeginCast(spell_id, CastTarget_None, -1, false);
        }
        if (target_type == 3)
        {
            if (MainForm->animation->interface_controller->group->members.size() > 1)
            {
                awaiting_target = false;
                selected_spell_id = -1;
                selected_spell_icon = -1;
                use_cooldown = 10;
                MainForm->players->ShowTextLabel(MainForm->net_client->GetPlayerId(), spell->shout, LabelColor_Silver,
                    true, 4 * spell->cast_time);
                MainForm->net_client->sender->SendSpellChant(spell_id);
                BeginCast(spell_id, CastTarget_Group, -1, false);
            }
            else
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_SelectedItemOnlyWorks));
        }
        return 1;
    }
    return -6;
}

bool SpellController::IsCasting()
{
    return is_casting;
}

// Starts casting the spell at the target, for four ticks per unit of its cast time, or at once when instant.
void SpellController::BeginCast(int spell_id, int target_kind, int target_id, char is_instant)
{
    SkillValue *spell = MainForm->skill_values->GetSkill(spell_id - 1);
    if (spell)
    {
        cast_ticks_left = 4 * spell->cast_time;
        if (is_instant)
            cast_ticks_left = -1;
        cast_is_instant = is_instant;
        cast_spell_id = spell_id;
        cast_target_kind = target_kind;
        cast_target_id = target_id;
        is_casting = true;
    }
}

void SpellController::CancelCast()
{
    if (is_casting)
        is_casting = false;
}

void SpellController::ClearSelection()
{
    awaiting_target = false;
    selected_spell_id = -1;
    selected_spell_icon = -1;
    is_dirty = true;
}

int SpellController::GetBlinkPhase()
{
    return blink_phase;
}

// Clears the panel's state and every spell list.
void SpellController::Reset()
{
    last_hover_col = -1;
    last_hover_row = -1;
    awaiting_target = false;
    selected_spell_id = -1;
    selected_spell_icon = -1;
    hover_col = -1;
    hover_row = -1;
    hover_message_delay = -1;
    hover_alt_frame = 0;
    has_hover = false;
    is_dragging = false;
    top_row = 0;
    scroll_thumb_offset = 0;
    use_cooldown = 0;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
    unused_reset_flag = true;
    train_confirm_ticks = 0;
    spells.clear();
    known_spells.clear();
    saved_slots.clear();
}
