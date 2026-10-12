#include <vcl.h>
#pragma hdrstop

#include "Ctrl_interface.h"
#include "Tmainform.h"
#include "Ctrl_chat.h"
#include "Ctrl_gamebars.h"
#include "Ctrl_help.h"
#include "Ctrl_news.h"
#include "Ctrl_playerlist.h"
#include "Ctrl_settings.h"
#include "Ctrl_status.h"
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Ctrl_book.h"
#include "Ctrl_dialog.h"
#include "Ctrl_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Gfx_interface.h"
#include "Ctrl_jukebox.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_menu.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_report.h"
#include "Ctrl_social.h"
#pragma package(smart_init)

#define STATUS_MESSAGE_EXPIRED 2

// Creates the controller of each panel; the warning for a big gold drop starts armed.
InterfaceController::InterfaceController()
{
    inventory_button_hover = false;
    minimap_button_hover = false;
    spells_button_hover = false;
    skills_button_hover = false;
    chat_button_hover = false;
    status_button_hover = false;
    players_button_hover = false;
    group_button_hover = false;
    third_right_button_hover = false;
    settings_button_hover = false;
    help_button_hover = false;
    friends_button_hover = false;
    ignore_button_hover = false;
    is_invalidated = false;
    corner_button_hover = false;
    unread_flag_at_13 = false;
    unread_flag_at_14 = false;
    status_message = "";
    status_message_state = 0;
    status_message_timer = 0;
    full_redraw_pending = false;
    button_hover_dirty = false;
    corner_hover_dirty = false;
    screenshot_cooldown = 0;
    player_list_cooldown = 0;
    trade_requester_id = 0;
    player_list_button_ready = true;
    timed_text_visible = false;
    chat = new ChatController();
    status = new StatusController();
    player_list = new PlayerlistController();
    group = new GroupController();
    settings = new SettingsController();
    news = new NewslistController();
    help = new HelpController();
    inventory = new InventoryController();
    spells = new SpellController();
    skills = new SkillController();
    game_bars = new GameBars();
    is_shaking = false;
    shake_offset = 0;
    gold_drop_warning_pending = true;
    welcome_notice_pending = false;
}

