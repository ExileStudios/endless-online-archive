#ifndef Sys_magicmapH
#define Sys_magicmapH

enum SpellEffect
{
    SpellEffect_WarpDepart = 2,
    SpellEffect_WarpArrive = 3,
    SpellEffect_Confetti = 4,
    SpellEffect_Sparkles = 5,
    SpellEffect_WaterSplash = 8,
    SpellEffect_Heal = 9,
    SpellEffect_SmallThunder = 10,
    SpellEffect_UltimaBlast = 12,
    SpellEffect_FireBall = 13,
    SpellEffect_MagicShield = 14,
    SpellEffect_RingFire = 15,
    SpellEffect_IceBlast = 16,
    SpellEffect_EnergyBall = 17,
    SpellEffect_Whirl = 18,
    SpellEffect_Aura = 19,
    SpellEffect_Boulders = 20,
    SpellEffect_Heaven = 21,
    SpellEffect_IceBall = 22,
    SpellEffect_DarkBeam = 23,
    SpellEffect_DarkHand = 25,
    SpellEffect_DarkSkull = 26,
    SpellEffect_FireBlast = 27,
    SpellEffect_Tentacles = 28,
    SpellEffect_PowerWind = 29,
    SpellEffect_MagicWhirl = 30,
    SpellEffect_DarkBite = 31,
    SpellEffect_Shell = 32,
    SpellEffect_GreenFlame = 33
};

// Per spell effect: which layers it draws, and its frame count.
struct MagicMapper
{
    char base_layer[240];
    char blend_layer[240];
    char solid_layer[240];

    MagicMapper();
    ~MagicMapper();
    int GetFrameCount(int effect_id);
};

#endif
