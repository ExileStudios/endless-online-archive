#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_guild.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

LearnController::LearnController()
{
    lines = new TStringList();
    line_right_texts = new TStringList();
    window_x = 49;
    window_y = 13;
    unused_flag = 1;
    unused_value = 0;
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_changed = 0;
    npc_session_id = -1;
    page = LearnPage_Menu;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = 0;
    close_hovered = 0;
    back_hovered = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    is_dragging = 0;
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
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->locker->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->guild->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
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
    is_visible = 1;
    unused_cleared_flag = 0;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void LearnController::Close()
{
    is_visible = 0;
    npc_session_id = -1;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Adds a skill on offer with its requirements, unless it is listed already or the character knows it.
void LearnController::AddSkill(int add_mode, int skill_id, int required_level, int required_class, int price, int first_required_skill, int second_required_skill, int third_required_skill, int fourth_required_skill, int required_str,
    int required_wis, int required_int, int required_agi, int required_con, int required_cha)
{
    if (add_mode == 1)
    {
        char known = 0;
        for (vector<LearnItem>::iterator it = offered_skills.begin(); it != offered_skills.end(); it++)
        {
            if (skill_id == it->skill_id)
            {
                known = 1;
                break;
            }
        }
        if (MainForm->animation->interface_controller->skills->HasSkill(skill_id) ||
            MainForm->animation->interface_controller->spells->HasSpell(skill_id))
            known = 1;
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
    char handled;
    int row = 0;
    handled = 0;
    if ((button == 0 || button == 1) && this->window_y + 43 < y && this->window_y + 243 > y)
    {
        if (window_x + 17 < x && window_x + 252 > x)
        {
            is_dragging = 0;
            row = top_line + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (page == LearnPage_Reset)
            {
                row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
                if (row == link_line_index)
                {
                    MainForm->sfx->PlaySfx(4);
                    MainForm->animation->game->ShowMessageBox(3, ConfirmAction_SkillReset,
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
                    String skill_name = MainForm->skill_values->GetName(selected_skill_id);
                    int row_pixel = (y - this->window_y - 50) % 37;
                    if (x > 125 && x < 190 && row_pixel > 18)
                    {
                        String class_name = MainForm->class_values->GetName(offered_skills[row].required_class);
                        if (class_name == "")
                            class_name = "All";
                        ClearText();
                        AddText(Client::Capitalize(MainForm->net_client, skill_name) + " [" + class_name + "]", false);
                        AddText("", false);
                        if (offered_skills[row].required_skill > 0 || offered_skills[row].second_required_skill > 0 ||
                            offered_skills[row].third_required_skill > 0 ||
                            offered_skills[row].fourth_required_skill > 0)
                        {
                            if (offered_skills[row].required_skill > 0)
                                AddText(MainForm->strings->GetText(GameText_Skill) + ": " +
                                    MainForm->skill_values->GetName(offered_skills[row].required_skill), false);
                            if (offered_skills[row].second_required_skill > 0)
                                AddText(MainForm->strings->GetText(GameText_Skill) + ": " +
                                    MainForm->skill_values->GetName(offered_skills[row].second_required_skill), false);
                            if (offered_skills[row].third_required_skill > 0)
                                AddText(MainForm->strings->GetText(GameText_Skill) + ": " +
                                    MainForm->skill_values->GetName(offered_skills[row].third_required_skill), false);
                            if (offered_skills[row].fourth_required_skill > 0)
                                AddText(MainForm->strings->GetText(GameText_Skill) + ": " +
                                    MainForm->skill_values->GetName(offered_skills[row].fourth_required_skill), false);
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
                        AddText(IntToStr(offered_skills[row].price) + " " + MainForm->item_values->GetName(1), false);
                        page = LearnPage_SkillInfo;
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        char can_learn = 1;
                        char class_ok = 0;
                        // A skill with no class requirement suits every class
                        if (!this->offered_skills[row].required_class)
                            class_ok = 1;
                        else
                        {
                            if (MainForm->class_values->IsDescendantOf(MainForm->animation->interface_controller->status->class_id,
                                offered_skills[row].required_class))
                                class_ok = 1;
                        }
                        if (MainForm->animation->interface_controller->inventory->GetItemAmount(1) <
                            offered_skills[row].price)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->level <
                            offered_skills[row].required_level)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->stat_str <
                            offered_skills[row].required_str)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->stat_int <
                            offered_skills[row].required_int)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->stat_wis <
                            offered_skills[row].required_wis)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->stat_agi <
                            offered_skills[row].required_agi)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->stat_con <
                            offered_skills[row].required_con)
                            can_learn = 0;
                        if (MainForm->animation->interface_controller->status->stat_cha <
                            offered_skills[row].required_cha)
                            can_learn = 0;
                        if (offered_skills[row].required_skill > 0 &&
                            !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].required_skill) &&
                            !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].required_skill))
                            can_learn = 0;
                        if (offered_skills[row].second_required_skill > 0 &&
                            !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].second_required_skill) &&
                            !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].second_required_skill))
                            can_learn = 0;
                        if (offered_skills[row].third_required_skill > 0 &&
                            !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].third_required_skill) &&
                            !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].third_required_skill))
                            can_learn = 0;
                        if (offered_skills[row].fourth_required_skill > 0 &&
                            !MainForm->animation->interface_controller->skills->HasSkill(offered_skills[row].fourth_required_skill) &&
                            !MainForm->animation->interface_controller->spells->HasSpell(offered_skills[row].fourth_required_skill))
                            can_learn = 0;
                        if (!class_ok)
                        {
                            String required_class_name = MainForm->class_values->GetName(offered_skills[row].required_class);
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongClass, false),
                                MainForm->strings->GetDialogText(DialogText_WrongClass, true) + " " +
                                required_class_name + ".");
                            MainForm->edits->caret_enabled = false;
                        }
                        else
                        {
                            if (can_learn)
                            {
                                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_LearnSkill,
                                    MainForm->strings->GetDialogText(DialogText_LearnConfirmation, false),
                                    MainForm->strings->GetDialogText(DialogText_LearnConfirmation, true) + " " +
                                    skill_name + "?");
                                MainForm->edits->caret_enabled = false;
                            }
                            else
                            {
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_CannotLearn, false),
                                    MainForm->strings->GetDialogText(DialogText_CannotLearn, true));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                    }
                }
            }
            if (page == LearnPage_Menu)
            {
                if (!row)
                {
                    handled = 1;
                    if (this->offered_skills.size() <= 0)
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_NothingToLearn, false),
                            MainForm->strings->GetDialogText(DialogText_NothingToLearn, true));
                    else
                    {
                        shown_skills.clear();
                        for (vector<LearnItem>::iterator i = offered_skills.begin(); i != offered_skills.end(); i++)
                            shown_skills.insert(shown_skills.end(), *i);
                        page = LearnPage_SkillList;
                    }
                }
                if (row == 1)
                    MainForm->animation->game->OpenInputBox(InputBoxKind_FindSkill,
                        MainForm->strings->GetDialogText(DialogText_ForgetSkill, true), 3, 32);
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
    }
    if (!handled)
    {
        if (this->window_x + 252 <= x && this->window_x + 267 >= x &&
            this->window_y + this->scroll_thumb_offset + 58 <= y &&
            this->window_y + this->scroll_thumb_offset + 73 >= y)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 44 <= y && window_y + 59 >= y)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 228 <= y && window_y + 243 >= y)
        {
            scroll_down_held = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (window_x <= x && window_x + 284 >= x && y >= window_y)
        {
            if (window_y + 290 >= y && !thumb_grabbed)
            {
                is_dragging = 1;
                drag_offset_x = x - window_x;
                drag_offset_y = y - window_y;
            }
        }
    }
}