// Back to the main menu: closes the connection, frees the game graphics, empties the lists and puts every window back.
void InterfaceController::Logout()
{
    MainForm->gfx->interface_graphics->minimap_visible = false;
    MainForm->map->path_finder->CancelPath();
    MainForm->sfx->StopMusic();
    MainForm->sfx->FadeOutAmbientSound();
    MainForm->gfx->CreateMenuGraphics();
    MainForm->gfx->FreeHoverLabel();
    MainForm->gfx->FreeGameGraphics();
    MainForm->gfx->FreeTradeGraphics();
    MainForm->gfx->FreeShopGraphics();
    MainForm->gfx->FreeBankGraphics();
    MainForm->gfx->FreeBarberGraphics();
    MainForm->gfx->FreeChestGraphics();
    MainForm->gfx->FreeSocialGraphics();
    MainForm->gfx->LoadMenuGraphics(MainForm->config->GetSkipSoftpad());
    MainForm->gfx->LoadMainMenuScreen(true);
    if (MainForm->client->Active)
        MainForm->client->Close();
    MainForm->animation->menu->is_loading_world = false;
    MainForm->edits->Clear();
    MainForm->players->Clear();
    MainForm->effects->Clear();
    MainForm->map_items->ClearAll();
    MainForm->npcs->Clear();
    MainForm->animation->interface_controller->chat->ResetChat();
    MainForm->animation->interface_controller->group->Clear();
    MainForm->animation->interface_controller->inventory->Clear();
    MainForm->animation->interface_controller->skills->Reset();
    MainForm->animation->interface_controller->spells->Reset();
    MainForm->gfx->objects->ReleaseGraphics();
    MainForm->gfx->masks->ReleaseGraphics();
    MainForm->gfx->walls->ReleaseGraphics();
    MainForm->gfx->tiles->ReleaseGraphics();
    MainForm->gfx->tops->ReleaseGraphics();
    MainForm->gfx->blends->ReleaseGraphics();
    MainForm->gfx->items->ReleaseGraphics();
    MainForm->gfx->items->ReleaseSubItemGraphics();
    MainForm->players->ReloadGraphicsLibraries();
    MainForm->npcs->ReloadGraphicsLibrary();
    MainForm->gfx->tiles->ReopenLibrary();
    MainForm->gfx->objects->ReopenLibrary();
    MainForm->gfx->masks->ReopenLibrary();
    MainForm->gfx->walls->ReopenLibrary();
    MainForm->gfx->tops->ReopenLibrary();
    MainForm->gfx->blends->ReopenLibrary();
    MainForm->gfx->items->ReopenLibrary();
    MainForm->gfx->icons->ReopenLibrary();
    MainForm->animation->paperdoll->equipped_head = -1;
    MainForm->animation->paperdoll->equipped_necklace = -1;
    MainForm->animation->paperdoll->equipped_armor = -1;
    MainForm->animation->paperdoll->equipped_weapon = -1;
    MainForm->animation->paperdoll->equipped_shield = -1;
    MainForm->animation->paperdoll->equipped_belt = -1;
    MainForm->animation->paperdoll->equipped_gloves = -1;
    MainForm->animation->paperdoll->equipped_boots = -1;
    MainForm->animation->paperdoll->equipped_type19_left = -1;
    MainForm->animation->paperdoll->equipped_type19_right = -1;
    MainForm->animation->paperdoll->equipped_type20_left = -1;
    MainForm->animation->paperdoll->equipped_type20_right = -1;
    MainForm->animation->paperdoll->equipped_type21_left = -1;
    MainForm->animation->paperdoll->equipped_type21_right = -1;
    MainForm->animation->paperdoll->equipped_type16 = -1;
    MainForm->animation->interface_controller->status->skip_next_exp_gain = true;
    MainForm->animation->interface_controller->status->total_exp_gained = 0;
    MainForm->animation->interface_controller->status->last_exp_gain = 0;
    MainForm->animation->interface_controller->status->best_exp_gain = 0;
    MainForm->animation->menu->is_message_open = false;
    MainForm->animation->menu->is_form_open = false;
    MainForm->animation->game->message_visible = false;
    MainForm->animation->game->large_box_visible = false;
    MainForm->animation->game->input_box_visible = false;
    MainForm->animation->game->form_visible = false;
    MainForm->animation->game->amount_box_visible = false;
    MainForm->animation->bank->is_visible = 0;
    MainForm->animation->jukebox->is_visible = 0;
    MainForm->animation->barber->is_visible = 0;
    MainForm->animation->shop->is_visible = 0;
    MainForm->animation->guild->is_visible = 0;
    MainForm->animation->memo->is_visible = 0;
    MainForm->animation->report->is_visible = 0;
    MainForm->animation->law->is_visible = 0;
    MainForm->animation->inn->is_visible = 0;
    MainForm->animation->learn->is_visible = 0;
    MainForm->animation->trade->is_visible = 0;
    MainForm->animation->chest->is_visible = 0;
    MainForm->animation->paperdoll->is_visible = 0;
    MainForm->animation->book->is_visible = 0;
    MainForm->animation->social->is_open = 0;
    MainForm->animation->message_board->is_visible = 0;
    MainForm->animation->dialog->is_visible = 0;
    MainForm->animation->paperdoll->window_x = 133;
    MainForm->animation->paperdoll->window_y = 13;
    MainForm->animation->menu->message_x = DEFAULT_MESSAGE_BOX_X;
    MainForm->animation->menu->message_y = DEFAULT_MESSAGE_BOX_Y;
    MainForm->animation->menu->form_x = DEFAULT_FORM_X;
    MainForm->animation->menu->form_y = DEFAULT_FORM_Y;
    MainForm->animation->game->message_x = 170;
    MainForm->animation->game->message_y = 100;
    MainForm->animation->game->large_box_x = 170;
    MainForm->animation->game->large_box_y = 68;
    MainForm->animation->game->input_box_x = 175;
    MainForm->animation->game->input_box_y = 95;
    MainForm->animation->game->amount_box_x = 190;
    MainForm->animation->game->amount_box_y = 70;
    MainForm->animation->shop->window_x = 49;
    MainForm->animation->shop->window_y = 13;
    MainForm->animation->chest->window_x = 35;
    MainForm->animation->chest->window_y = 23;
    MainForm->animation->trade->window_x = 49;
    MainForm->animation->trade->window_y = 13;
    MainForm->animation->barber->window_x = 165;
    MainForm->animation->barber->window_y = 53;
    MainForm->animation->social->window_x = 333;
    MainForm->animation->social->window_y = 13;
    MainForm->animation->message_board->window_x = 79;
    MainForm->animation->message_board->window_y = 16;
    MainForm->animation->dialog->window_x = 49;
    MainForm->animation->dialog->window_y = 115;
    MainForm->animation->game->SetGhostWalk(0);
    MainForm->animation->SetScreenMode(ScreenMode_Menu);
    MainForm->animation->SetSubState(MenuScreen_Main);
    MainForm->edits->caret_enabled = true;
    MainForm->animation->social->SaveIgnoreList();
    MainForm->animation->social->SaveFriendList();
}

