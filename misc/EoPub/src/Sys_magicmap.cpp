#include <vcl.h>
#pragma hdrstop

#include "Sys_magicmap.h"

#pragma package(smart_init)

// Fills the layer table of every effect: whether it draws a blended and a solid layer.
MagicMapper::MagicMapper()
{
    base_layer[0] = true;
    blend_layer[0] = true;
    solid_layer[0] = true;
    base_layer[1] = false;
    blend_layer[1] = false;
    solid_layer[1] = true;
    base_layer[2] = false;
    blend_layer[2] = true;
    solid_layer[2] = true;
    base_layer[3] = false;
    blend_layer[3] = false;
    solid_layer[3] = true;
    base_layer[4] = false;
    blend_layer[4] = false;
    solid_layer[4] = true;
    base_layer[5] = false;
    blend_layer[5] = true;
    solid_layer[5] = true;
    base_layer[6] = true;
    blend_layer[6] = false;
    solid_layer[6] = false;
    base_layer[7] = true;
    blend_layer[7] = false;
    solid_layer[7] = false;
    base_layer[8] = true;
    blend_layer[8] = false;
    solid_layer[8] = false;
    base_layer[9] = false;
    blend_layer[9] = true;
    solid_layer[9] = true;
    base_layer[10] = false;
    blend_layer[10] = false;
    solid_layer[10] = true;
    base_layer[11] = false;
    blend_layer[11] = false;
    solid_layer[11] = true;
    base_layer[12] = true;
    blend_layer[12] = true;
    solid_layer[12] = false;
    base_layer[13] = true;
    blend_layer[13] = true;
    solid_layer[13] = false;
    base_layer[14] = false;
    blend_layer[14] = true;
    solid_layer[14] = true;
    base_layer[15] = true;
    blend_layer[15] = false;
    solid_layer[15] = true;
    base_layer[16] = false;
    blend_layer[16] = true;
    solid_layer[16] = true;
    base_layer[17] = false;
    blend_layer[17] = false;
    solid_layer[17] = true;
    base_layer[18] = true;
    blend_layer[18] = true;
    solid_layer[18] = true;
    base_layer[19] = false;
    blend_layer[19] = true;
    solid_layer[19] = false;
    base_layer[20] = false;
    blend_layer[20] = false;
    solid_layer[20] = true;
    base_layer[21] = true;
    blend_layer[21] = true;
    solid_layer[21] = false;
    base_layer[22] = true;
    blend_layer[22] = true;
    solid_layer[22] = false;
    base_layer[23] = true;
    blend_layer[23] = true;
    solid_layer[23] = false;
    base_layer[24] = false;
    blend_layer[24] = false;
    solid_layer[24] = true;
    base_layer[25] = true;
    blend_layer[25] = true;
    solid_layer[25] = false;
    base_layer[26] = true;
    blend_layer[26] = true;
    solid_layer[26] = false;
    base_layer[27] = false;
    blend_layer[27] = false;
    solid_layer[27] = true;
    base_layer[28] = false;
    blend_layer[28] = false;
    solid_layer[28] = true;
    base_layer[29] = true;
    blend_layer[29] = false;
    solid_layer[29] = true;
    base_layer[30] = true;
    blend_layer[30] = false;
    solid_layer[30] = true;
    base_layer[31] = true;
    blend_layer[31] = true;
    solid_layer[31] = false;
    base_layer[32] = true;
    blend_layer[32] = true;
    solid_layer[32] = false;
    base_layer[33] = true;
    blend_layer[33] = true;
    solid_layer[33] = false;
}

MagicMapper::~MagicMapper()
{
}

// The number of frames in the effect's animation; 4 unless the effect has its own.
int MagicMapper::GetFrameCount(int effect_id)
{
    // Most effects play in four frames; the others below name their own count.
    int frame_count = 4;
    if (effect_id == SpellEffect_WarpDepart)
        frame_count = 8;
    if (effect_id == SpellEffect_WarpArrive)
        frame_count = 8;
    if (effect_id == SpellEffect_Confetti)
        frame_count = 7;
    if (effect_id == SpellEffect_Sparkles)
        frame_count = 5;
    if (effect_id == SpellEffect_WaterSplash)
        frame_count = 6;
    if (effect_id == SpellEffect_Heal)
        frame_count = 5;
    if (effect_id == SpellEffect_FireBall)
        frame_count = 6;
    if (effect_id == SpellEffect_MagicShield)
        frame_count = 6;
    if (effect_id == SpellEffect_IceBlast)
        frame_count = 7;
    if (effect_id == SpellEffect_EnergyBall)
        frame_count = 7;
    if (effect_id == SpellEffect_Aura)
        frame_count = 5;
    if (effect_id == SpellEffect_Boulders)
        frame_count = 7;
    if (effect_id == SpellEffect_Heaven)
        frame_count = 5;
    if (effect_id == SpellEffect_IceBall)
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
