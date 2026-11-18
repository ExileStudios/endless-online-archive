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
#include "Ctrl_stats.h"
#include "Ctrl_status.h"
#include <stdlib.h>
#include <vector.h>
#include "Ctrl_chest.h"
#include "Ctrl_gamebars.h"
#include "Ctrl_group.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_msgboard.h"
#include "Ctrl_shop.h"
#include "Ctrl_spells.h"
#include "Ctrl_trade.h"
#include <dinput.h>
#include "Pathfinder.h"
#include "Ctrl_hotkeys.h"
#include "Ctrl_crafter.h"
#include "Ctrl_visualdoc.h"

#pragma package(smart_init)

KeyBoard::KeyBoard()
{
    tab_wait = 0;
    backspace_wait = 0;
    key_delay = 0;
    menu_enter_wait = 0;
    enter_wait = 0;
    grid_enter_wait = 0;
    walk_key_held = false;
    wasd_key_used = false;
    attack_key_used = false;
    space_key_used = false;
    delete_pressed = false;
    for (int i = 0; i < 69; i++)
        key_waits[i] = 0;
    for (int j = 0; j < 4; j++)
        key_flags[j] = false;
}

KeyBoard::~KeyBoard()
{
}

char KeyBoard::WasDeletePressed()
{
    bool down = delete_pressed;
    delete_pressed = false;
    return down;
}

// Reads the keyboard while the window is active and hands the keys to the menu, edit, game and typing handlers in
// turn; any failure lets go of the modifier keys.
void KeyBoard::PollKeyboard()
{
    if (grid_enter_wait > 0)
        grid_enter_wait--;
    if (!MainForm->isActive())
        return;
    try
    {
        if (!MainForm->Dx_InputDevice1->Created)
        {
            MainForm->Dx_InputDevice1->Create(GUID_SysKeyboard, MainForm->Dx_Input1);
            return;
        }
        if (!MainForm->Dx_InputDevice1->Poll())
            return;
        if (!MainForm->Dx_InputDevice1->GetDeviceState((TDIData *)MainForm->DIKeyboardState1))
            return;
        // Each handler in turn can end the poll: the menu keys, the edit keys, the game keys, then typing.
        if (!HandleMenuKeys())
            return;
        if (!HandleEditKeys())
            return;
        if (!HandleGameKeys())
            return;
        if (!HandleTypedKeys())
            return;
    }
    catch (...)
    {
        ReleaseModifierKeys();
    }
}

// Walks in the direction of the held arrow keys, the last pressed one winning when several are held, and cancels a
// clicked path.
bool KeyBoard::UpdateWalkDirection()
{
    unsigned char held_count = 0;
    if (key_flags[1])
    {
        walk_direction = 1;
        held_count++;
    }
    if (key_flags[2])
    {
        walk_direction = 2;
        held_count++;
    }
    if (key_flags[3])
    {
        walk_direction = 3;
        held_count++;
    }
    if (key_flags[0])
    {
        walk_direction = 0;
        held_count++;
    }
    if (held_count > 0)
    {
        // With two keys held, the one pressed later wins over the one held first.
        if (held_count == 2)
        {
            if (last_walk_direction == 1 && key_flags[1])
            {
                if (key_flags[2])
                    walk_direction = 2;
                if (key_flags[3])
                    walk_direction = 3;
                if (key_flags[0])
                    walk_direction = 0;
            }
            if (last_walk_direction == 2 && key_flags[2])
            {
                if (key_flags[1])
                    walk_direction = 1;
                if (key_flags[3])
                    walk_direction = 3;
                if (key_flags[0])
                    walk_direction = 0;
            }
            if (last_walk_direction == 3 && key_flags[3])
            {
                if (key_flags[1])
                    walk_direction = 1;
                if (key_flags[2])
                    walk_direction = 2;
                if (key_flags[0])
                    walk_direction = 0;
            }
            if (!last_walk_direction && key_flags[0])
            {
                if (key_flags[1])
                    walk_direction = 1;
                if (key_flags[2])
                    walk_direction = 2;
                if (key_flags[3])
                    walk_direction = 3;
            }
        }
        else
            last_walk_direction = walk_direction;
        if (MainForm->net_client->CanAct())
        {
            MainForm->map->path_finder->CancelPath();
            MainForm->animation->game->Walk(walk_direction);
        }
        return true;
    }
    return false;
}

// Handles the keys of the menu screens: Enter, Escape, pasting with Ctrl+V into the focused box, the number keys that
// pick a character, and the letters of the screen that asks for them.
bool KeyBoard::HandleMenuKeys()
{
    unsigned int clip_handle;
    int clip_text;
    unsigned int clip_memory;
    if (MainForm->animation->GetScreenMode() != ScreenMode_Menu)
        return true;
    try
    {
        if (menu_enter_wait > 0)
            menu_enter_wait--;
        if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
        {
            if (!menu_enter_wait)
            {
                menu_enter_wait = 200;
                if (!key_waits[60])
                {
                    if (MainForm->animation->menu->is_message_open)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                        MainForm->animation->menu->is_message_open = false;
                        MainForm->edits->caret_enabled = true;
                        return true;
                    }
                    if (MainForm->animation->GetSubState() == MenuScreen_Main)
                        MainForm->login_screen_delay = 1;
                }
            }
        }
        else
            menu_enter_wait = 0;
        if (((MainForm->DIKeyboardState1->Keys[DIK_LCONTROL] & 0x80) != 0 ||
            (MainForm->DIKeyboardState1->Keys[DIK_RCONTROL] & 0x80) != 0) &&
            (MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0 && MainForm->animation->IsChatAllowed() &&
            TakeKeyRepeat(43))
        {
            MainForm->animation->RecordChatMessage();
            String pasted;
            Clipboard()->Open();
            try
            {
                clip_handle = Clipboard()->GetAsHandle(CF_TEXT);
                clip_memory = clip_handle;
                clip_text = (int)GlobalLock((void *)clip_memory);
                pasted = (char *)clip_text;
                GlobalUnlock((void *)clip_memory);
            }
            catch (...)
            {
            }
            Clipboard()->Close();
            if (pasted.Length() > 64)
                pasted = pasted.SubString(1, 64);
            if (pasted.Length() > 0)
            {
                MainForm->edits->TypeText(pasted);
                return true;
            }
        }
        if ((MainForm->DIKeyboardState1->Keys[DIK_ESCAPE] & 0x80) != 0)
        {
            if (MainForm->animation->menu->is_message_open)
            {
                MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                MainForm->animation->menu->is_message_open = false;
                MainForm->edits->caret_enabled = true;
            }
            else
                MainForm->edits->ClearFocusedText();
        }
        if (MainForm->animation->GetSubState() == MenuScreen_CharacterSelect)
        {
            if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0 ||
                (MainForm->DIKeyboardState1->Keys[DIK_NUMPAD0] & 0x80) != 0 ||
                (MainForm->DIKeyboardState1->Keys[DIK_NUMPAD1] & 0x80) != 0)
            {
                if (!MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotFirst) &&
                    !MainForm->animation->menu->is_message_open && !MainForm->animation->menu->is_form_open &&
                    !MainForm->animation->menu->is_loading_world)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotFirst));
                }
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0 ||
                (MainForm->DIKeyboardState1->Keys[DIK_NUMPAD2] & 0x80) != 0)
            {
                if (!MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotSecond) &&
                    !MainForm->animation->menu->is_message_open && !MainForm->animation->menu->is_form_open &&
                    !MainForm->animation->menu->is_loading_world)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotSecond));
                }
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0 ||
                (MainForm->DIKeyboardState1->Keys[DIK_NUMPAD3] & 0x80) != 0)
            {
                if (!MainForm->animation->menu->character_select_cooldown &&
                    !MainForm->players->GetDrawGroup(PlayerId_SlotThird) &&
                    !MainForm->animation->menu->is_message_open && !MainForm->animation->menu->is_form_open &&
                    !MainForm->animation->menu->is_loading_world)
                {
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->animation->menu->is_form_open = false;
                    MainForm->animation->menu->character_select_cooldown = CHARACTER_SELECT_COOLDOWN_TICKS;
                    MainForm->net_client->sender->SelectCharacter(MainForm->players->GetCharacterId(PlayerId_SlotThird));
                }
            }
        }
        // The password grid.
        // Enter takes the letter under the cursor and Backspace removes the last one.
        // The arrows move the cursor.
        // Typing a letter or digit brings up the message box that clicking the grid shows.
        if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid &&
            !MainForm->animation->menu->is_message_open)
        {
            char unhandled = true;
            if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
            {
                unhandled = false;
                if (grid_enter_wait <= 0)
                {
                    grid_enter_wait = 24;
                    MainForm->animation->menu->AddSelectedLetter();
                }
                else
                    grid_enter_wait = 24;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_BACK] & 0x80) != 0)
            {
                unhandled = false;
                if (grid_enter_wait <= 0)
                {
                    grid_enter_wait = 24;
                    MainForm->sfx->PlaySfx(SfxSound_Send);
                    MainForm->animation->menu->DeleteLastLetter();
                }
                else
                    grid_enter_wait = 24;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_LEFT] & 0x80) != 0)
            {
                unhandled = false;
                if (grid_enter_wait <= 0)
                {
                    grid_enter_wait = 16;
                    if (MainForm->animation->menu->grid_column > 0)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                        MainForm->animation->menu->grid_column--;
                        MainForm->animation->menu->cursor_blink = 5;
                    }
                }
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_RIGHT] & 0x80) != 0)
            {
                unhandled = false;
                if (grid_enter_wait <= 0)
                {
                    grid_enter_wait = 16;
                    if (MainForm->animation->menu->grid_column < 5)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                        MainForm->animation->menu->grid_column++;
                        MainForm->animation->menu->cursor_blink = 5;
                    }
                }
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_UP] & 0x80) != 0)
            {
                unhandled = false;
                if (grid_enter_wait <= 0)
                {
                    grid_enter_wait = 16;
                    if (MainForm->animation->menu->grid_row > 0)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
                        MainForm->animation->menu->grid_row--;
                        MainForm->animation->menu->cursor_blink = 5;
                    }
                }
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_DOWN] & 0x80) != 0)
            {
                unhandled = false;
                if (grid_enter_wait <= 0)
                {
                    grid_enter_wait = 16;
                    if (MainForm->animation->menu->grid_row < 5)
                    {
                        MainForm->sfx->PlaySfx(SfxSound_ItemPickup);
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
            if (unhandled)
                grid_enter_wait = 0;
        }
    }
    catch (...)
    {
        ReleaseModifierKeys();
        return false;
    }
    return true;
}