InterfaceController::~InterfaceController()
{
}

void InterfaceController::RequestFullRedraw()
{
    full_redraw_pending = true;
    full_redraw_countdown = 3;
}

void InterfaceController::Invalidate()
{
    is_invalidated = true;
}

void InterfaceController::OnMouseMove(int mouse_x, int mouse_y)
{
    UpdateButtonHover(mouse_x, mouse_y);
    UpdateCornerHover(mouse_x, mouse_y);
}

// Lights the slanted corner button at the top right and puts its tip on the status line.
bool InterfaceController::UpdateCornerHover(int mouse_x, int mouse_y)
{
    bool result = 0;
    if (mouse_x >= 594 && mouse_x <= 624)
    {
        if ((mouse_y > 10 && mouse_y < 17 && mouse_x < 3 * mouse_y + 560) ||
            (mouse_y > 16 && mouse_y < 31 && mouse_x < mouse_y + 594))
        {
            if (!corner_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_ExitGameAndGo));
                corner_button_hover = true;
                result = 1;
            }
        }
        else if (corner_button_hover)
        {
            corner_button_hover = false;
            result = 1;
        }
    }
    else if (corner_button_hover)
    {
        corner_button_hover = false;
        result = 1;
    }
    if (result)
        corner_hover_dirty = true;
    return result;
}

