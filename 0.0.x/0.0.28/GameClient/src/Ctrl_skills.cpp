#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

SkillController::SkillController()
{
    last_hover_col = -1;
    last_hover_row = -1;
    has_selection = false;
    selected_skill_id = -1;
    selected_skill_icon = -1;
    hover_col = -1;
    hover_row = -1;
    hover_message_delay = -1;
    hover_alt_frame = 0;
    has_hover = 0;
    is_dragging = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
    unused_reset_flag = 1;
    train_confirm_ticks = 0;
}

SkillController::~SkillController()
{
}

int SkillController::GetTopRow()
{
    return top_row;
}

int SkillController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

// Blinks the training buttons while skill points are left, runs the click and drag timers, and repeats a held
// scroll arrow.
void SkillController::Tick()
{
    if (train_confirm_ticks > 0)
        train_confirm_ticks--;
    if (MainForm->animation->interface_controller->status->skill_points > 0 && selected_skill_id > 0)
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

DEFINE_SKILL_SCROLL_UP(SkillController)

DEFINE_SKILL_SCROLL_DOWN(SkillController)

DEFINE_SKILL_DRAG_SCROLL_THUMB(SkillController)

// Moves a dragged skill icon and updates the scroll arrow and skill hover states.
DEFINE_SKILL_MOUSE_MOVE(SkillController)

DEFINE_SKILL_UPDATE_SCROLL_HOVER(SkillController)

// Tracks the skill under the mouse and shows its name in the status bar. While a skill is dragged, any cell counts.
void SkillController::UpdateHover(int x, int y)
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
            for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
            {
                int grid_row = result + top_row;
                if (col == it->grid_x && grid_row == it->grid_y && it->is_visible)
                {
                    found = 1;
                    if (hover_col != it->grid_x || hover_row != it->grid_y)
                    {
                        hover_skill_id = it->skill_id;
                        hover_col = it->grid_x;
                        hover_row = it->grid_y - top_row;
                        last_hover_col = it->grid_x;
                        last_hover_row = it->grid_y - top_row;
                        hover_text = MainForm->skill_values->GetName(it->skill_id);
                        message_type = MainForm->skill_values->GetCategory(it->skill_id) + 7;
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
                    hover_skill_id = -1;
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
            hover_skill_id = -1;
            hover_text = "";
        }
        hover_alt_frame = 0;
        has_hover = 0;
    }
}

// A click on the skill panel. The left button picks up and selects a skill, or drops a dragged one on a cell,
// swapping it with the skill there. Also handles the scroll bar and the training buttons.
void SkillController::MouseDown(int x, int y, int button)
{
    char cancel_drag;
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
                        if (click_ticks < 3)
                        {
                            vector<SkillItem>::iterator it;
                            for (it = skills.begin(); it != skills.end(); it++)
                            {
                                int grid_row = row + top_row;
                                if (col == it->grid_x && grid_row == it->grid_y)
                                {
                                    has_selection = true;
                                    selected_skill_id = it->skill_id;
                                    selected_skill_icon = MainForm->skill_values->GetIcon(it->skill_id);
                                    selected_skill_name = MainForm->skill_values->GetName(it->skill_id);
                                    clicked_skill_id = it->skill_id;
                                    clicked_col = it->grid_x;
                                    clicked_row = it->grid_y;
                                    selection_dirty = 1;
                                    is_dirty = 1;
                                    break;
                                }
                            }
                        }
                        vector<SkillItem>::iterator it;
                        swapped = 0;
                        drag_row = -1;
                        drag_col = -1;
                        row += top_row;
                        for (it = skills.begin(); it != skills.end(); it++)
                        {
                            if (drag_skill_id == it->skill_id)
                            {
                                drag_col = it->grid_x;
                                drag_row = it->grid_y;
                            }
                        }
                        for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
                        {
                            if (col == it->grid_x && row == it->grid_y && drag_skill_id != it->skill_id)
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
                            vector<SkillItem>::iterator it;
                            for (it = skills.begin(); it != skills.end(); it++)
                            {
                                if (drag_skill_id != it->skill_id)
                                    continue;
                                it->grid_x = col;
                                it->grid_y = row;
                                it->is_visible = 1;
                                cancel_drag = 1;
                            }
                        }
                        else
                        {
                            vector<SkillItem>::iterator it;
                            for (it = skills.begin(); it != skills.end(); it++)
                            {
                                if (drag_skill_id != it->skill_id)
                                    continue;
                                it->grid_x = col;
                                it->grid_y = row;
                                it->is_visible = 1;
                                cancel_drag = 1;
                            }
                        }
                        SaveLayout();
                    }
                    else
                    {
                        vector<SkillItem>::iterator it;
                        click_ticks = 0;
                        click_col = col;
                        click_row = row;
                        row += top_row;
                        found = 0;
                        for (it = skills.begin(); it != skills.end(); it++)
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
                                drag_skill_id = it->skill_id;
                                drag_icon = MainForm->skill_values->GetIcon(it->skill_id);
                                has_selection = true;
                                selected_skill_id = it->skill_id;
                                selected_skill_level = it->level;
                                selected_skill_icon = MainForm->skill_values->GetIcon(it->skill_id);
                                selected_skill_category = MainForm->skill_values->GetCategory(it->skill_id);
                                selected_skill_name = MainForm->skill_values->GetName(it->skill_id);
                                clicked_skill_id = it->skill_id;
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
                        if (!found && (selected_skill_id >= 0 || selected_skill_icon >= 0))
                        {
                            has_selection = false;
                            selected_skill_id = -1;
                            selected_skill_icon = -1;
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
    if (MainForm->animation->interface_controller->status->skill_points > 0 && selected_skill_id >= 0)
    {
        train_skill_id = 0;
        if (x > 173 && x < 190 && y > 407 && y < 424)
            train_skill_id = selected_skill_id;
        if (x > 173 && x < 190 && y > 425 && y < 442)
            train_skill_id = selected_skill_id;
        // A click just after a training is ignored
        if (train_skill_id > 0 && train_confirm_ticks < 195)
        {
            // Asks first, unless a training was sent recently
            if (train_confirm_ticks <= 0)
            {
                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_TrainSkill, "Skill training", "Do you want to train?");
                MainForm->edits->caret_enabled = false;
            }
            else
            {
                MainForm->net_client->sender->SendTrainSkill(train_skill_id);
                train_confirm_ticks = TRAIN_CONFIRM_TICKS;
            }
        }
    }
}

// Drops a dragged skill where the mouse is, once the drag has started, and lets go of the scroll arrows.
DEFINE_MOUSE_UP(SkillController)

DEFINE_TEST_AND_CLEAR_BUTTONS_DIRTY(SkillController)

DEFINE_TEST_AND_CLEAR_DIRTY(SkillController)

DEFINE_TEST_AND_CLEAR_DRAG_DIRTY(SkillController)

void SkillController::MarkSelectionDirty()
{
    selection_dirty = 1;
}

void SkillController::MarkDirty()
{
    is_dirty = 1;
}

void SkillController::MarkDragDirty()
{
    drag_dirty = 1;
}

// Ends a drag and marks every skill as back in its cell.
void SkillController::CancelDrag()
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
    for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
        it->is_visible = 1;
}

