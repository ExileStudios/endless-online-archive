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

// Sets the bar's health percent and its time; a hit starts it blinking from empty.
void BossBars::ApplyHealth(int npc_index, int percent, char was_hit)
{
    BossBar *bar;
    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;
    for (int i = 0; i < 3; i++)
    {
        bar = bars[i];
        if (npc_index == bar->npc_index)
        {
            bar->health_percent = percent;
            if (was_hit)
            {
                bar->is_blinking = true;
                bar->blink_counter = 0;
                bar->health_percent = 0;
            }
            bar->shown_time = clock();
            break;
        }
    }
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
    int index;
    int slot;
    for (int i = 0; i < MainForm->npcs->GetCount(); i++)
    {
        Npc npc = MainForm->npcs->GetNpc(i);
        if (false)
            continue;
        NpcValue npc_value = MainForm->npc_values->GetNpcByIndex(npc.npc_id - 1);
        if (npc_value.npc_id < 0)
            continue;
        if (npc_value.boss < 1)
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
                    if (npc.health_bar_width <= 5)
                        bar->health_color = 2;
                    else if (npc.health_bar_width > 12)
                        bar->health_color = 0;
                    else
                        bar->health_color = 1;
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
    if (MainForm->gfx->interface_graphics->minimap_visible == 2 && MainForm->config->GetRadarPosition() == 3)
        bar_x = 25;
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
