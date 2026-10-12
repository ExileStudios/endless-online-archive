#include <vcl.h>
#pragma hdrstop

#include "Players.h"
#include "Filecache.h"
#include "Gamecontrol.h"
#include "Itemvalue.h"
#include "Itemvalues.h"
#include "MainForm.h"
#include "Mysqlcontrols.h"
#include "Packets.h"
#include "Player.h"
#include "Playercommand.h"
#include "Playerinventory.h"
#include "Playerquest.h"
#include "Playerskill.h"
#include "Settings.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

// An empty list; no one online yet.
Players::Players(Settings *settings, mySQLdb *mysql_controls)
{
    this->online_count = 0;
    this->peak_online = 0;
    this->dirty = false;
    this->settings = settings;
    this->mysql_controls = mysql_controls;
    for (int i = 0; i < MAX_PLAYER_ID; i++)
        this->by_id[i] = 0;
}

Players::~Players()
{
}

// Closes the connections marked for removal, then runs each player's next queued action once its wait is over.
void Players::Tick()
{
    vector<Player *>::iterator remove_iter;
    vector<Player *>::iterator iter;
    TTimeStamp now;
    // .. one socket at a time: a close changes the list
    while (dirty)
    {
        dirty = false;
        for (remove_iter = players.begin(); remove_iter != players.end(); remove_iter++)
        {
            if ((*remove_iter)->removing)
            {
                (*remove_iter)->socket->Close();
                dirty = true;
                break;
            }
        }
    }
    now = DateTimeToTimeStamp(Now());
    for (iter = players.begin(); iter != players.end(); iter++)
    {
        if (!(*iter)->removing && (*iter)->flush_queue)
        {
            (*iter)->flush_queue = false;
            (*iter)->action_queue.clear();
            continue;
        }
        if (!(*iter)->removing && (*iter)->action_queue.size() > 0)
        {
            int delta = now.Time - (*iter)->last_action_tick.Time;
            if (delta > 7500000)  // the clock passed midnight
                delta = 300;
            // 300 ms between actions, 100 ms after a turn
            if (delta >= 300 || (delta >= 100 && (*iter)->fast_action))
            {
                Packets *server = GUI->Mainform_GetServer();
                (*iter)->flush_queue = false;
                (*iter)->fast_action = false;
                if ((*iter)->action_queue[0].family == OP_MOVE)
                    server->Walk_Execute(*iter, (*iter)->action_queue[0].action, &(*iter)->action_queue[0].text);
                if ((*iter)->action_queue[0].family == OP_ATTACK)
                    server->Attack_Execute(*iter, (*iter)->action_queue[0].action, &(*iter)->action_queue[0].text);
                if ((*iter)->action_queue[0].family == OP_MAGIC)
                    server->Spell_Execute(*iter, (*iter)->action_queue[0].action, &(*iter)->action_queue[0].text);
                if ((*iter)->action_queue[0].family == OP_SIT)
                    server->Chair_Execute(*iter, (*iter)->action_queue[0].action, &(*iter)->action_queue[0].text);
                if ((*iter)->action_queue[0].family == OP_DIR)
                {
                    (*iter)->fast_action = true;
                    server->Face_Execute(*iter, (*iter)->action_queue[0].action, &(*iter)->action_queue[0].text);
                }
                if (!(*iter)->flush_queue)
                {
                    (*iter)->action_queue.erase((*iter)->action_queue.begin());
                    continue;
                }
                (*iter)->flush_queue = false;
                (*iter)->action_queue.clear();
                continue;
            }
        }
    }
}

// A new connection; 0 when its handle is too big or in use, or its ip already has max_clones connections.
char Players::Add(Scktcomp::TCustomWinSocket *socket)
{
    if (socket->SocketHandle >= MAX_PLAYER_ID)
        return 0;
    if (by_id[socket->SocketHandle])
        return 0;
    int same_ip = 0;
    for (vector<Player *>::iterator iter = players.begin(); iter != players.end(); iter++)
    {
        if ((*iter)->socket->RemoteAddress != socket->RemoteAddress)
            continue;
        same_ip++;
        if (settings->GetMaxClones() <= same_ip)
            return 0;
    }
    Player *player = new Player(socket);
    by_id[socket->SocketHandle] = player;
    players.insert(players.end(), player);
    return 1;
}

// Marks the socket's player for removal at the next tick.
void Players::MarkRemoving(Scktcomp::TCustomWinSocket *socket)
{
    if (by_id[socket->SocketHandle])
    {
        Player *result = by_id[socket->SocketHandle];
        result->removing = true;
        dirty = true;
    }
}

// Drops the socket's player: their character slots, and when they played, their save and their lists.
void Players::Remove(Scktcomp::TCustomWinSocket *socket)
{
    if (by_id[socket->SocketHandle])
    {
        Player *player = by_id[socket->SocketHandle];
        for (int i = 0; i < CHARACTER_SLOT_COUNT; i++)
        {
            if (!player->character_slots[i])
                continue;
            Player *slot = player->character_slots[i];
            player->character_slots[i] = 0;
            delete slot;
        }
        if (player->logged_in)
        {
            if (online_count > 0)
                online_count--;
            mysql_controls->Mysql_SubmitExec(player->account_ident, Character_BuildSaveQuery(player, 0));
            player->trade_items.clear();
            player->inventory.clear();
            player->bank.clear();
            player->spells.clear();
            player->ClearPartyRoster();
            player->action_queue.clear();
            player->quest_trackers.clear();
            player->quest_history.clear();
        }
        for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
        {
            if ((*it)->player_id == socket->SocketHandle)
            {
                players.erase(it);
                break;
            }
        }
        by_id[socket->SocketHandle] = 0;
        delete player;
    }
}

