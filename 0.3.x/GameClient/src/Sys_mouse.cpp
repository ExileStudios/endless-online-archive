#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_menu.h"
#include "Ctrl_paperdoll.h"
#include "Packetproces.h"
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Ctrl_book.h"
#include "Ctrl_credits.h"
#include "Ctrl_dialog.h"
#include "Gfx_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Gfx_interface.h"
#include "Ctrl_jukebox.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_musician.h"
#include "Packetoperations.h"
#include "Ctrl_priest.h"
#include "Ctrl_quests.h"
#include "Ctrl_report.h"
#include "Ctrl_social.h"
#include "Ctrl_stats.h"
#include "Ctrl_chat.h"
#include "Ctrl_gamebars.h"
#include "Ctrl_help.h"
#include "Ctrl_news.h"
#include "Ctrl_playerlist.h"
#include "Ctrl_settings.h"
#include "Ctrl_status.h"
#include <mmsystem.h>
#include <stdlib.h>
#include <vector.h>
#include "Ctrl_chest.h"
#include "Ctrl_group.h"
#include "Ctrl_inventory.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_msgboard.h"
#include "Ctrl_shop.h"
#include "Ctrl_skills.h"
#include "Ctrl_spells.h"
#include "Ctrl_trade.h"
#include "Gfx_mapblends.h"
#include "Gfx_mapitems.h"
#include "Gfx_mapmasks.h"
#include "Gfx_mapobjects.h"
#include "Gfx_maptiles.h"
#include "Gfx_mapwalls.h"
#include "Pathfinder.h"
#include "Ctrl_popupelement.h"
#include "Ctrl_tooltip.h"
#include "Ctrl_hotkeys.h"
#include "Gfx_mapnpcs.h"
#include "Ctrl_crafter.h"
#include "Ctrl_visualdoc.h"

#pragma package(smart_init)

Mouser::Mouser()
{
    x = 0;
    y = 0;
    moved = 0;
}

Mouser::~Mouser()
{
}

void Mouser::SetPosition(int cursor_x, int cursor_y)
{
    moved = 1;
    x = cursor_x;
    y = cursor_y;
}

