#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Ctrl_book.h"
#include "Ctrl_credits.h"
#include "Ctrl_dialog.h"
#include "Ctrl_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_into.h"
#include "Ctrl_jukebox.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Ctrl_menu.h"
#include "Ctrl_musician.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_priest.h"
#include "Ctrl_quests.h"
#include "Ctrl_report.h"
#include "Ctrl_social.h"
#include "Ctrl_stats.h"
#include "Packetoperations.h"
#pragma package(smart_init)

#define MAX_RECENT_CHAT_MESSAGES 3
#define MAX_RECENT_GLOBAL_MESSAGES 2

// Starts on the intro, or straight on the menu screen with show_intro, and makes every window controller.
AnimationController::AnimationController(char show_intro)
{
    if (!show_intro)
    {
        screen_mode = ScreenMode_Intro;
        sub_state = MenuScreen_Main;
        frame_counter = 0;
    }
    else
    {
        screen_mode = ScreenMode_Menu;
        sub_state = MenuScreen_Main;
        frame_counter = 54;
        Screen->Cursor = crDefault;
        if (MainForm->config->GetFullscreen())
            SetCursorPos(20, 20);
    }
    animating = true;
    account_progress = 0;
    reply_timeout = 0;
    chat_delay = 0;
    chat_decay_ticks = 0;
    chat_message_count = 0;
    global_decay_ticks = 0;
    global_message_count = 0;
    chat_decay_check = IntToHex(chat_decay_ticks, 2);
    chat_count_check = IntToHex(chat_message_count, 2);
    object_click_delay = 0;
    tile_use_delay = 0;
    walk_block = 0;
    command_delay = 600;
    command_delay_length = 600;
    emote_delayed = false;
    refresh_delayed = false;
    sit_delayed = false;
    path_delayed = false;
    intro = new IntroController();
    menu = new MenuController();
    credits = new CreditsController();
    interface_controller = new InterfaceController();
    game = new GameController();
    paperdoll = new PaperdollController();
    book = new BookController();
    trade = new TradeController();
    shop = new ShopController();
    dialog = new DialogController();
    learn = new LearnController();
    priest = new PriestController();
    guild = new GuildController();
    memo = new MemoController();
    quests = new QuestController();
    report = new ReportController();
    law = new LawController();
    inn = new InnController();
    chest = new ChestController();
    bank = new BankController();
    jukebox = new JukeboxController();
    musician = new MusicianController();
    barber = new BarberController();
    locker = new LockerController();
    stat = new StatController();
    social = new SocialController();
    message_board = new MsgBoardController();
}

AnimationController::~AnimationController()
{
    delete intro;
    delete menu;
    delete credits;
    delete interface_controller;
    delete game;
    delete paperdoll;
    delete book;
    delete trade;
    delete bank;
    delete jukebox;
    delete barber;
    delete shop;
    delete dialog;
    delete guild;
    delete priest;
    delete musician;
    delete law;
    delete memo;
    delete report;
    delete chest;
    delete locker;
    delete stat;
    delete social;
    delete message_board;
}

void AnimationController::SetScreenMode(int new_screen_mode)
{
    screen_mode = new_screen_mode;
    SetFrameCounter(0);
}

void AnimationController::SetSubState(int new_sub_state)
{
    sub_state = new_sub_state;
    Sleep(10);
}

void AnimationController::SetFrameCounter(int frame)
{
    frame_counter = frame;
}

int AnimationController::GetSubState()
{
    return sub_state;
}

int AnimationController::GetScreenMode()
{
    return screen_mode;
}

bool AnimationController::IsAnimating()
{
    return animating;
}

void AnimationController::StartObjectClickDelay()
{
    object_click_delay = 4;
}

bool AnimationController::IsObjectClickDelayed()
{
    bool delayed = 0;
    if (object_click_delay > 0)
        delayed = 1;
    return delayed;
}

void AnimationController::SetCommandDelay(int seconds)
{
    command_delay = 15 * seconds;
    command_delay_length = 15 * seconds;
}

void AnimationController::RestartCommandDelay()
{
    command_delay = command_delay_length;
}

bool AnimationController::IsCommandDelayed()
{
    bool delayed = 0;
    if (command_delay > 0)
        delayed = 1;
    return delayed;
}

void AnimationController::StartTileUseDelay()
{
    tile_use_delay = 14;
}

bool AnimationController::IsTileUseDelayed()
{
    bool delayed = 0;
    if (tile_use_delay > 0)
        delayed = 1;
    return delayed;
}

void AnimationController::BlockWalking()
{
    walk_block = 14;
}

bool AnimationController::IsWalkingBlocked()
{
    bool blocked = 0;
    if (walk_block > 0)
        blocked = 1;
    return blocked;
}