// The game's shortcut keys: Escape clears the typed text or closes the top window, a key takes a screenshot, and Alt
// with a letter opens a panel, the paperdoll, the hotkeys or a chat tab.
bool KeyBoard::HandleGameKeys()
{
    unsigned int clip_handle;
    int clip_text;
    unsigned int clip_memory;
    if (MainForm->animation->GetScreenMode() != ScreenMode_Game)
        return true;
    try
    {
        if ((MainForm->DIKeyboardState1->Keys[DIK_DELETE] & 0x80) != 0)
            delete_pressed = true;
        // Scroll Lock saves a screenshot.
        if ((MainForm->DIKeyboardState1->Keys[DIK_SCROLL] & 0x80) != 0)
            MainForm->gfx->interface_graphics->SaveScreenshot();
        if ((MainForm->DIKeyboardState1->Keys[DIK_ESCAPE] & 0x80) != 0)
        {
            if (TakeKeyRepeat(34) && !MainForm->edits->ClearFocusedText())
                MainForm->animation->CloseTopWindow();
        }
        else
            ResetKeyRepeat(34);
        if (IsAnyWindowOpen())
            return true;
        if (key_delay > 0)
            key_delay--;
        if ((MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0 ||
            (MainForm->DIKeyboardState1->Keys[DIK_RMENU] & 0x80) != 0)
        {
            if ((MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->interface_controller->game_bars->ShowAllTexts();
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                {
                    MainForm->animation->hotkeys->Close();
                    MainForm->net_client->sender->RequestPaperdoll(MainForm->animation->game->player_menu_target_id);
                }
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                {
                    MainForm->animation->paperdoll->Close();
                    MainForm->animation->hotkeys->Open();
                }
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->interface_controller->game_bars->RequestQuestProgress(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenInventoryPanel(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenPlayerlistPanel(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenSpellPanel(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenSkillPanel(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                {
                    MainForm->mouse->OpenChatPanel(false);
                    MainForm->animation->interface_controller->chat->ShowLocalTab();
                }
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                {
                    MainForm->mouse->OpenChatPanel(false);
                    MainForm->animation->interface_controller->chat->ShowGlobalTab();
                }
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                {
                    MainForm->mouse->OpenChatPanel(false);
                    MainForm->animation->interface_controller->chat->ShowGroupTab();
                }
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                {
                    MainForm->mouse->OpenChatPanel(false);
                    MainForm->animation->interface_controller->chat->ShowSystemTab();
                }
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Happy);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Depressed);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Sad);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Angry);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Confused);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Surprised);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Hearts);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Moon);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Suicidal);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Playful);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->animation->game->PlayEmote(Emote_Embarrassed);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenStatusPanel(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenGroupPanel(false);
                return false;
            }
            if ((MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0)
            {
                if (TakeKeyDelay())
                    MainForm->mouse->OpenSettingsPanel(false);
                return false;
            }
        }
        walk_key_held = false;
        wasd_key_used = false;
        attack_key_used = false;
        space_key_used = false;
        // Ctrl and V paste the clipboard into an empty chat line.
        // Ctrl attacks; Ctrl and V paste the clipboard into an empty chat line.
        if ((MainForm->DIKeyboardState1->Keys[DIK_LCONTROL] & 0x80) != 0 ||
            (MainForm->DIKeyboardState1->Keys[DIK_RCONTROL] & 0x80) != 0)
        {
            if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
            {
                if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
                {
                    if (MainForm->edits->IsFocused(EditId_Chat) && MainForm->edits->GetTextLength(EditId_Chat) < 1 &&
                        MainForm->animation->IsChatAllowed())
                    {
                        MainForm->animation->RecordChatMessage();
                        String pasted;
                        Clipboard()->Open();
                        try
                        {
                            clip_handle = Clipboard()->GetAsHandle(CF_TEXT);
                            clip_memory = clip_handle;
                            clip_text = (int)GlobalLock((void *)clip_memory);
                            pasted = (char *)clip_text;
                            GlobalUnlock((void *)clip_memory);
                        }
                        catch (...)
                        {
                        }
                        Clipboard()->Close();
                        if (pasted.Length() > 128)
                            pasted = pasted.SubString(1, 128);
                        if (pasted.Length() > 0)
                        {
                            MainForm->edits->TypeText(pasted);
                            TakeKeyRepeat(43);
                            return true;
                        }
                    }
                    TakeKeyRepeat(43);
                    return false;
                }
            }
            else if (MainForm->net_client->CanAct())
            {
                MainForm->map->path_finder->CancelPath();
                MainForm->animation->game->AttackForward(false);
                attack_key_used = true;
            }
        }
        if ((MainForm->DIKeyboardState1->Keys[DIK_SPACE] & 0x80) != 0)
        {
            if (MainForm->edits->GetText(EditId_Chat).Length() <= 0)
            {
                space_key_used = true;
                if (MainForm->net_client->CanAct())
                {
                    MainForm->map->path_finder->CancelPath();
                    MainForm->animation->game->AttackForward(false);
                }
            }
        }
        if (!attack_key_used && !space_key_used)
        {
            for (int i = 0; i < 4; i++)
                key_flags[i] = false;
            key_flags[0] = (MainForm->DIKeyboardState1->Keys[DIK_DOWN] & 0x80) != 0;
            key_flags[1] = (MainForm->DIKeyboardState1->Keys[DIK_LEFT] & 0x80) != 0;
            key_flags[2] = (MainForm->DIKeyboardState1->Keys[DIK_UP] & 0x80) != 0;
            key_flags[3] = (MainForm->DIKeyboardState1->Keys[DIK_RIGHT] & 0x80) != 0;
            if (MainForm->config->GetWasdKeys())
            {
                if (!MainForm->edits->GetText(EditId_Chat).Length() && !enter_wait)
                {
                    if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) == 0 &&
                        (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) == 0)
                    {
                        if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
                        {
                            key_flags[2] = true;
                            wasd_key_used = true;
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
                        {
                            key_flags[1] = true;
                            wasd_key_used = true;
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0)
                        {
                            key_flags[3] = true;
                            wasd_key_used = true;
                        }
                        if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
                        {
                            key_flags[0] = true;
                            wasd_key_used = true;
                        }
                    }
                }
            }
            walk_key_held = UpdateWalkDirection();
        }
        // F1 to F8 use the hotkey in that column: the first row, or the second with Shift or Alt.
        // F1 to F8 use the spell in that column, from the second row with Shift.
        if ((MainForm->DIKeyboardState1->Keys[DIK_F1] & 0x80) != 0)
        {
            if (TakeKeyRepeat(61))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(0, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(0, 0);
            }
        }
        else
            ResetKeyRepeat(61);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F2] & 0x80) != 0)
        {
            if (TakeKeyRepeat(62))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(1, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(1, 0);
            }
        }
        else
            ResetKeyRepeat(62);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F3] & 0x80) != 0)
        {
            if (TakeKeyRepeat(63))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(2, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(2, 0);
            }
        }
        else
            ResetKeyRepeat(63);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F4] & 0x80) != 0)
        {
            if (TakeKeyRepeat(64))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(3, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(3, 0);
            }
        }
        else
            ResetKeyRepeat(64);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F5] & 0x80) != 0)
        {
            if (TakeKeyRepeat(65))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(4, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(4, 0);
            }
        }
        else
            ResetKeyRepeat(65);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F6] & 0x80) != 0)
        {
            if (TakeKeyRepeat(66))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(5, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(5, 0);
            }
        }
        else
            ResetKeyRepeat(66);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F7] & 0x80) != 0)
        {
            if (TakeKeyRepeat(67))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(6, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(6, 0);
            }
        }
        else
            ResetKeyRepeat(67);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F8] & 0x80) != 0)
        {
            if (TakeKeyRepeat(68))
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0 ||
                    (MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) != 0)
                    MainForm->animation->hotkeys->UseHotkey(7, 1);
                else
                    MainForm->animation->hotkeys->UseHotkey(7, 0);
            }
        }
        else
            ResetKeyRepeat(68);
        // F11 sits down or stands up.
        if ((MainForm->DIKeyboardState1->Keys[DIK_F11] & 0x80) != 0)
        {
            MainForm->map->path_finder->CancelPath();
            MainForm->animation->game->ToggleSit(MainForm->map->GetCursorX(), MainForm->map->GetCursorY());
        }
        // F12 asks for everything in view again, once per refresh delay.
        if ((MainForm->DIKeyboardState1->Keys[DIK_F12] & 0x80) != 0 && !MainForm->animation->IsRefreshDelayed())
        {
            MainForm->map->path_finder->CancelPath();
            MainForm->net_client->sender->RequestRefresh();
            MainForm->animation->StartRefreshDelay(14);
        }
        // The number pad keys play the emotes.
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD1] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Happy);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD2] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Depressed);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD3] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Sad);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD4] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Angry);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD5] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Confused);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD6] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Surprised);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD7] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Hearts);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD8] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Moon);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD9] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Suicidal);
        if ((MainForm->DIKeyboardState1->Keys[DIK_DECIMAL] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Embarrassed);
        if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD0] & 0x80) != 0)
            MainForm->animation->game->PlayEmote(Emote_Playful);
    }
    catch (...)
    {
        ReleaseModifierKeys();
        return false;
    }
    return true;
}