// Acts on a move once: the hover state of the open box or window, or else the map under the cursor and the panels.
void Mouser::HandleMove()
{
    int button_x;
    int second_button_x;
    int button_y;
    if (moved)
    {
        moved = 0;
        if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
        {
            MainForm->animation->game->MouseMove(x, y);
            if (MainForm->animation->game->message_visible || MainForm->animation->game->large_box_visible ||
                MainForm->animation->game->input_box_visible || MainForm->animation->game->form_visible ||
                MainForm->animation->visual_doc->is_visible || MainForm->animation->hotkeys->is_visible ||
                MainForm->animation->paperdoll->is_visible || MainForm->animation->book->is_visible ||
                MainForm->animation->trade->is_visible || MainForm->animation->bank->is_visible ||
                MainForm->animation->jukebox->is_visible || MainForm->animation->musician->is_visible ||
                MainForm->animation->barber->is_visible || MainForm->animation->learn->is_visible ||
                MainForm->animation->crafter->is_visible || MainForm->animation->shop->is_visible ||
                MainForm->animation->stat->is_visible || MainForm->animation->guild->is_visible ||
                MainForm->animation->memo->is_visible || MainForm->animation->dialog->is_visible ||
                MainForm->animation->report->is_visible || MainForm->animation->law->is_visible ||
                MainForm->animation->inn->is_visible || MainForm->animation->locker->is_visible ||
                MainForm->animation->chest->is_visible || MainForm->animation->social->is_open ||
                MainForm->animation->message_board->is_visible || MainForm->animation->quests->is_visible ||
                MainForm->animation->game->amount_box_visible)
            {
                char window_handled = false;
                if (MainForm->animation->game->message_visible)
                {
                    window_handled = true;
                    if (MainForm->animation->game->message_x + 184 <= x &&
                        x <= MainForm->animation->game->message_x + 272 &&
                        MainForm->animation->game->message_y + 83 <= y &&
                        MainForm->animation->game->message_y + 109 >= y)
                    {
                        if (!MainForm->animation->game->message_ok_hovered)
                        {
                            MainForm->animation->game->message_ok_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->message_ok_hovered)
                    {
                        MainForm->animation->game->message_ok_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_x + 91 <= x &&
                        x <= MainForm->animation->game->message_x + 179 &&
                        MainForm->animation->game->message_y + 83 <= y &&
                        MainForm->animation->game->message_y + 109 >= y)
                    {
                        if (!MainForm->animation->game->message_cancel_hovered)
                        {
                            MainForm->animation->game->message_cancel_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->message_cancel_hovered)
                    {
                        MainForm->animation->game->message_cancel_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                }
                if (MainForm->animation->game->large_box_visible)
                {
                    window_handled = true;
                    if (MainForm->animation->game->large_box_x + 184 <= x &&
                        MainForm->animation->game->large_box_x + 272 >= x &&
                        MainForm->animation->game->large_box_y + 147 <= y &&
                        MainForm->animation->game->large_box_y + 173 >= y)
                    {
                        if (!MainForm->animation->game->large_box_ok_hovered)
                        {
                            MainForm->animation->game->large_box_ok_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->large_box_ok_hovered)
                    {
                        MainForm->animation->game->large_box_ok_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->large_box_x + 91 <= x &&
                        MainForm->animation->game->large_box_x + 179 >= x &&
                        MainForm->animation->game->large_box_y + 147 <= y &&
                        MainForm->animation->game->large_box_y + 173 >= y)
                    {
                        if (!MainForm->animation->game->large_box_cancel_hovered)
                        {
                            MainForm->animation->game->large_box_cancel_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->large_box_cancel_hovered)
                    {
                        MainForm->animation->game->large_box_cancel_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                }
                if (MainForm->animation->game->input_box_visible)
                {
                    window_handled = true;
                    if (MainForm->animation->game->input_box_x + 41 <= x &&
                        MainForm->animation->game->input_box_x + 129 >= x &&
                        MainForm->animation->game->input_box_y + 103 <= y &&
                        MainForm->animation->game->input_box_y + 129 >= y)
                    {
                        if (!MainForm->animation->game->input_box_ok_hovered)
                        {
                            MainForm->animation->game->input_box_ok_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->input_box_ok_hovered)
                    {
                        MainForm->animation->game->input_box_ok_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->input_box_x + 134 <= x &&
                        MainForm->animation->game->input_box_x + 222 >= x &&
                        MainForm->animation->game->input_box_y + 103 <= y &&
                        MainForm->animation->game->input_box_y + 129 >= y)
                    {
                        if (!MainForm->animation->game->input_box_cancel_hovered)
                        {
                            MainForm->animation->game->input_box_cancel_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->input_box_cancel_hovered)
                    {
                        MainForm->animation->game->input_box_cancel_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                }
                // From here on only the first open window, in this order, gets the move.
                if (MainForm->animation->game->form_visible && !window_handled)
                {
                    window_handled = true;
                    int button_shift = MainForm->animation->game->form_row_count;
                    // A form with fewer than five rows is drawn shorter, so its buttons sit higher.
                    if (button_shift < 5)
                        button_shift = INPUT_FORM_ROW_HEIGHT * (5 - button_shift);
                    else
                        button_shift = 0;
                    if (MainForm->animation->game->form_x + 73 <= x && MainForm->animation->game->form_x + 161 >= x &&
                        MainForm->animation->game->form_y + 194 - button_shift <= y &&
                        MainForm->animation->game->form_y + 220 - button_shift >= y)
                    {
                        if (!MainForm->animation->game->form_ok_hovered)
                        {
                            MainForm->animation->game->form_ok_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->form_ok_hovered)
                    {
                        MainForm->animation->game->form_ok_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->form_x + 166 <= x && MainForm->animation->game->form_x + 254 >= x &&
                        MainForm->animation->game->form_y + 194 - button_shift <= y &&
                        MainForm->animation->game->form_y + 220 - button_shift >= y)
                    {
                        if (!MainForm->animation->game->form_cancel_hovered)
                        {
                            MainForm->animation->game->form_cancel_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->form_cancel_hovered)
                    {
                        MainForm->animation->game->form_cancel_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                }
                if (MainForm->animation->social->is_open && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->social->OnMouseMove(x, y);
                }
                if (MainForm->animation->message_board->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->message_board->MouseMove(x, y);
                }
                if (MainForm->animation->game->amount_box_visible && !window_handled)
                {
                    window_handled = true;
                    if (MainForm->animation->game->amount_box_x + 60 <= x &&
                        MainForm->animation->game->amount_box_x + 148 >= x &&
                        MainForm->animation->game->amount_box_y + 125 <= y &&
                        MainForm->animation->game->amount_box_y + 151 >= y)
                    {
                        if (!MainForm->animation->game->amount_ok_hovered)
                        {
                            MainForm->animation->game->amount_ok_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->amount_ok_hovered)
                    {
                        MainForm->animation->game->amount_ok_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->amount_box_x + 153 <= x &&
                        MainForm->animation->game->amount_box_x + 241 >= x &&
                        MainForm->animation->game->amount_box_y + 125 <= y &&
                        MainForm->animation->game->amount_box_y + 151 >= y)
                    {
                        if (!MainForm->animation->game->amount_cancel_hovered)
                        {
                            MainForm->animation->game->amount_cancel_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else if (MainForm->animation->game->amount_cancel_hovered)
                    {
                        MainForm->animation->game->amount_cancel_hovered = false;
                        MainForm->redraw_needed = true;
                    }
                }
                if (MainForm->animation->trade->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->trade->MouseMove(x, y);
                }
                if (MainForm->animation->visual_doc->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->visual_doc->MouseMove(x, y);
                }
                if (MainForm->animation->hotkeys->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->hotkeys->MouseMove(x, y);
                }
                if (MainForm->animation->paperdoll->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->paperdoll->MouseMove(x, y);
                }
                if (MainForm->animation->book->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->book->MouseMove(x, y);
                }
                if (MainForm->animation->bank->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->bank->MouseMove(x, y);
                }
                if (MainForm->animation->musician->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->musician->MouseMove(x, y);
                }
                if (MainForm->animation->jukebox->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->jukebox->MouseMove(x, y);
                }
                if (MainForm->animation->learn->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->learn->MouseMove(x, y);
                }
                if (MainForm->animation->crafter->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->crafter->MouseMove(x, y);
                }
                if (MainForm->animation->shop->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->shop->MouseMove(x, y);
                }
                if (MainForm->animation->locker->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->locker->MouseMove(x, y);
                }
                if (MainForm->animation->guild->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->guild->MouseMove(x, y);
                }
                if (MainForm->animation->dialog->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->dialog->MouseMove(x, y);
                }
                if (MainForm->animation->memo->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->memo->MouseMove(x, y);
                }
                if (MainForm->animation->report->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->report->MouseMove(x, y);
                }
                if (MainForm->animation->law->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->law->MouseMove(x, y);
                }
                if (MainForm->animation->inn->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->inn->MouseMove(x, y);
                }
                if (MainForm->animation->chest->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->chest->MouseMove(x, y);
                }
                if (MainForm->animation->barber->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->barber->MouseMove(x, y);
                }
                if (MainForm->animation->stat->is_visible && !window_handled)
                {
                    window_handled = true;
                    MainForm->animation->stat->MouseMove(x, y);
                }
                if (MainForm->animation->quests->is_visible && !window_handled)
                    MainForm->animation->quests->MouseMove(x, y);
            }
            else if (MainForm->gfx->game->view_clip_rect.left <= x && x <= MainForm->gfx->game->view_clip_rect.right &&
                MainForm->gfx->game->view_clip_rect.top <= y && MainForm->gfx->game->view_clip_rect.bottom >= y)
            {
                MainForm->players->SetMousePosition(x, y);
                MainForm->npcs->SetMousePosition(x, y);
                MainForm->map_items->SetMousePosition(x, y);
                MainForm->map->UpdateCursorTile(x, y);
            }
            MainForm->animation->interface_controller->OnMouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Inventory)
                MainForm->animation->interface_controller->inventory->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Spells)
                MainForm->animation->interface_controller->spells->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Skills)
                MainForm->animation->interface_controller->skills->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Chat)
                MainForm->animation->interface_controller->chat->OnMouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Playerlist)
                MainForm->animation->interface_controller->player_list->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Group)
                MainForm->animation->interface_controller->group->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Settings)
                MainForm->animation->interface_controller->settings->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_News)
                MainForm->animation->interface_controller->news->MouseMove(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Help)
                MainForm->animation->interface_controller->help->MouseMove(x, y);
        }
        if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
        {
            if (MainForm->animation->menu->is_message_open || MainForm->animation->menu->is_text_window_open ||
                MainForm->animation->menu->is_form_open || MainForm->animation->menu->is_loading_world)
            {
                MainForm->animation->menu->OnMouseMove(x, y);
                if (MainForm->animation->menu->is_text_window_open)
                {
                    if (MainForm->animation->menu->text_window_x + 137 <= x &&
                        MainForm->animation->menu->text_window_x + 225 >= x &&
                        MainForm->animation->menu->text_window_y + 196 <= y &&
                        MainForm->animation->menu->text_window_y + 224 >= y)
                        MainForm->animation->menu->text_window_ok_button_hover = true;
                    else
                        MainForm->animation->menu->text_window_ok_button_hover = false;
                }
                if (MainForm->animation->menu->is_message_open)
                {
                    if (MainForm->animation->menu->message_x + 182 <= x &&
                        MainForm->animation->menu->message_x + 270 >= x &&
                        MainForm->animation->menu->message_y + 114 <= y &&
                        MainForm->animation->menu->message_y + 140 >= y)
                        MainForm->animation->menu->message_close_button_hover = true;
                    else
                        MainForm->animation->menu->message_close_button_hover = false;
                    if (MainForm->animation->menu->message_x + 89 <= x &&
                        MainForm->animation->menu->message_x + 177 >= x &&
                        MainForm->animation->menu->message_y + 114 <= y &&
                        MainForm->animation->menu->message_y + 140 >= y)
                        MainForm->animation->menu->message_confirm_button_hover = true;
                    else
                        MainForm->animation->menu->message_confirm_button_hover = false;
                }
                if (MainForm->animation->menu->is_form_open && !MainForm->animation->menu->is_message_open)
                {
                    if (MainForm->animation->menu->form_kind == CHANGE_PASSWORD_DIALOG)
                    {
                        MainForm->edits->SetPosition(EditId_AccountName, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 39);
                        MainForm->edits->SetPosition(EditId_OldPassword, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 76);
                        MainForm->edits->SetPosition(EditId_NewPassword, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 128);
                        MainForm->edits->SetPosition(EditId_ConfirmNewPassword, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 166);
                    }
                    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                    {
                        MainForm->edits->SetPosition(EditId_InputBox, MainForm->animation->menu->form_x + 113,
                            MainForm->animation->menu->form_y + 38);
                        MainForm->players->SetPreviewPosition(MainForm->animation->menu->form_x + 385,
                            MainForm->animation->menu->form_y + 94);
                    }
                    if (MainForm->animation->menu->form_x + 162 <= x && MainForm->animation->menu->form_x + 217 >= x)
                    {
                        if (MainForm->animation->menu->form_y + 82 <= y && MainForm->animation->menu->form_y + 104 >= y)
                            MainForm->animation->menu->gender_arrow_hover = true;
                        else
                            MainForm->animation->menu->gender_arrow_hover = false;
                        if (MainForm->animation->menu->form_y + 111 <= y &&
                            MainForm->animation->menu->form_y + 133 >= y)
                            MainForm->animation->menu->hair_style_arrow_hover = true;
                        else
                            MainForm->animation->menu->hair_style_arrow_hover = false;
                        if (MainForm->animation->menu->form_y + 138 <= y &&
                            MainForm->animation->menu->form_y + 160 >= y)
                            MainForm->animation->menu->hair_color_arrow_hover = true;
                        else
                            MainForm->animation->menu->hair_color_arrow_hover = false;
                        if (MainForm->animation->menu->form_y + 165 <= y &&
                            MainForm->animation->menu->form_y + 187 >= y)
                            MainForm->animation->menu->skin_arrow_hover = true;
                        else
                            MainForm->animation->menu->skin_arrow_hover = false;
                    }
                    else
                    {
                        MainForm->animation->menu->gender_arrow_hover = false;
                        MainForm->animation->menu->hair_style_arrow_hover = false;
                        MainForm->animation->menu->hair_color_arrow_hover = false;
                        MainForm->animation->menu->skin_arrow_hover = false;
                    }
                    button_x = 218;
                    second_button_x = 311;
                    button_y = 216;
                    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                    {
                        button_x = 79;
                        second_button_x = 172;
                    }
                    if (MainForm->animation->menu->form_x + 25 <= x && MainForm->animation->menu->form_x + 44 >= x &&
                        MainForm->animation->menu->form_y + 125 <= y && MainForm->animation->menu->form_y + 161 >= y)
                        MainForm->animation->menu->left_arrow_state = 1;
                    else
                        MainForm->animation->menu->left_arrow_state = 0;
                    if (MainForm->animation->menu->form_x + 292 <= x && MainForm->animation->menu->form_x + 313 >= x &&
                        MainForm->animation->menu->form_y + 125 <= y && MainForm->animation->menu->form_y + 161 >= y)
                        MainForm->animation->menu->right_arrow_state = 1;
                    else
                        MainForm->animation->menu->right_arrow_state = 0;
                    if (MainForm->animation->menu->form_x + 430 <= x && MainForm->animation->menu->form_x + 445 >= x &&
                        MainForm->animation->menu->form_y + 44 <= y && MainForm->animation->menu->form_y + 66 >= y)
                        MainForm->animation->menu->preview_hover = true;
                    else
                        MainForm->animation->menu->preview_hover = false;
                    if (MainForm->animation->menu->form_x + button_x <= x &&
                        button_x + MainForm->animation->menu->form_x + 88 >= x &&
                        MainForm->animation->menu->form_y + button_y <= y &&
                        MainForm->animation->menu->form_y + button_y + 26 >= y)
                        MainForm->animation->menu->form_ok_button_hover = true;
                    else
                        MainForm->animation->menu->form_ok_button_hover = false;
                    if (MainForm->animation->menu->form_x + second_button_x <= x &&
                        second_button_x + MainForm->animation->menu->form_x + 88 >= x &&
                        MainForm->animation->menu->form_y + button_y <= y &&
                        button_y + MainForm->animation->menu->form_y + 26 >= y)
                        MainForm->animation->menu->form_cancel_button_hover = true;
                    else
                        MainForm->animation->menu->form_cancel_button_hover = false;
                }
            }
            else
            {
                if (MainForm->animation->GetSubState() == MenuScreen_CharacterSelect)
                {
                    // The corner close button is cut on a slant, so its area narrows toward the top.
                    if ((y > 10 && y < 17 && 3 * y + 560 > x) || (y > 16 && y < 31 && y + 594 > x))
                    {
                        if (x >= 594 && x <= 624)
                            MainForm->animation->menu->corner_close_hover = true;
                        else
                            MainForm->animation->menu->corner_close_hover = false;
                    }
                    else
                        MainForm->animation->menu->corner_close_hover = false;
                    if (x >= 452 && x <= 570 && y >= 416 && y <= 456)
                        MainForm->animation->menu->change_password_button_hover = true;
                    else
                        MainForm->animation->menu->change_password_button_hover = false;
                    if (x >= 332 && x <= 450 && y >= 416 && y <= 456)
                        MainForm->animation->menu->new_character_button_hover = true;
                    else
                        MainForm->animation->menu->new_character_button_hover = false;
                    if (x >= 485 && x <= 585 && y >= 93 && y <= 120)
                        MainForm->animation->menu->first_slot_login_hover = true;
                    else
                        MainForm->animation->menu->first_slot_login_hover = false;
                    if (x >= 485 && x <= 585 && y >= 217 && y <= 244)
                        MainForm->animation->menu->second_slot_login_hover = true;
                    else
                        MainForm->animation->menu->second_slot_login_hover = false;
                    if (x >= 485 && x <= 585 && y >= 341 && y <= 368)
                        MainForm->animation->menu->third_slot_login_hover = true;
                    else
                        MainForm->animation->menu->third_slot_login_hover = false;
                    if (x >= 485 && x <= 585 && y >= 121 && y <= 148)
                        MainForm->animation->menu->first_slot_delete_hover = true;
                    else
                        MainForm->animation->menu->first_slot_delete_hover = false;
                    if (x >= 485 && x <= 585 && y >= 245 && y <= 272)
                        MainForm->animation->menu->second_slot_delete_hover = true;
                    else
                        MainForm->animation->menu->second_slot_delete_hover = false;
                    if (x >= 485 && x <= 585 && y >= 369 && y <= 396)
                        MainForm->animation->menu->third_slot_delete_hover = true;
                    else
                        MainForm->animation->menu->third_slot_delete_hover = false;
                }
                if (MainForm->animation->GetSubState() == MenuScreen_Login ||
                    MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                    MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
                {
                    // The OK and Cancel buttons of the two login layouts sit in different places.
                    if (MainForm->config->GetSkipSoftpad())
                    {
                        if (x >= 360 && x <= 450 && y >= 394 && y <= 423)
                            MainForm->animation->menu->confirm_ok_hover = true;
                        else
                            MainForm->animation->menu->confirm_ok_hover = false;
                        if (x >= 452 && x <= 542 && y >= 394 && y <= 423)
                            MainForm->animation->menu->login_ok_button_hover = true;
                        else
                            MainForm->animation->menu->login_ok_button_hover = false;
                        if (x >= 460 && x <= 542 && y >= 374 && y <= 389)
                            MainForm->animation->menu->login_cancel_button_hover = true;
                        else
                            MainForm->animation->menu->login_cancel_button_hover = false;
                    }
                    else
                    {
                        if (x >= 382 && x <= 472 && y >= 391 && y <= 420)
                            MainForm->animation->menu->confirm_ok_hover = true;
                        else
                            MainForm->animation->menu->confirm_ok_hover = false;
                        if (x >= 474 && x <= 564 && y >= 391 && y <= 420)
                            MainForm->animation->menu->login_ok_button_hover = true;
                        else
                            MainForm->animation->menu->login_ok_button_hover = false;
                    }
                }
                if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
                {
                    if (x >= 464 && x <= 486 && y >= 335 && y <= 354)
                        MainForm->animation->menu->up_key_lit = true;
                    else
                        MainForm->animation->menu->up_key_lit = false;
                    if (x >= 445 && x <= 463 && y >= 355 && y <= 374)
                        MainForm->animation->menu->left_key_lit = true;
                    else
                        MainForm->animation->menu->left_key_lit = false;
                    if (x >= 464 && x <= 486 && y >= 355 && y <= 374)
                        MainForm->animation->menu->down_key_lit = true;
                    else
                        MainForm->animation->menu->down_key_lit = false;
                    if (x >= 487 && x <= 509 && y >= 355 && y <= 374)
                        MainForm->animation->menu->right_key_lit = true;
                    else
                        MainForm->animation->menu->right_key_lit = false;
                    if (x >= 514 && x <= 562 && y >= 335 && y <= 354)
                        MainForm->animation->menu->space_key_lit = true;
                    else
                        MainForm->animation->menu->space_key_lit = false;
                    if (x >= 514 && x <= 562 && y >= 355 && y <= 374)
                        MainForm->animation->menu->select_key_lit = true;
                    else
                        MainForm->animation->menu->select_key_lit = false;
                    if (x >= 558 && x <= 562 && y >= 284 && y <= 298)
                        MainForm->animation->menu->delete_key_lit = true;
                    else
                        MainForm->animation->menu->delete_key_lit = false;
                }
                if (MainForm->animation->GetSubState() == MenuScreen_CreateAccount)
                {
                    if ((y > 10 && y < 17 && 3 * y + 560 > x) || (y > 16 && y < 31 && y + 594 > x))
                    {
                        if (x >= 594 && x <= 624)
                            MainForm->animation->menu->corner_close_hover = true;
                        else
                            MainForm->animation->menu->corner_close_hover = false;
                    }
                    else
                        MainForm->animation->menu->corner_close_hover = false;
                    if (x >= 358 && x <= 478 && y >= 416 && y <= 455)
                        MainForm->animation->menu->account_create_button_hover = true;
                    else
                        MainForm->animation->menu->account_create_button_hover = false;
                    if (x >= 480 && x <= 598 && y >= 416 && y <= 455)
                        MainForm->animation->menu->account_cancel_button_hover = true;
                    else
                        MainForm->animation->menu->account_cancel_button_hover = false;
                }
                if (MainForm->animation->GetSubState() == MenuScreen_PasswordReset ||
                    MainForm->animation->GetSubState() == MenuScreen_SignupCode)
                {
                    if ((y > 10 && y < 17 && 3 * y + 560 > x) || (y > 16 && y < 31 && y + 594 > x))
                    {
                        if (x >= 594 && x <= 624)
                            MainForm->animation->menu->corner_close_hover = true;
                        else
                            MainForm->animation->menu->corner_close_hover = false;
                    }
                    else
                        MainForm->animation->menu->corner_close_hover = false;
                    if (x >= 358 && x <= 478 && y >= 416 && y <= 455)
                        MainForm->animation->menu->wide_confirm_hover = true;
                    else
                        MainForm->animation->menu->wide_confirm_hover = false;
                    if (x >= 480 && x <= 598 && y >= 416 && y <= 455)
                        MainForm->animation->menu->wide_cancel_hover = true;
                    else
                        MainForm->animation->menu->wide_cancel_hover = false;
                }
                if (MainForm->animation->GetSubState() == MenuScreen_Main ||
                    MainForm->animation->GetSubState() == MenuScreen_Credits ||
                    MainForm->animation->GetSubState() == MenuScreen_Login ||
                    MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                    MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
                {
                    if (x >= 30 && x <= 210 && y >= 282 && y <= 318)
                        MainForm->animation->menu->create_account_button_hover = true;
                    else
                        MainForm->animation->menu->create_account_button_hover = false;
                    if (x >= 30 && x <= 210 && y >= 322 && y <= 358)
                        MainForm->animation->menu->play_button_hover = true;
                    else
                        MainForm->animation->menu->play_button_hover = false;
                    if (x >= 30 && x <= 210 && y >= 362 && y <= 398)
                        MainForm->animation->menu->credits_button_hover = true;
                    else
                        MainForm->animation->menu->credits_button_hover = false;
                    if (x >= 30 && x <= 210 && y >= 402 && y <= 438)
                        MainForm->animation->menu->exit_button_hover = true;
                    else
                        MainForm->animation->menu->exit_button_hover = false;
                }
            }
        }
    }
}

// Acts on a button press: in game the open box or window on top takes it, or else the bars, the map and the
// panels; on the menu screens the open box or form, or else the screen's buttons.
// button is 0 for the left button and 1 for the right. x and y are where the press was; mouse points at the
// last position a move left, which most of the button tests read.
void Mouser::HandleMouseDown(int click_x, int click_y, int button)
{
    char close_input;
    int npc_height;
    char npc_clicked;
    int near_height;
    int target_y;
    int player_screen_y;
    int npc_y;
    int hair_index;
    int skin_x;
    int color_x;
    char handled;
    char box_closed;
    // In the game an open box or window takes the click first; with none open, the panels and the map.
    if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        MainForm->animation->game->ResetIdleTimer();
        // Only the first open box or window, in this order, takes the click.
        if (MainForm->animation->game->message_visible || MainForm->animation->game->large_box_visible ||
            MainForm->animation->game->input_box_visible || MainForm->animation->game->form_visible ||
            MainForm->animation->barber->is_visible || MainForm->animation->guild->is_visible ||
            MainForm->animation->memo->is_visible || MainForm->animation->dialog->is_visible ||
            MainForm->animation->report->is_visible || MainForm->animation->law->is_visible ||
            MainForm->animation->inn->is_visible || MainForm->animation->book->is_visible ||
            MainForm->animation->visual_doc->is_visible ||
            (MainForm->animation->hotkeys->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging) ||
            (MainForm->animation->paperdoll->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            (MainForm->animation->trade->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            (MainForm->animation->chest->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            (MainForm->animation->locker->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            (MainForm->animation->bank->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            (MainForm->animation->crafter->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            (MainForm->animation->shop->is_visible &&
                !MainForm->animation->interface_controller->inventory->is_dragging) ||
            MainForm->animation->learn->is_visible || MainForm->animation->stat->is_visible ||
            MainForm->animation->jukebox->is_visible || MainForm->animation->musician->is_visible ||
            MainForm->animation->social->is_open || MainForm->animation->message_board->is_visible ||
            MainForm->animation->quests->is_visible || MainForm->animation->game->amount_box_visible)
        {
            handled = false;
            box_closed = false;
            if (MainForm->animation->game->message_visible)
            {
                handled = true;
                box_closed = false;
                MainForm->animation->game->MouseDown(click_x, click_y);
                if (x >= MainForm->animation->game->message_x + 184 &&
                    x <= MainForm->animation->game->message_x + 272 && y >= MainForm->animation->game->message_y + 83 &&
                    y <= MainForm->animation->game->message_y + 109)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->game->message_visible = false;
                    MainForm->edits->caret_enabled = true;
                    MainForm->redraw_needed = true;
                    box_closed = true;
                    if (MainForm->animation->game->message_kind == ConfirmAction_ConnectionLost)
                        MainForm->animation->interface_controller->Logout();
                }
                if (x >= MainForm->animation->game->message_x + 90 && x <= MainForm->animation->game->message_x + 178 &&
                    y >= MainForm->animation->game->message_y + 83 && y <= MainForm->animation->game->message_y + 109)
                {
                    if (MainForm->animation->game->message_kind == ConfirmAction_Logout)
                    {
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = true;
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->interface_controller->Logout();
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptPartyJoin)
                    {
                        MainForm->net_client->sender->AcceptPartyJoin(MainForm->animation->interface_controller->group->GetRequesterId());
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptPartyInvite)
                    {
                        MainForm->net_client->sender->AcceptPartyInvite(MainForm->animation->interface_controller->group->GetRequesterId());
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_EnableSound)
                    {
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = true;
                        try
                        {
                            MainForm->sound_driver_names = new TStringList();
                            MainForm->sound_driver_modules = new TStringList();
                            MainForm->sound_driver_guids = new TList();
                            if (MainForm->Dx_Sound1->Created)
                                MainForm->Dx_Sound1->Destroy();
                            if (MainForm->Dx_Sound1->DSEnumerate(0) && MainForm->sound_driver_names->Count > 0)
                                MainForm->Dx_Sound1->Create((_GUID *)MainForm->sound_driver_guids->Items[0]);
                            MainForm->animation->interface_controller->settings->MarkDirty();
                            MainForm->config->SetSoundVolume(100);
                            MainForm->sfx->SetSoundEnabled(100);
                            MainForm->config->SetSoundAvailable(1);
                            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        }
                        catch (...)
                        {
                        }
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_EnableMusic)
                    {
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = true;
                        try
                        {
                            MainForm->sfx->InitMidiPlayer();
                            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                            MainForm->animation->interface_controller->settings->MarkDirty();
                            MainForm->config->SetMusicEnabled(100);
                            MainForm->config->SetMusicAvailable(100);
                        }
                        catch (...)
                        {
                        }
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptTrade)
                    {
                        if (MainForm->animation->interface_controller->status->weight <
                            MainForm->animation->interface_controller->status->max_weight)
                        {
                            MainForm->net_client->sender->AcceptTrade(MainForm->animation->interface_controller->GetTradeRequesterId());
                            MainForm->animation->game->message_visible = false;
                            MainForm->edits->caret_enabled = true;
                            box_closed = true;
                        }
                        else
                        {
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_Warning),
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TradeAgree)
                    {
                        if (MainForm->animation->trade->own_items.size() > 0 &&
                            !MainForm->animation->trade->trade_locked && !MainForm->animation->trade->IsAcceptDelayed())
                        {
                            MainForm->animation->trade->SetAcceptDelay(10);
                            if (MainForm->animation->interface_controller->inventory->CanFitTradeItems())
                            {
                                MainForm->sfx->PlaySfx(SfxSound_Accept);
                                MainForm->animation->trade->own_agreed = true;
                                MainForm->animation->trade->own_status_text =
                                    MainForm->strings->GetText(GameText_Agree);
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                    MainForm->strings->GetText(GameText_YouAcceptDeal));
                                MainForm->net_client->sender->SendTradeAgree();
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetText(GameText_Warning),
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_BuyItem)
                    {
                        if (MainForm->animation->interface_controller->status->weight <
                            MainForm->animation->interface_controller->status->max_weight)
                        {
                            if (MainForm->animation->interface_controller->inventory->TestAddItem(MainForm->animation->shop->selected_item_id))
                            {
                                int total_price = MainForm->animation->shop->selected_amount *
                                    MainForm->animation->shop->selected_price;
                                if (MainForm->animation->interface_controller->status->gold >= total_price)
                                {
                                    MainForm->net_client->sender->SendBuyItem(MainForm->animation->shop->npc_session_id,
                                        MainForm->animation->shop->selected_item_id,
                                        MainForm->animation->shop->selected_amount);
                                    box_closed = true;
                                    MainForm->animation->game->message_visible = false;
                                    MainForm->edits->caret_enabled = true;
                                    MainForm->redraw_needed = true;
                                }
                                else
                                {
                                    String gold_short_text =
                                        MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                                            LowerCase(MainForm->item_values->GetName(ItemId_Gold));
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                                        gold_short_text);
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetText(GameText_Warning),
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                        else
                        {
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_Warning),
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_SellItem)
                    {
                        MainForm->net_client->sender->SendSellItem(MainForm->animation->shop->npc_session_id,
                            MainForm->animation->shop->selected_item_id, MainForm->animation->shop->selected_amount);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_BuyHairstyle)
                    {
                        MainForm->net_client->sender->SendBuyHairstyle(MainForm->animation->barber->npc_session_id,
                            MainForm->animation->barber->hair_style, MainForm->animation->barber->hair_color);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                        if (!MainForm->animation->barber->hair_style)
                            MainForm->animation->game->PlayEmote(Emote_Surprised);
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_LeaveGuild)
                    {
                        MainForm->animation->interface_controller->status->guild_tag = "";
                        MainForm->animation->interface_controller->status->guild_rank = GUILD_RANK_LOWEST;
                        MainForm->animation->interface_controller->status->guild_name = "";
                        MainForm->animation->interface_controller->status->guild_rank_name = "";
                        MainForm->animation->interface_controller->status->MarkDirty();
                        MainForm->net_client->sender->LeaveGuild(MainForm->animation->guild->npc_session_id,
                            MainForm->animation->interface_controller->status->guild_tag);
                        MainForm->sfx->PlaySfx(SfxSound_Delete);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_CreateGuild)
                    {
                        MainForm->net_client->sender->SendGuildCreate(MainForm->animation->guild->npc_session_id,
                            MainForm->animation->guild->new_guild_tag, MainForm->animation->guild->new_guild_name,
                            MainForm->animation->guild->new_guild_description);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_DisbandGuild)
                    {
                        MainForm->net_client->sender->DisbandGuild(MainForm->animation->guild->npc_session_id);
                        MainForm->sfx->PlaySfx(SfxSound_Delete);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_UseItem)
                    {
                        MainForm->net_client->sender->SendUseItem(MainForm->animation->interface_controller->inventory->pending_use_item_id);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_JukeboxRequest)
                    {
                        TPoint jukebox_pos;
                        jukebox_pos.x = 0;
                        jukebox_pos.y = 0;
                        MainForm->net_client->sender->SendJukeboxRequest(jukebox_pos,
                            MainForm->animation->jukebox->selected_song);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TrainStat)
                    {
                        MainForm->net_client->sender->SendTrainStat(MainForm->animation->interface_controller->status->train_stat_index);
                        MainForm->animation->interface_controller->status->train_confirm_ticks = TRAIN_CONFIRM_TICKS;
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TrainSpell)
                    {
                        MainForm->net_client->sender->SendTrainSkill(MainForm->animation->interface_controller->spells->train_spell_id);
                        MainForm->animation->interface_controller->spells->train_confirm_ticks = TRAIN_CONFIRM_TICKS;
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TrainSkill)
                    {
                        MainForm->net_client->sender->SendTrainSkill(MainForm->animation->interface_controller->skills->train_skill_id);
                        MainForm->animation->interface_controller->skills->train_confirm_ticks = TRAIN_CONFIRM_TICKS;
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptWeddingRequest)
                    {
                        MainForm->net_client->sender->AcceptWeddingRequest(MainForm->animation->priest->GetRequestId());
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_ConfirmWeddingVow)
                    {
                        MainForm->net_client->sender->ConfirmWeddingVow(MainForm->animation->priest->GetRequestId());
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_LearnSkill)
                    {
                        MainForm->net_client->sender->SendLearnSkill(MainForm->animation->learn->npc_session_id,
                            MainForm->animation->learn->selected_skill_id);
                        box_closed = true;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_SkillReset)
                    {
                        MainForm->net_client->sender->SendSkillReset(MainForm->animation->learn->npc_session_id);
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_InnSleep)
                    {
                        MainForm->net_client->sender->ConfirmInnSleep(MainForm->animation->inn->npc_session_id);
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_BuyLockerUpgrade)
                    {
                        MainForm->net_client->sender->SendBuyLockerUpgrade();
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_DestroyItem)
                    {
                        MainForm->net_client->sender->SendJunkItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                            1);
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_CraftItem)
                    {
                        MainForm->net_client->sender->SendCraftItem(MainForm->animation->crafter->session_id,
                            MainForm->animation->crafter->result_item_id);
                        MainForm->animation->game->message_visible = false;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            SkillValue *forget_skill;
            if (MainForm->animation->game->large_box_visible && !handled)
            {
                handled = true;
                box_closed = false;
                MainForm->animation->game->MouseDown(click_x, click_y);
                if (x >= MainForm->animation->game->large_box_x + 184 &&
                    x <= MainForm->animation->game->large_box_x + 272 &&
                    y >= MainForm->animation->game->large_box_y + 147 &&
                    y <= MainForm->animation->game->large_box_y + 173)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->game->large_box_visible = false;
                    MainForm->edits->caret_enabled = true;
                    MainForm->redraw_needed = true;
                    box_closed = true;
                }
                if (x >= MainForm->animation->game->large_box_x + 90 &&
                    x <= MainForm->animation->game->large_box_x + 178 &&
                    y >= MainForm->animation->game->large_box_y + 147 &&
                    y <= MainForm->animation->game->large_box_y + 173)
                {
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_CraftItem)
                    {
                    }
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_AcceptGuildInvite)
                    {
                        // The guild window's requests carry the guild master's session.
                        // This answer carries a fixed value in its place.
                        MainForm->net_client->sender->AcceptGuildInvite(20202,
                            MainForm->animation->guild->request_player_id);
                        box_closed = true;
                        MainForm->animation->game->large_box_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_AcceptGuildJoinRequest)
                    {
                        MainForm->net_client->sender->AcceptGuildJoinRequest(MainForm->animation->guild->request_player_id);
                        box_closed = true;
                        MainForm->animation->game->large_box_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_WeddingRequest)
                    {
                        box_closed = true;
                        MainForm->animation->game->large_box_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->animation->game->OpenInputBox(InputBoxKind_WeddingPartner,
                            MainForm->strings->GetText(GameText_EnterNameOfYour), EditCharFilter_Letters, 14);
                    }
                }
            }
            // A message box hides the edit boxes of the window under it; they come back as it closes.
            if (box_closed)
            {
                MainForm->edits->SetVisible(EditId_Amount, true);
                MainForm->edits->SetVisible(EditId_InputBox, true);
                MainForm->edits->SetVisible(EditId_FormFirstRow, true);
                MainForm->edits->SetVisible(EditId_FormSecondRow, true);
                MainForm->edits->SetVisible(EditId_FormThirdRow, true);
                MainForm->edits->SetVisible(EditId_FormFourthRow, true);
                MainForm->edits->SetVisible(EditId_FormFifthRow, true);
                MainForm->edits->SetEditable(POST_SUBJECT_EDIT, true);
                MainForm->edits->SetEditable(POST_MESSAGE_EDIT, true);
            }
            if (MainForm->animation->game->input_box_visible && !handled)
            {
                handled = true;
                MainForm->animation->game->MouseDown(click_x, click_y);
                if (x >= MainForm->animation->game->input_box_x + 41 &&
                    x <= MainForm->animation->game->input_box_x + 129 &&
                    y >= MainForm->animation->game->input_box_y + 103 &&
                    y <= MainForm->animation->game->input_box_y + 129)
                {
                    close_input = true;
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_AddFriend)
                    {
                        String friend_name = MainForm->edits->GetText(EditId_InputBox);
                        if (friend_name.Length() < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                MainForm->strings->GetDialogText(DialogText_WrongName, true));
                            close_input = false;
                        }
                        else
                        {
                            MainForm->animation->social->AddFriend(MainForm->edits->GetText(EditId_InputBox));
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                MainForm->net_client->Capitalize(MainForm->edits->GetText(EditId_InputBox)) + " " +
                                    MainForm->strings->GetText(GameText_WillBeYourFriend));
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_AddIgnore)
                    {
                        String ignore_name = MainForm->edits->GetText(EditId_InputBox);
                        if (ignore_name.Length() < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                MainForm->strings->GetDialogText(DialogText_WrongName, true));
                            close_input = false;
                        }
                        else
                        {
                            MainForm->animation->social->AddIgnore(MainForm->edits->GetText(EditId_InputBox));
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                MainForm->net_client->Capitalize(MainForm->edits->GetText(EditId_InputBox)) + " " +
                                    MainForm->strings->GetText(GameText_WillBeIgnordedRemove));
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_GuildInfo)
                    {
                        String guild_query = MainForm->edits->GetText(EditId_InputBox);
                        if (guild_query.Length() < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            close_input = false;
                        }
                        else
                            MainForm->net_client->sender->RequestGuildInfo(MainForm->animation->guild->npc_session_id,
                                MainForm->edits->GetText(EditId_InputBox));
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_GuildDescription)
                        MainForm->net_client->sender->SetGuildDescription(MainForm->animation->guild->npc_session_id,
                            MainForm->edits->GetText(EditId_InputBox));
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_RemoveGuildMember)
                    {
                        String kick_name = MainForm->edits->GetText(EditId_InputBox);
                        if (kick_name.Length() < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                MainForm->strings->GetDialogText(DialogText_WrongName, true));
                            close_input = false;
                        }
                        else
                        {
                            MainForm->animation->guild->target_member_name =
                                MainForm->net_client->Capitalize(MainForm->edits->GetText(EditId_InputBox));
                            MainForm->net_client->sender->RemoveGuildMember(MainForm->animation->guild->npc_session_id,
                                MainForm->edits->GetText(EditId_InputBox));
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_GuildMemberList)
                    {
                        String member_query = MainForm->edits->GetText(EditId_InputBox);
                        if (member_query.Length() < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            close_input = false;
                        }
                        else
                            MainForm->net_client->sender->RequestGuildMemberList(MainForm->animation->guild->npc_session_id,
                                MainForm->edits->GetText(EditId_InputBox));
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_WeddingPartner)
                    {
                        String partner_name = MainForm->edits->GetText(EditId_InputBox);
                        if (partner_name.Length() > 3)
                            MainForm->net_client->sender->RequestWedding(MainForm->animation->priest->GetRequestId(),
                                partner_name);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_LawOption1)
                    {
                        String law_request_type_a = MainForm->edits->GetText(EditId_InputBox);
                        if (law_request_type_a.Length() > 3)
                            MainForm->net_client->sender->SendLawMenuOption1Request(MainForm->animation->law->npc_session_id,
                                law_request_type_a);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_LawOption2)
                    {
                        String law_request_type_b = MainForm->edits->GetText(EditId_InputBox);
                        if (law_request_type_b.Length() > 3)
                            MainForm->net_client->sender->SendLawMenuOption2Request(MainForm->animation->law->npc_session_id,
                                law_request_type_b);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_FindSkill)
                    {
                        String skill_name = MainForm->edits->GetText(EditId_InputBox);
                        forget_skill = MainForm->skill_values->FindByName(skill_name);
                        if (!forget_skill)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, false),
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, true));
                            close_input = false;
                        }
                        else if (!MainForm->animation->interface_controller->skills->HasSkill(forget_skill->skill_id) &&
                            !MainForm->animation->interface_controller->spells->HasSpell(forget_skill->skill_id))
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, false),
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, true));
                            close_input = false;
                        }
                        else
                            MainForm->net_client->sender->SendForgetSkill(MainForm->animation->learn->npc_session_id,
                                forget_skill->skill_id);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_HelpQuestion)
                    {
                        MainForm->net_client->sender->SendHelpQuestion(MainForm->edits->GetText(EditId_InputBox));
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_ThankYou, false),
                            MainForm->strings->GetDialogText(DialogText_ThankYou, true));
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_InnAnswer)
                    {
                        if (MainForm->edits->GetText(EditId_InputBox) != "")
                        {
                            MainForm->animation->inn->answers[MainForm->animation->inn->question_index] =
                                MainForm->edits->GetText(EditId_InputBox);
                            MainForm->animation->inn->question_index++;
                            if (MainForm->animation->inn->question_index < 3)
                            {
                                MainForm->animation->game->CloseInputBox();
                                MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                                String question_prompt = IntToStr(MainForm->animation->inn->question_index + 1) + ". " +
                                    MainForm->animation->inn->questions[MainForm->animation->inn->question_index];
                                MainForm->animation->game->OpenInputBox(InputBoxKind_InnAnswer, question_prompt,
                                    EditCharFilter_Email, 12);
                                close_input = false;
                            }
                            else
                                MainForm->net_client->sender->SendCitizenshipAnswers(MainForm->animation->inn->npc_session_id,
                                    MainForm->animation->inn->answers[0], MainForm->animation->inn->answers[1],
                                    MainForm->animation->inn->answers[2]);
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_ChooseTitle)
                        MainForm->net_client->sender->SendChooseTitle(MainForm->animation->interface_controller->inventory->pending_use_item_id,
                            MainForm->edits->GetText(EditId_InputBox));
                    if (close_input)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->game->CloseInputBox();
                    }
                }
                if (x >= MainForm->animation->game->input_box_x + 134 &&
                    x <= MainForm->animation->game->input_box_x + 222 &&
                    y >= MainForm->animation->game->input_box_y + 103 &&
                    y <= MainForm->animation->game->input_box_y + 129)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->game->CloseInputBox();
                }
            }
            if (MainForm->animation->game->form_visible && !handled)
            {
                handled = true;
                int form_button_shift = MainForm->animation->game->form_row_count;
                // Fewer than five rows make the form shorter and lift its buttons.
                if (form_button_shift < 5)
                    form_button_shift = INPUT_FORM_ROW_HEIGHT * (5 - form_button_shift);
                else
                    form_button_shift = 0;
                MainForm->animation->game->MouseDown(click_x, click_y);
                if (x >= MainForm->animation->game->form_x + 166 && x <= MainForm->animation->game->form_x + 254 &&
                    y >= MainForm->animation->game->form_y + 194 - form_button_shift &&
                    y <= MainForm->animation->game->form_y + 220 - form_button_shift)
                {
                    MainForm->edits->SetVisible(EditId_Amount, true);
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->game->CloseInputForm();
                    MainForm->redraw_needed = true;
                }
                if (x >= MainForm->animation->game->form_x + 73 && x <= MainForm->animation->game->form_x + 161 &&
                    y >= MainForm->animation->game->form_y + 194 - form_button_shift &&
                    y <= MainForm->animation->game->form_y + 220 - form_button_shift)
                {
                    MainForm->animation->game->SaveInputFormRows();
                    if (MainForm->animation->game->form_kind == FormKind_GuildCreate)
                    {
                        char create_failed = false;
                        String guild_tag = MainForm->edits->GetText(EditId_FormFirstRow);
                        String guild_name = MainForm->edits->GetText(EditId_FormSecondRow);
                        if (!create_failed && guild_tag == "")
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, true));
                            create_failed = true;
                        }
                        if (!create_failed && guild_name == "")
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildNameField, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildNameField, true));
                            create_failed = true;
                        }
                        if (!create_failed && MainForm->edits->GetTextLength(EditId_FormFirstRow) < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            create_failed = true;
                        }
                        if (!create_failed && MainForm->edits->GetTextLength(EditId_FormSecondRow) < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildShort, true));
                            create_failed = true;
                        }
                        if (!create_failed && LowerCase(guild_tag[1]) != LowerCase(guild_name[1]))
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_InvalidTag, false),
                                MainForm->strings->GetDialogText(DialogText_InvalidTag, true));
                            create_failed = true;
                        }
                        if (!create_failed && MainForm->animation->interface_controller->status->gold < 50000)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                                MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                                    MainForm->item_values->GetName(ItemId_Gold));
                            create_failed = true;
                        }
                        if (!create_failed)
                        {
                            MainForm->animation->guild->new_guild_tag = MainForm->edits->GetText(EditId_FormFirstRow);
                            MainForm->animation->guild->new_guild_name = MainForm->edits->GetText(EditId_FormSecondRow);
                            MainForm->animation->guild->new_guild_description = MainForm->edits->GetText(EditId_FormThirdRow);
                            MainForm->net_client->sender->RequestGuildCreate(MainForm->animation->guild->npc_session_id,
                                MainForm->animation->guild->new_guild_tag, MainForm->animation->guild->new_guild_name);
                        }
                    }
                    if (MainForm->animation->game->form_kind == FormKind_GuildJoin)
                    {
                        char join_failed = false;
                        String join_tag = MainForm->edits->GetText(EditId_FormFirstRow);
                        String join_detail = MainForm->edits->GetText(EditId_FormSecondRow);
                        if (!join_failed && join_tag == "")
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, true));
                            join_failed = true;
                        }
                        if (!join_failed && join_detail == "")
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputRecruiter, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputRecruiter, true));
                            join_failed = true;
                        }
                        if (!join_failed && MainForm->edits->GetTextLength(EditId_FormFirstRow) < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            join_failed = true;
                        }
                        if (!join_failed)
                        {
                            MainForm->net_client->sender->RequestGuildJoin(MainForm->animation->guild->npc_session_id,
                                join_tag, join_detail);
                            MainForm->animation->game->CloseInputForm();
                        }
                    }
                    if (MainForm->animation->game->form_kind == FormKind_GuildRankNames)
                    {
                        MainForm->animation->game->SaveInputFormRows();
                        MainForm->net_client->sender->SetGuildRankNames(MainForm->animation->guild->npc_session_id,
                            MainForm->animation->game->form_row_texts[0], MainForm->animation->game->form_row_texts[1],
                            MainForm->animation->game->form_row_texts[2], MainForm->animation->game->form_row_texts[3],
                            MainForm->animation->game->form_row_texts[4], MainForm->animation->game->form_row_texts[5],
                            MainForm->animation->game->form_row_texts[6], MainForm->animation->game->form_row_texts[7],
                            MainForm->animation->game->form_row_texts[8]);
                    }
                    if (MainForm->animation->game->form_kind == FormKind_GuildMemberRank)
                    {
                        char rank_failed = false;
                        String member_name = MainForm->edits->GetText(EditId_FormFirstRow);
                        String rank_text = MainForm->edits->GetText(EditId_FormSecondRow);
                        if (!rank_failed)
                        {
                            if ((int)(member_name == "" || rank_text == ""))
                            {
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_WrongInputFields, false),
                                    MainForm->strings->GetDialogText(DialogText_WrongInputFields, true));
                                rank_failed = true;
                            }
                        }
                        if (rank_text == "0")
                            rank_text = "9";
                        if (!rank_failed)
                        {
                            MainForm->animation->guild->target_member_name =
                                MainForm->net_client->Capitalize(member_name);
                            MainForm->net_client->sender->SetGuildMemberRank(MainForm->animation->guild->npc_session_id,
                                member_name, StrToInt(rank_text));
                            MainForm->animation->game->CloseInputForm();
                        }
                    }
                    if (MainForm->animation->game->form_kind == FormKind_ReportPlayer)
                    {
                        MainForm->net_client->sender->SendReportPlayer(MainForm->edits->GetText(EditId_FormFirstRow),
                            MainForm->edits->GetText(EditId_FormSecondRow));
                        MainForm->animation->game->CloseInputForm();
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_ThankYou, false),
                            MainForm->strings->GetDialogText(DialogText_ThankYou, true));
                    }
                }
            }
            if (MainForm->animation->social->is_open && !handled)
            {
                handled = true;
                MainForm->animation->social->OnMouseClick(click_x, click_y, button);
                if (x >= MainForm->animation->social->window_x + 48 &&
                    x <= MainForm->animation->social->window_x + 138 &&
                    y >= MainForm->animation->social->window_y + 253 &&
                    y <= MainForm->animation->social->window_y + 279)
                {
                    MainForm->animation->social->close_button_hover = false;
                    MainForm->animation->social->add_button_hover = false;
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    if (!MainForm->animation->social->list_kind)
                        MainForm->animation->game->OpenInputBox(InputBoxKind_AddFriend,
                            MainForm->strings->GetText(GameText_WhoDoYouWantTo), EditCharFilter_Alphanumeric, 14);
                    if (MainForm->animation->social->list_kind == 1)
                        MainForm->animation->game->OpenInputBox(InputBoxKind_AddIgnore,
                            MainForm->strings->GetText(GameText_WhoDoYouWant), EditCharFilter_Alphanumeric, 14);
                }
                if (x >= MainForm->animation->social->window_x + 144 &&
                    x <= MainForm->animation->social->window_x + 234 &&
                    y >= MainForm->animation->social->window_y + 253 &&
                    y <= MainForm->animation->social->window_y + 279)
                {
                    MainForm->edits->SetVisible(EditId_Amount, true);
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->social->CloseWindow();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->message_board->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->message_board->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->message_board->window_x + 284 &&
                    x <= MainForm->animation->message_board->window_x + 374 &&
                    y >= MainForm->animation->message_board->window_y + 253 &&
                    y <= MainForm->animation->message_board->window_y + 279)
                {
                    char board_handled = false;
                    if (MainForm->animation->message_board->view_mode <= 0 && !board_handled)
                    {
                        board_handled = true;
                        String own_name = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                        if (MainForm->animation->message_board->CountPostsByAuthor(own_name) < 2)
                        {
                            MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                            MainForm->animation->message_board->CreatePostEdits();
                            MainForm->edits->SetText(POST_SUBJECT_EDIT, "");
                            MainForm->edits->SetText(POST_MESSAGE_EDIT, "");
                            MainForm->edits->FocusEdit(POST_SUBJECT_EDIT);
                            MainForm->animation->message_board->current_post_id = -1;
                            MainForm->animation->message_board->current_post_author =
                                MainForm->strings->GetText(GameText_PostingNewMessage);
                            MainForm->animation->message_board->current_post_subject = "";
                            MainForm->animation->message_board->current_post_body = "";
                            MainForm->animation->message_board->is_composing = true;
                            MainForm->animation->message_board->view_mode = MSGBOARD_MODE_WRITE;
                            MainForm->redraw_needed = true;
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_TooManyMessages, false),
                                MainForm->strings->GetDialogText(DialogText_TooManyMessages, true));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    if (MainForm->animation->message_board->view_mode == 1 && !board_handled)
                    {
                        board_handled = true;
                        if (MainForm->net_client->GetAdminLevel() > 0)
                        {
                            MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                            MainForm->edits->SetEditable(POST_SUBJECT_EDIT, false);
                            MainForm->edits->SetEditable(POST_MESSAGE_EDIT, false);
                            MainForm->animation->message_board->DeletePost(MainForm->animation->message_board->current_post_id);
                        }
                    }
                    if (MainForm->animation->message_board->view_mode == MSGBOARD_MODE_WRITE && !board_handled)
                    {
                        board_handled = true;
                        if (MainForm->animation->message_board->is_composing)
                        {
                            String post_subject = MainForm->edits->GetText(POST_SUBJECT_EDIT);
                            String post_message = MainForm->edits->GetText(POST_MESSAGE_EDIT);
                            if (!post_subject.Length())
                            {
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_NoSubject, false),
                                    MainForm->strings->GetDialogText(DialogText_NoSubject, true));
                                MainForm->edits->caret_enabled = false;
                            }
                            if (!post_message.Length())
                            {
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_NoMessage, false),
                                    MainForm->strings->GetDialogText(DialogText_NoMessage, true));
                                MainForm->edits->caret_enabled = false;
                            }
                            if (post_subject.Length() > 0 && post_message.Length() > 0)
                            {
                                MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                                MainForm->edits->SetEditable(POST_SUBJECT_EDIT, false);
                                MainForm->edits->SetEditable(POST_MESSAGE_EDIT, false);
                                MainForm->animation->message_board->SubmitPost(post_subject, post_message);
                            }
                        }
                    }
                }
                if (x >= MainForm->animation->message_board->window_x + 380 &&
                    x <= MainForm->animation->message_board->window_x + 470 &&
                    y >= MainForm->animation->message_board->window_y + 253 &&
                    y <= MainForm->animation->message_board->window_y + 279)
                {
                    if (MainForm->animation->message_board->view_mode <= 0)
                        MainForm->edits->SetVisible(EditId_Amount, true);
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->message_board->Close();
                }
            }
            if (MainForm->animation->game->amount_box_visible && !handled)
            {
                handled = true;
                MainForm->animation->game->MouseDown(click_x, click_y);
                if (x >= MainForm->animation->game->amount_box_x + 60 &&
                    x <= MainForm->animation->game->amount_box_x + 148 &&
                    y >= MainForm->animation->game->amount_box_y + 125 &&
                    y <= MainForm->animation->game->amount_box_y + 151)
                {
                    if (MainForm->edits->GetText(EditId_Amount).Length() > 0)
                    {
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_DropItem)
                        {
                            int drop_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            if (drop_amount > 0)
                                MainForm->net_client->sender->SendDropItem(MainForm->animation->game->amount_item_id,
                                    drop_amount, MainForm->animation->game->drop_x, MainForm->animation->game->drop_y);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_JunkItem)
                        {
                            int junk_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            if (junk_amount > 0)
                                MainForm->net_client->sender->SendJunkItem(MainForm->animation->game->amount_item_id,
                                    junk_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_TradeAddItem)
                        {
                            int trade_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            if (trade_amount > 0 && !MainForm->animation->trade->trade_locked)
                                MainForm->net_client->sender->SendTradeAddItem(MainForm->animation->game->amount_item_id,
                                    trade_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ChestAddItem)
                        {
                            int chest_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            if (chest_amount > 0)
                            {
                                TPoint chest_pos;
                                chest_pos.x = MainForm->animation->chest->chest_x;
                                chest_pos.y = MainForm->animation->chest->chest_y;
                                MainForm->net_client->sender->SendChestAddItem(chest_pos,
                                    MainForm->animation->game->amount_item_id, chest_amount);
                            }
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_BankDeposit)
                        {
                            int deposit_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            MainForm->net_client->sender->SendBankDeposit(MainForm->animation->bank->npc_session_id,
                                deposit_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_BankWithdraw)
                        {
                            int withdraw_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            MainForm->net_client->sender->SendBankWithdraw(MainForm->animation->bank->npc_session_id,
                                withdraw_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ShopBuy)
                        {
                            MainForm->animation->shop->selected_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            String buy_question = MainForm->strings->GetText(GameText_Buy);
                            buy_question = buy_question + " ";
                            buy_question = buy_question + MainForm->edits->GetText(EditId_Amount);
                            buy_question = buy_question + " ";
                            buy_question = buy_question +
                                MainForm->item_values->GetName(MainForm->animation->shop->selected_item_id);
                            buy_question = buy_question + " ";
                            buy_question = buy_question + MainForm->strings->GetText(GameText_For);
                            buy_question = buy_question + " ";
                            buy_question =
                                buy_question +
                                    IntToStr(StrToInt(MainForm->edits->GetText(EditId_Amount)) *
                                        MainForm->animation->shop->selected_price);
                            buy_question = buy_question + " ";
                            buy_question = buy_question + LowerCase(MainForm->item_values->GetName(ItemId_Gold));
                            buy_question = buy_question + " ?";
                            MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_BuyItem,
                                MainForm->strings->GetText(GameText_BuyItemS), buy_question);
                            MainForm->edits->caret_enabled = false;
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ShopSell)
                        {
                            MainForm->animation->shop->selected_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            String sell_question = MainForm->strings->GetText(GameText_Sell);
                            sell_question = sell_question + " ";
                            sell_question = sell_question + MainForm->edits->GetText(EditId_Amount);
                            sell_question = sell_question + " ";
                            sell_question = sell_question +
                                MainForm->item_values->GetName(MainForm->animation->shop->selected_item_id);
                            sell_question = sell_question + " ";
                            sell_question = sell_question + MainForm->strings->GetText(GameText_For);
                            sell_question = sell_question + " ";
                            sell_question =
                                sell_question +
                                    IntToStr(StrToInt(MainForm->edits->GetText(EditId_Amount)) *
                                        MainForm->animation->shop->selected_price);
                            sell_question = sell_question + " ";
                            sell_question = sell_question + LowerCase(MainForm->item_values->GetName(ItemId_Gold));
                            sell_question = sell_question + " ?";
                            MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_SellItem,
                                MainForm->strings->GetText(GameText_SellItemsS), sell_question);
                            MainForm->edits->caret_enabled = false;
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_LockerAddItem)
                        {
                            int locker_amount = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            if (locker_amount > 0)
                            {
                                TPoint locker_pos;
                                locker_pos.x = MainForm->animation->locker->locker_x;
                                locker_pos.y = MainForm->animation->locker->locker_y;
                                int locker_total =
                                    MainForm->animation->locker->GetItemAmount(MainForm->animation->game->amount_item_id) +
                                        locker_amount;
                                if (locker_total <= 200)
                                    MainForm->net_client->sender->SendLockerAddItem(locker_pos,
                                        MainForm->animation->game->amount_item_id, locker_amount);
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LockerFullStore, false),
                                        MainForm->strings->GetDialogText(DialogText_LockerFullStore, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_GuildDeposit)
                        {
                            int guild_deposit = StrToInt(MainForm->edits->GetText(EditId_Amount));
                            if (guild_deposit < 1000)
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_DepositMinimum, false),
                                    MainForm->strings->GetDialogText(DialogText_DepositMinimum, true) + " " +
                                        MainForm->item_values->GetName(ItemId_Gold));
                            else
                            {
                                MainForm->animation->guild->guild_bank_gold += guild_deposit;
                                MainForm->net_client->sender->SendGuildDeposit(MainForm->animation->guild->npc_session_id,
                                    guild_deposit);
                            }
                        }
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                        MainForm->animation->interface_controller->inventory->hover_col = -1;
                        MainForm->animation->interface_controller->inventory->hover_row = -1;
                        MainForm->animation->interface_controller->inventory->MarkItemsDirty();
                        MainForm->animation->interface_controller->inventory->MarkHoverDirty();
                        MainForm->animation->game->amount_box_visible = false;
                        MainForm->edits->RemoveEdit(EditId_Amount);
                        MainForm->edits->FocusEdit(EditId_Chat);
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                }
                if (x >= MainForm->animation->game->amount_box_x + 153 &&
                    x <= MainForm->animation->game->amount_box_x + 241 &&
                    y >= MainForm->animation->game->amount_box_y + 125 &&
                    y <= MainForm->animation->game->amount_box_y + 151)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                    MainForm->animation->interface_controller->inventory->hover_col = -1;
                    MainForm->animation->interface_controller->inventory->hover_row = -1;
                    MainForm->animation->interface_controller->inventory->MarkItemsDirty();
                    MainForm->animation->interface_controller->inventory->MarkHoverDirty();
                    MainForm->animation->game->amount_box_visible = false;
                    MainForm->edits->RemoveEdit(EditId_Amount);
                    MainForm->edits->FocusEdit(EditId_Chat);
                    MainForm->edits->caret_enabled = true;
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->trade->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->trade->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->trade->window_x + 357 &&
                    x <= MainForm->animation->trade->window_x + 445 &&
                    y >= MainForm->animation->trade->window_y + 253 && y <= MainForm->animation->trade->window_y + 280)
                {
                    if (MainForm->animation->trade->partner_items.size() > 0 &&
                        MainForm->animation->trade->own_items.size() > 0)
                    {
                        if (!MainForm->animation->trade->trade_locked && !MainForm->animation->trade->IsAcceptDelayed())
                        {
                            if (MainForm->animation->interface_controller->inventory->CanFitTradeItems())
                            {
                                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel,
                                    ConfirmAction_TradeAgree, MainForm->strings->GetDialogText(DialogText_TradeAgree,
                                    false), MainForm->strings->GetDialogText(DialogText_TradeAgree, true));
                                MainForm->edits->caret_enabled = false;
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetText(GameText_Warning),
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                    }
                    else
                    {
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_BothPlayersNeedTo));
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetText(GameText_Warning),
                            MainForm->strings->GetText(GameText_BothPlayersNeedTo));
                        MainForm->edits->caret_enabled = false;
                    }
                }
                if (x >= MainForm->animation->trade->window_x + 450 &&
                    x <= MainForm->animation->trade->window_x + 538 &&
                    y >= MainForm->animation->trade->window_y + 253 && y <= MainForm->animation->trade->window_y + 280)
                {
                    if (!MainForm->animation->trade->own_agreed)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->trade->Close();
                        MainForm->net_client->sender->SendTradeClose();
                        MainForm->animation->trade->trade_locked = false;
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                            MainForm->strings->GetText(GameText_TradeHasBeenAborted));
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->sfx->PlaySfx(SfxSound_Leave);
                        MainForm->animation->trade->own_agreed = false;
                        MainForm->animation->trade->own_status_text = MainForm->strings->GetText(GameText_Trading);
                        MainForm->net_client->sender->SendTradeDisagree();
                        MainForm->animation->trade->trade_locked = false;
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                            MainForm->strings->GetText(GameText_YouCanceledDeal));
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->visual_doc->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->visual_doc->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->visual_doc->window_x + 159 &&
                    x <= MainForm->animation->visual_doc->window_x + 249 &&
                    y >= MainForm->animation->visual_doc->window_y + 255 &&
                    y <= MainForm->animation->visual_doc->window_y + 282)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->visual_doc->Close();
                }
            }
            if (MainForm->animation->hotkeys->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->hotkeys->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->hotkeys->window_x + 182 &&
                    x <= MainForm->animation->hotkeys->window_x + 272 &&
                    y >= MainForm->animation->hotkeys->window_y + 132 &&
                    y <= MainForm->animation->hotkeys->window_y + 165)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->hotkeys->Close();
                }
            }
            if (MainForm->animation->paperdoll->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->paperdoll->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->paperdoll->window_x + 277 &&
                    x <= MainForm->animation->paperdoll->window_x + 367 &&
                    y >= MainForm->animation->paperdoll->window_y + 254 &&
                    y <= MainForm->animation->paperdoll->window_y + 281)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->paperdoll->Close();
                }
            }
            if (MainForm->animation->book->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->book->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->book->window_x + 277 && x <= MainForm->animation->book->window_x + 367 &&
                    y >= MainForm->animation->book->window_y + 254 && y <= MainForm->animation->book->window_y + 281)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->book->is_visible = false;
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->bank->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->bank->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->bank->window_x + 93 && x <= MainForm->animation->bank->window_x + 181 &&
                    y >= MainForm->animation->bank->window_y + 192 && y <= MainForm->animation->bank->window_y + 218)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->bank->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->musician->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->musician->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->musician->window_x + 93 &&
                    x <= MainForm->animation->musician->window_x + 181 &&
                    y >= MainForm->animation->musician->window_y + 82 &&
                    y <= MainForm->animation->musician->window_y + 108)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->musician->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->jukebox->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->jukebox->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->jukebox->window_x + 93 &&
                    x <= MainForm->animation->jukebox->window_x + 181 &&
                    y >= MainForm->animation->jukebox->window_y + 155 &&
                    y <= MainForm->animation->jukebox->window_y + 181)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->jukebox->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->learn->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->learn->MouseDown(click_x, click_y, button);
                if (MainForm->animation->learn->page == LearnPage_Menu)
                {
                    if (x >= MainForm->animation->learn->window_x + 92 &&
                        x <= MainForm->animation->learn->window_x + 182 &&
                        y >= MainForm->animation->learn->window_y + 253 &&
                        y <= MainForm->animation->learn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->learn->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (x >= MainForm->animation->learn->window_x + 144 &&
                        x <= MainForm->animation->learn->window_x + 234 &&
                        y >= MainForm->animation->learn->window_y + 253 &&
                        y <= MainForm->animation->learn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->learn->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (x >= MainForm->animation->learn->window_x + 48 &&
                        x <= MainForm->animation->learn->window_x + 138 &&
                        y >= MainForm->animation->learn->window_y + 253 &&
                        y <= MainForm->animation->learn->window_y + 279)
                    {
                        if (MainForm->animation->learn->page == LearnPage_SkillList ||
                            MainForm->animation->learn->page == LearnPage_Reset)
                        {
                            LearnController *learn = MainForm->animation->learn;
                            learn->shown_skills.clear();
                            MainForm->animation->learn->ClearText();
                            MainForm->animation->learn->page = LearnPage_Menu;
                        }
                        if (MainForm->animation->learn->page == LearnPage_SkillInfo)
                            MainForm->animation->learn->page = LearnPage_SkillList;
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->crafter->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->crafter->MouseDown(click_x, click_y, button);
            }
            if (MainForm->animation->shop->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->shop->MouseDown(click_x, click_y, button);
                if (MainForm->animation->shop->page == ShopPage_Menu)
                {
                    if (x >= MainForm->animation->shop->window_x + 92 &&
                        x <= MainForm->animation->shop->window_x + 182 &&
                        y >= MainForm->animation->shop->window_y + 253 &&
                        y <= MainForm->animation->shop->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->shop->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (x >= MainForm->animation->shop->window_x + 144 &&
                        x <= MainForm->animation->shop->window_x + 234 &&
                        y >= MainForm->animation->shop->window_y + 253 &&
                        y <= MainForm->animation->shop->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->shop->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (x >= MainForm->animation->shop->window_x + 48 &&
                        x <= MainForm->animation->shop->window_x + 138 &&
                        y >= MainForm->animation->shop->window_y + 253 &&
                        y <= MainForm->animation->shop->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        ShopController *shop = MainForm->animation->shop;
                        shop->shown_items.clear();
                        MainForm->animation->shop->ResetScroll();
                        MainForm->animation->shop->page = ShopPage_Menu;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->locker->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->locker->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->locker->window_x + 92 &&
                    x <= MainForm->animation->locker->window_x + 182 &&
                    y >= MainForm->animation->locker->window_y + 253 &&
                    y <= MainForm->animation->locker->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->locker->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->guild->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->guild->MouseDown(click_x, click_y, button);
                if (MainForm->animation->guild->page == GuildPage_Main)
                {
                    if (x >= MainForm->animation->guild->window_x + 92 &&
                        x <= MainForm->animation->guild->window_x + 182 &&
                        y >= MainForm->animation->guild->window_y + 253 &&
                        y <= MainForm->animation->guild->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->guild->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (x >= MainForm->animation->guild->window_x + 144 &&
                        x <= MainForm->animation->guild->window_x + 234 &&
                        y >= MainForm->animation->guild->window_y + 253 &&
                        y <= MainForm->animation->guild->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->guild->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (x >= MainForm->animation->guild->window_x + 48 &&
                        x <= MainForm->animation->guild->window_x + 138 &&
                        y >= MainForm->animation->guild->window_y + 253 &&
                        y <= MainForm->animation->guild->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->guild->ClearText();
                        if (MainForm->animation->guild->page <= GuildPage_Lookup)
                            MainForm->animation->guild->page = GuildPage_Main;
                        if (MainForm->animation->guild->page == GuildPage_Join ||
                            MainForm->animation->guild->page == GuildPage_Leave ||
                            MainForm->animation->guild->page == GuildPage_Create)
                            MainForm->animation->guild->page = GuildPage_Membership;
                        if (MainForm->animation->guild->page == GuildPage_Info ||
                            MainForm->animation->guild->page == GuildPage_MemberList)
                            MainForm->animation->guild->page = GuildPage_Lookup;
                        if (MainForm->animation->guild->page == GuildPage_Bank)
                            MainForm->animation->guild->page = GuildPage_Main;
                        if (MainForm->animation->guild->page == GuildPage_Description ||
                            MainForm->animation->guild->page == GuildPage_RemoveMember ||
                            MainForm->animation->guild->page == GuildPage_Disband ||
                            MainForm->animation->guild->page == GuildPage_Rank)
                            MainForm->animation->guild->page = GuildPage_Manage;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->dialog->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->dialog->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->dialog->window_x + 89 &&
                    x <= MainForm->animation->dialog->window_x + 169 &&
                    y >= MainForm->animation->dialog->window_y + 153 &&
                    y <= MainForm->animation->dialog->window_y + 179)
                {
                    if (MainForm->animation->dialog->prev_button_kind == 1)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->dialog->Close();
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->sfx->PlaySfx(SfxSound_Select);
                        MainForm->animation->dialog->PreviousPage();
                        MainForm->redraw_needed = true;
                    }
                }
                if (x >= MainForm->animation->dialog->window_x + 183 &&
                    x <= MainForm->animation->dialog->window_x + 273 &&
                    y >= MainForm->animation->dialog->window_y + 153 &&
                    y <= MainForm->animation->dialog->window_y + 179)
                {
                    if (MainForm->animation->dialog->showing_quest_list)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->dialog->Close();
                        MainForm->redraw_needed = true;
                    }
                    else if (MainForm->animation->dialog->next_button_kind == DIALOG_BUTTON_OK_FRAME)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->net_client->sender->SendQuestDialogOk(MainForm->animation->dialog->dialog_echo_a,
                            MainForm->animation->dialog->quest_id, MainForm->animation->dialog->dialog_echo_b);
                        MainForm->animation->dialog->Close();
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->sfx->PlaySfx(SfxSound_Select);
                        MainForm->animation->dialog->NextPage();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->memo->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->memo->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->memo->window_x + 92 && x <= MainForm->animation->memo->window_x + 182 &&
                    y >= MainForm->animation->memo->window_y + 253 && y <= MainForm->animation->memo->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->memo->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->report->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->report->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->report->window_x + 92 &&
                    x <= MainForm->animation->report->window_x + 182 &&
                    y >= MainForm->animation->report->window_y + 253 &&
                    y <= MainForm->animation->report->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->report->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->law->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->law->MouseDown(click_x, click_y, button);
                if (!MainForm->animation->law->page)
                {
                    if (x >= MainForm->animation->law->window_x + 92 && x <= MainForm->animation->law->window_x + 182 &&
                        y >= MainForm->animation->law->window_y + 253 && y <= MainForm->animation->law->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->law->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (x >= MainForm->animation->law->window_x + 144 &&
                        x <= MainForm->animation->law->window_x + 234 &&
                        y >= MainForm->animation->law->window_y + 253 && y <= MainForm->animation->law->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->law->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (x >= MainForm->animation->law->window_x + 48 && x <= MainForm->animation->law->window_x + 138 &&
                        y >= MainForm->animation->law->window_y + 253 && y <= MainForm->animation->law->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->law->ClearText();
                        if (MainForm->animation->law->page == 1)
                            MainForm->animation->law->page = 0;
                        if (MainForm->animation->law->page == 6 || MainForm->animation->law->page == 7)
                            MainForm->animation->law->page = 1;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->inn->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->inn->MouseDown(click_x, click_y, button);
                if (!MainForm->animation->inn->page)
                {
                    if (x >= MainForm->animation->inn->window_x + 92 && x <= MainForm->animation->inn->window_x + 182 &&
                        y >= MainForm->animation->inn->window_y + 253 && y <= MainForm->animation->inn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->inn->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (x >= MainForm->animation->inn->window_x + 144 &&
                        x <= MainForm->animation->inn->window_x + 234 &&
                        y >= MainForm->animation->inn->window_y + 253 && y <= MainForm->animation->inn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->inn->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (x >= MainForm->animation->inn->window_x + 48 && x <= MainForm->animation->inn->window_x + 138 &&
                        y >= MainForm->animation->inn->window_y + 253 && y <= MainForm->animation->inn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->inn->ClearText();
                        if (MainForm->animation->inn->page == 1)
                            MainForm->animation->inn->page = 0;
                        if (MainForm->animation->inn->page == 6 || MainForm->animation->inn->page == 7)
                            MainForm->animation->inn->page = 1;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->chest->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->chest->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->chest->window_x + 92 && x <= MainForm->animation->chest->window_x + 182 &&
                    y >= MainForm->animation->chest->window_y + 231 && y <= MainForm->animation->chest->window_y + 254)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->chest->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->barber->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->barber->MouseDown(click_x, click_y, button);
            }
            if (MainForm->animation->stat->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->stat->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->stat->window_x + 92 && x <= MainForm->animation->stat->window_x + 182 &&
                    y >= MainForm->animation->stat->window_y + 213 && y <= MainForm->animation->stat->window_y + 239)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->stat->Close();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->quests->is_visible && !handled)
            {
                handled = true;
                MainForm->animation->quests->MouseDown(click_x, click_y, button);
                if (x >= MainForm->animation->quests->window_x + 288 &&
                    x <= MainForm->animation->quests->window_x + 378 &&
                    y >= MainForm->animation->quests->window_y + 253 &&
                    y <= MainForm->animation->quests->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    if (!MainForm->animation->quests->history_requested)
                    {
                        MainForm->animation->quests->history_requested = true;
                        MainForm->net_client->sender->RequestQuestHistory();
                    }
                    if (!MainForm->animation->quests->showing_history)
                    {
                        String second_page_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                        second_page_title = MainForm->net_client->Capitalize(second_page_title);
                        second_page_title =
                            second_page_title + "'s " + MainForm->strings->GetText(GameText_QuestHistory);
                        MainForm->animation->quests->title = second_page_title;
                        MainForm->animation->quests->ResetScroll();
                        MainForm->animation->quests->showing_history = true;
                    }
                    else
                    {
                        String first_page_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                        first_page_title = MainForm->net_client->Capitalize(first_page_title);
                        first_page_title =
                            first_page_title + "'s " + MainForm->strings->GetText(GameText_QuestProgress);
                        MainForm->animation->quests->title = first_page_title;
                        MainForm->animation->quests->ResetScroll();
                        MainForm->animation->quests->showing_history = false;
                    }
                    MainForm->redraw_needed = true;
                }
                if (x >= MainForm->animation->quests->window_x + 380 &&
                    x <= MainForm->animation->quests->window_x + 470 &&
                    y >= MainForm->animation->quests->window_y + 253 &&
                    y <= MainForm->animation->quests->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->quests->Close();
                    MainForm->redraw_needed = true;
                }
            }
        }
        else
        {
            // No window is open: the corner close button, the bars along the top, then the map.
            char bar_clicked = false;
            if (click_x >= 594 && click_x <= 623 && click_y > 332 && click_y < 350)
            {
                bar_clicked = true;
                // During a warp or a map download the close button logs out without asking.
                if (MainForm->map->warp_pending || MainForm->map->loading_map)
                    MainForm->animation->interface_controller->Logout();
                else
                {
                    MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_Logout,
                        MainForm->strings->GetDialogText(DialogText_ExitGame, false),
                        MainForm->strings->GetDialogText(DialogText_ExitGame, true));
                    MainForm->edits->caret_enabled = false;
                }
            }
            // Only the top edge of the strip and its two ends belong to the bars; the rest of it is the map.
            if (click_x >= 55 && click_x <= 563 && click_y <= 13)
            {
                char on_bar = false;
                if (click_y <= 10)
                    on_bar = true;
                if (click_x >= 55 && click_x <= 94)
                    on_bar = true;
                if (click_x >= 546 && click_x <= 565)
                    on_bar = true;
                if (on_bar)
                {
                    bar_clicked = true;
                    MainForm->animation->interface_controller->game_bars->MouseDown(click_x, click_y, button);
                }
            }
            if (MainForm->gfx->game->view_clip_rect.top < click_y &&
                MainForm->gfx->game->view_clip_rect.bottom > click_y &&
                MainForm->gfx->game->view_clip_rect.left < click_x &&
                MainForm->gfx->game->view_clip_rect.right > click_x && !bar_clicked)
            {
                char drop_handled = false;
                if (MainForm->animation->interface_controller->spells->is_dragging)
                {
                    if (!button && MainForm->animation->hotkeys->is_visible)
                    {
                        drop_handled = true;
                        MainForm->animation->hotkeys->DropSpell(MainForm->animation->interface_controller->spells->drag_spell_id,
                            click_x, click_y);
                    }
                }
                else if (MainForm->animation->interface_controller->inventory->is_dragging)
                {
                    if (!MainForm->animation->interface_controller->inventory->dragging_from_paperdoll && !button)
                    {
                        if (MainForm->animation->trade->is_visible)
                        {
                            drop_handled = true;
                            if (MainForm->animation->trade->window_y + 43 < click_y &&
                                MainForm->animation->trade->window_y + 243 > click_y &&
                                MainForm->animation->trade->window_x + 280 < click_x &&
                                MainForm->animation->trade->window_x + 515 > click_x &&
                                !MainForm->animation->game->message_visible &&
                                !MainForm->animation->trade->trade_locked)
                            {
                                if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                                    ItemSpecial_Bound)
                                {
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                                else if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                    2)
                                {
                                    MainForm->net_client->sender->SendTradeAddItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                        1);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                                else
                                {
                                    MainForm->animation->game->OpenAmountBox(AmountBoxKind_TradeAddItem,
                                        MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                        MainForm->animation->interface_controller->inventory->drag_item_id, 0);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                            }
                        }
                        if (MainForm->animation->hotkeys->is_visible)
                        {
                            drop_handled = true;
                            MainForm->animation->hotkeys->DropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                click_x, click_y);
                        }
                        if (MainForm->animation->paperdoll->is_visible)
                        {
                            drop_handled = true;
                            MainForm->animation->paperdoll->EquipDroppedItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                click_x, click_y);
                        }
                        if (MainForm->animation->bank->is_visible)
                        {
                            drop_handled = true;
                            if (MainForm->animation->bank->window_y + 50 < click_y &&
                                MainForm->animation->bank->window_y + 170 > click_y &&
                                MainForm->animation->bank->window_x + 17 < click_x &&
                                MainForm->animation->bank->window_x + 252 > click_x &&
                                MainForm->animation->interface_controller->inventory->drag_item_id == ItemId_Gold)
                            {
                                int gold_amount =
                                    MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                                if (gold_amount > 0)
                                {
                                    if (gold_amount == 1)
                                        MainForm->net_client->sender->SendBankDeposit(MainForm->animation->bank->npc_session_id,
                                            1);
                                    else
                                    {
                                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_BankDeposit, gold_amount,
                                            ItemId_Gold, 0);
                                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                                    }
                                }
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, false),
                                        MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                        MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                                }
                            }
                        }
                        if (MainForm->animation->locker->is_visible)
                        {
                            drop_handled = true;
                            if (MainForm->animation->locker->window_y + 17 < click_y &&
                                MainForm->animation->locker->window_y + 217 > click_y &&
                                MainForm->animation->locker->window_x + 17 < click_x &&
                                MainForm->animation->locker->window_x + 252 > click_x)
                            {
                                if (MainForm->animation->interface_controller->inventory->drag_item_id > 1)
                                {
                                    if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                        2)
                                    {
                                        TPoint drop_locker_pos;
                                        drop_locker_pos.x = MainForm->animation->locker->locker_x;
                                        drop_locker_pos.y = MainForm->animation->locker->locker_y;
                                        if (MainForm->animation->locker->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                            200)
                                        {
                                            if (MainForm->animation->locker->CanStoreItem(MainForm->animation->interface_controller->inventory->drag_item_id))
                                            {
                                                MainForm->net_client->sender->SendLockerAddItem(drop_locker_pos,
                                                    MainForm->animation->interface_controller->inventory->drag_item_id,
                                                    1);
                                                MainForm->animation->interface_controller->inventory->input_enabled =
                                                    true;
                                            }
                                            else
                                            {
                                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                    ConfirmAction_None,
                                                    MainForm->strings->GetDialogText(DialogText_LockerFullHold, false),
                                                    MainForm->strings->GetDialogText(DialogText_LockerFullHold, true));
                                                MainForm->edits->caret_enabled = false;
                                            }
                                        }
                                        else
                                        {
                                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                ConfirmAction_None,
                                                MainForm->strings->GetDialogText(DialogText_LockerFullStore, false),
                                                MainForm->strings->GetDialogText(DialogText_LockerFullStore, true));
                                            MainForm->edits->caret_enabled = false;
                                        }
                                    }
                                    else
                                    {
                                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_LockerAddItem,
                                            MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                            MainForm->animation->interface_controller->inventory->drag_item_id, 0);
                                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                                    }
                                }
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_RefusedDeposit, false),
                                        MainForm->strings->GetDialogText(DialogText_RefusedDeposit, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                        }
                        if (MainForm->animation->chest->is_visible)
                        {
                            drop_handled = true;
                            if (MainForm->animation->chest->window_y + 17 < click_y &&
                                MainForm->animation->chest->window_y + 217 > click_y &&
                                MainForm->animation->chest->window_x + 17 < click_x &&
                                MainForm->animation->chest->window_x + 252 > click_x)
                            {
                                if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                                    ItemSpecial_Bound)
                                {
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                                else if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                    2)
                                {
                                    TPoint drop_chest_pos;
                                    drop_chest_pos.x = MainForm->animation->chest->chest_x;
                                    drop_chest_pos.y = MainForm->animation->chest->chest_y;
                                    MainForm->net_client->sender->SendChestAddItem(drop_chest_pos,
                                        MainForm->animation->interface_controller->inventory->drag_item_id, 1);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                                else
                                {
                                    MainForm->animation->game->OpenAmountBox(AmountBoxKind_ChestAddItem,
                                        MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                        MainForm->animation->interface_controller->inventory->drag_item_id, 0);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                            }
                        }
                        if (!drop_handled)
                        {
                            char can_drop;
                            if (MainForm->net_client->IsWithinReach(MainForm->map->GetCenterX(),
                                MainForm->map->GetCenterY(), MainForm->map->GetCursorX(),
                                MainForm->map->GetCursorY()) &&
                                !MainForm->map->IsWallOrSpecTile(MainForm->map->GetCursorX(),
                                    MainForm->map->GetCursorY()))
                            {
                                MainForm->animation->interface_controller->inventory->CancelDrag();
                                can_drop = true;
                                if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                                    ItemSpecial_Bound)
                                {
                                    can_drop = false;
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                                if (can_drop &&
                                    MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
                                {
                                    can_drop = false;
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetText(GameText_Warning),
                                        MainForm->strings->GetText(GameText_YouCannotDropItems));
                                    MainForm->edits->caret_enabled = false;
                                }
                                if (MainForm->map_items->GetProtectionOwnerAt(MainForm->map->GetCursorX(),
                                    MainForm->map->GetCursorY()) > 0)
                                    can_drop = false;
                                if (can_drop)
                                {
                                    if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                        2)
                                    {
                                        if (MainForm->map->GetCursorX() >= 0 && MainForm->map->GetCursorY() >= 0)
                                        {
                                            if (MainForm->map->GetCursorX() <= MainForm->map->GetWidth())
                                            {
                                                if (MainForm->map->GetCursorY() <= MainForm->map->GetHeight())
                                                {
                                                    if (MainForm->animation->interface_controller->inventory->drag_item_id ==
                                                        1)
                                                    {
                                                        if (!MainForm->animation->interface_controller->inventory->GetItemAmount(1))
                                                        {
                                                            String gold_name = MainForm->item_values->GetName(ItemId_Gold);
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                                MainForm->strings->GetText(GameText_YouHaveNoMore) +
                                                                    " " + gold_name);
                                                        }
                                                        else
                                                        {
                                                            MainForm->net_client->sender->SendDropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                                                1, MainForm->map->GetCursorX(),
                                                                MainForm->map->GetCursorY());
                                                            MainForm->animation->interface_controller->inventory->input_enabled =
                                                                true;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        MainForm->net_client->sender->SendDropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                                            1, MainForm->map->GetCursorX(),
                                                            MainForm->map->GetCursorY());
                                                        MainForm->animation->interface_controller->inventory->input_enabled =
                                                            true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        MainForm->animation->game->OpenAmountBoxAtTile(AmountBoxKind_DropItem,
                                            MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                            MainForm->animation->interface_controller->inventory->drag_item_id, 0,
                                            MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                                        MainForm->animation->interface_controller->inventory->input_enabled = false;
                                    }
                                }
                            }
                            else if (!MainForm->net_client->IsWithinReach(MainForm->map->GetCenterX(),
                                MainForm->map->GetCenterY(), MainForm->map->GetCursorX(), MainForm->map->GetCursorY()))
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_OutOfDropRange));
                                MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_OutOfRange,
                                    CHAT_COLOR_DEFAULT, MainForm->strings->GetText(GameText_OutOfDropRange), true);
                            }
                        }
                    }
                }
                else
                {
                    MainForm->map->path_finder->CancelPath();
                    if (!button)
                    {
                        char click_used = false;
                        // The menu a right click opens on a player.
                        // Its options in order: paperdoll, book, join, invite, trade, private message, friend, ignore.
                        if (MainForm->animation->game->player_menu_visible)
                        {
                            if (MainForm->animation->game->player_menu_x + 12 < click_x &&
                                MainForm->animation->game->player_menu_y + 142 > click_y &&
                                MainForm->animation->game->player_menu_x + 101 > click_x &&
                                MainForm->animation->game->player_menu_y + 5 < click_y)
                            {
                                if (MainForm->animation->game->player_menu_hovered_option >= 0)
                                {
                                    char close_menu = true;
                                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                                    if (!MainForm->animation->game->player_menu_hovered_option)
                                    {
                                        MainForm->animation->hotkeys->Close();
                                        MainForm->net_client->sender->RequestPaperdoll(MainForm->animation->game->player_menu_target_id);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_PaperdollIsNowViewed));
                                        close_menu = true;
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 1)
                                    {
                                        MainForm->net_client->sender->RequestBook(MainForm->animation->game->player_menu_target_id);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_BookIsNowViewed));
                                        close_menu = true;
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 2)
                                    {
                                        if (!MainForm->animation->interface_controller->group->IsInviteCoolingDown())
                                        {
                                            if (!MainForm->animation->interface_controller->group->HasMember(MainForm->animation->game->player_menu_target_id))
                                            {
                                                MainForm->animation->interface_controller->group->SetInviteCooldown(200);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                                    MainForm->strings->GetText(GameText_RequestedToJoinParty));
                                                MainForm->net_client->sender->RequestPartyJoin(MainForm->animation->game->player_menu_target_id);
                                            }
                                            else
                                            {
                                                String existing_member =
                                                    MainForm->animation->interface_controller->group->GetMemberName(MainForm->animation->game->player_menu_target_id);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->net_client->Capitalize(existing_member) + " " +
                                                        MainForm->strings->GetText(GameText_IsAlreadyInYour));
                                                MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_Error, CHAT_COLOR_WARNING,
                                                    MainForm->net_client->Capitalize(existing_member) + " " +
                                                        MainForm->strings->GetText(GameText_IsAlreadyInYour), false);
                                            }
                                        }
                                        else
                                        {
                                            close_menu = false;
                                            MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_Error, CHAT_COLOR_WARNING,
                                                MainForm->strings->GetText(GameText_YouRecentlyRequestedAnother),
                                                false);
                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                MainForm->strings->GetText(GameText_YouRecentlyRequestedAnother));
                                        }
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 3)
                                    {
                                        if (!MainForm->animation->interface_controller->group->IsInviteCoolingDown())
                                        {
                                            if (!MainForm->animation->interface_controller->group->HasMember(MainForm->animation->game->player_menu_target_id))
                                            {
                                                MainForm->animation->interface_controller->group->SetInviteCooldown(300);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                                    MainForm->net_client->Capitalize(
                                                    MainForm->animation->game->player_menu_name) + " " +
                                                    MainForm->strings->GetText(GameText_IsInvitedToParty));
                                                MainForm->net_client->sender->RequestPartyInvite(MainForm->animation->game->player_menu_target_id);
                                            }
                                            else
                                            {
                                                String party_member =
                                                    MainForm->animation->interface_controller->group->GetMemberName(MainForm->animation->game->player_menu_target_id);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->net_client->Capitalize(party_member) + " " +
                                                        MainForm->strings->GetText(GameText_IsAlreadyInYour));
                                                MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_Error, CHAT_COLOR_WARNING,
                                                    MainForm->net_client->Capitalize(party_member) + " " +
                                                        MainForm->strings->GetText(GameText_IsAlreadyInYour), false);
                                            }
                                        }
                                        else
                                        {
                                            close_menu = false;
                                            MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_Error, CHAT_COLOR_WARNING,
                                                MainForm->strings->GetText(GameText_YouRecentlyRequestedAnother),
                                                false);
                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                                MainForm->strings->GetText(GameText_YouRecentlyRequestedAnother));
                                        }
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 4)
                                    {
                                        if (MainForm->map->GetMapId() != MainForm->animation->game->restricted_map_id)
                                        {
                                            if (MainForm->animation->interface_controller->status->weight <
                                                MainForm->animation->interface_controller->status->max_weight)
                                            {
                                                if (!MainForm->animation->interface_controller->group->IsInviteCoolingDown())
                                                {
                                                    MainForm->animation->interface_controller->group->SetInviteCooldown(300);
                                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                                        MainForm->strings->GetText(GameText_RequestedToTrade));
                                                    MainForm->net_client->sender->RequestTrade(MainForm->animation->game->player_menu_target_id);
                                                }
                                                else
                                                {
                                                    close_menu = false;
                                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                        ConfirmAction_None,
                                                        MainForm->strings->GetDialogText(DialogText_TradeWarning,
                                                        false),
                                                        MainForm->strings->GetText(GameText_YouRecentlyRequestedAnotherTrade));
                                                    MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_Error,
                                                        CHAT_COLOR_WARNING,
                                                        MainForm->strings->GetText(GameText_YouRecentlyRequestedAnotherTrade),
                                                        false);
                                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                                        MainForm->strings->GetText(GameText_YouRecentlyRequestedAnotherTrade));
                                                }
                                            }
                                            else
                                            {
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                    ConfirmAction_None, MainForm->strings->GetText(GameText_Warning),
                                                    MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                                                MainForm->edits->caret_enabled = false;
                                            }
                                        }
                                        else
                                        {
                                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                ConfirmAction_None, MainForm->strings->GetText(GameText_Warning),
                                                MainForm->strings->GetText(GameText_YouCannotTradeIn));
                                            MainForm->edits->caret_enabled = false;
                                        }
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 5)
                                        MainForm->edits->SetText(EditId_Chat,
                                            "!" +
                                            MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name) + " ");
                                    if (MainForm->animation->game->player_menu_hovered_option == 6)
                                    {
                                        MainForm->animation->social->ToggleFriend(MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name));
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_WillBeYourFriend));
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 7)
                                    {
                                        MainForm->animation->social->ToggleIgnore(MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name));
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            MainForm->net_client->Capitalize(
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_WillBeIgnordedRemove));
                                    }
                                    if (close_menu)
                                        MainForm->animation->game->ClosePlayerMenu();
                                    click_used = true;
                                }
                            }
                            else
                            {
                                MainForm->animation->game->ClosePlayerMenu();
                                click_used = true;
                            }
                        }
                        // Then, in order: sitting, standing up, a chair, a spell's target, an item to pick up, an npc,
                        // a locker or chest, a sign, a board, a jukebox and a door; a click on none of them walks to the tile.
                        if (!click_used &&
                            MainForm->animation->game->SitOnChair(MainForm->map->GetCenterX(),
                                MainForm->map->GetCursorY()))
                            click_used = true;
                        if (!click_used && MainForm->animation->game->StandUp())
                            click_used = true;
                        if (!click_used)
                        {
                            int chair_spec =
                                MainForm->map->GetChairSpec(MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                            // A chair is reached from the tile on the side it faces.
                            if (chair_spec >= TileSpec_ChairYHigh && chair_spec <= TileSpec_ChairAnySide)
                            {
                                char chair_walk_started = false;
                                if (chair_spec == TileSpec_ChairYHigh || chair_spec == TileSpec_ChairYHighXHigh ||
                                    chair_spec == TileSpec_ChairAnySide)
                                    chair_walk_started =
                                        MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX(),
                                            MainForm->map->GetCursorY() + 1);
                                if (chair_spec == TileSpec_ChairXLow)
                                    chair_walk_started =
                                        MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX() - 1,
                                            MainForm->map->GetCursorY());
                                if (chair_spec == TileSpec_ChairXHigh)
                                    chair_walk_started =
                                        MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX() + 1,
                                            MainForm->map->GetCursorY());
                                if (chair_spec == TileSpec_ChairYLow || chair_spec == TileSpec_ChairXLowYLow)
                                    chair_walk_started =
                                        MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX(),
                                            MainForm->map->GetCursorY() - 1);
                                if (chair_walk_started)
                                    MainForm->animation->interface_controller->spells->CancelCast();
                                click_used = true;
                            }
                        }
                        if (!click_used)
                        {
                            char click_pending = true;
                            char cast_cleared = false;
                            if (MainForm->animation->interface_controller->spells->IsCasting())
                            {
                                MainForm->animation->interface_controller->spells->ClearSelection();
                                cast_cleared = true;
                            }
                            if (MainForm->animation->interface_controller->spells->awaiting_target && !cast_cleared)
                            {
                                char player_targeted = false;
                                int target_player_id = 0;
                                char npc_targeted = false;
                                int target_npc_index = 0;
                                int target_npc_id = 0;
                                for (int npc_slot = MainForm->npcs->GetCount() - 1; npc_slot >= 0; npc_slot--)
                                {
                                    Npc target_npc = MainForm->npcs->GetNpc(npc_slot);
                                    NpcValue *target_npc_value = MainForm->npc_values->GetNpc(target_npc.npc_id - 1);
                                    if (target_npc.frame_width > 0 && target_npc.frame_height > 0)
                                    {
                                        npc_height = target_npc.frame_height;
                                        if (npc_height > 70)
                                            npc_height = 70;
                                        int target_x =
                                            TILE_HALF_WIDTH * target_npc.x - TILE_HALF_WIDTH * target_npc.y +
                                                MainForm->map->GetScrollOffsetX() + 319;
                                        target_y =
                                            16 * target_npc.x + 16 * target_npc.y + MainForm->map->GetScrollOffsetY() +
                                                160;
                                        if (target_x - target_npc.frame_width / 2 <= click_x &&
                                            target_npc.frame_width / 2 + target_x >= click_x &&
                                            target_y - npc_height <= click_y && target_y + 5 >= click_y)
                                        {
                                            if (target_npc_value->npc_type == NpcType_Passive ||
                                                target_npc_value->npc_type == NpcType_Aggressive)
                                            {
                                                npc_targeted = true;
                                                target_npc_index = target_npc.index;
                                                target_npc_id = target_npc.npc_id;
                                            }
                                            else
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_YouCannotAttackThis));
                                            break;
                                        }
                                    }
                                }
                                for (int player_slot = MainForm->players->GetCount() - 1; player_slot >= 0;
                                    player_slot--)
                                {
                                    Player target_player = MainForm->players->GetPlayer(player_slot);
                                    int player_screen_x =
                                        TILE_HALF_WIDTH * target_player.x - TILE_HALF_WIDTH * target_player.y +
                                            MainForm->map->GetScrollOffsetX() + 310;
                                    player_screen_y =
                                        TILE_HALF_HEIGHT * target_player.x + TILE_HALF_HEIGHT * target_player.y +
                                            MainForm->map->GetScrollOffsetY() + 102;
                                    if (player_screen_x - 1 <= click_x && player_screen_x + 19 >= click_x &&
                                        player_screen_y <= click_y && player_screen_y + 58 >= click_y)
                                    {
                                        player_targeted = true;
                                        target_player_id = target_player.player_id;
                                        break;
                                    }
                                }
                                char spell_cast = false;
                                if (!spell_cast && npc_targeted &&
                                    (!MainForm->animation->interface_controller->spells->selected_target_kind ||
                                        MainForm->animation->interface_controller->spells->selected_target_kind == 2))
                                {
                                    char popup_shown = false;
                                    if (MainForm->animation->interface_controller->spells->selected_spell_type == 3)
                                    {
                                        NpcValue *spell_npc_value = MainForm->npc_values->GetNpc(target_npc_id - 1);
                                        MainForm->animation->popup->Show(230, 244, spell_npc_value->element_weakness,
                                            spell_npc_value->name);
                                        popup_shown = true;
                                    }
                                    int caster_id = MainForm->net_client->GetPlayerId();
                                    SkillValue *npc_spell_shout =
                                        MainForm->skill_values->GetSkill(MainForm->animation->interface_controller->spells->selected_spell_id -
                                            1);
                                    MainForm->players->ShowTextLabel(caster_id, npc_spell_shout->shout,
                                        LabelColor_Silver, true, 4 * npc_spell_shout->cast_time);
                                    MainForm->net_client->sender->SendSpellChant(MainForm->animation->interface_controller->spells->selected_spell_id);
                                    MainForm->animation->interface_controller->spells->BeginCast(MainForm->animation->interface_controller->spells->selected_spell_id,
                                        CastTarget_Npc, target_npc_index, popup_shown);
                                    spell_cast = true;
                                }
                                if (!spell_cast && player_targeted &&
                                    (MainForm->animation->interface_controller->spells->selected_target_kind == 1 ||
                                        MainForm->animation->interface_controller->spells->selected_target_kind == 2))
                                {
                                    if (MainForm->map->GetMapType() != PVP_MAP_TYPE &&
                                        MainForm->animation->interface_controller->spells->selected_spell_type == 1)
                                    {
                                        MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_MapNotice, CHAT_COLOR_DEFAULT,
                                            MainForm->strings->GetText(GameText_YouCannotAttackOther), false);
                                        spell_cast = true;
                                    }
                                    else
                                    {
                                        int self_id = MainForm->net_client->GetPlayerId();
                                        SkillValue *player_spell =
                                            MainForm->skill_values->GetSkill(MainForm->animation->interface_controller->spells->selected_spell_id -
                                                1);
                                        MainForm->players->ShowTextLabel(self_id, player_spell->shout,
                                            LabelColor_Silver, true, 4 * player_spell->cast_time);
                                        MainForm->net_client->sender->SendSpellChant(MainForm->animation->interface_controller->spells->selected_spell_id);
                                        MainForm->animation->interface_controller->spells->BeginCast(MainForm->animation->interface_controller->spells->selected_spell_id,
                                            CastTarget_Player, target_player_id, false);
                                        spell_cast = true;
                                    }
                                }
                                MainForm->animation->interface_controller->spells->ClearSelection();
                                click_pending = false;
                            }
                            if (click_pending &&
                                MainForm->net_client->IsWithinReach(MainForm->map->GetCenterX(),
                                    MainForm->map->GetCenterY(), MainForm->map->GetCursorX(),
                                    MainForm->map->GetCursorY()))
                            {
                                int item_uid =
                                    MainForm->map_items->FindItemAt(MainForm->map->GetCursorX(),
                                        MainForm->map->GetCursorY());
                                if (item_uid >= 0)
                                {
                                    if (MainForm->animation->interface_controller->inventory->HasRoomForItem(MainForm->map_items->GetItemId(item_uid)))
                                    {
                                        if (MainForm->animation->interface_controller->status->weight +
                                            MainForm->item_values->GetWeight(MainForm->map_items->GetItemId(item_uid)) <=
                                            MainForm->animation->interface_controller->status->max_weight)
                                        {
                                            int owner_id = MainForm->map_items->GetProtectionOwner(item_uid);
                                            if (owner_id <= PlayerId_Unset ||
                                                MainForm->net_client->GetPlayerId() == owner_id)
                                                MainForm->net_client->sender->SendPickupItem(item_uid);
                                            else
                                            {
                                                String owner_name = MainForm->players->GetName(owner_id);
                                                if (owner_name.Length() > 0)
                                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                        MainForm->strings->GetText(GameText_ThisItemIsProtectedBy) +
                                                            " " + owner_name);
                                                else
                                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                        MainForm->strings->GetText(GameText_ThisItemIsProtected));
                                            }
                                        }
                                        else
                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                MainForm->strings->GetText(GameText_ItsTooHeavyYou));
                                        click_pending = false;
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                            MainForm->strings->GetText(GameText_YouCouldNotPick));
                                }
                            }
                            if (click_pending)
                            {
                                npc_clicked = false;
                                if (!npc_clicked)
                                {
                                    char found_npc = false;
                                    for (int npc_index = MainForm->npcs->GetCount() - 1;
                                        npc_index >= 0 && found_npc == 0; npc_index--)
                                    {
                                        Npc clicked_npc = MainForm->npcs->GetNpc(npc_index);
                                        if (clicked_npc.frame_width > 0 && clicked_npc.frame_height > 0)
                                        {
                                            near_height = clicked_npc.frame_height;
                                            if (near_height > 70)
                                                near_height = 70;
                                            int npc_x =
                                                TILE_HALF_WIDTH * clicked_npc.x - TILE_HALF_WIDTH * clicked_npc.y +
                                                    MainForm->map->GetScrollOffsetX() + 319;
                                            npc_y =
                                                16 * clicked_npc.x + 16 * clicked_npc.y +
                                                    MainForm->map->GetScrollOffsetY() + 160;
                                            if (npc_x - clicked_npc.frame_width / 2 <= click_x)
                                            {
                                                if (clicked_npc.frame_width / 2 + npc_x >= click_x &&
                                                    npc_y - near_height <= click_y && npc_y + 5 >= click_y)
                                                {
                                                    found_npc = true;
                                                    npc_clicked = true;
                                                    NpcValue *clicked_value =
                                                        MainForm->npc_values->GetNpc(clicked_npc.npc_id - 1);
                                                    if (clicked_value->npc_type == NpcType_Shop)
                                                        MainForm->net_client->sender->RequestShopOpen(clicked_npc.index);
                                                    if (clicked_value->npc_type == NpcType_Inn)
                                                    {
                                                        MainForm->animation->inn->npc_name = clicked_npc.name;
                                                        MainForm->net_client->sender->RequestInnOpen(clicked_npc.index);
                                                    }
                                                    if (clicked_value->npc_type == NpcType_Bank)
                                                        MainForm->net_client->sender->RequestBankOpen(clicked_npc.index);
                                                    if (clicked_value->npc_type == NpcType_Barber)
                                                        MainForm->net_client->sender->RequestBarberOpen(clicked_npc.index);
                                                    if (clicked_value->npc_type == NpcType_Guild)
                                                        MainForm->net_client->sender->RequestGuildOpen(clicked_npc.index);
                                                    if (clicked_value->npc_type == NpcType_Priest)
                                                        MainForm->net_client->sender->RequestPriestOpen(clicked_npc.index);
                                                    if (clicked_value->npc_type == NpcType_Law)
                                                    {
                                                        MainForm->animation->law->npc_name = clicked_npc.name;
                                                        MainForm->net_client->sender->RequestLawOpen(clicked_npc.index);
                                                    }
                                                    if (clicked_value->npc_type == NpcType_Trainer)
                                                        MainForm->net_client->sender->RequestSkillMasterOpen(clicked_npc.index);
                                                    if (clicked_value->npc_type == NpcType_Quest &&
                                                        MainForm->animation->dialog->request_cooldown <= 0)
                                                    {
                                                        MainForm->animation->dialog->npc_name = clicked_npc.name;
                                                        MainForm->animation->dialog->npc_index = clicked_npc.index;
                                                        MainForm->animation->dialog->request_cooldown =
                                                            QUEST_REQUEST_COOLDOWN_TICKS;
                                                        MainForm->net_client->sender->RequestQuestDialog(clicked_npc.index,
                                                            QUEST_DEFAULT_DIALOG);
                                                    }
                                                    if (clicked_value->npc_type == NpcType_Passive ||
                                                        clicked_value->npc_type == NpcType_Aggressive ||
                                                        clicked_value->npc_type == NpcType_Monster4 ||
                                                        clicked_value->npc_type == 5)
                                                    {
                                                        int self_x =
                                                            MainForm->players->GetX(MainForm->net_client->GetPlayerId());
                                                        int self_y =
                                                            MainForm->players->GetY(MainForm->net_client->GetPlayerId());
                                                        unsigned char attack_direction =
                                                            MainForm->net_client->GetAdjacentDirection(clicked_npc.x,
                                                                clicked_npc.y, self_x, self_y);
                                                        if (attack_direction)
                                                        {
                                                            if (MainForm->net_client->CanAct())
                                                            {
                                                                MainForm->map->path_finder->CancelPath();
                                                                MainForm->animation->game->Attack(false,
                                                                    attack_direction - 1);
                                                            }
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Npc,
                                                                clicked_value->name);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                // A locker or chest opens only from a tile beside it, once per short delay;
                                // a locked chest needs its key.
                                if (!npc_clicked && !MainForm->animation->IsObjectClickDelayed())
                                {
                                    MainForm->animation->StartObjectClickDelay();
                                    int player_x_tile = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
                                    int player_y_tile = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
                                    TPoint clicked_locker = MainForm->map->FindLockerAt(click_x, click_y);
                                    if (clicked_locker.x >= 0 && clicked_locker.y >= 0 &&
                                        MainForm->net_client->GetAdjacentDirection(clicked_locker.x, clicked_locker.y,
                                            player_x_tile, player_y_tile))
                                    {
                                        npc_clicked = true;
                                        MainForm->net_client->sender->RequestLockerOpen(clicked_locker);
                                    }
                                    TPoint clicked_chest = MainForm->map->FindChestAt(click_x, click_y);
                                    if (clicked_chest.x >= 0 && clicked_chest.y >= 0 &&
                                        MainForm->net_client->GetAdjacentDirection(clicked_chest.x, clicked_chest.y,
                                            player_x_tile, player_y_tile))
                                    {
                                        npc_clicked = true;
                                        int chest_key = MainForm->map->GetChestKey(clicked_chest.x, clicked_chest.y);
                                        if (chest_key > 0)
                                        {
                                            if (MainForm->animation->interface_controller->inventory->HasKey(chest_key))
                                                MainForm->net_client->sender->RequestChestOpen(clicked_chest);
                                            else
                                            {
                                                String chest_key_name = MainForm->item_values->GetKeyName(chest_key);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_ChestIsLocked2) + " - " +
                                                        chest_key_name);
                                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                    ConfirmAction_None,
                                                    MainForm->strings->GetDialogText(DialogText_ChestLocked, false),
                                                    MainForm->strings->GetDialogText(DialogText_ChestLocked, true));
                                                MainForm->sfx->PlaySfx(SfxSound_Locked);
                                            }
                                        }
                                        else
                                            MainForm->net_client->sender->RequestChestOpen(clicked_chest);
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    String sign_data = MainForm->map->FindSignTextAt(click_x, click_y);
                                    if (sign_data.Length() > 1)
                                    {
                                        npc_clicked = true;
                                        if (!MainForm->animation->game->message_visible)
                                        {
                                            int title_length =
                                                MainForm->net_client->DecodeNumber(sign_data.SubString(1, 1));
                                            sign_data.Delete(1, 1);
                                            String sign_title = sign_data.SubString(1, title_length);
                                            sign_data.Delete(1, title_length);
                                            if (!sign_title.Length())
                                                sign_title = "Sign";
                                            String sign_message = sign_data;
                                            MainForm->sfx->PlaySfx(SfxSound_Select);
                                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                ConfirmAction_None, sign_title, sign_message);
                                            MainForm->edits->caret_enabled = false;
                                        }
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    TPoint board_tile = MainForm->map->FindBoardAt(click_x, click_y);
                                    if (board_tile.x > 0 && board_tile.y > 0)
                                    {
                                        npc_clicked = true;
                                        int board_id =
                                            MainForm->map->GetUsableTileSpec(board_tile.x, board_tile.y) - 19;
                                        if (board_id >= 0 && board_id <= 7)
                                            MainForm->net_client->sender->RequestBoardOpen(board_id);
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    TPoint jukebox_tile = MainForm->map->FindJukeboxAt(click_x, click_y);
                                    if (jukebox_tile.x > 0 && jukebox_tile.y > 0)
                                    {
                                        npc_clicked = true;
                                        MainForm->net_client->sender->RequestJukeboxOpen(jukebox_tile);
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    TPoint door_tile = MainForm->map->FindDoorAt(click_x, click_y);
                                    if (door_tile.x > 0 && door_tile.y > 0)
                                    {
                                        npc_clicked = true;
                                        int door_key = MainForm->map->GetDoorKey(door_tile.x, door_tile.y);
                                        if (door_key > 0)
                                        {
                                            if (MainForm->animation->interface_controller->inventory->HasKey(door_key))
                                                MainForm->net_client->sender->SendOpenDoor(door_tile.x, door_tile.y);
                                            else
                                            {
                                                String door_key_name = MainForm->item_values->GetKeyName(door_key);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_DoorIsLocked2) + " - " +
                                                        door_key_name);
                                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok,
                                                    ConfirmAction_None,
                                                    MainForm->strings->GetDialogText(DialogText_DoorLocked, false),
                                                    MainForm->strings->GetDialogText(DialogText_DoorLocked, true));
                                                MainForm->sfx->PlaySfx(SfxSound_Locked);
                                            }
                                        }
                                        else
                                            MainForm->net_client->sender->SendOpenDoor(door_tile.x, door_tile.y);
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    char walk_started =
                                        MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX(),
                                            MainForm->map->GetCursorY());
                                    if (walk_started)
                                        MainForm->animation->interface_controller->spells->CancelCast();
                                }
                            }
                        }
                    }
                    // A right click on another player opens the player menu.
                    // On the player's own figure it asks for the player's own paperdoll.
                    if (button == 1)
                    {
                        char menu_handled = false;
                        if (MainForm->animation->game->player_menu_visible)
                        {
                            MainForm->animation->game->ClosePlayerMenu();
                            menu_handled = true;
                        }
                        if (!menu_handled)
                        {
                            for (int menu_slot = MainForm->players->GetCount() - 1; menu_slot >= 0; menu_slot--)
                            {
                                Player menu_player = MainForm->players->GetPlayer(menu_slot);
                                int menu_x =
                                    TILE_HALF_WIDTH * menu_player.x - TILE_HALF_WIDTH * menu_player.y +
                                        MainForm->map->GetScrollOffsetX() + 310;
                                int menu_y =
                                    TILE_HALF_HEIGHT * menu_player.x + TILE_HALF_HEIGHT * menu_player.y +
                                        MainForm->map->GetScrollOffsetY() + 102;
                                if (menu_x - 1 <= click_x && menu_x + 19 >= click_x && menu_y <= click_y &&
                                    menu_y + 58 >= click_y && menu_player.hover_label_shown)
                                {
                                    if (MainForm->net_client->GetPlayerId() != menu_player.player_id)
                                    {
                                        MainForm->animation->game->OpenPlayerMenu(menu_x, menu_y, menu_player.player_id,
                                            menu_player.name);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            MainForm->strings->GetText(GameText_MenuBelongsToPlayer) + " " +
                                                MainForm->net_client->Capitalize(MainForm->animation->game->player_menu_name));
                                        menu_handled = true;
                                        break;
                                    }
                                    MainForm->animation->hotkeys->Close();
                                    MainForm->net_client->sender->RequestPaperdoll(MainForm->net_client->GetPlayerId());
                                }
                            }
                        }
                    }
                }
            }
        }
        // Asking for the player list holds every panel click until the list comes or a short wait ends.
        if (MainForm->animation->interface_controller->player_list_button_ready)
        {
            // The open panel takes the click, then the buttons beside it.
            if (MainForm->animation->GetSubState() == GamePanel_Status)
                MainForm->animation->interface_controller->status->MouseDown(click_x, click_y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Chat)
                MainForm->animation->interface_controller->chat->OnMouseClick(click_x, click_y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Settings)
                MainForm->animation->interface_controller->settings->MouseDown(click_x, click_y);
            if (MainForm->animation->GetSubState() == GamePanel_News)
                MainForm->animation->interface_controller->news->MouseDown(click_x, click_y);
            if (MainForm->animation->GetSubState() == GamePanel_Help)
                MainForm->animation->interface_controller->help->MouseDown(click_x, click_y);
            if (MainForm->animation->GetSubState() == GamePanel_Inventory)
                MainForm->animation->interface_controller->inventory->MouseDown(click_x, click_y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Spells)
                MainForm->animation->interface_controller->spells->MouseDown(click_x, click_y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Skills)
                MainForm->animation->interface_controller->skills->MouseDown(click_x, click_y, button);
            // The left column: the minimap, inventory, spell, skill, chat and status buttons; none works
            // while an icon is being dragged.
            if (click_x >= 62 && click_x <= 98 && click_y >= 330 && click_y <= 348 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                ToggleMinimap(true);
            if (click_x >= 62 && click_x <= 98 && click_y >= 350 && click_y <= 368 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenInventoryPanel(true);
            if (click_x >= 62 && click_x <= 98 && click_y >= 370 && click_y <= 388 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenSpellPanel(true);
            if (click_x >= 62 && click_x <= 98 && click_y >= 390 && click_y <= 408 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenSkillPanel(true);
            if (click_x >= 62 && click_x <= 98 && click_y >= 410 && click_y <= 428 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenChatPanel(true);
            if (click_x >= 62 && click_x <= 98 && click_y >= 430 && click_y <= 448 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenStatusPanel(true);
            // The paperdoll button asks for the player's own paperdoll;
            // with the setting on, the inventory opens too.
            if (click_x >= 590 && click_x <= 633 && click_y >= 350 && click_y <= 368 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                if (!MainForm->animation->paperdoll->is_visible)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->hotkeys->Close();
                    MainForm->net_client->sender->RequestPaperdoll(MainForm->net_client->GetPlayerId());
                    if (MainForm->config->GetAutoOpenInventory())
                        OpenInventoryPanel(false);
                }
                else
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->paperdoll->Close();
                }
            }
            if (click_x >= 590 && click_x <= 633 && click_y >= 370 && click_y <= 388 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                if (!MainForm->animation->hotkeys->is_visible)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->paperdoll->Close();
                    MainForm->animation->hotkeys->Open();
                }
                else
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->hotkeys->Close();
                }
            }
            if (click_x >= 590 && click_x <= 633 && click_y >= 390 && click_y <= 408 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenPlayerlistPanel(true);
            if (MainForm->animation->GetSubState() == GamePanel_Playerlist)
                MainForm->animation->interface_controller->player_list->MouseDown(click_x, click_y, button);
            if (click_x >= 590 && click_x <= 633 && click_y >= 410 && click_y <= 428 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenGroupPanel(true);
            if (MainForm->animation->GetSubState() == GamePanel_Group)
                MainForm->animation->interface_controller->group->MouseDown(click_x, click_y);
            if (click_x >= 590 && click_x <= 633 && click_y >= 430 && click_y <= 448 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
                OpenSettingsPanel(true);
            // The friend list; the button beside it opens the ignore list.
            if (click_x >= 598 && click_x <= 615 && click_y >= 311 && click_y <= 330 &&
                !MainForm->animation->social->is_open)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_FriendList) + " " +
                        MainForm->strings->GetText(GameText_UseRightMouseClick));
                MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                MainForm->animation->social->ClearOnlineNames();
                MainForm->animation->social->OpenWindow();
                String social_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                social_title = MainForm->net_client->Capitalize(social_title);
                social_title = social_title + "'s " + MainForm->strings->GetText(GameText_FriendList);
                MainForm->animation->social->title = social_title;
                MainForm->animation->social->list_kind = 0;
                MainForm->net_client->sender->RequestOnlineNames();
            }
            if (click_x >= 616 && click_x <= 633 && click_y >= 311 && click_y <= 330 &&
                !MainForm->animation->social->is_open)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_IgnoreList) + " " +
                        MainForm->strings->GetText(GameText_UseRightMouseClick));
                MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                MainForm->animation->social->ClearOnlineNames();
                MainForm->animation->social->OpenWindow();
                String other_social_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                other_social_title = MainForm->net_client->Capitalize(other_social_title);
                other_social_title = other_social_title + "'s " + MainForm->strings->GetText(GameText_IgnoreList);
                MainForm->animation->social->title = other_social_title;
                MainForm->animation->social->list_kind = 1;
            }
        }
    }
    int preview_direction;
    unsigned char hair_cell_width;
    unsigned char hair_cell_height;
    int skin_left;
    int skin_top;
    int skin_width;
    int skin_gap;
    int last_hair_page;
    int color_left;
    int color_top;
    int color_width;
    int color_gap;
    int ok_x;
    int cancel_x;
    int button_y;
    char password_error;
    int change_problem;
    unsigned char first_facing;
    unsigned char second_facing;
    unsigned char third_facing;
    // On the menu screens an open message, text window, form or world load takes the click first.
    if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
    {
        if (MainForm->animation->menu->is_message_open || MainForm->animation->menu->is_text_window_open ||
            MainForm->animation->menu->is_form_open || MainForm->animation->menu->is_loading_world)
        {
            MainForm->animation->menu->OnMouseDown(click_x, click_y);
            if (MainForm->animation->menu->is_text_window_open && x >= MainForm->animation->menu->text_window_x + 137 &&
                x <= MainForm->animation->menu->text_window_x + 225 &&
                y >= MainForm->animation->menu->text_window_y + 196 &&
                y <= MainForm->animation->menu->text_window_y + 224)
            {
                MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                MainForm->animation->menu->is_text_window_open = false;
                MainForm->edits->caret_enabled = true;
            }
            if (MainForm->animation->menu->is_message_open)
            {
                if (x >= MainForm->animation->menu->message_x + 182 &&
                    x <= MainForm->animation->menu->message_x + 270 &&
                    y >= MainForm->animation->menu->message_y + 114 && y <= MainForm->animation->menu->message_y + 140)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->menu->is_message_open = false;
                    MainForm->edits->caret_enabled = true;
                    if (MainForm->animation->menu->message_kind == MessageBoxKind_RecoverNotice)
                    {
                        if (MainForm->animation->GetSubState() == MenuScreen_PasswordReset)
                        {
                            MainForm->animation->StopAccountProgress();
                            MainForm->edits->Clear();
                            MainForm->gfx->LoadMainMenuScreen(true);
                            MainForm->animation->SetSubState(MenuScreen_Main);
                        }
                        MainForm->edits->caret_enabled = true;
                    }
                    if (MainForm->animation->menu->message_kind == MessageBoxKind_RecoverAccepted)
                    {
                        MainForm->animation->menu->recover_step = 1;
                        MainForm->edits->Clear();
                        MainForm->gfx->LoadPasswordResetScreen(0, false);
                    }
                }
                if (x >= MainForm->animation->menu->message_x + 89 && x <= MainForm->animation->menu->message_x + 177 &&
                    y >= MainForm->animation->menu->message_y + 114 &&
                    y <= MainForm->animation->menu->message_y + 140 &&
                    MainForm->animation->menu->message_kind == MessageBoxKind_Confirm)
                {
                    MainForm->net_client->sender->ConfirmCharacterDelete();
                    MainForm->animation->menu->is_message_open = false;
                    MainForm->edits->caret_enabled = true;
                }
            }
            if (MainForm->animation->menu->is_form_open && !MainForm->animation->menu->is_message_open)
            {
                if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                {
                    if (x >= MainForm->animation->menu->form_x + 430 && x <= MainForm->animation->menu->form_x + 445 &&
                        y >= MainForm->animation->menu->form_y + 44 && y <= MainForm->animation->menu->form_y + 66)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ArrowButton);
                        if (MainForm->animation->menu->new_hair_style)
                        {
                            MainForm->animation->menu->new_hair_style = 0;
                            MainForm->players->SetPreviewHairStyle(MainForm->animation->menu->new_hair_style);
                            MainForm->players->ReloadGraphics(PlayerId_Preview);
                        }
                    }
                    else if (x >= MainForm->animation->menu->form_x + 335 &&
                        x <= MainForm->animation->menu->form_x + 448 && y >= MainForm->animation->menu->form_y + 38 &&
                        y <= MainForm->animation->menu->form_y + 202)
                    {
                        preview_direction = MainForm->players->GetDirection(PlayerId_Preview);
                        if (MainForm->players->GetDirection(PlayerId_Preview) < 3)
                            MainForm->players->SetDirection(PlayerId_Preview, preview_direction + 1);
                        else
                            MainForm->players->SetDirection(PlayerId_Preview, FacingDirection_Down);
                        MainForm->sfx->PlaySfx(SfxSound_Select);
                    }
                    if (x >= MainForm->animation->menu->form_x + 30 && x <= MainForm->animation->menu->form_x + 62 &&
                        y >= MainForm->animation->menu->form_y + 28 && y <= MainForm->animation->menu->form_y + 60)
                    {
                        if (MainForm->animation->menu->new_gender < 1)
                            MainForm->animation->menu->new_gender++;
                        else
                            MainForm->animation->menu->new_gender = 0;
                        MainForm->players->SetPreviewGender(MainForm->animation->menu->new_gender);
                        MainForm->players->ReloadGraphics(PlayerId_Preview);
                        MainForm->sfx->PlaySfx(SfxSound_Select);
                    }
                    hair_cell_width = 31;
                    hair_cell_height = 33;
                    for (int hair_slot = 0; hair_slot < 8; hair_slot++)
                    {
                        if (MainForm->animation->menu->form_x + hair_cell_width * hair_slot + 45 <= x &&
                            MainForm->animation->menu->form_x + hair_cell_width * hair_slot + hair_cell_width + 45 >=
                                x && y >= MainForm->animation->menu->form_y + 127 &&
                            MainForm->animation->menu->form_y + hair_cell_height + 127 >= y)
                        {
                            hair_index = 8 * MainForm->animation->menu->hair_page + hair_slot;
                            if (hair_index >= 0 && hair_index < MainForm->animation->menu->hair_style_count)
                            {
                                MainForm->animation->menu->new_hair_style = hair_index + 1;
                                MainForm->players->SetPreviewHairStyle(MainForm->animation->menu->new_hair_style);
                                MainForm->players->ReloadGraphics(PlayerId_Preview);
                                MainForm->sfx->PlaySfx(SfxSound_Select);
                            }
                        }
                    }
                    skin_left = 37;
                    skin_top = 88;
                    skin_width = 21;
                    skin_gap = 1;
                    for (int skin_slot = 0; skin_slot < 12; skin_slot++)
                    {
                        skin_x = skin_slot * skin_width + skin_left + skin_slot * skin_gap;
                        if (x >= MainForm->animation->menu->form_x + skin_x &&
                            skin_x + skin_width + MainForm->animation->menu->form_x >= x &&
                            y >= MainForm->animation->menu->form_y + skin_top - 2 &&
                            y <= MainForm->animation->menu->form_y + skin_top + 21)
                        {
                            MainForm->animation->menu->new_skin = skin_slot;
                            MainForm->players->SetPreviewSkin(MainForm->animation->menu->new_skin);
                            MainForm->players->ReloadGraphics(PlayerId_Preview);
                            MainForm->sfx->PlaySfx(SfxSound_Select);
                        }
                    }
                    if (x >= MainForm->animation->menu->form_x + 25 && x <= MainForm->animation->menu->form_x + 44 &&
                        y >= MainForm->animation->menu->form_y + 125 && y <= MainForm->animation->menu->form_y + 161 &&
                        MainForm->animation->menu->hair_page > 0)
                    {
                        MainForm->animation->menu->hair_page--;
                        MainForm->sfx->PlaySfx(SfxSound_Select);
                    }
                    if (x >= MainForm->animation->menu->form_x + 292 && x <= MainForm->animation->menu->form_x + 313 &&
                        y >= MainForm->animation->menu->form_y + 125 && y <= MainForm->animation->menu->form_y + 161)
                    {
                        last_hair_page = (MainForm->animation->menu->hair_style_count - 1) / 8;
                        if (MainForm->animation->menu->hair_page < last_hair_page)
                        {
                            MainForm->animation->menu->hair_page++;
                            MainForm->sfx->PlaySfx(SfxSound_Select);
                        }
                    }
                    color_left = 56;
                    color_top = 166;
                    color_width = 21;
                    color_gap = 1;
                    for (int color_slot = 0; color_slot < 10; color_slot++)
                    {
                        color_x = color_slot * color_width + color_left + color_slot * color_gap;
                        if (x >= MainForm->animation->menu->form_x + color_x &&
                            color_x + color_width + MainForm->animation->menu->form_x >= x &&
                            y >= MainForm->animation->menu->form_y + color_top - 2 &&
                            y <= MainForm->animation->menu->form_y + color_top + 21)
                        {
                            MainForm->animation->menu->new_hair_color = color_slot;
                            MainForm->players->SetPreviewHairColor(MainForm->animation->menu->new_hair_color);
                            MainForm->players->ReloadGraphics(PlayerId_Preview);
                            MainForm->sfx->PlaySfx(SfxSound_Select);
                        }
                    }
                }
                ok_x = 218;
                cancel_x = 311;
                button_y = 216;
                if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                {
                    ok_x = 79;
                    cancel_x = 172;
                }
                if (x >= ok_x + MainForm->animation->menu->form_x &&
                    x <= MainForm->animation->menu->form_x + ok_x + 88 &&
                    y >= button_y + MainForm->animation->menu->form_y &&
                    y <= MainForm->animation->menu->form_y + button_y + 26)
                {
                    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG &&
                        MainForm->animation->menu->create_character_enabled)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        if (MainForm->edits->GetText(EditId_InputBox) != "")
                        {
                            if (MainForm->edits->GetText(EditId_InputBox).Length() < 4)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                    MainForm->strings->GetDialogText(DialogText_WrongName, true));
                                MainForm->edits->SetText(EditId_InputBox, "");
                                MainForm->edits->FocusEdit(EditId_InputBox);
                            }
                            else if (MainForm->animation->menu->new_skin >= MainForm->animation->menu->skin_count)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain, "Skin Lock",
                                    "The selected skin is not supported on this server, please pick another one.");
                            }
                            else
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->create_character_enabled = false;
                                MainForm->net_client->sender->SendCharacterCreate(MainForm->edits->GetText(EditId_InputBox),
                                    MainForm->animation->menu->new_gender, MainForm->animation->menu->new_hair_style,
                                    MainForm->animation->menu->new_hair_color, MainForm->animation->menu->new_skin);
                            }
                        }
                    }
                    if (MainForm->animation->menu->form_kind == CHANGE_PASSWORD_DIALOG &&
                        MainForm->animation->menu->change_password_enabled)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        if (MainForm->edits->GetText(EditId_AccountName) != "" &&
                            MainForm->edits->GetText(EditId_OldPassword) != "" &&
                            MainForm->edits->GetText(EditId_NewPassword) != "" &&
                            MainForm->edits->GetText(EditId_ConfirmNewPassword) != "")
                        {
                            password_error = false;
                            if (MainForm->edits->GetText(EditId_NewPassword) !=
                                MainForm->edits->GetText(EditId_ConfirmNewPassword))
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, false),
                                    MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, true));
                                MainForm->edits->SetText(EditId_NewPassword, "");
                                MainForm->edits->SetText(EditId_ConfirmNewPassword, "");
                                MainForm->edits->FocusEdit(EditId_NewPassword);
                                password_error = true;
                            }
                            change_problem = MainForm->edits->GetPasswordProblem(EditId_NewPassword);
                            if (change_problem == 4)
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(change_problem, false),
                                    MainForm->strings->GetDialogText(change_problem, true));
                            if (change_problem == 5)
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(change_problem, false),
                                    MainForm->strings->GetDialogText(change_problem, true));
                            if (change_problem == -1)
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                    "At least 1 uppercase character ( A,B etc..) is required in your new password");
                            if (change_problem == -2)
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                    "At least 1 1owercase character ( a,b etc..) is required in your new password");
                            if (change_problem == -3)
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                    "At least 1 number or token ( 1,2,#,$ etc..) is required in your new password");
                            if (change_problem)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->edits->SetText(EditId_NewPassword, "");
                                MainForm->edits->SetText(EditId_ConfirmNewPassword, "");
                                MainForm->edits->FocusEdit(EditId_NewPassword);
                                password_error = true;
                            }
                            if (!password_error)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->change_password_enabled = false;
                                MainForm->net_client->sender->SendChangePassword(MainForm->edits->GetText(EditId_AccountName),
                                    MainForm->edits->GetText(EditId_OldPassword),
                                        MainForm->edits->GetText(EditId_NewPassword));
                            }
                        }
                    }
                }
                if (x >= cancel_x + MainForm->animation->menu->form_x &&
                    x <= MainForm->animation->menu->form_x + cancel_x + 88 &&
                    y >= button_y + MainForm->animation->menu->form_y &&
                    y <= MainForm->animation->menu->form_y + button_y + 26)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->players->RemovePreviewPlayer();
                    MainForm->edits->Clear();
                }
            }
        }
        else
        {
            // The character list: the three slots are kept as the players 999991 to 999993.
            // A click on a slot's figure turns it; its log-in button picks that character.
            if (MainForm->animation->GetSubState() == MenuScreen_CharacterSelect)
            {
                if (((click_y > 10 && click_y < 17 && 3 * click_y + 560 > click_x) ||
                    (click_y > 16 && click_y < 31 && click_y + 594 > click_x)) && click_x >= 594 && click_x <= 624)
                {
                    if (MainForm->client->Active)
                        ((TAbstractSocket *)MainForm->client)->Close();
                    MainForm->animation->menu->is_loading_world = false;
                }
                if (click_x >= 345 && click_x <= 450 && click_y >= 45 && click_y <= 150)
                {
                    first_facing = MainForm->players->GetDirection(PlayerId_SlotFirst);
                    first_facing++;
                    if (first_facing > 3)
                        first_facing = 0;
                    MainForm->players->SetDirection(PlayerId_SlotFirst, first_facing);
                    MainForm->sfx->PlaySfx(SfxSound_Select);
                }
                if (click_x >= 485 && click_x <= 585 && click_y >= 93 && click_y <= 120 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotFirst))
                {
                    MainForm->registry->SetCharacterName(MainForm->players->GetName(PlayerId_SlotFirst));
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->animation->menu->SetMapNeeded(false);
                    MainForm->animation->menu->SetItemFileNeeded(false);
                    MainForm->animation->menu->SetClassFileNeeded(false);
                    MainForm->animation->menu->SetReservedFileNeeded(false);
                    MainForm->animation->menu->SetEnteringGame(false);
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotFirst));
                }
                if (click_x >= 345 && click_x <= 450 && click_y >= 170 && click_y <= 275)
                {
                    second_facing = MainForm->players->GetDirection(PlayerId_SlotSecond);
                    second_facing++;
                    if (second_facing > 3)
                        second_facing = 0;
                    MainForm->players->SetDirection(PlayerId_SlotSecond, second_facing);
                    MainForm->sfx->PlaySfx(SfxSound_Select);
                }
                if (click_x >= 485 && click_x <= 585 && click_y >= 217 && click_y <= 244 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotSecond))
                {
                    MainForm->registry->SetCharacterName(MainForm->players->GetName(PlayerId_SlotSecond));
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->animation->menu->SetMapNeeded(false);
                    MainForm->animation->menu->SetItemFileNeeded(false);
                    MainForm->animation->menu->SetClassFileNeeded(false);
                    MainForm->animation->menu->SetReservedFileNeeded(false);
                    MainForm->animation->menu->SetEnteringGame(false);
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotSecond));
                }
                if (click_x >= 345 && click_x <= 450 && click_y >= 294 && click_y <= 399)
                {
                    third_facing = MainForm->players->GetDirection(PlayerId_SlotThird);
                    third_facing++;
                    if (third_facing > 3)
                        third_facing = 0;
                    MainForm->players->SetDirection(PlayerId_SlotThird, third_facing);
                    MainForm->sfx->PlaySfx(SfxSound_Select);
                }
                if (click_x >= 485 && click_x <= 585 && click_y >= 341 && click_y <= 368 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotThird))
                {
                    MainForm->registry->SetCharacterName(MainForm->players->GetName(PlayerId_SlotThird));
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->animation->menu->SetMapNeeded(false);
                    MainForm->animation->menu->SetItemFileNeeded(false);
                    MainForm->animation->menu->SetClassFileNeeded(false);
                    MainForm->animation->menu->SetReservedFileNeeded(false);
                    MainForm->animation->menu->SetEnteringGame(false);
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotThird));
                }
                if (click_x >= 485 && click_x <= 585 && click_y >= 121 && click_y <= 148 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotFirst))
                {
                    if (MainForm->animation->menu->delete_armed_slot == 1)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        MainForm->animation->menu->is_form_open = false;
                        MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                        MainForm->net_client->sender->RequestCharacterDelete(MainForm->players->GetCharacterId(PlayerId_SlotFirst));
                    }
                    else
                    {
                        MainForm->animation->menu->delete_armed_slot = 1;
                        MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                            MainForm->strings->GetDialogText(DialogText_DeleteCharacterConfirm, false),
                            "Character '" + MainForm->players->GetName(PlayerId_SlotFirst) + "' " +
                                MainForm->strings->GetDialogText(DialogText_DeleteCharacterConfirm, true));
                    }
                }
                if (click_x >= 485 && click_x <= 585 && click_y >= 245 && click_y <= 272 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotSecond))
                {
                    if (MainForm->animation->menu->delete_armed_slot == 2)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        MainForm->animation->menu->is_form_open = false;
                        MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                        MainForm->net_client->sender->RequestCharacterDelete(MainForm->players->GetCharacterId(PlayerId_SlotSecond));
                    }
                    else
                    {
                        MainForm->animation->menu->delete_armed_slot = 2;
                        MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                            MainForm->strings->GetDialogText(DialogText_DeleteCharacterConfirm, false),
                            "Character '" + MainForm->players->GetName(PlayerId_SlotSecond) + "' " +
                                MainForm->strings->GetDialogText(DialogText_DeleteCharacterConfirm, true));
                    }
                }
                if (click_x >= 485 && click_x <= 585 && click_y >= 369 && click_y <= 396 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotThird))
                {
                    if (MainForm->animation->menu->delete_armed_slot == 3)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        MainForm->animation->menu->is_form_open = false;
                        MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                        MainForm->net_client->sender->RequestCharacterDelete(MainForm->players->GetCharacterId(PlayerId_SlotThird));
                    }
                    else
                    {
                        MainForm->animation->menu->delete_armed_slot = 3;
                        MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                            MainForm->strings->GetDialogText(DialogText_DeleteCharacterConfirm, false),
                            "Character '" + MainForm->players->GetName(PlayerId_SlotThird) + "' " +
                                MainForm->strings->GetDialogText(DialogText_DeleteCharacterConfirm, true));
                    }
                }
                if (click_x >= 452 && click_x <= 570 && click_y >= 416 && click_y <= 456)
                {
                    MainForm->gfx->LoadChangePasswordPanel();
                    MainForm->animation->menu->change_password_enabled = true;
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, MainForm->animation->menu->form_x +
                        200, MainForm->animation->menu->form_y + 39, 226, EDIT_LINE_HEIGHT);
                    MainForm->edits->AddEdit(EditId_OldPassword, EditGroup_Menu, MainForm->animation->menu->form_x +
                        200, MainForm->animation->menu->form_y + 76, 226, EDIT_LINE_HEIGHT);
                    MainForm->edits->AddEdit(EditId_NewPassword, EditGroup_Menu, MainForm->animation->menu->form_x +
                        200, MainForm->animation->menu->form_y + 128, 226, EDIT_LINE_HEIGHT);
                    MainForm->edits->AddEdit(EditId_ConfirmNewPassword, EditGroup_Menu,
                        MainForm->animation->menu->form_x + 200, MainForm->animation->menu->form_y + 166, 226,
                        EDIT_LINE_HEIGHT);
                    MainForm->edits->SetBackgroundVisible(EditId_AccountName, false);
                    MainForm->edits->SetBorderVisible(EditId_AccountName, false);
                    MainForm->edits->SetBackgroundVisible(EditId_OldPassword, false);
                    MainForm->edits->SetBorderVisible(EditId_OldPassword, false);
                    MainForm->edits->SetBackgroundVisible(EditId_NewPassword, false);
                    MainForm->edits->SetBorderVisible(EditId_NewPassword, false);
                    MainForm->edits->SetBackgroundVisible(EditId_ConfirmNewPassword, false);
                    MainForm->edits->SetBorderVisible(EditId_ConfirmNewPassword, false);
                    MainForm->edits->SetTextColor(EditId_AccountName, MainForm->themes->color_popup_edits);
                    MainForm->edits->SetTextColor(EditId_OldPassword, MainForm->themes->color_popup_edits);
                    MainForm->edits->SetTextColor(EditId_NewPassword, MainForm->themes->color_popup_edits);
                    MainForm->edits->SetTextColor(EditId_ConfirmNewPassword, MainForm->themes->color_popup_edits);
                    MainForm->edits->SetMaxLength(EditId_AccountName, 16);
                    MainForm->edits->SetMaxLength(EditId_OldPassword, 16);
                    MainForm->edits->SetMaxLength(EditId_NewPassword, 16);
                    MainForm->edits->SetMaxLength(EditId_ConfirmNewPassword, 16);
                    MainForm->edits->SetAllowSpaces(EditId_AccountName, true);
                    MainForm->edits->SetAllowSpaces(EditId_OldPassword, false);
                    MainForm->edits->SetAllowSpaces(EditId_NewPassword, false);
                    MainForm->edits->SetAllowSpaces(EditId_ConfirmNewPassword, false);
                    MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Alphanumeric);
                    MainForm->edits->SetCharFilter(EditId_OldPassword, EditCharFilter_Password);
                    MainForm->edits->SetCharFilter(EditId_NewPassword, EditCharFilter_Password);
                    MainForm->edits->SetCharFilter(EditId_ConfirmNewPassword, EditCharFilter_Password);
                    MainForm->edits->SetPasswordMode(EditId_NewPassword, true);
                    MainForm->edits->SetPasswordMode(EditId_ConfirmNewPassword, true);
                    MainForm->edits->SetShowStrength(EditId_NewPassword, true);
                    MainForm->edits->FocusEdit(EditId_AccountName);
                    MainForm->animation->menu->OpenFormWindow(CHANGE_PASSWORD_DIALOG);
                }
                if (click_x >= 332 && click_x <= 450 && click_y >= 416 && click_y <= 456 &&
                    !MainForm->animation->menu->character_select_cooldown)
                {
                    MainForm->edits->Clear();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->net_client->sender->RequestCharacterCreate();
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_CreateAccount)
            {
                if (((click_y > 10 && click_y < 17 && 3 * click_y + 560 > click_x) ||
                    (click_y > 16 && click_y < 31 && click_y + 594 > click_x)) && click_x >= 594 && click_x <= 624)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
                if (click_x >= 580 && click_x <= 600 && click_y >= 363 && click_y <= 383)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->FocusEdit(EditId_Email);
                    MainForm->edits->TypeText("@");
                }
                if (click_x >= 358 && click_x <= 476 && click_y >= 416 && click_y <= 455 &&
                    MainForm->animation->menu->create_account_enabled)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->account_name = MainForm->edits->GetText(EditId_AccountName);
                    MainForm->animation->account_password = MainForm->edits->GetText(EditId_AccountPassword);
                    MainForm->animation->account_detail_a = MainForm->edits->GetText(EditId_AccountDetailA);
                    MainForm->animation->account_detail_b = MainForm->edits->GetText(EditId_AccountDetailB);
                    MainForm->animation->account_email = MainForm->edits->GetText(EditId_Email);
                    if (MainForm->edits->GetText(EditId_AccountName) != "" &&
                        MainForm->edits->GetText(EditId_AccountPassword) != "" &&
                        MainForm->edits->GetText(EditId_ConfirmPassword) != "" &&
                        MainForm->edits->GetText(EditId_AccountDetailA) != "" &&
                        MainForm->edits->GetText(EditId_AccountDetailB) != "" &&
                        MainForm->edits->GetText(EditId_Email) != "")
                    {
                        char account_error = false;
                        String new_account = MainForm->edits->GetText(EditId_AccountName);
                        if (new_account.Length() < 4)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongAccount, false),
                                MainForm->strings->GetDialogText(DialogText_WrongAccount, true));
                            MainForm->edits->SetText(EditId_AccountName, "");
                            MainForm->edits->FocusEdit(EditId_AccountName);
                            account_error = true;
                        }
                        else
                        {
                            if (new_account[new_account.Length()] == ' ')
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_WrongAccount, false),
                                    "Please remove the last spacebar token from your accountname and try again.");
                                MainForm->edits->FocusEdit(EditId_AccountName);
                                account_error = true;
                            }
                            if (new_account[1] == ' ')
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_WrongAccount, false),
                                    "Please remove the first spacebar token from your accountname and try again.");
                                MainForm->edits->FocusEdit(EditId_AccountName);
                                account_error = true;
                            }
                        }
                        if (MainForm->edits->HasFewDistinctChars(MainForm->edits->GetText(EditId_AccountName)) &&
                            !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_AccountToObvious, false),
                                MainForm->strings->GetDialogText(DialogText_AccountToObvious, true));
                            MainForm->edits->SetText(EditId_AccountName, "");
                            MainForm->edits->FocusEdit(EditId_AccountName);
                            account_error = true;
                        }
                        if (MainForm->edits->GetText(EditId_AccountPassword) !=
                            MainForm->edits->GetText(EditId_ConfirmPassword) && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, false),
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, true));
                            MainForm->edits->SetText(EditId_AccountPassword, "");
                            MainForm->edits->SetText(EditId_ConfirmPassword, "");
                            MainForm->edits->FocusEdit(EditId_AccountPassword);
                            account_error = true;
                        }
                        int account_problem = MainForm->edits->GetPasswordProblem(EditId_AccountPassword);
                        if (account_problem == 4)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(account_problem, false),
                                MainForm->strings->GetDialogText(account_problem, true));
                        if (account_problem == 5)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(account_problem, false),
                                MainForm->strings->GetDialogText(account_problem, true));
                        if (account_problem == -1)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                "At least 1 uppercase character ( A,B etc..) is required in your new password");
                        if (account_problem == -2)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                "At least 1 1owercase character ( a,b etc..) is required in your new password");
                        if (account_problem == -3)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                "At least 1 number or token ( 1,2,#,$ etc..) is required in your new password");
                        if (account_problem)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->edits->SetText(EditId_AccountPassword, "");
                            MainForm->edits->SetText(EditId_ConfirmPassword, "");
                            MainForm->edits->FocusEdit(EditId_AccountPassword);
                            account_error = true;
                        }
                        if (!MainForm->edits->IsValidEmail(MainForm->edits->GetText(EditId_Email)) && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongInputValid, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputValid, true));
                            MainForm->edits->SetText(EditId_Email, "");
                            MainForm->edits->FocusEdit(EditId_Email);
                            account_error = true;
                        }
                        if (!account_error)
                        {
                            if (MainForm->animation->account_name.Length() >= 4 &&
                                MainForm->animation->account_password.Length() >= 6)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->create_account_enabled = false;
                                MainForm->net_client->sender->RequestAccountNameCheck(MainForm->edits->GetText(EditId_AccountName));
                            }
                            else
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_WrongInputFields, false),
                                    MainForm->strings->GetDialogText(DialogText_WrongInputFields, true));
                            }
                        }
                    }
                    else
                    {
                        MainForm->edits->caret_enabled = false;
                        MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                            MainForm->strings->GetDialogText(DialogText_WrongInputFields, false),
                            MainForm->strings->GetDialogText(DialogText_WrongInputFields, true));
                    }
                }
                if (click_x >= 480 && click_x <= 598 && click_y >= 416 && click_y <= 455)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_SignupCode)
            {
                if (click_x >= 358 && click_x <= 476 && click_y >= 416 && click_y <= 455)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    if (MainForm->animation->menu->activation_step >= 2 &&
                        MainForm->animation->menu->create_account_enabled)
                    {
                        if (MainForm->edits->GetText(EditId_Code).Length() < 7)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain, "Wrong PIN",
                                "Please enter the 7-digit code you received in your email.");
                            MainForm->edits->SetText(EditId_Code, "");
                            MainForm->edits->FocusEdit(EditId_Code);
                        }
                        else
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->create_account_enabled = false;
                            MainForm->net_client->sender->SendAccountCreate(MainForm->animation->account_name,
                                MainForm->animation->account_password, MainForm->animation->account_detail_a,
                                MainForm->animation->account_detail_b, MainForm->animation->account_email,
                                MainForm->edits->GetText(EditId_Code));
                        }
                    }
                }
                if (((click_y > 10 && click_y < 17 && 3 * click_y + 560 > click_x) ||
                    (click_y > 16 && click_y < 31 && click_y + 594 > click_x)) && click_x >= 594 && click_x <= 624)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
                if (click_x >= 480 && click_x <= 598 && click_y >= 416 && click_y <= 455)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_PasswordReset)
            {
                if (click_x >= 358 && click_x <= 476 && click_y >= 416 && click_y <= 455)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    if (!MainForm->animation->menu->recover_step)
                    {
                        MainForm->animation->account_name = MainForm->edits->GetText(EditId_AccountName);
                        if (MainForm->edits->GetText(EditId_AccountName) != "")
                        {
                            char recover_refused = false;
                            if (MainForm->edits->GetText(EditId_AccountName).Length() < 4)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                    MainForm->strings->GetDialogText(DialogText_WrongAccount, false),
                                    MainForm->strings->GetDialogText(DialogText_WrongAccount, true));
                                MainForm->edits->SetText(EditId_AccountName, "");
                                MainForm->edits->FocusEdit(EditId_AccountName);
                                recover_refused = true;
                            }
                            if (!recover_refused)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->create_account_enabled = false;
                                MainForm->net_client->sender->RequestPasswordReset(MainForm->edits->GetText(EditId_AccountName));
                            }
                        }
                    }
                    if (MainForm->animation->menu->recover_step == 2)
                    {
                        MainForm->animation->account_password = MainForm->edits->GetText(EditId_Code);
                        if (MainForm->edits->GetText(EditId_Code).Length() < 7)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain, "Wrong PIN",
                                "Please enter the 7-digit code you received in your email.");
                            MainForm->edits->SetText(EditId_Code, "");
                            MainForm->edits->FocusEdit(EditId_Code);
                        }
                        else
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->create_account_enabled = false;
                            MainForm->net_client->sender->SendPasswordResetCode(MainForm->edits->GetText(EditId_Code));
                        }
                    }
                    if (MainForm->animation->menu->recover_step == 3)
                    {
                        char reset_refused = false;
                        if (MainForm->edits->GetText(EditId_AccountPassword) !=
                            MainForm->edits->GetText(EditId_ConfirmPassword) && !reset_refused)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, false),
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, true));
                            MainForm->edits->SetText(EditId_AccountPassword, "");
                            MainForm->edits->SetText(EditId_ConfirmPassword, "");
                            MainForm->edits->FocusEdit(EditId_AccountPassword);
                            reset_refused = true;
                        }
                        int reset_problem = MainForm->edits->GetPasswordProblem(EditId_AccountPassword);
                        if (reset_problem == 4)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(reset_problem, false),
                                MainForm->strings->GetDialogText(reset_problem, true));
                        if (reset_problem == 5)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(reset_problem, false),
                                MainForm->strings->GetDialogText(reset_problem, true));
                        if (reset_problem == -1)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                "At least 1 uppercase character ( A,B etc..) is required in your new password");
                        if (reset_problem == -2)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                "At least 1 1owercase character ( a,b etc..) is required in your new password");
                        if (reset_problem == -3)
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                "At least 1 number or token ( 1,2,#,$ etc..) is required in your new password");
                        if (reset_problem)
                        {
                            MainForm->edits->SetText(EditId_AccountPassword, "");
                            MainForm->edits->SetText(EditId_ConfirmPassword, "");
                            MainForm->edits->FocusEdit(EditId_AccountPassword);
                            reset_refused = true;
                        }
                        if (!reset_refused)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->create_account_enabled = false;
                            MainForm->net_client->sender->SendPasswordReset(MainForm->animation->account_name,
                                MainForm->animation->account_password,
                                MainForm->edits->GetText(EditId_AccountPassword));
                        }
                    }
                }
                if (((click_y > 10 && click_y < 17 && 3 * click_y + 560 > click_x) ||
                    (click_y > 16 && click_y < 31 && click_y + 594 > click_x)) && click_x >= 594 && click_x <= 624)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
                if (click_x >= 580 && click_x <= 600 && click_y >= 353 && click_y <= 373)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->FocusEdit(EditId_Email);
                    MainForm->edits->TypeText("@");
                }
                if (click_x >= 480 && click_x <= 598 && click_y >= 416 && click_y <= 455)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_Login ||
                MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                if (MainForm->animation->GetSubState() == MenuScreen_Login)
                {
                    if (x >= 360 && x <= 450 && y >= 394 && y <= 426 && MainForm->animation->menu->login_enabled)
                    {
                        if (MainForm->edits->GetText(EditId_AccountName) != "" &&
                            MainForm->edits->GetText(EditId_Password) != "")
                        {
                            MainForm->animation->menu->login_enabled = false;
                            MainForm->registry->SetServerHost(MainForm->config->GetHost());
                            MainForm->registry->SetServerPort(MainForm->config->GetPort());
                            MainForm->registry->SetAccountName(MainForm->edits->GetText(EditId_AccountName));
                            if (MainForm->animation->GetSubState() == MenuScreen_Login)
                            {
                                MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                                MainForm->net_client->sender->SendLogin(MainForm->edits->GetText(EditId_AccountName),
                                    MainForm->edits->GetText(EditId_Password));
                            }
                        }
                    }
                    if (x >= 460 && x <= 542 && y >= 374 && y <= 389 && !MainForm->animation->menu->recover_pending)
                    {
                        MainForm->animation->menu->StartRecoverTimeout();
                        MainForm->net_client->SendPacket(AC_GET, OP_LOGIN, "R");
                    }
                }
                if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid && x >= 360 && x <= 450 && y >= 388 &&
                    y <= 417 && MainForm->animation->menu->login_enabled)
                {
                    if (MainForm->animation->menu->account_name != "" && MainForm->animation->menu->password != "")
                    {
                        MainForm->animation->menu->login_enabled = false;
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        MainForm->net_client->sender->SendLogin(LowerCase(MainForm->animation->menu->account_name),
                            LowerCase(MainForm->animation->menu->password));
                    }
                }
                if (MainForm->animation->GetSubState() == MenuScreen_GridAccountName)
                {
                    if (MainForm->edits->GetText(EditId_AccountName) != "")
                    {
                        MainForm->registry->SetServerHost(MainForm->config->GetHost());
                        MainForm->registry->SetServerPort(MainForm->config->GetPort());
                        MainForm->registry->SetAccountName(MainForm->edits->GetText(EditId_AccountName));
                        MainForm->animation->menu->account_name = MainForm->edits->GetText(EditId_AccountName);
                        MainForm->animation->menu->account_name =
                            MainForm->net_client->Capitalize(MainForm->animation->menu->account_name);
                        MainForm->animation->menu->grid_column = random(4) + 1;
                        MainForm->animation->menu->grid_row = random(4) + 1;
                        MainForm->animation->menu->password = "";
                        MainForm->edits->Clear();
                        MainForm->net_client->login_failure_count = 0;
                        MainForm->gfx->LoadPasswordScreen();
                        MainForm->animation->SetSubState(MenuScreen_PasswordGrid);
                        if (MainForm->animation->menu->retry_ticks > 0)
                        {
                            if (LowerCase(MainForm->animation->menu->retry_account_name) ==
                                LowerCase(MainForm->animation->menu->account_name))
                            {
                                MainForm->animation->menu->retry_ticks = 0;
                                MainForm->animation->menu->password = MainForm->animation->menu->retry_password;
                            }
                        }
                    }
                }
                if (x >= 452 && x <= 542 && y >= 394 && y <= 423)
                {
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                if (x >= 289 && x <= 423 && y >= 240 && y <= 374)
                {
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetText(GameText_Information),
                        MainForm->strings->GetText(GameText_UseYourArrowkeysAnd));
                    MainForm->animation->menu->message_shows_image = true;
                    MainForm->edits->caret_enabled = false;
                }
                if (x >= 464 && x <= 486 && y >= 335 && y <= 354 && MainForm->animation->menu->grid_row > 0)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                    MainForm->animation->menu->grid_row--;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (x >= 445 && x <= 463 && y >= 355 && y <= 374 && MainForm->animation->menu->grid_column > 0)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                    MainForm->animation->menu->grid_column--;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (x >= 464 && x <= 486 && y >= 355 && y <= 374 && MainForm->animation->menu->grid_row < 5)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                    MainForm->animation->menu->grid_row++;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (x >= 487 && x <= 509 && y >= 355 && y <= 374 && MainForm->animation->menu->grid_column < 5)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                    MainForm->animation->menu->grid_column++;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (x >= 514 && x <= 562 && y >= 335 && y <= 354)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->AddSpace();
                }
                if (x >= 514 && x <= 562 && y >= 355 && y <= 374)
                    MainForm->animation->menu->AddSelectedLetter();
                if (x >= 558 && x <= 562 && y >= 284 && y <= 298)
                {
                    MainForm->sfx->PlaySfx(SfxSound_Send);
                    MainForm->animation->menu->DeleteLastLetter();
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_Main || MainForm->animation->GetSubState() == MenuScreen_Credits ||
                MainForm->animation->GetSubState() == MenuScreen_Login || MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                // The main menu buttons, from the bottom: exit, the credits, log in, create an account.
                if (click_x >= 30 && click_x <= 210 && click_y >= 400 && click_y <= 440)
                {
                    if (MainForm->mm_timer_id)
                        timeKillEvent((unsigned int)MainForm->mm_timer_id);
                    if (MainForm->client->Active)
                        ((TAbstractSocket *)MainForm->client)->Close();
                    MainForm->edits->Clear();
                    MainForm->players->Clear();
                    MainForm->effects->Clear();
                    MainForm->map_items->ClearAll();
                    MainForm->gfx->objects->ReleaseGraphics();
                    MainForm->gfx->masks->ReleaseGraphics();
                    MainForm->gfx->walls->ReleaseGraphics();
                    MainForm->gfx->tiles->ReleaseGraphics();
                    MainForm->gfx->blends->ReleaseGraphics();
                    MainForm->gfx->items->ReleaseGraphics();
                    MainForm->gfx->items->ReleaseSubItemGraphics();
                    MainForm->gfx->npc_cache->ReleaseGraphics();
                    MainForm->gfx->FreeMenuGraphics();
                    MainForm->gfx->FreeGameGraphics();
                    MainForm->gfx->FreeHoverLabel();
                    if (MainForm->Dx_Input1->Created)
                        MainForm->Dx_Input1->Destroy();
                    if (MainForm->Dx_Sound1->Created)
                        MainForm->Dx_Sound1->Destroy();
                    if (MainForm->Dx_Draw1->Created && MainForm->config->GetFullscreen())
                        MainForm->Dx_Draw1->FlipToGDISurface();
                    if (MainForm->Dx_Draw1->Created)
                        MainForm->Dx_Draw1->Destroy();
                    Application->Terminate();
                }
                if (click_x >= 30 && click_x <= 210 && click_y >= 360 && click_y <= 400)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->edits->Clear();
                    MainForm->animation->SetSubState(MenuScreen_Credits);
                    if (!MainForm->animation->credits->is_running)
                    {
                        MainForm->gfx->LoadMenuBackground();
                        MainForm->animation->credits->Start();
                        MainForm->SampleCreditNameColor();
                    }
                }
                if (click_x >= 30 && click_x <= 210 && click_y >= 320 && click_y <= 360)
                    OpenLoginScreen();
                if (click_x >= 30 && click_x <= 210 && click_y >= 280 && click_y <= 320)
                {
                    MainForm->animation->menu->create_account_enabled = true;
                    // With no session yet the button only connects; the reply then opens the login screen.
                    if (!MainForm->net_client->is_initialized)
                    {
                        if (!MainForm->animation->menu->connect_for_recovery &&
                            !MainForm->animation->menu->connect_for_create_account &&
                            !MainForm->animation->menu->connect_for_login &&
                            (MainForm->client->Host.Length() > 0 || MainForm->client->Address.Length() > 0))
                        {
                            if (MainForm->client->Active)
                                MainForm->client->Active = false;
                            MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                            MainForm->client->Active = true;
                            MainForm->animation->menu->connect_for_recovery = false;
                            MainForm->animation->menu->connect_for_login = false;
                            MainForm->animation->menu->connect_for_create_account = 1;
                        }
                    }
                    else
                    {
                        if (MainForm->animation->credits->is_running)
                            MainForm->animation->credits->Stop();
                        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                        MainForm->edits->Clear();
                        MainForm->animation->SetSubState(MenuScreen_CreateAccount);
                        MainForm->gfx->LoadCreateAccountScreen();
                        MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                            ACCOUNT_NAME_EDIT_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(EditId_AccountPassword, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                            ACCOUNT_PASSWORD_EDIT_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(EditId_ConfirmPassword, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                            ACCOUNT_CONFIRM_EDIT_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(EditId_AccountDetailA, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                            ACCOUNT_EDIT8_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(EditId_AccountDetailB, EditGroup_Menu, ACCOUNT_FORM_EDIT_X,
                            ACCOUNT_EDIT9_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(EditId_Email, EditGroup_Menu, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT10_Y,
                            221, EDIT_LINE_HEIGHT);
                        MainForm->edits->SetPasswordMode(EditId_AccountPassword, true);
                        MainForm->edits->SetPasswordMode(EditId_ConfirmPassword, true);
                        MainForm->edits->SetShowStrength(EditId_AccountPassword, true);
                        MainForm->edits->SetAllowSpaces(EditId_AccountName, true);
                        MainForm->edits->SetAllowSpaces(EditId_AccountPassword, false);
                        MainForm->edits->SetAllowSpaces(EditId_ConfirmPassword, false);
                        MainForm->edits->SetAllowSpaces(EditId_Email, false);
                        MainForm->edits->DisallowLeadingSpace(EditId_AccountName);
                        MainForm->edits->DisallowLeadingSpace(EditId_AccountDetailA);
                        MainForm->edits->DisallowLeadingSpace(EditId_AccountDetailB);
                        MainForm->edits->DisallowDoubleSpaces(EditId_AccountName);
                        MainForm->edits->DisallowDoubleSpaces(EditId_AccountDetailA);
                        MainForm->edits->DisallowDoubleSpaces(EditId_AccountDetailB);
                        MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Alphanumeric);
                        MainForm->edits->SetCharFilter(EditId_AccountPassword, EditCharFilter_Password);
                        MainForm->edits->SetCharFilter(EditId_ConfirmPassword, EditCharFilter_Password);
                        MainForm->edits->SetCharFilter(EditId_AccountDetailA, EditCharFilter_Letters);
                        MainForm->edits->SetCharFilter(EditId_AccountDetailB, EditCharFilter_Letters);
                        MainForm->edits->SetCharFilter(EditId_Email, EditCharFilter_Email);
                        MainForm->edits->SetMaxLength(EditId_AccountName, 16);
                        MainForm->edits->SetMaxLength(EditId_AccountPassword, 16);
                        MainForm->edits->SetMaxLength(EditId_ConfirmPassword, 16);
                        MainForm->edits->FocusEdit(EditId_AccountName);
                        MainForm->edits->caret_enabled = false;
                        MainForm->animation->menu->OpenTextWindow();
                        MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_ItIsVeryImportant),
                            true);
                        MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_LotOfPlayersWho),
                            true);
                        MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_YourInformationWillOnly),
                            false);
                    }
                }
            }
        }
    }
}

