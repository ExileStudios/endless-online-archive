#include <vcl.h>
#pragma hdrstop

#include "Sys_soundmap.h"
#include "Tmainform.h"
#include <stdlib.h>

#pragma package(smart_init)

SoundMapper::SoundMapper()
{
}

SoundMapper::~SoundMapper()
{
}

// Plays the attack sound of the weapon at the volume: bare hands, a few weapon groups by number, or the common
// sound.
void SoundMapper::PlayWeaponSound(int weapon, int volume)
{
    bool sound_played = false;
    if (!weapon)
    {
        MainForm->sfx->PlayEffectSoundAt(SfxSound_PunchAttack, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_DarkBlade || weapon == WeaponGraphic_ReaperScyth || weapon == WeaponGraphic_Reaper ||
        weapon == WeaponGraphic_AbloStaff)
    {
        MainForm->sfx->PlayEffectSoundAt(SfxSound_HeavyWeaponAttack, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Bow || weapon == WeaponGraphic_Crossbow || weapon == WeaponGraphic_ScavBow)
    {
        MainForm->sfx->PlayEffectSoundAt(SfxSound_BowAttack, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Harp)
    {
        MainForm->sfx->PlayEffectSoundAt(random(3) + SfxSound_Harp, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Guitar)
    {
        MainForm->sfx->PlayEffectSoundAt(random(3) + SfxSound_Guitar, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Gun)
    {
        MainForm->sfx->PlayEffectSoundAt(SfxSound_GunAttack, volume);
        sound_played = true;
    }
    if (!sound_played)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_MeleeAttack, volume);
}

// Plays the sound of the spell effect, by its number, at the volume.
void SoundMapper::PlayEffectSound(int effect_id, int volume)
{
    if (!effect_id)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Flames, volume);
    if (effect_id == SpellEffect_Hearts)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Learn, volume);
    if (effect_id == SpellEffect_WarpDepart)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Warp, volume);
    if (effect_id == SpellEffect_WarpArrive)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Warp, volume);
    if (effect_id == SpellEffect_Confetti)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Confetti, volume);
    if (effect_id == SpellEffect_Sparkles)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Sparkles, volume);
    if (effect_id == SpellEffect_Pentagram)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Pentagram, volume);
    if (effect_id == SpellEffect_LargePentagram)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Pentagram, volume);
    if (effect_id == SpellEffect_Heal)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Heal, volume);
    if (effect_id == SpellEffect_SmallThunder)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Thunder, volume);
    if (effect_id == SpellEffect_UltimaBlast)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_UltimaBlast, volume);
    if (effect_id == SpellEffect_FireBall)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Flames, volume);
    if (effect_id == SpellEffect_MagicShield)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_MagicShield, volume);
    if (effect_id == SpellEffect_RingFire)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_RingOfFire, volume);
    if (effect_id == SpellEffect_IceBlast)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_IceBlast, volume);
    if (effect_id == SpellEffect_EnergyBall)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_EnergyBall, volume);
    if (effect_id == SpellEffect_Whirl)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Whirl, volume);
    if (effect_id == SpellEffect_Aura)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Aura, volume);
    if (effect_id == SpellEffect_Boulders)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Boulders, volume);
    if (effect_id == SpellEffect_Heaven)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Heaven, volume);
    if (effect_id == SpellEffect_IceBall)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_IceBall, volume);
    if (effect_id == SpellEffect_DarkBeam)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Heaven, volume);
    if (effect_id == SpellEffect_Vanish)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Vanish, volume);
    if (effect_id == SpellEffect_DarkHand)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_DarkHand, volume);
    if (effect_id == SpellEffect_DarkSkull)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_DarkHand, volume);
    if (effect_id == SpellEffect_FireBlast)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_FireBlast, volume);
    if (effect_id == SpellEffect_Tentacles)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Tentacles, volume);
    if (effect_id == SpellEffect_PowerWind)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_PowerWind, volume);
    if (effect_id == SpellEffect_MagicWhirl)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_MagicWhirl, volume);
    if (effect_id == SpellEffect_DarkBite)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_DarkHand, volume);
    if (effect_id == SpellEffect_Shell)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_Aura, volume);
    if (effect_id == SpellEffect_GreenFlame)
        MainForm->sfx->PlayEffectSoundAt(SfxSound_EnergyBall, volume);
}