// The keys of the edit boxes: Tab moves to the next box, and Enter submits the focused one; a chat line is sent, or
// run when it is one of the # commands, such as finding a player or looking up an item or npc.
bool KeyBoard::HandleEditKeys()
{
    int days;
    int minutes;
    if (!MainForm->edits->GetCount())
        return true;
    if (!MainForm->edits->caret_enabled)
        return true;
    try
    {
        if (tab_wait > 0)
            tab_wait--;
        // Tab moves to the next box; with a single box it toggles the minimap, unless Alt is held.
        if ((MainForm->DIKeyboardState1->Keys[DIK_TAB] & 0x80) != 0 && !tab_wait)
        {
            if (MainForm->edits->GetCount() >= 2)
            {
                MainForm->edits->FocusNext();
                MainForm->sfx->PlaySfx(SfxSound_Select);
                tab_wait = 60;
            }
            else
            {
                if ((MainForm->DIKeyboardState1->Keys[DIK_LMENU] & 0x80) == 0 &&
                    (MainForm->DIKeyboardState1->Keys[DIK_RMENU] & 0x80) == 0)
                    MainForm->mouse->ToggleMinimap(true);
                tab_wait = 60;
            }
        }
        if (enter_wait > 0)
            enter_wait--;
        if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
        {
            enter_wait = 60;
            MainForm->animation->game->ResetIdleTimer();
            if (MainForm->animation->CanSendChat())
            {
                MainForm->edits->TypeLineBreak();
                if (!MainForm->edits->IsFocused(EditId_Chat))
                    MainForm->animation->StartChatDelay();
                else if (MainForm->edits->GetText(EditId_Chat).Length() > 0)
                {
                    if (MainForm->animation->IsChatAllowed())
                    {
                        MainForm->animation->StartChatDelay();
                        MainForm->animation->RecordChatMessage();
                        if (MainForm->edits->GetText(EditId_Chat)[1] != '!' &&
                            MainForm->edits->GetText(EditId_Chat)[1] != '~' &&
                            MainForm->edits->GetText(EditId_Chat)[1] != '@' &&
                            MainForm->edits->GetText(EditId_Chat)[1] != '+' &&
                            MainForm->edits->GetText(EditId_Chat)[1] != '&' &&
                            String(MainForm->edits->GetText(EditId_Chat)[1]) != "'")
                        {
                            char handled = false;
                            if (MainForm->edits->GetText(EditId_Chat).Length() > 1)
                            {
                                if (MainForm->edits->GetText(EditId_Chat)[1] == '$' &&
                                    MainForm->edits->GetText(EditId_Chat)[2] != 'v')
                                {
                                    if (MainForm->animation->IsCommandDelayed())
                                        MainForm->edits->SetText(EditId_Chat, "");
                                    else
                                        MainForm->animation->RestartCommandDelay();
                                }
                            }
                            if (MainForm->edits->GetText(EditId_Chat) ==
                                MainForm->net_client->DeobfuscateString("h8l,X|"))
                            {
                                handled = true;
                                MainForm->sfx->PlaySfx(SfxSound_Notice);
                                TDateTime now = Now();
                                TTimeStamp start_stamp;
                                start_stamp =
                                    DateTimeToTimeStamp(MainForm->animation->interface_controller->status->usage_start_time);
                                TTimeStamp now_stamp;
                                now_stamp = DateTimeToTimeStamp(now);
                                int elapsed_ms = now_stamp.Time - start_stamp.Time;
                                days = now_stamp.Date - start_stamp.Date;
                                minutes = elapsed_ms / 60000 + 1440 * days;
                                minutes += MainForm->animation->interface_controller->status->usage_minutes;
                                int usage_hours = minutes / 60;
                                int usage_mins = minutes % 60;
                                String usage_text =
                                    IntToStr(usage_hours) + MainForm->net_client->DeobfuscateString(" qZ-e") +
                                        IntToStr(usage_mins) + MainForm->net_client->DeobfuscateString("C1d2");
                                MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Lookup,
                                    CHAT_COLOR_DEFAULT, MainForm->net_client->DeobfuscateString("`:Y,TL"),
                                    MainForm->net_client->DeobfuscateString(" e :f>Z* BUD") + usage_text, true);
                            }
                            if (MainForm->edits->GetText(EditId_Chat).Length() > 6)
                            {
                                if (MainForm->edits->GetText(EditId_Chat).SubString(1, 5) ==
                                    MainForm->net_client->DeobfuscateString("i1d9N"))
                                {
                                    handled = true;
                                    String find_name = MainForm->edits->GetText(EditId_Chat);
                                    find_name.Delete(1, 6);
                                    if (find_name.Length() > 3 && find_name.Length() < 17)
                                        MainForm->net_client->sender->SendFindPlayer(find_name);
                                }
                            }
                            if (MainForm->edits->GetText(EditId_Chat) ==
                                MainForm->net_client->DeobfuscateString("j0a|") ||
                                MainForm->edits->GetText(EditId_Chat) ==
                                    MainForm->net_client->DeobfuscateString("_0d+l<^3N"))
                            {
                                handled = true;
                                String location_text =
                                    MainForm->strings->GetText(GameText_YourCurrentLocationIs) +
                                        IntToStr(MainForm->map->GetMapId()) + "  x:";
                                location_text = location_text + IntToStr(MainForm->map->GetCenterX()) + "  y:";
                                location_text = location_text + IntToStr(MainForm->map->GetCenterY());
                                MainForm->sfx->PlaySfx(SfxSound_Notice);
                                MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Lookup,
                                    CHAT_COLOR_DEFAULT, MainForm->net_client->DeobfuscateString("`:Y,TL"),
                                    location_text, true);
                            }
                            if (MainForm->edits->GetText(EditId_Chat) ==
                                MainForm->net_client->DeobfuscateString("f1d/N"))
                            {
                                handled = true;
                                MainForm->sfx->PlaySfx(SfxSound_Select);
                                MainForm->animation->interface_controller->status->ping_sent_time = Now();
                                MainForm->net_client->sender->SendPingRequest();
                            }
                            if (MainForm->edits->GetText(EditId_Chat) ==
                                MainForm->net_client->DeobfuscateString("a3l(^1N"))
                            {
                                handled = true;
                                MainForm->sfx->PlaySfx(SfxSound_Select);
                                MainForm->animation->game->ToggleGhostWalk();
                            }
                            if (MainForm->edits->GetText(EditId_Chat) ==
                                MainForm->net_client->DeobfuscateString("h1d8_:N"))
                            {
                                handled = true;
                                MainForm->sfx->PlaySfx(SfxSound_Notice);
                                String info_text = "VDACL";
                                info_text.Delete(2, 2);
                                if (MainForm->config->GetUseBitBltDraw())
                                    info_text = MainForm->net_client->DeobfuscateString("([*");
                                MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Lookup,
                                    CHAT_COLOR_DEFAULT, MainForm->net_client->DeobfuscateString("`:Y,TL"),
                                    MainForm->net_client->DeobfuscateString(" e 1^6Z-h) +_:d3j p'r") +
                                    MainForm->VersionString(MainForm->version_major, MainForm->version_minor,
                                    MainForm->version_build, MainForm->version_letter), true);
                                MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Lookup,
                                    CHAT_COLOR_DEFAULT, MainForm->net_client->DeobfuscateString("`:Y,TL"),
                                    MainForm->net_client->DeobfuscateString(" e :_6f1h [:i1h- BUD") + info_text, true);
                            }
                            if (!handled)
                            {
                                String say_text = MainForm->edits->GetText(EditId_Chat);
                                if (MainForm->animation->interface_controller->chat->IsDrunk())
                                    say_text = MainForm->net_client->MakeDrunkText(say_text);
                                String say_shown = MainForm->net_client->CensorText(say_text);
                                if (say_shown.Length() >= 1)
                                {
                                    MainForm->net_client->sender->SendPublicChat(say_text);
                                    MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_None,
                                        CHAT_COLOR_DEFAULT,
                                        MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                        say_shown, true);
                                    MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(), say_shown, -1);
                                }
                                else
                                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                        MainForm->strings->GetText(GameText_YourMindPreventsYou));
                            }
                        }
                        if (MainForm->edits->GetText(EditId_Chat).Length() > 1)
                        {
                            if (MainForm->edits->GetText(EditId_Chat)[1] == '+')
                            {
                                if (MainForm->net_client->GetAdminLevel() > 0)
                                {
                                    String admin_text = MainForm->edits->GetText(EditId_Chat);
                                    String admin_shown =
                                        MainForm->net_client->CensorText(admin_text.SubString(2,
                                            admin_text.Length() - 1));
                                    if (admin_shown.Length() >= 1)
                                    {
                                        MainForm->net_client->sender->SendAdminChat(admin_text.SubString(2,
                                            admin_text.Length() - 1));
                                        MainForm->sfx->PlaySfx(SfxSound_ChannelMessage);
                                        MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Admin,
                                            CHAT_COLOR_ADMIN,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            admin_shown, true);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                                else
                                {
                                    String plus_say_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        plus_say_text = MainForm->net_client->MakeDrunkText(plus_say_text);
                                    String plus_say_shown = MainForm->net_client->CensorText(plus_say_text);
                                    if (plus_say_shown.Length() >= 1)
                                    {
                                        MainForm->net_client->sender->SendPublicChat(plus_say_text);
                                        MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_None,
                                            CHAT_COLOR_DEFAULT,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            plus_say_shown, true);
                                        MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(),
                                            plus_say_shown, -1);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                            }
                            if (MainForm->edits->GetText(EditId_Chat)[1] == '~')
                            {
                                if (MainForm->animation->IsGlobalMessageAllowed())
                                {
                                    if (MainForm->net_client->GetAdminLevel() <= 0)
                                        MainForm->animation->RecordGlobalMessage();
                                    String global_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        global_text = MainForm->net_client->MakeDrunkText(global_text);
                                    String global_shown =
                                        MainForm->net_client->CensorText(global_text.SubString(2,
                                            global_text.Length() - 1));
                                    if (global_shown.Length() >= 1)
                                    {
                                        MainForm->net_client->sender->SendGlobalChat(global_text.SubString(2,
                                            global_text.Length() - 1));
                                        MainForm->animation->interface_controller->chat->AddGlobalMessage(ChatIcon_None,
                                            CHAT_COLOR_DEFAULT,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            global_shown, true, false);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                            }
                            if (MainForm->edits->GetText(EditId_Chat)[1] == '@')
                            {
                                if (MainForm->net_client->GetAdminLevel() > 1)
                                {
                                    String announce_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        announce_text = MainForm->net_client->MakeDrunkText(announce_text);
                                    String announce_shown =
                                        MainForm->net_client->CensorText(announce_text.SubString(2,
                                            announce_text.Length() - 1));
                                    if (announce_shown.Length() >= 1)
                                    {
                                        MainForm->sfx->PlaySfx(SfxSound_AnnounceReceived);
                                        MainForm->net_client->sender->SendAnnouncement(announce_text.SubString(2,
                                            announce_text.Length() - 1));
                                        MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Announce,
                                            CHAT_COLOR_NOTICE,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            announce_shown, true);
                                        MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Announce,
                                            CHAT_COLOR_NOTICE,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            announce_shown, true);
                                        MainForm->animation->interface_controller->chat->AddGlobalMessage(ChatIcon_Announce,
                                            CHAT_COLOR_NOTICE,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            announce_shown, true, false);
                                        MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(),
                                            announce_shown, -1);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                                else
                                {
                                    String at_say_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        at_say_text = MainForm->net_client->MakeDrunkText(at_say_text);
                                    String at_say_shown = MainForm->net_client->CensorText(at_say_text);
                                    if (at_say_shown.Length() >= 1)
                                    {
                                        MainForm->net_client->sender->SendPublicChat(at_say_text);
                                        MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_None,
                                            CHAT_COLOR_DEFAULT,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            at_say_shown, true);
                                        MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(),
                                            at_say_shown, -1);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                            }
                            if (MainForm->edits->GetText(EditId_Chat)[1] == '&')
                            {
                                if (MainForm->animation->interface_controller->status->IsInGuild())
                                {
                                    String guild_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        guild_text = MainForm->net_client->MakeDrunkText(guild_text);
                                    String guild_shown =
                                        MainForm->net_client->CensorText(guild_text.SubString(2,
                                            guild_text.Length() - 1));
                                    if (guild_shown.Length() >= 1)
                                    {
                                        MainForm->sfx->PlaySfx(SfxSound_ChannelMessage);
                                        MainForm->net_client->sender->SendGuildChat(guild_text.SubString(2,
                                            guild_text.Length() - 1));
                                        MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Guild,
                                            CHAT_COLOR_DEFAULT,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            guild_shown, true);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                                else
                                {
                                    String amp_say_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        amp_say_text = MainForm->net_client->MakeDrunkText(amp_say_text);
                                    String amp_say_shown = MainForm->net_client->CensorText(amp_say_text);
                                    if (amp_say_shown.Length() >= 1)
                                    {
                                        MainForm->net_client->sender->SendPublicChat(amp_say_text);
                                        MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_None,
                                            CHAT_COLOR_DEFAULT,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            amp_say_shown, true);
                                        MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(),
                                            amp_say_shown, -1);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                            }
                            if (String(MainForm->edits->GetText(EditId_Chat)[1]) == "'")
                            {
                                if (MainForm->animation->interface_controller->group->members.size() > 0)
                                {
                                    String party_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        party_text = MainForm->net_client->MakeDrunkText(party_text);
                                    String party_shown =
                                        MainForm->net_client->CensorText(party_text.SubString(2,
                                            party_text.Length() - 1));
                                    if (party_shown.Length() >= 1)
                                    {
                                        MainForm->net_client->sender->SendPartyChat(party_text.SubString(2,
                                            party_text.Length() - 1));
                                        MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(), party_shown,
                                            MainForm->config->GetGroupColor());
                                        MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Party,
                                            CHAT_COLOR_DEFAULT,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            party_shown, true);
                                        MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Party,
                                            CHAT_COLOR_PRIVATE,
                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                            party_shown, true);
                                    }
                                    else
                                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                                            MainForm->strings->GetText(GameText_YourMindPreventsYou));
                                }
                            }
                            // In a private tab the text goes to that tab's partner.
                            // Elsewhere the name comes first, then a space.
                            if (MainForm->edits->GetText(EditId_Chat)[1] == '!')
                            {
                                if (MainForm->animation->interface_controller->chat->active_tab <= 2)
                                {
                                    String reply_text = MainForm->edits->GetText(EditId_Chat);
                                    if (MainForm->animation->interface_controller->chat->IsDrunk())
                                        reply_text = MainForm->net_client->MakeDrunkText(reply_text);
                                    String reply_name = "";
                                    String reply_body = reply_text.SubString(2, reply_text.Length() - 1);
                                    String reply_shown = MainForm->net_client->CensorText(reply_body);
                                    if (reply_shown.Length() >= 1)
                                    {
                                        if (MainForm->animation->interface_controller->chat->active_tab == 1)
                                            reply_name =
                                                MainForm->animation->interface_controller->chat->first_private_name;
                                        else
                                            reply_name =
                                                MainForm->animation->interface_controller->chat->second_private_name;
                                        if (reply_body.Length() >= 2)
                                        {
                                            if (reply_name != "")
                                            {
                                                MainForm->sfx->PlaySfx(SfxSound_Send);
                                                MainForm->net_client->sender->SendPrivateMessage(LowerCase(reply_name),
                                                    reply_body);
                                                MainForm->animation->interface_controller->chat->OpenPrivateTab(reply_name);
                                                int reply_tab =
                                                    MainForm->animation->interface_controller->chat->FindPrivateTab(reply_name);
                                                if (reply_tab != -1)
                                                    MainForm->animation->interface_controller->chat->AddPrivateMessage(reply_tab,
                                                        CHAT_COLOR_PRIVATE,
                                                        MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                        reply_shown, true);
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
                                        private_text = MainForm->net_client->MakeDrunkText(private_text);
                                    int name_end = private_text.Pos(" ");
                                    if (name_end >= 3)
                                    {
                                        String private_name = private_text.SubString(2, name_end - 2);
                                        String private_body =
                                            private_text.SubString(name_end + 1, private_text.Length() - name_end);
                                        String private_shown = MainForm->net_client->CensorText(private_body);
                                        if (private_shown.Length() >= 1)
                                        {
                                            if (private_name.Length() >= 4 && private_name.Length() <= 13 &&
                                                private_body.Length() >= 1)
                                            {
                                                if (LowerCase(private_name) !=
                                                    LowerCase(MainForm->players->GetName(MainForm->net_client->GetPlayerId())))
                                                {
                                                    MainForm->sfx->PlaySfx(SfxSound_Send);
                                                    MainForm->net_client->sender->SendPrivateMessage(LowerCase(private_name),
                                                        private_body);
                                                    MainForm->animation->interface_controller->chat->OpenPrivateTab(private_name);
                                                    int private_tab =
                                                        MainForm->animation->interface_controller->chat->FindPrivateTab(private_name);
                                                    if (private_tab != -1)
                                                        MainForm->animation->interface_controller->chat->AddPrivateMessage(private_tab,
                                                            CHAT_COLOR_PRIVATE,
                                                            MainForm->net_client->Capitalize(MainForm->players->GetName(MainForm->net_client->GetPlayerId())),
                                                            private_shown, true);
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
            if (MainForm->edits->IsFocused(EditId_Password) && MainForm->animation->GetScreenMode() ==
                ScreenMode_Menu && MainForm->animation->GetSubState() == MenuScreen_Login &&
                MainForm->animation->menu->login_enabled)
            {
                if (MainForm->edits->GetText(EditId_AccountName) != "" && MainForm->edits->GetText(EditId_Password) !=
                    "")
                {
                    MainForm->animation->menu->login_enabled = false;
                    MainForm->registry->SetServerHost(MainForm->config->GetHost());
                    MainForm->registry->SetServerPort(MainForm->config->GetPort());
                    MainForm->registry->SetAccountName(MainForm->edits->GetText(EditId_AccountName));
                    MainForm->sfx->PlaySfx(SfxSound_ButtonClick);
                    MainForm->net_client->sender->SendLogin(MainForm->edits->GetText(EditId_AccountName),
                        MainForm->edits->GetText(EditId_Password));
                }
            }
            if (MainForm->edits->IsFocused(EditId_AccountName) && MainForm->animation->GetScreenMode() ==
                ScreenMode_Menu && MainForm->animation->GetSubState() == MenuScreen_GridAccountName)
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
                    MainForm->keyboard->grid_enter_wait = 24;
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
    catch (...)
    {
        ReleaseModifierKeys();
        return false;
    }
    return true;
}

// Types the pressed keys into the focused edit box: letters, digits and signs as the keyboard layout and shift give
// them, space, backspace and line breaks, with key repeat.
bool KeyBoard::HandleTypedKeys()
{
    if (key_delay > 0)
        return true;
    // Each key's repeat wait counts down by one.
    for (int key = 0; key < 61; key++)
    {
        if (key_waits[key] > 0)
            key_waits[key]--;
    }
    if (!MainForm->edits->GetCount())
        return true;
    if (!MainForm->edits->caret_enabled)
        return true;
    if (MainForm->animation->GetScreenMode() == ScreenMode_Game)
    {
        if (space_key_used)
            return true;
        if (wasd_key_used)
            return true;
    }
    try
    {
        if (!MainForm->animation->CanSendChat())
            return true;
        // The typed character, from the key state and the keyboard layout setting.
        // Layouts 1 to 3 move symbols to other keys.
        // Layout 3 also swaps A with Q and Z with W, as AZERTY does.
        char shifted = false;
        String typed = "";
        if ((MainForm->DIKeyboardState1->Keys[DIK_LSHIFT] & 0x80) != 0 ||
            (MainForm->DIKeyboardState1->Keys[DIK_RSHIFT] & 0x80) != 0)
            shifted = true;
        if ((MainForm->DIKeyboardState1->Keys[DIK_SPACE] & 0x80) != 0)
        {
            if (TakeKeyRepeat(0))
                typed += ' ';
        }
        else
            ResetKeyRepeat(0);
        if ((MainForm->DIKeyboardState1->Keys[DIK_GRAVE] & 0x80) != 0)
        {
            if (TakeKeyRepeat(1))
                typed += '~';
        }
        else
            ResetKeyRepeat(1);
        // The digit row; layout 3 is an AZERTY keyboard, whose digits need Shift.
        if ((MainForm->DIKeyboardState1->Keys[DIK_1] & 0x80) != 0)
        {
            if (TakeKeyRepeat(2))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '!';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '!';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '!';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '1';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += '&';
                else
                    typed += '1';
            }
        }
        else
            ResetKeyRepeat(2);
        if ((MainForm->DIKeyboardState1->Keys[DIK_2] & 0x80) != 0)
        {
            if (TakeKeyRepeat(3))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '@';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '@';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '2';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += (char)-23;
                else
                    typed += '2';
            }
        }
        else
            ResetKeyRepeat(3);
        if ((MainForm->DIKeyboardState1->Keys[DIK_3] & 0x80) != 0)
        {
            if (TakeKeyRepeat(4))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '#';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "#";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '#';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '3';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += '"';
                else
                    typed += '3';
            }
        }
        else
            ResetKeyRepeat(4);
        if ((MainForm->DIKeyboardState1->Keys[DIK_4] & 0x80) != 0)
        {
            if (TakeKeyRepeat(5))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '$';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '$';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '$';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '4';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += "'";
                else
                    typed += '4';
            }
        }
        else
            ResetKeyRepeat(5);
        if ((MainForm->DIKeyboardState1->Keys[DIK_5] & 0x80) != 0)
        {
            if (TakeKeyRepeat(6))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '%';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '%';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '%';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '5';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += '(';
                else
                    typed += '5';
            }
        }
        else
            ResetKeyRepeat(6);
        if ((MainForm->DIKeyboardState1->Keys[DIK_6] & 0x80) != 0)
        {
            if (TakeKeyRepeat(7))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '^';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "&";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '&';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '6';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += (char)-89;
                else
                    typed += '6';
            }
        }
        else
            ResetKeyRepeat(7);
        if ((MainForm->DIKeyboardState1->Keys[DIK_7] & 0x80) != 0)
        {
            if (TakeKeyRepeat(8))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '&';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "_";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '/';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '7';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += (char)-24;
                else
                    typed += '7';
            }
        }
        else
            ResetKeyRepeat(8);
        if ((MainForm->DIKeyboardState1->Keys[DIK_8] & 0x80) != 0)
        {
            if (TakeKeyRepeat(9))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '*';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "(";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '(';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '8';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += '!';
                else
                    typed += '8';
            }
        }
        else
            ResetKeyRepeat(9);
        if ((MainForm->DIKeyboardState1->Keys[DIK_9] & 0x80) != 0)
        {
            if (TakeKeyRepeat(10))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '(';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += ")";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += ')';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '9';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += (char)-25;
                else
                    typed += '9';
            }
        }
        else
            ResetKeyRepeat(10);
        if ((MainForm->DIKeyboardState1->Keys[DIK_0] & 0x80) != 0)
        {
            if (TakeKeyRepeat(11))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += ')';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '=';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '0';
                }
                else if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                    typed += (char)-32;
                else
                    typed += '0';
            }
        }
        else
            ResetKeyRepeat(11);
        if ((MainForm->DIKeyboardState1->Keys[DIK_MINUS] & 0x80) != 0)
        {
            if (TakeKeyRepeat(12))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '_';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "?";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '?';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += ')';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '-';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "/";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '+';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += ')';
                }
            }
        }
        else
            ResetKeyRepeat(12);
        if ((MainForm->DIKeyboardState1->Keys[DIK_EQUALS] & 0x80) != 0)
        {
            if (TakeKeyRepeat(13))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '+';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '~';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '-';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '=';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '-';
                }
            }
        }
        else
            ResetKeyRepeat(13);
        if ((MainForm->DIKeyboardState1->Keys[DIK_LBRACKET] & 0x80) != 0)
        {
            if (TakeKeyRepeat(14))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '[';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '^';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += (char)-59;
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '^';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '[';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '*';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += (char)-27;
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '[';
                }
            }
        }
        else
            ResetKeyRepeat(14);
        if ((MainForm->DIKeyboardState1->Keys[DIK_RBRACKET] & 0x80) != 0)
        {
            if (TakeKeyRepeat(15))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += ']';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '|';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '^';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '*';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += ']';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '*';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '\\';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += ']';
                }
            }
        }
        else
            ResetKeyRepeat(15);
        if ((MainForm->DIKeyboardState1->Keys[DIK_SEMICOLON] & 0x80) != 0)
        {
            if (TakeKeyRepeat(16))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += ':';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '+';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += (char)-42;
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += 'M';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += ';';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '+';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += (char)-10;
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '-';
                }
            }
        }
        else
            ResetKeyRepeat(16);
        if ((MainForm->DIKeyboardState1->Keys[DIK_APOSTROPHE] & 0x80) != 0)
        {
            if (TakeKeyRepeat(17))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '"';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '"';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += "\xc4";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '%';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += (char)-28;
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += "'";
                }
            }
        }
        else
            ResetKeyRepeat(17);
        if ((MainForm->DIKeyboardState1->Keys[DIK_BACKSLASH] & 0x80) != 0)
        {
            if (TakeKeyRepeat(18))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '|';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '>';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '*';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += (char)-93;
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '\\';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '<';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += "'";
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '?';
                }
            }
        }
        else
            ResetKeyRepeat(18);
        if ((MainForm->DIKeyboardState1->Keys[DIK_COMMA] & 0x80) != 0)
        {
            if (TakeKeyRepeat(19))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '<';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += ';';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += ';';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '.';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += ',';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += ',';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += ',';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += ';';
                }
            }
        }
        else
            ResetKeyRepeat(19);
        if ((MainForm->DIKeyboardState1->Keys[DIK_PERIOD] & 0x80) != 0)
        {
            if (TakeKeyRepeat(20))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '>';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += ':';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += ':';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += ':';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '.';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '.';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '.';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '/';
                }
            }
        }
        else
            ResetKeyRepeat(20);
        if ((MainForm->DIKeyboardState1->Keys[DIK_SLASH] & 0x80) != 0)
        {
            if (TakeKeyRepeat(59))
            {
                if (shifted)
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '?';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '=';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '_';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '+';
                }
                else
                {
                    if (!MainForm->config->GetKeyboardLayout())
                        typed += '/';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Dutch)
                        typed += '.';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_Swedish)
                        typed += '-';
                    if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                        typed += '=';
                }
            }
        }
        else
            ResetKeyRepeat(59);
        // The letter keys, with Shift for capitals; layout 3 places them as an AZERTY keyboard does.
        if ((MainForm->DIKeyboardState1->Keys[DIK_A] & 0x80) != 0)
        {
            if (TakeKeyRepeat(21))
            {
                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                {
                    if (shifted)
                        typed += "Q";
                    else
                        typed += "q";
                }
                else if (shifted)
                    typed += "A";
                else
                    typed += "a";
            }
        }
        else
            ResetKeyRepeat(21);
        if ((MainForm->DIKeyboardState1->Keys[DIK_B] & 0x80) != 0)
        {
            if (TakeKeyRepeat(22))
            {
                if (shifted)
                    typed += "B";
                else
                    typed += "b";
            }
        }
        else
            ResetKeyRepeat(22);
        if ((MainForm->DIKeyboardState1->Keys[DIK_C] & 0x80) != 0)
        {
            if (TakeKeyRepeat(23))
            {
                if (shifted)
                    typed += "C";
                else
                    typed += "c";
            }
        }
        else
            ResetKeyRepeat(23);
        if ((MainForm->DIKeyboardState1->Keys[DIK_D] & 0x80) != 0)
        {
            if (TakeKeyRepeat(24))
            {
                if (shifted)
                    typed += "D";
                else
                    typed += "d";
            }
        }
        else
            ResetKeyRepeat(24);
        if ((MainForm->DIKeyboardState1->Keys[DIK_E] & 0x80) != 0)
        {
            if (TakeKeyRepeat(25))
            {
                if (shifted)
                    typed += "E";
                else
                    typed += "e";
            }
        }
        else
            ResetKeyRepeat(25);
        if ((MainForm->DIKeyboardState1->Keys[DIK_F] & 0x80) != 0)
        {
            if (TakeKeyRepeat(26))
            {
                if (shifted)
                    typed += "F";
                else
                    typed += "f";
            }
        }
        else
            ResetKeyRepeat(26);
        if ((MainForm->DIKeyboardState1->Keys[DIK_G] & 0x80) != 0)
        {
            if (TakeKeyRepeat(27))
            {
                if (shifted)
                    typed += "G";
                else
                    typed += "g";
            }
        }
        else
            ResetKeyRepeat(27);
        if ((MainForm->DIKeyboardState1->Keys[DIK_H] & 0x80) != 0)
        {
            if (TakeKeyRepeat(28))
            {
                if (shifted)
                    typed += "H";
                else
                    typed += "h";
            }
        }
        else
            ResetKeyRepeat(28);
        if ((MainForm->DIKeyboardState1->Keys[DIK_I] & 0x80) != 0)
        {
            if (TakeKeyRepeat(29))
            {
                if (shifted)
                    typed += "I";
                else
                    typed += "i";
            }
        }
        else
            ResetKeyRepeat(29);
        if ((MainForm->DIKeyboardState1->Keys[DIK_J] & 0x80) != 0)
        {
            if (TakeKeyRepeat(30))
            {
                if (shifted)
                    typed += "J";
                else
                    typed += "j";
            }
        }
        else
            ResetKeyRepeat(30);
        if ((MainForm->DIKeyboardState1->Keys[DIK_K] & 0x80) != 0)
        {
            if (TakeKeyRepeat(31))
            {
                if (shifted)
                    typed += "K";
                else
                    typed += "k";
            }
        }
        else
            ResetKeyRepeat(31);
        if ((MainForm->DIKeyboardState1->Keys[DIK_L] & 0x80) != 0)
        {
            if (TakeKeyRepeat(32))
            {
                if (shifted)
                    typed += "L";
                else
                    typed += "l";
            }
        }
        else
            ResetKeyRepeat(32);
        if ((MainForm->DIKeyboardState1->Keys[DIK_M] & 0x80) != 0)
        {
            if (TakeKeyRepeat(33))
            {
                if (shifted)
                    typed += "M";
                else
                    typed += "m";
            }
        }
        else
            ResetKeyRepeat(33);
        if ((MainForm->DIKeyboardState1->Keys[DIK_N] & 0x80) != 0)
        {
            if (TakeKeyRepeat(35))
            {
                if (shifted)
                    typed += "N";
                else
                    typed += "n";
            }
        }
        else
            ResetKeyRepeat(35);
        if ((MainForm->DIKeyboardState1->Keys[DIK_O] & 0x80) != 0)
        {
            if (TakeKeyRepeat(36))
            {
                if (shifted)
                    typed += "O";
                else
                    typed += "o";
            }
        }
        else
            ResetKeyRepeat(36);
        if ((MainForm->DIKeyboardState1->Keys[DIK_P] & 0x80) != 0)
        {
            if (TakeKeyRepeat(37))
            {
                if (shifted)
                    typed += "P";
                else
                    typed += "p";
            }
        }
        else
            ResetKeyRepeat(37);
        if ((MainForm->DIKeyboardState1->Keys[DIK_Q] & 0x80) != 0)
        {
            if (TakeKeyRepeat(38))
            {
                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                {
                    if (shifted)
                        typed += "A";
                    else
                        typed += "a";
                }
                else if (shifted)
                    typed += "Q";
                else
                    typed += "q";
            }
        }
        else
            ResetKeyRepeat(38);
        if ((MainForm->DIKeyboardState1->Keys[DIK_R] & 0x80) != 0)
        {
            if (TakeKeyRepeat(39))
            {
                if (shifted)
                    typed += "R";
                else
                    typed += "r";
            }
        }
        else
            ResetKeyRepeat(39);
        if ((MainForm->DIKeyboardState1->Keys[DIK_S] & 0x80) != 0)
        {
            if (TakeKeyRepeat(40))
            {
                if (shifted)
                    typed += "S";
                else
                    typed += "s";
            }
        }
        else
            ResetKeyRepeat(40);
        if ((MainForm->DIKeyboardState1->Keys[DIK_T] & 0x80) != 0)
        {
            if (TakeKeyRepeat(41))
            {
                if (shifted)
                    typed += "T";
                else
                    typed += "t";
            }
        }
        else
            ResetKeyRepeat(41);
        if ((MainForm->DIKeyboardState1->Keys[DIK_U] & 0x80) != 0)
        {
            if (TakeKeyRepeat(42))
            {
                if (shifted)
                    typed += "U";
                else
                    typed += "u";
            }
        }
        else
            ResetKeyRepeat(42);
        if ((MainForm->DIKeyboardState1->Keys[DIK_V] & 0x80) != 0)
        {
            if (TakeKeyRepeat(43))
            {
                if (shifted)
                    typed += "V";
                else
                    typed += "v";
            }
        }
        else
            ResetKeyRepeat(43);
        if ((MainForm->DIKeyboardState1->Keys[DIK_W] & 0x80) != 0)
        {
            if (TakeKeyRepeat(44))
            {
                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                {
                    if (shifted)
                        typed += "Z";
                    else
                        typed += "z";
                }
                else if (shifted)
                    typed += "W";
                else
                    typed += "w";
            }
        }
        else
            ResetKeyRepeat(44);
        if ((MainForm->DIKeyboardState1->Keys[DIK_X] & 0x80) != 0)
        {
            if (TakeKeyRepeat(45))
            {
                if (shifted)
                    typed += "X";
                else
                    typed += "x";
            }
        }
        else
            ResetKeyRepeat(45);
        if ((MainForm->DIKeyboardState1->Keys[DIK_Y] & 0x80) != 0)
        {
            if (TakeKeyRepeat(46))
            {
                if (shifted)
                    typed += "Y";
                else
                    typed += "y";
            }
        }
        else
            ResetKeyRepeat(46);
        if ((MainForm->DIKeyboardState1->Keys[DIK_Z] & 0x80) != 0)
        {
            if (TakeKeyRepeat(47))
            {
                if (MainForm->config->GetKeyboardLayout() == KeyboardLayout_French)
                {
                    if (shifted)
                        typed += "W";
                    else
                        typed += "w";
                }
                else if (shifted)
                    typed += "Z";
                else
                    typed += "z";
            }
        }
        else
            ResetKeyRepeat(47);
        // On the menu screens the number pad types its digits and point.
        if (MainForm->animation->GetScreenMode() == ScreenMode_Menu)
        {
            if ((MainForm->DIKeyboardState1->Keys[DIK_DECIMAL] & 0x80) != 0)
            {
                if (TakeKeyRepeat(48))
                    typed += '.';
            }
            else
                ResetKeyRepeat(48);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD0] & 0x80) != 0)
            {
                if (TakeKeyRepeat(49))
                    typed += '0';
            }
            else
                ResetKeyRepeat(49);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD1] & 0x80) != 0)
            {
                if (TakeKeyRepeat(50))
                    typed += '1';
            }
            else
                ResetKeyRepeat(50);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD2] & 0x80) != 0)
            {
                if (TakeKeyRepeat(51))
                    typed += '2';
            }
            else
                ResetKeyRepeat(51);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD3] & 0x80) != 0)
            {
                if (TakeKeyRepeat(52))
                    typed += '3';
            }
            else
                ResetKeyRepeat(52);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD4] & 0x80) != 0)
            {
                if (TakeKeyRepeat(53))
                    typed += '4';
            }
            else
                ResetKeyRepeat(53);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD5] & 0x80) != 0)
            {
                if (TakeKeyRepeat(54))
                    typed += '5';
            }
            else
                ResetKeyRepeat(54);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD6] & 0x80) != 0)
            {
                if (TakeKeyRepeat(55))
                    typed += '6';
            }
            else
                ResetKeyRepeat(55);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD7] & 0x80) != 0)
            {
                if (TakeKeyRepeat(56))
                    typed += '7';
            }
            else
                ResetKeyRepeat(56);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD8] & 0x80) != 0)
            {
                if (TakeKeyRepeat(57))
                    typed += '8';
            }
            else
                ResetKeyRepeat(57);
            if ((MainForm->DIKeyboardState1->Keys[DIK_NUMPAD9] & 0x80) != 0)
            {
                if (TakeKeyRepeat(58))
                    typed += '9';
            }
            else
                ResetKeyRepeat(58);
        }
        if ((MainForm->DIKeyboardState1->Keys[DIK_RETURN] & 0x80) != 0)
        {
            if (TakeKeyRepeat(60))
                MainForm->edits->TypeLineBreak();
            else
                ResetKeyRepeat(60);
        }
        // Backspace deletes once, then again after a long wait, and quicker from there while held.
        if (backspace_wait >= 1)
            backspace_wait--;
        if ((MainForm->DIKeyboardState1->Keys[DIK_BACK] & 0x80) == 0)
            backspace_wait = 0;
        if ((MainForm->DIKeyboardState1->Keys[DIK_BACK] & 0x80) != 0 && !typed.Length() && backspace_wait <= 2)
        {
            if (backspace_wait >= 1)
                backspace_wait = 16;
            else
                backspace_wait = 64;
            MainForm->edits->Backspace();
        }
        if (typed.Length() > 0)
            MainForm->edits->TypeText(typed);
    }
    catch (...)
    {
        ReleaseModifierKeys();
        return false;
    }
    return true;
}