// Scrolls with the wheel whatever takes it first: the input form, an open window's list or text, or the item tip
// shown over the inventory, paperdoll, shop, trade or locker.
void Mouser::HandleWheel(int direction)
{
    char handled = false;
    // The wheel scrolls the first open form, window or tip, in this order; the panels come last.
    if (!handled && direction == WheelDirection_Up && MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (!handled && MainForm->animation->game->form_visible)
        {
            MainForm->animation->game->InputFormScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->guild->is_visible)
        {
            MainForm->animation->guild->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->dialog->is_visible)
        {
            MainForm->animation->dialog->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->memo->is_visible)
        {
            MainForm->animation->memo->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->report->is_visible)
        {
            MainForm->animation->report->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->interface_controller->inventory->tooltip->IsVisible())
        {
            handled = MainForm->animation->interface_controller->inventory->tooltip->ScrollUp();
            if (handled)
                MainForm->animation->interface_controller->inventory->MarkItemsDirty();
        }
        if (!handled && MainForm->animation->paperdoll->tooltip->IsVisible())
            handled = MainForm->animation->paperdoll->tooltip->ScrollUp();
        if (!handled && MainForm->animation->shop->tooltip->IsVisible())
            handled = MainForm->animation->shop->tooltip->ScrollUp();
        if (!handled && MainForm->animation->trade->tooltip->IsVisible())
            handled = MainForm->animation->trade->tooltip->ScrollUp();
        if (!handled && MainForm->animation->locker->tooltip->IsVisible())
            handled = MainForm->animation->locker->tooltip->ScrollUp();
        if (!handled && MainForm->animation->learn->is_visible)
        {
            MainForm->animation->learn->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->shop->is_visible)
        {
            MainForm->animation->shop->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->law->is_visible)
        {
            MainForm->animation->law->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->inn->is_visible)
        {
            MainForm->animation->inn->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->locker->is_visible)
        {
            MainForm->animation->locker->ScrollUp();
            handled = true;
        }
        if (!handled && MainForm->animation->message_board->is_visible)
        {
            MainForm->animation->message_board->ScrollUp();
            handled = true;
        }
    }
    if (!handled && direction == WheelDirection_Down && MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (!handled && MainForm->animation->game->form_visible)
        {
            MainForm->animation->game->InputFormScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->guild->is_visible)
        {
            MainForm->animation->guild->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->dialog->is_visible)
        {
            MainForm->animation->dialog->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->memo->is_visible)
        {
            MainForm->animation->memo->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->report->is_visible)
        {
            MainForm->animation->report->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->interface_controller->inventory->tooltip->IsVisible())
        {
            handled = MainForm->animation->interface_controller->inventory->tooltip->ScrollDown();
            if (handled)
                MainForm->animation->interface_controller->inventory->MarkItemsDirty();
        }
        if (!handled && MainForm->animation->paperdoll->tooltip->IsVisible())
            handled = MainForm->animation->paperdoll->tooltip->ScrollDown();
        if (!handled && MainForm->animation->shop->tooltip->IsVisible())
            handled = MainForm->animation->shop->tooltip->ScrollDown();
        if (!handled && MainForm->animation->trade->tooltip->IsVisible())
            handled = MainForm->animation->trade->tooltip->ScrollDown();
        if (!handled && MainForm->animation->locker->tooltip->IsVisible())
            handled = MainForm->animation->locker->tooltip->ScrollDown();
        if (!handled && MainForm->animation->learn->is_visible)
        {
            MainForm->animation->learn->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->shop->is_visible)
        {
            MainForm->animation->shop->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->law->is_visible)
        {
            MainForm->animation->law->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->inn->is_visible)
        {
            MainForm->animation->inn->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->locker->is_visible)
        {
            MainForm->animation->locker->ScrollDown();
            handled = true;
        }
        if (!handled && MainForm->animation->message_board->is_visible)
        {
            MainForm->animation->message_board->ScrollDown();
            handled = true;
        }
    }
    if (!handled && direction == WheelDirection_Up && MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (MainForm->animation->GetSubState() == GamePanel_Chat)
            MainForm->animation->interface_controller->chat->ScrollUp();
        if (MainForm->animation->GetSubState() == GamePanel_Playerlist)
            MainForm->animation->interface_controller->player_list->ScrollUp();
        if (MainForm->animation->GetSubState() == GamePanel_Group)
            MainForm->animation->interface_controller->group->ScrollUp();
        if (MainForm->animation->GetSubState() == GamePanel_News)
            MainForm->animation->interface_controller->news->ScrollUp();
        if (MainForm->animation->GetSubState() == GamePanel_Help)
            MainForm->animation->interface_controller->help->ScrollUp();
        if (MainForm->animation->GetSubState() == GamePanel_Spells)
            MainForm->animation->interface_controller->spells->ScrollUp();
        if (MainForm->animation->GetSubState() == GamePanel_Skills)
            MainForm->animation->interface_controller->skills->ScrollUp();
    }
    if (!handled && direction == WheelDirection_Down && MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (MainForm->animation->GetSubState() == GamePanel_Chat)
            MainForm->animation->interface_controller->chat->ScrollDown();
        if (MainForm->animation->GetSubState() == GamePanel_Playerlist)
            MainForm->animation->interface_controller->player_list->ScrollDown();
        if (MainForm->animation->GetSubState() == GamePanel_Group)
            MainForm->animation->interface_controller->group->ScrollDown();
        if (MainForm->animation->GetSubState() == GamePanel_News)
            MainForm->animation->interface_controller->news->ScrollDown();
        if (MainForm->animation->GetSubState() == GamePanel_Help)
            MainForm->animation->interface_controller->help->ScrollDown();
        if (MainForm->animation->GetSubState() == GamePanel_Spells)
            MainForm->animation->interface_controller->spells->ScrollDown();
        if (MainForm->animation->GetSubState() == GamePanel_Skills)
            MainForm->animation->interface_controller->skills->ScrollDown();
    }
}

