#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Ctrl_musician.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

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
    has_hover = 0;
    is_dragging = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    use_cooldown = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
    unused_reset_flag = 1;
    train_confirm_ticks = 0;
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
            if (!cast_target_kind)
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
                buttons_dirty = 1;
                is_dirty = 1;
            }
        }
        else
        {
            blink_phase = 0;
            buttons_dirty = 1;
            is_dirty = 1;
        }
    }
    if (use_cooldown > 0)
        use_cooldown--;
    if (click_ticks < 10)
        click_ticks++;
    if (hover_message_delay > 0)
        hover_message_delay--;
    if (drag_delay_ticks < 10)
    {
        if (is_dragging)
        {
            drag_delay_ticks++;
            if (drag_delay_ticks > 1)
                drag_started = 1;
        }
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

// Tracks the spell under the mouse and shows its name in the status bar. While a spell is dragged, any cell counts.
void SpellController::UpdateHover(int x, int y)
{
    char found;
    HDC dc;
    int col;
    int result;
    int message_type;
    col = (x - 201) / SLOT_GRID_CELL_WIDTH;
    result = (y - 339) / SLOT_GRID_CELL_HEIGHT;
    last_mouse_x = x;
    last_mouse_y = y;
    if (result >= 0 && result <= 1 && col >= 0 && col <= 7 && x > 201 && y > 333)
    {
        if (is_dragging)
        {
            hover_col = col;
            hover_row = result;
            last_hover_col = col;
            last_hover_row = result;
            hover_text_width = 0;
            has_hover = 1;
            selection_dirty = 1;
            is_dirty = 1;
        }
        else
        {
            found = 0;
            for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
            {
                int grid_row = result + top_row;
                if (col == it->grid_x && grid_row == it->grid_y && it->is_visible)
                {
                    found = 1;
                    if (hover_col != it->grid_x || hover_row != it->grid_y)
                    {
                        hover_spell_id = it->spell_id;
                        hover_col = it->grid_x;
                        hover_row = it->grid_y - top_row;
                        last_hover_col = it->grid_x;
                        last_hover_row = it->grid_y - top_row;
                        hover_text = MainForm->skill_values->GetName(it->spell_id);
                        message_type = MainForm->skill_values->GetCategory(it->spell_id) + 7;
                        if (hover_message_delay < 1)
                            MainForm->animation->interface_controller->ShowStatusMessage(message_type, hover_text);
                        hover_alt_frame = 0;
                        has_hover = 1;
                        selection_dirty = 1;
                        is_dirty = 1;
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
                    selection_dirty = 1;
                    is_dirty = 1;
                    hover_col = -1;
                    hover_row = -1;
                    hover_spell_id = -1;
                    hover_text = "";
                }
                hover_alt_frame = 0;
                has_hover = 0;
            }
        }
    }
    else
    {
        if (has_hover)
        {
            selection_dirty = 1;
            is_dirty = 1;
            hover_col = -1;
            hover_row = -1;
            hover_spell_id = -1;
            hover_text = "";
        }
        hover_alt_frame = 0;
        has_hover = 0;
    }
}

// A click on the spell panel. The left button picks up and selects a spell, drops a dragged one on a cell, swapping
// it with the spell there, or uses it on a second quick click. Also handles the scroll bar and the training buttons.
void SpellController::MouseDown(int x, int y, int button)
{
    char cancel_drag;
    char used;
    char swapped;
    int drag_row;
    char found;
    HDC dc;
    int col;
    int row;
    int drag_col;
    cancel_drag = 0;
    if (x >= 201 && x <= 560)
    {
        if (y >= 339 && y <= 440)
        {
            col = (x - 201) / SLOT_GRID_CELL_WIDTH;
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
                            vector<SpellItem>::iterator it;
                            used = 0;
                            for (it = spells.begin(); it != spells.end(); it++)
                            {
                                int grid_row = row + top_row;
                                if (col == it->grid_x && grid_row == it->grid_y)
                                {
                                    UseSpell(it->grid_x, it->grid_y, it->spell_id);
                                    used = 1;
                                    break;
                                }
                            }
                            if (!used && (selected_spell_id >= 0 || selected_spell_icon >= 0))
                            {
                                awaiting_target = false;
                                selected_spell_id = -1;
                                selected_spell_icon = -1;
                                selection_dirty = 1;
                                is_dirty = 1;
                                selected_text_width = 0;
                            }
                        }
                        vector<SpellItem>::iterator it;
                        swapped = 0;
                        drag_row = -1;
                        drag_col = -1;
                        row += top_row;
                        for (it = spells.begin(); it != spells.end(); it++)
                        {
                            if (drag_spell_id == it->spell_id)
                            {
                                drag_col = it->grid_x;
                                drag_row = it->grid_y;
                            }
                        }
                        for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
                        {
                            if (col == it->grid_x && row == it->grid_y && drag_spell_id != it->spell_id)
                            {
                                it->grid_x = drag_col;
                                it->grid_y = drag_row;
                                it->is_visible = 1;
                                swapped = 1;
                                break;
                            }
                        }
                        if (!swapped)
                        {
                            vector<SpellItem>::iterator it;
                            for (it = spells.begin(); it != spells.end(); it++)
                            {
                                if (drag_spell_id != it->spell_id)
                                    continue;
                                it->grid_x = col;
                                it->grid_y = row;
                                it->is_visible = 1;
                                cancel_drag = 1;
                            }
                        }
                        else
                        {
                            vector<SpellItem>::iterator it;
                            for (it = spells.begin(); it != spells.end(); it++)
                            {
                                if (drag_spell_id != it->spell_id)
                                    continue;
                                it->grid_x = col;
                                it->grid_y = row;
                                it->is_visible = 1;
                                cancel_drag = 1;
                            }
                        }
                        SaveLayout();
                    }
                    else if (!is_casting)
                    {
                        vector<SpellItem>::iterator it;
                        click_ticks = 0;
                        click_col = col;
                        click_row = row;
                        row += top_row;
                        found = 0;
                        for (it = spells.begin(); it != spells.end(); it++)
                        {
                            if (col == it->grid_x && row == it->grid_y)
                            {
                                MainForm->sfx->PlaySfx(20);
                                found = 1;
                                is_dragging = 1;
                                drag_started = 0;
                                drag_delay_ticks = 0;
                                drag_x = x;
                                drag_y = y;
                                drag_spell_id = it->spell_id;
                                drag_icon = MainForm->skill_values->GetIcon(it->spell_id);
                                awaiting_target = false;
                                selected_spell_id = it->spell_id;
                                selected_spell_level = it->level;
                                selected_spell_icon = MainForm->skill_values->GetIcon(it->spell_id);
                                selected_spell_category = MainForm->skill_values->GetCategory(it->spell_id);
                                selected_spell_type = MainForm->skill_values->GetType(it->spell_id);
                                selected_target_kind = MainForm->skill_values->GetTargetRestriction(it->spell_id);
                                selected_spell_name = MainForm->skill_values->GetName(it->spell_id);
                                clicked_spell_id = it->spell_id;
                                clicked_col = it->grid_x;
                                clicked_row = it->grid_y;
                                it->is_visible = 0;
                                MainForm->dragging = true;
                                selection_dirty = 1;
                                is_dirty = 1;
                                selected_text_width = 0;
                                if (MainForm->OffscreenSurface->GetDC(&dc))
                                {
                                    try
                                    {
                                        MainForm->text_canvas->Handle = dc;
                                        selected_text_width = MainForm->text_canvas->TextWidth(hover_text);
                                        MainForm->text_canvas->Handle = 0;
                                    }
                                    catch (Exception &)
                                    {
                                    }
                                    MainForm->OffscreenSurface->ReleaseDC(dc);
                                }
                                break;
                            }
                        }
                        if (!found && (selected_spell_id >= 0 || selected_spell_icon >= 0))
                        {
                            awaiting_target = false;
                            selected_spell_id = -1;
                            selected_spell_icon = -1;
                            selection_dirty = 1;
                            is_dirty = 1;
                            selected_text_width = 0;
                        }
                    }
                }
                else if (is_dragging)
                    cancel_drag = 1;
            }
        }
        else
            cancel_drag = 1;
    }
    else
    {
        // Outside the grid a drag is cancelled, except over the scroll bar
        if (x > 585)
            cancel_drag = 1;
        if (y < 339 || y > 440)
            cancel_drag = 1;
        if (x >= 569 && x <= 585 && y >= 332 && y <= 447)
            cancel_drag = 0;
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
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset;
            DragScrollThumb(y);
        }
        if (y >= 332 && y <= 347)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (y >= 432 && y <= 447)
        {
            scroll_down_held = 1;
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
                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_TrainSpell, "Skill training", "Do you want to train?");
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
    selection_dirty = 1;
}

