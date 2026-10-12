#ifndef Sys_magicmapH
#define Sys_magicmapH

// stored as the loaded value - 1 (PacketProcessor::DispatchPacket passes attack_spell_graphic - 1)
enum SpellEffect
{
    SpellEffect_SmallThunder = 10,
    SpellEffect_UltimaBlast = 12,
    SpellEffect_FireBall = 13,
    SpellEffect_RingFire = 15,
    SpellEffect_EnergyBall = 17,
    SpellEffect_Whirl = 18,
    SpellEffect_Aura = 19,
    SpellEffect_Boulders = 20,
    SpellEffect_Heaven = 21,
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

// Per spell effect: which layers it draws, and its frame and loop counts.
struct MagicMapper
{
    char base_layer[240];
    char blend_layer[240];
    char solid_layer[240];

    MagicMapper();
    ~MagicMapper();
    int GetFrameCount(int effect_id);
    int GetLoopCount(int effect_id);
};

#endif