// The UPDATE that saves the character; flags goes to its online column.
// with sqlsmart on, the stats, equipment, inventory and bank are written only when they changed
String Players::Character_BuildSaveQuery(Player *player, int flags)
{
    TTimeStamp enter;
    TTimeStamp now;
    int date_diff;
    int time_diff;
    vector<PlayerInventory>::iterator inv_it;
    vector<PlayerInventory>::iterator bank_it;
    vector<PlayerSkill>::iterator spell_it;
    vector<PlayerQuest>::iterator quest_it;
    if (!player->logged_in)
        return "";
    enter = DateTimeToTimeStamp(player->enter_game_timestamp);
    now = DateTimeToTimeStamp(Now());
    date_diff = now.Date - enter.Date;
    time_diff = now.Time - enter.Time;
    date_diff = time_diff / MS_PER_MINUTE + date_diff * 1440;  // minutes played this time
    player->usage = player->usage + date_diff;
    String invblob = "";
    String skillblob = "";
    String bankblob = "";
    String questcache = "";
    String questblob = "";
    // .. each list as id:amount: ( the quests id:state:version and their counters ), ended by EOF
    for (inv_it = player->inventory.begin(); inv_it != player->inventory.end(); inv_it++)
    {
        if (inv_it->item_id <= 0)
            continue;
        invblob.Insert(IntToStr(inv_it->item_id), invblob.Length() + 1);
        invblob.Insert(":", invblob.Length() + 1);
        invblob.Insert(IntToStr((DWORD)inv_it->amount), invblob.Length() + 1);
        invblob.Insert(":", invblob.Length() + 1);
    }
    for (bank_it = player->bank.begin(); bank_it != player->bank.end(); bank_it++)
    {
        if (bank_it->item_id <= 0)
            continue;
        bankblob.Insert(IntToStr(bank_it->item_id), bankblob.Length() + 1);
        bankblob.Insert(":", bankblob.Length() + 1);
        bankblob.Insert(IntToStr((DWORD)bank_it->amount), bankblob.Length() + 1);
        bankblob.Insert(":", bankblob.Length() + 1);
    }
    for (spell_it = player->spells.begin(); spell_it != player->spells.end(); spell_it++)
    {
        if (spell_it->skill_id <= 0)
            continue;
        skillblob.Insert(IntToStr(spell_it->skill_id), skillblob.Length() + 1);
        skillblob.Insert(":", skillblob.Length() + 1);
        skillblob.Insert(IntToStr((DWORD)spell_it->level), skillblob.Length() + 1);
        skillblob.Insert(":", skillblob.Length() + 1);
    }
    for (quest_it = player->quest_trackers.begin(); quest_it != player->quest_trackers.end(); quest_it++)
    {
        if (quest_it->quest_id <= 0)
            continue;
        questcache.Insert(IntToStr(quest_it->quest_id), questcache.Length() + 1);
        questcache.Insert(":", questcache.Length() + 1);
        questcache.Insert(IntToStr(quest_it->state_index), questcache.Length() + 1);
        questcache.Insert(":", questcache.Length() + 1);
        questcache.Insert(IntToStr(quest_it->version), questcache.Length() + 1);
        questcache.Insert(":", questcache.Length() + 1);
        for (int n = 0; n < QUEST_COUNTER_SLOTS; n++)
        {
            questcache.Insert(IntToStr(quest_it->counters[n]), questcache.Length() + 1);
            questcache.Insert(":", questcache.Length() + 1);
        }
    }
    for (quest_it = player->quest_history.begin(); quest_it != player->quest_history.end(); quest_it++)
    {
        if (quest_it->quest_id > 0)
        {
            questblob.Insert(IntToStr(quest_it->quest_id), questblob.Length() + 1);
            questblob.Insert(":", questblob.Length() + 1);
        }
    }
    invblob = invblob + "EOF";
    skillblob = skillblob + "EOF";
    bankblob = bankblob + "EOF";
    questcache = questcache + "EOF";
    questblob = questblob + "EOF";
    String invblob_tail;
    String bankblob_tail;
    String skillblob_tail;
    String questblob_tail;
    // .. a list longer than 255 goes on in its second column
    if (invblob.Length() > 255)
    {
        invblob_tail = invblob.SubString(256, invblob.Length() - 255);
        invblob.Delete(256, invblob.Length() - 255);
    }
    if (bankblob.Length() > 255)
    {
        bankblob_tail = bankblob.SubString(256, bankblob.Length() - 255);
        bankblob.Delete(256, bankblob.Length() - 255);
    }
    if (skillblob.Length() > 255)
    {
        skillblob_tail = skillblob.SubString(256, skillblob.Length() - 255);
        skillblob.Delete(256, invblob.Length() - 255);
    }
    if (questblob.Length() > 255)
    {
        questblob_tail = questblob.SubString(256, questblob.Length() - 255);
        questblob.Delete(256, questblob.Length() - 255);
    }
    String query = "UPDATE endl_characters SET ";
    query.Insert("ident_class = " + IntToStr(player->class_id), query.Length() + 1);
    query.Insert(",ident_rank = " + IntToStr(player->guild_rank_id), query.Length() + 1);
    query.Insert(",ident_guild = '" + player->guild_tag + "'", query.Length() + 1);
    query.Insert(",partner = '" + player->partner_name + "'", query.Length() + 1);
    query.Insert(",title = '" + player->title + "'", query.Length() + 1);
    query.Insert(",guild = '" + player->guild_name + "'", query.Length() + 1);
    query.Insert(",rank = '" + player->guild_rank_name + "'", query.Length() + 1);
    query.Insert(",citizenship = '" + IntToStr(player->home_id) + "'", query.Length() + 1);
    query.Insert(",experience = " + IntToStr(player->experience), query.Length() + 1);
    query.Insert(",level = " + IntToStr(player->level), query.Length() + 1);
    query.Insert(",stat_points = " + IntToStr(player->stat_points), query.Length() + 1);
    query.Insert(",skill_points = " + IntToStr(player->skill_points), query.Length() + 1);
    query.Insert(",gender = " + IntToStr(player->gender), query.Length() + 1);
    query.Insert(",hairmodal = " + IntToStr(player->hair_style), query.Length() + 1);
    query.Insert(",haircolor = " + IntToStr(player->hair_color), query.Length() + 1);
    query.Insert(",skincolor = " + IntToStr(player->skin), query.Length() + 1);
    query.Insert(",nav_map = " + IntToStr(player->map_id), query.Length() + 1);
    query.Insert(",nav_x = " + IntToStr(player->x), query.Length() + 1);
    query.Insert(",nav_y = " + IntToStr(player->y), query.Length() + 1);
    query.Insert(",nav_direction = " + IntToStr(player->direction), query.Length() + 1);
    query.Insert(",hp_max = " + IntToStr(player->base_hp), query.Length() + 1);
    query.Insert(",hp_now = " + IntToStr(player->hp), query.Length() + 1);
    query.Insert(",mp_max = " + IntToStr(player->base_tp), query.Length() + 1);
    query.Insert(",mp_now = " + IntToStr(player->tp), query.Length() + 1);
    query.Insert(",sp_max = " + IntToStr(player->base_sp), query.Length() + 1);
    if (player->base_stats_dirty || !settings->GetSqlSmart())
    {
        query.Insert(",stat_strenght = " + IntToStr(player->base_strength), query.Length() + 1);
        query.Insert(",stat_wisdom = " + IntToStr(player->base_wisdom), query.Length() + 1);
        query.Insert(",stat_intelligence = " + IntToStr(player->base_intelligence), query.Length() + 1);
        query.Insert(",stat_agility = " + IntToStr(player->base_agility), query.Length() + 1);
        query.Insert(",stat_constitution = " + IntToStr(player->base_constitution), query.Length() + 1);
        query.Insert(",stat_charisma = " + IntToStr(player->base_charisma), query.Length() + 1);
        player->base_stats_dirty = false;
    }
    if (player->money_bank < 0)
        player->money_bank = 0;
    query.Insert(",clientusge = " + IntToStr(player->usage), query.Length() + 1);
    query.Insert(",money_bank = " + IntToStr(player->money_bank), query.Length() + 1);
    query.Insert(",locker_bank = " + IntToStr(player->locker_bank), query.Length() + 1);
    query.Insert(",alignment_good = " + IntToStr(player->karma), query.Length() + 1);
    if (player->equipment_dirty || !settings->GetSqlSmart())
    {
        query.Insert(",eq_boots = " + IntToStr(player->boots_item_id), query.Length() + 1);
        query.Insert(",eq_pants = " + IntToStr(player->accessory_item_id), query.Length() + 1);
        query.Insert(",eq_gloves = " + IntToStr(player->gloves_item_id), query.Length() + 1);
        query.Insert(",eq_armor = " + IntToStr(player->armor_item_id), query.Length() + 1);
        query.Insert(",eq_belt = " + IntToStr(player->belt_item_id), query.Length() + 1);
        query.Insert(",eq_necklage = " + IntToStr(player->necklace_item_id), query.Length() + 1);
        query.Insert(",eq_hat = " + IntToStr(player->hat_item_id), query.Length() + 1);
        query.Insert(",eq_shield = " + IntToStr(player->shield_item_id), query.Length() + 1);
        query.Insert(",eq_weapon = " + IntToStr(player->weapon_item_id), query.Length() + 1);
        query.Insert(",eq_ring_l = " + IntToStr(player->ring1_item_id), query.Length() + 1);
        query.Insert(",eq_ring_r = " + IntToStr(player->ring2_item_id), query.Length() + 1);
        query.Insert(",eq_armlet_l = " + IntToStr(player->armlet1_item_id), query.Length() + 1);
        query.Insert(",eq_armlet_r = " + IntToStr(player->armlet2_item_id), query.Length() + 1);
        query.Insert(",eq_bracer_l = " + IntToStr(player->bracer1_item_id), query.Length() + 1);
        query.Insert(",eq_bracer_r = " + IntToStr(player->bracer2_item_id), query.Length() + 1);
        player->equipment_dirty = false;
    }
    if (player->inventory_dirty || !settings->GetSqlSmart())
    {
        query.Insert(",invblob = '" + invblob + "'", query.Length() + 1);
        query.Insert(",invblob2 = '" + invblob_tail + "'", query.Length() + 1);
        player->inventory_dirty = false;
    }
    if (player->bank_dirty || !settings->GetSqlSmart())
    {
        query.Insert(",invblob3 = '" + bankblob + "'", query.Length() + 1);
        query.Insert(",invblob4 = '" + bankblob_tail + "'", query.Length() + 1);
        player->bank_dirty = false;
    }
    query.Insert(",skillblob = '" + skillblob + "'", query.Length() + 1);
    query.Insert(",skillblob2 = '" + skillblob_tail + "'", query.Length() + 1);
    query.Insert(",questcache = '" + questcache + "'", query.Length() + 1);
    query.Insert(",questblob = '" + questblob + "'", query.Length() + 1);
    query.Insert(",questblob2 = '" + questblob_tail + "'", query.Length() + 1);
    int sit = 0;
    if (player->on_chair)
        sit = 1;
    if (player->sitting)
        sit = 2;
    // 0 = standing, 1 = on a chair, 2 = on the floor
    query.Insert(",sitting = " + IntToStr(sit), query.Length() + 1);
    query.Insert(",online = " + IntToStr(flags), query.Length() + 1);
    query.Insert(" WHERE ident = " + IntToStr(player->character_id), query.Length() + 1);
    query.Insert(" AND ident_account = " + IntToStr(player->account_id), query.Length() + 1);
    mysql_controls->file_cache->UpdatePlayerCache(player);
    return query;
}

