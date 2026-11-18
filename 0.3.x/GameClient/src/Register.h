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

// Remembers where each character keeps its items, spells, skills and macros, in the registry of the current user.
struct WinRegister
{
    String root_key;
    String character_key;
    String inventory_key;
    String spells_key;
    String skills_key;
    String macros_key;
    String value_prefix;
    String separator;
    String current_key;
    String account_name;
    String character_name;
    char unused_bytes[1024];
    int unused_value;

    WinRegister();
    ~WinRegister();
    void SetAccountName(String name);
    void SetCharacterName(String name);
    void BuildCharacterKeys();
    void BuildKeyPaths(String account, String character);
    void WriteSlotValue(int slot_type, int slot_index, int value);
    int ReadSlotValue(int slot_type, int slot_index);
};

#endif
