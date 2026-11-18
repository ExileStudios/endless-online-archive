#include <vcl.h>
#pragma hdrstop

#include "Ctrl_gamebars.h"
#include "Tmainform.h"
#include "Packetoperations.h"
#include "Ctrl_stats.h"

#pragma package(smart_init)

GameBars::GameBars()
{
    health_segments = BAR_FULL_SEGMENTS;
    mana_segments = BAR_FULL_SEGMENTS;
    stamina_segments = BAR_FULL_SEGMENTS;
    experience_segments = BAR_FULL_SEGMENTS;
    quest_request_cooldown = 0;
    is_dirty = true;
    show_health_text = false;
    show_mana_text = false;
    show_stamina_text = false;
    show_experience_text = false;
}

GameBars::~GameBars()
{
}

// Shows the text of all four bars.
void GameBars::ShowAllTexts()
{
    ToggleHealthText(true, false);
    ToggleManaText(true, false);
    ToggleStaminaText(true, false);
    ToggleExperienceText(true, false);
}

void GameBars::ToggleHealthText(bool force, bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    if (!show_health_text || force)
    {
        show_health_text = true;
        health_text_timer = 40;
    }
    else
    {
        show_health_text = false;
        health_text_timer = 0;
    }
    is_dirty = true;
    MainForm->redraw_needed = true;
}

void GameBars::ToggleManaText(bool force, bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    if (!show_mana_text || force)
    {
        show_mana_text = true;
        mana_text_timer = 40;
    }
    else
    {
        show_mana_text = false;
        mana_text_timer = 0;
    }
    is_dirty = true;
    MainForm->redraw_needed = true;
}

void GameBars::ToggleStaminaText(bool force, bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    if (!show_stamina_text || force)
    {
        show_stamina_text = true;
        stamina_text_timer = 40;
    }
    else
    {
        show_stamina_text = false;
        stamina_text_timer = 0;
    }
    is_dirty = true;
    MainForm->redraw_needed = true;
}

void GameBars::ToggleExperienceText(bool force, bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    if (!show_experience_text || force)
    {
        show_experience_text = true;
        experience_text_timer = 40;
    }
    else
    {
        show_experience_text = false;
        experience_text_timer = 0;
    }
    is_dirty = true;
    MainForm->redraw_needed = true;
}

// Asks the server for the quest progress at most every 20 ticks.
void GameBars::RequestQuestProgress(char play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    if (quest_request_cooldown <= 0)
    {
        MainForm->net_client->sender->RequestQuestProgress();
        quest_request_cooldown = 20;
    }
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    is_dirty = true;
    MainForm->redraw_needed = true;
}

void GameBars::OpenStats(bool play_sound)
{
    if (play_sound)
        MainForm->sfx->PlaySfx(SfxSound_HudBarClick);
    MainForm->animation->stat->Open();
    is_dirty = true;
    MainForm->redraw_needed = true;
}

// A left click on the top bar: the stats and quest buttons open their windows, and a click on a bar shows or hides its
// text.
void GameBars::MouseDown(int x, int y, int button)
{
    if (!button)
    {
        if (x >= 55 && x <= 74 && y >= 0 && y <= 10)
            OpenStats(true);
        if (x >= 75 && x <= 94 && y >= 0 && y <= 10)
            RequestQuestProgress(true);
        if (x >= 99 && x <= 213 && y >= 0 && y <= 10)
            ToggleHealthText(false, true);
        if (x >= 214 && x <= 322 && y >= 0 && y <= 10)
            ToggleManaText(false, true);
        if (x >= 325 && x <= 433 && y >= 0 && y <= 10)
            ToggleStaminaText(false, true);
        if (x >= 434 && x <= 542 && y >= 0 && y <= 10)
            ToggleExperienceText(false, true);
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
            is_dirty = true;
            MainForm->redraw_needed = true;
        }
    }
    if (show_mana_text)
    {
        mana_text_timer--;
        if (mana_text_timer <= 0)
        {
            show_mana_text = false;
            is_dirty = true;
            MainForm->redraw_needed = true;
        }
    }
    if (show_stamina_text)
    {
        stamina_text_timer--;
        if (stamina_text_timer <= 0)
        {
            show_stamina_text = false;
            is_dirty = true;
            MainForm->redraw_needed = true;
        }
    }
    if (show_experience_text)
    {
        experience_text_timer--;
        if (experience_text_timer <= 0)
        {
            show_experience_text = false;
            is_dirty = true;
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
        fraction = (double)health / max_health;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        health_segments = percent / 5;
    else
        health_segments = 0;
    if (percent > 98)
        health_segments = BAR_FULL_SEGMENTS;
    is_dirty = true;
    MainForm->redraw_needed = true;
}

// One segment for each full 5 percent of max_mana; above 98 percent the bar is full.
void GameBars::SetManaBar(int mana, int max_mana)
{
    double fraction = 0.0;
    if (max_mana > 0 && mana > 0)
        fraction = (double)mana / max_mana;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        mana_segments = percent / 5;
    else
        mana_segments = 0;
    if (percent > 98)
        mana_segments = BAR_FULL_SEGMENTS;
    is_dirty = true;
    MainForm->redraw_needed = true;
}

// One segment for each full 5 percent of max_stamina; above 98 percent the bar is full.
void GameBars::SetStaminaBar(int stamina, int max_stamina)
{
    double fraction = 0.0;
    if (max_stamina > 0 && stamina > 0)
        fraction = (double)stamina / max_stamina;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        stamina_segments = percent / 5;
    else
        stamina_segments = 0;
    if (percent > 98)
        stamina_segments = BAR_FULL_SEGMENTS;
    is_dirty = true;
    MainForm->redraw_needed = true;
}

// One segment for each full 5 percent of the way through the level; above 98 percent the bar is full.
void GameBars::SetExperienceBar(int level_progress, int level_span)
{
    double fraction = 0.0;
    if (level_span > 0 && level_progress > 0)
        fraction = (double)level_progress / level_span;
    int percent = fraction * PERCENT_SCALE;
    if (percent >= 5)
        experience_segments = percent / 5;
    else
        experience_segments = 0;
    if (percent > 98)
        experience_segments = BAR_FULL_SEGMENTS;
    is_dirty = true;
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
    is_dirty = true;
}

DEFINE_TEST_AND_CLEAR_DIRTY(GameBars)
