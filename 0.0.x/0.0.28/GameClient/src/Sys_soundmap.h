#ifndef Sys_soundmapH
#define Sys_soundmapH

// Picks the sound for a weapon swing or a spell effect.
struct SoundMapper
{
    char pad_0x0[8];

    SoundMapper();
    ~SoundMapper();
    void PlayWeaponSound(int weapon);
    void PlayEffectSound(int effect_id);
};

#endif
