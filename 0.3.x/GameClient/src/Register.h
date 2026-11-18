#ifndef RegisterH
#define RegisterH

enum LayoutKey
{
    LayoutKey_Inventory = 1,
    LayoutKey_Spells = 2,
    LayoutKey_Skills = 3,
    LayoutKey_Macros = 4,
    LayoutKey_Hotkeys = 5
};

#include <Registry.hpp>

// Remembers where each character keeps its items, spells, skills and macros, in the registry of the current user.
struct WinRegister
{
    TRegIniFile *registry;
    String root_key;
    String character_key;
    String inventory_key;
    String spells_key;
    String skills_key;
    String macros_key;
    String hotkeys_key;
    String value_prefix;
    String separator;
    String current_key;
    String server_host;
    String server_port;
    String account_name;
    String character_name;

    WinRegister();
    ~WinRegister();
    void SetServerHost(String host);
    void SetServerPort(int port);
    void SetAccountName(String name);
    void SetCharacterName(String name);
    void BuildKeyPaths();
    String GetSlotKey(int slot_type);
    void OpenSlotKey(int slot_type);
    void CloseSlotKey();
    void WriteSlotValue(int slot_index, int value);
    int ReadSlotValue(int slot_index);
    void WriteSlotText(int slot_index, String text);
    String ReadSlotText(int slot_index);
};

#endif