// Ends a drag: an item let go over an open window or the map goes there, and every window stops dragging.
void Mouser::HandleMouseUp()
{
    int own_id;
    int own_y;
    int own_x;
    int gold_amount;
    if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        // Only the first open box or window, in this order, takes the click.
        char over_window = false;
        if (MainForm->animation->interface_controller->spells->is_dragging)
        {
            if (MainForm->animation->hotkeys->is_visible)
            {
                over_window = true;
                if (MainForm->animation->hotkeys->window_y < y && MainForm->animation->hotkeys->window_y + 122 > y &&
                    MainForm->animation->hotkeys->window_x < x && MainForm->animation->hotkeys->window_x + 398 > x)
                    MainForm->animation->hotkeys->DropSpell(MainForm->animation->interface_controller->spells->drag_spell_id,
                        x, y);
            }
        }
        // A dragged item goes to the open window under it, or onto the map within reach.
        if (MainForm->animation->interface_controller->inventory->is_dragging &&
            !MainForm->animation->interface_controller->inventory->dragging_from_paperdoll &&
            MainForm->gfx->game->view_clip_rect.top < y && MainForm->gfx->game->view_clip_rect.bottom > y &&
            MainForm->gfx->game->view_clip_rect.left < x && MainForm->gfx->game->view_clip_rect.right > x)
        {
            if (MainForm->animation->trade->is_visible)
            {
                over_window = true;
                if (MainForm->animation->trade->window_y + 43 < y && MainForm->animation->trade->window_y + 243 > y &&
                    MainForm->animation->trade->window_x + 280 < x && MainForm->animation->trade->window_x + 515 > x &&
                    !MainForm->animation->game->message_visible && !MainForm->animation->trade->trade_locked)
                {
                    if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                        ItemSpecial_Bound)
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                            MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    else if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                        2)
                    {
                        MainForm->net_client->sender->SendTradeAddItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                            1);
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                    }
                    else
                    {
                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_TradeAddItem,
                            MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                            MainForm->animation->interface_controller->inventory->drag_item_id, 0);
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                    }
                }
            }
            if (MainForm->animation->hotkeys->is_visible)
            {
                over_window = true;
                if (MainForm->animation->hotkeys->window_y < y && MainForm->animation->hotkeys->window_y + 122 > y &&
                    MainForm->animation->hotkeys->window_x < x && MainForm->animation->hotkeys->window_x + 398 > x)
                    MainForm->animation->hotkeys->DropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                        x, y);
            }
            if (MainForm->animation->paperdoll->is_visible)
            {
                over_window = true;
                if (MainForm->animation->paperdoll->window_y < y &&
                    MainForm->animation->paperdoll->window_y + 290 > y &&
                    MainForm->animation->paperdoll->window_x < x && MainForm->animation->paperdoll->window_x + 380 > x)
                    MainForm->animation->paperdoll->EquipDroppedItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                        x, y);
            }
            if (MainForm->animation->bank->is_visible)
            {
                over_window = true;
                if (MainForm->animation->bank->window_y + 50 < y && y < MainForm->animation->bank->window_y + 170 &&
                    MainForm->animation->bank->window_x + 17 < x && MainForm->animation->bank->window_x + 252 > x)
                {
                    if (MainForm->animation->interface_controller->inventory->drag_item_id == ItemId_Gold)
                    {
                        gold_amount = MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                        if (gold_amount > 0)
                        {
                            if (gold_amount == 1)
                                MainForm->net_client->sender->SendBankDeposit(MainForm->animation->bank->npc_session_id,
                                    1);
                            else
                            {
                                MainForm->animation->game->OpenAmountBox(AmountBoxKind_BankDeposit, gold_amount,
                                    ItemId_Gold, 0);
                                MainForm->animation->interface_controller->inventory->input_enabled = true;
                            }
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, false),
                                MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                        }
                    }
                }
            }
            if (MainForm->animation->locker->is_visible)
            {
                over_window = true;
                if (MainForm->animation->locker->window_y + 17 < y && y < MainForm->animation->locker->window_y + 217 &&
                    MainForm->animation->locker->window_x + 17 < x && MainForm->animation->locker->window_x + 252 > x)
                {
                    if (MainForm->animation->interface_controller->inventory->drag_item_id > 1)
                    {
                        if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                            2)
                        {
                            TPoint locker_position;
                            locker_position.x = MainForm->animation->locker->locker_x;
                            locker_position.y = MainForm->animation->locker->locker_y;
                            if (MainForm->animation->locker->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                200)
                            {
                                if (MainForm->animation->locker->CanStoreItem(MainForm->animation->interface_controller->inventory->drag_item_id))
                                {
                                    MainForm->net_client->sender->SendLockerAddItem(locker_position,
                                        MainForm->animation->interface_controller->inventory->drag_item_id, 1);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LockerFullHold, false),
                                        MainForm->strings->GetDialogText(DialogText_LockerFullHold, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                            else
                            {
                                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_LockerFullStore, false),
                                    MainForm->strings->GetDialogText(DialogText_LockerFullStore, true));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                        else
                        {
                            MainForm->animation->game->OpenAmountBox(AmountBoxKind_LockerAddItem,
                                MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                MainForm->animation->interface_controller->inventory->drag_item_id, 0);
                            MainForm->animation->interface_controller->inventory->input_enabled = true;
                        }
                    }
                    else
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_RefusedDeposit, false),
                            MainForm->strings->GetDialogText(DialogText_RefusedDeposit, true));
                        MainForm->edits->caret_enabled = false;
                    }
                }
            }
            if (MainForm->animation->chest->is_visible)
            {
                over_window = true;
                if (MainForm->animation->chest->window_y + 17 < y && MainForm->animation->chest->window_y + 217 > y &&
                    MainForm->animation->chest->window_x + 17 < x && MainForm->animation->chest->window_x + 252 > x)
                {
                    if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                        ItemSpecial_Bound)
                    {
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                            MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    else if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                        2)
                    {
                        TPoint chest_position;
                        chest_position.x = MainForm->animation->chest->chest_x;
                        chest_position.y = MainForm->animation->chest->chest_y;
                        MainForm->net_client->sender->SendChestAddItem(chest_position,
                            MainForm->animation->interface_controller->inventory->drag_item_id, 1);
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                    }
                    else
                    {
                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_ChestAddItem,
                            MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                            MainForm->animation->interface_controller->inventory->drag_item_id, 0);
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                    }
                }
            }
            if (!over_window)
            {
                own_id = MainForm->net_client->GetPlayerId();
                own_x = MainForm->players->GetX(own_id);
                own_y = MainForm->players->GetY(own_id);
                if (MainForm->net_client->IsWithinReach(own_x, own_y, MainForm->map->GetCursorX(),
                    MainForm->map->GetCursorY()) &&
                    !MainForm->map->IsWallOrSpecTile(MainForm->map->GetCursorX(), MainForm->map->GetCursorY()))
                {
                    MainForm->animation->interface_controller->inventory->CancelDrag();
                    char can_drop = true;
                    if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                        ItemSpecial_Bound)
                    {
                        can_drop = false;
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                            MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    if (can_drop && MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
                    {
                        can_drop = false;
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetText(GameText_Warning),
                            MainForm->strings->GetText(GameText_YouCannotDropItems));
                        MainForm->edits->caret_enabled = false;
                    }
                    if (MainForm->map_items->GetProtectionOwnerAt(MainForm->map->GetCursorX(),
                        MainForm->map->GetCursorY()) > 0)
                        can_drop = false;
                    if (can_drop)
                    {
                        if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                            2)
                        {
                            if (MainForm->map->GetCursorX() >= 0 && MainForm->map->GetCursorY() >= 0 &&
                                MainForm->map->GetCursorX() <= MainForm->map->GetWidth() &&
                                MainForm->map->GetCursorY() <= MainForm->map->GetHeight())
                            {
                                if (MainForm->animation->interface_controller->inventory->drag_item_id == ItemId_Gold)
                                {
                                    if (!MainForm->animation->interface_controller->inventory->GetItemAmount(1))
                                    {
                                        String gold_name = MainForm->item_values->GetName(ItemId_Gold);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                            MainForm->strings->GetText(GameText_YouHaveNoMore) + " " + gold_name);
                                    }
                                    else
                                    {
                                        MainForm->net_client->sender->SendDropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                            1, MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                                    }
                                }
                                else
                                {
                                    MainForm->net_client->sender->SendDropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                        1, MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                            }
                        }
                        else
                        {
                            MainForm->animation->game->OpenAmountBoxAtTile(AmountBoxKind_DropItem,
                                MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                MainForm->animation->interface_controller->inventory->drag_item_id, 0,
                                MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                            MainForm->animation->interface_controller->inventory->input_enabled = false;
                        }
                    }
                }
                else if (!MainForm->net_client->IsWithinReach(own_x, own_y, MainForm->map->GetCursorX(),
                    MainForm->map->GetCursorY()))
                {
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_OutOfDropRange));
                    MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_OutOfRange,
                        CHAT_COLOR_DEFAULT, MainForm->strings->GetText(GameText_OutOfDropRange), true);
                }
            }
        }
        MainForm->animation->interface_controller->chat->StopScrolling();
        MainForm->animation->interface_controller->player_list->MouseUp();
        MainForm->animation->interface_controller->group->MouseUp();
        MainForm->animation->interface_controller->news->MouseUp();
        MainForm->animation->interface_controller->help->MouseUp();
        MainForm->animation->interface_controller->inventory->MouseUp();
        MainForm->animation->interface_controller->spells->MouseUp();
        MainForm->animation->interface_controller->skills->MouseUp();
        MainForm->animation->visual_doc->MouseUp();
        MainForm->animation->hotkeys->MouseUp();
        MainForm->animation->paperdoll->MouseUp();
        MainForm->animation->book->MouseUp();
        MainForm->animation->trade->MouseUp();
        MainForm->animation->bank->MouseUp();
        MainForm->animation->jukebox->MouseUp();
        MainForm->animation->musician->MouseUp();
        MainForm->animation->barber->MouseUp();
        MainForm->animation->stat->MouseUp();
        MainForm->animation->learn->MouseUp();
        MainForm->animation->crafter->MouseUp();
        MainForm->animation->shop->MouseUp();
        MainForm->animation->guild->MouseUp();
        MainForm->animation->memo->MouseUp();
        MainForm->animation->dialog->MouseUp();
        MainForm->animation->report->MouseUp();
        MainForm->animation->law->MouseUp();
        MainForm->animation->inn->MouseUp();
        MainForm->animation->chest->MouseUp();
        MainForm->animation->locker->MouseUp();
        MainForm->animation->social->StopDragging();
        MainForm->animation->message_board->MouseUp();
        MainForm->animation->quests->MouseUp();
        MainForm->animation->game->MouseUp();
    }
    if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
        MainForm->animation->menu->StopDragging();
}

