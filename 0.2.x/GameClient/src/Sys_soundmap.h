#ifndef Sys_soundmapH
#define Sys_soundmapH

// Picks the sound for a weapon swing or a spell effect.
struct SoundMapper
{
    char unused_bytes[8];

    SoundMapper();
    ~SoundMapper();
    void PlayWeaponSound(int weapon, int volume);
    void PlayEffectSound(int effect_id, int volume);
};

#endif
