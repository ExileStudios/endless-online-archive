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

// Plays the swing sound of the weapon; some weapons pick one of three at random, unlisted ones play the default.
void SoundMapper::PlayWeaponSound(int weapon)
{
    bool sound_played = false;
    if (!weapon)
    {
        MainForm->sfx->PlaySfx(SfxSound_PunchAttack);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_DarkBlade || weapon == WeaponGraphic_ReaperScyth || weapon == WeaponGraphic_Reaper ||
        weapon == WeaponGraphic_AbloStaff)
    {
        MainForm->sfx->PlaySfx(SfxSound_HeavyWeaponAttack);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Bow || weapon == WeaponGraphic_Crossbow || weapon == WeaponGraphic_ScavBow)
    {
        MainForm->sfx->PlaySfx(SfxSound_BowAttack);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Harp)
    {
        MainForm->sfx->PlaySfx(random(3) + SfxSound_Harp);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Guitar)
    {
        MainForm->sfx->PlaySfx(random(3) + SfxSound_Guitar);
        sound_played = true;
    }
    if (weapon == WeaponGraphic_Gun)
    {
        MainForm->sfx->PlaySfx(SfxSound_GunAttack);
        sound_played = true;
    }
    if (!sound_played)
        MainForm->sfx->PlaySfx(SfxSound_MeleeAttack);
}

// Plays the sound of the spell effect; effects not listed play none.
void SoundMapper::PlayEffectSound(int effect_id)
{
    if (!effect_id)
        MainForm->sfx->PlaySfx(SfxSound_Flames);
    if (effect_id == SpellEffect_Hearts)
        MainForm->sfx->PlaySfx(SfxSound_Learn);
    if (effect_id == SpellEffect_WarpDepart)
        MainForm->sfx->PlaySfx(SfxSound_Warp);
    if (effect_id == SpellEffect_WarpArrive)
        MainForm->sfx->PlaySfx(SfxSound_Warp);
    if (effect_id == SpellEffect_Confetti)
        MainForm->sfx->PlaySfx(SfxSound_Confetti);
    if (effect_id == SpellEffect_Sparkles)
        MainForm->sfx->PlaySfx(SfxSound_Sparkles);
    if (effect_id == SpellEffect_Pentagram)
        MainForm->sfx->PlaySfx(SfxSound_Pentagram);
    if (effect_id == SpellEffect_LargePentagram)
        MainForm->sfx->PlaySfx(SfxSound_Pentagram);
    if (effect_id == SpellEffect_Heal)
        MainForm->sfx->PlaySfx(SfxSound_Heal);
    if (effect_id == SpellEffect_SmallThunder)
        MainForm->sfx->PlaySfx(SfxSound_Thunder);
    if (effect_id == SpellEffect_UltimaBlast)
        MainForm->sfx->PlaySfx(SfxSound_UltimaBlast);
    if (effect_id == SpellEffect_FireBall)
        MainForm->sfx->PlaySfx(SfxSound_Flames);
    if (effect_id == SpellEffect_MagicShield)
        MainForm->sfx->PlaySfx(SfxSound_MagicShield);
    if (effect_id == SpellEffect_RingFire)
        MainForm->sfx->PlaySfx(SfxSound_RingOfFire);
    if (effect_id == SpellEffect_IceBlast)
        MainForm->sfx->PlaySfx(SfxSound_IceBlast);
    if (effect_id == SpellEffect_EnergyBall)
        MainForm->sfx->PlaySfx(SfxSound_EnergyBall);
    if (effect_id == SpellEffect_Whirl)
        MainForm->sfx->PlaySfx(SfxSound_Whirl);
    if (effect_id == SpellEffect_Aura)
        MainForm->sfx->PlaySfx(SfxSound_Aura);
    if (effect_id == SpellEffect_Boulders)
        MainForm->sfx->PlaySfx(SfxSound_Boulders);
    if (effect_id == SpellEffect_Heaven)
        MainForm->sfx->PlaySfx(SfxSound_Heaven);
    if (effect_id == SpellEffect_IceBall)
        MainForm->sfx->PlaySfx(SfxSound_IceBall);
    if (effect_id == SpellEffect_DarkBeam)
        MainForm->sfx->PlaySfx(SfxSound_Heaven);
    if (effect_id == SpellEffect_Vanish)
        MainForm->sfx->PlaySfx(SfxSound_Vanish);
    if (effect_id == SpellEffect_DarkHand)
        MainForm->sfx->PlaySfx(SfxSound_DarkHand);
    if (effect_id == SpellEffect_DarkSkull)
        MainForm->sfx->PlaySfx(SfxSound_DarkHand);
    if (effect_id == SpellEffect_FireBlast)
        MainForm->sfx->PlaySfx(SfxSound_FireBlast);
    if (effect_id == SpellEffect_Tentacles)
        MainForm->sfx->PlaySfx(SfxSound_Tentacles);
    if (effect_id == SpellEffect_PowerWind)
        MainForm->sfx->PlaySfx(SfxSound_PowerWind);
    if (effect_id == SpellEffect_MagicWhirl)
        MainForm->sfx->PlaySfx(SfxSound_MagicWhirl);
    if (effect_id == SpellEffect_DarkBite)
        MainForm->sfx->PlaySfx(SfxSound_DarkHand);
    if (effect_id == SpellEffect_Shell)
        MainForm->sfx->PlaySfx(SfxSound_Aura);
    if (effect_id == SpellEffect_GreenFlame)
        MainForm->sfx->PlaySfx(SfxSound_EnergyBall);
}
