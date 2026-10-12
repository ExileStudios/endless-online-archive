#ifndef Sys_playersH
#define Sys_playersH

#include <vector.h>

enum PlayerId
{
    PlayerId_None = -1,
    PlayerId_Unset = 0,
    PlayerId_LocalLimit = 90000,
    PlayerId_SlotFirst = 999991,
    PlayerId_SlotSecond = 999992,
    PlayerId_SlotThird = 999993,
    PlayerId_Preview = 999999
};

#include "Sys_player.h"

// The players on the map (and the preview and character-slot players), with their animations and frame cells.
struct Players
{
    char hover_label_shown;
    char list_changed;
    char pad_0x2[6];
    vector<Player> players;
    int current_index;
    bool gender_one_libraries_loaded;
    int gender_one_idle_ticks;
    bool gender_zero_libraries_loaded;
    int gender_zero_idle_ticks;
    int mouse_x;
    int mouse_y;
    HMODULE hair_library_gender_one;
    HMODULE hair_library_gender_zero;
    HMODULE boots_library_gender_one;
    HMODULE boots_library_gender_zero;
    HMODULE armor_library_gender_one;
    HMODULE armor_library_gender_zero;
    HMODULE hat_library_gender_one;
    HMODULE hat_library_gender_zero;
    HMODULE weapon_library_gender_one;
    HMODULE weapon_library_gender_zero;
    HMODULE shield_library_gender_one;
    HMODULE shield_library_gender_zero;
    char preload_graphics;

    Players(char preload_option);
    ~Players();
    void ReloadGraphicsLibraries();
    void Animate();
    void RemoveExpiredPlayers(int center_x, int center_y);
    void LoadGraphicsLibraries(int gender);
    void LoadPendingGraphics();
    void HideTextLabel(int player_id);
    void ShowTextLabel(int player_id, String text, int label_style, char blinking, int duration);
    void ReloadGraphics(int player_id);
    bool IsInViewRange(int center_x, int center_y, int x, int y);
    void SortByPosition();
    void AddPreviewPlayer(int x, int y);
    void AddCharacterSlots();
    void RemovePreviewPlayer();
    void SetPreviewPosition(int x, int y);
    void ShowBalloon(int player_id, String *lines, int width, int height, int tinted);
    void SetPreviewGender(int gender);
    void SetPreviewSkin(int skin);
    void SetPreviewHairStyle(int hair_style);
    void SetPreviewHairColor(int hair_color);
    void AddPlayer(int player_id, String name, String secondary_name, int position_extra, int x, int y, int direction,
        int record_value_a, String guild_tag, int unused_value, int gender, int hair_style, int hair_color, int skin,
        int equip_slot_a, int equip_slot_b, int equip_slot_c, int equip_slot_d, int boots, int equip_after_boots_a,
        int equip_after_boots_b, int equip_after_armor, int armor, int equip_before_hat, int hat, int shield,
        int weapon, int sit_state, int hidden, int arrival_type);
    void HidePlayer(int player_id);
    void UnhidePlayer(int player_id);
    void KillPlayer(int player_id);
    void RemovePlayer(int player_id);
    void StartJump(int player_id, int x, int y, int direction);
    void StartWalk(int player_id, int x, int y, int direction);
    void SetDirection(int player_id, int direction);
    int GetDirection(int player_id);
    void SetGender(int player_id, int gender);
    void SetHairColor(int player_id, int hair_color);
    void SetHairStyle(int player_id, int hair_style);
    void SetSkin(int player_id, int skin);
    void SetBadgeLevel(int player_id, int badge_level);
    void SetDisplayNumber(int player_id, int display_number);
    void SetCharacterId(int player_id, int character_id);
    void SetSlotState(int player_id, int slot_state);
    void SetName(int player_id, String name);
    void SetBoots(int player_id, int boots);
    void SetArmor(int player_id, int armor);
    void SetHat(int player_id, int hat);
    void SetShield(int player_id, int shield);
    void SetWeapon(int player_id, int weapon);
    int GetX(int player_id);
    int GetY(int player_id);
    int GetGender(int player_id);
    int GetHairColor(int player_id);
    int GetHairStyle(int player_id);
    int GetSkin(int player_id);
    int GetCharacterId(int player_id);
    int GetSlotState(int player_id);
    String GetName(int player_id);
    String GetNameByCharacterId(int character_id);
    bool ConsumeRedraw();
    bool HasPlayer(int player_id);
    int GetBoots(int player_id);
    int GetArmor(int player_id);
    int GetShield(int player_id);
    int GetWeapon(int player_id);
    bool HasBalloon(int player_id);
    int FindPlayerAt(int x, int y);
    bool HasVisiblePlayerAt(int x, int y);
    bool HasPlayerAt(int x, int y);
    void ShowDamage(int player_id, int amount, char heal);
    void ShowHealthBar(int player_id, int percent, int duration);
    void StartEffect(int player_id, int effect_id);
    char IsEffectPlaying(int player_id);
    void StartLevelUpEffect(int player_id);
    void StartEmote(int player_id, int emote_id);
    char IsEmoting(int player_id);
    char IsWalking(int player_id);
    void StartAttack(int player_id, int direction, char silent);
    char IsAttacking(int player_id);
    void StartSpellCast(int player_id, int direction);
    char IsCasting(int player_id);
    void StartStandUp(int player_id, int x, int y);
    void StartSitOnFloor(int player_id, int x, int y, int direction);
    void StartSitOnChair(int player_id, int x, int y, int direction);
    char IsSittingOnFloor(int player_id);
    char IsSittingOnChair(int player_id);
    Player GetPlayerById(int player_id);
    Player GetPlayer(int index);
    void Clear();
    unsigned int GetCount();
    void SetMousePosition(int cursor_x, int cursor_y);
};

#endif
