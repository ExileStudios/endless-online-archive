#include <vcl.h>
#pragma hdrstop

#include "Ctrl_status.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_gamebars.h"

#pragma package(smart_init)

// Every stat starts at zero, and the stamina at 20 of 20.
StatusController::StatusController()
{
    weight = 0;
    max_weight = 0;
    health = 0;
    max_health = 0;
    mana = 0;
    max_mana = 0;
    stamina = 20;
    max_stamina = 20;
    damage_min = 0;
    damage_max = 0;
    hitrate = 0;
    dodge = 0;
    armor = 0;
    stamina_check_text = IntToStr(stamina);
    max_stamina_check_text = IntToStr(max_stamina);
    unused_value = 0;
    stat_points = 0;
    skill_points = 0;
    train_confirm_ticks = 0;
    skip_next_exp_gain = true;
    total_exp_gained = 0;
    last_exp_gain = 0;
    best_exp_gain = 0;
    stamina_regen_ticks = 0;
    is_low_health = false;
    flash_ticks = 0;
}

StatusController::~StatusController()
{
}

// One status tick: the training question's wait, stamina coming back, faster while sitting on the floor, and the
// warning sound while health is under 19 percent.
void StatusController::Tick()
{
    if (train_confirm_ticks > 0)
        train_confirm_ticks--;
    if (max_weight > 255)
    {
        max_stamina = 0;
        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
            MainForm->strings->GetDialogText(DialogText_HelloHax0r, false),
            MainForm->strings->GetDialogText(DialogText_HelloHax0r, true));
        MainForm->edits->caret_enabled = false;
        return;
    }
    if (stamina < max_stamina)
    {
        stamina_regen_ticks++;
        // Faster while sitting on the floor
        if (stamina_regen_ticks > 14 ||
            (stamina_regen_ticks == 7 && MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId())))
        {
            stamina_regen_ticks = 0;
            int regen_amount = 1;
            int tenth_max_stamina = max_stamina / 10;
            if (tenth_max_stamina > 0)
                regen_amount = tenth_max_stamina;
            stamina += regen_amount;
            if (max_stamina < stamina)
                stamina = max_stamina;
            MainForm->animation->interface_controller->game_bars->SetStaminaBar(stamina, max_stamina);
            is_dirty = true;
        }
    }
    stamina_check_text = IntToStr(stamina);
    max_stamina_check_text = IntToStr(max_stamina);
    if (blink_phase < 10)
    {
        blink_phase++;
        if (blink_phase == 6)
            is_dirty = true;
    }
    else
    {
        blink_phase = 0;
        is_dirty = true;
    }
    if (MainForm->animation->GetScreenMode() != ScreenMode_Game)
        return;
    if (GetHealthPercent() < 19)
    {
        flash_ticks = 0;
        if (!is_low_health)
        {
            is_low_health = true;
            low_health_ticks = 20;
        }
        low_health_ticks++;
        if (low_health_ticks < 14)
            MainForm->redraw_needed = true;
        if (low_health_ticks > 20)
        {
            low_health_ticks = 0;
            MainForm->sfx->PlaySfx(SfxSound_LowHealth);
        }
    }
    else
    {
        if (flash_ticks > 0)
        {
            flash_ticks--;
            MainForm->redraw_needed = true;
        }
        is_low_health = false;
    }
}

bool StatusController::IsLowHealth()
{
    return is_low_health;
}

// The low health flash's alpha: 40 down to 10 over ticks 0 to 3, then 50 down to 5 until tick 14.
char StatusController::GetLowHealthAlpha()
{
    int alpha = 0;
    if (low_health_ticks < 4)
        alpha = 10 * (4 - low_health_ticks);
    else if (low_health_ticks < 14)
        alpha = 5 * (14 - low_health_ticks);
    return alpha;
}

// Flashes the screen in the colour for 12 ticks.
void StatusController::FlashScreen(int color)
{
    flash_color = color;
    flash_ticks = 12;
}

