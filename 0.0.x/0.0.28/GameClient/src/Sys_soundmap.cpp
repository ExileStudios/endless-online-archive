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
    bool sound_played = 0;
    if (!weapon)
    {
        MainForm->sfx->PlaySfx(10);
        sound_played = 1;
    }
    if (weapon == WeaponGraphic_DarkBlade || weapon == WeaponGraphic_ReaperScyth || weapon == WeaponGraphic_Reaper ||
        weapon == WeaponGraphic_AbloStaff)
    {
        MainForm->sfx->PlaySfx(30);
        sound_played = 1;
    }
    if (weapon == WeaponGraphic_Bow || weapon == WeaponGraphic_Crossbow || weapon == WeaponGraphic_ScavBow)
    {
        MainForm->sfx->PlaySfx(38);
        sound_played = 1;
    }
    if (weapon == WeaponGraphic_Harp)
    {
        MainForm->sfx->PlaySfx(random(3) + 44);
        sound_played = 1;
    }
    if (weapon == WeaponGraphic_Guitar)
    {
        MainForm->sfx->PlaySfx(random(3) + 47);
        sound_played = 1;
    }
    if (weapon == WeaponGraphic_Gun)
    {
        MainForm->sfx->PlaySfx(53);
        sound_played = 1;
    }
    if (!sound_played)
        MainForm->sfx->PlaySfx(15);
}

// Plays the sound of the spell effect; effects not listed play none.
void SoundMapper::PlayEffectSound(int effect_id)
{
    if (!effect_id)
        MainForm->sfx->PlaySfx(31);
    if (effect_id == 1)
        MainForm->sfx->PlaySfx(37);
    if (effect_id == 2)
        MainForm->sfx->PlaySfx(32);
    if (effect_id == 3)
        MainForm->sfx->PlaySfx(32);
    if (effect_id == 4)
        MainForm->sfx->PlaySfx(35);
    if (effect_id == 5)
        MainForm->sfx->PlaySfx(36);
    if (effect_id == 6)
        MainForm->sfx->PlaySfx(34);
    if (effect_id == 7)
        MainForm->sfx->PlaySfx(34);
    if (effect_id == 9)
        MainForm->sfx->PlaySfx(43);
    if (effect_id == SpellEffect_SmallThunder)
        MainForm->sfx->PlaySfx(50);
    if (effect_id == SpellEffect_UltimaBlast)
        MainForm->sfx->PlaySfx(54);
    if (effect_id == SpellEffect_FireBall)
        MainForm->sfx->PlaySfx(31);
    if (effect_id == 14)
        MainForm->sfx->PlaySfx(55);
    if (effect_id == SpellEffect_RingFire)
        MainForm->sfx->PlaySfx(56);
    if (effect_id == 16)
        MainForm->sfx->PlaySfx(57);
    if (effect_id == SpellEffect_EnergyBall)
        MainForm->sfx->PlaySfx(58);
    if (effect_id == SpellEffect_Whirl)
        MainForm->sfx->PlaySfx(59);
    if (effect_id == SpellEffect_Aura)
        MainForm->sfx->PlaySfx(61);
    if (effect_id == SpellEffect_Boulders)
        MainForm->sfx->PlaySfx(60);
    if (effect_id == SpellEffect_Heaven)
        MainForm->sfx->PlaySfx(62);
    if (effect_id == 22)
        MainForm->sfx->PlaySfx(63);
    if (effect_id == SpellEffect_DarkBeam)
        MainForm->sfx->PlaySfx(62);
    if (effect_id == 24)
        MainForm->sfx->PlaySfx(66);
    if (effect_id == SpellEffect_DarkHand)
        MainForm->sfx->PlaySfx(76);
    if (effect_id == SpellEffect_DarkSkull)
        MainForm->sfx->PlaySfx(76);
    if (effect_id == SpellEffect_FireBlast)
        MainForm->sfx->PlaySfx(80);
    if (effect_id == SpellEffect_Tentacles)
        MainForm->sfx->PlaySfx(77);
    if (effect_id == SpellEffect_PowerWind)
        MainForm->sfx->PlaySfx(79);
    if (effect_id == SpellEffect_MagicWhirl)
        MainForm->sfx->PlaySfx(78);
    if (effect_id == SpellEffect_DarkBite)
        MainForm->sfx->PlaySfx(76);
    if (effect_id == SpellEffect_Shell)
        MainForm->sfx->PlaySfx(61);
    if (effect_id == SpellEffect_GreenFlame)
        MainForm->sfx->PlaySfx(58);
}
