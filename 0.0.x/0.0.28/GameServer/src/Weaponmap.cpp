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

char WeaponMapper::IsRangedWeapon(int doll_graphic_id)
{
    char ranged = false;
    if (doll_graphic_id == WeaponGraphic_Bow || doll_graphic_id == WeaponGraphic_Crossbow ||
        doll_graphic_id == WeaponGraphic_Harp || doll_graphic_id == WeaponGraphic_Guitar ||
        doll_graphic_id == WeaponGraphic_Gun || doll_graphic_id == WeaponGraphic_ScavBow)
        ranged = true;
    return ranged;
}
