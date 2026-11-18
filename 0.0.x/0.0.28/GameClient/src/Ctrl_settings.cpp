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
    int language;
    if (x >= 317 && x <= 336)
    {
        if (y >= 355 && y <= 370)
        {
            is_dirty = true;
            if (MainForm->config->GetSoundAvailable())
            {
                if (MainForm->config->GetSoundEnabled())
                {
                    MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                    MainForm->config->SetSoundEnabled(0);
                    MainForm->sfx->SetSoundEnabled(0);
                    MainForm->sfx->FadeOutAmbientSound();
                }
                else
                {
                    MainForm->config->SetSoundEnabled(1);
                    MainForm->sfx->SetSoundEnabled(1);
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
            if (MainForm->config->GetMusicAvailable())
            {
                if (MainForm->config->GetMusicEnabled())
                {
                    MainForm->sfx->StopMusic();
                    MainForm->config->SetMusicEnabled(0);
                }
                else
                    MainForm->config->SetMusicEnabled(1);
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
            language = MainForm->config->GetLanguage();
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
            else
            {
                if (MainForm->config->GetFilterEnabled() && !MainForm->config->GetStrictFilter())
                {
                    MainForm->config->SetFilterEnabled(true);
                    MainForm->config->SetStrictFilter(true);
                }
                else
                {
                    if (MainForm->config->GetFilterEnabled() && MainForm->config->GetStrictFilter())
                    {
                        MainForm->config->SetFilterEnabled(false);
                        MainForm->config->SetStrictFilter(false);
                    }
                }
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
}

// Shows help text 54 while the mouse is over a switch.
void SettingsController::MouseMove(int x, int y)
{
    char result = SettingOption_None;
    if (x >= 317 && x <= 336)
    {
        if (y >= 355 && y <= 370)
        {
            if (hovered_option != SettingOption_Sound)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Sound;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Sound)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 373 && y <= 388)
        {
            if (hovered_option != SettingOption_Music)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Music;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Music)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 391 && y <= 406)
        {
            if (hovered_option != SettingOption_Keyboard)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Keyboard;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Keyboard)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 409 && y <= 424)
        {
            if (hovered_option != SettingOption_Language)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Language;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Language)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 427 && y <= 443)
        {
            if (hovered_option != SettingOption_PrivateMessages)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_PrivateMessages;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_PrivateMessages)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
    }
    else if (hovered_option > SettingOption_None && hovered_option < SettingOption_Balloons)
    {
        hovered_option = SettingOption_None;
        result = 1;
    }
    if (x >= 557 && x <= 576)
    {
        if (y >= 355 && y <= 370)
        {
            if (hovered_option != SettingOption_Balloons)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Balloons;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Balloons)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 373 && y <= 388)
        {
            if (hovered_option != SettingOption_Shadows)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Shadows;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Shadows)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 391 && y <= 406)
        {
            if (hovered_option != SettingOption_Filter)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Filter;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Filter)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 409 && y <= 424)
        {
            if (hovered_option != SettingOption_LogChat)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_LogChat;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_LogChat)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
        if (y >= 427 && y <= 443)
        {
            if (hovered_option != SettingOption_Requests)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                    MainForm->strings->GetText(GameText_ClickOnThisButton));
                hovered_option = SettingOption_Requests;
                result = 1;
            }
        }
        else if (hovered_option == SettingOption_Requests)
        {
            hovered_option = SettingOption_None;
            result = 1;
        }
    }
    else if (hovered_option > SettingOption_PrivateMessages && hovered_option < SettingOption_GameHelp)
    {
        hovered_option = SettingOption_None;
        result = 1;
    }
    if (result)
        is_dirty = true;
}