// Lights the panel button under the mouse and puts its tip on the status line.
bool InterfaceController::UpdateButtonHover(int mouse_x, int mouse_y)
{
    bool result = 0;
    if (mouse_x >= 62 && mouse_x <= 98)
    {
        if (mouse_y >= 330 && mouse_y <= 348)
        {
            if (!inventory_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_SeeInventory));
                inventory_button_hover = true;
                result = 1;
            }
        }
        else if (inventory_button_hover)
        {
            inventory_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 350 && mouse_y <= 368)
        {
            if (!minimap_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_LookAtMap));
                minimap_button_hover = true;
                result = 1;
            }
        }
        else if (minimap_button_hover)
        {
            minimap_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 370 && mouse_y <= 388)
        {
            if (!spells_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_ActiveSkills));
                spells_button_hover = true;
                result = 1;
            }
        }
        else if (spells_button_hover)
        {
            spells_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 390 && mouse_y <= 408)
        {
            if (!skills_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_PassiveSkills));
                skills_button_hover = true;
                result = 1;
            }
        }
        else if (skills_button_hover)
        {
            skills_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 410 && mouse_y <= 428)
        {
            if (!chat_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_TalkWithOtherPeople));
                chat_button_hover = true;
                result = 1;
            }
        }
        else if (chat_button_hover)
        {
            chat_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 430 && mouse_y <= 448)
        {
            if (!status_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_CharacterStatus));
                status_button_hover = true;
                result = 1;
            }
        }
        else if (status_button_hover)
        {
            status_button_hover = false;
            result = 1;
        }
    }
    else if (inventory_button_hover || minimap_button_hover || spells_button_hover || skills_button_hover ||
        chat_button_hover || status_button_hover)
    {
        inventory_button_hover = false;
        result = 1;
        minimap_button_hover = false;
        spells_button_hover = false;
        skills_button_hover = false;
        chat_button_hover = false;
        status_button_hover = false;
    }
    if (mouse_x >= 590 && mouse_x <= 626)
    {
        if (mouse_y >= 350 && mouse_y <= 368)
        {
            if (!players_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_WhoIsOnline));
                players_button_hover = true;
                result = 1;
            }
        }
        else if (players_button_hover)
        {
            players_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 370 && mouse_y <= 388)
        {
            if (!group_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_GroupWithOtherPeople));
                group_button_hover = true;
                result = 1;
            }
        }
        else if (group_button_hover)
        {
            group_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 390 && mouse_y <= 408)
        {
            if (!third_right_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_KeyboardMacroAndHotkeys));
                third_right_button_hover = true;
                result = 1;
            }
        }
        else if (third_right_button_hover)
        {
            third_right_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 410 && mouse_y <= 428)
        {
            if (!settings_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_GameSettingsAndOptions));
                settings_button_hover = true;
                result = 1;
            }
        }
        else if (settings_button_hover)
        {
            settings_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 430 && mouse_y <= 448)
        {
            if (!help_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_GameHelp));
                help_button_hover = true;
                result = 1;
            }
        }
        else if (help_button_hover)
        {
            help_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 310 && mouse_y <= 327 && mouse_x <= 607)
        {
            if (!friends_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_FriendList));
                friends_button_hover = true;
                result = 1;
            }
        }
        else if (friends_button_hover)
        {
            friends_button_hover = false;
            result = 1;
        }
        if (mouse_y >= 310 && mouse_y <= 327 && mouse_x > 607)
        {
            if (!ignore_button_hover)
            {
                ShowStatusMessage(StatusCategory_Button, MainForm->strings->GetText(GameText_IgnoreList));
                ignore_button_hover = true;
                result = 1;
            }
        }
        else if (ignore_button_hover)
        {
            ignore_button_hover = false;
            result = 1;
        }
    }
    else if (players_button_hover || group_button_hover || third_right_button_hover || settings_button_hover ||
        help_button_hover || friends_button_hover || ignore_button_hover)
    {
        players_button_hover = false;
        group_button_hover = false;
        third_right_button_hover = false;
        settings_button_hover = false;
        help_button_hover = false;
        friends_button_hover = false;
        ignore_button_hover = false;
        result = 1;
    }
    if (result)
        button_hover_dirty = true;
    return result;
}

bool InterfaceController::TakeInvalidated()
{
    bool was_invalidated = 0;
    if (is_invalidated)
    {
        is_invalidated = false;
        was_invalidated = 1;
    }
    return was_invalidated;
}

// True for the next four calls after RequestFullRedraw.
bool InterfaceController::IsFullRedrawPending()
{
    bool is_pending = 0;
    if (full_redraw_pending)
    {
        is_pending = 1;
        if (full_redraw_countdown > 0)
            full_redraw_countdown--;
        else
            full_redraw_pending = false;
    }
    return is_pending;
}

void InterfaceController::ClearFullRedraw()
{
    full_redraw_pending = false;
    full_redraw_countdown = 0;
}

bool InterfaceController::TakeButtonHoverDirty()
{
    bool was_dirty = 0;
    if (button_hover_dirty)
    {
        button_hover_dirty = false;
        was_dirty = 1;
    }
    return was_dirty;
}

bool InterfaceController::TakeCornerHoverDirty()
{
    bool was_dirty = 0;
    if (corner_hover_dirty)
    {
        corner_hover_dirty = false;
        was_dirty = 1;
    }
    return was_dirty;
}