// 1 when a connection is on the account.
char Players::IsAccountIdentOnline(int account_ident)
{
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->account_ident == account_ident)
            return 1;
    }
    return 0;
}

// True when another connection is logged in with the account name ( names under 3 characters never are ).
bool Players::IsAccountNameTaken(String account_name, int player_id)
{
    if (account_name.Length() < 3)
        return false;
    for (vector<Player *>::iterator iter = players.begin(); iter != players.end(); iter++)
    {
        if (account_name == (*iter)->account_name)
        {
            if ((*iter)->player_id != player_id)
                return true;
        }
    }
    return false;
}

// 1 when a player stands on the tile.
char Players::IsPlayerAt(int map_id, int x, int y)
{
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->x == x && (*it)->y == y && (*it)->map_id == map_id)
            return 1;
    }
    return 0;
}

// True when the player already holds a name, or another player holds this one; else they now hold it.
bool Players::CharName_IsReserved(Player *player, String name)
{
    if (player->pending_char_name != "")
        return true;
    else
    {
        vector<Player *>::iterator it;
        for (it = players.begin(); it != players.end(); it++)
        {
            if (name == (*it)->pending_char_name)
                return true;
        }
        player->pending_char_name = name;
        return false;
    }
}

// 1 when the player carries the key ( an EifItemType_Key item ) for the lock.
char Players::Player_HasKeyItem(Player *player, int key_id)
{
    for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
    {
        if (GUI->item_values->GetType(it->item_id) == EifItemType_Key &&
            GUI->item_values->GetSpec1(it->item_id) == key_id)
            return 1;
    }
    return 0;
}

// The spell's level, or -1 when the player does not know it.
int Players::Player_GetSpellLevel(Player *player, int spell_id)
{
    for (vector<PlayerSkill>::iterator it = player->spells.begin(); it != player->spells.end(); it++)
    {
        if (it->skill_id == spell_id)
            return it->level;
    }
    return -1;
}

char Players::Player_HasSpellId(Player *player, int spell_id)
{
    for (vector<PlayerSkill>::iterator it = player->spells.begin(); it != player->spells.end(); it++)
    {
        if (it->skill_id == spell_id)
            return 1;
    }
    return 0;
}

char Players::Player_HasBankItem(Player *player, int item_id)
{
    for (vector<PlayerInventory>::iterator it = player->bank.begin(); it != player->bank.end(); it++)
    {
        if (it->item_id == item_id)
            return 1;
    }
    return 0;
}

