#include <vcl.h>
#pragma hdrstop

#include "Ctrl_bossbars.h"
#include "Tmainform.h"
#include "Ctrl_bossbar.h"
#include "Gfx_interface.h"
#include <time.h>

#pragma package(smart_init)

BossBars::BossBars(int left, int top)
{
    bars[0] = new BossBar();
    bars[1] = new BossBar();
    bars[2] = new BossBar();
    x = left;
    y = top;
}

BossBars::~BossBars()
{
    delete bars[0];
    delete bars[1];
    delete bars[2];
}

void BossBars::HideAll()
{
    for (int i = 0; i < 3; i++)
        bars[i]->Hide();
}

void BossBars::RenderNames()
{
    for (int i = 0; i < 3; i++)
        bars[i]->RenderName();
}

// Finds the bosses in view, then steps each of the three bars.
void BossBars::Update()
{
    BossBar *bar;
    ScanNpcs();
    for (int i = 0; i < 3; i++)
    {
        bar = bars[i];
        bar->Update();
    }
}

// Sets the health percent of a boss's bar, kept to 0 to 100, giving the npc a free bar when it has none.
void BossBars::ShowHealth(int npc_index, int npc_id, int percent, char was_hit)
{
    BossBar *bar;
    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;
    for (int i = 0; i < 3; i++)
    {
        bar = bars[i];
        if (bar->npc_index == npc_index)
        {
            ApplyHealth(bar, percent, was_hit);
            bar->has_health = true;
            return;
        }
    }
    unsigned char slot = FindFreeBar();
    bars[slot]->Show(npc_index, npc_id - 1);
    ApplyHealth(bars[slot], percent, was_hit);
}

// Sets the bar's health percent and its time; a hit starts it blinking from empty.
void BossBars::ApplyHealth(BossBar *bar, int percent, char was_hit)
{
    bar->health_percent = percent;
    if (was_hit)
    {
        bar->is_blinking = true;
        bar->blink_counter = 0;
        bar->health_percent = 0;
    }
    bar->shown_time = clock();
}

// The first bar no npc holds, or the last one when all are taken.
int BossBars::FindFreeBar()
{
    for (int i = 0; i < 3; i++)
    {
        if (bars[i]->npc_index < 0)
            return i;
    }
    return 2;
}

int BossBars::FindBar(int npc_index)
{
    BossBar *bar;
    for (int i = 0; i < 3; i++)
    {
        bar = bars[i];
        if (npc_index == bar->npc_index)
            return i;
    }
    return -1;
}

// Gives every boss npc on the map a health bar, finding its bar or a free one, and keeps each bar's health up to date.
int BossBars::ScanNpcs()
{
    BossBar *bar;
    NpcValue *npc_record;
    int index;
    int slot;
    for (int i = 0; i < MainForm->npcs->GetCount(); i++)
    {
        Npc npc = MainForm->npcs->GetNpc(i);
        if (false)
            continue;
        npc_record = MainForm->npc_values->GetNpc(npc.npc_id - 1);
        if (npc_record->npc_id < 0)
            continue;
        if (npc_record->boss < 1)
            continue;
        index = FindBar(npc.bar_index);
        if (index < 0)
        {
            slot = FindFreeBar();
            bars[slot]->Show(npc.bar_index, npc.npc_id - 1);
        }
        else
        {
            bar = bars[index];
            if (!bar->is_blinking && npc.npc_id - 1 == bar->npc_id)
            {
                if (!bar->has_health && npc.health_bar_width <= 0)
                    bar->health_percent = 0;
                if (npc.health_bar_width > 0)
                {
                    bar->health_percent = npc.health_bar_width * 2.8F;
                    if (bar->health_percent > 100)
                        bar->health_percent = 100;
                    bar->has_health = true;
                }
                if (npc.killed_by_hit)
                {
                    bar->is_blinking = true;
                    bar->blink_counter = 0;
                    bar->health_percent = 0;
                }
                bar->shown_time = clock();
                continue;
            }
        }
    }
    return 0;
}

bool BossBars::IsAnyVisible()
{
    for (int i = 0; i < 3; i++)
    {
        if (bars[i]->IsVisible())
            return true;
    }
    return false;
}

// Draws each shown bar, one under the other.
void BossBars::Draw()
{
    int bar_x = x;
    int offset_y = 0;
    for (int i = 0; i < 3; i++)
    {
        if ((char)bars[i]->is_visible && bars[i]->npc_index >= 0)
        {
            bars[i]->Draw(bar_x, y, offset_y);
            offset_y += 25;
        }
    }
}
