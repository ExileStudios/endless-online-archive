#include <vcl.h>
#pragma hdrstop

#include "Sys_magicmap.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

MagicMapper::MagicMapper()
{
    base_layer[0] = 1; blend_layer[0] = 1; solid_layer[0] = 1;
    base_layer[1] = 0; blend_layer[1] = 0; solid_layer[1] = 1;
    base_layer[2] = 0; blend_layer[2] = 1; solid_layer[2] = 1;
    base_layer[3] = 0; blend_layer[3] = 0; solid_layer[3] = 1;
    base_layer[4] = 0; blend_layer[4] = 0; solid_layer[4] = 1;
    base_layer[5] = 0; blend_layer[5] = 1; solid_layer[5] = 1;
    base_layer[6] = 1; blend_layer[6] = 0; solid_layer[6] = 0;
    base_layer[7] = 1; blend_layer[7] = 0; solid_layer[7] = 0;
    base_layer[8] = 1; blend_layer[8] = 0; solid_layer[8] = 0;
    base_layer[9] = 0; blend_layer[9] = 1; solid_layer[9] = 1;
    base_layer[10] = 0; blend_layer[10] = 0; solid_layer[10] = 1;
    base_layer[11] = 0; blend_layer[11] = 0; solid_layer[11] = 1;
    base_layer[12] = 1; blend_layer[12] = 1; solid_layer[12] = 0;
    base_layer[13] = 1; blend_layer[13] = 1; solid_layer[13] = 0;
    base_layer[14] = 0; blend_layer[14] = 1; solid_layer[14] = 1;
    base_layer[15] = 1; blend_layer[15] = 0; solid_layer[15] = 1;
    base_layer[16] = 0; blend_layer[16] = 1; solid_layer[16] = 1;
    base_layer[17] = 0; blend_layer[17] = 0; solid_layer[17] = 1;
    base_layer[18] = 1; blend_layer[18] = 1; solid_layer[18] = 1;
    base_layer[19] = 0; blend_layer[19] = 1; solid_layer[19] = 0;
    base_layer[20] = 0; blend_layer[20] = 0; solid_layer[20] = 1;
    base_layer[21] = 1; blend_layer[21] = 1; solid_layer[21] = 0;
    base_layer[22] = 1; blend_layer[22] = 1; solid_layer[22] = 0;
    base_layer[23] = 1; blend_layer[23] = 1; solid_layer[23] = 0;
    base_layer[24] = 0; blend_layer[24] = 0; solid_layer[24] = 1;
    base_layer[25] = 1; blend_layer[25] = 1; solid_layer[25] = 0;
    base_layer[26] = 1; blend_layer[26] = 1; solid_layer[26] = 0;
    base_layer[27] = 0; blend_layer[27] = 0; solid_layer[27] = 1;
    base_layer[28] = 0; blend_layer[28] = 0; solid_layer[28] = 1;
    base_layer[29] = 1; blend_layer[29] = 0; solid_layer[29] = 1;
    base_layer[30] = 1; blend_layer[30] = 0; solid_layer[30] = 1;
    base_layer[31] = 1; blend_layer[31] = 1; solid_layer[31] = 0;
    base_layer[32] = 1; blend_layer[32] = 1; solid_layer[32] = 0;
    base_layer[33] = 1; blend_layer[33] = 1; solid_layer[33] = 0;
}

MagicMapper::~MagicMapper()
{
}

// The number of frames of the effect.
int MagicMapper::GetFrameCount(int effect_id)
{
    int frame_count = 4;
    if (effect_id == 2)
        frame_count = 8;
    if (effect_id == 3)
        frame_count = 8;
    if (effect_id == 4)
        frame_count = 7;
    if (effect_id == 5)
        frame_count = 5;
    if (effect_id == 8)
        frame_count = 6;
    if (effect_id == 9)
        frame_count = 5;
    if (effect_id == SpellEffect_FireBall)
        frame_count = 6;
    if (effect_id == 14)
        frame_count = 6;
    if (effect_id == 16)
        frame_count = 7;
    if (effect_id == SpellEffect_EnergyBall)
        frame_count = 7;
    if (effect_id == SpellEffect_Aura)
        frame_count = 5;
    if (effect_id == SpellEffect_Boulders)
        frame_count = 7;
    if (effect_id == SpellEffect_Heaven)
        frame_count = 5;
    if (effect_id == 22)
        frame_count = 6;
    if (effect_id == SpellEffect_DarkBeam)
        frame_count = 5;
    if (effect_id == SpellEffect_DarkHand)
        frame_count = 5;
    if (effect_id == SpellEffect_DarkSkull)
        frame_count = 5;
    if (effect_id == SpellEffect_Tentacles)
        frame_count = 5;
    if (effect_id == SpellEffect_PowerWind)
        frame_count = 6;
    if (effect_id == SpellEffect_MagicWhirl)
        frame_count = 15;
    if (effect_id == SpellEffect_DarkBite)
        frame_count = 6;
    if (effect_id == SpellEffect_Shell)
        frame_count = 4;
    if (effect_id == SpellEffect_GreenFlame)
        frame_count = 5;
    return frame_count;
}

// How many times the effect plays through.
int MagicMapper::GetLoopCount(int effect_id)
{
    int loop_count = 2;
    if (effect_id == 1)
        loop_count = 4;
    if (effect_id == 2)
        loop_count = 1;
    if (effect_id == 3)
        loop_count = 1;
    if (effect_id == 5)
        loop_count = 1;
    if (effect_id == 6)
        loop_count = 4;
    if (effect_id == 7)
        loop_count = 4;
    if (effect_id == 8)
        loop_count = 1;
    if (effect_id == 9)
        loop_count = 1;
    if (effect_id == 11)
        loop_count = 8;
    if (effect_id == SpellEffect_UltimaBlast)
        loop_count = 3;
    if (effect_id == SpellEffect_FireBall)
        loop_count = 1;
    if (effect_id == 14)
        loop_count = 1;
    if (effect_id == SpellEffect_RingFire)
        loop_count = 3;
    if (effect_id == 16)
        loop_count = 1;
    if (effect_id == SpellEffect_EnergyBall)
        loop_count = 1;
    if (effect_id == SpellEffect_Aura)
        loop_count = 3;
    if (effect_id == SpellEffect_Boulders)
        loop_count = 1;
    if (effect_id == SpellEffect_Heaven)
        loop_count = 4;
    if (effect_id == 22)
        loop_count = 1;
    if (effect_id == SpellEffect_DarkBeam)
        loop_count = 4;
    if (effect_id == 24)
        loop_count = 2;
    if (effect_id == SpellEffect_FireBlast)
        loop_count = 1;
    if (effect_id == SpellEffect_PowerWind)
        loop_count = 2;
    if (effect_id == SpellEffect_MagicWhirl)
        loop_count = 1;
    if (effect_id == SpellEffect_Shell)
        loop_count = 4;
    if (effect_id == SpellEffect_GreenFlame)
        loop_count = 1;
    return loop_count;
}
