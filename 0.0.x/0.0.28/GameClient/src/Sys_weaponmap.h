#ifndef Sys_weaponmapH
#define Sys_weaponmapH

enum WeaponGraphic
{
    WeaponGraphic_DarkBlade = 39,
    WeaponGraphic_ReaperScyth = 40,
    WeaponGraphic_Bow = 42,
    WeaponGraphic_Crossbow = 43,
    WeaponGraphic_Reaper = 44,
    WeaponGraphic_Harp = 49,
    WeaponGraphic_Guitar = 50,
    WeaponGraphic_AbloStaff = 55,
    WeaponGraphic_Gun = 58,
    WeaponGraphic_ScavBow = 73
};

#define MAX_WEAPON_GRAPHICS 240

// Per weapon: the swing effect drawn with it, and whether it uses the ranged attack frames.
struct WeaponMapper
{
    int weapon_effect_types[MAX_WEAPON_GRAPHICS];

    WeaponMapper();
    ~WeaponMapper();
    bool IsRangedWeapon(int weapon);
};

#endif
