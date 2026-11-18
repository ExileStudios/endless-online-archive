#include <vcl.h>
#pragma hdrstop

#include "Ctrl_settings.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"

#pragma package(smart_init)

SettingsController::SettingsController()
{
    hovered_option = SettingOption_None;
    is_dirty = false;
}

SettingsController::~SettingsController()
{
}

DEFINE_TEST_AND_CLEAR_DIRTY(SettingsController)

void SettingsController::MarkDirty()
{
    is_dirty = true;
}

// A click on a switch, by row; the left column is at x 317 to 336 and the right column at x 557 to 576:
// y 355 = sound              | speech balloons
// y 373 = music              | shadows
// y 391 = keyboard layout    | word filter: off, on, strict
// y 409 = language           | chat log
// y 427 = private messages   | requests from players
// When sound or music is marked unavailable, a message box offers to start it.
void SettingsController::MouseDown(int x, int y)
{
    // The left column: sound, music, keyboard layout, language and private messages;
    // the right one: balloons, shadows, the word filter, the chat log and requests.
    if (x >= 317 && x <= 336)
    {
        if (y >= 355 && y <= 370)
        {
            is_dirty = true;
            if (MainForm->config->GetSoundAvailable() > 0)
            {
                if (MainForm->config->GetSoundVolume() >= 100)
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->config->SetSoundVolume(0);
                    MainForm->sfx->SetSoundEnabled(0);
                    MainForm->sfx->FadeOutAmbientSound();
                }
                else if (MainForm->config->GetSoundVolume() <= 0)
                {
                    MainForm->config->SetSoundVolume(50);
                    MainForm->sfx->SetSoundEnabled(50);
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                }
                else
                {
                    MainForm->config->SetSoundVolume(100);
                    MainForm->sfx->SetSoundEnabled(100);
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                }
            }
            else
            {
                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_EnableSound,
                    MainForm->strings->GetDialogText(DialogText_SoundDisabled, false),
                    MainForm->strings->GetDialogText(DialogText_SoundDisabled, true));
                MainForm->edits->caret_enabled = false;
            }
        }
        if (y >= 373 && y <= 388)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            is_dirty = true;
            if (MainForm->config->GetMusicAvailable() > 0)
            {
                if (MainForm->config->GetMusicVolume() >= 100)
                {
                    MainForm->sfx->SetMusicVolume(0);
                    MainForm->config->SetMusicEnabled(0);
                }
                else if (MainForm->config->GetMusicVolume() <= 0)
                {
                    MainForm->sfx->SetMusicVolume(50);
                    MainForm->config->SetMusicEnabled(50);
                }
                else
                {
                    MainForm->sfx->SetMusicVolume(100);
                    MainForm->config->SetMusicEnabled(100);
                }
            }
            else
            {
                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_EnableMusic,
                    MainForm->strings->GetDialogText(DialogText_MusicDisabled, false),
                    MainForm->strings->GetDialogText(DialogText_MusicDisabled, true));
                MainForm->edits->caret_enabled = false;
            }
        }
        if (y >= 391 && y <= 406)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            int keyboard_layout = MainForm->config->GetKeyboardLayout();
            is_dirty = true;
            if (keyboard_layout <= KeyboardLayout_Swedish)
                MainForm->config->SetKeyboardLayout(keyboard_layout + 1);
            else
                MainForm->config->SetKeyboardLayout(0);
        }
        if (y >= 409 && y <= 424)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            int language = MainForm->config->GetLanguage();
            is_dirty = true;
            if (language <= 2)
            {
                MainForm->config->SetLanguage(language + 1);
                MainForm->strings->SetLanguage(language + 1);
            }
            else
            {
                MainForm->config->SetLanguage(0);
                MainForm->strings->SetLanguage(0);
            }
        }
        if (y >= 427 && y <= 443)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            MainForm->redraw_needed = true;
            is_dirty = true;
            if (MainForm->config->GetAcceptPrivateMessages())
            {
                MainForm->net_client->sender->SendPrivateMessagesOff();
                MainForm->config->SetAcceptPrivateMessages(false);
            }
            else
            {
                MainForm->net_client->sender->SendPrivateMessagesOn();
                MainForm->config->SetAcceptPrivateMessages(true);
            }
        }
    }
    if (x >= 557 && x <= 576)
    {
        if (y >= 355 && y <= 370)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            MainForm->redraw_needed = true;
            is_dirty = true;
            if (MainForm->config->GetShowBalloons())
                MainForm->config->SetShowBalloons(false);
            else
                MainForm->config->SetShowBalloons(true);
        }
        if (y >= 373 && y <= 388)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            MainForm->redraw_needed = true;
            is_dirty = true;
            if (MainForm->config->GetShowShadows())
                MainForm->config->SetShowShadows(false);
            else
                MainForm->config->SetShowShadows(true);
        }
        if (y >= 391 && y <= 406)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            is_dirty = true;
            if (!MainForm->config->GetFilterEnabled() && !MainForm->config->GetStrictFilter())
            {
                MainForm->config->SetFilterEnabled(true);
                MainForm->config->SetStrictFilter(false);
            }
            else if (MainForm->config->GetFilterEnabled() && !MainForm->config->GetStrictFilter())
            {
                MainForm->config->SetFilterEnabled(true);
                MainForm->config->SetStrictFilter(true);
            }
            else if (MainForm->config->GetFilterEnabled() && MainForm->config->GetStrictFilter())
            {
                MainForm->config->SetFilterEnabled(false);
                MainForm->config->SetStrictFilter(false);
            }
        }
        if (y >= 409 && y <= 424)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            is_dirty = true;
            if (MainForm->config->GetLogChat())
            {
                MainForm->config->SetLogChat(false);
                MainForm->chat_log->Close();
            }
            else
            {
                MainForm->config->SetLogChat(true);
                MainForm->chat_log->Open(MainForm->config->GetChatLogPath());
            }
        }
        if (y >= 427 && y <= 443)
        {
            MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
            MainForm->redraw_needed = true;
            is_dirty = true;
            if (MainForm->config->GetAcceptRequests())
                MainForm->config->SetAcceptRequests(false);
            else
                MainForm->config->SetAcceptRequests(true);
        }
    }
    if (x >= 558 && x <= 588 && y >= 331 && y <= 348)
        MainForm->mouse->OpenHelpPanel(true);
}