void SpellController::MarkDirty()
{
    is_dirty = 1;
}

void SpellController::MarkDragDirty()
{
    drag_dirty = 1;
}

// Ends a drag and marks every spell as back in its cell.
void SpellController::CancelDrag()
{
    MainForm->sfx->PlaySfx(21);
    is_dragging = 0;
    drag_started = 0;
    drag_delay_ticks = 0;
    hover_col = -1;
    hover_row = -1;
    is_dirty = 1;
    selection_dirty = 1;
    MainForm->dragging = false;
    for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
        it->is_visible = 1;
}

// Writes the spell in each grid cell, or 0 for an empty one, to the registry.
void SpellController::SaveLayout()
{
    int j;
    int slot_spell_id;
    for (int i = 0; i < 4; i++)
    {
        for (j = 0; j < 8; j++)
        {
            slot_spell_id = 0;
            for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                {
                    slot_spell_id = it->spell_id;
                    break;
                }
            }
            int slot = j + 8 * i;
            MainForm->registry->WriteSlotValue(2, slot, slot_spell_id);
        }
    }
}

// Records a spell from the server's list; RestoreLayout places it on the grid.
void SpellController::AddKnownSpell(int spell_id, int level)
{
    SpellItem handle(spell_id);
    handle.level = level;
    handle.is_placed = 0;
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
            selection_dirty = 1;
            is_dirty = 1;
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
            return 1;
    }
    return 0;
}

