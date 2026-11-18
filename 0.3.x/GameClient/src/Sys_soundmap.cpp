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
        MainForm->sfx->PlayControlSoundAt(ControlSound_PunchAttack, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_DarkBlade || weapon == WeaponGraphic_ReaperScyth || weapon == WeaponGraphic_Reaper ||
        weapon == WeaponGraphic_AbloStaff)
    {
        MainForm->sfx->PlayControlSoundAt(ControlSound_HeavyWeaponAttack, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Bow || weapon == WeaponGraphic_Crossbow || weapon == WeaponGraphic_ScavBow)
    {
        MainForm->sfx->PlayControlSoundAt(ControlSound_BowAttack, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Harp)
    {
        MainForm->sfx->PlayControlSoundAt(ControlSound_Harp, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Guitar)
    {
        MainForm->sfx->PlayControlSoundAt(ControlSound_Guitar, volume);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Gun)
    {
        MainForm->sfx->PlayControlSoundAt(ControlSound_GunAttack, volume);
        sound_played = true;
    }
    if (!sound_played)
        MainForm->sfx->PlayControlSoundAt(ControlSound_MeleeAttack, volume);
}

// Plays the sound of the spell effect, by its number, at the volume.
void SoundMapper::PlayEffectSound(int effect_id, int volume)
{
    if (!effect_id)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Flames, volume);
    if (effect_id == SpellEffect_Hearts)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Learn, volume);
    if (effect_id == SpellEffect_WarpDepart)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Warp, volume);
    if (effect_id == SpellEffect_WarpArrive)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Warp, volume);
    if (effect_id == SpellEffect_Confetti)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Confetti, volume);
    if (effect_id == SpellEffect_Sparkles)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Sparkles, volume);
    if (effect_id == SpellEffect_Pentagram)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Pentagram, volume);
    if (effect_id == SpellEffect_LargePentagram)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Pentagram, volume);
    if (effect_id == SpellEffect_Heal)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Heal, volume);
    if (effect_id == SpellEffect_SmallThunder)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Thunder, volume);
    if (effect_id == SpellEffect_UltimaBlast)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_UltimaBlast, volume);
    if (effect_id == SpellEffect_FireBall)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Flames, volume);
    if (effect_id == SpellEffect_MagicShield)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_MagicShield, volume);
    if (effect_id == SpellEffect_RingFire)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_RingOfFire, volume);
    if (effect_id == SpellEffect_IceBlast)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_IceBlast, volume);
    if (effect_id == SpellEffect_EnergyBall)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_EnergyBall, volume);
    if (effect_id == SpellEffect_Whirl)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Whirl, volume);
    if (effect_id == SpellEffect_Aura)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Aura, volume);
    if (effect_id == SpellEffect_Boulders)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Boulders, volume);
    if (effect_id == SpellEffect_Heaven)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Heaven, volume);
    if (effect_id == SpellEffect_IceBall)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_IceBall, volume);
    if (effect_id == SpellEffect_DarkBeam)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Heaven, volume);
    if (effect_id == SpellEffect_Vanish)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Vanish, volume);
    if (effect_id == SpellEffect_DarkHand)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_DarkHand, volume);
    if (effect_id == SpellEffect_DarkSkull)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_DarkHand, volume);
    if (effect_id == SpellEffect_FireBlast)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_FireBlast, volume);
    if (effect_id == SpellEffect_Tentacles)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Tentacles, volume);
    if (effect_id == SpellEffect_PowerWind)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_PowerWind, volume);
    if (effect_id == SpellEffect_MagicWhirl)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_MagicWhirl, volume);
    if (effect_id == SpellEffect_DarkBite)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_DarkHand, volume);
    if (effect_id == SpellEffect_Shell)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_Aura, volume);
    if (effect_id == SpellEffect_GreenFlame)
        MainForm->sfx->PlayEffectSoundAt(EffectSound_EnergyBall, volume);
}
