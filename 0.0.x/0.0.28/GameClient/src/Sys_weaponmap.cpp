#include <vcl.h>
#pragma hdrstop

#include "Sys_weaponmap.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

// Fills the swing effect group of each weapon; weapons not listed use group 0.
WeaponMapper::WeaponMapper()
{
    for (int i = 0; i < MAX_WEAPON_GRAPHICS; i++)
        weapon_effect_types[i] = 0;
    weapon_effect_types[24] = 1;
    weapon_effect_types[35] = 1;
    weapon_effect_types[41] = 1;
    weapon_effect_types[47] = 1;
    weapon_effect_types[52] = 1;
    weapon_effect_types[53] = 1;
    weapon_effect_types[54] = 1;
    weapon_effect_types[56] = 1;
    weapon_effect_types[66] = 1;
    weapon_effect_types[67] = 2;
    weapon_effect_types[3] = 2;
    weapon_effect_types[5] = 2;
    weapon_effect_types[21] = 2;
    weapon_effect_types[14] = 2;
    weapon_effect_types[19] = 2;
    weapon_effect_types[12] = 2;
    weapon_effect_types[18] = 2;
    weapon_effect_types[27] = 2;
    weapon_effect_types[28] = 2;
    weapon_effect_types[30] = 2;
    weapon_effect_types[31] = 2;
    weapon_effect_types[32] = 2;
    weapon_effect_types[57] = 2;
    weapon_effect_types[1] = 3;
    weapon_effect_types[9] = 3;
    weapon_effect_types[65] = 3;
    weapon_effect_types[67] = 3;
    weapon_effect_types[36] = 4;
    weapon_effect_types[37] = 4;
    weapon_effect_types[33] = 5;
    weapon_effect_types[34] = 5;
    weapon_effect_types[38] = 5;
    weapon_effect_types[46] = 5;
    weapon_effect_types[51] = 5;
    weapon_effect_types[39] = 7;
    weapon_effect_types[40] = 7;
    weapon_effect_types[55] = 7;
    weapon_effect_types[44] = 8;
}

WeaponMapper::~WeaponMapper()
{
}

// True for the weapons drawn with the ranged attack frames.
bool WeaponMapper::IsRangedWeapon(int weapon)
{
    bool ranged = 0;
    if (weapon == WeaponGraphic_Bow || weapon == WeaponGraphic_Crossbow || weapon == WeaponGraphic_Harp ||
        weapon == WeaponGraphic_Guitar || weapon == WeaponGraphic_Gun || weapon == WeaponGraphic_ScavBow)
        ranged = 1;
    return ranged;
}
