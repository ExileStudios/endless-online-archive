#include <vcl.h>
#pragma hdrstop

#include "Ctrl_skills.h"
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
    has_hover = false;
    is_dragging = false;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
    unused_reset_flag = true;
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
    {
        scroll_repeat_delay--;
        return;
    }
    if (scroll_up_held)
        ScrollUp();
    if (scroll_down_held)
        ScrollDown();
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
    SkillItem *it;
    int message_type;
    char found;
    HDC dc;
    int col = (x - 201) / SLOT_GRID_CELL_WIDTH;
    int result = (y - 339) / SLOT_GRID_CELL_HEIGHT;
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
            has_hover = true;
            selection_dirty = true;
            is_dirty = true;
        }
        else
        {
            found = false;
            it = skills.begin();
            while (it != skills.end())
            {
                int grid_row = top_row + result;
                if (col == it->grid_x && grid_row == it->grid_y && it->is_visible)
                {
                    found = true;
                    if (hover_col == it->grid_x && hover_row == it->grid_y)
                        break;
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
                    has_hover = true;
                    selection_dirty = true;
                    is_dirty = true;
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
                        MainForm->OffscreenSurface->ReleaseDC(dc);
                    }
                    break;
                }
                it++;
            }
            if (!found)
            {
                if (has_hover)
                {
                    selection_dirty = true;
                    is_dirty = true;
                    hover_col = -1;
                    hover_row = -1;
                    hover_skill_id = -1;
                    hover_text = "";
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
            hover_skill_id = -1;
            hover_text = "";
        }
        hover_alt_frame = 0;
        has_hover = false;
    }
}

// A click on the skill panel. The left button picks up and selects a skill, or drops a dragged one on a cell,
// swapping it with the skill there. Also handles the scroll bar and the training buttons.
void SkillController::MouseDown(int x, int y, int button)
{
    vector<SkillItem>::iterator skill_item;
    int view_row;
    int grid_row;
    int drag_col;
    vector<SkillItem>::iterator it;
    vector<SkillItem>::iterator other;
    vector<SkillItem>::iterator moved;
    vector<SkillItem>::iterator placed;
    char cancel_drag = false;
    if (x >= 201 && x <= 560)
    {
        if (y >= 339 && y <= 440)
        {
            int col = (x - 201) / SLOT_GRID_CELL_WIDTH;
            view_row = (y - 339) / SLOT_GRID_CELL_HEIGHT;
            if (view_row >= 0 && view_row <= 1 && col >= 0 && col <= 7)
            {
                if (!button)
                {
                    if (is_dragging)
                    {
                        if (click_ticks < 3)
                        {
                            for (skill_item = skills.begin(); skill_item != skills.end(); skill_item++)
                            {
                                grid_row = top_row + view_row;
                                if (col == skill_item->grid_x && grid_row == skill_item->grid_y)
                                {
                                    has_selection = true;
                                    selected_skill_id = skill_item->skill_id;
                                    selected_skill_icon = MainForm->skill_values->GetIcon(skill_item->skill_id);
                                    selected_skill_name = MainForm->skill_values->GetName(skill_item->skill_id);
                                    clicked_skill_id = skill_item->skill_id;
                                    clicked_col = skill_item->grid_x;
                                    clicked_row = skill_item->grid_y;
                                    selection_dirty = true;
                                    is_dirty = true;
                                    break;
                                }
                            }
                        }
                        char swapped = false;
                        drag_col = -1;
                        int drag_row = -1;
                        view_row += top_row;
                        for (it = skills.begin(); it != skills.end(); it++)
                        {
                            if (drag_skill_id == it->skill_id)
                            {
                                drag_col = it->grid_x;
                                drag_row = it->grid_y;
                            }
                        }
                        for (other = skills.begin(); other != skills.end(); other++)
                        {
                            if (col == other->grid_x && view_row == other->grid_y && drag_skill_id != other->skill_id)
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
                            for (moved = skills.begin(); moved != skills.end(); moved++)
                            {
                                if (drag_skill_id == moved->skill_id)
                                {
                                    moved->grid_x = col;
                                    moved->grid_y = view_row;
                                    moved->is_visible = true;
                                    cancel_drag = true;
                                }
                            }
                        }
                        else
                        {
                            for (placed = skills.begin(); placed != skills.end(); placed++)
                            {
                                if (drag_skill_id == placed->skill_id)
                                {
                                    placed->grid_x = col;
                                    placed->grid_y = view_row;
                                    placed->is_visible = true;
                                    cancel_drag = true;
                                }
                            }
                        }
                        SaveLayout();
                    }
                    else
                    {
                        click_ticks = 0;
                        click_col = col;
                        click_row = view_row;
                        view_row += top_row;
                        char found = false;
                        for (skill_item = skills.begin(); skill_item != skills.end(); skill_item++)
                        {
                            if (col == skill_item->grid_x && view_row == skill_item->grid_y)
                            {
                                MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                                found = true;
                                is_dragging = true;
                                drag_started = false;
                                drag_delay_ticks = 0;
                                drag_x = x;
                                drag_y = y;
                                drag_skill_id = skill_item->skill_id;
                                drag_icon = MainForm->skill_values->GetIcon(skill_item->skill_id);
                                has_selection = true;
                                selected_skill_id = skill_item->skill_id;
                                selected_skill_level = skill_item->level;
                                selected_skill_icon = MainForm->skill_values->GetIcon(skill_item->skill_id);
                                selected_skill_category = MainForm->skill_values->GetCategory(skill_item->skill_id);
                                selected_skill_name = MainForm->skill_values->GetName(skill_item->skill_id);
                                clicked_skill_id = skill_item->skill_id;
                                clicked_col = skill_item->grid_x;
                                clicked_row = skill_item->grid_y;
                                skill_item->is_visible = false;
                                MainForm->dragging = true;
                                selection_dirty = true;
                                is_dirty = true;
                                selected_text_width = 0;
                                HDC dc;
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
                            selection_dirty = true;
                            is_dirty = true;
                            selected_text_width = 0;
                        }
                    }
                }
                else if (is_dragging)
                    cancel_drag = true;
            }
        }
        else
            cancel_drag = true;
    }
    else
    {
        // Outside the grid a drag is cancelled, except over the scroll bar
        if (x > 585)
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
                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_TrainSkill,
                    "Skill training", "Do you want to train?");
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
    selection_dirty = true;
}

void SkillController::MarkDirty()
{
    is_dirty = true;
}

void SkillController::MarkDragDirty()
{
    drag_dirty = true;
}

// Ends a drag and marks every skill as back in its cell.
void SkillController::CancelDrag()
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
    for (vector<SkillItem>::iterator it = skills.begin(); it != skills.end(); it++)
        it->is_visible = true;
}

// Writes the skill in each grid cell, or 0 for an empty one, to the registry.
void SkillController::SaveLayout()
{
    int skill_id;
    vector<SkillItem>::iterator it;
    int slot;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            skill_id = 0;
            for (it = skills.begin(); it != skills.end(); it++)
            {
                if (j == it->grid_x && i == it->grid_y)
                {
                    skill_id = it->skill_id;
                    break;
                }
            }
            slot = j + 8 * i;
            MainForm->registry->WriteSlotValue(LayoutKey_Skills, slot, skill_id);
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
            selection_dirty = true;
            is_dirty = true;
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
            return true;
    }
    return false;
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
    has_hover = false;
    is_dragging = false;
    top_row = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
    unused_reset_flag = true;
    train_confirm_ticks = 0;
    skills.clear();
    unused_items.clear();
    second_unused_items.clear();
}