// Counts the action and chat delays down, lets the chat counts decay, and scrolls the credits.
void AnimationController::UpdateTimers()
{
    // Every write in this class updates the hex copy with the count.
    // They differ only when a count was changed from outside, and then the client stalls for a second.
    if (IntToHex(chat_decay_ticks, 2) != chat_decay_check || IntToHex(chat_message_count, 2) != chat_count_check)
        Sleep(1001);
    if (object_click_delay > 0)
        object_click_delay--;
    if (command_delay > 0)
        command_delay--;
    if (tile_use_delay > 0)
        tile_use_delay--;
    if (walk_block > 0)
        walk_block--;
    if (chat_delay > 0)
        chat_delay--;
    // One chat message is forgiven each time the decay countdown runs out.
    if (chat_decay_ticks > 0)
    {
        chat_decay_ticks--;
        chat_decay_check = IntToHex(chat_decay_ticks, 2);
    }
    else
    {
        if (chat_message_count > 0)
        {
            chat_message_count--;
            chat_count_check = IntToHex(chat_message_count, 2);
        }
        chat_decay_ticks = 44;
        chat_decay_check = IntToHex(chat_decay_ticks, 2);
    }
    if (global_decay_ticks > 0)
        global_decay_ticks--;
    else
    {
        if (global_message_count > 0)
            global_message_count--;
        global_decay_ticks = 550;
    }
    // The credits: a new line each time its delay passes, each line one pixel higher a step.
    // A head line follows soonest and a title line waits longest.
    if (screen_mode == ScreenMode_Menu)
    {
        if (credits->is_running)
        {
            if (credits->is_running)
            {
                credits->tick_counter++;
                CreditsController *credits_controller = credits;
                if (credits_controller->credit_styles->Count > credits_controller->next_line_index)
                {
                    int line_delay = 35;
                    if (credits->credit_styles->Strings[credits->next_line_index] == "head")
                        line_delay = 4;
                    if (credits->credit_styles->Strings[credits->next_line_index] == "title")
                        line_delay = 65;
                    if (line_delay <= credits->tick_counter)
                    {
                        credits->tick_counter = 0;
                        credits->shown_lines->Add(credits->credit_texts->Strings[credits->next_line_index]);
                        credits->shown_styles->Add(credits->credit_styles->Strings[credits->next_line_index]);
                        credits->shown_y->Add(IntToStr(credits->start_y));
                        credits->next_line_index++;
                    }
                }
                else if (credits->start_y - credits->top_y + 10 <= credits->tick_counter)
                {
                    MainForm->gfx->LoadMainMenuScreen(true);
                    credits->is_running = false;
                }
            }
            for (int i = 0; i < credits->shown_lines->Count; i++)
                credits->shown_y->Strings[i] = StrToInt(credits->shown_y->Strings[i]) - 1;
            if (credits->shown_lines->Count >= 1)
            {
                if (StrToInt(credits->shown_y->Strings[0]) <= credits->top_y)
                {
                    credits->shown_lines->Delete(0);
                    credits->shown_styles->Delete(0);
                    credits->shown_y->Delete(0);
                }
            }
        }
    }
}

void AnimationController::StartChatDelay()
{
    chat_delay = 5;
}

// Counts a chat message; the third before a decay jumps the count, so the wait is longer than one decay.
void AnimationController::RecordChatMessage()
{
    chat_message_count++;
    if (chat_message_count >= MAX_RECENT_CHAT_MESSAGES)
        chat_message_count = 6;
    chat_count_check = IntToHex(chat_message_count, 2);
}

// Counts a global message; the second before a decay jumps the count, so the wait is longer than one decay.
void AnimationController::RecordGlobalMessage()
{
    global_message_count++;
    if (global_message_count >= MAX_RECENT_GLOBAL_MESSAGES)
        global_message_count = 4;
}

bool AnimationController::CanSendChat()
{
    bool allowed = 0;
    if (chat_delay <= 0)
        allowed = 1;
    return allowed;
}

// Whether chat may be sent now; when not, the status bar says so.
bool AnimationController::IsChatAllowed()
{
    bool allowed = 0;
    if (chat_message_count < MAX_RECENT_CHAT_MESSAGES)
        allowed = 1;
    else
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_WaitMinuteBeforeTalking));
    return allowed;
}

// Whether a global message may be sent now; when not, the status bar says so.
bool AnimationController::IsGlobalMessageAllowed()
{
    bool allowed = 0;
    if (global_message_count < MAX_RECENT_GLOBAL_MESSAGES)
        allowed = 1;
    else
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_WaitMinuteBeforeUsing));
    return allowed;
}

void AnimationController::StartAccountProgress()
{
    account_progress = 1;
}

void AnimationController::StopAccountProgress()
{
    account_progress = 0;
}

int AnimationController::GetAccountProgress()
{
    return account_progress;
}

