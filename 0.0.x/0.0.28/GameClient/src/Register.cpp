#include <vcl.h>
#pragma hdrstop

#include "Register.h"
#include <Registry.hpp>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

WinRegister::WinRegister()
{
    root_key = "Software\\EndlessOnline";
}

WinRegister::~WinRegister()
{
}

void WinRegister::SetAccountName(String name)
{
    account_name = LowerCase(name);
}

void WinRegister::SetCharacterName(String name)
{
    character_name = LowerCase(name);
}

void WinRegister::BuildCharacterKeys()
{
    BuildKeyPaths(account_name, character_name);
}

// The keys Software\EndlessOnline\<account>\<character>\ with inventory, spells, skills and macros after it.
void WinRegister::BuildKeyPaths(String account, String character)
{
    separator = "\\";
    value_prefix = "item";
    account_name = LowerCase(account);
    character_name = LowerCase(character);
    unread_int_at_1068 = 0;
    character_key = root_key;
    character_key.Insert(separator, character_key.Length() + 1);
    character_key.Insert(account_name, character_key.Length() + 1);
    character_key.Insert(separator, character_key.Length() + 1);
    character_key.Insert(character_name, character_key.Length() + 1);
    character_key.Insert(separator, character_key.Length() + 1);
    inventory_key = character_key;
    spells_key = character_key;
    skills_key = character_key;
    macros_key = character_key;
    inventory_key.Insert("inventory", inventory_key.Length() + 1);
    spells_key.Insert("spells", spells_key.Length() + 1);
    skills_key.Insert("skills", skills_key.Length() + 1);
    macros_key.Insert("macros", macros_key.Length() + 1);
}

// Writes one slot as "item" and its index, under the key of its kind:
// 1 = inventory, 2 = spells, 3 = skills, 4 = macros
void WinRegister::WriteSlotValue(int slot_type, int slot_index, int value)
{
    if (!slot_type)
        current_key = "";
    if (slot_type == 1)
        current_key = inventory_key;
    if (slot_type == 2)
        current_key = spells_key;
    if (slot_type == 3)
        current_key = skills_key;
    if (slot_type == 4)
        current_key = macros_key;
    TRegIniFile *registry = new TRegIniFile(current_key);
    registry->RootKey = HKEY_CURRENT_USER;
    try
    {
        registry->WriteInteger("", value_prefix + IntToStr(slot_index), value);
    }
    __finally
    {
        delete registry;
    }
}

// Reads one slot the same way; -1 when it is not there.
int WinRegister::ReadSlotValue(int slot_type, int slot_index)
{
    int Integer = -1;
    if (!slot_type)
        current_key = "";
    if (slot_type == 1)
        current_key = inventory_key;
    if (slot_type == 2)
        current_key = spells_key;
    if (slot_type == 3)
        current_key = skills_key;
    if (slot_type == 4)
        current_key = macros_key;
    TRegIniFile *registry = new TRegIniFile(current_key);
    registry->RootKey = HKEY_CURRENT_USER;
    try
    {
        Integer = registry->ReadInteger("", value_prefix + IntToStr(slot_index), Integer);
    }
    __finally
    {
        delete registry;
    }
    return Integer;
}

void Register_UnusedLiterals1()
{
    "";
    "";
    "";
    "";
    "";
    "";
    "";
    "";
}