// Marks the setting under the mouse in either column, or the top button with its tip on the status line.
void SettingsController::MouseMove(int x, int y)
{
    int old_option = hovered_option;
    hovered_option = SettingOption_None;
    if (x >= 317 && x <= 336)
    {
        if (y >= 355 && y <= 370)
            hovered_option = SettingOption_Sound;
        if (y >= 373 && y <= 388)
            hovered_option = SettingOption_Music;
        if (y >= 391 && y <= 406)
            hovered_option = SettingOption_Keyboard;
        if (y >= 409 && y <= 424)
            hovered_option = SettingOption_Language;
        if (y >= 427 && y <= 443)
            hovered_option = SettingOption_PrivateMessages;
    }
    if (x >= 557 && x <= 576)
    {
        if (y >= 355 && y <= 370)
            hovered_option = SettingOption_Balloons;
        if (y >= 373 && y <= 388)
            hovered_option = SettingOption_Shadows;
        if (y >= 391 && y <= 406)
            hovered_option = SettingOption_Filter;
        if (y >= 409 && y <= 424)
            hovered_option = SettingOption_LogChat;
        if (y >= 427 && y <= 443)
            hovered_option = SettingOption_Requests;
    }
    if (x >= 558 && x <= 588 && y >= 331 && y <= 348)
    {
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
            MainForm->strings->GetText(GameText_GameHelp));
        hovered_option = SettingOption_GameHelp;
    }
    if (hovered_option != old_option)
        is_dirty = true;
}