// Puts an item from the inventory on: 1 when it went on, 0 when there is none or the slot is taken.
// an armor must fit the gender; slot picks the left ( 0 ) or right ( 1 ) ring, armlet or bracer
char Players::Player_EquipItem(Player *player, int item_id, int slot)
{
    player->equipment_dirty = true;
    player->inventory_dirty = true;
    player->equip_result = 0;
    int type = GUI->item_values->GetType(item_id);
    for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
    {
        if (it->item_id == item_id)
        {
            if (it->amount > 0)
            {
                player->equip_result = 0;
                if (type == EifItemType_Weapon && player->weapon_item_id <= 0)
                {
                    player->equip_result = 2;
                    player->weapon_item_id = item_id;
                    player->weapon_graphic_id = GUI->item_values->GetSpec1ForTypes(item_id);
                }
                if (type == EifItemType_Shield && player->shield_item_id <= 0)
                {
                    player->equip_result = 2;
                    player->shield_item_id = item_id;
                    player->shield_graphic_id = GUI->item_values->GetSpec1ForTypes(item_id);
                }
                if (type == EifItemType_Armor && player->armor_item_id <= 0 &&
                    GUI->item_values->GetGender(item_id) == player->gender)
                {
                    player->equip_result = 2;
                    player->armor_item_id = item_id;
                    player->armor_graphic_id = GUI->item_values->GetSpec1ForTypes(item_id);
                }
                if (type == EifItemType_Hat && player->hat_item_id <= 0)
                {
                    player->equip_result = 2;
                    player->hat_item_id = item_id;
                    player->hat_graphic_id = GUI->item_values->GetSpec1ForTypes(item_id);
                }
                if (type == EifItemType_Boots && player->boots_item_id <= 0)
                {
                    player->equip_result = 2;
                    player->boots_item_id = item_id;
                    player->boots_graphic_id = GUI->item_values->GetSpec1ForTypes(item_id);
                }
                if (type == EifItemType_Gloves && player->gloves_item_id <= 0)
                {
                    player->equip_result = 2;
                    player->gloves_item_id = item_id;
                    player->gloves_graphic_id = item_id;
                }
                if (type == EifItemType_Accessory && player->accessory_item_id <= 0)
                {
                    player->equip_result = 1;
                    player->accessory_item_id = item_id;
                    player->accessory_graphic_id = item_id;
                }
                if (type == EifItemType_Belt && player->belt_item_id <= 0)
                {
                    player->equip_result = 1;
                    player->belt_item_id = item_id;
                    player->belt_graphic_id = item_id;
                }
                if (type == EifItemType_Necklace && player->necklace_item_id <= 0)
                {
                    player->equip_result = 1;
                    player->necklace_item_id = item_id;
                    player->necklace_graphic_id = item_id;
                }
                if (type == EifItemType_Ring && player->ring1_item_id <= 0 && !slot)
                {
                    player->equip_result = 1;
                    player->ring1_item_id = item_id;
                    player->ring1_graphic_id = item_id;
                }
                if (type == EifItemType_Ring && player->ring2_item_id <= 0 && slot == 1)
                {
                    player->equip_result = 1;
                    player->ring2_item_id = item_id;
                    player->ring2_graphic_id = item_id;
                }
                if (type == EifItemType_Armlet && player->armlet1_item_id <= 0 && !slot)
                {
                    player->equip_result = 1;
                    player->armlet1_item_id = item_id;
                    player->armlet1_graphic_id = item_id;
                }
                if (type == EifItemType_Armlet && player->armlet2_item_id <= 0 && slot == 1)
                {
                    player->equip_result = 1;
                    player->armlet2_item_id = item_id;
                    player->armlet2_graphic_id = item_id;
                }
                if (type == EifItemType_Bracer && player->bracer1_item_id <= 0 && !slot)
                {
                    player->equip_result = 1;
                    player->bracer1_item_id = item_id;
                    player->bracer1_graphic_id = item_id;
                }
                if (type == EifItemType_Bracer && player->bracer2_item_id <= 0 && slot == 1)
                {
                    player->equip_result = 1;
                    player->bracer2_item_id = item_id;
                    player->bracer2_graphic_id = item_id;
                }
                if (player->equip_result > 0)
                {
                    it->amount--;
                    player->equip_result_count = it->amount;
                    if (it->amount < 1)
                        player->inventory.erase(it);
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}

// Takes an equipped item off, back into the inventory: 1 when it was on, else 0.
char Players::Player_UnequipItem(Player *player, int item_id, int slot)
{
    player->equipment_dirty = true;
    player->inventory_dirty = true;
    player->equip_result = 0;
    int type = GUI->item_values->GetType(item_id);
    if (type == EifItemType_Weapon && player->weapon_item_id == item_id)
    {
        player->weapon_item_id = 0;
        player->weapon_graphic_id = 0;
        player->equip_result = 2;
    }
    if (type == EifItemType_Shield && player->shield_item_id == item_id)
    {
        player->shield_item_id = 0;
        player->shield_graphic_id = 0;
        player->equip_result = 2;
    }
    if (type == EifItemType_Armor && player->armor_item_id == item_id)
    {
        player->armor_item_id = 0;
        player->armor_graphic_id = 0;
        player->equip_result = 2;
    }
    if (type == EifItemType_Hat && player->hat_item_id == item_id)
    {
        player->hat_item_id = 0;
        player->hat_graphic_id = 0;
        player->equip_result = 2;
    }
    if (type == EifItemType_Boots && player->boots_item_id == item_id)
    {
        player->boots_item_id = 0;
        player->boots_graphic_id = 0;
        player->equip_result = 2;
    }
    if (type == EifItemType_Gloves && player->gloves_item_id == item_id)
    {
        player->gloves_item_id = 0;
        player->gloves_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Accessory && player->accessory_item_id == item_id)
    {
        player->accessory_item_id = 0;
        player->accessory_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Belt && player->belt_item_id == item_id)
    {
        player->belt_item_id = 0;
        player->belt_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Necklace && player->necklace_item_id == item_id)
    {
        player->necklace_item_id = 0;
        player->necklace_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Ring && player->ring1_item_id == item_id && !slot)
    {
        player->ring1_item_id = 0;
        player->ring1_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Ring && player->ring2_item_id == item_id && slot == 1)
    {
        player->ring2_item_id = 0;
        player->ring2_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Armlet && player->armlet1_item_id == item_id && !slot)
    {
        player->armlet1_item_id = 0;
        player->armlet1_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Armlet && player->armlet2_item_id == item_id && slot == 1)
    {
        player->armlet2_item_id = 0;
        player->armlet2_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Bracer && player->bracer1_item_id == item_id && !slot)
    {
        player->bracer1_item_id = 0;
        player->bracer1_graphic_id = 0;
        player->equip_result = 1;
    }
    if (type == EifItemType_Bracer && player->bracer2_item_id == item_id && slot == 1)
    {
        player->bracer2_item_id = 0;
        player->bracer2_graphic_id = 0;
        player->equip_result = 1;
    }
    if (player->equip_result > 0)
    {
        for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
        {
            if (it->item_id == item_id)
            {
                it->amount++;
                player->equip_result_count = it->amount;
                return 1;
            }
        }
        PlayerInventory item(item_id);
        item.amount = 1;
        player->inventory.insert(player->inventory.end(), item);
        player->equip_result_count = 1;
        return 1;
    }
    return 0;
}

// Adds to the inventory, stacking on the item when the player has it.
void Players::Player_AddItem(Player *player, int item_id, int amount)
{
    player->inventory_dirty = true;
    for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
    {
        if (it->item_id == item_id)
        {
            it->amount += amount;
            player->item_change_amount = it->amount;
            return;
        }
    }
    PlayerInventory item(item_id);
    item.amount = amount;
    player->inventory.insert(player->inventory.end(), item);
    player->item_change_amount = amount;
}

// Takes amount of the item from the inventory: 0 when the player has less.
char Players::Player_RemoveItem(Player *player, int item_id, unsigned int amount)
{
    player->equipment_dirty = true;
    player->inventory_dirty = true;
    for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
    {
        if (it->item_id != item_id)
            continue;
        if (amount > it->amount)
        {
            player->item_change_id = -1;
            player->item_change_count = 0;
            player->item_change_remaining = 0;
            return 0;
        }
        else
        {
            if (amount == it->amount)
            {
                player->item_change_id = item_id;
                player->item_change_count = amount;
                player->item_change_remaining = 0;
                player->inventory.erase(it);
                return 1;
            }
            else
            {
                it->amount -= amount;
                player->item_change_id = item_id;
                player->item_change_count = amount;
                player->item_change_remaining = it->amount;
                return 1;
            }
        }
    }
    return 0;
}

// Takes up to amount of the item from the inventory, all the player has when they have less.
void Players::Player_RemoveItemNoQuestRules(Player *player, int item_id, unsigned int amount)
{
    player->equipment_dirty = true;
    player->inventory_dirty = true;
    for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
    {
        if (it->item_id == item_id)
        {
            if (amount >= it->amount)
            {
                player->item_change_id = item_id;
                player->item_change_count = it->amount;
                player->item_change_remaining = 0;
                player->inventory.erase(it);
                return;
            }
            else
            {
                it->amount -= amount;
                player->item_change_id = item_id;
                player->item_change_count = amount;
                player->item_change_remaining = it->amount;
            }
            return;
        }
    }
}

// How many of the item the player carries, or -1 when none.
int Players::GetItemAmount(Player *player, int item_id)
{
    for (vector<PlayerInventory>::iterator it = player->inventory.begin(); it != player->inventory.end(); it++)
    {
        if (it->item_id == item_id)
            return it->amount;
    }
    return -1;
}

// Adds to the locker, stacking on the item; a stack holds 120 at most.
void Players::Player_AddBankItem(Player *player, int item_id, int amount)
{
    player->bank_dirty = true;
    for (vector<PlayerInventory>::iterator it = player->bank.begin(); it != player->bank.end(); it++)
    {
        if (it->item_id == item_id)
        {
            it->amount += amount;
            if (it->amount > 120)
                it->amount = 120;
            return;
        }
    }
    PlayerInventory item(item_id);
    item.amount = amount;
    player->bank.insert(player->bank.end(), item);
}

// Takes the whole stack of the item out of the locker.
char Players::Player_RemoveBankItem(Player *player, int item_id)
{
    player->bank_dirty = true;
    for (vector<PlayerInventory>::iterator it = player->bank.begin(); it != player->bank.end(); it++)
    {
        if (it->item_id == item_id)
        {
            player->item_change_id = it->item_id;
            player->item_change_count = it->amount;
            player->bank.erase(it);
            return 1;
        }
    }
    return 0;
}

// Offers an item in the trade ( 10 at most ), or a new amount of one offered; either undoes the accept.
char Players::Player_AddTradeItem(Player *player, int item_id, int amount)
{
    if (player->trade_items.size() > 9)
        return 0;
    for (vector<PlayerInventory>::iterator it = player->trade_items.begin(); it != player->trade_items.end(); it++)
    {
        if (it->item_id == item_id)
        {
            player->trade_accepted = false;
            it->amount = amount;
            return 1;
        }
    }
    PlayerInventory item(item_id);
    item.amount = amount;
    player->trade_accepted = false;
    player->trade_items.insert(player->trade_items.end(), item);
    return 1;
}

// Takes an item back out of the trade, unless the player accepted it.
char Players::Player_RemoveTradeItem(Player *player, int item_id)
{
    if (player->trade_accepted)
        return 0;
    if (player->trade_items.size() > 5)
        return 0;
    for (vector<PlayerInventory>::iterator it = player->trade_items.begin(); it != player->trade_items.end(); it++)
    {
        if (it->item_id == item_id)
        {
            player->trade_items.erase(it);
            return 1;
        }
    }
    return 0;
}

// Learns a spell at level 0, once.
void Players::Player_AddSpell(Player *player, int spell_id)
{
    for (vector<PlayerSkill>::iterator it = player->spells.begin(); it != player->spells.end(); it++)
    {
        if (it->skill_id == spell_id)
            return;
    }
    PlayerSkill spell(spell_id);
    spell.level = 0;
    player->spells.insert(player->spells.end(), spell);
    return;
}

// Raises the spell a level ( 100 at most ); its level, or 0 when the player does not know it.
int Players::Player_LevelUpSpell(Player *player, int spell_id)
{
    for (vector<PlayerSkill>::iterator it = player->spells.begin(); it != player->spells.end(); it++)
    {
        if (it->skill_id == spell_id)
        {
            if (it->level < 100)
                it->level++;
            return it->level;
        }
    }
    return 0;
}

char Players::Player_RemoveSpell(Player *player, int spell_id)
{
    for (vector<PlayerSkill>::iterator it = player->spells.begin(); it != player->spells.end(); it++)
    {
        if (it->skill_id == spell_id)
        {
            player->spells.erase(it);
            return 1;
        }
    }
    return 0;
}

void Players::Player_ClearSpells(Player *player)
{
    player->spells.clear();
}

// Takes every cursed item off, with its bonuses: 1 when the weapon, shield, armor, hat or boots changed.
char Players::Player_RemoveCursedItems(Player *player)
{
    char changed = 0;
    if (GUI->item_values->GetSpecial(player->weapon_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *item = GUI->item_values->GetByIndex(player->weapon_item_id - 1);
        if (item->element < ELEMENT_COUNT)
            player->element_resistances[item->element] += item->element_damage;
        player->min_damage -= item->min_damage;
        player->max_damage -= item->max_damage;
        player->accuracy -= item->accuracy;
        player->evasion -= item->evade;
        player->armor -= item->armor;
        player->equip_bonus_hp -= item->hp;
        player->equip_bonus_tp -= item->tp;
        player->equip_strength_bonus -= item->strength;
        player->equip_wisdom_bonus -= item->wisdom;
        player->equip_intelligence_bonus -= item->intelligence;
        player->equip_agility_bonus -= item->agility;
        player->equip_constitution_bonus -= item->constitution;
        player->equip_charisma_bonus -= item->charisma;
        player->weapon_item_id = 0;
        player->weapon_graphic_id = 0;
        changed = 1;
    }
    if (GUI->item_values->GetSpecial(player->shield_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *shield_item = GUI->item_values->GetByIndex(player->shield_item_id - 1);
        if (shield_item->element < ELEMENT_COUNT)
            player->element_resistances[shield_item->element] += shield_item->element_damage;
        player->min_damage -= shield_item->min_damage;
        player->max_damage -= shield_item->max_damage;
        player->accuracy -= shield_item->accuracy;
        player->evasion -= shield_item->evade;
        player->armor -= shield_item->armor;
        player->equip_bonus_hp -= shield_item->hp;
        player->equip_bonus_tp -= shield_item->tp;
        player->equip_strength_bonus -= shield_item->strength;
        player->equip_wisdom_bonus -= shield_item->wisdom;
        player->equip_intelligence_bonus -= shield_item->intelligence;
        player->equip_agility_bonus -= shield_item->agility;
        player->equip_constitution_bonus -= shield_item->constitution;
        player->equip_charisma_bonus -= shield_item->charisma;
        player->shield_item_id = 0;
        player->shield_graphic_id = 0;
        changed = 1;
    }
    if (GUI->item_values->GetSpecial(player->armor_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *armor_item = GUI->item_values->GetByIndex(player->armor_item_id - 1);
        if (armor_item->element < ELEMENT_COUNT)
            player->element_resistances[armor_item->element] += armor_item->element_damage;
        player->min_damage -= armor_item->min_damage;
        player->max_damage -= armor_item->max_damage;
        player->accuracy -= armor_item->accuracy;
        player->evasion -= armor_item->evade;
        player->armor -= armor_item->armor;
        player->equip_bonus_hp -= armor_item->hp;
        player->equip_bonus_tp -= armor_item->tp;
        player->equip_strength_bonus -= armor_item->strength;
        player->equip_wisdom_bonus -= armor_item->wisdom;
        player->equip_intelligence_bonus -= armor_item->intelligence;
        player->equip_agility_bonus -= armor_item->agility;
        player->equip_constitution_bonus -= armor_item->constitution;
        player->equip_charisma_bonus -= armor_item->charisma;
        player->armor_item_id = 0;
        player->armor_graphic_id = 0;
        changed = 1;
    }
    if (GUI->item_values->GetSpecial(player->hat_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *hat_item = GUI->item_values->GetByIndex(player->hat_item_id - 1);
        if (hat_item->element < ELEMENT_COUNT)
            player->element_resistances[hat_item->element] += hat_item->element_damage;
        player->min_damage -= hat_item->min_damage;
        player->max_damage -= hat_item->max_damage;
        player->accuracy -= hat_item->accuracy;
        player->evasion -= hat_item->evade;
        player->armor -= hat_item->armor;
        player->equip_bonus_hp -= hat_item->hp;
        player->equip_bonus_tp -= hat_item->tp;
        player->equip_strength_bonus -= hat_item->strength;
        player->equip_wisdom_bonus -= hat_item->wisdom;
        player->equip_intelligence_bonus -= hat_item->intelligence;
        player->equip_agility_bonus -= hat_item->agility;
        player->equip_constitution_bonus -= hat_item->constitution;
        player->equip_charisma_bonus -= hat_item->charisma;
        player->hat_item_id = 0;
        player->hat_graphic_id = 0;
        changed = 1;
    }
    if (GUI->item_values->GetSpecial(player->boots_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *boots_item = GUI->item_values->GetByIndex(player->boots_item_id - 1);
        if (boots_item->element < ELEMENT_COUNT)
            player->element_resistances[boots_item->element] += boots_item->element_damage;
        player->min_damage -= boots_item->min_damage;
        player->max_damage -= boots_item->max_damage;
        player->accuracy -= boots_item->accuracy;
        player->evasion -= boots_item->evade;
        player->armor -= boots_item->armor;
        player->equip_bonus_hp -= boots_item->hp;
        player->equip_bonus_tp -= boots_item->tp;
        player->equip_strength_bonus -= boots_item->strength;
        player->equip_wisdom_bonus -= boots_item->wisdom;
        player->equip_intelligence_bonus -= boots_item->intelligence;
        player->equip_agility_bonus -= boots_item->agility;
        player->equip_constitution_bonus -= boots_item->constitution;
        player->equip_charisma_bonus -= boots_item->charisma;
        player->boots_item_id = 0;
        player->boots_graphic_id = 0;
        changed = 1;
    }
    if (GUI->item_values->GetSpecial(player->gloves_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *gloves_item = GUI->item_values->GetByIndex(player->gloves_item_id - 1);
        if (gloves_item->element < ELEMENT_COUNT)
            player->element_resistances[gloves_item->element] += gloves_item->element_damage;
        player->min_damage -= gloves_item->min_damage;
        player->max_damage -= gloves_item->max_damage;
        player->accuracy -= gloves_item->accuracy;
        player->evasion -= gloves_item->evade;
        player->armor -= gloves_item->armor;
        player->equip_bonus_hp -= gloves_item->hp;
        player->equip_bonus_tp -= gloves_item->tp;
        player->equip_strength_bonus -= gloves_item->strength;
        player->equip_wisdom_bonus -= gloves_item->wisdom;
        player->equip_intelligence_bonus -= gloves_item->intelligence;
        player->equip_agility_bonus -= gloves_item->agility;
        player->equip_constitution_bonus -= gloves_item->constitution;
        player->equip_charisma_bonus -= gloves_item->charisma;
        player->gloves_item_id = 0;
        player->gloves_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->accessory_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *accessory_item = GUI->item_values->GetByIndex(player->accessory_item_id - 1);
        if (accessory_item->element < ELEMENT_COUNT)
            player->element_resistances[accessory_item->element] += accessory_item->element_damage;
        player->min_damage -= accessory_item->min_damage;
        player->max_damage -= accessory_item->max_damage;
        player->accuracy -= accessory_item->accuracy;
        player->evasion -= accessory_item->evade;
        player->armor -= accessory_item->armor;
        player->equip_bonus_hp -= accessory_item->hp;
        player->equip_bonus_tp -= accessory_item->tp;
        player->equip_strength_bonus -= accessory_item->strength;
        player->equip_wisdom_bonus -= accessory_item->wisdom;
        player->equip_intelligence_bonus -= accessory_item->intelligence;
        player->equip_agility_bonus -= accessory_item->agility;
        player->equip_constitution_bonus -= accessory_item->constitution;
        player->equip_charisma_bonus -= accessory_item->charisma;
        player->accessory_item_id = 0;
        player->accessory_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->belt_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *belt_item = GUI->item_values->GetByIndex(player->belt_item_id - 1);
        if (belt_item->element < ELEMENT_COUNT)
            player->element_resistances[belt_item->element] += belt_item->element_damage;
        player->min_damage -= belt_item->min_damage;
        player->max_damage -= belt_item->max_damage;
        player->accuracy -= belt_item->accuracy;
        player->evasion -= belt_item->evade;
        player->armor -= belt_item->armor;
        player->equip_bonus_hp -= belt_item->hp;
        player->equip_bonus_tp -= belt_item->tp;
        player->equip_strength_bonus -= belt_item->strength;
        player->equip_wisdom_bonus -= belt_item->wisdom;
        player->equip_intelligence_bonus -= belt_item->intelligence;
        player->equip_agility_bonus -= belt_item->agility;
        player->equip_constitution_bonus -= belt_item->constitution;
        player->equip_charisma_bonus -= belt_item->charisma;
        player->belt_item_id = 0;
        player->belt_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->necklace_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *necklace_item = GUI->item_values->GetByIndex(player->necklace_item_id - 1);
        if (necklace_item->element < ELEMENT_COUNT)
            player->element_resistances[necklace_item->element] += necklace_item->element_damage;
        player->min_damage -= necklace_item->min_damage;
        player->max_damage -= necklace_item->max_damage;
        player->accuracy -= necklace_item->accuracy;
        player->evasion -= necklace_item->evade;
        player->armor -= necklace_item->armor;
        player->equip_bonus_hp -= necklace_item->hp;
        player->equip_bonus_tp -= necklace_item->tp;
        player->equip_strength_bonus -= necklace_item->strength;
        player->equip_wisdom_bonus -= necklace_item->wisdom;
        player->equip_intelligence_bonus -= necklace_item->intelligence;
        player->equip_agility_bonus -= necklace_item->agility;
        player->equip_constitution_bonus -= necklace_item->constitution;
        player->equip_charisma_bonus -= necklace_item->charisma;
        player->necklace_item_id = 0;
        player->necklace_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->ring1_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *ring1_item = GUI->item_values->GetByIndex(player->ring1_item_id - 1);
        if (ring1_item->element < ELEMENT_COUNT)
            player->element_resistances[ring1_item->element] += ring1_item->element_damage;
        player->min_damage -= ring1_item->min_damage;
        player->max_damage -= ring1_item->max_damage;
        player->accuracy -= ring1_item->accuracy;
        player->evasion -= ring1_item->evade;
        player->armor -= ring1_item->armor;
        player->equip_bonus_hp -= ring1_item->hp;
        player->equip_bonus_tp -= ring1_item->tp;
        player->equip_strength_bonus -= ring1_item->strength;
        player->equip_wisdom_bonus -= ring1_item->wisdom;
        player->equip_intelligence_bonus -= ring1_item->intelligence;
        player->equip_agility_bonus -= ring1_item->agility;
        player->equip_constitution_bonus -= ring1_item->constitution;
        player->equip_charisma_bonus -= ring1_item->charisma;
        player->ring1_item_id = 0;
        player->ring1_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->ring2_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *ring2_item = GUI->item_values->GetByIndex(player->ring2_item_id - 1);
        if (ring2_item->element < ELEMENT_COUNT)
            player->element_resistances[ring2_item->element] += ring2_item->element_damage;
        player->min_damage -= ring2_item->min_damage;
        player->max_damage -= ring2_item->max_damage;
        player->accuracy -= ring2_item->accuracy;
        player->evasion -= ring2_item->evade;
        player->armor -= ring2_item->armor;
        player->equip_bonus_hp -= ring2_item->hp;
        player->equip_bonus_tp -= ring2_item->tp;
        player->equip_strength_bonus -= ring2_item->strength;
        player->equip_wisdom_bonus -= ring2_item->wisdom;
        player->equip_intelligence_bonus -= ring2_item->intelligence;
        player->equip_agility_bonus -= ring2_item->agility;
        player->equip_constitution_bonus -= ring2_item->constitution;
        player->equip_charisma_bonus -= ring2_item->charisma;
        player->ring2_item_id = 0;
        player->ring2_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->armlet1_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *armlet1_item = GUI->item_values->GetByIndex(player->armlet1_item_id - 1);
        if (armlet1_item->element < ELEMENT_COUNT)
            player->element_resistances[armlet1_item->element] += armlet1_item->element_damage;
        player->min_damage -= armlet1_item->min_damage;
        player->max_damage -= armlet1_item->max_damage;
        player->accuracy -= armlet1_item->accuracy;
        player->evasion -= armlet1_item->evade;
        player->armor -= armlet1_item->armor;
        player->equip_bonus_hp -= armlet1_item->hp;
        player->equip_bonus_tp -= armlet1_item->tp;
        player->equip_strength_bonus -= armlet1_item->strength;
        player->equip_wisdom_bonus -= armlet1_item->wisdom;
        player->equip_intelligence_bonus -= armlet1_item->intelligence;
        player->equip_agility_bonus -= armlet1_item->agility;
        player->equip_constitution_bonus -= armlet1_item->constitution;
        player->equip_charisma_bonus -= armlet1_item->charisma;
        player->armlet1_item_id = 0;
        player->armlet1_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->armlet2_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *armlet2_item = GUI->item_values->GetByIndex(player->armlet2_item_id - 1);
        if (armlet2_item->element < ELEMENT_COUNT)
            player->element_resistances[armlet2_item->element] += armlet2_item->element_damage;
        player->min_damage -= armlet2_item->min_damage;
        player->max_damage -= armlet2_item->max_damage;
        player->accuracy -= armlet2_item->accuracy;
        player->evasion -= armlet2_item->evade;
        player->armor -= armlet2_item->armor;
        player->equip_bonus_hp -= armlet2_item->hp;
        player->equip_bonus_tp -= armlet2_item->tp;
        player->equip_strength_bonus -= armlet2_item->strength;
        player->equip_wisdom_bonus -= armlet2_item->wisdom;
        player->equip_intelligence_bonus -= armlet2_item->intelligence;
        player->equip_agility_bonus -= armlet2_item->agility;
        player->equip_constitution_bonus -= armlet2_item->constitution;
        player->equip_charisma_bonus -= armlet2_item->charisma;
        player->armlet2_item_id = 0;
        player->armlet2_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->bracer1_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *bracer1_item = GUI->item_values->GetByIndex(player->bracer1_item_id - 1);
        if (bracer1_item->element < ELEMENT_COUNT)
            player->element_resistances[bracer1_item->element] += bracer1_item->element_damage;
        player->min_damage -= bracer1_item->min_damage;
        player->max_damage -= bracer1_item->max_damage;
        player->accuracy -= bracer1_item->accuracy;
        player->evasion -= bracer1_item->evade;
        player->armor -= bracer1_item->armor;
        player->equip_bonus_hp -= bracer1_item->hp;
        player->equip_bonus_tp -= bracer1_item->tp;
        player->equip_strength_bonus -= bracer1_item->strength;
        player->equip_wisdom_bonus -= bracer1_item->wisdom;
        player->equip_intelligence_bonus -= bracer1_item->intelligence;
        player->equip_agility_bonus -= bracer1_item->agility;
        player->equip_constitution_bonus -= bracer1_item->constitution;
        player->equip_charisma_bonus -= bracer1_item->charisma;
        player->bracer1_item_id = 0;
        player->bracer1_graphic_id = 0;
    }
    if (GUI->item_values->GetSpecial(player->bracer2_item_id) == ItemSpecial_Cursed)
    {
        ItemValue *bracer2_item = GUI->item_values->GetByIndex(player->bracer2_item_id - 1);
        if (bracer2_item->element < ELEMENT_COUNT)
            player->element_resistances[bracer2_item->element] += bracer2_item->element_damage;
        player->min_damage -= bracer2_item->min_damage;
        player->max_damage -= bracer2_item->max_damage;
        player->accuracy -= bracer2_item->accuracy;
        player->evasion -= bracer2_item->evade;
        player->armor -= bracer2_item->armor;
        player->equip_bonus_hp -= bracer2_item->hp;
        player->equip_bonus_tp -= bracer2_item->tp;
        player->equip_strength_bonus -= bracer2_item->strength;
        player->equip_wisdom_bonus -= bracer2_item->wisdom;
        player->equip_intelligence_bonus -= bracer2_item->intelligence;
        player->equip_agility_bonus -= bracer2_item->agility;
        player->equip_constitution_bonus -= bracer2_item->constitution;
        player->equip_charisma_bonus -= bracer2_item->charisma;
        player->bracer2_item_id = 0;
        player->bracer2_graphic_id = 0;
    }
    return changed;
}

// The players on the map still in the arena's fight.
int Players::CountArenaPlayers(int map_id)
{
    int count = 0;
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->map_id == map_id && (*it)->in_arena)
            count++;
    }
    return count;
}

// The players this player has invited to their new guild.
int Players::CountGuildInvites(Player *player)
{
    int count = 0;
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->guild_inviter_id == player->player_id && (*it)->player_id != player->player_id)
            count++;
    }
    return count;
}

// The other players on the map with no guild.
int Players::CountGuildlessOnMap(Player *player)
{
    int count;
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->map_id == player->map_id && (*it)->guild_tag.Length() < GUILD_TAG_MIN_LENGTH &&
            (*it)->player_id != player->player_id)
            count++;
    }
    return count;
}

// The new guild's invited players join it as new members ( 10 at most ).
void Players::GuildSetMemberInfo(Player *player, String guild_name, String guild_tag)
{
    int count;
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->guild_inviter_id == player->player_id && (*it)->player_id != player->player_id)
        {
            (*it)->guild_tag = guild_tag;
            (*it)->guild_name = guild_name;
            (*it)->guild_rank_name = "";
            (*it)->guild_rank_id = GUILD_RANK_NEW_MEMBER;
            (*it)->guild_inviter_id = -1;
            count++;
            if (count > 10)
                return;
        }
    }
}

// A player joins the party: every member adds them, and they take the party's roster and leader.
void Players::Party_AddNewMember(Player *existing_member, Player *new_member)
{
    for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        Player *player = GetById(existing_member->party_ids[i]);
        if (player)
            player->AddPartyMember(new_member->player_id);
    }
    new_member->party_leader_id = existing_member->party_leader_id;
    new_member->ClearPartyRoster();
    new_member->in_party = true;
    for (int member_slot = 0; member_slot < PARTY_MAX_MEMBERS; member_slot++)
        new_member->AddPartyMember(existing_member->party_ids[member_slot]);
}

// A player leaves the party; when they led it, or only one would be left, the party ends.
void Players::Player_LeaveParty(Player *player)
{
    char disband = 0;
    player->in_party = false;
    if (player->player_id == player->party_leader_id || player->CountPartyMembers() < 3)
        disband = 1;
    for (int i = 0; i < PARTY_MAX_MEMBERS; i++)
    {
        if (player->party_ids[i] == player->player_id)
            continue;
        Player *member = GetById(player->party_ids[i]);
        if (member)
        {
            if (disband)
            {
                member->ClearPartyRoster();
                member->in_party = false;
            }
            else
            {
                for (int j = 0; j < PARTY_MAX_MEMBERS; j++)
                {
                    if (player->player_id == member->party_ids[j])
                        member->party_ids[j] = -1;
                }
            }
        }
    }
    player->ClearPartyRoster();
    player->in_party = false;
}

// Refills the chat, drop and global budgets and heals a tenth of the max hp and tp, a fifth when sitting.
void Players::Player_RegenHpTp(Player *player)
{
    int regen;
    player->talk_tokens = TALK_TOKEN_BUDGET;
    player->drop_counter = DROP_TOKEN_BUDGET;
    player->global_tokens = GLOBAL_TOKEN_BUDGET;
    if (player->sitting)
    {
        regen = player->max_hp / 5;
        regen++;
    }
    else
    {
        regen = player->max_hp / 10;
        regen++;
    }
    player->hp += regen;
    if (player->hp > player->max_hp)
        player->hp = player->max_hp;
    if (player->sitting)
    {
        regen = player->max_tp / 5;
        regen++;
    }
    else
    {
        regen = player->max_tp / 10;
        regen++;
    }
    player->tp += regen;
    if (player->tp > player->max_tp)
        player->tp = player->max_tp;
}

// The player by id ( their socket handle ), or 0.
Player * Players::GetById(int player_id)
{
    if (player_id > 0 && player_id < MAX_PLAYER_ID)
        return by_id[player_id];
    else
        return 0;
}

// The player by name, in any case, or 0.
Player * Players::FindByName(String name)
{
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (LowerCase((*it)->name) == LowerCase(name))
            return *it;
    }
    return 0;
}

