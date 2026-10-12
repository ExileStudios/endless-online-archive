#include <vcl.h>
#pragma hdrstop

#include "Ctrl_status.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_gamebars.h"
#pragma package(smart_init)

#define MAX_LEVEL 240

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
    label_413_from = 0;
    label_413_to = 0;
    label_414_value = 0;
    label_416_value = 0;
    label_415_value = 0;
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
}

StatusController::~StatusController()
{
}

// Gives stamina back over time and blinks the training buttons. When the weight limit is out of range or stamina
// no longer matches its text copy, it zeroes the stamina maximum and shows a message instead.
void StatusController::Tick()
{
    if (train_confirm_ticks > 0)
        train_confirm_ticks--;
    if (max_weight > 255)
    {
        max_stamina = 0;
        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
            MainForm->strings->GetDialogText(DialogText_HelloHax0r, false),
            MainForm->strings->GetDialogText(DialogText_HelloHax0r, true));
        MainForm->edits->caret_enabled = false;
        return;
    }
    // The setters keep the text copies in step, so a mismatch means stamina changed some other way
    else if (StrToInt(stamina_check_text) != stamina || StrToInt(max_stamina_check_text) != max_stamina)
    {
        Sleep(500);
        max_stamina = 0;
        MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
            MainForm->strings->GetDialogText(DialogText_HelloHax0r, false),
            MainForm->strings->GetDialogText(DialogText_HelloHax0r, true));
        MainForm->edits->caret_enabled = false;
        return;
    }
    else
    {
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
                // An overloaded character gets no stamina back
                if (weight > max_weight)
                {
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_YouCannotAttackBecause));
                    stamina = 0;
                }
                MainForm->animation->interface_controller->game_bars->SetStaminaBar(stamina, max_stamina);
                is_dirty = 1;
            }
        }
        stamina_check_text = IntToStr(stamina);
        max_stamina_check_text = IntToStr(max_stamina);
        if (blink_phase < 10)
        {
            blink_phase++;
            if (blink_phase == 6)
                is_dirty = 1;
        }
        else
        {
            blink_phase = 0;
            is_dirty = 1;
        }
    }
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
                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_TrainStat, "Character training", "Do you want to train?");
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

// Sets health, not below zero, and updates the health bar.
void StatusController::SetHealth(int new_health)
{
    health = new_health;
    is_dirty = 1;
    if (health < 0)
        health = 0;
    MainForm->animation->interface_controller->game_bars->SetHealthBar(health, max_health);
}

// Sets mana, not below zero, and updates the mana bar.
void StatusController::SetMana(int new_mana)
{
    mana = new_mana;
    is_dirty = 1;
    if (mana < 0)
        mana = 0;
    MainForm->animation->interface_controller->game_bars->SetManaBar(mana, max_mana);
}

// Sets stamina, not below zero, updates the stamina bar and refreshes the text copies.
void StatusController::SetStamina(int new_stamina)
{
    stamina = new_stamina;
    is_dirty = 1;
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

// Adds a group share of experience and updates the experience bar and the amounts to the next level.
void StatusController::AddExperience(int gained_experience)
{
    int level_start_experience;
    int level_end_experience = 0;
    int level_range = 0;
    int bar_progress = 0;
    if (!skip_next_exp_gain)
    {
        if (gained_experience > experience)
        {
            last_exp_gain = gained_experience - experience;
            total_exp_gained += last_exp_gain;
            if (last_exp_gain > best_exp_gain)
                best_exp_gain = last_exp_gain;
        }
    }
    skip_next_exp_gain = false;
    experience += gained_experience;
    level_start_experience = MainForm->animation->game->GetExperienceForLevel(level);
    level_end_experience = MainForm->animation->game->GetExperienceForLevel(level + 1);
    level_range = level_end_experience - level_start_experience;
    bar_progress = level_end_experience;
    if (level < MAX_LEVEL)
    {
        if (experience < level_end_experience)
        {
            bar_progress = experience - level_start_experience;
            next_level_experience = level_end_experience;
        }
        else
        {
            bar_progress = level_range;
            next_level_experience = level_end_experience;
        }
    }
    else
        next_level_experience = MainForm->animation->game->GetExperienceForLevel(level);
    if (experience < level_start_experience)
    {
        bar_progress = 0;
        experience_to_next_level = 0;
    }
    else
        experience_to_next_level = level_range - bar_progress;
    MainForm->animation->interface_controller->game_bars->SetExperienceBar(bar_progress, level_range);
}

// Sets the experience total from the server, counts the gain for the stats window, and updates the experience bar
// and the amounts to the next level.
void StatusController::SetExperience(int new_experience)
{
    int level_start_experience;
    int level_end_experience = 0;
    int level_range = 0;
    int bar_progress = 0;
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
    skip_next_exp_gain = false;
    experience = new_experience;
    level_start_experience = MainForm->animation->game->GetExperienceForLevel(level);
    level_end_experience = MainForm->animation->game->GetExperienceForLevel(level + 1);
    level_range = level_end_experience - level_start_experience;
    bar_progress = level_end_experience;
    if (level < MAX_LEVEL)
    {
        if (experience < level_end_experience)
        {
            bar_progress = experience - level_start_experience;
            next_level_experience = level_end_experience;
        }
        else
        {
            bar_progress = level_range;
            next_level_experience = level_end_experience;
        }
    }
    else
        next_level_experience = MainForm->animation->game->GetExperienceForLevel(level);
    if (experience < level_start_experience)
    {
        bar_progress = 0;
        experience_to_next_level = 0;
    }
    else
        experience_to_next_level = level_range - bar_progress;
    MainForm->animation->interface_controller->game_bars->SetExperienceBar(bar_progress, level_range);
}

// Sets the level and updates the experience bar and the amounts to the next level.
void StatusController::SetLevel(int new_level)
{
    level = new_level;
    int level_start_experience = MainForm->animation->game->GetExperienceForLevel(this->level);
    int level_end_experience = MainForm->animation->game->GetExperienceForLevel(level + 1);
    int level_range = level_end_experience - level_start_experience;
    int bar_progress = level_end_experience;
    if (level < MAX_LEVEL)
    {
        if (this->experience < level_end_experience)
        {
            bar_progress = this->experience - level_start_experience;
            next_level_experience = level_end_experience;
        }
        else
        {
            bar_progress = level_range;
            next_level_experience = level_end_experience;
        }
    }
    else
        next_level_experience = MainForm->animation->game->GetExperienceForLevel(level);
    if (this->experience < level_start_experience)
    {
        bar_progress = 0;
        experience_to_next_level = 0;
    }
    else
        this->experience_to_next_level = level_range - bar_progress;
    MainForm->animation->interface_controller->game_bars->SetExperienceBar(bar_progress, level_range);
}

// True when the guild tag is a real tag: blank tags and tags starting with a zero do not count.
bool StatusController::IsInGuild()
{
    bool in_guild = 1;
    if (guild_tag.Length() < 2)
        in_guild = 0;
    else
    {
        if (guild_tag[1] == '0' || guild_tag[1] == ' ')
            in_guild = 0;
        if (guild_tag[2] == ' ')
            in_guild = 0;
    }
    return in_guild;
}

DEFINE_TEST_AND_CLEAR_DIRTY(StatusController)

void StatusController::MarkDirty()
{
    is_dirty = 1;
}