// Opens the chat panel, with a click when asked; global chat follows its tab.
void Mouser::OpenChatPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Chat)
    {
        MainForm->animation->interface_controller->chat->SyncGlobalSubscription();
        MainForm->gfx->LoadChatPanel();
        MainForm->animation->SetSubState(GamePanel_Chat);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
            MainForm->strings->GetText(GameText_ChatPanelIsNow));
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->chat->Invalidate();
    }
}

// Asks the server for the online players and shows the player list panel, leaving global chat.
void Mouser::OpenPlayerlistPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    MainForm->animation->interface_controller->player_list->Clear();
    MainForm->net_client->sender->RequestOnlinePlayers();
    MainForm->animation->interface_controller->StartPlayerListCooldown();
    if (MainForm->animation->GetSubState() != GamePanel_Playerlist)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadPlayerlistPanel();
        MainForm->animation->SetSubState(GamePanel_Playerlist);
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->player_list->MarkListDirty();
    }
}

// Opens the status panel, with a click when asked, leaving global chat.
void Mouser::OpenStatusPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Status)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadStatusPanel();
        MainForm->animation->SetSubState(GamePanel_Status);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
            MainForm->strings->GetText(GameText_StatusPanelIsNow));
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->status->MarkDirty();
    }
}

// Opens the inventory panel, with a click when asked, leaving global chat.
void Mouser::OpenInventoryPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Inventory)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadInventoryPanel();
        MainForm->animation->SetSubState(GamePanel_Inventory);
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->inventory->MarkItemsDirty();
        MainForm->animation->interface_controller->inventory->MarkHoverDirty();
    }
}