// Writes the skill in each grid cell, or 0 for an empty one, to the registry.
void SkillController::SaveLayout()
{
    int j;
    int slot_skill_id;
    for (int i = 0; i < 4; i++)
    {
        for (j = 0; j < 8; j++)
        {
            slot_skill_id = 0;
            for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                {
                    slot_skill_id = it->skill_id;
                    break;
                }
            }
            int slot = j + 8 * i;
            MainForm->registry->WriteSlotValue(3, slot, slot_skill_id);
        }
    }
}

// Sets a raised skill's level; true when the skill is on the grid.
bool SkillController::SetSkillLevel(int skill_id, int level)
{
    if (skill_id == selected_skill_id)
        selected_skill_level = level;
    for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            selection_dirty = 1;
            is_dirty = 1;
            it->level = level;
            return true;
        }
    }
    return false;
}

// Takes a forgotten skill off the grid; true when it was there.
bool SkillController::RemoveSkill(int skill_id)
{
    for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            skills.erase(it);
            return true;
        }
    }
    return false;
}

char SkillController::HasSkill(int skill_id)
{
    for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
    {
        if (skill_id == it->skill_id)
            return 1;
    }
    return 0;
}

int SkillController::GetBlinkPhase()
{
    return blink_phase;
}

// Clears the panel's state and every skill list.
void SkillController::Reset()
{
    last_hover_col = -1;
    last_hover_row = -1;
    has_selection = false;
    selected_skill_id = -1;
    selected_skill_icon = -1;
    hover_col = -1;
    hover_row = -1;
    hover_message_delay = -1;
    hover_alt_frame = 0;
    has_hover = 0;
    is_dragging = 0;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
    unused_reset_flag = 1;
    train_confirm_ticks = 0;
    skills.clear();
    unused_items_a.clear();
    unused_items_b.clear();
}
