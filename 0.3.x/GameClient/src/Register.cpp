#include <vcl.h>
#pragma hdrstop

#include "Register.h"
#include <Registry.hpp>

#pragma package(smart_init)

WinRegister::WinRegister()
{
    registry = 0;
    root_key = "Software\\EndlessOnline";
    server_port = 0;
}

WinRegister::~WinRegister()
{
}

void WinRegister::SetServerHost(String host)
{
    server_host = host;
}

void WinRegister::SetServerPort(int port)
{
    server_port = IntToStr(port);
}

void WinRegister::SetAccountName(String name)
{
    name = name;
}

void WinRegister::SetCharacterName(String name)
{
    character_name = name;
}

// Builds the registry keys of the character from the server, account and character names: its inventory, spells,
// skills, macros and hotkeys.
void WinRegister::BuildKeyPaths()
{
    separator = "\\";
    value_prefix = "item";
    server_host = LowerCase(server_host);
    account_name = LowerCase(account_name);
    character_name = LowerCase(character_name);
    character_key = root_key;
    if (server_host.Length() > 0)
    {
        String server_key = server_host + ":" + server_port;
        character_key.Insert(separator, character_key.Length() + 1);
        character_key.Insert(server_key, character_key.Length() + 1);
    }
    character_key.Insert(separator, character_key.Length() + 1);
    character_key.Insert(account_name, character_key.Length() + 1);
    character_key.Insert(separator, character_key.Length() + 1);
    character_key.Insert(character_name, character_key.Length() + 1);
    character_key.Insert(separator, character_key.Length() + 1);
    inventory_key = character_key;
    spells_key = character_key;
    skills_key = character_key;
    macros_key = character_key;
    hotkeys_key = character_key;
    inventory_key.Insert("inventory", inventory_key.Length() + 1);
    spells_key.Insert("spells", spells_key.Length() + 1);
    skills_key.Insert("skills", skills_key.Length() + 1);
    macros_key.Insert("macros", macros_key.Length() + 1);
    hotkeys_key.Insert("hotkeys", macros_key.Length() + 1);
}

// The registry key of a slot type, 1 to 5: inventory, spells, skills, macros or hotkeys.
String WinRegister::GetSlotKey(int slot_type)
{
    if (slot_type == LayoutKey_Inventory)
        return inventory_key;
    if (slot_type == LayoutKey_Spells)
        return spells_key;
    if (slot_type == LayoutKey_Skills)
        return skills_key;
    if (slot_type == LayoutKey_Macros)
        return macros_key;
    if (slot_type == LayoutKey_Hotkeys)
        return hotkeys_key;
    return "";
}

// Opens the registry key of the slot type under the current user.
void WinRegister::OpenSlotKey(int slot_type)
{
    TRegIniFile *old = registry;
    if (old)
        delete old;
    current_key = GetSlotKey(slot_type);
    registry = new TRegIniFile(current_key);
    registry->RootKey = (void *)-2147483647;
}

void WinRegister::CloseSlotKey()
{
    if (!registry)
        return;
    delete (TRegIniFile *)registry;
    registry = 0;
}

// Keeps the number for the slot in the open key.
void WinRegister::WriteSlotValue(int slot_index, int value)
{
    if (!registry)
        return;
    try
    {
        registry->WriteInteger("", value_prefix + IntToStr(slot_index), value);
    }
    catch (...)
    {
    }
}

// The number kept for the slot in the open key, or -1.
int WinRegister::ReadSlotValue(int slot_index)
{
    if (!registry)
        return -1;
    int value = -1;
    try
    {
        value = registry->ReadInteger("", value_prefix + IntToStr(slot_index), value);
    }
    catch (...)
    {
    }
    return value;
}

void Register_UnusedLiterals1()
{
    "";
    "";
}

// Keeps the text for the slot in the open key.
void WinRegister::WriteSlotText(int slot_index, String text)
{
    if (!registry)
        return;
    try
    {
        registry->WriteString("", value_prefix + IntToStr(slot_index), text);
    }
    catch (...)
    {
    }
}

// The text kept for the slot in the open key, or empty.
String WinRegister::ReadSlotText(int slot_index)
{
    if (!registry)
        return "";
    String text = "";
    try
    {
        text = registry->ReadString("", value_prefix + IntToStr(slot_index), text);
    }
    catch (...)
    {
    }
    return text;
}