// Steps the minimap through its modes by the minimap setting, or off; on a map without one it says so.
void Mouser::ToggleMinimap(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (!MainForm->map->IsMinimapDisabled())
    {
        if (!MainForm->gfx->interface_graphics->minimap_visible)
        {
            if (MainForm->config->GetMinimapMode() < 2)
                MainForm->gfx->interface_graphics->minimap_visible = 1;
            else
                MainForm->gfx->interface_graphics->minimap_visible = 2;
        }
        else if (!MainForm->config->GetMinimapMode() && MainForm->gfx->interface_graphics->minimap_visible == 1)
            MainForm->gfx->interface_graphics->minimap_visible = 2;
        else
            MainForm->gfx->interface_graphics->minimap_visible = 0;
    }
    else
    {
        MainForm->gfx->interface_graphics->minimap_visible = 0;
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_ThereIsNoMap));
    }
    MainForm->redraw_needed = true;
}

// Shows the group panel, leaving global chat, and asks the server for the member list at most every 50 ticks.
void Mouser::OpenGroupPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Group)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadGroupPanel();
        MainForm->animation->SetSubState(GamePanel_Group);
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->group->MarkListDirty();
        if (!MainForm->animation->interface_controller->group->IsListRequestCoolingDown())
        {
            MainForm->animation->interface_controller->group->SetListRequestCooldown(50);
            MainForm->net_client->SendPacket(AC_GET, OP_GROUP,
                MainForm->net_client->EncodeNumber(MainForm->animation->interface_controller->group->members.size(),
                    1));
        }
    }
}

