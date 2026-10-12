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
#pragma package(smart_init)

#define WRAP_TEXT_WIDTH 200

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
    if (moved)
    {
        moved = 0;
        if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
        {
            MainForm->animation->game->MouseMove(x, y);
            if (MainForm->animation->game->message_visible || MainForm->animation->game->large_box_visible ||
                MainForm->animation->game->input_box_visible || MainForm->animation->game->form_visible ||
                MainForm->animation->paperdoll->is_visible || MainForm->animation->book->is_visible ||
                MainForm->animation->trade->is_visible || MainForm->animation->bank->is_visible ||
                MainForm->animation->jukebox->is_visible || MainForm->animation->musician->is_visible ||
                MainForm->animation->barber->is_visible || MainForm->animation->learn->is_visible ||
                MainForm->animation->shop->is_visible || MainForm->animation->stat->is_visible ||
                MainForm->animation->guild->is_visible || MainForm->animation->memo->is_visible ||
                MainForm->animation->dialog->is_visible || MainForm->animation->report->is_visible ||
                MainForm->animation->law->is_visible || MainForm->animation->inn->is_visible ||
                MainForm->animation->locker->is_visible || MainForm->animation->chest->is_visible ||
                MainForm->animation->social->is_open || MainForm->animation->message_board->is_visible ||
                MainForm->animation->quests->is_visible || MainForm->animation->game->amount_box_visible)
            {
                char window_handled = 0;
                if (MainForm->animation->game->message_visible)
                {
                    window_handled = 1;
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
                    else
                    {
                        if (MainForm->animation->game->message_ok_hovered)
                        {
                            MainForm->animation->game->message_ok_hovered = false;
                            MainForm->redraw_needed = true;
                        }
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
                    else
                    {
                        if (MainForm->animation->game->message_cancel_hovered)
                        {
                            MainForm->animation->game->message_cancel_hovered = false;
                            MainForm->redraw_needed = true;
                        }
                    }
                }
                if (MainForm->animation->game->large_box_visible)
                {
                    window_handled = 1;
                    if (MainForm->animation->game->large_box_x + 184 <= x &&
                        x <= MainForm->animation->game->large_box_x + 272 &&
                        MainForm->animation->game->large_box_y + 147 <= y &&
                        MainForm->animation->game->large_box_y + 173 >= y)
                    {
                        if (!MainForm->animation->game->large_box_ok_hovered)
                        {
                            MainForm->animation->game->large_box_ok_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else
                    {
                        if (MainForm->animation->game->large_box_ok_hovered)
                        {
                            MainForm->animation->game->large_box_ok_hovered = false;
                            MainForm->redraw_needed = true;
                        }
                    }
                    if (MainForm->animation->game->large_box_x + 91 <= x &&
                        x <= MainForm->animation->game->large_box_x + 179 &&
                        MainForm->animation->game->large_box_y + 147 <= y &&
                        MainForm->animation->game->large_box_y + 173 >= y)
                    {
                        if (!MainForm->animation->game->large_box_cancel_hovered)
                        {
                            MainForm->animation->game->large_box_cancel_hovered = true;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else
                    {
                        if (MainForm->animation->game->large_box_cancel_hovered)
                        {
                            MainForm->animation->game->large_box_cancel_hovered = false;
                            MainForm->redraw_needed = true;
                        }
                    }
                }
                if (MainForm->animation->game->input_box_visible)
                {
                    window_handled = 1;
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
                    else
                    {
                        if (MainForm->animation->game->input_box_ok_hovered)
                        {
                            MainForm->animation->game->input_box_ok_hovered = false;
                            MainForm->redraw_needed = true;
                        }
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
                    else
                    {
                        if (MainForm->animation->game->input_box_cancel_hovered)
                        {
                            MainForm->animation->game->input_box_cancel_hovered = false;
                            MainForm->redraw_needed = true;
                        }
                    }
                }
                // From here on only the first open window, in this order, gets the move.
                if (MainForm->animation->game->form_visible && !window_handled)
                {
                    window_handled = 1;
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
                    else
                    {
                        if (MainForm->animation->game->form_ok_hovered)
                        {
                            MainForm->animation->game->form_ok_hovered = false;
                            MainForm->redraw_needed = true;
                        }
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
                    else
                    {
                        if (MainForm->animation->game->form_cancel_hovered)
                        {
                            MainForm->animation->game->form_cancel_hovered = false;
                            MainForm->redraw_needed = true;
                        }
                    }
                }
                if (MainForm->animation->social->is_open && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->social->OnMouseMove(x, y);
                }
                if (MainForm->animation->message_board->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->message_board->MouseMove(x, y);
                }
                if (MainForm->animation->game->amount_box_visible && !window_handled)
                {
                    window_handled = 1;
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
                    else
                    {
                        if (MainForm->animation->game->amount_ok_hovered)
                        {
                            MainForm->animation->game->amount_ok_hovered = false;
                            MainForm->redraw_needed = true;
                        }
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
                    else
                    {
                        if (MainForm->animation->game->amount_cancel_hovered)
                        {
                            MainForm->animation->game->amount_cancel_hovered = false;
                            MainForm->redraw_needed = true;
                        }
                    }
                }
                if (MainForm->animation->trade->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->trade->MouseMove(x, y);
                }
                if (MainForm->animation->paperdoll->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->paperdoll->MouseMove(x, y);
                }
                if (MainForm->animation->book->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->book->MouseMove(x, y);
                }
                if (MainForm->animation->bank->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->bank->MouseMove(x, y);
                }
                if (MainForm->animation->musician->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->musician->MouseMove(x, y);
                }
                if (MainForm->animation->jukebox->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->jukebox->MouseMove(x, y);
                }
                if (MainForm->animation->learn->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->learn->MouseMove(x, y);
                }
                if (MainForm->animation->shop->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->shop->MouseMove(x, y);
                }
                if (MainForm->animation->locker->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->locker->MouseMove(x, y);
                }
                if (MainForm->animation->guild->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->guild->MouseMove(x, y);
                }
                if (MainForm->animation->dialog->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->dialog->MouseMove(x, y);
                }
                if (MainForm->animation->memo->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->memo->MouseMove(x, y);
                }
                if (MainForm->animation->report->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->report->MouseMove(x, y);
                }
                if (MainForm->animation->law->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->law->MouseMove(x, y);
                }
                if (MainForm->animation->inn->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->inn->MouseMove(x, y);
                }
                if (MainForm->animation->chest->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->chest->MouseMove(x, y);
                }
                if (MainForm->animation->barber->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->barber->MouseMove(x, y);
                }
                if (MainForm->animation->stat->is_visible && !window_handled)
                {
                    window_handled = 1;
                    MainForm->animation->stat->MouseMove(x, y);
                }
                if (MainForm->animation->quests->is_visible && !window_handled)
                    MainForm->animation->quests->MouseMove(x, y);
            }
            else
            {
                if (MainForm->gfx->game->view_clip_rect.left <= x && x <= MainForm->gfx->game->view_clip_rect.right &&
                    MainForm->gfx->game->view_clip_rect.top <= y && MainForm->gfx->game->view_clip_rect.bottom >= y)
                {
                    MainForm->players->SetMousePosition(x, y);
                    MainForm->npcs->SetMousePosition(x, y);
                    MainForm->map_items->SetMousePosition(x, y);
                    MainForm->map->UpdateCursorTile(x, y);
                }
            }
            MainForm->animation->interface_controller->OnMouseMove(x, y);
            // In game the sub-state is the interface panel that is open.
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
                    if (MainForm->animation->menu->form_kind == 1)
                    {
                        MainForm->edits->SetPosition(1, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 60);
                        MainForm->edits->SetPosition(5, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 89);
                        MainForm->edits->SetPosition(6, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 119);
                        MainForm->edits->SetPosition(7, MainForm->animation->menu->form_x + 200,
                            MainForm->animation->menu->form_y + 149);
                    }
                    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                    {
                        MainForm->edits->SetPosition(12, MainForm->animation->menu->form_x + 83,
                            MainForm->animation->menu->form_y + 56);
                        MainForm->players->SetPreviewPosition(MainForm->animation->menu->form_x + 275,
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
                    if (MainForm->animation->menu->form_x + 157 <= x && MainForm->animation->menu->form_x + 245 >= x &&
                        MainForm->animation->menu->form_y + 195 <= y && MainForm->animation->menu->form_y + 221 >= y)
                        MainForm->animation->menu->form_ok_button_hover = true;
                    else
                        MainForm->animation->menu->form_ok_button_hover = false;
                    if (MainForm->animation->menu->form_x + 250 <= x && MainForm->animation->menu->form_x + 338 >= x &&
                        MainForm->animation->menu->form_y + 195 <= y && MainForm->animation->menu->form_y + 221 >= y)
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
                    if (x >= 495 && x <= 615 && y >= 93 && y <= 120)
                        MainForm->animation->menu->first_slot_login_hover = true;
                    else
                        MainForm->animation->menu->first_slot_login_hover = false;
                    if (x >= 495 && x <= 615 && y >= 217 && y <= 244)
                        MainForm->animation->menu->second_slot_login_hover = true;
                    else
                        MainForm->animation->menu->second_slot_login_hover = false;
                    if (x >= 495 && x <= 615 && y >= 341 && y <= 368)
                        MainForm->animation->menu->third_slot_login_hover = true;
                    else
                        MainForm->animation->menu->third_slot_login_hover = false;
                    if (x >= 495 && x <= 615 && y >= 121 && y <= 148)
                        MainForm->animation->menu->first_slot_delete_hover = true;
                    else
                        MainForm->animation->menu->first_slot_delete_hover = false;
                    if (x >= 495 && x <= 615 && y >= 245 && y <= 272)
                        MainForm->animation->menu->second_slot_delete_hover = true;
                    else
                        MainForm->animation->menu->second_slot_delete_hover = false;
                    if (x >= 495 && x <= 615 && y >= 369 && y <= 396)
                        MainForm->animation->menu->third_slot_delete_hover = true;
                    else
                        MainForm->animation->menu->third_slot_delete_hover = false;
                }
                if (MainForm->animation->GetSubState() == MenuScreen_Login || MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                    MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
                {
                    // The OK and Cancel buttons of the two login layouts sit in different places.
                    if (MainForm->config->GetSkipSoftpad())
                    {
                        if (x >= 360 && x <= 450 && y >= 388 && y <= 417)
                            MainForm->animation->menu->login_ok_button_hover = true;
                        else
                            MainForm->animation->menu->login_ok_button_hover = false;
                        if (x >= 452 && x <= 542 && y >= 388 && y <= 417)
                            MainForm->animation->menu->login_cancel_button_hover = true;
                        else
                            MainForm->animation->menu->login_cancel_button_hover = false;
                    }
                    else
                    {
                        if (x >= 382 && x <= 472 && y >= 391 && y <= 420)
                            MainForm->animation->menu->login_ok_button_hover = true;
                        else
                            MainForm->animation->menu->login_ok_button_hover = false;
                        if (x >= 474 && x <= 564 && y >= 391 && y <= 420)
                            MainForm->animation->menu->login_cancel_button_hover = true;
                        else
                            MainForm->animation->menu->login_cancel_button_hover = false;
                    }
                }
                // The keys of the password grid's soft keypad light under the mouse.
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
                if (MainForm->animation->GetSubState() == MenuScreen_Main || MainForm->animation->GetSubState() == MenuScreen_Credits ||
                    MainForm->animation->GetSubState() == MenuScreen_Login || MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
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
void Sys_mouse_HandleClick(int *mouse, int x, int y, unsigned int button)
{
    char close_input;
    int skill_id = 0;
    int player_slot = 0;
    int player_x = 0;
    int player_y;
    char npc_clicked;
    int npc_height = 0;
    int npc_y;
    char menu_closed = 0;
    int menu_slot = 0;
    int menu_x = 0;
    int menu_y = 0;
    if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        MainForm->animation->game->ResetIdleTimer();
        if (MainForm->animation->game->message_visible || MainForm->animation->game->large_box_visible ||
            MainForm->animation->game->input_box_visible || MainForm->animation->game->form_visible ||
            MainForm->animation->barber->is_visible || MainForm->animation->guild->is_visible ||
            MainForm->animation->memo->is_visible || MainForm->animation->dialog->is_visible ||
            MainForm->animation->report->is_visible || MainForm->animation->law->is_visible ||
            MainForm->animation->inn->is_visible || MainForm->animation->book->is_visible ||
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
            (MainForm->animation->shop->is_visible &&
            !MainForm->animation->interface_controller->inventory->is_dragging) ||
            MainForm->animation->learn->is_visible || MainForm->animation->stat->is_visible ||
            MainForm->animation->jukebox->is_visible || MainForm->animation->musician->is_visible ||
            MainForm->animation->social->is_open || MainForm->animation->message_board->is_visible ||
            MainForm->animation->quests->is_visible || MainForm->animation->game->amount_box_visible)
        {
            // Only the first open box or window, in this order, takes the click.
            char handled = 0;
            char box_closed = 0;
            if (MainForm->animation->game->message_visible)
            {
                handled = 1;
                box_closed = 0;
                MainForm->animation->game->MouseDown(x, y);
                if (*mouse >= MainForm->animation->game->message_x + 184 &&
                    *mouse <= MainForm->animation->game->message_x + 272 &&
                    mouse[1] >= MainForm->animation->game->message_y + 83 &&
                    mouse[1] <= MainForm->animation->game->message_y + 109)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->game->message_visible = false;
                    MainForm->edits->caret_enabled = true;
                    MainForm->redraw_needed = true;
                    box_closed = 1;
                    if (MainForm->animation->game->message_kind == ConfirmAction_ConnectionLost)
                        MainForm->animation->interface_controller->Logout();
                }
                // The left button answers what the box asked; the box's kind says what that was.
                if (*mouse >= MainForm->animation->game->message_x + 90 &&
                    *mouse <= MainForm->animation->game->message_x + 178 &&
                    mouse[1] >= MainForm->animation->game->message_y + 83 &&
                    mouse[1] <= MainForm->animation->game->message_y + 109)
                {
                    if (MainForm->animation->game->message_kind == ConfirmAction_Logout)
                    {
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = 1;
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->interface_controller->Logout();
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptPartyJoin)
                    {
                        MainForm->net_client->sender->AcceptPartyJoin(MainForm->animation->interface_controller->group->GetRequesterId());
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = 1;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptPartyInvite)
                    {
                        MainForm->net_client->sender->AcceptPartyInvite(MainForm->animation->interface_controller->group->GetRequesterId());
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = 1;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_EnableSound)
                    {
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = 1;
                        try
                        {
                            MainForm->sound_driver_names = new TStringList();
                            MainForm->sound_driver_modules = new TStringList();
                            MainForm->sound_driver_guids = new TList();
                            if (MainForm->Dx_Sound1->Created)
                            {
                                MainForm->Dx_Sound1->Destroy();
                                Sleep(150);
                            }
                            if (MainForm->Dx_Sound1->DSEnumerate(0) && MainForm->sound_driver_names->Count > 0)
                                MainForm->Dx_Sound1->Create((GUID *)MainForm->sound_driver_guids->Items[0]);
                            MainForm->animation->interface_controller->settings->MarkDirty();
                            MainForm->config->SetSoundEnabled(1);
                            MainForm->sfx->SetSoundEnabled(1);
                            MainForm->config->SetSoundAvailable(1);
                            MainForm->sfx->PlaySfx(3);
                        }
                        catch (...)
                        {
                        }
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_EnableMusic)
                    {
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        box_closed = 1;
                        try
                        {
                            MainForm->sfx->CreateMidiPlayer();
                            MainForm->sfx->PlaySfx(3);
                            MainForm->animation->interface_controller->settings->MarkDirty();
                            MainForm->config->SetMusicEnabled(1);
                            MainForm->config->SetMusicAvailable(1);
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
                            box_closed = 1;
                        }
                        else
                        {
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
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
                            MainForm->animation->trade->SetAcceptDelay(TRADE_ACCEPT_DELAY_TICKS);
                            if (MainForm->animation->interface_controller->inventory->CanFitTradeItems())
                            {
                                MainForm->sfx->PlaySfx(17);
                                MainForm->animation->trade->own_agreed = true;
                                MainForm->animation->trade->own_status_text = MainForm->strings->GetText(GameText_Agree);
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                    MainForm->strings->GetText(GameText_YouAcceptDeal));
                                MainForm->net_client->sender->SendTradeAgree();
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
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
                                    box_closed = 1;
                                    MainForm->animation->game->message_visible = false;
                                    MainForm->edits->caret_enabled = true;
                                    MainForm->redraw_needed = true;
                                }
                                else
                                {
                                    String gold_short_text = MainForm->strings->GetDialogText(DialogText_WarningEnough,
                                        true) + " " +
                                        LowerCase(MainForm->item_values->GetName(1));
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                                        gold_short_text);
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetText(GameText_Warning),
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                        else
                        {
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_Warning),
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_SellItem)
                    {
                        MainForm->net_client->sender->SendSellItem(MainForm->animation->shop->npc_session_id,
                            MainForm->animation->shop->selected_item_id, MainForm->animation->shop->selected_amount);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_BuyHairstyle)
                    {
                        MainForm->net_client->sender->SendBuyHairstyle(MainForm->animation->barber->npc_session_id,
                            MainForm->animation->barber->hair_style, MainForm->animation->barber->hair_color);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
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
                        MainForm->sfx->PlaySfx(6);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_CreateGuild)
                    {
                        MainForm->net_client->sender->SendGuildCreate(MainForm->animation->guild->npc_session_id,
                            MainForm->animation->guild->new_guild_tag, MainForm->animation->guild->new_guild_name,
                            MainForm->animation->guild->new_guild_description);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_DisbandGuild)
                    {
                        MainForm->net_client->sender->DisbandGuild(MainForm->animation->guild->npc_session_id);
                        MainForm->sfx->PlaySfx(6);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_UseItem)
                    {
                        MainForm->net_client->sender->SendUseItem(MainForm->animation->interface_controller->inventory->pending_use_item_id);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_JukeboxRequest)
                    {
                        Windows::TPoint jukebox_pos;
                        jukebox_pos.x = 0;
                        jukebox_pos.y = 0;
                        MainForm->net_client->sender->SendJukeboxRequest(jukebox_pos,
                            MainForm->animation->jukebox->selected_song);
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TrainStat)
                    {
                        MainForm->net_client->sender->SendTrainStat(MainForm->animation->interface_controller->status->train_stat_index);
                        MainForm->animation->interface_controller->status->train_confirm_ticks = TRAIN_CONFIRM_TICKS;
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TrainSpell)
                    {
                        MainForm->net_client->sender->SendTrainSkill(MainForm->animation->interface_controller->spells->train_spell_id);
                        MainForm->animation->interface_controller->spells->train_confirm_ticks = TRAIN_CONFIRM_TICKS;
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_TrainSkill)
                    {
                        MainForm->net_client->sender->SendTrainSkill(MainForm->animation->interface_controller->skills->train_skill_id);
                        MainForm->animation->interface_controller->skills->train_confirm_ticks = TRAIN_CONFIRM_TICKS;
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_AcceptWeddingRequest)
                    {
                        MainForm->net_client->sender->AcceptWeddingRequest(MainForm->animation->priest->GetRequestId());
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_ConfirmWeddingVow)
                    {
                        MainForm->net_client->sender->ConfirmWeddingVow(MainForm->animation->priest->GetRequestId());
                        box_closed = 1;
                        MainForm->animation->game->message_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->message_kind == ConfirmAction_LearnSkill)
                    {
                        MainForm->net_client->sender->SendLearnSkill(MainForm->animation->learn->npc_session_id,
                            MainForm->animation->learn->selected_skill_id);
                        box_closed = 1;
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
                }
            }
            if (MainForm->animation->game->large_box_visible && !handled)
            {
                handled = 1;
                box_closed = 0;
                MainForm->animation->game->MouseDown(x, y);
                if (*mouse >= MainForm->animation->game->large_box_x + 184 &&
                    *mouse <= MainForm->animation->game->large_box_x + 272 &&
                    mouse[1] >= MainForm->animation->game->large_box_y + 147 &&
                    mouse[1] <= MainForm->animation->game->large_box_y + 173)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->game->large_box_visible = false;
                    MainForm->edits->caret_enabled = true;
                    MainForm->redraw_needed = true;
                    box_closed = 1;
                }
                if (*mouse >= MainForm->animation->game->large_box_x + 90 &&
                    *mouse <= MainForm->animation->game->large_box_x + 178 &&
                    mouse[1] >= MainForm->animation->game->large_box_y + 147 &&
                    mouse[1] <= MainForm->animation->game->large_box_y + 173)
                {
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_CraftItem)
                    {
                        if (MainForm->animation->interface_controller->status->weight <
                            MainForm->animation->interface_controller->status->max_weight)
                        {
                            if (MainForm->animation->interface_controller->inventory->TestAddItem(MainForm->animation->shop->selected_item_id))
                            {
                                MainForm->net_client->sender->SendCraftItem(MainForm->animation->shop->npc_session_id,
                                    MainForm->animation->shop->selected_item_id);
                                box_closed = 1;
                                MainForm->animation->game->large_box_visible = false;
                                MainForm->edits->caret_enabled = true;
                                MainForm->redraw_needed = true;
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetText(GameText_Warning),
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->edits->caret_enabled = false;
                            }
                        }
                        else
                        {
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetText(GameText_Warning),
                                MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_AcceptGuildInvite)
                    {
                        // The guild window's requests carry the guild master's session.
                        // This answer carries a fixed value in its place.
                        MainForm->net_client->sender->AcceptGuildInvite(20202,
                            MainForm->animation->guild->request_player_id);
                        box_closed = 1;
                        MainForm->animation->game->large_box_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_AcceptGuildJoinRequest)
                    {
                        MainForm->net_client->sender->AcceptGuildJoinRequest(MainForm->animation->guild->request_player_id);
                        box_closed = 1;
                        MainForm->animation->game->large_box_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                    if (MainForm->animation->game->large_box_kind == LargeBoxKind_WeddingRequest)
                    {
                        box_closed = 1;
                        MainForm->animation->game->large_box_visible = false;
                        MainForm->edits->caret_enabled = true;
                        MainForm->animation->game->OpenInputBox(InputBoxKind_WeddingPartner,
                            MainForm->strings->GetText(GameText_EnterNameOfYour), 4, 12);
                    }
                }
            }
            // A message box hides the edit boxes of the window under it; they come back as it closes.
            if (box_closed)
            {
                MainForm->edits->SetVisible(13, 1);
                MainForm->edits->SetVisible(12, 1);
                MainForm->edits->SetVisible(EditId_FormFirstRow, 1);
                MainForm->edits->SetVisible(EditId_FormSecondRow, 1);
                MainForm->edits->SetVisible(16, 1);
                MainForm->edits->SetVisible(17, 1);
                MainForm->edits->SetVisible(18, 1);
                MainForm->edits->SetEditable(POST_SUBJECT_EDIT, 1);
                MainForm->edits->SetEditable(POST_MESSAGE_EDIT, 1);
            }
            if (MainForm->animation->game->input_box_visible && !handled)
            {
                handled = 1;
                MainForm->animation->game->MouseDown(x, y);
                if (*mouse >= MainForm->animation->game->input_box_x + 41 &&
                    *mouse <= MainForm->animation->game->input_box_x + 129 &&
                    mouse[1] >= MainForm->animation->game->input_box_y + 103 &&
                    mouse[1] <= MainForm->animation->game->input_box_y + 129)
                {
                    close_input = 1;
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_AddFriend)
                    {
                        String friend_name = MainForm->edits->GetText(12);
                        if (friend_name.Length() < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                MainForm->strings->GetDialogText(DialogText_WrongName, true));
                            close_input = 0;
                        }
                        else
                        {
                            MainForm->animation->social->AddFriend(MainForm->edits->GetText(12));
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                Client::Capitalize(MainForm->net_client, MainForm->edits->GetText(12)) + " " +
                                MainForm->strings->GetText(GameText_WillBeYourFriend));
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_AddIgnore)
                    {
                        String ignore_name = MainForm->edits->GetText(12);
                        if (ignore_name.Length() < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                MainForm->strings->GetDialogText(DialogText_WrongName, true));
                            close_input = 0;
                        }
                        else
                        {
                            MainForm->animation->social->AddIgnore(MainForm->edits->GetText(12));
                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                Client::Capitalize(MainForm->net_client, MainForm->edits->GetText(12)) + " " +
                                MainForm->strings->GetText(GameText_WillBeIgnordedRemove));
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_GuildInfo)
                    {
                        String guild_query = MainForm->edits->GetText(12);
                        if (guild_query.Length() < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            close_input = 0;
                        }
                        else
                            MainForm->net_client->sender->RequestGuildInfo(MainForm->animation->guild->npc_session_id,
                                MainForm->edits->GetText(12));
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_GuildDescription)
                        MainForm->net_client->sender->SetGuildDescription(MainForm->animation->guild->npc_session_id,
                            MainForm->edits->GetText(12));
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_RemoveGuildMember)
                    {
                        String kick_name = MainForm->edits->GetText(12);
                        if (kick_name.Length() < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                MainForm->strings->GetDialogText(DialogText_WrongName, true));
                            close_input = 0;
                        }
                        else
                        {
                            MainForm->animation->guild->target_member_name = Client::Capitalize(MainForm->net_client,
                                MainForm->edits->GetText(12));
                            MainForm->net_client->sender->RemoveGuildMember(MainForm->animation->guild->npc_session_id,
                                MainForm->edits->GetText(12));
                        }
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_GuildMemberList)
                    {
                        String member_query = MainForm->edits->GetText(12);
                        if (member_query.Length() < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            close_input = 0;
                        }
                        else
                            MainForm->net_client->sender->RequestGuildMemberList(MainForm->animation->guild->npc_session_id,
                                MainForm->edits->GetText(12));
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_WeddingPartner)
                    {
                        String partner_name = MainForm->edits->GetText(12);
                        if (partner_name.Length() > 3)
                            MainForm->net_client->sender->RequestWedding(MainForm->animation->priest->GetRequestId(),
                                partner_name);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_LawOption1)
                    {
                        String law_request_type_a = MainForm->edits->GetText(12);
                        if (law_request_type_a.Length() > 3)
                            MainForm->net_client->sender->SendLawMenuOption1Request(MainForm->animation->law->npc_session_id,
                                law_request_type_a);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_LawOption2)
                    {
                        String law_request_type_b = MainForm->edits->GetText(12);
                        if (law_request_type_b.Length() > 3)
                            MainForm->net_client->sender->SendLawMenuOption2Request(MainForm->animation->law->npc_session_id,
                                law_request_type_b);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_FindSkill)
                    {
                        String skill_name = LowerCase(MainForm->edits->GetText(12));
                        skill_id = MainForm->skill_values->FindByName(skill_name);
                        if (skill_id < 0)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, false),
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, true));
                            close_input = 0;
                        }
                        else if (!MainForm->animation->interface_controller->skills->HasSkill(skill_id) &&
                            !MainForm->animation->interface_controller->spells->HasSpell(skill_id))
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, false),
                                MainForm->strings->GetDialogText(DialogText_CouldNotRemoveSkill, true));
                            close_input = 0;
                        }
                        else
                            MainForm->net_client->sender->SendForgetSkill(MainForm->animation->learn->npc_session_id,
                                skill_id);
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_HelpQuestion)
                    {
                        MainForm->net_client->sender->SendHelpQuestion(MainForm->edits->GetText(12));
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_ThankYou, false),
                            MainForm->strings->GetDialogText(DialogText_ThankYou, true));
                    }
                    if (MainForm->animation->game->input_box_kind == InputBoxKind_InnAnswer)
                    {
                        if (MainForm->edits->GetText(12) != "")
                        {
                            MainForm->animation->inn->answers[MainForm->animation->inn->question_index] = MainForm->edits->GetText(12);
                            MainForm->animation->inn->question_index++;
                            if (MainForm->animation->inn->question_index < 3)
                            {
                                MainForm->animation->game->CloseInputBox();
                                MainForm->sfx->PlaySfx(3);
                                String question_prompt = IntToStr(MainForm->animation->inn->question_index + 1) + ". " +
                                    MainForm->animation->inn->questions[MainForm->animation->inn->question_index];
                                MainForm->animation->game->OpenInputBox(InputBoxKind_InnAnswer, question_prompt, 2, 12);
                                close_input = 0;
                            }
                            else
                                MainForm->net_client->sender->SendCitizenshipAnswers(MainForm->animation->inn->npc_session_id,
                                    MainForm->animation->inn->answers[0], MainForm->animation->inn->answers[1],
                                    MainForm->animation->inn->answers[2]);
                        }
                    }
                    if (close_input)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->game->CloseInputBox();
                    }
                }
                if (*mouse >= MainForm->animation->game->input_box_x + 134 &&
                    *mouse <= MainForm->animation->game->input_box_x + 222 &&
                    mouse[1] >= MainForm->animation->game->input_box_y + 103 &&
                    mouse[1] <= MainForm->animation->game->input_box_y + 129)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->game->CloseInputBox();
                }
            }
            if (MainForm->animation->game->form_visible && !handled)
            {
                handled = 1;
                int form_button_shift = MainForm->animation->game->form_row_count;
                // Fewer than five rows make the form shorter and lift its buttons.
                if (form_button_shift < 5)
                    form_button_shift = INPUT_FORM_ROW_HEIGHT * (5 - form_button_shift);
                else
                    form_button_shift = 0;
                MainForm->animation->game->MouseDown(x, y);
                if (*mouse >= MainForm->animation->game->form_x + 166 &&
                    *mouse <= MainForm->animation->game->form_x + 254 &&
                    mouse[1] >= MainForm->animation->game->form_y + 194 - form_button_shift &&
                    mouse[1] <= MainForm->animation->game->form_y + 220 - form_button_shift)
                {
                    MainForm->edits->SetVisible(13, 1);
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->game->CloseInputForm();
                    MainForm->redraw_needed = true;
                }
                if (*mouse >= MainForm->animation->game->form_x + 73 &&
                    *mouse <= MainForm->animation->game->form_x + 161 &&
                    mouse[1] >= MainForm->animation->game->form_y + 194 - form_button_shift &&
                    mouse[1] <= MainForm->animation->game->form_y + 220 - form_button_shift)
                {
                    MainForm->animation->game->SaveInputFormRows();
                    if (MainForm->animation->game->form_kind == FormKind_GuildCreate)
                    {
                        char create_failed = 0;
                        String guild_tag = MainForm->edits->GetText(EditId_FormFirstRow);
                        String guild_name = MainForm->edits->GetText(EditId_FormSecondRow);
                        if (!create_failed && guild_tag == "")
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, true));
                            create_failed = 1;
                        }
                        if (!create_failed && guild_name == "")
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildNameField, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildNameField, true));
                            create_failed = 1;
                        }
                        if (!create_failed && MainForm->edits->GetTextLength(EditId_FormFirstRow) < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            create_failed = 1;
                        }
                        if (!create_failed && MainForm->edits->GetTextLength(EditId_FormSecondRow) < 4)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildShort, true));
                            create_failed = 1;
                        }
                        if (!create_failed && LowerCase(String(guild_tag[1])) != LowerCase(String(guild_name[1])))
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_InvalidTag, false),
                                MainForm->strings->GetDialogText(DialogText_InvalidTag, true));
                            create_failed = 1;
                        }
                        if (!create_failed && MainForm->animation->interface_controller->status->gold < 50000)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                                MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                                MainForm->item_values->GetName(1));
                            create_failed = 1;
                        }
                        if (!create_failed)
                        {
                            MainForm->animation->guild->new_guild_tag = MainForm->edits->GetText(EditId_FormFirstRow);
                            MainForm->animation->guild->new_guild_name = MainForm->edits->GetText(EditId_FormSecondRow);
                            MainForm->animation->guild->new_guild_description = MainForm->edits->GetText(16);
                            MainForm->net_client->sender->RequestGuildCreate(MainForm->animation->guild->npc_session_id,
                                MainForm->animation->guild->new_guild_tag, MainForm->animation->guild->new_guild_name);
                        }
                    }
                    if (MainForm->animation->game->form_kind == FormKind_GuildJoin)
                    {
                        char join_failed = 0;
                        String join_tag = MainForm->edits->GetText(EditId_FormFirstRow);
                        String join_detail = MainForm->edits->GetText(EditId_FormSecondRow);
                        if (!join_failed && join_tag == "")
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputGuildTag, true));
                            join_failed = 1;
                        }
                        if (!join_failed && join_detail == "")
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputRecruiter, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputRecruiter, true));
                            join_failed = 1;
                        }
                        if (!join_failed && MainForm->edits->GetTextLength(EditId_FormFirstRow) < 2)
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputTagShort, true));
                            join_failed = 1;
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
                        char rank_failed = 0;
                        String member_name = MainForm->edits->GetText(EditId_FormFirstRow);
                        String rank_text = MainForm->edits->GetText(EditId_FormSecondRow);
                        if (!rank_failed && (member_name == "" || rank_text == ""))
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_WrongInputFields, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputFields, true));
                            rank_failed = 1;
                        }
                        if (rank_text == "0")
                            rank_text = "9";
                        if (!rank_failed)
                        {
                            MainForm->animation->guild->target_member_name = Client::Capitalize(MainForm->net_client,
                                member_name);
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
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_ThankYou, false),
                            MainForm->strings->GetDialogText(DialogText_ThankYou, true));
                    }
                }
            }
            if (MainForm->animation->social->is_open && !handled)
            {
                handled = 1;
                MainForm->animation->social->OnMouseClick(x, y, button);
                if (*mouse >= MainForm->animation->social->window_x + 48 &&
                    *mouse <= MainForm->animation->social->window_x + 138 &&
                    mouse[1] >= MainForm->animation->social->window_y + 253 &&
                    mouse[1] <= MainForm->animation->social->window_y + 279)
                {
                    MainForm->animation->social->close_button_hover = false;
                    MainForm->animation->social->add_button_hover = false;
                    MainForm->sfx->PlaySfx(3);
                    if (!MainForm->animation->social->list_kind)
                        MainForm->animation->game->OpenInputBox(InputBoxKind_AddFriend,
                            MainForm->strings->GetText(GameText_WhoDoYouWantTo), 4, 12);
                    if (MainForm->animation->social->list_kind == 1)
                        MainForm->animation->game->OpenInputBox(InputBoxKind_AddIgnore,
                            MainForm->strings->GetText(GameText_WhoDoYouWant), 4, 12);
                }
                if (*mouse >= MainForm->animation->social->window_x + 144 &&
                    *mouse <= MainForm->animation->social->window_x + 234 &&
                    mouse[1] >= MainForm->animation->social->window_y + 253 &&
                    mouse[1] <= MainForm->animation->social->window_y + 279)
                {
                    MainForm->edits->SetVisible(13, 1);
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->social->CloseWindow();
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->message_board->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->message_board->MouseDown(x, y, button);
                if (*mouse >= MainForm->animation->message_board->window_x + 284 &&
                    *mouse <= MainForm->animation->message_board->window_x + 374 &&
                    mouse[1] >= MainForm->animation->message_board->window_y + 253 &&
                    mouse[1] <= MainForm->animation->message_board->window_y + 279)
                {
                    char board_handled = 0;
                    if (MainForm->animation->message_board->view_mode <= 0 && !board_handled)
                    {
                        board_handled = 1;
                        String own_name = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                        if (MainForm->animation->message_board->CountPostsByAuthor(own_name) < 2)
                        {
                            MainForm->sfx->PlaySfx(2);
                            MainForm->animation->message_board->CreatePostEdits();
                            MainForm->edits->SetText(20, "");
                            MainForm->edits->SetText(21, "");
                            MainForm->edits->FocusEdit(20);
                            MainForm->animation->message_board->current_post_id = -1;
                            MainForm->animation->message_board->current_post_author = MainForm->strings->GetText(GameText_PostingNewMessage);
                            MainForm->animation->message_board->current_post_subject = "";
                            MainForm->animation->message_board->current_post_body = "";
                            MainForm->animation->message_board->is_composing = true;
                            MainForm->animation->message_board->view_mode = MSGBOARD_MODE_WRITE;
                            MainForm->redraw_needed = true;
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_TooManyMessages, false),
                                MainForm->strings->GetDialogText(DialogText_TooManyMessages, true));
                            MainForm->edits->caret_enabled = false;
                        }
                    }
                    if (MainForm->animation->message_board->view_mode == 1 && !board_handled)
                    {
                        board_handled = 1;
                        if (MainForm->net_client->GetAdminLevel() > 0)
                        {
                            MainForm->sfx->PlaySfx(2);
                            MainForm->edits->SetEditable(POST_SUBJECT_EDIT, 0);
                            MainForm->edits->SetEditable(POST_MESSAGE_EDIT, 0);
                            MainForm->animation->message_board->DeletePost(MainForm->animation->message_board->current_post_id);
                        }
                    }
                    if (MainForm->animation->message_board->view_mode == MSGBOARD_MODE_WRITE && !board_handled)
                    {
                        board_handled = 1;
                        if (MainForm->animation->message_board->is_composing)
                        {
                            String post_subject = MainForm->edits->GetText(20);
                            String post_message = MainForm->edits->GetText(21);
                            if (!post_subject.Length())
                            {
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_NoSubject, false),
                                    MainForm->strings->GetDialogText(DialogText_NoSubject, true));
                                MainForm->edits->caret_enabled = false;
                            }
                            if (!post_message.Length())
                            {
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_NoMessage, false),
                                    MainForm->strings->GetDialogText(DialogText_NoMessage, true));
                                MainForm->edits->caret_enabled = false;
                            }
                            if (post_subject.Length() > 0)
                            {
                                if (post_message.Length() > 0)
                                {
                                    MainForm->sfx->PlaySfx(2);
                                    MainForm->edits->SetEditable(POST_SUBJECT_EDIT, 0);
                                    MainForm->edits->SetEditable(POST_MESSAGE_EDIT, 0);
                                    MainForm->animation->message_board->SubmitPost(post_subject, post_message);
                                }
                            }
                        }
                    }
                }
                if (*mouse >= MainForm->animation->message_board->window_x + 380 &&
                    *mouse <= MainForm->animation->message_board->window_x + 470 &&
                    mouse[1] >= MainForm->animation->message_board->window_y + 253 &&
                    mouse[1] <= MainForm->animation->message_board->window_y + 279)
                {
                    if (MainForm->animation->message_board->view_mode <= 0)
                    {
                        MainForm->edits->SetVisible(13, 1);
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->message_board->Close();
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->animation->message_board->ResetScroll();
                        MainForm->animation->message_board->view_mode = 0;
                        MainForm->animation->message_board->HidePostEdits();
                        MainForm->sfx->PlaySfx(3);
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->game->amount_box_visible && !handled)
            {
                handled = 1;
                MainForm->animation->game->MouseDown(x, y);
                if (*mouse >= MainForm->animation->game->amount_box_x + 60 &&
                    *mouse <= MainForm->animation->game->amount_box_x + 148 &&
                    mouse[1] >= MainForm->animation->game->amount_box_y + 125 &&
                    mouse[1] <= MainForm->animation->game->amount_box_y + 151)
                {
                    if (MainForm->edits->GetText(13).Length() > 0)
                    {
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_DropItem)
                        {
                            int drop_amount = StrToInt(MainForm->edits->GetText(13));
                            if (drop_amount > 0)
                                MainForm->net_client->sender->SendDropItem(MainForm->animation->game->amount_item_id,
                                    drop_amount, MainForm->animation->game->drop_x, MainForm->animation->game->drop_y);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_JunkItem)
                        {
                            int junk_amount = StrToInt(MainForm->edits->GetText(13));
                            if (junk_amount > 0)
                                MainForm->net_client->sender->SendJunkItem(MainForm->animation->game->amount_item_id,
                                    junk_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_TradeAddItem)
                        {
                            int trade_amount = StrToInt(MainForm->edits->GetText(13));
                            if (trade_amount > 0 && !MainForm->animation->trade->trade_locked)
                                MainForm->net_client->sender->SendTradeAddItem(MainForm->animation->game->amount_item_id,
                                    trade_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ChestAddItem)
                        {
                            int chest_amount = StrToInt(MainForm->edits->GetText(13));
                            if (chest_amount > 0)
                            {
                                Windows::TPoint chest_pos;
                                chest_pos.x = MainForm->animation->chest->chest_x;
                                chest_pos.y = MainForm->animation->chest->chest_y;
                                MainForm->net_client->sender->SendChestAddItem(chest_pos,
                                    MainForm->animation->game->amount_item_id, chest_amount);
                            }
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_BankDeposit)
                        {
                            int deposit_amount = StrToInt(MainForm->edits->GetText(13));
                            MainForm->net_client->sender->SendBankDeposit(MainForm->animation->bank->npc_session_id,
                                deposit_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_BankWithdraw)
                        {
                            int withdraw_amount = StrToInt(MainForm->edits->GetText(13));
                            MainForm->net_client->sender->SendBankWithdraw(MainForm->animation->bank->npc_session_id,
                                withdraw_amount);
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ShopBuy)
                        {
                            MainForm->animation->shop->selected_amount = StrToInt(MainForm->edits->GetText(13));
                            String buy_question = MainForm->strings->GetText(GameText_Buy);
                            buy_question = buy_question + " ";
                            buy_question = buy_question + MainForm->edits->GetText(13);
                            buy_question = buy_question + " ";
                            buy_question = buy_question +
                                MainForm->item_values->GetName(MainForm->animation->shop->selected_item_id);
                            buy_question = buy_question + " ";
                            buy_question = buy_question + MainForm->strings->GetText(GameText_For);
                            buy_question = buy_question + " ";
                            buy_question = buy_question +
                                IntToStr(MainForm->animation->shop->selected_price *
                                StrToInt(MainForm->edits->GetText(13)));
                            buy_question = buy_question + " ";
                            buy_question = buy_question + LowerCase(MainForm->item_values->GetName(1));
                            buy_question = buy_question + " ?";
                            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_BuyItem,
                                MainForm->strings->GetText(GameText_BuyItemS), buy_question);
                            MainForm->edits->caret_enabled = false;
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_ShopSell)
                        {
                            MainForm->animation->shop->selected_amount = StrToInt(MainForm->edits->GetText(13));
                            String sell_question = MainForm->strings->GetText(GameText_Sell);
                            sell_question = sell_question + " ";
                            sell_question = sell_question + MainForm->edits->GetText(13);
                            sell_question = sell_question + " ";
                            sell_question = sell_question +
                                MainForm->item_values->GetName(MainForm->animation->shop->selected_item_id);
                            sell_question = sell_question + " ";
                            sell_question = sell_question + MainForm->strings->GetText(GameText_For);
                            sell_question = sell_question + " ";
                            sell_question = sell_question +
                                IntToStr(MainForm->animation->shop->selected_price *
                                StrToInt(MainForm->edits->GetText(13)));
                            sell_question = sell_question + " ";
                            sell_question = sell_question + LowerCase(MainForm->item_values->GetName(1));
                            sell_question = sell_question + " ?";
                            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_SellItem,
                                MainForm->strings->GetText(GameText_SellItemsS), sell_question);
                            MainForm->edits->caret_enabled = false;
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_LockerAddItem)
                        {
                            int locker_amount = StrToInt(MainForm->edits->GetText(13));
                            if (locker_amount > 0)
                            {
                                Windows::TPoint locker_pos;
                                locker_pos.x = MainForm->animation->locker->locker_x;
                                locker_pos.y = MainForm->animation->locker->locker_y;
                                int locker_total = locker_amount +
                                    MainForm->animation->locker->GetItemAmount(MainForm->animation->game->amount_item_id);
                                if (locker_total <= 200)
                                {
                                    if (MainForm->animation->locker->CanStoreItem(MainForm->animation->game->amount_item_id))
                                    {
                                        MainForm->net_client->sender->SendLockerAddItem(locker_pos,
                                            MainForm->animation->game->amount_item_id, locker_amount);
                                    }
                                    else
                                    {
                                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                            MainForm->strings->GetDialogText(DialogText_LockerFullHold, false),
                                            MainForm->strings->GetDialogText(DialogText_LockerFullHold, true));
                                        MainForm->edits->caret_enabled = false;
                                    }
                                }
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LockerFullStore, false),
                                        MainForm->strings->GetDialogText(DialogText_LockerFullStore, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                        }
                        if (MainForm->animation->game->amount_box_kind == AmountBoxKind_GuildDeposit)
                        {
                            int guild_deposit = StrToInt(MainForm->edits->GetText(13));
                            if (guild_deposit < 1000)
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                    MainForm->strings->GetDialogText(DialogText_DepositMinimum, false),
                                    MainForm->strings->GetDialogText(DialogText_DepositMinimum, true) + " " +
                                    MainForm->item_values->GetName(1));
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
                        MainForm->edits->RemoveEdit(13);
                        MainForm->edits->FocusEdit(EditId_Chat);
                        MainForm->edits->caret_enabled = true;
                        MainForm->redraw_needed = true;
                    }
                }
                if (*mouse >= MainForm->animation->game->amount_box_x + 153 &&
                    *mouse <= MainForm->animation->game->amount_box_x + 241 &&
                    mouse[1] >= MainForm->animation->game->amount_box_y + 125 &&
                    mouse[1] <= MainForm->animation->game->amount_box_y + 151)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                    MainForm->animation->interface_controller->inventory->hover_col = -1;
                    MainForm->animation->interface_controller->inventory->hover_row = -1;
                    MainForm->animation->interface_controller->inventory->MarkItemsDirty();
                    MainForm->animation->interface_controller->inventory->MarkHoverDirty();
                    MainForm->animation->game->amount_box_visible = false;
                    MainForm->edits->RemoveEdit(13);
                    MainForm->edits->FocusEdit(EditId_Chat);
                    MainForm->edits->caret_enabled = true;
                    MainForm->redraw_needed = true;
                }
            }
            if (MainForm->animation->trade->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->trade->MouseDown(x, y, button);
                if (*mouse >= MainForm->animation->trade->window_x + 357 &&
                    *mouse <= MainForm->animation->trade->window_x + 445 &&
                    mouse[1] >= MainForm->animation->trade->window_y + 253 &&
                    mouse[1] <= MainForm->animation->trade->window_y + 280)
                {
                    if (MainForm->animation->trade->partner_items.size() > 0 &&
                        MainForm->animation->trade->own_items.size() > 0)
                    {
                        if (!MainForm->animation->trade->trade_locked && !MainForm->animation->trade->IsAcceptDelayed())
                        {
                            if (MainForm->animation->interface_controller->inventory->CanFitTradeItems())
                            {
                                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_TradeAgree,
                                    MainForm->strings->GetDialogText(DialogText_TradeAgree, false),
                                    MainForm->strings->GetDialogText(DialogText_TradeAgree, true));
                                MainForm->edits->caret_enabled = false;
                            }
                            else
                            {
                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                    MainForm->strings->GetText(GameText_YouHaveNotEnough));
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
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
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetText(GameText_Warning),
                            MainForm->strings->GetText(GameText_BothPlayersNeedTo));
                        MainForm->edits->caret_enabled = false;
                    }
                }
                if (*mouse >= MainForm->animation->trade->window_x + 450 &&
                    *mouse <= MainForm->animation->trade->window_x + 538 &&
                    mouse[1] >= MainForm->animation->trade->window_y + 253 &&
                    mouse[1] <= MainForm->animation->trade->window_y + 280)
                {
                    if (!MainForm->animation->trade->own_agreed)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->trade->Close();
                        MainForm->net_client->sender->SendTradeClose();
                        MainForm->animation->trade->trade_locked = false;
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                            MainForm->strings->GetText(GameText_TradeHasBeenAborted));
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->sfx->PlaySfx(16);
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
            if (MainForm->animation->paperdoll->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->paperdoll->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->paperdoll->window_x + 277 &&
                        *mouse <= MainForm->animation->paperdoll->window_x + 367 &&
                        mouse[1] >= MainForm->animation->paperdoll->window_y + 254 &&
                        mouse[1] <= MainForm->animation->paperdoll->window_y + 281)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->paperdoll->is_visible = 0;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->book->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->book->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->book->window_x + 277 &&
                        *mouse <= MainForm->animation->book->window_x + 367 &&
                        mouse[1] >= MainForm->animation->book->window_y + 254 &&
                        mouse[1] <= MainForm->animation->book->window_y + 281)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->book->is_visible = 0;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->bank->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->bank->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->bank->window_x + 93 &&
                        *mouse <= MainForm->animation->bank->window_x + 181 &&
                        mouse[1] >= MainForm->animation->bank->window_y + 192 &&
                        mouse[1] <= MainForm->animation->bank->window_y + 218)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->bank->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->musician->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->musician->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->musician->window_x + 93 &&
                        *mouse <= MainForm->animation->musician->window_x + 181 &&
                        mouse[1] >= MainForm->animation->musician->window_y + 82 &&
                        mouse[1] <= MainForm->animation->musician->window_y + 108)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->musician->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->jukebox->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->jukebox->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->jukebox->window_x + 93 &&
                        *mouse <= MainForm->animation->jukebox->window_x + 181 &&
                        mouse[1] >= MainForm->animation->jukebox->window_y + 155 &&
                        mouse[1] <= MainForm->animation->jukebox->window_y + 181)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->jukebox->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->learn->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->learn->MouseDown(x, y, button);
                if (MainForm->animation->learn->page == LearnPage_Menu)
                {
                    if (*mouse >= MainForm->animation->learn->window_x + 92 &&
                        *mouse <= MainForm->animation->learn->window_x + 182 &&
                        mouse[1] >= MainForm->animation->learn->window_y + 253 &&
                        mouse[1] <= MainForm->animation->learn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->learn->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (*mouse >= MainForm->animation->learn->window_x + 144 &&
                        *mouse <= MainForm->animation->learn->window_x + 234 &&
                        mouse[1] >= MainForm->animation->learn->window_y + 253 &&
                        mouse[1] <= MainForm->animation->learn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->learn->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (*mouse >= MainForm->animation->learn->window_x + 48 &&
                        *mouse <= MainForm->animation->learn->window_x + 138 &&
                        mouse[1] >= MainForm->animation->learn->window_y + 253 &&
                        mouse[1] <= MainForm->animation->learn->window_y + 279)
                    {
                        if (MainForm->animation->learn->page == LearnPage_SkillList || MainForm->animation->learn->page == LearnPage_Reset)
                        {
                            LearnController *learn = MainForm->animation->learn;
                            learn->shown_skills.clear();
                            MainForm->animation->learn->ClearText();
                            MainForm->animation->learn->page = LearnPage_Menu;
                        }
                        if (MainForm->animation->learn->page == LearnPage_SkillInfo)
                            MainForm->animation->learn->page = LearnPage_SkillList;
                        MainForm->sfx->PlaySfx(3);
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->shop->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->shop->MouseDown(x, y, button);
                if (!MainForm->animation->shop->page)
                {
                    if (*mouse >= MainForm->animation->shop->window_x + 92 &&
                        *mouse <= MainForm->animation->shop->window_x + 182 &&
                        mouse[1] >= MainForm->animation->shop->window_y + 253 &&
                        mouse[1] <= MainForm->animation->shop->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->shop->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (*mouse >= MainForm->animation->shop->window_x + 144 &&
                        *mouse <= MainForm->animation->shop->window_x + 234 &&
                        mouse[1] >= MainForm->animation->shop->window_y + 253 &&
                        mouse[1] <= MainForm->animation->shop->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->shop->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (*mouse >= MainForm->animation->shop->window_x + 48 &&
                        *mouse <= MainForm->animation->shop->window_x + 138 &&
                        mouse[1] >= MainForm->animation->shop->window_y + 253 &&
                        mouse[1] <= MainForm->animation->shop->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        ShopController *shop = MainForm->animation->shop;
                        shop->shown_items.clear();
                        MainForm->animation->shop->ResetScroll();
                        MainForm->animation->shop->page = 0;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->locker->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->locker->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->locker->window_x + 92 &&
                        *mouse <= MainForm->animation->locker->window_x + 182 &&
                        mouse[1] >= MainForm->animation->locker->window_y + 253 &&
                        mouse[1] <= MainForm->animation->locker->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->locker->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->guild->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->guild->MouseDown(x, y, button);
                if (MainForm->animation->guild->page == GuildPage_Main)
                {
                    if (*mouse >= MainForm->animation->guild->window_x + 92 &&
                        *mouse <= MainForm->animation->guild->window_x + 182 &&
                        mouse[1] >= MainForm->animation->guild->window_y + 253 &&
                        mouse[1] <= MainForm->animation->guild->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->guild->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (*mouse >= MainForm->animation->guild->window_x + 144 &&
                        *mouse <= MainForm->animation->guild->window_x + 234 &&
                        mouse[1] >= MainForm->animation->guild->window_y + 253 &&
                        mouse[1] <= MainForm->animation->guild->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->guild->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (*mouse >= MainForm->animation->guild->window_x + 48 &&
                        *mouse <= MainForm->animation->guild->window_x + 138 &&
                        mouse[1] >= MainForm->animation->guild->window_y + 253 &&
                        mouse[1] <= MainForm->animation->guild->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->guild->ClearText();
                        if (MainForm->animation->guild->page <= GuildPage_Lookup)
                            MainForm->animation->guild->page = GuildPage_Main;
                        if (MainForm->animation->guild->page == GuildPage_Join || MainForm->animation->guild->page == GuildPage_Leave ||
                            MainForm->animation->guild->page == GuildPage_Create)
                            MainForm->animation->guild->page = GuildPage_Membership;
                        if (MainForm->animation->guild->page == GuildPage_Info || MainForm->animation->guild->page == GuildPage_MemberList)
                            MainForm->animation->guild->page = GuildPage_Lookup;
                        if (MainForm->animation->guild->page == GuildPage_Bank)
                            MainForm->animation->guild->page = GuildPage_Main;
                        if (MainForm->animation->guild->page == GuildPage_Description || MainForm->animation->guild->page == GuildPage_RemoveMember ||
                            MainForm->animation->guild->page == GuildPage_Disband || MainForm->animation->guild->page == GuildPage_Rank)
                            MainForm->animation->guild->page = GuildPage_Manage;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->dialog->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->dialog->MouseDown(x, y, button);
                if (*mouse >= MainForm->animation->dialog->window_x + 89 &&
                    *mouse <= MainForm->animation->dialog->window_x + 169 &&
                    mouse[1] >= MainForm->animation->dialog->window_y + 153 &&
                    mouse[1] <= MainForm->animation->dialog->window_y + 179)
                {
                    if (MainForm->animation->dialog->prev_button_kind == 1)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->dialog->Close();
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->sfx->PlaySfx(4);
                        MainForm->animation->dialog->PreviousPage();
                        MainForm->redraw_needed = true;
                    }
                }
                if (*mouse >= MainForm->animation->dialog->window_x + 183 &&
                    *mouse <= MainForm->animation->dialog->window_x + 273 &&
                    mouse[1] >= MainForm->animation->dialog->window_y + 153 &&
                    mouse[1] <= MainForm->animation->dialog->window_y + 179)
                {
                    if (MainForm->animation->dialog->showing_quest_list)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->dialog->Close();
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        if (MainForm->animation->dialog->next_button_kind == DIALOG_BUTTON_OK_FRAME)
                        {
                            MainForm->sfx->PlaySfx(3);
                            MainForm->net_client->sender->SendQuestDialogOk(MainForm->animation->dialog->dialog_echo_a,
                                MainForm->animation->dialog->quest_id, MainForm->animation->dialog->dialog_echo_b);
                            MainForm->animation->dialog->Close();
                            MainForm->redraw_needed = true;
                        }
                        else
                        {
                            MainForm->sfx->PlaySfx(4);
                            MainForm->animation->dialog->NextPage();
                            MainForm->redraw_needed = true;
                        }
                    }
                }
            }
            if (MainForm->animation->memo->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->memo->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->memo->window_x + 92 &&
                        *mouse <= MainForm->animation->memo->window_x + 182 &&
                        mouse[1] >= MainForm->animation->memo->window_y + 253 &&
                        mouse[1] <= MainForm->animation->memo->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->memo->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->report->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->report->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->report->window_x + 92 &&
                        *mouse <= MainForm->animation->report->window_x + 182 &&
                        mouse[1] >= MainForm->animation->report->window_y + 253 &&
                        mouse[1] <= MainForm->animation->report->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->report->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->law->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->law->MouseDown(x, y, button);
                if (!MainForm->animation->law->page)
                {
                    if (*mouse >= MainForm->animation->law->window_x + 92 &&
                        *mouse <= MainForm->animation->law->window_x + 182 &&
                        mouse[1] >= MainForm->animation->law->window_y + 253 &&
                        mouse[1] <= MainForm->animation->law->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->law->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (*mouse >= MainForm->animation->law->window_x + 144 &&
                        *mouse <= MainForm->animation->law->window_x + 234 &&
                        mouse[1] >= MainForm->animation->law->window_y + 253 &&
                        mouse[1] <= MainForm->animation->law->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->law->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (*mouse >= MainForm->animation->law->window_x + 48 &&
                        *mouse <= MainForm->animation->law->window_x + 138 &&
                        mouse[1] >= MainForm->animation->law->window_y + 253 &&
                        mouse[1] <= MainForm->animation->law->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
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
                handled = 1;
                MainForm->animation->inn->MouseDown(x, y, button);
                if (!MainForm->animation->inn->page)
                {
                    if (*mouse >= MainForm->animation->inn->window_x + 92 &&
                        *mouse <= MainForm->animation->inn->window_x + 182 &&
                        mouse[1] >= MainForm->animation->inn->window_y + 253 &&
                        mouse[1] <= MainForm->animation->inn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->inn->Close();
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    if (*mouse >= MainForm->animation->inn->window_x + 144 &&
                        *mouse <= MainForm->animation->inn->window_x + 234 &&
                        mouse[1] >= MainForm->animation->inn->window_y + 253 &&
                        mouse[1] <= MainForm->animation->inn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->inn->Close();
                        MainForm->redraw_needed = true;
                    }
                    if (*mouse >= MainForm->animation->inn->window_x + 48 &&
                        *mouse <= MainForm->animation->inn->window_x + 138 &&
                        mouse[1] >= MainForm->animation->inn->window_y + 253 &&
                        mouse[1] <= MainForm->animation->inn->window_y + 279)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->inn->ClearText();
                        if (MainForm->animation->inn->page == 1)
                            MainForm->animation->inn->page = 0;
                        if (MainForm->animation->inn->page == 6 || MainForm->animation->inn->page == 7)
                            MainForm->animation->inn->page = 1;
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->chest->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->chest->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->chest->window_x + 92 &&
                        *mouse <= MainForm->animation->chest->window_x + 182 &&
                        mouse[1] >= MainForm->animation->chest->window_y + 231 &&
                        mouse[1] <= MainForm->animation->chest->window_y + 254)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->chest->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->barber->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->barber->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->barber->window_x + 215 &&
                        *mouse <= MainForm->animation->barber->window_x + 304 &&
                        mouse[1] >= MainForm->animation->barber->window_y + 151 &&
                        mouse[1] <= MainForm->animation->barber->window_y + 178)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->barber->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->stat->is_visible)
            {
                if (!handled)
                {
                    handled = 1;
                    MainForm->animation->stat->MouseDown(x, y, button);
                    if (*mouse >= MainForm->animation->stat->window_x + 92 &&
                        *mouse <= MainForm->animation->stat->window_x + 182 &&
                        mouse[1] >= MainForm->animation->stat->window_y + 213 &&
                        mouse[1] <= MainForm->animation->stat->window_y + 239)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->stat->Close();
                        MainForm->redraw_needed = true;
                    }
                }
            }
            if (MainForm->animation->quests->is_visible && !handled)
            {
                handled = 1;
                MainForm->animation->quests->MouseDown(x, y, button);
                if (*mouse >= MainForm->animation->quests->window_x + 288 &&
                    *mouse <= MainForm->animation->quests->window_x + 378 &&
                    mouse[1] >= MainForm->animation->quests->window_y + 253 &&
                    mouse[1] <= MainForm->animation->quests->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(3);
                    if (!MainForm->animation->quests->history_requested)
                    {
                        MainForm->animation->quests->history_requested = true;
                        MainForm->net_client->sender->RequestQuestHistory();
                    }
                    if (!MainForm->animation->quests->showing_history)
                    {
                        String second_page_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                        second_page_title = Client::Capitalize(MainForm->net_client, second_page_title);
                        second_page_title = second_page_title + "'s " +
                            MainForm->strings->GetText(GameText_QuestHistory);
                        MainForm->animation->quests->title = second_page_title;
                        MainForm->animation->quests->ResetScroll();
                        MainForm->animation->quests->showing_history = 1;
                    }
                    else
                    {
                        String first_page_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                        first_page_title = Client::Capitalize(MainForm->net_client, first_page_title);
                        first_page_title = first_page_title + "'s " +
                            MainForm->strings->GetText(GameText_QuestProgress);
                        MainForm->animation->quests->title = first_page_title;
                        MainForm->animation->quests->ResetScroll();
                        MainForm->animation->quests->showing_history = 0;
                    }
                    MainForm->redraw_needed = true;
                }
                if (*mouse >= MainForm->animation->quests->window_x + 380 &&
                    *mouse <= MainForm->animation->quests->window_x + 470 &&
                    mouse[1] >= MainForm->animation->quests->window_y + 253 &&
                    mouse[1] <= MainForm->animation->quests->window_y + 279)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->quests->Close();
                    MainForm->redraw_needed = true;
                }
            }
        }
        else
        {
            // No window is open: the corner close button, the bars along the top, then the map.
            char bar_clicked = 0;
            if ((y > 10 && y < 17 && x > 590 && 3 * y + 560 > x) || (y > 16 && y < 31 && x > 590 && y + 594 > x))
            {
                bar_clicked = 1;
                // During a warp or a map download the close button logs out without asking.
                if (MainForm->map->warp_pending || MainForm->map->loading_map)
                    MainForm->animation->interface_controller->Logout();
                else
                {
                    MainForm->animation->game->ShowMessageBox(3, ConfirmAction_Logout,
                        MainForm->strings->GetDialogText(DialogText_ExitGame, false),
                        MainForm->strings->GetDialogText(DialogText_ExitGame, true));
                    MainForm->edits->caret_enabled = false;
                }
            }
            if (x >= 55 && x <= 542 && y <= 10)
            {
                bar_clicked = 1;
                MainForm->animation->interface_controller->game_bars->MouseDown(x, y, button);
            }
            if (MainForm->gfx->game->view_clip_rect.top < y && MainForm->gfx->game->view_clip_rect.bottom > y &&
                MainForm->gfx->game->view_clip_rect.left < x && MainForm->gfx->game->view_clip_rect.right > x &&
                !bar_clicked)
            {
                // A dragged item goes to the open window under it, or onto the map within reach.
                if (MainForm->animation->interface_controller->inventory->is_dragging)
                {
                    if (!MainForm->animation->interface_controller->inventory->dragging_from_paperdoll && !button)
                    {
                        char drop_handled = 0;
                        if (MainForm->animation->trade->is_visible)
                        {
                            drop_handled = 1;
                            if (MainForm->animation->trade->window_y + 43 < y &&
                                MainForm->animation->trade->window_y + 243 > y &&
                                MainForm->animation->trade->window_x + 280 < x &&
                                MainForm->animation->trade->window_x + 515 > x &&
                                !MainForm->animation->game->message_visible &&
                                !MainForm->animation->trade->trade_locked)
                            {
                                if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                                    ItemSpecial_Bound)
                                {
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                                else
                                {
                                    if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
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
                        }
                        if (MainForm->animation->paperdoll->is_visible)
                        {
                            drop_handled = 1;
                            MainForm->animation->paperdoll->EquipDroppedItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                x, y);
                        }
                        if (MainForm->animation->bank->is_visible)
                        {
                            drop_handled = 1;
                            if (MainForm->animation->bank->window_y + 50 < y &&
                                MainForm->animation->bank->window_y + 170 > y &&
                                MainForm->animation->bank->window_x + 17 < x &&
                                MainForm->animation->bank->window_x + 252 > x &&
                                MainForm->animation->interface_controller->inventory->drag_item_id == 1)
                            {
                                int gold_amount = MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                                if (gold_amount > 0)
                                {
                                    if (gold_amount == 1)
                                        MainForm->net_client->sender->SendBankDeposit(MainForm->animation->bank->npc_session_id,
                                            1);
                                    else
                                    {
                                        MainForm->animation->game->OpenAmountBox(AmountBoxKind_BankDeposit, gold_amount, 1, 0);
                                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                                    }
                                }
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, false),
                                        MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                        MainForm->strings->GetDialogText(DialogText_CouldNotDeposit, true));
                                }
                            }
                        }
                        if (MainForm->animation->locker->is_visible)
                        {
                            drop_handled = 1;
                            if (MainForm->animation->locker->window_y + 17 < y &&
                                MainForm->animation->locker->window_y + 217 > y &&
                                MainForm->animation->locker->window_x + 17 < x &&
                                MainForm->animation->locker->window_x + 252 > x)
                            {
                                if (MainForm->animation->interface_controller->inventory->drag_item_id > 1)
                                {
                                    if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                        2)
                                    {
                                        Windows::TPoint drop_locker_pos;
                                        drop_locker_pos.x = MainForm->animation->locker->locker_x;
                                        drop_locker_pos.y = MainForm->animation->locker->locker_y;
                                        if (MainForm->animation->locker->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                            WRAP_TEXT_WIDTH)
                                        {
                                            if (MainForm->animation->locker->CanStoreItem(MainForm->animation->interface_controller->inventory->drag_item_id))
                                            {
                                                MainForm->net_client->sender->SendLockerAddItem(drop_locker_pos,
                                                    MainForm->animation->interface_controller->inventory->drag_item_id,
                                                    1);
                                                MainForm->animation->interface_controller->inventory->input_enabled = true;
                                            }
                                            else
                                            {
                                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                                    MainForm->strings->GetDialogText(DialogText_LockerFullHold, false),
                                                    MainForm->strings->GetDialogText(DialogText_LockerFullHold, true));
                                                MainForm->edits->caret_enabled = false;
                                            }
                                        }
                                        else
                                        {
                                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
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
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_RefusedDeposit, false),
                                        MainForm->strings->GetDialogText(DialogText_RefusedDeposit, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                        }
                        if (MainForm->animation->chest->is_visible)
                        {
                            drop_handled = 1;
                            if (MainForm->animation->chest->window_y + 17 < y &&
                                MainForm->animation->chest->window_y + 217 > y &&
                                MainForm->animation->chest->window_x + 17 < x &&
                                MainForm->animation->chest->window_x + 252 > x)
                            {
                                if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                                    ItemSpecial_Bound)
                                {
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                                else
                                {
                                    if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                        2)
                                    {
                                        Windows::TPoint drop_chest_pos;
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
                        }
                        if (!drop_handled)
                        {
                            if (MainForm->net_client->IsWithinReach(MainForm->map->GetCenterX(),
                                MainForm->map->GetCenterY(), MainForm->map->GetCursorX(),
                                MainForm->map->GetCursorY()) &&
                                !MainForm->map->IsWallOrSpecTile(MainForm->map->GetCursorX(),
                                MainForm->map->GetCursorY()))
                            {
                                MainForm->animation->interface_controller->inventory->CancelDrag();
                                char can_drop = 1;
                                if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                                    ItemSpecial_Bound)
                                {
                                    can_drop = 0;
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                                        MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                                if (can_drop &&
                                    MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
                                {
                                    can_drop = 0;
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetText(GameText_Warning),
                                        MainForm->strings->GetText(GameText_YouCannotDropItems));
                                    MainForm->edits->caret_enabled = false;
                                }
                                if (MainForm->map_items->GetProtectionOwnerAt(MainForm->map->GetCursorX(),
                                    MainForm->map->GetCursorY()) > 0)
                                    can_drop = 0;
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
                                                            String gold_name = MainForm->item_values->GetName(1);
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                                MainForm->strings->GetText(GameText_YouHaveNoMore) + " " + gold_name);
                                                        }
                                                        else
                                                        {
                                                            MainForm->net_client->sender->SendDropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                                                1, MainForm->map->GetCursorX(),
                                                                MainForm->map->GetCursorY());
                                                            MainForm->animation->interface_controller->inventory->input_enabled = true;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        MainForm->net_client->sender->SendDropItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                                                            1, MainForm->map->GetCursorX(),
                                                            MainForm->map->GetCursorY());
                                                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        MainForm->animation->game->OpenAmountBoxAtTile(1,
                                            MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                            MainForm->animation->interface_controller->inventory->drag_item_id, 0,
                                            MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                                        MainForm->animation->interface_controller->inventory->input_enabled = false;
                                    }
                                }
                            }
                            else
                            {
                                if (!MainForm->net_client->IsWithinReach(MainForm->map->GetCenterX(),
                                    MainForm->map->GetCenterY(), MainForm->map->GetCursorX(),
                                    MainForm->map->GetCursorY()))
                                {
                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                        MainForm->strings->GetText(GameText_OutOfDropRange));
                                    MainForm->animation->interface_controller->chat->AddSystemMessage(18, 0,
                                        MainForm->strings->GetText(GameText_OutOfDropRange), true);
                                }
                            }
                        }
                    }
                }
                else
                {
                    MainForm->map->path_finder->CancelPath();
                    if (!button)
                    {
                        bool click_used = 0;
                        // The menu a right click opens on a player.
                        // Its options in order: paperdoll, book, join, invite, trade, private message, friend, ignore.
                        if (MainForm->animation->game->player_menu_visible)
                        {
                            if (MainForm->animation->game->player_menu_x + 12 < x &&
                                MainForm->animation->game->player_menu_y + 142 > y &&
                                MainForm->animation->game->player_menu_x + 101 > x &&
                                MainForm->animation->game->player_menu_y + 5 < y)
                            {
                                if (MainForm->animation->game->player_menu_hovered_option >= 0)
                                {
                                    char close_menu = 1;
                                    MainForm->sfx->PlaySfx(3);
                                    if (!MainForm->animation->game->player_menu_hovered_option)
                                    {
                                        MainForm->net_client->sender->RequestPaperdoll(MainForm->animation->game->player_menu_target_id);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_PaperdollIsNowViewed));
                                        close_menu = 1;
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 1)
                                    {
                                        MainForm->net_client->sender->RequestBook(MainForm->animation->game->player_menu_target_id);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_BookIsNowViewed));
                                        close_menu = 1;
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
                                                String existing_member = MainForm->animation->interface_controller->group->GetMemberName(MainForm->animation->game->player_menu_target_id);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    Client::Capitalize(MainForm->net_client, existing_member) + " " +
                                                    MainForm->strings->GetText(GameText_IsAlreadyInYour));
                                                MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
                                                    Client::Capitalize(MainForm->net_client, existing_member) + " " +
                                                    MainForm->strings->GetText(GameText_IsAlreadyInYour), false);
                                            }
                                        }
                                        else
                                        {
                                            close_menu = 0;
                                            MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
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
                                                    Client::Capitalize(MainForm->net_client,
                                                    MainForm->animation->game->player_menu_name) + " " +
                                                    MainForm->strings->GetText(GameText_IsInvitedToParty));
                                                MainForm->net_client->sender->RequestPartyInvite(MainForm->animation->game->player_menu_target_id);
                                            }
                                            else
                                            {
                                                String party_member = MainForm->animation->interface_controller->group->GetMemberName(MainForm->animation->game->player_menu_target_id);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    Client::Capitalize(MainForm->net_client, party_member) + " " +
                                                    MainForm->strings->GetText(GameText_IsAlreadyInYour));
                                                MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
                                                    Client::Capitalize(MainForm->net_client, party_member) + " " +
                                                    MainForm->strings->GetText(GameText_IsAlreadyInYour), false);
                                            }
                                        }
                                        else
                                        {
                                            close_menu = 0;
                                            MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
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
                                                    close_menu = 0;
                                                    MainForm->animation->interface_controller->chat->AddSystemMessage(2,
                                                        3, MainForm->strings->GetText(GameText_YouRecentlyRequestedAnotherTrade), false);
                                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                                        MainForm->strings->GetText(GameText_YouRecentlyRequestedAnotherTrade));
                                                }
                                            }
                                            else
                                            {
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                                    MainForm->strings->GetText(GameText_Warning),
                                                    MainForm->strings->GetText(GameText_YouCannotCarryEnough));
                                                MainForm->edits->caret_enabled = false;
                                            }
                                        }
                                        else
                                        {
                                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                                MainForm->strings->GetText(GameText_Warning),
                                                MainForm->strings->GetText(GameText_YouCannotTradeIn));
                                            MainForm->edits->caret_enabled = false;
                                        }
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 5)
                                        MainForm->edits->SetText(EditId_Chat,
                                            "!" +
                                            Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name) + " ");
                                    if (MainForm->animation->game->player_menu_hovered_option == 6)
                                    {
                                        MainForm->animation->social->ToggleFriend(Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name));
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_WillBeYourFriend));
                                    }
                                    if (MainForm->animation->game->player_menu_hovered_option == 7)
                                    {
                                        MainForm->animation->social->ToggleIgnore(Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name));
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name) + " " +
                                            MainForm->strings->GetText(GameText_WillBeIgnordedRemove));
                                    }
                                    if (close_menu)
                                        MainForm->animation->game->ClosePlayerMenu();
                                    click_used = 1;
                                }
                            }
                            else
                            {
                                MainForm->animation->game->ClosePlayerMenu();
                                click_used = 1;
                            }
                        }
                        if (!click_used)
                        {
                            if (MainForm->animation->game->SitOnChair(MainForm->map->GetCursorX(),
                                MainForm->map->GetCursorY()))
                                click_used = 1;
                        }
                        if (!click_used)
                        {
                            if (MainForm->animation->game->StandUp())
                                click_used = 1;
                        }
                        if (!click_used)
                        {
                            // A chair is reached from the tile on the side it faces.
                            int chair_spec = MainForm->map->GetChairSpec(MainForm->map->GetCursorX(),
                                MainForm->map->GetCursorY());
                            if (chair_spec >= TileSpec_ChairYHigh && chair_spec <= TileSpec_ChairAnySide)
                            {
                                char chair_walk_started = 0;
                                if (chair_spec == TileSpec_ChairYHigh || chair_spec == TileSpec_ChairYHighXHigh ||
                                    chair_spec == TileSpec_ChairAnySide)
                                    chair_walk_started = MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX(),
                                        MainForm->map->GetCursorY() + 1);
                                if (chair_spec == TileSpec_ChairXLow)
                                    chair_walk_started = MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX() -
                                        1, MainForm->map->GetCursorY());
                                if (chair_spec == TileSpec_ChairXHigh)
                                    chair_walk_started = MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX() +
                                        1, MainForm->map->GetCursorY());
                                if (chair_spec == TileSpec_ChairYLow || chair_spec == TileSpec_ChairXLowYLow)
                                    chair_walk_started = MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX(),
                                        MainForm->map->GetCursorY() - 1);
                                if (chair_walk_started)
                                    MainForm->animation->interface_controller->spells->CancelCast();
                                click_used = 1;
                            }
                        }
                        // Then, in order: a spell's target, an item to pick up, an npc, a locker or chest, a sign,
                        // a board, a jukebox and a door; a click on none of them walks to the tile.
                        if (!click_used && (x <= 590 || y >= 35))
                        {
                            char click_pending = 1;
                            char cast_cleared = 0;
                            if (MainForm->animation->interface_controller->spells->IsCasting())
                            {
                                MainForm->animation->interface_controller->spells->ClearSelection();
                                cast_cleared = 1;
                            }
                            if (MainForm->animation->interface_controller->spells->awaiting_target && !cast_cleared)
                            {
                                char player_targeted = 0;
                                int target_player_id = 0;
                                char npc_targeted = 0;
                                int target_npc_index = 0;
                                for (int npc_slot = MainForm->npcs->GetCount() - 1; npc_slot >= 0; npc_slot--)
                                {
                                    Npc target_npc = MainForm->npcs->GetNpc(npc_slot);
                                    NpcTypeInfo target_type = MainForm->npc_values->GetType(target_npc.npc_id);
                                    if (target_type.npc_type == NpcType_Unknown1 || target_type.npc_type == NpcType_Unknown2)
                                    {
                                        if (target_npc.frame_width > 0 && target_npc.frame_height > 0)
                                        {
                                            int target_height = target_npc.frame_height;
                                            if (target_height > 70)
                                                target_height = 70;
                                            int target_x = TILE_HALF_WIDTH * target_npc.x - TILE_HALF_WIDTH * target_npc.y +
                                                MainForm->map->GetScrollOffsetX() + 319;
                                            int target_y = 16 * target_npc.x + 16 * target_npc.y +
                                                MainForm->map->GetScrollOffsetY() + 160;
                                            if (target_x - target_npc.frame_width / 2 <= x &&
                                                target_npc.frame_width / 2 + target_x >= x &&
                                                target_y - target_height <= y && target_y + 5 >= y)
                                            {
                                                npc_targeted = 1;
                                                target_npc_index = target_npc.index;
                                                break;
                                            }
                                        }
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YouCannotAttackThis));
                                }
                                for (player_slot = MainForm->players->GetCount() - 1; player_slot >= 0; player_slot--)
                                {
                                    Player target_player = MainForm->players->GetPlayer(player_slot);
                                    player_x = TILE_HALF_WIDTH * target_player.x - TILE_HALF_WIDTH * target_player.y +
                                        MainForm->map->GetScrollOffsetX() + 310;
                                    player_y = TILE_HALF_HEIGHT * target_player.x + TILE_HALF_HEIGHT * target_player.y +
                                        MainForm->map->GetScrollOffsetY() + 102;
                                    if (player_x - 1 <= x && player_x + 19 >= x && player_y <= y && player_y + 58 >= y)
                                    {
                                        player_targeted = 1;
                                        target_player_id = target_player.player_id;
                                        break;
                                    }
                                }
                                if (!MainForm->animation->game->IsActionDelayElapsed() ||
                                    !MainForm->net_client->CanAct())
                                {
                                    npc_targeted = 0;
                                    player_targeted = 0;
                                }
                                char spell_cast = 0;
                                if (!spell_cast && npc_targeted &&
                                    (!MainForm->animation->interface_controller->spells->selected_target_kind ||
                                    MainForm->animation->interface_controller->spells->selected_target_kind == 2))
                                {
                                    int caster_id = MainForm->net_client->GetPlayerId();
                                    String npc_spell_shout = MainForm->skill_values->GetShout(MainForm->animation->interface_controller->spells->selected_spell_id);
                                    MainForm->players->ShowTextLabel(caster_id, npc_spell_shout, 2, 1,
                                        SHOUT_LABEL_TICKS);
                                    MainForm->net_client->sender->SendSpellChant(MainForm->animation->interface_controller->spells->selected_spell_id);
                                    MainForm->animation->interface_controller->spells->BeginCast(MainForm->animation->interface_controller->spells->selected_spell_id,
                                        CastTarget_Npc, target_npc_index);
                                    spell_cast = 1;
                                }
                                if (!spell_cast && player_targeted &&
                                    (MainForm->animation->interface_controller->spells->selected_target_kind == 1 ||
                                    MainForm->animation->interface_controller->spells->selected_target_kind == 2))
                                {
                                    if (MainForm->map->GetMapType() != PVP_MAP_TYPE &&
                                        MainForm->animation->interface_controller->spells->selected_spell_type == 1)
                                    {
                                        MainForm->animation->interface_controller->chat->AddSystemMessage(3, 0,
                                            MainForm->strings->GetText(GameText_YouCannotAttackOther), false);
                                        spell_cast = 1;
                                    }
                                    else
                                    {
                                        int self_id = MainForm->net_client->GetPlayerId();
                                        String player_spell_shout = MainForm->skill_values->GetShout(MainForm->animation->interface_controller->spells->selected_spell_id);
                                        MainForm->players->ShowTextLabel(self_id, player_spell_shout, 2, 1,
                                            SHOUT_LABEL_TICKS);
                                        MainForm->net_client->sender->SendSpellChant(MainForm->animation->interface_controller->spells->selected_spell_id);
                                        MainForm->animation->interface_controller->spells->BeginCast(MainForm->animation->interface_controller->spells->selected_spell_id,
                                            CastTarget_Player, target_player_id);
                                        spell_cast = 1;
                                    }
                                }
                                MainForm->animation->interface_controller->spells->ClearSelection();
                                click_pending = 0;
                            }
                            if (click_pending)
                            {
                                if (MainForm->net_client->IsWithinReach(MainForm->map->GetCenterX(),
                                    MainForm->map->GetCenterY(), MainForm->map->GetCursorX(),
                                    MainForm->map->GetCursorY()))
                                {
                                    int item_uid = MainForm->map_items->FindItemAt(MainForm->map->GetCursorX(),
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
                                                            MainForm->strings->GetText(GameText_ThisItemIsProtectedBy) + " " + owner_name);
                                                    else
                                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                            MainForm->strings->GetText(GameText_ThisItemIsProtected));
                                                }
                                            }
                                            else
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_ItsTooHeavyYou));
                                            click_pending = 0;
                                        }
                                        else
                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                                                MainForm->strings->GetText(GameText_YouCouldNotPick));
                                    }
                                }
                            }
                            if (click_pending)
                            {
                                npc_clicked = 0;
                                if (!npc_clicked)
                                {
                                    char found_npc = 0;
                                    for (int i = MainForm->npcs->GetCount() - 1; i >= 0 && !found_npc; i--)
                                    {
                                        Npc clicked_npc = MainForm->npcs->GetNpc(i);
                                        if (clicked_npc.frame_width > 0 && clicked_npc.frame_height > 0)
                                        {
                                            npc_height = clicked_npc.frame_height;
                                            if (npc_height > 70)
                                                npc_height = 70;
                                            int npc_x = TILE_HALF_WIDTH * clicked_npc.x - TILE_HALF_WIDTH * clicked_npc.y +
                                                MainForm->map->GetScrollOffsetX() + 319;
                                            npc_y = 16 * clicked_npc.x + 16 * clicked_npc.y +
                                                MainForm->map->GetScrollOffsetY() + 160;
                                            if (npc_x - clicked_npc.frame_width / 2 <= x &&
                                                npc_x + clicked_npc.frame_width / 2 >= x && npc_y - npc_height <= y &&
                                                npc_y + 5 >= y)
                                            {
                                                found_npc = 1;
                                                npc_clicked = 1;
                                                NpcTypeInfo npc_type = MainForm->npc_values->GetType(clicked_npc.npc_id);
                                                // The npc's type picks what it opens: shop, inn, bank,
                                                // barber, guild, priest, law, skill master or quest.
                                                if (npc_type.npc_type == NpcType_Shop)
                                                    MainForm->net_client->sender->RequestShopOpen(clicked_npc.index);
                                                if (npc_type.npc_type == NpcType_Inn)
                                                {
                                                    MainForm->animation->inn->npc_name = clicked_npc.name;
                                                    MainForm->net_client->sender->RequestInnOpen(clicked_npc.index);
                                                }
                                                if (npc_type.npc_type == NpcType_Bank)
                                                    MainForm->net_client->sender->RequestBankOpen(clicked_npc.index);
                                                if (npc_type.npc_type == NpcType_Barber)
                                                    MainForm->net_client->sender->RequestBarberOpen(clicked_npc.index);
                                                if (npc_type.npc_type == NpcType_Guild)
                                                    MainForm->net_client->sender->RequestGuildOpen(clicked_npc.index);
                                                if (npc_type.npc_type == NpcType_Priest)
                                                    MainForm->net_client->sender->RequestPriestOpen(clicked_npc.index);
                                                if (npc_type.npc_type == NpcType_Law)
                                                {
                                                    MainForm->animation->law->npc_name = clicked_npc.name;
                                                    MainForm->net_client->sender->RequestLawOpen(clicked_npc.index);
                                                }
                                                if (npc_type.npc_type == NpcType_Trainer)
                                                    MainForm->net_client->sender->RequestSkillMasterOpen(clicked_npc.index);
                                                if (npc_type.npc_type == NpcType_Quest &&
                                                    MainForm->animation->dialog->request_cooldown <= 0)
                                                {
                                                    MainForm->animation->dialog->npc_name = clicked_npc.name;
                                                    MainForm->animation->dialog->npc_index = clicked_npc.index;
                                                    MainForm->animation->dialog->request_cooldown = QUEST_REQUEST_COOLDOWN_TICKS;
                                                    MainForm->net_client->sender->RequestQuestDialog(clicked_npc.index,
                                                        0);
                                                }
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Npc,
                                                    MainForm->npc_values->GetName(clicked_npc.npc_id));
                                            }
                                        }
                                    }
                                }
                                if (!npc_clicked && !MainForm->animation->IsObjectClickDelayed())
                                {
                                    MainForm->animation->StartObjectClickDelay();
                                    int player_x_tile = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
                                    int player_y_tile = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
                                    TPoint clicked_locker = MapController::FindLockerAt(MainForm->map, x, y);
                                    if (clicked_locker.x >= 0 && clicked_locker.y >= 0 &&
                                        MainForm->net_client->IsAdjacent(clicked_locker.x, clicked_locker.y,
                                        player_x_tile, player_y_tile))
                                    {
                                        npc_clicked = 1;
                                        MainForm->net_client->sender->RequestLockerOpen(clicked_locker);
                                    }
                                    TPoint clicked_chest = MapController::FindChestAt(MainForm->map, x, y);
                                    if (clicked_chest.x >= 0 && clicked_chest.y >= 0 &&
                                        MainForm->net_client->IsAdjacent(clicked_chest.x, clicked_chest.y,
                                        player_x_tile, player_y_tile))
                                    {
                                        npc_clicked = 1;
                                        int chest_key = MainForm->map->GetChestKey(clicked_chest.x, clicked_chest.y);
                                        if (chest_key > 0)
                                        {
                                            if (MainForm->animation->interface_controller->inventory->HasKey(chest_key))
                                            {
                                                MainForm->net_client->sender->RequestChestOpen(clicked_chest);
                                            }
                                            else
                                            {
                                                String chest_key_name = MainForm->item_values->GetKeyName(chest_key);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_ChestIsLocked2) + " - " + chest_key_name);
                                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                                    MainForm->strings->GetDialogText(DialogText_ChestLocked, false),
                                                    MainForm->strings->GetDialogText(DialogText_ChestLocked, true));
                                                MainForm->sfx->PlaySfx(25);
                                            }
                                        }
                                        else
                                        {
                                            MainForm->net_client->sender->RequestChestOpen(clicked_chest);
                                        }
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    String sign_data = MapController::FindSignTextAt(MainForm->map, x, y);
                                    if (sign_data.Length() > 1)
                                    {
                                        npc_clicked = 1;
                                        if (!MainForm->animation->game->message_visible)
                                        {
                                            // Sign text: a length, the title, then the message.
                                            int title_length = MainForm->net_client->DecodeNumber(sign_data.SubString(1,
                                                1));
                                            sign_data.Delete(1, 1);
                                            String sign_title = sign_data.SubString(1, title_length);
                                            sign_data.Delete(1, title_length);
                                            if (!sign_title.Length())
                                                sign_title = "Sign";
                                            String sign_message = sign_data;
                                            MainForm->sfx->PlaySfx(4);
                                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None, sign_title, sign_message);
                                            MainForm->edits->caret_enabled = false;
                                        }
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    Windows::TPoint board_tile = MapController::FindBoardAt(MainForm->map, x, y);
                                    if (board_tile.x > 0 && board_tile.y > 0)
                                    {
                                        npc_clicked = 1;
                                        // The eight board specs follow each other, so the spec gives the board number.
                                        int board_id = (int)MainForm->map->GetUsableTileSpec(board_tile.x,
                                            board_tile.y) - 19;
                                        if (board_id >= 0 && board_id <= 7)
                                            MainForm->net_client->sender->RequestBoardOpen(board_id);
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    TPoint jukebox_tile = MapController::FindJukeboxAt(MainForm->map, x, y);
                                    if (jukebox_tile.x > 0 && jukebox_tile.y > 0)
                                    {
                                        npc_clicked = 1;
                                        MainForm->net_client->sender->RequestJukeboxOpen(jukebox_tile);
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    Windows::TPoint door_tile = MapController::FindDoorAt(MainForm->map, x, y);
                                    if (door_tile.x > 0 && door_tile.y > 0)
                                    {
                                        npc_clicked = 1;
                                        int door_key = MainForm->map->GetDoorKey(door_tile.x, door_tile.y);
                                        if (door_key > 0)
                                        {
                                            if (MainForm->animation->interface_controller->inventory->HasKey(door_key))
                                            {
                                                MainForm->net_client->sender->SendOpenDoor(door_tile.x, door_tile.y);
                                            }
                                            else
                                            {
                                                String door_key_name = MainForm->item_values->GetKeyName(door_key);
                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                    MainForm->strings->GetText(GameText_DoorIsLocked2) + " - " + door_key_name);
                                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                                    MainForm->strings->GetDialogText(DialogText_DoorLocked, false),
                                                    MainForm->strings->GetDialogText(DialogText_DoorLocked, true));
                                                MainForm->sfx->PlaySfx(25);
                                            }
                                        }
                                        else
                                        {
                                            MainForm->net_client->sender->SendOpenDoor(door_tile.x, door_tile.y);
                                        }
                                    }
                                }
                                if (!npc_clicked)
                                {
                                    char walk_started = MainForm->map->path_finder->FindPath(MainForm->map->GetCursorX(),
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
                        menu_closed = 0;
                        if (MainForm->animation->game->player_menu_visible)
                        {
                            MainForm->animation->game->ClosePlayerMenu();
                            menu_closed = 1;
                        }
                        if (!menu_closed)
                        {
                            for (menu_slot = MainForm->players->GetCount() - 1; menu_slot >= 0; menu_slot--)
                            {
                                Player menu_player = MainForm->players->GetPlayer(menu_slot);
                                menu_x = TILE_HALF_WIDTH * menu_player.x - TILE_HALF_WIDTH * menu_player.y + MainForm->map->GetScrollOffsetX() +
                                    310;
                                menu_y = TILE_HALF_HEIGHT * menu_player.x + TILE_HALF_HEIGHT * menu_player.y + MainForm->map->GetScrollOffsetY() +
                                    102;
                                if (menu_x - 1 <= x && menu_x + 19 >= x && menu_y <= y && menu_y + 58 >= y &&
                                    menu_player.hover_label_shown)
                                {
                                    if (MainForm->net_client->GetPlayerId() != menu_player.player_id)
                                    {
                                        MainForm->animation->game->OpenPlayerMenu(menu_x, menu_y, menu_player.player_id,
                                            menu_player.name);
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                                            MainForm->strings->GetText(GameText_MenuBelongsToPlayer) + " " +
                                            Client::Capitalize(MainForm->net_client,
                                            MainForm->animation->game->player_menu_name));
                                        break;
                                    }
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
            if (MainForm->animation->GetSubState() == GamePanel_Status)
                MainForm->animation->interface_controller->status->MouseDown(x, y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Chat)
                MainForm->animation->interface_controller->chat->OnMouseClick(x, y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Settings)
                MainForm->animation->interface_controller->settings->MouseDown(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_News)
                MainForm->animation->interface_controller->news->MouseDown(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Help)
                MainForm->animation->interface_controller->help->MouseDown(x, y);
            if (MainForm->animation->GetSubState() == GamePanel_Inventory)
                MainForm->animation->interface_controller->inventory->MouseDown(x, y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Spells)
                MainForm->animation->interface_controller->spells->MouseDown(x, y, button);
            if (MainForm->animation->GetSubState() == GamePanel_Skills)
                MainForm->animation->interface_controller->skills->MouseDown(x, y, button);
            if (x >= 62 && x <= 98 && y >= 410 && y <= 428 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
            if (x >= 590 && x <= 626 && y >= 430 && y <= 448 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
                if (MainForm->animation->GetSubState() != GamePanel_Help)
                {
                    MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
                    MainForm->animation->report->Open(MainForm->strings->GetText(GameText_EndlessHelp));
                    MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_ListWithHelpfullTopics),
                        "", true);
                    MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_YouCanAlwaysAsk), "",
                        true);
                    MainForm->animation->report->AddText(MainForm->strings->GetText(GameText_IForgotMyPassword), "",
                        false);
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
            if (x >= 62 && x <= 98 && y >= 430 && y <= 448 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
            if (x >= 590 && x <= 626 && y >= 350 && y <= 368 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
            if (MainForm->animation->GetSubState() == GamePanel_Playerlist)
                MainForm->animation->interface_controller->player_list->MouseDown(x, y, button);
            if (x >= 590 && x <= 626 && y >= 370 && y <= 388 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
                            Client::EncodeNumber(MainForm->net_client,
                            MainForm->animation->interface_controller->group->members.size(), 1));
                    }
                }
            }
            if (MainForm->animation->GetSubState() == GamePanel_Group)
                MainForm->animation->interface_controller->group->MouseDown(x, y);
            if (x >= 62 && x <= 98 && y >= 330 && y <= 348 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
            if (x >= 62 && x <= 98 && y >= 350 && y <= 368 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
                if (!MainForm->map->IsMinimapDisabled())
                {
                    if (MainForm->gfx->interface_graphics->minimap_visible)
                    {
                        MainForm->gfx->interface_graphics->minimap_visible = false;
                        MainForm->redraw_needed = true;
                    }
                    else
                    {
                        MainForm->gfx->interface_graphics->minimap_visible = true;
                        MainForm->redraw_needed = true;
                    }
                }
                else
                {
                    MainForm->gfx->interface_graphics->minimap_visible = false;
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_ThereIsNoMap));
                }
            }
            if (x >= 62 && x <= 98 && y >= 370 && y <= 388 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
            if (x >= 62 && x <= 98 && y >= 390 && y <= 408 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
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
            if (x >= 590 && x <= 626 && y >= 410 && y <= 428 &&
                !MainForm->animation->interface_controller->inventory->is_dragging &&
                !MainForm->animation->interface_controller->spells->is_dragging &&
                !MainForm->animation->interface_controller->skills->is_dragging)
            {
                MainForm->sfx->PlaySfx(2);
                if (MainForm->animation->GetSubState() != GamePanel_Settings)
                {
                    MainForm->animation->interface_controller->chat->UnsubscribeGlobalChat();
                    MainForm->gfx->LoadSettingsPanel();
                    MainForm->animation->SetSubState(GamePanel_Settings);
                    MainForm->animation->interface_controller->Invalidate();
                    MainForm->animation->interface_controller->settings->MarkDirty();
                }
            }
            // The friend list; the button beside it opens the ignore list.
            if (x >= 590 && x <= 607 && y >= 311 && y <= 327 && !MainForm->animation->social->is_open)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_FriendList) + " " +
                    MainForm->strings->GetText(GameText_UseRightMouseClick));
                MainForm->sfx->PlaySfx(2);
                MainForm->animation->social->ClearOnlineNames();
                MainForm->animation->social->OpenWindow();
                String social_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                social_title = Client::Capitalize(MainForm->net_client, social_title);
                social_title = social_title + "'s " + MainForm->strings->GetText(GameText_FriendList);
                MainForm->animation->social->title = social_title;
                MainForm->animation->social->list_kind = 0;
                MainForm->net_client->sender->RequestOnlineNames();
            }
            if (x >= 608 && x <= 624 && y >= 311 && y <= 327 && !MainForm->animation->social->is_open)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_IgnoreList) + " " +
                    MainForm->strings->GetText(GameText_UseRightMouseClick));
                MainForm->sfx->PlaySfx(2);
                MainForm->animation->social->ClearOnlineNames();
                MainForm->animation->social->OpenWindow();
                String other_social_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                other_social_title = Client::Capitalize(MainForm->net_client, other_social_title);
                other_social_title = other_social_title + "'s " + MainForm->strings->GetText(GameText_IgnoreList);
                MainForm->animation->social->title = other_social_title;
                MainForm->animation->social->list_kind = 1;
            }
        }
    }
    if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
    {
        if (MainForm->animation->menu->is_message_open || MainForm->animation->menu->is_text_window_open ||
            MainForm->animation->menu->is_form_open || MainForm->animation->menu->is_loading_world)
        {
            MainForm->animation->menu->OnMouseDown(x, y);
            if (MainForm->animation->menu->is_text_window_open &&
                *mouse >= MainForm->animation->menu->text_window_x + 137 &&
                *mouse <= MainForm->animation->menu->text_window_x + 225 &&
                mouse[1] >= MainForm->animation->menu->text_window_y + 196 &&
                mouse[1] <= MainForm->animation->menu->text_window_y + 224)
            {
                MainForm->sfx->PlaySfx(3);
                MainForm->animation->menu->is_text_window_open = false;
                MainForm->edits->caret_enabled = true;
            }
            if (MainForm->animation->menu->is_message_open)
            {
                if (*mouse >= MainForm->animation->menu->message_x + 182 &&
                    *mouse <= MainForm->animation->menu->message_x + 270 &&
                    mouse[1] >= MainForm->animation->menu->message_y + 114 &&
                    mouse[1] <= MainForm->animation->menu->message_y + 140)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->menu->is_message_open = false;
                    MainForm->edits->caret_enabled = true;
                }
                if (MainForm->animation->menu->message_kind == MessageBoxKind_Confirm &&
                    *mouse >= MainForm->animation->menu->message_x + 89 &&
                    *mouse <= MainForm->animation->menu->message_x + 177 &&
                    mouse[1] >= MainForm->animation->menu->message_y + 114 &&
                    mouse[1] <= MainForm->animation->menu->message_y + 140)
                {
                    MainForm->net_client->sender->ConfirmCharacterDelete();
                    MainForm->animation->menu->is_message_open = false;
                    MainForm->edits->caret_enabled = true;
                }
            }
            if (MainForm->animation->menu->is_form_open)
            {
                if (!MainForm->animation->menu->is_message_open)
                {
                    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                    {
                        if (*mouse >= MainForm->animation->menu->form_x + 244 &&
                            *mouse <= MainForm->animation->menu->form_x + 325 &&
                            mouse[1] >= MainForm->animation->menu->form_y + 75 &&
                            mouse[1] <= MainForm->animation->menu->form_y + 159)
                        {
                            int preview_direction = MainForm->players->GetDirection(PlayerId_Preview);
                            if (MainForm->players->GetDirection(PlayerId_Preview) < 3)
                                MainForm->players->SetDirection(PlayerId_Preview, preview_direction + 1);
                            else
                                MainForm->players->SetDirection(PlayerId_Preview, FacingDirection_Down);
                            MainForm->sfx->PlaySfx(4);
                        }
                        if (*mouse >= MainForm->animation->menu->form_x + 162 &&
                            *mouse <= MainForm->animation->menu->form_x + 217)
                        {
                            if (mouse[1] >= MainForm->animation->menu->form_y + 82 &&
                                mouse[1] <= MainForm->animation->menu->form_y + 104)
                            {
                                if (MainForm->animation->menu->new_gender < 1)
                                    MainForm->animation->menu->new_gender++;
                                else
                                    MainForm->animation->menu->new_gender = 0;
                                MainForm->players->SetPreviewGender(MainForm->animation->menu->new_gender);
                                MainForm->players->ReloadGraphics(PlayerId_Preview);
                                MainForm->sfx->PlaySfx(4);
                            }
                            if (mouse[1] >= MainForm->animation->menu->form_y + 111 &&
                                mouse[1] <= MainForm->animation->menu->form_y + 133)
                            {
                                if (MainForm->animation->menu->new_hair_style < 20)
                                    MainForm->animation->menu->new_hair_style++;
                                else
                                    MainForm->animation->menu->new_hair_style = 1;
                                MainForm->players->SetPreviewHairStyle(MainForm->animation->menu->new_hair_style);
                                MainForm->players->ReloadGraphics(PlayerId_Preview);
                                MainForm->sfx->PlaySfx(4);
                            }
                            if (mouse[1] >= MainForm->animation->menu->form_y + 138 &&
                                mouse[1] <= MainForm->animation->menu->form_y + 160)
                            {
                                if (MainForm->animation->menu->new_hair_color < 9)
                                    MainForm->animation->menu->new_hair_color++;
                                else
                                    MainForm->animation->menu->new_hair_color = 0;
                                MainForm->players->SetPreviewHairColor(MainForm->animation->menu->new_hair_color);
                                MainForm->players->ReloadGraphics(PlayerId_Preview);
                                MainForm->sfx->PlaySfx(4);
                            }
                            if (mouse[1] >= MainForm->animation->menu->form_y + 165 &&
                                mouse[1] <= MainForm->animation->menu->form_y + 187)
                            {
                                if (MainForm->animation->menu->new_skin < 3)
                                    MainForm->animation->menu->new_skin++;
                                else
                                    MainForm->animation->menu->new_skin = 0;
                                MainForm->players->SetPreviewSkin(MainForm->animation->menu->new_skin);
                                MainForm->players->ReloadGraphics(PlayerId_Preview);
                                MainForm->sfx->PlaySfx(4);
                            }
                        }
                    }
                    if (*mouse >= MainForm->animation->menu->form_x + 158 &&
                        *mouse <= MainForm->animation->menu->form_x + 246 &&
                        mouse[1] >= MainForm->animation->menu->form_y + 195 &&
                        mouse[1] <= MainForm->animation->menu->form_y + 221)
                    {
                        if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
                        {
                            if (MainForm->animation->menu->create_character_enabled)
                            {
                                MainForm->sfx->PlaySfx(2);
                                if (MainForm->edits->GetText(12) != "")
                                {
                                    if (MainForm->edits->GetText(12).Length() < 4)
                                    {
                                        MainForm->edits->caret_enabled = false;
                                        MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                            MainForm->strings->GetDialogText(DialogText_WrongName, false),
                                            MainForm->strings->GetDialogText(DialogText_WrongName, true));
                                        MainForm->edits->SetText(12, "");
                                        MainForm->edits->FocusEdit(12);
                                    }
                                    else
                                    {
                                        MainForm->edits->caret_enabled = false;
                                        MainForm->animation->menu->create_character_enabled = false;
                                        MainForm->net_client->sender->SendCharacterCreate(MainForm->edits->GetText(12),
                                            MainForm->animation->menu->new_gender,
                                            MainForm->animation->menu->new_hair_style,
                                            MainForm->animation->menu->new_hair_color,
                                            MainForm->animation->menu->new_skin);
                                    }
                                }
                            }
                        }
                        if (MainForm->animation->menu->form_kind == 1 &&
                            MainForm->animation->menu->change_password_enabled)
                        {
                            MainForm->sfx->PlaySfx(2);
                            if (MainForm->edits->GetText(1) != "" && MainForm->edits->GetText(5) != "" &&
                                MainForm->edits->GetText(6) != "" && MainForm->edits->GetText(7) != "")
                            {
                                char password_error = 0;
                                if (MainForm->edits->GetText(6) != MainForm->edits->GetText(7))
                                {
                                    MainForm->edits->caret_enabled = false;
                                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                        MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, false),
                                        MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, true));
                                    MainForm->edits->SetText(6, "");
                                    MainForm->edits->SetText(7, "");
                                    MainForm->edits->FocusEdit(6);
                                    password_error = 1;
                                }
                                if (MainForm->edits->GetText(6).Length() < 6 && !password_error)
                                {
                                    MainForm->edits->caret_enabled = false;
                                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                        MainForm->strings->GetDialogText(DialogText_WrongPasswordSafety, false),
                                        MainForm->strings->GetDialogText(DialogText_WrongPasswordSafety, true));
                                    MainForm->edits->SetText(6, "");
                                    MainForm->edits->SetText(7, "");
                                    MainForm->edits->FocusEdit(6);
                                    password_error = 1;
                                }
                                if (MainForm->edits->HasFewDistinctChars(MainForm->edits->GetText(6)) &&
                                    !password_error)
                                {
                                    MainForm->edits->caret_enabled = false;
                                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                        MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                        MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, true));
                                    MainForm->edits->SetText(6, "");
                                    MainForm->edits->SetText(7, "");
                                    MainForm->edits->FocusEdit(6);
                                    password_error = 1;
                                }
                                if (!password_error)
                                {
                                    MainForm->edits->caret_enabled = false;
                                    MainForm->animation->menu->change_password_enabled = false;
                                    MainForm->net_client->sender->SendChangePassword(MainForm->edits->GetText(1),
                                        MainForm->edits->GetText(5), MainForm->edits->GetText(6));
                                }
                            }
                        }
                    }
                    if (*mouse >= MainForm->animation->menu->form_x + 250)
                    {
                        if (*mouse <= MainForm->animation->menu->form_x + 338)
                        {
                            if (mouse[1] >= MainForm->animation->menu->form_y + 195)
                            {
                                if (mouse[1] <= MainForm->animation->menu->form_y + 221)
                                {
                                    MainForm->sfx->PlaySfx(3);
                                    MainForm->animation->menu->is_form_open = false;
                                    MainForm->players->RemovePreviewPlayer();
                                    MainForm->edits->Clear();
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // The character list: the three slots are kept as the players 999991 to 999993.
            // Each slot has a log-in and a delete button; the first press on delete only arms it and asks.
            if (MainForm->animation->GetSubState() == MenuScreen_CharacterSelect)
            {
                if (((y > 10 && y < 17 && 3 * y + 560 > x) || (y > 16 && y < 31 && y + 594 > x)) && x >= 594 &&
                    x <= 624)
                {
                    if (MainForm->client->Active)
                        MainForm->client->Close();
                    MainForm->animation->menu->is_loading_world = false;
                }
                if (x >= 495 && x <= 615 && y >= 93 && y <= 120 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetSlotState(PlayerId_SlotFirst))
                {
                    MainForm->registry->SetCharacterName(MainForm->players->GetName(PlayerId_SlotFirst));
                    MainForm->sfx->PlaySfx(2);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->animation->menu->SetMapNeeded(0);
                    MainForm->animation->menu->SetItemFileNeeded(0);
                    MainForm->animation->menu->SetClassFileNeeded(0);
                    MainForm->animation->menu->SetReservedFileNeeded(0);
                    MainForm->animation->menu->SetEnteringGame(0);
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotFirst));
                }
                if (x >= 495 && x <= 615 && y >= 217 && y <= 244 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetSlotState(PlayerId_SlotSecond))
                {
                    MainForm->registry->SetCharacterName(MainForm->players->GetName(PlayerId_SlotSecond));
                    MainForm->sfx->PlaySfx(2);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->animation->menu->SetMapNeeded(0);
                    MainForm->animation->menu->SetItemFileNeeded(0);
                    MainForm->animation->menu->SetClassFileNeeded(0);
                    MainForm->animation->menu->SetReservedFileNeeded(0);
                    MainForm->animation->menu->SetEnteringGame(0);
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotSecond));
                }
                if (x >= 495 && x <= 615 && y >= 341 && y <= 368 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetSlotState(PlayerId_SlotThird))
                {
                    MainForm->registry->SetCharacterName(MainForm->players->GetName(PlayerId_SlotThird));
                    MainForm->sfx->PlaySfx(2);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->animation->menu->SetMapNeeded(0);
                    MainForm->animation->menu->SetItemFileNeeded(0);
                    MainForm->animation->menu->SetClassFileNeeded(0);
                    MainForm->animation->menu->SetReservedFileNeeded(0);
                    MainForm->animation->menu->SetEnteringGame(0);
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotThird));
                }
                if (x >= 495 && x <= 615 && y >= 121 && y <= 148 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetSlotState(PlayerId_SlotFirst))
                {
                    if (MainForm->animation->menu->delete_armed_slot == 1)
                    {
                        MainForm->sfx->PlaySfx(2);
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
                if (x >= 495 && x <= 615 && y >= 245 && y <= 272 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetSlotState(PlayerId_SlotSecond))
                {
                    if (MainForm->animation->menu->delete_armed_slot == 2)
                    {
                        MainForm->sfx->PlaySfx(2);
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
                if (x >= 495 && x <= 615 && y >= 369 && y <= 396 &&
                    !MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetSlotState(PlayerId_SlotThird))
                {
                    if (MainForm->animation->menu->delete_armed_slot == 3)
                    {
                        MainForm->sfx->PlaySfx(2);
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
                if (x >= 452 && x <= 570 && y >= 416 && y <= 456)
                {
                    MainForm->gfx->LoadChangePasswordPanel();
                    MainForm->animation->menu->change_password_enabled = true;
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->sfx->PlaySfx(2);
                    MainForm->edits->AddEdit(1, 2, MainForm->animation->menu->form_x + 200,
                        MainForm->animation->menu->form_y + 60, 132, EDIT_LINE_HEIGHT);
                    MainForm->edits->AddEdit(5, 2, MainForm->animation->menu->form_x + 200,
                        MainForm->animation->menu->form_y + 89, 132, EDIT_LINE_HEIGHT);
                    MainForm->edits->AddEdit(6, 2, MainForm->animation->menu->form_x + 200,
                        MainForm->animation->menu->form_y + 119, 132, EDIT_LINE_HEIGHT);
                    MainForm->edits->AddEdit(7, 2, MainForm->animation->menu->form_x + 200,
                        MainForm->animation->menu->form_y + 149, 132, EDIT_LINE_HEIGHT);
                    MainForm->edits->SetBackgroundVisible(1, 0);
                    MainForm->edits->SetBorderVisible(1, 0);
                    MainForm->edits->SetBackgroundVisible(5, 0);
                    MainForm->edits->SetBorderVisible(5, 0);
                    MainForm->edits->SetBackgroundVisible(6, 0);
                    MainForm->edits->SetBorderVisible(6, 0);
                    MainForm->edits->SetBackgroundVisible(7, 0);
                    MainForm->edits->SetBorderVisible(7, 0);
                    MainForm->edits->SetTextColor(1, 11847900);
                    MainForm->edits->SetTextColor(5, 11847900);
                    MainForm->edits->SetTextColor(6, 11847900);
                    MainForm->edits->SetTextColor(7, 11847900);
                    MainForm->edits->SetMaxLength(1, 16);
                    MainForm->edits->SetMaxLength(5, 12);
                    MainForm->edits->SetMaxLength(6, 12);
                    MainForm->edits->SetMaxLength(7, 12);
                    MainForm->edits->SetCharFilter(1, 2);
                    MainForm->edits->SetCharFilter(5, 2);
                    MainForm->edits->SetCharFilter(6, 2);
                    MainForm->edits->SetCharFilter(7, 2);
                    MainForm->edits->SetPasswordMode(6, 1);
                    MainForm->edits->SetPasswordMode(7, 1);
                    MainForm->edits->FocusEdit(1);
                    MainForm->animation->menu->OpenFormWindow(1);
                }
                if (x >= 332 && x <= 450 && y >= 416 && y <= 456 &&
                    !MainForm->animation->menu->character_select_cooldown)
                {
                    MainForm->edits->Clear();
                    MainForm->sfx->PlaySfx(2);
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->net_client->sender->RequestCharacterCreate();
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_CreateAccount)
            {
                if (((y > 10 && y < 17 && 3 * y + 560 > x) || (y > 16 && y < 31 && y + 594 > x)) && x >= 594 &&
                    x <= 624)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(2);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
                // The button beside the email box types an @.
                if (x >= 580 && x <= 600 && y >= 363 && y <= 383)
                {
                    MainForm->sfx->PlaySfx(2);
                    MainForm->edits->FocusEdit(10);
                    MainForm->edits->TypeText("@");
                }
                if (x >= 358 && x <= 476 && y >= 416 && y <= 455 && MainForm->animation->menu->create_account_enabled)
                {
                    MainForm->sfx->PlaySfx(2);
                    MainForm->animation->account_name = MainForm->edits->GetText(1);
                    MainForm->animation->account_password = MainForm->edits->GetText(3);
                    MainForm->animation->account_detail_a = MainForm->edits->GetText(8);
                    MainForm->animation->account_detail_b = MainForm->edits->GetText(9);
                    MainForm->animation->account_email = MainForm->edits->GetText(10);
                    if (MainForm->edits->GetText(1) != "" && MainForm->edits->GetText(3) != "" &&
                        MainForm->edits->GetText(4) != "" && MainForm->edits->GetText(8) != "" &&
                        MainForm->edits->GetText(9) != "" && MainForm->edits->GetText(10) != "")
                    {
                        char account_error = 0;
                        if (MainForm->edits->GetText(1).Length() < 4)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongAccount, false),
                                MainForm->strings->GetDialogText(DialogText_WrongAccount, true));
                            MainForm->edits->SetText(1, "");
                            MainForm->edits->FocusEdit(1);
                            account_error = 1;
                        }
                        if (MainForm->edits->HasFewDistinctChars(MainForm->edits->GetText(1)) && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_AccountToObvious, false),
                                MainForm->strings->GetDialogText(DialogText_AccountToObvious, true));
                            MainForm->edits->SetText(1, "");
                            MainForm->edits->FocusEdit(1);
                            account_error = 1;
                        }
                        if (MainForm->edits->GetText(3) != MainForm->edits->GetText(4) && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, false),
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordTwo, true));
                            MainForm->edits->SetText(3, "");
                            MainForm->edits->SetText(4, "");
                            MainForm->edits->FocusEdit(3);
                            account_error = 1;
                        }
                        if (MainForm->edits->GetText(3).Length() < 6 && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordSafety, false),
                                MainForm->strings->GetDialogText(DialogText_WrongPasswordSafety, true));
                            MainForm->edits->SetText(3, "");
                            MainForm->edits->SetText(4, "");
                            MainForm->edits->FocusEdit(3);
                            account_error = 1;
                        }
                        if (MainForm->edits->HasFewDistinctChars(MainForm->edits->GetText(3)) && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, false),
                                MainForm->strings->GetDialogText(DialogText_PasswordTooObvious, true));
                            MainForm->edits->SetText(3, "");
                            MainForm->edits->SetText(4, "");
                            MainForm->edits->FocusEdit(3);
                            account_error = 1;
                        }
                        if (!MainForm->edits->IsValidEmail(MainForm->edits->GetText(10)) && !account_error)
                        {
                            MainForm->edits->caret_enabled = false;
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetDialogText(DialogText_WrongInputValid, false),
                                MainForm->strings->GetDialogText(DialogText_WrongInputValid, true));
                            MainForm->edits->SetText(10, "");
                            MainForm->edits->FocusEdit(10);
                            account_error = 1;
                        }
                        if (!account_error)
                        {
                            if (MainForm->animation->account_name.Length() >= 4 &&
                                MainForm->animation->account_password.Length() >= 6)
                            {
                                MainForm->edits->caret_enabled = false;
                                MainForm->animation->menu->create_account_enabled = false;
                                MainForm->net_client->sender->RequestAccountNameCheck(MainForm->edits->GetText(1));
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
                if (x >= 480 && x <= 598 && y >= 416 && y <= 455)
                {
                    MainForm->animation->StopAccountProgress();
                    MainForm->sfx->PlaySfx(2);
                    MainForm->edits->caret_enabled = true;
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_Login || MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                if (MainForm->animation->GetSubState() == MenuScreen_Login && *mouse >= 360 && *mouse <= 450 && mouse[1] >= 388 &&
                    mouse[1] <= 417 && MainForm->animation->menu->login_enabled)
                {
                    if (MainForm->edits->GetText(1) != "" && MainForm->edits->GetText(2) != "")
                    {
                        MainForm->animation->menu->login_enabled = false;
                        MainForm->registry->SetAccountName(MainForm->edits->GetText(1));
                        if (MainForm->animation->GetSubState() == MenuScreen_Login)
                        {
                            MainForm->sfx->PlaySfx(2);
                            MainForm->net_client->sender->SendLogin(MainForm->edits->GetText(1),
                                MainForm->edits->GetText(2));
                        }
                    }
                }
                if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid && *mouse >= 360 && *mouse <= 450 && mouse[1] >= 388 &&
                    mouse[1] <= 417 && MainForm->animation->menu->login_enabled)
                {
                    if (MainForm->animation->menu->account_name != "" && MainForm->animation->menu->password != "")
                    {
                        MainForm->animation->menu->login_enabled = false;
                        MainForm->sfx->PlaySfx(2);
                        MainForm->net_client->sender->SendLogin(LowerCase(MainForm->animation->menu->account_name),
                            LowerCase(MainForm->animation->menu->password));
                    }
                }
                if (MainForm->animation->GetSubState() == MenuScreen_GridAccountName)
                {
                    if (MainForm->edits->GetText(1) != "")
                    {
                        MainForm->registry->SetAccountName(MainForm->edits->GetText(1));
                        MainForm->animation->menu->account_name = MainForm->edits->GetText(1);
                        MainForm->animation->menu->account_name = Client::Capitalize(MainForm->net_client,
                            MainForm->animation->menu->account_name);
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
                if (*mouse >= 452 && *mouse <= 542 && mouse[1] >= 388 && mouse[1] <= 417)
                {
                    MainForm->edits->Clear();
                    MainForm->gfx->LoadMainMenuScreen(true);
                    MainForm->animation->SetSubState(MenuScreen_Main);
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                if (*mouse >= 289 && *mouse <= 423 && mouse[1] >= 240 && mouse[1] <= 374)
                {
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                        MainForm->strings->GetText(GameText_Information),
                        MainForm->strings->GetText(GameText_UseYourArrowkeysAnd));
                    MainForm->animation->menu->message_shows_image = true;
                    MainForm->edits->caret_enabled = false;
                }
                if (*mouse >= 464 && *mouse <= 486 && mouse[1] >= 335 && mouse[1] <= 354 &&
                    MainForm->animation->menu->grid_row > 0)
                {
                    MainForm->sfx->PlaySfx(20);
                    MainForm->animation->menu->grid_row--;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (*mouse >= 445 && *mouse <= 463 && mouse[1] >= 355 && mouse[1] <= 374 &&
                    MainForm->animation->menu->grid_column > 0)
                {
                    MainForm->sfx->PlaySfx(20);
                    MainForm->animation->menu->grid_column--;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (*mouse >= 464 && *mouse <= 486 && mouse[1] >= 355 && mouse[1] <= 374 &&
                    MainForm->animation->menu->grid_row < 5)
                {
                    MainForm->sfx->PlaySfx(20);
                    MainForm->animation->menu->grid_row++;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (*mouse >= 487 && *mouse <= 509 && mouse[1] >= 355 && mouse[1] <= 374 &&
                    MainForm->animation->menu->grid_column < 5)
                {
                    MainForm->sfx->PlaySfx(20);
                    MainForm->animation->menu->grid_column++;
                    MainForm->animation->menu->cursor_blink = 5;
                }
                if (*mouse >= 514 && *mouse <= 562 && mouse[1] >= 335 && mouse[1] <= 354)
                {
                    MainForm->sfx->PlaySfx(2);
                    MainForm->animation->menu->AddSpace();
                }
                if (*mouse >= 514 && *mouse <= 562 && mouse[1] >= 355 && mouse[1] <= 374)
                    MainForm->animation->menu->AddSelectedLetter();
                if (*mouse >= 558 && *mouse <= 562 && mouse[1] >= 284 && mouse[1] <= 298)
                {
                    MainForm->sfx->PlaySfx(19);
                    MainForm->animation->menu->DeleteLastLetter();
                }
            }
            if (MainForm->animation->GetSubState() == MenuScreen_Main || MainForm->animation->GetSubState() == MenuScreen_Credits ||
                MainForm->animation->GetSubState() == MenuScreen_Login || MainForm->animation->GetSubState() == MenuScreen_GridAccountName ||
                MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            {
                if (x >= 30 && x <= 210 && y >= 400 && y <= 440)
                {
                    if (MainForm->mm_timer_id)
                        timeKillEvent(MainForm->mm_timer_id);
                    if (MainForm->client->Active)
                        MainForm->client->Close();
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
                if (x >= 30 && x <= 210 && y >= 360 && y <= 400)
                {
                    MainForm->sfx->PlaySfx(2);
                    MainForm->edits->Clear();
                    MainForm->animation->SetSubState(MenuScreen_Credits);
                    if (!MainForm->animation->credits->is_running)
                    {
                        MainForm->gfx->LoadMenuBackground();
                        MainForm->animation->credits->Start();
                    }
                }
                if (x >= 30 && x <= 210 && y >= 320 && y <= 360)
                {
                    MainForm->animation->menu->login_enabled = true;
                    // With no session yet the button only connects; the reply then opens the login screen.
                    if (!MainForm->net_client->is_initialized)
                    {
                        if (!MainForm->animation->menu->connect_for_login)
                        {
                            if (!MainForm->animation->menu->connect_for_create_account)
                            {
                                if (!MainForm->animation->menu->connect_for_login)
                                {
                                    if (!MainForm->animation->menu->connect_for_create_account)
                                    {
                                        if (MainForm->client->Active)
                                        {
                                            MainForm->client->Active = false;
                                            Sleep(100);
                                        }
                                        MainForm->sfx->PlaySfx(2);
                                        MainForm->client->Active = true;
                                        MainForm->animation->menu->connect_for_login = 1;
                                        MainForm->animation->menu->connect_for_create_account = 0;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (MainForm->animation->credits->is_running)
                            MainForm->animation->credits->Stop();
                        MainForm->sfx->PlaySfx(2);
                        MainForm->edits->Clear();
                        MainForm->net_client->login_failure_count = 0;
                        MainForm->gfx->LoadLoginScreen();
                        if (MainForm->config->GetSkipSoftpad())
                        {
                            MainForm->animation->SetSubState(MenuScreen_Login);
                            MainForm->edits->AddEdit(1, 2, LOGIN_EDIT_X, LOGIN_NAME_EDIT_Y, 141, EDIT_LINE_HEIGHT);
                            MainForm->edits->AddEdit(2, 2, LOGIN_EDIT_X, LOGIN_PASSWORD_EDIT_Y, 141, EDIT_LINE_HEIGHT);
                            MainForm->edits->SetCharFilter(1, 2);
                            MainForm->edits->SetCharFilter(2, 1);
                            MainForm->edits->SetPasswordMode(2, 1);
                            MainForm->edits->SetMaxLength(1, 16);
                            MainForm->edits->SetMaxLength(2, 12);
                            MainForm->edits->FocusEdit(1);
                        }
                        else
                        {
                            MainForm->animation->SetSubState(MenuScreen_GridAccountName);
                            MainForm->edits->AddEdit(1, 2, LOGIN_EDIT_X, GRID_LOGIN_NAME_EDIT_Y, 161, EDIT_LINE_HEIGHT);
                            MainForm->edits->SetCharFilter(1, 2);
                            MainForm->edits->SetMaxLength(1, 16);
                            MainForm->edits->FocusEdit(1);
                        }
                    }
                }
                if (x >= 30 && x <= 210 && y >= 280 && y <= 320)
                {
                    MainForm->animation->menu->create_account_enabled = true;
                    if (!MainForm->net_client->is_initialized)
                    {
                        if (!MainForm->animation->menu->connect_for_login)
                        {
                            if (!MainForm->animation->menu->connect_for_create_account)
                            {
                                if (MainForm->client->Host.Length() > 0 || MainForm->client->Address.Length() > 0)
                                {
                                    if (MainForm->client->Active)
                                    {
                                        MainForm->client->Active = false;
                                        Sleep(100);
                                    }
                                    MainForm->sfx->PlaySfx(2);
                                    MainForm->client->Active = true;
                                    MainForm->animation->menu->connect_for_login = 0;
                                    MainForm->animation->menu->connect_for_create_account = 1;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (MainForm->animation->credits->is_running)
                            MainForm->animation->credits->Stop();
                        MainForm->sfx->PlaySfx(2);
                        MainForm->edits->Clear();
                        MainForm->animation->SetSubState(MenuScreen_CreateAccount);
                        MainForm->gfx->LoadCreateAccountScreen();
                        MainForm->edits->AddEdit(1, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_NAME_EDIT_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(3, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_PASSWORD_EDIT_Y, 242,
                            EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(4, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_CONFIRM_EDIT_Y, 242,
                            EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(8, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT8_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(9, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT9_Y, 242, EDIT_LINE_HEIGHT);
                        MainForm->edits->AddEdit(10, 2, ACCOUNT_FORM_EDIT_X, ACCOUNT_EDIT10_Y, 221, EDIT_LINE_HEIGHT);
                        MainForm->edits->SetPasswordMode(3, 1);
                        MainForm->edits->SetPasswordMode(4, 1);
                        MainForm->edits->SetCharFilter(1, 2);
                        MainForm->edits->SetCharFilter(3, 2);
                        MainForm->edits->SetCharFilter(4, 2);
                        MainForm->edits->SetCharFilter(8, 3);
                        MainForm->edits->SetCharFilter(9, 3);
                        MainForm->edits->SetCharFilter(10, 1);
                        MainForm->edits->SetMaxLength(1, 16);
                        MainForm->edits->SetMaxLength(3, 12);
                        MainForm->edits->SetMaxLength(4, 12);
                        MainForm->edits->SetMaxLength(1, 16);
                        MainForm->edits->SetMaxLength(3, 12);
                        MainForm->edits->SetMaxLength(4, 12);
                        MainForm->edits->FocusEdit(1);
                        MainForm->edits->caret_enabled = false;
                        MainForm->animation->menu->OpenTextWindow();
                        MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_ItIsVeryImportant), true);
                        MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_LotOfPlayersWho),
                            true);
                        MainForm->animation->menu->AddWrappedText(MainForm->strings->GetText(GameText_YourInformationWillOnly), false);
                    }
                }
            }
        }
    }
}

// Scrolls the open window on top, or with none open, the open interface panel.
void Mouser::HandleWheel(int direction)
{
    char handled = 0;
    if (!handled && direction == FacingDirection_Left && MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (!handled && MainForm->animation->game->form_visible)
        {
            MainForm->animation->game->InputFormScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->guild->is_visible)
        {
            MainForm->animation->guild->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->dialog->is_visible)
        {
            MainForm->animation->dialog->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->memo->is_visible)
        {
            MainForm->animation->memo->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->report->is_visible)
        {
            MainForm->animation->report->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->learn->is_visible)
        {
            MainForm->animation->learn->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->shop->is_visible)
        {
            MainForm->animation->shop->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->law->is_visible)
        {
            MainForm->animation->law->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->inn->is_visible)
        {
            MainForm->animation->inn->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->locker->is_visible)
        {
            MainForm->animation->locker->ScrollUp();
            handled = 1;
        }
        if (!handled && MainForm->animation->message_board->is_visible)
        {
            MainForm->animation->message_board->ScrollUp();
            handled = 1;
        }
    }
    if (!handled && direction == FacingDirection_Up && MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (!handled && MainForm->animation->game->form_visible)
        {
            MainForm->animation->game->InputFormScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->guild->is_visible)
        {
            MainForm->animation->guild->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->dialog->is_visible)
        {
            MainForm->animation->dialog->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->memo->is_visible)
        {
            MainForm->animation->memo->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->report->is_visible)
        {
            MainForm->animation->report->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->learn->is_visible)
        {
            MainForm->animation->learn->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->shop->is_visible)
        {
            MainForm->animation->shop->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->law->is_visible)
        {
            MainForm->animation->law->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->inn->is_visible)
        {
            MainForm->animation->inn->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->locker->is_visible)
        {
            MainForm->animation->locker->ScrollDown();
            handled = 1;
        }
        if (!handled && MainForm->animation->message_board->is_visible)
        {
            MainForm->animation->message_board->ScrollDown();
            handled = 1;
        }
    }
    if (!handled && direction == FacingDirection_Left && MainForm->animation->GetScreenMode() == ScreenMode_Game)
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
    if (!handled && direction == FacingDirection_Up && MainForm->animation->GetScreenMode() == ScreenMode_Game)
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
    TPoint locker_position;
    char over_window;
    int gold_amount;
    int own_id = 0;
    int own_x = 0;
    int own_y = 0;
    if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (MainForm->animation->interface_controller->inventory->is_dragging &&
            !MainForm->animation->interface_controller->inventory->dragging_from_paperdoll &&
            MainForm->gfx->game->view_clip_rect.top < y && MainForm->gfx->game->view_clip_rect.bottom > y &&
            MainForm->gfx->game->view_clip_rect.left < x && MainForm->gfx->game->view_clip_rect.right > x)
        {
            over_window = 0;
            if (MainForm->animation->trade->is_visible)
            {
                over_window = 1;
                if (MainForm->animation->trade->window_y + 43 < y && MainForm->animation->trade->window_y + 243 > y &&
                    MainForm->animation->trade->window_x + 280 < x && MainForm->animation->trade->window_x + 515 > x &&
                    !MainForm->animation->game->message_visible && !MainForm->animation->trade->trade_locked)
                {
                    if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                        ItemSpecial_Bound)
                    {
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                            MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    else
                    {
                        if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
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
            }
            if (MainForm->animation->paperdoll->is_visible)
            {
                over_window = 1;
                if (MainForm->animation->paperdoll->window_y < y &&
                    MainForm->animation->paperdoll->window_y + 290 > y &&
                    MainForm->animation->paperdoll->window_x < x && MainForm->animation->paperdoll->window_x + 380 > x)
                    MainForm->animation->paperdoll->EquipDroppedItem(MainForm->animation->interface_controller->inventory->drag_item_id,
                        x, y);
            }
            if (MainForm->animation->bank->is_visible)
            {
                over_window = 1;
                if (MainForm->animation->bank->window_y + 50 < y && y < MainForm->animation->bank->window_y + 170 &&
                    MainForm->animation->bank->window_x + 17 < x && MainForm->animation->bank->window_x + 252 > x)
                {
                    if (MainForm->animation->interface_controller->inventory->drag_item_id == 1)
                    {
                        gold_amount = MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                        if (gold_amount > 0)
                        {
                            if (gold_amount == 1)
                                MainForm->net_client->sender->SendBankDeposit(MainForm->animation->bank->npc_session_id,
                                    1);
                            else
                            {
                                MainForm->animation->game->OpenAmountBox(AmountBoxKind_BankDeposit, gold_amount, 1, 0);
                                MainForm->animation->interface_controller->inventory->input_enabled = true;
                            }
                        }
                        else
                        {
                            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
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
                over_window = 1;
                if (MainForm->animation->locker->window_y + 17 < y && y < MainForm->animation->locker->window_y + 217 &&
                    MainForm->animation->locker->window_x + 17 < x && MainForm->animation->locker->window_x + 252 > x)
                {
                    if (MainForm->animation->interface_controller->inventory->drag_item_id > 1)
                    {
                        if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                            2)
                        {
                            LockerController *locker = MainForm->animation->locker;
                            locker_position.x = locker->locker_x;
                            locker_position.y = locker->locker_y;
                            if (locker->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
                                WRAP_TEXT_WIDTH)
                            {
                                if (MainForm->animation->locker->CanStoreItem(MainForm->animation->interface_controller->inventory->drag_item_id))
                                {
                                    MainForm->net_client->sender->SendLockerAddItem(locker_position,
                                        MainForm->animation->interface_controller->inventory->drag_item_id, 1);
                                    MainForm->animation->interface_controller->inventory->input_enabled = true;
                                }
                                else
                                {
                                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                                        MainForm->strings->GetDialogText(DialogText_LockerFullHold, false),
                                        MainForm->strings->GetDialogText(DialogText_LockerFullHold, true));
                                    MainForm->edits->caret_enabled = false;
                                }
                            }
                            else
                            {
                                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
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
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_RefusedDeposit, false),
                            MainForm->strings->GetDialogText(DialogText_RefusedDeposit, true));
                        MainForm->edits->caret_enabled = false;
                    }
                }
            }
            if (MainForm->animation->chest->is_visible)
            {
                over_window = 1;
                if (MainForm->animation->chest->window_y + 17 < y && MainForm->animation->chest->window_y + 217 > y &&
                    MainForm->animation->chest->window_x + 17 < x && MainForm->animation->chest->window_x + 252 > x)
                {
                    if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                        ItemSpecial_Bound)
                    {
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                            MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    else
                    {
                        if (MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id) <
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
                    char can_drop = 1;
                    if (MainForm->item_values->GetSpecial(MainForm->animation->interface_controller->inventory->drag_item_id) ==
                        ItemSpecial_Bound)
                    {
                        can_drop = 0;
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_LoreItem, false),
                            MainForm->strings->GetDialogText(DialogText_LoreItem, true));
                        MainForm->edits->caret_enabled = false;
                    }
                    if (can_drop && MainForm->map->GetMapId() == MainForm->animation->game->restricted_map_id)
                    {
                        can_drop = 0;
                        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                            MainForm->strings->GetText(GameText_Warning),
                            MainForm->strings->GetText(GameText_YouCannotDropItems));
                        MainForm->edits->caret_enabled = false;
                    }
                    if (MainForm->map_items->GetProtectionOwnerAt(MainForm->map->GetCursorX(),
                        MainForm->map->GetCursorY()) > 0)
                        can_drop = 0;
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
                                        if (MainForm->animation->interface_controller->inventory->drag_item_id == 1)
                                        {
                                            if (!MainForm->animation->interface_controller->inventory->GetItemAmount(1))
                                            {
                                                String gold_name = MainForm->item_values->GetName(1);
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
                            }
                        }
                        else
                        {
                            MainForm->animation->game->OpenAmountBoxAtTile(1,
                                MainForm->animation->interface_controller->inventory->GetItemAmount(MainForm->animation->interface_controller->inventory->drag_item_id),
                                MainForm->animation->interface_controller->inventory->drag_item_id, 0,
                                MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
                            MainForm->animation->interface_controller->inventory->input_enabled = false;
                        }
                    }
                }
                else
                {
                    if (!MainForm->net_client->IsWithinReach(own_x, own_y, MainForm->map->GetCursorX(),
                        MainForm->map->GetCursorY()))
                    {
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_OutOfDropRange));
                        MainForm->animation->interface_controller->chat->AddSystemMessage(18, 0,
                            MainForm->strings->GetText(GameText_OutOfDropRange), true);
                    }
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
        MainForm->animation->paperdoll->MouseUp();
        MainForm->animation->book->MouseUp();
        MainForm->animation->trade->MouseUp();
        MainForm->animation->bank->MouseUp();
        MainForm->animation->jukebox->MouseUp();
        MainForm->animation->musician->MouseUp();
        MainForm->animation->barber->MouseUp();
        MainForm->animation->stat->MouseUp();
        MainForm->animation->learn->MouseUp();
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