void AnimationController::StartReplyTimeout()
{
    reply_timeout = 1;
}

void AnimationController::StopReplyTimeout()
{
    reply_timeout = 0;
}

void AnimationController::StartEmoteDelay(int ticks)
{
    emote_delayed = true;
    emote_delay = ticks;
}

bool AnimationController::IsEmoteDelayed()
{
    return emote_delayed;
}

void AnimationController::StartRefreshDelay(int ticks)
{
    refresh_delayed = true;
    refresh_delay = ticks;
}

bool AnimationController::IsSitDelayed()
{
    return sit_delayed;
}

void AnimationController::StartSitDelay(int ticks)
{
    sit_delayed = true;
    sit_delay = ticks;
}

bool AnimationController::IsPathDelayed()
{
    return path_delayed;
}

void AnimationController::StartPathDelay(int ticks)
{
    path_delayed = true;
    path_delay = ticks;
}

bool AnimationController::IsRefreshDelayed()
{
    return refresh_delayed;
}

// One step of the short delays, the loading spinner, the world reply wait, the account creation wait and the intro.
void AnimationController::UpdateIntroAndDelays()
{
    if (emote_delayed)
    {
        emote_delay--;
        if (emote_delay <= 0)
            emote_delayed = false;
    }
    if (refresh_delayed)
    {
        refresh_delay--;
        if (refresh_delay <= 0)
            refresh_delayed = false;
    }
    if (sit_delayed)
    {
        sit_delay--;
        if (sit_delay <= 0)
            sit_delayed = false;
    }
    if (path_delayed)
    {
        path_delay--;
        if (path_delay <= 0)
            path_delayed = false;
    }
    if (animating)
        frame_counter++;
    if (MainForm->animation->menu->is_loading_world)
    {
        MainForm->animation->menu->loading_frame++;
        if (MainForm->animation->menu->loading_frame >= 8)
            MainForm->animation->menu->loading_frame = 0;
    }
    // Partway through the world reply wait the menu is marked as entering the game.
    // Later the wait gives up with a message.
    if (reply_timeout >= 1 && menu->is_loading_world)
    {
        reply_timeout++;
        if (reply_timeout == 50)
        {
            menu->SetEnteringGame(1);
            menu->StartRequestDelay();
            reply_timeout = 0;
        }
        if (reply_timeout == 175)
        {
            MainForm->gfx->FreeGameGraphics();
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_ServerBusy, false),
                MainForm->strings->GetDialogText(DialogText_ServerBusy, true));
            menu->is_loading_world = false;
            reply_timeout = 0;
        }
    }
    // The account is sent once the message box has stayed up long enough.
    // Closing the box first cancels it.
    if (account_progress >= 1)
    {
        if (menu->is_message_open)
        {
            account_progress++;
            if (account_progress >= 512)
            {
                if (GetScreenMode() == ScreenMode_Menu && GetSubState() == MenuScreen_CreateAccount)
                {
                    MainForm->net_client->sender->SendAccountCreate(account_name, account_password, account_detail_a,
                        account_detail_b, account_email);
                    account_name = "";
                    account_password = "";
                    account_detail_a = "";
                    account_detail_b = "";
                    account_email = "";
                    menu->is_message_open = false;
                }
                account_progress = 0;
            }
        }
        else
        {
            MainForm->edits->caret_enabled = true;
            MainForm->animation->menu->create_account_enabled = true;
            account_progress = 0;
        }
    }
    if (screen_mode == ScreenMode_Menu && sub_state == MenuScreen_CharacterSelect)
    {
        if (menu->character_select_cooldown > 0)
            menu->character_select_cooldown--;
    }
    // The intro: the logo and its sound, then the menu music and the menu screen.
    if (screen_mode == ScreenMode_Intro)
    {
        if (frame_counter == 1 || frame_counter == 2 || frame_counter == 11)
            intro->clear_screen_pending = true;
        if (frame_counter == 17)
        {
            MainForm->sfx->PlaySfx(1);
            intro->logo_redraw_pending = true;
            intro->logo_sound_played = true;
        }
        if (frame_counter == 22)
            intro->logo_redraw_pending = true;
        if (frame_counter >= 34 && frame_counter <= 36)
        {
            intro->logo_redraw_pending = false;
            intro->logo_sound_played = false;
            intro->is_animating = false;
            intro->clear_screen_pending = true;
        }
        if (frame_counter == 40)
        {
            MainForm->sfx->QueueMusic(1);
            Screen->Cursor = crDefault;
            if (MainForm->config->GetFullscreen())
            {
                if (Application->Active)
                    SetCursorPos(20, 20);
            }
            screen_mode = ScreenMode_Menu;
            sub_state = MenuScreen_Main;
            menu->unread_flag_at_48 = true;
            MainForm->gfx->FreeIntroGraphics();
        }
    }
}
