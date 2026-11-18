#include <vcl.h>
#pragma hdrstop

#include "Sys_effect.h"
#include "Tmainform.h"
#pragma package(smart_init)

MapEffect::MapEffect()
{
}

MapEffect::MapEffect(int tile_x, int tile_y, int effect_number)
{
    sound_pending = true;
    finished = false;
    x = tile_x;
    y = tile_y;
    effect_id = effect_number;
    frame = 0;
    has_base_layer = MainForm->magic_mapper->base_layer[effect_number];
    has_blend_layer = MainForm->magic_mapper->blend_layer[effect_number];
    has_solid_layer = MainForm->magic_mapper->solid_layer[effect_number];
    last_frame = MainForm->magic_mapper->GetFrameCount(effect_number) - 2;
    loops_left = MainForm->magic_mapper->GetLoopCount(effect_number);
}

MapEffect::~MapEffect()
{
}
