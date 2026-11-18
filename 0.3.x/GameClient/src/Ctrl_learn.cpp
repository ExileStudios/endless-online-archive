#include <vcl.h>
#pragma hdrstop

#include "Ctrl_learn.h"
#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_status.h"
#include "Ctrl_inventory.h"
#include "Ctrl_locker.h"
#include "Ctrl_shop.h"
#include "Ctrl_skills.h"
#include "Ctrl_spells.h"
#include "Ctrl_tooltip.h"
#include "Ctrl_crafter.h"

#pragma package(smart_init)

// A skill master window on its menu page, at its default spot.
LearnController::LearnController()
{
    lines = new TStringList();
    line_right_texts = new TStringList();
    window_x = 49;
    window_y = 13;
    unused_flag = true;
    unused_value = 0;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    scroll_changed = false;
    npc_session_id = -1;
    page = LearnPage_Menu;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = false;
    close_hovered = false;
    back_hovered = false;
    scroll_up_held = false;
    scroll_down_held = false;
    is_dragging = false;
    tooltip = new Tooltip(68, 50, 2);
    tooltip->SetFlipEdge(630, 63);
}

LearnController::~LearnController()
{
}

DEFINE_ADD_TEXT(LearnController)

// Takes a skill that was just learned off the list on offer.
void LearnController::RemoveSkill(int skill_id)
{
    for (vector<LearnItem>::iterator it = offered_skills.begin(); it != offered_skills.end(); it++)
    {
        if (skill_id == it->skill_id)
        {
            offered_skills.erase(it);
            break;
        }
    }
}

