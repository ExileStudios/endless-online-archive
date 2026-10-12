#include <vcl.h>
#pragma hdrstop

#include "Sys_effects.h"
#include "Tmainform.h"
#pragma package(smart_init)

Effects::Effects()
{
    current_index = -1;
}

Effects::~Effects()
{
    Clear();
}

// Steps every effect's frame, playing its sound on the first step; an effect ends after its last loop.
void Effects::Animate()
{
    if (effects.size() > 0)
    {
        MainForm->redraw_needed = true;
        for (vector<MapEffect>::iterator it = effects.begin(); it != effects.end(); it++)
        {
            if (it->finished)
                continue;
            if (it->sound_pending)
            {
                MainForm->sound_mapper->PlayEffectSound(it->effect_id);
                it->sound_pending = false;
            }
            if (it->frame <= it->last_frame)
                it->frame++;
            else
            {
                it->loops_left--;
                it->frame = 0;
                if (it->loops_left <= 0)
                    it->finished = true;
            }
        }
    }
}

// Drops the effects that have ended.
void Effects::RemoveFinished()
{
    int removed_one;
    do
    {
        removed_one = 0;
        for (vector<MapEffect>::iterator it = effects.begin(); it != effects.end(); it++)
        {
            if (it->finished)
            {
                effects.erase(it);
                removed_one = 1;
                break;
            }
        }
    } while (removed_one);
}

// Adds an effect on the tile, then bubble-sorts the list by position.
void Effects::AddEffect(int x, int y, int effect_id)
{
    MapEffect new_effect(x, y, effect_id);
    effects.insert(effects.end(), new_effect);
    char swapped;
    do
    {
        swapped = 0;
        for (int i = 0; i < GetCount() - 1; i++)
        {
            MapEffect handle;
            if (effects[i].y == effects[i + 1].y && effects[i].x > effects[i + 1].x)
            {
                handle = effects[i];
                effects[i] = effects[i + 1];
                effects[i + 1] = handle;
                swapped = 1;
            }
            if (effects[i].y > effects[i + 1].y)
            {
                handle = effects[i];
                effects[i] = effects[i + 1];
                effects[i + 1] = handle;
                swapped = 1;
            }
        }
    } while (swapped);
}

void Effects::Clear()
{
    current_index = -1;
    effects.clear();
}

MapEffect Effects::GetEffect(int index)
{
    return effects[index];
}

int Effects::GetCount()
{
    return effects.size();
}
