#ifndef ItemvalueH
#define ItemvalueH

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

// The item types of the pub item file that the server acts on.
enum EifItemType
{
    EifItemType_Heal = 3,
    EifItemType_Teleport = 4,
    EifItemType_ExpReward = 6,
    EifItemType_Key = 9,
    EifItemType_Weapon = 10,
    EifItemType_Shield = 11,
    EifItemType_Armor = 12,
    EifItemType_Hat = 13,
    EifItemType_Boots = 14,
    EifItemType_Gloves = 15,
    EifItemType_Accessory = 16,
    EifItemType_Belt = 17,
    EifItemType_Necklace = 18,
    EifItemType_Ring = 19,
    EifItemType_Armlet = 20,
    EifItemType_Bracer = 21,
    EifItemType_Alcohol = 22,
    EifItemType_EffectPotion = 23,
    EifItemType_HairDye = 24,
    EifItemType_CureCurse = 25
};



struct ItemValue;

// An item from the pub item file: its type, bonuses, needs and specs.
struct ItemValue
{
    int id;
    short type;
    short subtype;
    short special;
    short hp;
    short tp;
    short min_damage;
    short max_damage;
    short accuracy;
    short evade;
    short armor;
    short return_damage;
    short strength;
    short intelligence;
    short wisdom;
    short agility;
    short constitution;
    short charisma;
    short light_resistance;
    short dark_resistance;
    short earth_resistance;
    short air_resistance;
    short water_resistance;
    short fire_resistance;
    int spec1;
    short spec2;
    short spec3;
    short level_requirement;
    short class_requirement;
    short strength_requirement;
    short intelligence_requirement;
    short wisdom_requirement;
    short agility_requirement;
    short constitution_requirement;
    short charisma_requirement;
    short weight;
    short size;
    short weapon_target_area;
    short element;
    short element_damage;

    ItemValue();
    ItemValue(int id);
    ~ItemValue();
};


#endif