// Opens the spell panel, with a click when asked, leaving global chat.
void Mouser::OpenSpellPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Spells)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadSpellPanel();
        MainForm->animation->SetSubState(GamePanel_Spells);
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->spells->MarkDirty();
        MainForm->animation->interface_controller->spells->MarkSelectionDirty();
    }
}

// Opens the skill panel, with a click when asked, leaving global chat.
void Mouser::OpenSkillPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Skills)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadSkillPanel();
        MainForm->animation->SetSubState(GamePanel_Skills);
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->skills->MarkDirty();
        MainForm->animation->interface_controller->skills->MarkSelectionDirty();
    }
}

// Opens the settings panel, with a click when asked, leaving global chat.
void Mouser::OpenSettingsPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Settings)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->gfx->LoadSettingsPanel();
        MainForm->animation->SetSubState(GamePanel_Settings);
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->settings->MarkDirty();
    }
}

// Shows the help panel at its index page with the report window's help lines, leaving global chat; when it is shown
// already, goes back to the index.
void Mouser::OpenHelpPanel(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    if (MainForm->animation->GetSubState() != GamePanel_Help)
    {
        MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
        MainForm->animation->report->Open(MainForm->strings->GetText(GameText_EndlessHelp));
        MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_ListWithHelpfullTopics), "", true);
        MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_YouCanAlwaysAsk), "", true);
        MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IForgotMyPassword), "", false);
        MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IWantToReport), "", false);
        MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IWantToSpeak), "", false);
        MainForm->gfx->LoadHelpPanel();
        MainForm->animation->SetSubState(GamePanel_Help);
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
            MainForm->strings->GetText(GameText_GameHelp));
        MainForm->animation->interface_controller->Invalidate();
        MainForm->animation->interface_controller->help->LoadPage("index");
        MainForm->animation->interface_controller->help->MarkTextDirty();
    }
    else
    {
        MainForm->animation->interface_controller->help->LoadPage("index");
        MainForm->animation->interface_controller->help->MarkTextDirty();
    }
}