// Puts a spell in the first free cell, row by row, and saves that cell; false when the grid is full.
char SpellController::AddSpell(int spell_id, int level)
{
    char placed;
    int free_col;
    int free_row;
    placed = 0;
    for (int i = 0; i < 4 && !placed; i++)
    {
        for (int j = 0; j < 8 && !placed; j++)
        {
            char occupied = 0;
            for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                    occupied = 1;
            }
            if (!occupied)
            {
                free_col = j;
                free_row = i;
                placed = 1;
            }
        }
        if (placed)
        {
            SpellItem handle(spell_id);
            handle.grid_x = free_col;
            handle.grid_y = free_row;
            handle.level = level;
            spells.insert(spells.end(), handle);
            int value = 8 * free_row + free_col;
            MainForm->registry->WriteSlotValue(2, value, spell_id);
            selection_dirty = 1;
            is_dirty = 1;
        }
    }
    return placed;
}

// Places the known spells on the grid: first in the cells saved in the registry, then the rest in free cells.
// The icon of each placed spell is requested.
void SpellController::RestoreLayout()
{
    saved_slots.clear();
    for (int i = 0; i < 32; i++)
    {
        int slot_spell_id = MainForm->registry->ReadSlotValue(2, i);
        if (slot_spell_id > 0)
        {
            int cell_col = i % 8;
            int cell_row = i / 8;
            SpellItem handle(slot_spell_id);
            handle.grid_y = cell_row;
            handle.grid_x = cell_col;
            saved_slots.insert(saved_slots.end(), handle);
        }
    }
    vector<SpellItem>::iterator it;
    for (it = known_spells.begin(); it != known_spells.end(); it++)
    {
        char matched = 0;
        for (vector<SpellItem>::iterator it2 = saved_slots.begin(); it2 != saved_slots.end() && !matched; it2++)
        {
            if (it2->spell_id != it->spell_id)
                continue;
            it->is_placed = 1;
            matched = 1;
            SpellItem placed_spell(it->spell_id);
            placed_spell.grid_x = it2->grid_x;
            placed_spell.grid_y = it2->grid_y;
            placed_spell.level = it->level;
            spells.insert(spells.end(), placed_spell);
            int restored_icon = MainForm->skill_values->GetIcon(it->spell_id);
            MainForm->gfx->icons->RequestGraphic(restored_icon);
        }
    }
    for (it = known_spells.begin(); it != known_spells.end(); it++)
    {
        if (it->is_placed)
            continue;
        if (AddSpell(it->spell_id, it->level))
        {
            it->is_placed = 1;
            int added_icon = MainForm->skill_values->GetIcon(it->spell_id);
            MainForm->gfx->icons->RequestGraphic(added_icon);
        }
    }
    unused_reset_flag = 1;
}