// Shows the skill master's window for the npc on its menu page with an empty list, closing the other npc windows.
void LearnController::Open(String npc, int session_id)
{
    MainForm->animation->crafter->is_visible = false;
    MainForm->animation->shop->is_visible = false;
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->guild->is_visible = false;
    MainForm->animation->memo->is_visible = false;
    MainForm->animation->inn->is_visible = false;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    npc_name = npc;
    npc_session_id = session_id;
    top_line = 0;
    scroll_thumb_offset = 0;
    page = LearnPage_Menu;
    shown_skills.clear();
    offered_skills.clear();
    unused_items.clear();
    is_visible = true;
    unused_cleared_flag = false;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
DEFINE_CLOSE(LearnController)

// Adds a skill on offer with its requirements, unless it is listed already or the character knows it.
void LearnController::AddSkill(int add_mode, int skill_id, int required_level, int required_class, int price,
    int first_required_skill, int second_required_skill, int third_required_skill, int fourth_required_skill,
    int required_str, int required_wis, int required_int, int required_agi, int required_con, int required_cha)
{
    if (add_mode == SkillListMode_Offered)
    {
        char known = false;
        for (vector<LearnItem>::iterator it = offered_skills.begin(); it != offered_skills.end(); it++)
        {
            if (skill_id == it->skill_id)
            {
                known = true;
                break;
            }
        }
        if (MainForm->animation->interface_controller->skills->HasSkill(skill_id) ||
            MainForm->animation->interface_controller->spells->HasSpell(skill_id))
            known = true;
        if (!known)
        {
            LearnItem handle(skill_id);
            handle.required_level = required_level;
            handle.required_class = required_class;
            handle.price = price;
            handle.required_skill = first_required_skill;
            handle.second_required_skill = second_required_skill;
            handle.third_required_skill = third_required_skill;
            handle.fourth_required_skill = fourth_required_skill;
            handle.required_str = required_str;
            handle.required_wis = required_wis;
            handle.required_int = required_int;
            handle.required_agi = required_agi;
            handle.required_con = required_con;
            handle.required_cha = required_cha;
            offered_skills.insert(offered_skills.end(), handle);
        }
    }
}

// Repeats a held scroll arrow.
DEFINE_TICK(LearnController)

int LearnController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int LearnController::GetTopLine()
{
    return top_line;
}

// Empties the text page and scrolls back to the top.
void LearnController::ClearText()
{
    lines->Clear();
    line_right_texts->Clear();
    link_line_index = 0;
    hovered_row = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
}

// A click in the window. On the menu: list the skills, forget a skill by name, or read the info text. On the list:
// the info link shows a skill's details, and the rest of the row asks to learn it when every requirement is met.
// On the info text the link line opens its question. Otherwise the scroll bar and moving the window.
void LearnController::MouseDown(int x, int y, int button)
{
    vector<LearnItem>::iterator i;
    int row_pixel;
    char handled = false;
    if ((!button || button == 1) && window_y + 43 < y && window_y + 243 > y && window_x + 17 < x && window_x + 252 > x)
    {
        is_dragging = false;
        int row = top_line + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
        if (page == LearnPage_Reset)
        {
            row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (row == link_line_index)
            {
                MainForm->sfx->PlaySfx(SfxSound_Select);
                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_SkillReset,
                    MainForm->strings->GetDialogText(DialogText_ResetCharacterStats, false),
                    MainForm->strings->GetDialogText(DialogText_ResetCharacterStats, true));
                MainForm->edits->caret_enabled = false;
            }
        }
        if (page == LearnPage_SkillList && row >= 0)
        {
            if (row < (int)offered_skills.size())
            {
                selected_skill_id = offered_skills[row].skill_id;
                SkillValue *skill = MainForm->skill_values->GetSkill(selected_skill_id - 1);
                String skill_name = skill->name;
                row_pixel = (y - window_y - 50) % 37;
                if (window_x + 75 < x && window_x + 140 > x && row_pixel > 18)
                {
                    ClassValue *required_class =
                        MainForm->class_values->GetRecord(offered_skills[row].required_class - 1);
                    String class_name = required_class->name;
                    if (class_name == "")
                        class_name = "All";
                    ClearText();
                    AddText(MainForm->net_client->Capitalize(skill_name) + " [" + class_name + "]", false);
                    AddText("", false);
                    if (offered_skills[row].required_skill > 0 || offered_skills[row].second_required_skill > 0 ||
                        offered_skills[row].third_required_skill > 0 || offered_skills[row].fourth_required_skill > 0)
                    {
                        if (offered_skills[row].required_skill > 0)
                        {
                            SkillValue *first_skill =
                                MainForm->skill_values->GetSkill(offered_skills[row].required_skill - 1);
                            AddText(MainForm->strings->GetText(GameText_Skill) + ": " + first_skill->name, false);
                        }
                        if (offered_skills[row].second_required_skill > 0)
                        {
                            SkillValue *second_skill =
                                MainForm->skill_values->GetSkill(offered_skills[row].second_required_skill - 1);
                            AddText(MainForm->strings->GetText(GameText_Skill) + ": " + second_skill->name, false);
                        }
                        if (offered_skills[row].third_required_skill > 0)
                        {
                            SkillValue *third_skill =
                                MainForm->skill_values->GetSkill(offered_skills[row].third_required_skill - 1);
                            AddText(MainForm->strings->GetText(GameText_Skill) + ": " + third_skill->name, false);
                        }
                        if (offered_skills[row].fourth_required_skill > 0)
                        {
                            SkillValue *fourth_skill =
                                MainForm->skill_values->GetSkill(offered_skills[row].fourth_required_skill - 1);
                            AddText(MainForm->strings->GetText(GameText_Skill) + ": " + fourth_skill->name, false);
                        }
                        AddText("", false);
                    }
                    if (offered_skills[row].required_str > 0)
                        AddText(IntToStr(offered_skills[row].required_str) + "  " +
                            MainForm->strings->GetText(GameText_Strength), false);
                    if (offered_skills[row].required_int > 0)
                        AddText(IntToStr(offered_skills[row].required_int) + "  " +
                            MainForm->strings->GetText(GameText_Intelligence), false);
                    if (offered_skills[row].required_wis > 0)
                        AddText(IntToStr(offered_skills[row].required_wis) + "  " +
                            MainForm->strings->GetText(GameText_Wisdom), false);
                    if (offered_skills[row].required_agi > 0)
                        AddText(IntToStr(offered_skills[row].required_agi) + "  " +
                            MainForm->strings->GetText(GameText_Agility), false);
                    if (offered_skills[row].required_con > 0)
                        AddText(IntToStr(offered_skills[row].required_con) + "  " +
                            MainForm->strings->GetText(GameText_Constitution), false);
                    if (offered_skills[row].required_cha > 0)
                        AddText(IntToStr(offered_skills[row].required_cha) + "  " +
                            MainForm->strings->GetText(GameText_Charisma), false);
                    AddText("", false);
                    AddText(IntToStr(offered_skills[row].required_level) + " " +
                        MainForm->strings->GetText(GameText_Level), false);
                    AddText(IntToStr(offered_skills[row].price) + " " + MainForm->item_values->GetName(ItemId_Gold),
                        false);
                    page = LearnPage_SkillInfo;
                    MainForm->redraw_needed = true;
                }
                else
                {
                    char can_learn = true;
                    char class_ok = false;
                    // A skill with no class requirement suits every class
                    if (!offered_skills[row].required_class)
                        class_ok = true;
                    else if (MainForm->class_values->IsDescendantOf(MainForm->animation->interface_controller->status->class_id,
                        offered_skills[row].required_class))
                        class_ok = true;
                    if (MainForm->animation->interface_controller->inventory->GetItemAmount(1) <
                        offered_skills[row].price)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->level < offered_skills[row].required_level)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->stat_str < offered_skills[row].required_str)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->stat_int < offered_skills[row].required_int)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->stat_wis < offered_skills[row].required_wis)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->stat_agi < offered_skills[row].required_agi)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->stat_con < offered_skills[row].required_con)
                        can_learn = false;
                    if (MainForm->animation->interface_controller->status->stat_cha < offered_skills[row].required_cha)
                        can_learn = false;
                    if (offered_skills[row].required_skill > 0 &&
                        !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].required_skill) &&
                        !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].required_skill))
                        can_learn = false;
                    if (offered_skills[row].second_required_skill > 0 &&
                        !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].second_required_skill) &&
                        !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].second_required_skill))
                        can_learn = false;
                    if (offered_skills[row].third_required_skill > 0 &&
                        !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].third_required_skill) &&
                        !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].third_required_skill))
                        can_learn = false;
                    if (offered_skills[row].fourth_required_skill > 0 &&
                        !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].fourth_required_skill) &&
                        !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].fourth_required_skill))
                        can_learn = false;
                    if (!class_ok)
                    {
                        ClassValue *class_record =
                            MainForm->class_values->GetRecord(offered_skills[row].required_class - 1);
                        String required_class_name = class_record->name;
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_WrongClass, false),
                            MainForm->strings->GetDialogText(DialogText_WrongClass, true) + " " + required_class_name +
                                ".");
                        MainForm->edits->caret_enabled = false;
                    }
                    else if (can_learn)
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_LearnSkill,
                            MainForm->strings->GetDialogText(DialogText_LearnConfirmation, false),
                            MainForm->strings->GetDialogText(DialogText_LearnConfirmation, true) + " " + skill_name +
                                "?");
                        MainForm->edits->caret_enabled = false;
                    }
                    else
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_CannotLearn, false),
                            MainForm->strings->GetDialogText(DialogText_CannotLearn, true));
                        MainForm->edits->caret_enabled = false;
                    }
                }
            }
        }
        if (page == LearnPage_Menu)
        {
            if (!row)
            {
                handled = true;
                if (offered_skills.size() <= 0)
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_NothingToLearn, false),
                        MainForm->strings->GetDialogText(DialogText_NothingToLearn, true));
                else
                {
                    shown_skills.clear();
                    for (i = offered_skills.begin(); i != offered_skills.end(); i++)
                        shown_skills.insert(shown_skills.end(), *i);
                    page = LearnPage_SkillList;
                }
            }
            if (row == 1)
                MainForm->animation->game->OpenInputBox(InputBoxKind_FindSkill,
                    MainForm->strings->GetDialogText(DialogText_ForgetSkill, true), EditCharFilter_Password, 32);
            if (row == 2)
            {
                ClearText();
                AddText(MainForm->strings->GetText(GameText_ForgetAll), true);
                AddText(MainForm->strings->GetText(GameText_UseThisOptionIf), true);
                AddText(MainForm->strings->GetText(GameText_AllYourActiveAnd), true);
                AddText(MainForm->strings->GetText(GameText_InOrderToReset), true);
                AddText(MainForm->strings->GetText(GameText_ClickHereToForget), true);
                page = LearnPage_Reset;
                MainForm->redraw_needed = true;
            }
        }
    }
    // Elsewhere the scroll bar takes the click.
    if (!handled)
    {
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + scroll_thumb_offset + 58 <= y &&
            window_y + scroll_thumb_offset + 73 >= y)
        {
            thumb_grabbed = true;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 44 <= y && window_y + 59 >= y)
        {
            scroll_up_held = true;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 228 <= y && window_y + 243 >= y)
        {
            scroll_down_held = true;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (window_x <= x && window_x + 284 >= x && y >= window_y && window_y + 290 >= y && !thumb_grabbed)
        {
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen; tracks the hovered row, names a hovered skill with its requirements in the
// status bar, and tracks the close and back buttons.
void LearnController::MouseMove(int x, int y)
{
    int offered_index;
    int skill_id;
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
        int row;
        if (page == LearnPage_Menu)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 2)
                row = -1;
        }
        if (page == LearnPage_SkillList)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if ((int)offered_skills.size() <= row || row > 4)
                row = -1;
        }
        if (page >= LearnPage_SkillInfo)
        {
            row = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (row > 11)
                row = -1;
            if (lines->Count <= row)
                row = -1;
        }
        if (row != hovered_row)
        {
            if (row != -1 && page == LearnPage_SkillList)
            {
                SkillValue *skill = MainForm->skill_values->GetSkill(offered_skills[row].skill_id - 1);
                ClassValue *required_class = MainForm->class_values->GetRecord(offered_skills[row].required_class - 1);
                String tooltip_text = MainForm->net_client->Capitalize(skill->name);
                tooltip_text = tooltip_text + " ";
                tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_level);
                tooltip_text = tooltip_text + " LVL, ";
                if (offered_skills[row].required_str > 0)
                    tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_str) + " STR, ";
                if (offered_skills[row].required_int > 0)
                    tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_int) + " INT, ";
                if (offered_skills[row].required_wis > 0)
                    tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_wis) + " WIS, ";
                if (offered_skills[row].required_agi > 0)
                    tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_agi) + " AGI, ";
                if (offered_skills[row].required_con > 0)
                    tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_con) + " CON, ";
                if (offered_skills[row].required_cha > 0)
                    tooltip_text = tooltip_text + IntToStr(offered_skills[row].required_cha) + " CHA, ";
                tooltip_text =
                    tooltip_text + IntToStr(offered_skills[row].price) + " " +
                        MainForm->item_values->GetName(ItemId_Gold) + ", ";
                tooltip_text = tooltip_text + required_class->name;
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, tooltip_text);
            }
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
        if (window_x + 76 > x && hovered_row >= 0 && page == LearnPage_SkillList)
        {
            offered_index = hovered_row + GetTopLine();
            skill_id = MainForm->animation->learn->offered_skills[offered_index].skill_id;
            tooltip->ShowSpell(skill_id);
            tooltip->SetAnchorX(window_x);
            tooltip->SetAnchorY(37 * row + window_y);
            MainForm->redraw_needed = true;
        }
        else if (tooltip->Hide())
            MainForm->redraw_needed = true;
    }
    else if (hovered_row >= 0)
    {
        hovered_row = -1;
        MainForm->redraw_needed = true;
    }
    if (page == LearnPage_Menu)
    {
        if (x >= window_x + 92 && x <= window_x + 182 && y >= window_y + 253 && y <= window_y + 279)
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
    }
    else
    {
        if (x >= window_x + 144 && x <= window_x + 234 && y >= window_y + 253 && y <= window_y + 279)
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
        if (x >= window_x + 48 && x <= window_x + 138 && y >= window_y + 253 && y <= window_y + 279)
        {
            if (!back_hovered)
            {
                back_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (back_hovered)
        {
            back_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(LearnController)

// Follows a grabbed scroll thumb, kept on its track, and picks the top line of the list or the text from it.
void LearnController::DragScrollThumb(int y)
{
    int thumb_offset;
    int line_count;
    int line_total = shown_skills.size();
    int thumb_fraction = 5;
    if (page >= LearnPage_SkillInfo)
    {
        line_total = lines->Count;
        thumb_fraction = 12;
    }
    if (thumb_grabbed)
    {
        thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (line_total > thumb_fraction)
        {
            double scaled_line = thumb_offset / NPC_THUMB_TRAVEL;
            double scaled_top_line = ((double)line_total - thumb_fraction) * scaled_line;
            line_count = scaled_top_line;
            if (line_count != top_line)
                top_line = line_count;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list or the text up one line and moves the scroll thumb to match.
void LearnController::ScrollUp()
{
    int result = shown_skills.size();
    int visible_rows = 5;
    if (page >= LearnPage_SkillInfo)
    {
        result = lines->Count;
        visible_rows = 12;
    }
    if (result > visible_rows && top_line > 0)
    {
        top_line--;
        double scroll_fraction = (double)top_line / (result - visible_rows);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list or the text down one line and moves the scroll thumb to match.
void LearnController::ScrollDown()
{
    int result = shown_skills.size();
    int visible_rows = 5;
    if (page >= LearnPage_SkillInfo)
    {
        result = lines->Count;
        visible_rows = 12;
    }
    if (result - visible_rows > top_line)
    {
        top_line++;
        double scroll_fraction = (double)top_line / (result - visible_rows);
        double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void LearnController::MouseUp()
{
    is_dragging = false;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
}
