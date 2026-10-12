#ifndef WeaponmapH
#define WeaponmapH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

enum WeaponGraphic
{
    WeaponGraphic_Bow = 42,
    WeaponGraphic_Crossbow = 43,
    WeaponGraphic_Harp = 49,
    WeaponGraphic_Guitar = 50,
    WeaponGraphic_Gun = 58,
    WeaponGraphic_ScavBow = 73
};



struct WeaponMapper;

// Tells the ranged weapons by their graphic.
struct WeaponMapper
{
    char pad_0x0[8];

    WeaponMapper();
    ~WeaponMapper();
    char Combat_IsRangedWeapon(int doll_graphic_id);
};

#endif