// The flash's alpha, 4 for each tick left.
unsigned char StatusController::GetFlashAlpha()
{
    return flash_ticks << 2;
}

int StatusController::GetFlashColor()
{
    return flash_color;
}

// A click on a stat's training button while stat points are left.
void StatusController::MouseDown(int x, int y, int button)
{
    if (stat_points > 0)
    {
        train_stat_index = 0;
        if (x > 208 && x < 227 && y > 337 && y < 354)
            train_stat_index = 1;
        if (x > 208 && x < 227 && y > 355 && y < 372)
            train_stat_index = 2;
        if (x > 208 && x < 227 && y > 373 && y < 390)
            train_stat_index = 3;
        if (x > 208 && x < 227 && y > 391 && y < 408)
            train_stat_index = 4;
        if (x > 208 && x < 227 && y > 409 && y < 426)
            train_stat_index = 5;
        if (x > 208 && x < 227 && y > 427 && y < 444)
            train_stat_index = 6;
        // A click just after a training is ignored
        if (train_stat_index > 0 && train_confirm_ticks < 195)
        {
            // Asks first, unless a training was sent recently
            if (train_confirm_ticks <= 0)
            {
                MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_TrainStat,
                    "Character training", "Do you want to train?");
                MainForm->edits->caret_enabled = false;
            }
            else
            {
                MainForm->net_client->sender->SendTrainStat(train_stat_index);
                train_confirm_ticks = TRAIN_CONFIRM_TICKS;
            }
        }
    }
}

int StatusController::GetBlinkPhase()
{
    return blink_phase;
}

// Sets the health, at least 0; a rise or drop shows the bar's text when the options ask for it.
void StatusController::SetHealth(int new_health, char show_change)
{
    if (show_change)
    {
        if (new_health > health && MainForm->config->GetAutoDropRecoverBar())
            MainForm->animation->interface_controller->game_bars->ToggleHealthTextFor(true, false, 14);
        else if (new_health < health && MainForm->config->GetAutoDropDamageBar())
            MainForm->animation->interface_controller->game_bars->ToggleHealthTextFor(true, false, 14);
    }
    health = new_health;
    is_dirty = true;
    if (health < 0)
        health = 0;
    MainForm->animation->interface_controller->game_bars->SetHealthBar(health, max_health);
}

// Sets the mana, at least 0; a rise or drop shows the bar's text when the options ask for it.
void StatusController::SetMana(int new_mana, char show_change)
{
    if (show_change)
    {
        if (new_mana > mana && MainForm->config->GetAutoDropRecoverBar())
            MainForm->animation->interface_controller->game_bars->ToggleManaTextFor(true, false, 14);
        else if (new_mana < mana && MainForm->config->GetAutoDropDamageBar())
            MainForm->animation->interface_controller->game_bars->ToggleManaTextFor(true, false, 14);
    }
    mana = new_mana;
    is_dirty = true;
    if (mana < 0)
        mana = 0;
    MainForm->animation->interface_controller->game_bars->SetManaBar(mana, max_mana);
}

// Sets the stamina, kept at 0 or more, and its bar; a gain or loss shows the bar's text when the settings ask for it.
void StatusController::SetStamina(int new_stamina, char show_change)
{
    if (show_change)
    {
        if (new_stamina > stamina && MainForm->config->GetAutoDropRecoverBar())
            MainForm->animation->interface_controller->game_bars->ToggleStaminaTextFor(true, false, 14);
        else if (new_stamina < stamina && MainForm->config->GetAutoDropDamageBar())
            MainForm->animation->interface_controller->game_bars->ToggleStaminaTextFor(true, false, 14);
    }
    stamina = new_stamina;
    is_dirty = true;
    if (stamina < 0)
        stamina = 0;
    MainForm->animation->interface_controller->game_bars->SetStaminaBar(stamina, max_stamina);
    stamina_check_text = IntToStr(stamina);
    max_stamina_check_text = IntToStr(max_stamina);
}