// True when a key may act now; the next must wait 48 ticks.
bool KeyBoard::TakeKeyDelay()
{
    if (key_delay > 0)
        return false;
    key_delay = 48;
    return true;
}

// True when the held key may act again; the first press waits 80 ticks, each repeat 16.
bool KeyBoard::TakeKeyRepeat(unsigned char key)
{
    int wait = key_waits[key];
    if (key_waits[key] > 6)
        return false;
    if (!key_waits[key])
        key_waits[key] = 80;
    else
        key_waits[key] = 16;
    return true;
}

void KeyBoard::ResetKeyRepeat(unsigned char key)
{
    key_waits[key] = 0;
}

// True when any box or window is open over the game.
bool KeyBoard::IsAnyWindowOpen()
{
    if (MainForm->animation->game->message_visible)
        return true;
    if (MainForm->animation->game->large_box_visible)
        return true;
    if (MainForm->animation->game->input_box_visible)
        return true;
    if (MainForm->animation->game->form_visible)
        return true;
    if (MainForm->animation->visual_doc->is_visible)
        return true;
    if (MainForm->animation->hotkeys->is_visible)
        return true;
    if (MainForm->animation->paperdoll->is_visible)
        return true;
    if (MainForm->animation->book->is_visible)
        return true;
    if (MainForm->animation->trade->is_visible)
        return true;
    if (MainForm->animation->locker->is_visible)
        return true;
    if (MainForm->animation->learn->is_visible)
        return true;
    if (MainForm->animation->crafter->is_visible)
        return true;
    if (MainForm->animation->shop->is_visible)
        return true;
    if (MainForm->animation->stat->is_visible)
        return true;
    if (MainForm->animation->guild->is_visible)
        return true;
    if (MainForm->animation->memo->is_visible)
        return true;
    if (MainForm->animation->dialog->is_visible)
        return true;
    if (MainForm->animation->report->is_visible)
        return true;
    if (MainForm->animation->law->is_visible)
        return true;
    if (MainForm->animation->inn->is_visible)
        return true;
    if (MainForm->animation->bank->is_visible)
        return true;
    if (MainForm->animation->jukebox->is_visible)
        return true;
    if (MainForm->animation->musician->is_visible)
        return true;
    if (MainForm->animation->barber->is_visible)
        return true;
    if (MainForm->animation->chest->is_visible)
        return true;
    if (MainForm->animation->social->is_open)
        return true;
    if (MainForm->animation->message_board->is_visible)
        return true;
    if (MainForm->animation->quests->is_visible)
        return true;
    if (MainForm->animation->game->amount_box_visible)
        return true;
    return false;
}

void KeyBoard::ReleaseModifierKeys()
{
    walk_key_held = false;
    wasd_key_used = false;
    attack_key_used = false;
    space_key_used = false;
}