// Shows a text on the status line for 10 ticks, after the label of its category (6 is "Npc").
void InterfaceController::ShowStatusMessage(int category, String text)
{
    status_message = text;
    if (category == StatusCategory_Button)
        status_message = "[ " + MainForm->strings->GetText(GameText_Button) + " ] " + status_message;
    if (category == StatusCategory_Action)
        status_message = "[ " + MainForm->strings->GetText(GameText_Action) + " ] " + status_message;
    if (category == StatusCategory_Warning)
        status_message = "[ " + MainForm->strings->GetText(GameText_Warning) + " ] " + status_message;
    if (category == StatusCategory_Information)
        status_message = "[ " + MainForm->strings->GetText(GameText_Information) + " ] " + status_message;
    if (category == StatusCategory_Item)
        status_message = "[ " + MainForm->strings->GetText(GameText_Item) + " ] " + status_message;
    if (category == StatusCategory_Spell)
        status_message = "[ " + MainForm->strings->GetText(GameText_Spell) + " ] " + status_message;
    if (category == StatusCategory_Skill)
        status_message = "[ " + MainForm->strings->GetText(GameText_Skill) + " ] " + status_message;
    if (category == StatusCategory_Npc)
        status_message = "[ Npc ] " + status_message;
    status_message_state = 1;
    status_message_timer = 10;
}

// Once: 1 when a status text was set, 2 when it ran out; else 0.
int InterfaceController::TakeStatusChange()
{
    int state = 0;
    if (status_message_state == 1)
        state = 1;
    if (status_message_state == STATUS_MESSAGE_EXPIRED)
        state = 2;
    status_message_state = 0;
    return state;
}

String InterfaceController::GetStatusMessage()
{
    return status_message;
}

bool InterfaceController::IsScreenshotCooldown()
{
    bool is_cooling_down = 1;
    if (screenshot_cooldown <= 0)
        is_cooling_down = 0;
    return is_cooling_down;
}

void InterfaceController::StartScreenshotCooldown()
{
    screenshot_cooldown = 2;
}

// The player list button waits 5 ticks before it can be used again.
void InterfaceController::StartPlayerListCooldown()
{
    player_list_button_ready = false;
    player_list_cooldown = 5;
}

// Counts down the player list and screenshot waits and the status text.
void InterfaceController::OnTimer()
{
    if (player_list_cooldown >= 1)
    {
        player_list_cooldown--;
        if (!player_list_cooldown)
            player_list_button_ready = true;
    }
    if (screenshot_cooldown >= 1)
        screenshot_cooldown--;
    if (status_message_timer >= 1)
    {
        status_message_timer--;
        if (status_message_timer <= 0)
            status_message_state = STATUS_MESSAGE_EXPIRED;
    }
}

// Runs the timed text and the screen shake, which moves the screen a random amount each frame.
void InterfaceController::UpdateScreenEffects()
{
    if (timed_text_visible)
    {
        if (timed_text_duration > 0)
            timed_text_duration--;
        else
        {
            timed_text_visible = false;
            MainForm->redraw_needed = true;
        }
    }
    if (is_shaking)
    {
        shake_duration--;
        shake_offset = random(shake_amplitude);
        if (random(2) >= 1)
            shake_offset = 0 - shake_offset;
        if (shake_duration <= 0)
        {
            shake_offset = 0;
            is_shaking = false;
        }
        MainForm->redraw_needed = true;
    }
}

void InterfaceController::ShowTimedText(String text, int duration)
{
    timed_text_visible = true;
    timed_text_duration = duration;
    timed_text = text;
    MainForm->redraw_needed = true;
}

// Shakes for 10 + 3 x strength frames, by up to 10 + 4 x strength each way.
void InterfaceController::StartScreenShake(int strength)
{
    shake_duration = 3 * strength + 10;
    shake_amplitude = 4 * strength + 10;
    shake_offset = 0;
    is_shaking = true;
}

void InterfaceController::SetTradeRequesterId(int player_id)
{
    trade_requester_id = player_id;
}

int InterfaceController::GetTradeRequesterId()
{
    return trade_requester_id;
}