// Sets the health maximum and updates the health bar.
void StatusController::SetMaxHealth(int new_max_health)
{
    max_health = new_max_health;
    MainForm->animation->interface_controller->game_bars->SetHealthBar(health, max_health);
}

// Sets the mana maximum and updates the mana bar.
void StatusController::SetMaxMana(int new_max_mana)
{
    max_mana = new_max_mana;
    MainForm->animation->interface_controller->game_bars->SetManaBar(mana, max_mana);
}

// Sets the stamina maximum, lowering stamina to it, updates the bar and refreshes the text copies.
void StatusController::SetMaxStamina(int new_max_stamina)
{
    max_stamina = new_max_stamina;
    if (max_stamina < stamina)
        stamina = max_stamina;
    MainForm->animation->interface_controller->game_bars->SetStaminaBar(stamina, max_stamina);
    stamina_check_text = IntToStr(stamina);
    max_stamina_check_text = IntToStr(max_stamina);
}

// The health as a percent of the maximum, at most 100; 0 when either is 0.
short StatusController::GetHealthPercent()
{
    short percent;
    if (!health)
        percent = 0;
    else if (!max_health)
        percent = 0;
    else
    {
        percent = 100 * health / max_health;
        if (percent > 100)
            percent = 100;
    }
    return percent;
}

// The mana as a percent of the maximum, at most 100; 0 when either is 0.
short StatusController::GetManaPercent()
{
    short percent;
    if (!mana)
        percent = 0;
    else if (!max_mana)
        percent = 0;
    else
    {
        percent = 100 * mana / max_mana;
        if (percent > 100)
            percent = 100;
    }
    return percent;
}

String StatusController::GetWeightText()
{
    String text = IntToStr(weight);
    text += " / ";
    text += IntToStr(max_weight);
    text += " carried.";
    return text;
}

// Sets the experience, keeping the last and best gains, and fills the experience bar by the progress through the level.
void StatusController::SetExperience(int new_experience)
{
    int level_index;
    // The first total after logging in is not counted as a gain
    if (!skip_next_exp_gain)
    {
        if (new_experience > experience)
        {
            last_exp_gain = new_experience - experience;
            total_exp_gained += last_exp_gain;
            if (last_exp_gain > best_exp_gain)
                best_exp_gain = last_exp_gain;
        }
    }
    if (new_experience > experience && MainForm->config->GetAutoDropExpBar())
        MainForm->animation->interface_controller->game_bars->ToggleExperienceTextFor(true, false, 14);
    skip_next_exp_gain = false;
    experience = new_experience;
    for (level_index = 0; level_index <= 252; level_index++)
    {
        if (MainForm->animation->game->GetExperienceForLevel(level_index + 1) >= experience || level_index >= 252)
            break;
    }
    int level_start = MainForm->animation->game->GetExperienceForLevel(level_index);
    int next_level_start = MainForm->animation->game->GetExperienceForLevel(level_index + 1);
    if (level_index < level)
        next_level_start = MainForm->animation->game->GetExperienceForLevel(level + 1);
    next_level_experience = next_level_start - level_start;
    experience_to_next_level = next_level_start - experience;
    int progress = next_level_experience - experience_to_next_level;
    MainForm->animation->interface_controller->game_bars->SetExperienceBar(progress, next_level_experience);
    is_dirty = true;
}

// Sets the level and updates the experience bar and the amounts to the next level.
void StatusController::SetLevel(int new_level)
{
    level = new_level;
    is_dirty = true;
}

// True when the guild tag is a real tag: blank tags and tags starting with a zero do not count.
bool StatusController::IsInGuild()
{
    bool in_guild = true;
    if (guild_tag.Length() < 2)
        in_guild = false;
    else
    {
        if (guild_tag[1] == '0' || guild_tag[1] == ' ')
            in_guild = false;
        if (guild_tag[2] == ' ')
            in_guild = false;
    }
    return in_guild;
}

DEFINE_TEST_AND_CLEAR_DIRTY(StatusController)

void StatusController::MarkDirty()
{
    is_dirty = true;
}