// The player on the tile, or 0.
Player * Players::GetByMapTile(int map_id, int x, int y)
{
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->map_id == map_id && (*it)->x == x && (*it)->y == y)
            return *it;
    }
    return 0;
}

// Levels the player up while their experience is enough; the new level, or 0 when it did not change.
int Players::Player_TryLevelUp(Player *player)
{
    int new_level = 0;
    while (GUI->game_control->Exp_RequiredForLevel(player->level + 1) <= player->experience)
    {
        Player_LevelUp(player);
        new_level = player->level;
    }
    return new_level;
}

// One level: 3 stat and 4 skill points, more base hp, tp and sp ( more at higher levels ).
void Players::Player_LevelUp(Player *player)
{
    player->level++;
    player->stat_points += 3;
    player->skill_points += 4;
    int hp_bonus = player->level / 10;
    player->base_hp += 2;
    player->base_hp += random(2);
    player->base_hp += hp_bonus;
    player->base_tp += 2;
    player->base_tp += random(2);
    int sp_bonus = player->level / 20;
    player->base_sp++;
    player->base_sp += sp_bonus;
    if (player->base_hp > 64000)
        player->base_hp = 64000;
    if (player->base_tp > 64000)
        player->base_tp = 64000;
    if (player->base_sp > 64000)
        player->base_sp = 64000;
    player->CalculateHP_TP_SP();
}

int Players::GetActiveCount()
{
    return players.size();
}

int Players::GetOnlineCount()
{
    return online_count;
}

int Players::GetPeakOnline()
{
    return peak_online;
}

void Players::MarkDirty()
{
    dirty = true;
}

// Counts the characters in the game, and keeps the highest count.
void Players::UpdatePeakOnline()
{
    online_count = 0;
    for (vector<Player *>::iterator it = players.begin(); it != players.end(); it++)
    {
        if ((*it)->logged_in)
            online_count++;
    }
    if (online_count > peak_online)
        peak_online = online_count;
}
