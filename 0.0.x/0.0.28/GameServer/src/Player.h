#ifndef PlayerH
#define PlayerH

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
#include "Constants.h"

enum AdminLevel
{
    AdminLevel_Player = 0,
    AdminLevel_LightGuide = 1,
    AdminLevel_Guardian = 2,
    AdminLevel_GameMaster = 3,
    AdminLevel_HighGameMaster = 4
};

enum FacingDirection
{
    FacingDirection_Down = 0,
    FacingDirection_Left = 1,
    FacingDirection_Up = 2,
    FacingDirection_Right = 3
};
#define CHARACTER_SLOT_COUNT 3
#define DROP_TOKEN_BUDGET 20
#define GLOBAL_TOKEN_BUDGET 3
#define PARTY_MAX_MEMBERS 10
#define TALK_TOKEN_BUDGET 30
#define WEIGHT_DISPLAY_MAX 250

struct PlayerCommand;
struct PlayerInventory;
struct PlayerQuest;
struct PlayerSkill;

#include "Playercommand.h"
#include "Playerinventory.h"
#include "Playerquest.h"
#include "Playerskill.h"

struct Player;

// A connection and, once logged in, its character: the account, the stats, the items and the state of play.
struct Player
{
    bool connected;
    bool initialized;
    bool removing;
    bool account_logged_in;
    bool logged_in;
    unsigned char ping_timeout;
    int player_id;
    int account_ident;
    int query_id;
    String guild_query_text;
    String signup;
    int server_encryption_multiple;
    int client_encryption_multiple;
    int sequence;
    int account_create_cooldown;
    int field_0x2c;
    int session_id;
    int challenge_response;
    String receive_buffer;
    String hdid;
    String remote_ip;
    String account_name;
    String account_type;
    int packet_count;
    int remove_timer;
    bool in_arena;
    bool arena_entering;
    int arena_kills;
    bool bank_dirty;
    bool inventory_dirty;
    bool equipment_dirty;
    bool base_stats_dirty;
    int character_id;
    int class_id;
    int account_id;
    int guild_rank_id;
    int party_requested_id;
    int trade_partner_id;
    int wedding_request_id;
    int npc_index;
    int dialog_behavior_id;
    int session_token;
    int guild_inviter_id;
    String guild_recruiter_name;
    bool trade_accepted;
    int trade_value;
    int admin_level;
    int save_ticks;
    int recover_ticks;
    bool field_0xa4;
    String name;
    String partner_name;
    String title;
    String guild_rank_name;
    String guild_tag;
    String guild_name;
    int experience;
    int level;
    bool whispers_disabled;
    int gender;
    int hair_style;
    int hair_color;
    int skin;
    int map_id;
    int x;
    int y;
    int direction;
    int warp_map;
    int warp_effect;
    short warp_x;
    short warp_y;
    int restore_map;
    short restore_x;
    short restore_y;
    bool warp_pending;
    bool map_switch_pending;
    int base_hp;
    int max_hp;
    int hp;
    int base_tp;
    int max_tp;
    int tp;
    int base_sp;
    int max_sp;
    int usage;
    int home_id;
    String home_name;
    int money_bank;
    int locker_bank;
    int karma;
    int weight_current;
    int weight_max;
    int stat_points;
    int skill_points;
    short min_damage;
    short max_damage;
    short accuracy;
    short evasion;
    short armor;
    short class_min_damage;
    short class_max_damage;
    short class_accuracy;
    short class_evasion;
    short class_armor;
    short element_resistances[ELEMENT_COUNT];
    int equip_bonus_hp;
    int equip_bonus_tp;
    int equip_strength_bonus;
    int equip_wisdom_bonus;
    int equip_intelligence_bonus;
    int equip_agility_bonus;
    int equip_constitution_bonus;
    int equip_charisma_bonus;
    int base_strength;
    int base_wisdom;
    int base_intelligence;
    int base_agility;
    int base_constitution;
    int base_charisma;
    int adj_strength;
    int adj_wisdom;
    int adj_intelligence;
    int adj_agility;
    int adj_constitution;
    int adj_charisma;
    int boots_graphic_id;
    int accessory_graphic_id;
    int gloves_graphic_id;
    int armor_graphic_id;
    int belt_graphic_id;
    int necklace_graphic_id;
    int hat_graphic_id;
    int shield_graphic_id;
    int weapon_graphic_id;
    int ring1_graphic_id;
    int ring2_graphic_id;
    int armlet1_graphic_id;
    int armlet2_graphic_id;
    int bracer1_graphic_id;
    int bracer2_graphic_id;
    int boots_item_id;
    int accessory_item_id;
    int gloves_item_id;
    int armor_item_id;
    int belt_item_id;
    int necklace_item_id;
    int hat_item_id;
    int shield_item_id;
    int weapon_item_id;
    int ring1_item_id;
    int ring2_item_id;
    int armlet1_item_id;
    int armlet2_item_id;
    int bracer1_item_id;
    int bracer2_item_id;
    vector<PlayerInventory> inventory;
    vector<PlayerInventory> trade_items;
    vector<PlayerInventory> bank;
    vector<PlayerSkill> spells;
    vector<PlayerQuest> quest_trackers;
    vector<PlayerQuest> quest_history;
    bool in_party;
    int party_leader_id;
    int party_ids[PARTY_MAX_MEMBERS];
    String invblob1;
    String bankblob;
    String skillblob;
    String quest_cache;
    String quest_blob;
    Player *character_slots[CHARACTER_SLOT_COUNT];
    bool stats_dirty;
    bool dead;
    int last_client_tick;
    int sync_base_ahead;
    int sync_base_behind;
    TTimeStamp last_pass_ms;
    int ghost_walk_tokens;
    int ghost_token_ticks;
    int attack_tokens;
    int attack_token_ticks;
    int queued_spell_id;
    int expected_cast_timestamp;
    int global_tokens;
    int talk_tokens;
    int drop_counter;
    TTimeStamp last_action_tick;
    vector<PlayerCommand> action_queue;
    bool fast_action;
    bool flush_queue;
    TCustomWinSocket *socket;
    char pad_0x3b4[12];
    String pending_char_name;
    int item_change_id;
    int item_change_count;
    int item_change_remaining;
    int item_change_amount;
    int equip_result;
    int equip_result_count;
    char map_has_quakes;
    char map_has_hp_drain;
    char map_has_tp_drain;
    char map_has_spikes;
    int idle_ticks;
    char on_chair;
    char sitting;
    bool hidden;
    bool hide_online;
    bool cheater_flag;
    bool global_chat;
    TDateTime enter_game_timestamp;

    Player(Scktcomp::TCustomWinSocket *socket);
    ~Player();
    int HpPercent();
    int CountPartyMembers();
    void AddPartyMember(int member_id);
    char IsPartyMember(int player_id);
    void ClearPartyRoster();
    void CalculateAdjStats();
    void CalculateHP_TP_SP();
};

#endif