// A spell hotkey: uses the spell in that column and row of the grid, or reports an empty cell.
void SpellController::UseSlot(int col, int row)
{
    if (drag_started && is_dragging)
        CancelDrag();
    col--;
    // The hotkeys reach the top two rows of the grid, whatever the scroll position
    row--;
    char found = 0;
    for (vector<SpellItem>::iterator it = spells.begin(); it != spells.end(); it++)
    {
        if (col == it->grid_x && row == it->grid_y)
        {
            UseSpell(it->grid_x, it->grid_y, it->spell_id);
            found = 1;
            break;
        }
    }
    if (!found)
    {
        awaiting_target = false;
        selected_spell_id = -1;
        selected_spell_icon = -1;
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_NothingWasSelectedNo));
    }
    MainForm->redraw_needed = true;
    selection_dirty = 1;
    is_dirty = 1;
}

// Uses a spell when there is mana and stamina for it and the character is free to act. A spell that needs a target
// waits for one; one without a target is chanted and cast at once; a group spell needs a group of two or more.
void SpellController::UseSpell(int col, int row, int spell_id)
{
    int target_type;
    HDC dc;
    int check_result;
    int tp_cost;
    int sp_cost;
    char was_casting;
    if (use_cooldown <= 0)
    {
        use_cooldown = 2;
        tp_cost = MainForm->skill_values->GetTpCost(spell_id);
        sp_cost = MainForm->skill_values->GetSpCost(spell_id);
        bool blocked = 0;
        if (!blocked)
        {
            if (tp_cost > MainForm->animation->interface_controller->status->mana)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_YouAreExhaustedNo));
                blocked = 1;
            }
        }
        if (!blocked)
        {
            if (sp_cost > MainForm->animation->interface_controller->status->stamina)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_YouAreExhaustedNoMore));
                blocked = 1;
            }
        }
        if (!blocked)
        {
            was_casting = 0;
            // Using a spell during a cast only clears the selection
            if (MainForm->animation->interface_controller->spells->IsCasting())
            {
                MainForm->animation->interface_controller->spells->ClearSelection();
                was_casting = 1;
            }
            if (!MainForm->map->IsActionBlocked())
            {
                if (!MainForm->animation->IsSitDelayed())
                {
                    if (!MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()))
                    {
                        if (!MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()))
                        {
                            if (!MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()))
                            {
                                if (!MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()) &&
                                    !was_casting)
                                {
                                    target_type = MainForm->skill_values->GetTargetType(spell_id);
                                    // This type only opens the musician window, and only with some weapons held
                                    if (MainForm->skill_values->GetType(spell_id) == 2)
                                    {
                                        awaiting_target = false;
                                        selected_spell_id = -1;
                                        selected_spell_icon = -1;
                                        use_cooldown = 10;
                                        check_result = MainForm->players->GetWeapon(MainForm->net_client->GetPlayerId());
                                        if (check_result == 49 || check_result == 50)
                                            MainForm->animation->musician->Open();
                                        return;
                                    }
                                    else
                                    {
                                        if (!target_type || target_type == 2)
                                        {
                                            awaiting_target = true;
                                            selected_spell_id = spell_id;
                                            selected_spell_icon = MainForm->skill_values->GetIcon(spell_id);
                                            selected_spell_category = MainForm->skill_values->GetCategory(spell_id);
                                            selected_spell_type = MainForm->skill_values->GetType(spell_id);
                                            selected_target_kind = MainForm->skill_values->GetTargetRestriction(spell_id);
                                            selected_spell_name = MainForm->skill_values->GetName(spell_id);
                                            clicked_spell_id = spell_id;
                                            clicked_col = col;
                                            clicked_row = row;
                                            selected_text_width = 0;
                                            if (MainForm->OffscreenSurface->GetDC(&dc))
                                            {
                                                try
                                                {
                                                    MainForm->text_canvas->Handle = dc;
                                                    selected_text_width = MainForm->text_canvas->TextWidth(hover_text);
                                                    MainForm->text_canvas->Handle = 0;
                                                }
                                                catch (Exception &)
                                                {
                                                }
                                                MainForm->OffscreenSurface->ReleaseDC(dc);
                                            }
                                            MainForm->redraw_needed = true;
                                            MainForm->sfx->PlaySfx(4);
                                            MainForm->animation->interface_controller->ShowStatusMessage(selected_spell_category +
                                                7, selected_spell_name + " " + MainForm->strings->GetText(GameText_WasSelected));
                                        }
                                        if (target_type == 1)
                                        {
                                            awaiting_target = false;
                                            selected_spell_id = -1;
                                            selected_spell_icon = -1;
                                            use_cooldown = 10;
                                            String shout_text = MainForm->skill_values->GetShout(spell_id);
                                            MainForm->players->ShowTextLabel(MainForm->net_client->GetPlayerId(),
                                                shout_text, 2, 1, SHOUT_LABEL_TICKS);
                                            MainForm->net_client->sender->SendSpellChant(spell_id);
                                            BeginCast(spell_id, CastTarget_None, -1);
                                        }
                                        if (target_type == 3)
                                        {
                                            if (MainForm->animation->interface_controller->group->members.size() > 1)
                                            {
                                                awaiting_target = false;
                                                selected_spell_id = -1;
                                                selected_spell_icon = -1;
                                                use_cooldown = 10;
                                                String group_shout_text = MainForm->skill_values->GetShout(spell_id);
                                                MainForm->players->ShowTextLabel(MainForm->net_client->GetPlayerId(),
                                                    group_shout_text, 2, 1, SHOUT_LABEL_TICKS);
                                                MainForm->net_client->sender->SendSpellChant(spell_id);
                                                BeginCast(spell_id, CastTarget_Group, -1);
                                            }
                                            else
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                    MainForm->strings->GetText(GameText_SelectedItemOnlyWorks));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

char SpellController::IsCasting()
{
    return is_casting;
}

// Starts the cast countdown, from the spell's cast time, for the given target kind and target.
void SpellController::BeginCast(int spell_id, int target_kind, int target_id)
{
    cast_ticks_left = 4 * MainForm->skill_values->GetCastTime(spell_id);
    cast_spell_id = spell_id;
    cast_target_kind = target_kind;
    cast_target_id = target_id;
    is_casting = true;
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
    is_dirty = 1;
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
    has_hover = 0;
    is_dragging = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    use_cooldown = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
    unused_reset_flag = 1;
    train_confirm_ticks = 0;
    spells.clear();
    known_spells.clear();
    saved_slots.clear();
}
