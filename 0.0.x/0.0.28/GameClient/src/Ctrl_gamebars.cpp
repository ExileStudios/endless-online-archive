#include <vcl.h>
#pragma hdrstop

#include "Ctrl_gamebars.h"
#include "Tmainform.h"
#include "Packetoperations.h"
#include "Ctrl_stats.h"
#pragma package(smart_init)

#define BAR_FULL_SEGMENTS 20
#define PERCENT_SCALE 100.0

GameBars::GameBars()
{
    health_segments = BAR_FULL_SEGMENTS;
    mana_segments = BAR_FULL_SEGMENTS;
    stamina_segments = BAR_FULL_SEGMENTS;
    experience_segments = BAR_FULL_SEGMENTS;
    quest_request_cooldown = 0;
    is_dirty = 1;
    show_health_text = false;
    show_mana_text = false;
    show_stamina_text = false;
    show_experience_text = false;
}

GameBars::~GameBars()
{
}

// A left click on the strip, by x:
//  55 to  74 = opens the stats window
//  75 to  94 = asks for the quests in progress, at most once in 20 ticks
//  99 to 213 = shows or hides the health numbers
// 214 to 322 = shows or hides the mana numbers
// 325 to 433 = shows or hides the stamina numbers
// 434 to 542 = shows or hides the experience numbers
void GameBars::MouseDown(int x, int y, int button)
{
    if (!button)
    {
        if (x >= 55 && x <= 74 && y >= 0 && y <= 10)
        {
            MainForm->animation->stat->Open();
            MainForm->sfx->PlaySfx(13);
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
        if (x >= 75 && x <= 94 && y >= 0 && y <= 10)
        {
            if (quest_request_cooldown <= 0)
            {
                MainForm->net_client->sender->RequestQuestProgress();
                quest_request_cooldown = 20;
            }
            MainForm->sfx->PlaySfx(13);
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
        if (x >= 99 && x <= 213 && y >= 0 && y <= 10)
        {
            if (!show_health_text)
            {
                show_health_text = true;
                health_text_timer = 40;
            }
            else
            {
                show_health_text = false;
                health_text_timer = 0;
            }
            MainForm->sfx->PlaySfx(13);
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
        if (x >= 214 && x <= 322 && y >= 0 && y <= 10)
        {
            if (!show_mana_text)
            {
                show_mana_text = true;
                mana_text_timer = 40;
            }
            else
            {
                show_mana_text = false;
                mana_text_timer = 0;
            }
            MainForm->sfx->PlaySfx(13);
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
        if (x >= 325 && x <= 433 && y >= 0 && y <= 10)
        {
            if (!show_stamina_text)
            {
                show_stamina_text = true;
                stamina_text_timer = 40;
            }
            else
            {
                show_stamina_text = false;
                stamina_text_timer = 0;
            }
            MainForm->sfx->PlaySfx(13);
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
        if (x >= 434 && x <= 542 && y >= 0 && y <= 10)
        {
            if (!show_experience_text)
            {
                show_experience_text = true;
                experience_text_timer = 40;
            }
            else
            {
                show_experience_text = false;
                experience_text_timer = 0;
            }
            MainForm->sfx->PlaySfx(13);
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
    }
}

// Hides the numbers of a bar 40 ticks after they were shown, and counts down the quest button delay.
void GameBars::Tick()
{
    if (show_health_text)
    {
        health_text_timer--;
        if (health_text_timer <= 0)
        {
            show_health_text = false;
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
    }
    if (show_mana_text)
    {
        mana_text_timer--;
        if (mana_text_timer <= 0)
        {
            show_mana_text = false;
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
    }
    if (show_stamina_text)
    {
        stamina_text_timer--;
        if (stamina_text_timer <= 0)
        {
            show_stamina_text = false;
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
    }
    if (show_experience_text)
    {
        experience_text_timer--;
        if (experience_text_timer <= 0)
        {
            show_experience_text = false;
            is_dirty = 1;
            MainForm->redraw_needed = true;
        }
    }
    if (quest_request_cooldown > 0)
        quest_request_cooldown--;
}

// One segment for each full 5 percent of max_health; above 98 percent the bar is full.
void GameBars::SetHealthBar(int health, int max_health)
{
    double fraction = 0.0;
    if (max_health > 0 && health > 0)
        fraction = (double)health / (double)max_health;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        health_segments = percent / 5;
    else
        health_segments = 0;
    if (percent > 98)
        health_segments = BAR_FULL_SEGMENTS;
    is_dirty = 1;
    MainForm->redraw_needed = true;
}

// One segment for each full 5 percent of max_mana; above 98 percent the bar is full.
void GameBars::SetManaBar(int mana, int max_mana)
{
    double fraction = 0.0;
    if (max_mana > 0 && mana > 0)
        fraction = (double)mana / (double)max_mana;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        mana_segments = percent / 5;
    else
        mana_segments = 0;
    if (percent > 98)
        mana_segments = BAR_FULL_SEGMENTS;
    is_dirty = 1;
    MainForm->redraw_needed = true;
}

// One segment for each full 5 percent of max_stamina; above 98 percent the bar is full.
void GameBars::SetStaminaBar(int stamina, int max_stamina)
{
    double fraction = 0.0;
    if (max_stamina > 0 && stamina > 0)
        fraction = (double)stamina / (double)max_stamina;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        stamina_segments = percent / 5;
    else
        stamina_segments = 0;
    if (percent > 98)
        stamina_segments = BAR_FULL_SEGMENTS;
    is_dirty = 1;
    MainForm->redraw_needed = true;
}

// One segment for each full 5 percent of the way through the level; above 98 percent the bar is full.
void GameBars::SetExperienceBar(int level_progress, int level_span)
{
    double fraction = 0.0;
    if (level_span > 0 && level_progress > 0)
        fraction = (double)level_progress / (double)level_span;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        experience_segments = percent / 5;
    else
        experience_segments = 0;
    if (percent > 98)
        experience_segments = BAR_FULL_SEGMENTS;
    is_dirty = 1;
    MainForm->redraw_needed = true;
}

int GameBars::GetHealthBarSegments()
{
    return health_segments;
}

int GameBars::GetManaBarSegments()
{
    return mana_segments;
}

int GameBars::GetStaminaBarSegments()
{
    return stamina_segments;
}

int GameBars::GetExperienceBarSegments()
{
    return experience_segments;
}

void GameBars::MarkDirty()
{
    is_dirty = 1;
}

DEFINE_TEST_AND_CLEAR_DIRTY(GameBars)