// Opens the login screen with the account name and password boxes, first connecting when no connection is up.
void Mouser::OpenLoginScreen()
{
    MainForm->animation->menu->login_enabled = true;
    if (!MainForm->net_client->is_initialized)
    {
        if (MainForm->animation->menu->connect_for_recovery || MainForm->animation->menu->connect_for_create_account ||
            MainForm->animation->menu->connect_for_login)
            return;
        if (MainForm->animation->menu->connect_for_recovery || MainForm->animation->menu->connect_for_create_account ||
            MainForm->animation->menu->connect_for_login)
            return;
        if (MainForm->client->Active)
        {
            MainForm->client->Active = false;
            Sleep(60);
        }
        MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
        MainForm->client->Active = true;
        MainForm->animation->menu->connect_for_recovery = true;
        MainForm->animation->menu->connect_for_create_account = 0;
        MainForm->animation->menu->connect_for_login = false;
        return;
    }
    if (MainForm->animation->credits->is_running)
        MainForm->animation->credits->Stop();
    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
    MainForm->edits->Clear();
    MainForm->net_client->login_failure_count = 0;
    MainForm->gfx->LoadLoginScreen();
    if (MainForm->config->GetSkipSoftpad())
    {
        MainForm->animation->SetSubState(MenuScreen_Login);
        MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, LOGIN_EDIT_X, 308, 141, EDIT_LINE_HEIGHT);
        MainForm->edits->AddEdit(EditId_Password, EditGroup_Menu, LOGIN_EDIT_X, 339, 141, EDIT_LINE_HEIGHT);
        MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Alphanumeric);
        MainForm->edits->SetCharFilter(EditId_Password, EditCharFilter_Password);
        MainForm->edits->SetAllowSpaces(EditId_AccountName, true);
        MainForm->edits->SetAllowSpaces(EditId_Password, false);
        MainForm->edits->SetPasswordMode(EditId_Password, true);
        MainForm->edits->SetMaxLength(EditId_AccountName, 16);
        MainForm->edits->SetMaxLength(EditId_Password, 16);
        MainForm->edits->FocusEdit(EditId_AccountName);
        return;
    }
    MainForm->animation->SetSubState(MenuScreen_GridAccountName);
    MainForm->edits->AddEdit(EditId_AccountName, EditGroup_Menu, LOGIN_EDIT_X, GRID_LOGIN_NAME_EDIT_Y, 161,
        EDIT_LINE_HEIGHT);
    MainForm->edits->SetCharFilter(EditId_AccountName, EditCharFilter_Letters);
    MainForm->edits->SetAllowSpaces(EditId_AccountName, true);
    MainForm->edits->SetMaxLength(EditId_AccountName, 16);
    MainForm->edits->FocusEdit(EditId_AccountName);
}
