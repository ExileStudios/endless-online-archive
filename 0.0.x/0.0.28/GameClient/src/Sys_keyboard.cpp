#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_chat.h"
#include "Ctrl_menu.h"
#include "Packetproces.h"
#include <Clipbrd.hpp>
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Ctrl_book.h"
#include "Ctrl_dialog.h"
#include "Ctrl_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Gfx_interface.h"
#include "Ctrl_jukebox.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_musician.h"
#include "Packetoperations.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_quests.h"
#include "Ctrl_report.h"
#include "Ctrl_social.h"
#include "Gfx_sprites.h"
#include "Ctrl_stats.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

KeyBoard::KeyBoard()
{
    tab_delay = 0;
    backspace_delay = 0;
    repeat_interval = 0;
    repeat_delay = 0;
    key_delay = 0;
}

KeyBoard::~KeyBoard()
{
}

bool KeyBoard::IsArrowKeyDown()
{
    return arrow_held;
}

String KeyBoard::GetWalkDirection()
{
    return walk_direction;
}

// Reads the keyboard once and acts on it: menu keys, game keys, then typing into the focused edit box.
// When the window is inactive or the read fails, the key repeat and the arrow state start over.
void KeyBoard::PollKeyboard()
{
    void *AsHandle;
    LPVOID clipboard_data;
    int arrows_held = 0;
    int played_minutes;
    try
    {
        if (key_delay > 0)
            key_delay--;
        if (MainForm->active)
        {
            MainForm->Dx_InputDevice1->Poll();
            if (MainForm->Dx_InputDevice1->GetDeviceState(MainForm->DIKeyboardState1))
            {
                if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
                {
                    if (MainForm->animation->menu->is_message_open && (MainForm->DIKeyboardState1->Keys[DIK_ESCAPE] & 0x80) != 0)
                    {
                        MainForm->sfx->PlaySfx(3);
                        MainForm->animation->menu->is_message_open = false;
                        MainForm->edits->caret_enabled = true;
                    }
                    // On the character list, keys 1 to 3 log in with the character in that slot.
                    if (MainForm->animation->GetSubState() == MenuScreen_CharacterSelect)
                    {
                        if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0 &&
                            !MainForm->animation->menu->character_select_cooldown &&
                            !MainForm->players->GetSlotState(PlayerId_SlotFirst) &&
                            !MainForm->animation->menu->is_message_open && !MainForm->animation->menu->is_form_open &&
                            !MainForm->animation->menu->is_loading_world)
                        {
                            MainForm->sfx->PlaySfx(2);
                            MainForm->animation->menu->is_form_open = false;
                            MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                            MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotFirst));
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0 &&
                            !MainForm->animation->menu->character_select_cooldown &&
                            !MainForm->players->GetSlotState(PlayerId_SlotSecond) &&
                            !MainForm->animation->menu->is_message_open && !MainForm->animation->menu->is_form_open &&
                            !MainForm->animation->menu->is_loading_world)
                        {
                            MainForm->sfx->PlaySfx(2);
                            MainForm->animation->menu->is_form_open = false;
                            MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                            MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotSecond));
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0 &&
                            !MainForm->animation->menu->character_select_cooldown &&
                            !MainForm->players->GetSlotState(PlayerId_SlotThird) &&
                            !MainForm->animation->menu->is_message_open && !MainForm->animation->menu->is_form_open &&
                            !MainForm->animation->menu->is_loading_world)
                        {
                            MainForm->sfx->PlaySfx(2);
                            MainForm->animation->menu->is_form_open = false;
                            MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                            MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotThird));
                        }
                    }
                    // The password grid.
                    // Enter takes the letter under the cursor and Backspace removes the last one.
                    // The arrows move the cursor.
                    // Typing a letter or digit brings up the message box that clicking the grid shows.
                    if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid && !MainForm->animation->menu->is_message_open)
                    {
                        bool key_flag = 1;
                        if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
                        {
                            key_flag = 0;
                            if (key_delay <= 0)
                            {
                                key_delay = 12;
                                MainForm->animation->menu->AddSelectedLetter();
                            }
                            else
                                key_delay = 12;
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_BACK] & 0x80) != 0)
                        {
                            key_flag = 0;
                            if (key_delay <= 0)
                            {
                                key_delay = 12;
                                MainForm->sfx->PlaySfx(19);
                                MainForm->animation->menu->DeleteLastLetter();
                            }
                            else
                                key_delay = 12;
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_LEFT] & 0x80) != 0)
                        {
                            key_flag = 0;
                            if (key_delay <= 0)
                            {
                                key_delay = 8;
                                if (MainForm->animation->menu->grid_column > 0)
                                {
                                    MainForm->sfx->PlaySfx(20);
                                    MainForm->animation->menu->grid_column--;
                                    MainForm->animation->menu->cursor_blink = 5;
                                }
                            }
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_RIGHT] & 0x80) != 0)
                        {
                            key_flag = 0;
                            if (key_delay <= 0)
                            {
                                key_delay = 8;
                                if (MainForm->animation->menu->grid_column < 5)
                                {
                                    MainForm->sfx->PlaySfx(20);
                                    MainForm->animation->menu->grid_column++;
                                    MainForm->animation->menu->cursor_blink = 5;
                                }
                            }
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_UP] & 0x80) != 0)
                        {
                            key_flag = 0;
                            if (key_delay <= 0)
                            {
                                key_delay = 8;
                                if (MainForm->animation->menu->grid_row > 0)
                                {
                                    MainForm->sfx->PlaySfx(20);
                                    MainForm->animation->menu->grid_row--;
                                    MainForm->animation->menu->cursor_blink = 5;
                                }
                            }
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_DOWN] & 0x80) != 0)
                        {
                            key_flag = 0;
                            if (key_delay <= 0)
                            {
                                key_delay = 8;
                                if (MainForm->animation->menu->grid_row < 5)
                                {
                                    MainForm->sfx->PlaySfx(20);
                                    MainForm->animation->menu->grid_row++;
                                    MainForm->animation->menu->cursor_blink = 5;
                                }
                            }
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)
                        {
                            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                                MainForm->strings->GetText(GameText_Information),
                                MainForm->strings->GetText(GameText_UseYourArrowkeysAnd));
                            MainForm->animation->menu->message_shows_image = true;
                            MainForm->edits->caret_enabled = false;
                        }
                        if (key_flag)
                            key_delay = 0;
                    }
                }
                if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
                {
                    if ((MainForm->DIKeyboardState1->Keys[DIK_SCROLL] & 0x80) != 0)  // Scroll Lock
                        MainForm->gfx->interface_graphics->SaveScreenshot();
                    if (!MainForm->animation->game->message_visible && !MainForm->animation->game->large_box_visible &&
                        !MainForm->animation->game->input_box_visible && !MainForm->animation->game->form_visible &&
                        !MainForm->animation->paperdoll->is_visible && !MainForm->animation->book->is_visible &&
                        !MainForm->animation->trade->is_visible && !MainForm->animation->locker->is_visible &&
                        !MainForm->animation->learn->is_visible && !MainForm->animation->shop->is_visible &&
                        !MainForm->animation->stat->is_visible && !MainForm->animation->guild->is_visible &&
                        !MainForm->animation->memo->is_visible && !MainForm->animation->dialog->is_visible &&
                        !MainForm->animation->report->is_visible && !MainForm->animation->law->is_visible &&
                        !MainForm->animation->inn->is_visible && !MainForm->animation->bank->is_visible &&
                        !MainForm->animation->jukebox->is_visible && !MainForm->animation->musician->is_visible &&
                        !MainForm->animation->barber->is_visible && !MainForm->animation->chest->is_visible &&
                        !MainForm->animation->social->is_open && !MainForm->animation->message_board->is_visible &&
                        !MainForm->animation->quests->is_visible && !MainForm->animation->game->amount_box_visible)
                    {
                        arrow_held = false;
                        ctrl_attacked = false;
                        // Ctrl attacks; Ctrl and V paste the clipboard into an empty chat line.
                        if ((MainForm->DIKeyboardState1->Keys[DIK_LCONTROL] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_RCONTROL] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
                            {
                                if (MainForm->animation->GetScreenMode() == ScreenMode_Game &&
                                    MainForm->edits->IsFocused(EditId_Chat) &&
                                    MainForm->edits->GetTextLength(EditId_Chat) < 1 &&
                                    MainForm->animation->IsChatAllowed())
                                {
                                    MainForm->animation->RecordChatMessage();
                                    String clipboard_text;
                                    Clipboard()->Open();
                                    try
                                    {
                                        AsHandle = (void *)Clipboard()->GetAsHandle(1);
                                        clipboard_data = GlobalLock(AsHandle);
                                        clipboard_text = (char *)clipboard_data;
                                        GlobalUnlock(AsHandle);
                                    }
                                    catch (...)
                                    {
                                        Clipboard()->Close();
                                    }
                                    Clipboard()->Close();
                                    if (clipboard_text.Length() > 128)
                                        clipboard_text = clipboard_text.SubString(1, 128);
                                    if (clipboard_text.Length() > 0)
                                    {
                                        MainForm->edits->TypeText(clipboard_text);
                                        return;
                                    }
                                }
                            }
                            else if (MainForm->net_client->CanAct())
                            {
                                MainForm->map->path_finder->CancelPath();
                                MainForm->animation->game->Attack(0);
                                ctrl_attacked = true;
                            }
                        }
                        if (((MainForm->DIKeyboardState1->Keys[DIK_LEFT] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_UP] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_RIGHT] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_DOWN] & 0x80) != 0) && !ctrl_attacked)
                        {
                            arrow_held = true;
                            bool key_flag = 0;
                            char up_held = 0;
                            char right_held = 0;
                            char down_held = 0;
                            arrows_held = 0;
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LEFT] & 0x80) != 0)
                            {
                                key_flag = 1;
                                arrows_held++;
                            }
                            if ((MainForm->DIKeyboardState1->Keys[DIK_UP] & 0x80) != 0)
                            {
                                up_held = 1;
                                arrows_held++;
                            }
                            if ((MainForm->DIKeyboardState1->Keys[DIK_RIGHT] & 0x80) != 0)
                            {
                                right_held = 1;
                                arrows_held++;
                            }
                            if ((MainForm->DIKeyboardState1->Keys[DIK_DOWN] & 0x80) != 0)
                            {
                                down_held = 1;
                                arrows_held++;
                            }
                            // With two arrows down, the walk follows the one pressed second.
                            // Three or more change nothing.
                            if (arrows_held < 3)
                            {
                                char kept_first_direction = false;
                                if (arrows_held == 2)
                                {
                                    two_arrows_held = true;
                                    if (first_direction == "L" && key_flag)
                                    {
                                        kept_first_direction = true;
                                        first_direction = "L";
                                        if (up_held)
                                            walk_direction = "U";
                                        if (right_held)
                                            walk_direction = "R";
                                        if (down_held)
                                            walk_direction = "D";
                                    }
                                    if (first_direction == "U" && up_held)
                                    {
                                        kept_first_direction = true;
                                        first_direction = "U";
                                        if (key_flag)
                                            walk_direction = "L";
                                        if (right_held)
                                            walk_direction = "R";
                                        if (down_held)
                                            walk_direction = "D";
                                    }
                                    if (first_direction == "R" && right_held)
                                    {
                                        kept_first_direction = true;
                                        first_direction = "R";
                                        if (key_flag)
                                            walk_direction = "L";
                                        if (up_held)
                                            walk_direction = "U";
                                        if (down_held)
                                            walk_direction = "D";
                                    }
                                    if (first_direction == "D" && down_held)
                                    {
                                        kept_first_direction = true;
                                        first_direction = "D";
                                        if (key_flag)
                                            walk_direction = "L";
                                        if (up_held)
                                            walk_direction = "U";
                                        if (right_held)
                                            walk_direction = "R";
                                    }
                                }
                                if (arrows_held == 1 || !kept_first_direction)
                                {
                                    if (key_flag)
                                        walk_direction = "L";
                                    if (up_held)
                                        walk_direction = "U";
                                    if (right_held)
                                        walk_direction = "R";
                                    if (down_held)
                                        walk_direction = "D";
                                    first_direction = walk_direction;
                                }
                            }
                        }
                        else
                            two_arrows_held = false;
                        if (IsArrowKeyDown())
                        {
                            if (GetWalkDirection() == "L" && MainForm->net_client->CanAct())
                            {
                                MainForm->map->path_finder->CancelPath();
                                MainForm->animation->game->Walk(FacingDirection_Left);
                            }
                            if (GetWalkDirection() == "U" && MainForm->net_client->CanAct())
                            {
                                MainForm->map->path_finder->CancelPath();
                                MainForm->animation->game->Walk(FacingDirection_Up);
                            }
                            if (GetWalkDirection() == "R" && MainForm->net_client->CanAct())
                            {
                                MainForm->map->path_finder->CancelPath();
                                MainForm->animation->game->Walk(FacingDirection_Right);
                            }
                            if (GetWalkDirection() == "D" && MainForm->net_client->CanAct())
                            {
                                MainForm->map->path_finder->CancelPath();
                                MainForm->animation->game->Walk(FacingDirection_Down);
                            }
                        }
                        // F1 to F8 use the spell in that column, from the second row with Shift.
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F1] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(1, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(1, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F2] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(2, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(2, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F3] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(3, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(3, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F4] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(4, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(4, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F5] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(5, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(5, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F6] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(6, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(6, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F7] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(7, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(7, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F8] & 0x80) != 0)
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                                (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                                MainForm->animation->interface_controller->spells->UseSlot(8, 2);
                            else
                                MainForm->animation->interface_controller->spells->UseSlot(8, 1);
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F11] & 0x80) != 0)  // F11
                        {
                            MainForm->map->path_finder->CancelPath();
                            MainForm->animation->game->ToggleSit(MainForm->map->GetCursorX(),
                                MainForm->map->GetCursorY());
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_F12] & 0x80) != 0 &&  // F12
                            !MainForm->animation->IsRefreshDelayed())
                        {
                            MainForm->map->path_finder->CancelPath();
                            MainForm->net_client->sender->RequestRefresh();
                            MainForm->animation->StartRefreshDelay(14);
                        }
                        // The number pad keys play the emotes.
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD1] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(1);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD2] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(2);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD3] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(3);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD4] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(4);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD5] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(5);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD6] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(6);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD7] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(7);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD8] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(8);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD9] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(9);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_DECIMAL] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(10);
                        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD0] & 0x80) != 0)
                            MainForm->animation->game->PlayEmote(14);
                    }
                }
                if (MainForm->edits->GetCount() >= 1)
                {
                    if (MainForm->edits->caret_enabled)
                    {
                        String typed_text = "";
                        if (tab_delay >= 1)
                            tab_delay--;
                        if ((MainForm->DIKeyboardState1->Keys[DIK_TAB] & 0x80) != 0 && !tab_delay &&
                            MainForm->edits->GetCount() >= 2)
                        {
                            MainForm->edits->FocusNext();
                            MainForm->sfx->PlaySfx(4);
                            tab_delay = 32;
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
                        {
                            MainForm->animation->game->ResetIdleTimer();
                            if (MainForm->animation->CanSendChat())
                            {
                                MainForm->animation->StartChatDelay();
                                MainForm->edits->TypeLineBreak();
                                if (MainForm->edits->IsFocused(EditId_Chat))
                                {
                                    if (MainForm->edits->GetText(EditId_Chat).Length() > 0)
                                    {
                                        if (MainForm->animation->IsChatAllowed())
                                        {
                                            MainForm->animation->RecordChatMessage();
                                            // A chat line starts with the mark of its channel.
                                            // With no mark it is local chat or one of the commands.
                                            // Marks: + admin, ~ global, @ announcement, & guild, ' party, ! private.
                                            if (MainForm->edits->GetText(EditId_Chat)[1] != 33 &&
                                                MainForm->edits->GetText(EditId_Chat)[1] != 126 &&
                                                MainForm->edits->GetText(EditId_Chat)[1] != 64 &&
                                                MainForm->edits->GetText(EditId_Chat)[1] != 43 &&
                                                MainForm->edits->GetText(EditId_Chat)[1] != 38 &&
                                                String(MainForm->edits->GetText(EditId_Chat)[1]) != "'")
                                            {
                                                bool key_flag = 0;
                                                if (MainForm->edits->GetText(EditId_Chat).Length() > 1)
                                                {
                                                    if (MainForm->edits->GetText(EditId_Chat)[1] == 36 &&
                                                        MainForm->edits->GetText(EditId_Chat)[2] != 118)
                                                    {
                                                        if (MainForm->animation->IsCommandDelayed())
                                                            MainForm->edits->SetText(EditId_Chat, "");
                                                        else
                                                            MainForm->animation->RestartCommandDelay();
                                                    }
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat) ==
                                                    Client::DeobfuscateString(MainForm->net_client, "h8l,X|"))
                                                {
                                                    key_flag = 1;
                                                    MainForm->sfx->PlaySfx(5);
                                                    TDateTime now1 = Now();
                                                    TTimeStamp login_stamp = DateTimeToTimeStamp(MainForm->animation->interface_controller->status->usage_start_time);
                                                    TTimeStamp now_stamp = DateTimeToTimeStamp(now1);
                                                    int days = now_stamp.Date - login_stamp.Date;
                                                    int milliseconds = now_stamp.Time - login_stamp.Time;
                                                    played_minutes = milliseconds / 60000 + 1440 * days;
                                                    played_minutes += MainForm->animation->interface_controller->status->usage_minutes;
                                                    int hours = played_minutes / 60;
                                                    int minutes = played_minutes % MINUTES_PER_HOUR;
                                                    String played_text = IntToStr(hours) +
                                                        Client::DeobfuscateString(MainForm->net_client, " qZ-e") +
                                                        IntToStr(minutes) +
                                                        Client::DeobfuscateString(MainForm->net_client, "C1d2");
                                                    MainForm->animation->interface_controller->chat->AddLocalMessage(7,
                                                        0, Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"),
                                                        Client::DeobfuscateString(MainForm->net_client,
                                                        " e :f>Z* BUD") + played_text, true);
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat).Length() > 6)
                                                {
                                                    if (MainForm->edits->GetText(EditId_Chat).SubString(1, 5) ==
                                                        Client::DeobfuscateString(MainForm->net_client, "i1d9N"))
                                                    {
                                                        key_flag = 1;
                                                        String find_name = MainForm->edits->GetText(EditId_Chat);
                                                        find_name.Delete(1, 6);
                                                        if (find_name.Length() > 3)
                                                        {
                                                            if (find_name.Length() < 17)
                                                                MainForm->net_client->sender->SendFindPlayer(find_name);
                                                        }
                                                    }
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat) ==
                                                    Client::DeobfuscateString(MainForm->net_client, "j0a|") ||
                                                    MainForm->edits->GetText(EditId_Chat) ==
                                                    Client::DeobfuscateString(MainForm->net_client, "_0d+l<^3N"))
                                                {
                                                    key_flag = 1;
                                                    String location_text = MainForm->strings->GetText(GameText_YourCurrentLocationIs) +
                                                        IntToStr(MainForm->map->GetMapId()) + "  x:";
                                                    location_text = location_text +
                                                        IntToStr(MainForm->map->GetCenterX()) + "  y:";
                                                    location_text = location_text +
                                                        IntToStr(MainForm->map->GetCenterY());
                                                    MainForm->sfx->PlaySfx(5);
                                                    MainForm->animation->interface_controller->chat->AddLocalMessage(7,
                                                        0, Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"),
                                                        location_text, true);
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat) ==
                                                    Client::DeobfuscateString(MainForm->net_client, "f1d/N"))
                                                {
                                                    key_flag = 1;
                                                    MainForm->sfx->PlaySfx(4);
                                                    MainForm->animation->interface_controller->status->ping_sent_time = Now();
                                                    MainForm->net_client->sender->SendPingRequest();
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat) ==
                                                    Client::DeobfuscateString(MainForm->net_client, "a3l(^1N"))
                                                {
                                                    key_flag = 1;
                                                    MainForm->sfx->PlaySfx(4);
                                                    MainForm->animation->game->ToggleGhostWalk();
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat) ==
                                                    Client::DeobfuscateString(MainForm->net_client, "h1d8_:N"))
                                                {
                                                    key_flag = 1;
                                                    MainForm->sfx->PlaySfx(5);
                                                    String draw_mode_name = "VDACL";
                                                    draw_mode_name.Delete(2, 2);
                                                    if (MainForm->config->GetUseBitBltDraw())
                                                        draw_mode_name = Client::DeobfuscateString(MainForm->net_client,
                                                            "([*");
                                                    MainForm->animation->interface_controller->chat->AddLocalMessage(7,
                                                        0, Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"),
                                                        Client::DeobfuscateString(MainForm->net_client,
                                                        " e 1^6Z-h) +_:d3j p'r") +
                                                        MainForm->VersionString(MainForm->version_major,
                                                        MainForm->version_minor, MainForm->version_build), true);
                                                    MainForm->animation->interface_controller->chat->AddLocalMessage(7,
                                                        0, Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"),
                                                        Client::DeobfuscateString(MainForm->net_client,
                                                        " e :_6f1h [:i1h- BUD") + draw_mode_name, true);
                                                }
                                                if (!key_flag)
                                                {
                                                    String public_text = MainForm->edits->GetText(EditId_Chat);
                                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                        public_text = Client::MakeDrunkText(MainForm->net_client,
                                                            public_text);
                                                    String public_censored = Client::CensorText(MainForm->net_client,
                                                        public_text);
                                                    if (public_censored.Length() >= 1)
                                                    {
                                                        MainForm->net_client->sender->SendPublicChat(public_text);
                                                        MainForm->animation->interface_controller->chat->AddLocalMessage(-1,
                                                            0,
                                                            Client::Capitalize(MainForm->net_client,
                                                            MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                            public_censored, true);
                                                        MainForm->gfx->sprites->ShowSpeechBalloon(public_censored,
                                                            MainForm->net_client->GetPlayerId(), 0);
                                                    }
                                                    else
                                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                }
                                            }
                                            if (MainForm->edits->GetText(EditId_Chat).Length() > 1)
                                            {
                                                if (MainForm->edits->GetText(EditId_Chat)[1] == 43)
                                                {
                                                    if (MainForm->net_client->GetAdminLevel() > 0)
                                                    {
                                                        String admin_text = MainForm->edits->GetText(EditId_Chat);
                                                        String admin_censored = Client::CensorText(MainForm->net_client,
                                                            admin_text.SubString(2, admin_text.Length() - 1));
                                                        if (admin_censored.Length() >= 1)
                                                        {
                                                            MainForm->net_client->sender->SendAdminChat(admin_text.SubString(2,
                                                                admin_text.Length() - 1));
                                                            MainForm->sfx->PlaySfx(29);
                                                            MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Admin,
                                                                CHAT_COLOR_ADMIN,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                admin_censored, true);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                    else
                                                    {
                                                        String non_admin_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            non_admin_text = Client::MakeDrunkText(MainForm->net_client,
                                                                non_admin_text);
                                                        String non_admin_censored = Client::CensorText(MainForm->net_client,
                                                            non_admin_text);
                                                        if (non_admin_censored.Length() >= 1)
                                                        {
                                                            MainForm->net_client->sender->SendPublicChat(non_admin_text);
                                                            MainForm->animation->interface_controller->chat->AddLocalMessage(-1,
                                                                0,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                non_admin_censored, true);
                                                            MainForm->gfx->sprites->ShowSpeechBalloon(non_admin_censored,
                                                                MainForm->net_client->GetPlayerId(), 0);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat)[1] == 126)
                                                {
                                                    if (MainForm->animation->IsGlobalMessageAllowed())
                                                    {
                                                        if (MainForm->net_client->GetAdminLevel() <= 0)
                                                            MainForm->animation->RecordGlobalMessage();
                                                        String global_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            global_text = Client::MakeDrunkText(MainForm->net_client,
                                                                global_text);
                                                        String global_censored = Client::CensorText(MainForm->net_client,
                                                            global_text.SubString(2, global_text.Length() - 1));
                                                        if (global_censored.Length() >= 1)
                                                        {
                                                            MainForm->net_client->sender->SendGlobalChat(global_text.SubString(2,
                                                                global_text.Length() - 1));
                                                            MainForm->animation->interface_controller->chat->AddGlobalMessage(-1,
                                                                0,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                global_censored, true, false);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat)[1] == 64)
                                                {
                                                    if (MainForm->net_client->GetAdminLevel() > 1)
                                                    {
                                                        String announce_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            announce_text = Client::MakeDrunkText(MainForm->net_client,
                                                                announce_text);
                                                        String announce_censored = Client::CensorText(MainForm->net_client,
                                                            announce_text.SubString(2, announce_text.Length() - 1));
                                                        if (announce_censored.Length() >= 1)
                                                        {
                                                            MainForm->sfx->PlaySfx(14);
                                                            MainForm->net_client->sender->SendAnnouncement(announce_text.SubString(2,
                                                                announce_text.Length() - 1));
                                                            MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Announce,
                                                                CHAT_COLOR_NOTICE,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                announce_censored, true);
                                                            MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Announce,
                                                                CHAT_COLOR_NOTICE,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                announce_censored, true);
                                                            MainForm->animation->interface_controller->chat->AddGlobalMessage(ChatIcon_Announce,
                                                                CHAT_COLOR_NOTICE,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                announce_censored, true, false);
                                                            MainForm->gfx->sprites->ShowSpeechBalloon(announce_censored,
                                                                MainForm->net_client->GetPlayerId(), 0);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                    else
                                                    {
                                                        String non_announcer_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            non_announcer_text = Client::MakeDrunkText(MainForm->net_client,
                                                                non_announcer_text);
                                                        String non_announcer_censored = Client::CensorText(MainForm->net_client,
                                                            non_announcer_text);
                                                        if (non_announcer_censored.Length() >= 1)
                                                        {
                                                            MainForm->net_client->sender->SendPublicChat(non_announcer_text);
                                                            MainForm->animation->interface_controller->chat->AddLocalMessage(-1,
                                                                0,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                non_announcer_censored, true);
                                                            MainForm->gfx->sprites->ShowSpeechBalloon(non_announcer_censored,
                                                                MainForm->net_client->GetPlayerId(), 0);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                }
                                                if (MainForm->edits->GetText(EditId_Chat)[1] == 38)
                                                {
                                                    if (MainForm->animation->interface_controller->status->IsInGuild())
                                                    {
                                                        String guild_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            guild_text = Client::MakeDrunkText(MainForm->net_client,
                                                                guild_text);
                                                        String guild_censored = Client::CensorText(MainForm->net_client,
                                                            guild_text.SubString(2, guild_text.Length() - 1));
                                                        if (guild_censored.Length() >= 1)
                                                        {
                                                            MainForm->sfx->PlaySfx(29);
                                                            MainForm->net_client->sender->SendGuildChat(guild_text.SubString(2,
                                                                guild_text.Length() - 1));
                                                            MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Guild,
                                                                0,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                guild_censored, true);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                    else
                                                    {
                                                        String guildless_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            guildless_text = Client::MakeDrunkText(MainForm->net_client,
                                                                guildless_text);
                                                        String guildless_censored = Client::CensorText(MainForm->net_client,
                                                            guildless_text);
                                                        if (guildless_censored.Length() >= 1)
                                                        {
                                                            MainForm->net_client->sender->SendPublicChat(guildless_text);
                                                            MainForm->animation->interface_controller->chat->AddLocalMessage(-1,
                                                                0,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                guildless_censored, true);
                                                            MainForm->gfx->sprites->ShowSpeechBalloon(guildless_censored,
                                                                MainForm->net_client->GetPlayerId(), 0);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                }
                                                if (String(MainForm->edits->GetText(EditId_Chat)[1]) == "'")
                                                {
                                                    if (MainForm->animation->interface_controller->group->members.size() >
                                                        0)
                                                    {
                                                        String party_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            party_text = Client::MakeDrunkText(MainForm->net_client,
                                                                party_text);
                                                        String party_censored = Client::CensorText(MainForm->net_client,
                                                            party_text.SubString(2, party_text.Length() - 1));
                                                        if (party_censored.Length() >= 1)
                                                        {
                                                            MainForm->net_client->sender->SendPartyChat(party_text.SubString(2,
                                                                party_text.Length() - 1));
                                                            MainForm->gfx->sprites->ShowSpeechBalloon(party_censored,
                                                                MainForm->net_client->GetPlayerId(), 1);
                                                            MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Party,
                                                                0,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                party_censored, true);
                                                            MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Party,
                                                                1,
                                                                Client::Capitalize(MainForm->net_client,
                                                                MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                party_censored, true);
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                }
                                                // In a private tab the text goes to that tab's partner.
                                                // Elsewhere the name comes first, then a space.
                                                if (MainForm->edits->GetText(EditId_Chat)[1] == 33)
                                                {
                                                    if (MainForm->animation->interface_controller->chat->active_tab <=
                                                        2)
                                                    {
                                                        String tab_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            tab_text = Client::MakeDrunkText(MainForm->net_client,
                                                                tab_text);
                                                        String tab_partner = "";
                                                        String tab_message = tab_text.SubString(2,
                                                            tab_text.Length() - 1);
                                                        String tab_censored = Client::CensorText(MainForm->net_client,
                                                            tab_message);
                                                        if (tab_censored.Length() >= 1)
                                                        {
                                                            if (MainForm->animation->interface_controller->chat->active_tab ==
                                                                1)
                                                                tab_partner = MainForm->animation->interface_controller->chat->private_name_1;
                                                            else
                                                                tab_partner = MainForm->animation->interface_controller->chat->private_name_2;
                                                            if (tab_message.Length() >= 2)
                                                            {
                                                                if (tab_partner != "")
                                                                {
                                                                    MainForm->sfx->PlaySfx(19);
                                                                    MainForm->net_client->sender->SendPrivateMessage(LowerCase(tab_partner),
                                                                        tab_message);
                                                                    MainForm->animation->interface_controller->chat->OpenPrivateTab(tab_partner);
                                                                    int tab_index = MainForm->animation->interface_controller->chat->FindPrivateTab(tab_partner);
                                                                    if (tab_index != -1)
                                                                        MainForm->animation->interface_controller->chat->AddPrivateMessage(tab_index,
                                                                            1,
                                                                            Client::Capitalize(MainForm->net_client,
                                                                            MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                            tab_censored, true);
                                                                }
                                                            }
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                    }
                                                    else
                                                    {
                                                        String private_text = MainForm->edits->GetText(EditId_Chat);
                                                        if (MainForm->animation->interface_controller->chat->IsDrunk())
                                                            private_text = Client::MakeDrunkText(MainForm->net_client,
                                                                private_text);
                                                        int space_pos = private_text.Pos(" ");
                                                        if (space_pos >= 3)
                                                        {
                                                            String recipient = private_text.SubString(2, space_pos - 2);
                                                            String private_message = private_text.SubString(space_pos +
                                                                1, private_text.Length() - space_pos);
                                                            String private_censored = Client::CensorText(MainForm->net_client,
                                                                private_message);
                                                            if (private_censored.Length() >= 1)
                                                            {
                                                                if (recipient.Length() >= 4)
                                                                {
                                                                    if (recipient.Length() <= 13)
                                                                    {
                                                                        if (private_message.Length() >= 1)
                                                                        {
                                                                            if (LowerCase(recipient) !=
                                                                                LowerCase(MainForm->players->GetName(MainForm->net_client->GetPlayerId())))
                                                                            {
                                                                                MainForm->sfx->PlaySfx(19);
                                                                                MainForm->net_client->sender->SendPrivateMessage(LowerCase(recipient),
                                                                                    private_message);
                                                                                MainForm->animation->interface_controller->chat->OpenPrivateTab(recipient);
                                                                                int private_tab = MainForm->animation->interface_controller->chat->FindPrivateTab(recipient);
                                                                                if (private_tab != -1)
                                                                                    MainForm->animation->interface_controller->chat->AddPrivateMessage(private_tab,
                                                                                        1,
                                                                                        Client::Capitalize(MainForm->net_client,
                                                                                        MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                                                        private_censored, true);
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            else
                                                                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                    MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                                        }
                                                        else
                                                            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                                                MainForm->strings->GetText(GameText_InvalidInputTryUse));
                                                    }
                                                }
                                            }
                                            MainForm->edits->ClearFocusedText();
                                        }
                                    }
                                }
                            }
                            if (MainForm->edits->IsFocused(2) &&
                                MainForm->animation->GetScreenMode() == ScreenMode_Menu &&
                                MainForm->animation->GetSubState() == MenuScreen_Login && MainForm->animation->menu->login_enabled)
                            {
                                if (MainForm->edits->GetText(1) != "" && MainForm->edits->GetText(2) != "")
                                {
                                    MainForm->animation->menu->login_enabled = false;
                                    MainForm->registry->SetAccountName(MainForm->edits->GetText(1));
                                    MainForm->sfx->PlaySfx(2);
                                    MainForm->net_client->sender->SendLogin(MainForm->edits->GetText(1),
                                        MainForm->edits->GetText(2));
                                }
                            }
                            // Enter on the account name opens the password grid with the cursor on a random cell.
                            if (MainForm->edits->IsFocused(1))
                            {
                                if (MainForm->animation->GetScreenMode() == ScreenMode_Menu &&
                                    MainForm->animation->GetSubState() == MenuScreen_GridAccountName)
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
                                        MainForm->keyboard->key_delay = 12;
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
                            }
                        }
                        // The typed character, from the key state and the keyboard layout setting.
                        // Layouts 1 to 3 move symbols to other keys.
                        // Layout 3 also swaps A with Q and Z with W, as AZERTY does.
                        char shift_held = 0;
                        if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                            (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
                            shift_held = 1;
                        if (MainForm->animation->CanSendChat())
                        {
                            if ((MainForm->DIKeyboardState1->Keys[DIK_SPACE] & 0x80) != 0)
                                typed_text = typed_text + " ";
                            if (shift_held)
                            {
                                if (!MainForm->config->GetKeyboardLayout())
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
                                        typed_text = typed_text + "!";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
                                        typed_text = typed_text + "@";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
                                        typed_text = typed_text + "#";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
                                        typed_text = typed_text + "$";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
                                        typed_text = typed_text + "%";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
                                        typed_text = typed_text + "^";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
                                        typed_text = typed_text + "&";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
                                        typed_text = typed_text + "*";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
                                        typed_text = typed_text + "(";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
                                        typed_text = typed_text + ")";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + "_";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "+";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "[";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "]";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + ":";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "|";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + "<";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ">";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "?";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
                                        typed_text = typed_text + "!";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
                                        typed_text = typed_text + "#";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
                                        typed_text = typed_text + "$";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
                                        typed_text = typed_text + "%";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
                                        typed_text = typed_text + "&";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
                                        typed_text = typed_text + "_";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
                                        typed_text = typed_text + "(";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
                                        typed_text = typed_text + ")";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + "?";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "^";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "|";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + "+";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + ">";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ";";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ":";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "=";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
                                        typed_text = typed_text + "!";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
                                        typed_text = typed_text + "@";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
                                        typed_text = typed_text + "#";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
                                        typed_text = typed_text + "$";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
                                        typed_text = typed_text + "%";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
                                        typed_text = typed_text + "&";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
                                        typed_text = typed_text + "/";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
                                        typed_text = typed_text + "(";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
                                        typed_text = typed_text + ")";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
                                        typed_text = typed_text + "=";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + "?";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "\xc5";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "^";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + "\xd6";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "\xc4";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "*";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ";";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ":";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "_";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
                                        typed_text = typed_text + "1";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
                                        typed_text = typed_text + "2";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
                                        typed_text = typed_text + "3";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
                                        typed_text = typed_text + "4";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
                                        typed_text = typed_text + "5";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
                                        typed_text = typed_text + "6";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
                                        typed_text = typed_text + "7";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
                                        typed_text = typed_text + "8";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
                                        typed_text = typed_text + "9";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
                                        typed_text = typed_text + "0";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + ")";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "-";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "^";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "*";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + "M";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "%";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "\xa3";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ".";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ":";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "+";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
                                        typed_text = typed_text + "Q";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0)
                                        typed_text = typed_text + "B";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0)
                                        typed_text = typed_text + "C";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0)
                                        typed_text = typed_text + "D";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0)
                                        typed_text = typed_text + "E";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0)
                                        typed_text = typed_text + "F";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0)
                                        typed_text = typed_text + "G";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0)
                                        typed_text = typed_text + "H";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0)
                                        typed_text = typed_text + "I";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0)
                                        typed_text = typed_text + "J";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0)
                                        typed_text = typed_text + "K";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0)
                                        typed_text = typed_text + "L";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0)
                                        typed_text = typed_text + "?";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0)
                                        typed_text = typed_text + "N";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0)
                                        typed_text = typed_text + "O";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0)
                                        typed_text = typed_text + "P";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0)
                                        typed_text = typed_text + "A";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0)
                                        typed_text = typed_text + "R";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
                                        typed_text = typed_text + "S";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0)
                                        typed_text = typed_text + "T";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0)
                                        typed_text = typed_text + "U";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
                                        typed_text = typed_text + "V";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
                                        typed_text = typed_text + "Z";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0)
                                        typed_text = typed_text + "X";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0)
                                        typed_text = typed_text + "Y";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)
                                        typed_text = typed_text + "W";
                                }
                                else
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
                                        typed_text = typed_text + "A";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0)
                                        typed_text = typed_text + "B";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0)
                                        typed_text = typed_text + "C";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0)
                                        typed_text = typed_text + "D";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0)
                                        typed_text = typed_text + "E";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0)
                                        typed_text = typed_text + "F";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0)
                                        typed_text = typed_text + "G";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0)
                                        typed_text = typed_text + "H";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0)
                                        typed_text = typed_text + "I";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0)
                                        typed_text = typed_text + "J";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0)
                                        typed_text = typed_text + "K";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0)
                                        typed_text = typed_text + "L";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0)
                                        typed_text = typed_text + "M";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0)
                                        typed_text = typed_text + "N";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0)
                                        typed_text = typed_text + "O";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0)
                                        typed_text = typed_text + "P";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0)
                                        typed_text = typed_text + "Q";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0)
                                        typed_text = typed_text + "R";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
                                        typed_text = typed_text + "S";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0)
                                        typed_text = typed_text + "T";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0)
                                        typed_text = typed_text + "U";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
                                        typed_text = typed_text + "V";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
                                        typed_text = typed_text + "W";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0)
                                        typed_text = typed_text + "X";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0)
                                        typed_text = typed_text + "Y";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)
                                        typed_text = typed_text + "Z";
                                }
                            }
                            else
                            {
                                if (!MainForm->config->GetKeyboardLayout())
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + "-";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "=";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "[";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "]";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + ";";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "\\";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ",";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ".";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "/";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "@";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + "/";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "*";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + "+";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "<";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ",";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ".";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "-";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "~";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + "+";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "\xe5";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "\\";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + "\xf6";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "\xe4";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ",";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + ".";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "-";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
                                        typed_text = typed_text + ")";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
                                        typed_text = typed_text + "-";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "[";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
                                        typed_text = typed_text + "]";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
                                        typed_text = typed_text + "m";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
                                        typed_text = typed_text + "?";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
                                        typed_text = typed_text + ";";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
                                        typed_text = typed_text + "/";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
                                        typed_text = typed_text + "=";
                                }
                                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
                                        typed_text = typed_text + "&";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
                                        typed_text = typed_text + "\xe9";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
                                        typed_text = typed_text + "'";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
                                        typed_text = typed_text + "(";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
                                        typed_text = typed_text + "\xa7";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
                                        typed_text = typed_text + "\xe8";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
                                        typed_text = typed_text + "!";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
                                        typed_text = typed_text + "\xe7";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
                                        typed_text = typed_text + "\xe0";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
                                        typed_text = typed_text + "q";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0)
                                        typed_text = typed_text + "b";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0)
                                        typed_text = typed_text + "c";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0)
                                        typed_text = typed_text + "d";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0)
                                        typed_text = typed_text + "e";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0)
                                        typed_text = typed_text + "f";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0)
                                        typed_text = typed_text + "g";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0)
                                        typed_text = typed_text + "h";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0)
                                        typed_text = typed_text + "i";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0)
                                        typed_text = typed_text + "j";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0)
                                        typed_text = typed_text + "k";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0)
                                        typed_text = typed_text + "l";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0)
                                        typed_text = typed_text + ",";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0)
                                        typed_text = typed_text + "n";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0)
                                        typed_text = typed_text + "o";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0)
                                        typed_text = typed_text + "p";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0)
                                        typed_text = typed_text + "a";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0)
                                        typed_text = typed_text + "r";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
                                        typed_text = typed_text + "s";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0)
                                        typed_text = typed_text + "t";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0)
                                        typed_text = typed_text + "u";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
                                        typed_text = typed_text + "v";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
                                        typed_text = typed_text + "z";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0)
                                        typed_text = typed_text + "x";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0)
                                        typed_text = typed_text + "y";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)
                                        typed_text = typed_text + "w";
                                }
                                else
                                {
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
                                        typed_text = typed_text + "1";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
                                        typed_text = typed_text + "2";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
                                        typed_text = typed_text + "3";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
                                        typed_text = typed_text + "4";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
                                        typed_text = typed_text + "5";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
                                        typed_text = typed_text + "6";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
                                        typed_text = typed_text + "7";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
                                        typed_text = typed_text + "8";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
                                        typed_text = typed_text + "9";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
                                        typed_text = typed_text + "0";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
                                        typed_text = typed_text + "a";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0)
                                        typed_text = typed_text + "b";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0)
                                        typed_text = typed_text + "c";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0)
                                        typed_text = typed_text + "d";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0)
                                        typed_text = typed_text + "e";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0)
                                        typed_text = typed_text + "f";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0)
                                        typed_text = typed_text + "g";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0)
                                        typed_text = typed_text + "h";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0)
                                        typed_text = typed_text + "i";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0)
                                        typed_text = typed_text + "j";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0)
                                        typed_text = typed_text + "k";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0)
                                        typed_text = typed_text + "l";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0)
                                        typed_text = typed_text + "m";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0)
                                        typed_text = typed_text + "n";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0)
                                        typed_text = typed_text + "o";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0)
                                        typed_text = typed_text + "p";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0)
                                        typed_text = typed_text + "q";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0)
                                        typed_text = typed_text + "r";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
                                        typed_text = typed_text + "s";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0)
                                        typed_text = typed_text + "t";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0)
                                        typed_text = typed_text + "u";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
                                        typed_text = typed_text + "v";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
                                        typed_text = typed_text + "w";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0)
                                        typed_text = typed_text + "x";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0)
                                        typed_text = typed_text + "y";
                                    if ((MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)
                                        typed_text = typed_text + "z";
                                }
                            }
                            if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
                            {
                                if ((MainForm->DIKeyboardState1->Keys[DIK_DECIMAL] & 0x80) != 0)
                                    typed_text = typed_text + ".";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD9] & 0x80) != 0)
                                    typed_text = typed_text + "9";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD8] & 0x80) != 0)
                                    typed_text = typed_text + "8";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD7] & 0x80) != 0)
                                    typed_text = typed_text + "7";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD6] & 0x80) != 0)
                                    typed_text = typed_text + "6";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD5] & 0x80) != 0)
                                    typed_text = typed_text + "5";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD4] & 0x80) != 0)
                                    typed_text = typed_text + "4";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD3] & 0x80) != 0)
                                    typed_text = typed_text + "3";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD2] & 0x80) != 0)
                                    typed_text = typed_text + "2";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD1] & 0x80) != 0)
                                    typed_text = typed_text + "1";
                                if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD0] & 0x80) != 0)
                                    typed_text = typed_text + "0";
                            }
                            if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
                                MainForm->edits->TypeLineBreak();
                            if ((MainForm->DIKeyboardState1->Keys[DIK_ESCAPE] & 0x80) != 0)
                                MainForm->edits->ClearFocusedText();
                            if (backspace_delay >= 1)
                                backspace_delay--;
                            if ((MainForm->DIKeyboardState1->Keys[DIK_BACK] & 0x80) != 0)
                            {
                                if (!typed_text.Length() && backspace_delay <= 2)
                                {
                                    if (backspace_delay >= 1)
                                        backspace_delay = 6;
                                    else
                                        backspace_delay = 24;
                                    MainForm->edits->Backspace();
                                }
                            }
                            if (repeat_delay >= 1)
                                repeat_delay--;
                            if (repeat_interval >= 1)
                                repeat_interval--;
                            // A held key types once, and again when the repeat delay runs out.
                            // It then types once per repeat interval. Two keys at once type nothing.
                            String char_to_type = "";
                            if (typed_text.Length() == 1)
                            {
                                String key_upper = UpperCase(String(typed_text[1]));
                                String key_char = String(typed_text[1]);
                                if (repeat_delay >= 1 && key_upper == last_key)
                                {
                                    if (repeat_interval <= 0)
                                    {
                                        if (repeat_key == key_upper)
                                        {
                                            repeat_interval = 5;
                                            last_key = key_upper;
                                            char_to_type = key_char;
                                        }
                                    }
                                }
                                else
                                {
                                    // Letting go of Shift while a key is held does not type its unshifted character.
                                    if ((last_key == (char)63 && key_upper == (char)47) ||
                                        (last_key == (char)59 && key_upper == (char)58) ||
                                        (last_key == (char)58 && key_upper == (char)59) ||
                                        (last_key == (char)33 && key_upper == (char)49) ||
                                        (last_key == (char)64 && key_upper == (char)50) ||
                                        (last_key == (char)35 && key_upper == (char)51) ||
                                        (last_key == (char)36 && key_upper == (char)52) ||
                                        (last_key == (char)37 && key_upper == (char)53) ||
                                        (last_key == (char)94 && key_upper == (char)54) ||
                                        (last_key == (char)38 && key_upper == (char)55) ||
                                        (last_key == (char)42 && key_upper == (char)56) ||
                                        (last_key == (char)40 && key_upper == (char)57) ||
                                        (last_key == (char)41 && key_upper == (char)48) ||
                                        (last_key == (char)60 && key_upper == (char)44) ||
                                        (last_key == (char)62 && key_upper == (char)46) ||
                                        (last_key == (char)43 && key_upper == (char)61))
                                    {
                                        repeat_interval = 5;
                                        last_key = key_upper;
                                        char_to_type = "";
                                    }
                                    else
                                    {
                                        if (last_key == key_upper)
                                            repeat_key = key_upper;
                                        else
                                            repeat_key = "";
                                        repeat_delay = 40;
                                        last_key = key_upper;
                                        char_to_type = key_char;
                                    }
                                }
                            }
                            else
                            {
                                repeat_delay = 0;
                                last_key = "";
                                repeat_key = "";
                            }
                            MainForm->edits->TypeText(char_to_type);
                        }
                    }
                }
            }
            else
            {
                repeat_delay = 0;
                last_key = "";
                repeat_key = "";
                repeat_key = "";
                last_key = "";
                ctrl_attacked = false;
                arrow_held = false;
                two_arrows_held = false;
            }
        }
        else
        {
            repeat_delay = 0;
            last_key = "";
            repeat_key = "";
            repeat_key = "";
            last_key = "";
            ctrl_attacked = false;
            arrow_held = false;
            two_arrows_held = false;
        }
    }
    catch (...)
    {
        repeat_delay = 0;
        last_key = "";
        repeat_key = "";
        repeat_key = "";
        last_key = "";
        ctrl_attacked = false;
        arrow_held = false;
        two_arrows_held = false;
    }
}
