#include <vcl.h>
#pragma hdrstop

#include "Player.h"

#pragma package(smart_init)

#include <stdlib.h>

// A new connection: its own encryption multiples and session, the chat, drop and attack budgets full.
Player::Player(Scktcomp::TCustomWinSocket *socket)
{
    this->socket = socket;
    this->player_id = socket->SocketHandle;
    this->remote_ip = socket->RemoteAddress;
    this->character_slots[0] = 0;
    this->character_slots[1] = 0;
    this->character_slots[2] = 0;
    this->in_party = false;
    this->hidden = false;
    this->hide_online = false;
    this->ClearPartyRoster();
    this->initialized = false;
    this->removing = false;
    this->connected = false;
    this->account_logged_in = false;
    this->logged_in = false;
    this->map_switch_pending = false;
    this->arena_entering = false;
    this->in_arena = false;
    this->global_chat = false;
    this->admin_level = AdminLevel_Player;
    this->account_create_cooldown = 0;
    this->field_0x2c = 0;
    // the multiples are 5 to 12
    this->server_encryption_multiple = random(8) + 5;
    this->client_encryption_multiple = random(8) + 5;
    this->sequence = 0;
    this->query_id = random(10000000) + 10;
    this->session_id = random(50000) + 10000;
    this->receive_buffer = "";
    this->ping_timeout = 0;
    this->packet_count = 0;
    this->account_ident = -1;
    this->character_id = -1;
    this->idle_ticks = 0;
    this->map_id = 0;
    this->stat_points = 0;
    this->skill_points = 0;
    this->save_ticks = 0;
    this->recover_ticks = 0;
    this->field_0xa4 = false;
    this->whispers_disabled = false;
    this->party_requested_id = -1;
    this->trade_partner_id = -1;
    this->session_token = -1;
    this->trade_accepted = false;
    this->stats_dirty = false;
    this->dead = false;
    this->bank_dirty = false;
    this->inventory_dirty = false;
    this->equipment_dirty = false;
    this->base_stats_dirty = false;
    this->map_has_quakes = 0;
    this->map_has_spikes = 0;
    this->map_has_hp_drain = 0;
    this->map_has_tp_drain = 0;
    this->last_pass_ms = DateTimeToTimeStamp(Now());
    this->talk_tokens = TALK_TOKEN_BUDGET;
    this->drop_counter = DROP_TOKEN_BUDGET;
    this->global_tokens = GLOBAL_TOKEN_BUDGET;
    this->attack_tokens = 40;
    this->attack_token_ticks = 0;
    this->ghost_walk_tokens = 0;
    this->ghost_token_ticks = 0;
    this->last_client_tick = 9000000;
    this->sync_base_ahead = -1;
    this->sync_base_behind = -1;
    this->pending_char_name = "";
    this->flush_queue = false;
    this->cheater_flag = false;
}

Player::~Player()
{
}

// The hp as a percent, 0 to 100.
int Player::HpPercent()
{
    if (max_hp < 1)
        return 0;
    double percent = (double)hp / (double)max_hp * 100.0;
    if (percent < 1.0)
        return 0;
    if (percent > 100.0)
        return 100;
    return percent;
}

// The members in the party roster ( its first 9 slots ).
int Player::CountPartyMembers()
{
    int count = 0;
    for (int i = 0; i < 9; i++)
    {
        if (party_ids[i] > -1)
            count++;
    }
    return count;
}

// Adds a member to the first free slot of the roster, once.
void Player::AddPartyMember(int member_id)
{
    for (int i = 0; i < 9; i++)
    {
        if (party_ids[i] == member_id)
            break;
        if (party_ids[i] < 0)
        {
            party_ids[i] = member_id;
            return;
        }
    }
}

char Player::IsPartyMember(int player_id)
{
    for (int i = 0; i < 9; i++)
    {
        if (party_ids[i] == player_id)
            return 1;
    }
    return 0;
}

void Player::ClearPartyRoster()
{
    party_ids[0] = -1;
    party_ids[1] = -1;
    party_ids[2] = -1;
    party_ids[3] = -1;
    party_ids[4] = -1;
    party_ids[5] = -1;
    party_ids[6] = -1;
    party_ids[7] = -1;
    party_ids[8] = -1;
}

// The stats with the equipment's bonuses added.
void Player::CalculateAdjStats()
{
    adj_strength = base_strength + equip_strength_bonus;
    adj_wisdom = base_wisdom + equip_wisdom_bonus;
    adj_intelligence = base_intelligence + equip_intelligence_bonus;
    adj_agility = base_agility + equip_agility_bonus;
    adj_constitution = base_constitution + equip_constitution_bonus;
    adj_charisma = base_charisma + equip_charisma_bonus;
}

// The max hp, tp and sp from the base values, the level, the stats and the equipment; each at most 64000.
void Player::CalculateHP_TP_SP()
{
    max_hp = base_hp;
    max_tp = base_tp;
    max_sp = base_sp;
    double level_mult = (level + 10) / 10;  // an int division: the level counts in steps of 10
    double con_bonus = adj_constitution * 1.25;
    double int_bonus = adj_intelligence * 1.25;
    double wis_bonus = adj_wisdom * 0.75;
    double sp_base_bonus = (adj_constitution + 1) / 3;
    double sp_extra_bonus = (adj_constitution + 1) / 3;
    max_hp = max_hp + level_mult * con_bonus;
    max_tp = max_tp + level_mult * int_bonus;
    max_tp = max_tp + level_mult * wis_bonus;
    max_sp = max_sp + level_mult * sp_base_bonus;
    max_sp = max_sp + level_mult * sp_extra_bonus;
    max_hp = max_hp + equip_bonus_hp;
    max_tp = max_tp + equip_bonus_tp;
    weight_max = adj_strength + 70;
    if (weight_max > WEIGHT_DISPLAY_MAX)
        weight_max = WEIGHT_DISPLAY_MAX;
    if (max_hp > 64000)
        max_hp = 64000;
    if (max_tp > 64000)
        max_tp = 64000;
    if (max_sp > 64000)
        max_sp = 64000;
}
