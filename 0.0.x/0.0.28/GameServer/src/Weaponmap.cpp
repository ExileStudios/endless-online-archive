#include <vcl.h>
#pragma hdrstop

#include "Weaponmap.h"

#pragma package(smart_init)

WeaponMapper::WeaponMapper()
{
}

WeaponMapper::~WeaponMapper()
{
}

char WeaponMapper::Combat_IsRangedWeapon(int doll_graphic_id)
{
    char ranged = 0;
    if (doll_graphic_id == WeaponGraphic_Bow || doll_graphic_id == WeaponGraphic_Crossbow ||
        doll_graphic_id == WeaponGraphic_Harp || doll_graphic_id == WeaponGraphic_Guitar ||
        doll_graphic_id == WeaponGraphic_Gun || doll_graphic_id == WeaponGraphic_ScavBow)
        ranged = 1;
    return ranged;
}