// Moves a dragged window, kept on screen; tracks the hovered row, names a hovered skill with its requirements in the
// status bar, and tracks the close and back buttons.
void LearnController::MouseMove(int x, int y)
{
    int row;
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
                String tooltip_text = Client::Capitalize(MainForm->net_client,
                    MainForm->skill_values->GetName(offered_skills[row].skill_id));
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
                tooltip_text = tooltip_text + IntToStr(offered_skills[row].price) + " " +
                    MainForm->item_values->GetName(1) + ", ";
                tooltip_text = tooltip_text + MainForm->class_values->GetName(offered_skills[row].required_class);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, tooltip_text);
            }
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else
    {
        if (hovered_row >= 0)
        {
            hovered_row = -1;
            MainForm->redraw_needed = true;
        }
    }
    if (page == LearnPage_Menu)
    {
        if (x >= window_x + 92 && x <= window_x + 182 && y >= window_y + 253 && y <= window_y + 279)
        {
            if (!close_hovered)
            {
                close_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else
        {
            if (close_hovered)
            {
                close_hovered = 0;
                MainForm->redraw_needed = true;
            }
        }
    }
    else
    {
        if (x >= window_x + 144 && x <= window_x + 234 && y >= window_y + 253 && y <= window_y + 279)
        {
            if (!close_hovered)
            {
                close_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else
        {
            if (close_hovered)
            {
                close_hovered = 0;
                MainForm->redraw_needed = true;
            }
        }
        if (x >= window_x + 48 && x <= window_x + 138 && y >= window_y + 253 && y <= window_y + 279)
        {
            if (!back_hovered)
            {
                back_hovered = 1;
                MainForm->redraw_needed = true;
            }
        }
        else
        {
            if (back_hovered)
            {
                back_hovered = 0;
                MainForm->redraw_needed = true;
            }
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(LearnController)

// Follows a grabbed scroll thumb, kept on its track, and picks the top line of the list or the text from it.
void LearnController::DragScrollThumb(int y)
{
    double scaled_line;
    double scaled_top_line;
    int __slot1;
    int line_count = shown_skills.size();
    int thumb_fraction = 5;
    if (page >= LearnPage_SkillInfo)
    {
        line_count = lines->Count;
        thumb_fraction = 12;
    }
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (line_count > thumb_fraction)
        {
            scaled_line = thumb_offset / NPC_THUMB_TRAVEL;
            scaled_top_line = ((double)line_count - (double)thumb_fraction) * scaled_line;
            line_count = scaled_top_line;
            if (line_count != top_line)
                top_line = line_count;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list or the text up one line and moves the scroll thumb to match.
void LearnController::ScrollUp()
{
    double scroll_fraction;
    double thumb_pos;
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
        scroll_fraction = (double)top_line / (double)(result - visible_rows);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the list or the text down one line and moves the scroll thumb to match.
void LearnController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
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
        scroll_fraction = (double)top_line / (double)(result - visible_rows);
        thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_changed = 1;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void LearnController::MouseUp()
{
    is_dragging = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}
