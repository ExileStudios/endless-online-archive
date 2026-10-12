#include <vcl.h>
#pragma hdrstop

#include "Effectcontrol.h"
#include "Mapcontrol.h"
#include "Packets.h"
#include "Player.h"
#include "Players.h"
#include "Settings.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

EffectController::EffectController(MapContainer *map_control, Players *players, Packets *server, Settings *settings)
{
    this->encode_scratch = new char[8];
    this->map_control = map_control;
    this->settings = settings;
    this->players = players;
    this->server = server;
    for (int i = 0; i < 4; i++)
    {
        this->quake_countdowns[i] = 10; this->quake_elapsed[i] = 0; this->quake_strengths[i] = 1;
    }
    this->broadcast_gate = 10;
}

EffectController::~EffectController()
{
}

// Once a second: the quakes, the spike damage, and every 10 seconds the hp and tp drain.
void EffectController::Tick()
{
    for (int countdown_slot = 0; countdown_slot < 4; countdown_slot++)
        quake_countdowns[countdown_slot]--;
    broadcast_gate--;
    for (vector<Player *>::iterator player_iter = players->players.begin(); player_iter != players->players.end();
        player_iter++)
    {
        if ((*player_iter)->map_has_quakes)
        {
            int quake_level = map_control->maps[(*player_iter)->map_id - 1].timed_effect - 3;
            if (quake_level < 0 || quake_level > 3)
                continue;
            if (quake_countdowns[quake_level] < 1)
            {
                String quake_packet = EncodeNumber(1, 1);
                quake_packet.Insert(EncodeNumber(quake_strengths[quake_level], 1), quake_packet.Length() + 1);
                server->Client_SendEncoded(*player_iter, AC_DO, OP_EFFECT, quake_packet);
            }
        }
        if (broadcast_gate < 1)
        {
            if ((*player_iter)->map_has_hp_drain && (*player_iter)->hp > 0)
            {
                // .. hp drain: a tenth of the max hp, never the last point
                int hp_drain = (*player_iter)->max_hp / 10;
                if (hp_drain < 1)
                    hp_drain = 1;
                if ((*player_iter)->hp <= hp_drain)
                    hp_drain = (*player_iter)->hp - 1;
                (*player_iter)->hp -= hp_drain;
                (*player_iter)->item_change_count = hp_drain;
                if (map_control->maps[(*player_iter)->map_id - 1].hp_drain_others_sent)
                {
                    map_control->maps[(*player_iter)->map_id - 1].hp_drain_others = "";
                    map_control->maps[(*player_iter)->map_id - 1].hp_drain_others_sent = false;
                }
                map_control->maps[(*player_iter)->map_id - 1].hp_drain_others.Insert(EncodeNumber((*player_iter)->player_id,
                    2), map_control->maps[(*player_iter)->map_id - 1].hp_drain_others.Length() + 1);
                map_control->maps[(*player_iter)->map_id - 1].hp_drain_others.Insert(EncodeNumber((*player_iter)->HpPercent(),
                    1), map_control->maps[(*player_iter)->map_id - 1].hp_drain_others.Length() + 1);
                map_control->maps[(*player_iter)->map_id - 1].hp_drain_others.Insert(EncodeNumber(hp_drain, 2),
                    map_control->maps[(*player_iter)->map_id - 1].hp_drain_others.Length() + 1);
            }
            if ((*player_iter)->map_has_tp_drain && (*player_iter)->tp > 0)
            {
                // .. tp drain: a tenth of the max tp, never the last point
                int tp_drain = (*player_iter)->max_tp / 10;
                if (tp_drain < 1)
                    tp_drain = 1;
                if ((*player_iter)->tp <= tp_drain)
                    tp_drain = (*player_iter)->tp - 1;
                (*player_iter)->tp -= tp_drain;
                String pkt = EncodeNumber(MapDamageType_TpDrain, 1);
                pkt.Insert(EncodeNumber(tp_drain, 2), pkt.Length() + 1);
                pkt.Insert(EncodeNumber((*player_iter)->tp, 2), pkt.Length() + 1);
                pkt.Insert(EncodeNumber((*player_iter)->max_tp, 2), pkt.Length() + 1);
                server->Client_SendEncoded(*player_iter, AC_HIT, OP_EFFECT, pkt);
            }
        }
        if ((*player_iter)->map_has_spikes)
        {
            int spec = map_control->GetTileSpec((*player_iter)->map_id, (*player_iter)->x, (*player_iter)->y);
            // .. the spike tiles: a fifth of the max hp each second
            if (spec == 33 || spec == 34)
            {
                if ((*player_iter)->hp > 0)
                {
                    int dmg = (*player_iter)->max_hp / 5;
                    unsigned int died = 0;
                    if (dmg < 1)
                        dmg = 1;
                    if ((*player_iter)->hp < dmg)
                        dmg = (*player_iter)->hp;
                    (*player_iter)->hp -= dmg;
                    if ((*player_iter)->hp <= 0)
                    {
                        (*player_iter)->hp = 0;
                        died = 1;
                    }
                    String spike_packet = EncodeNumber(MapDamageType_Spikes, 1);
                    spike_packet.Insert(EncodeNumber(dmg, 2), spike_packet.Length() + 1);
                    spike_packet.Insert(EncodeNumber((*player_iter)->hp, 2), spike_packet.Length() + 1);
                    spike_packet.Insert(EncodeNumber((*player_iter)->max_hp, 2), spike_packet.Length() + 1);
                    server->Client_SendEncoded(*player_iter, AC_HIT, OP_EFFECT, spike_packet);
                    spike_packet = EncodeNumber((*player_iter)->player_id, 2);
                    spike_packet.Insert(EncodeNumber((*player_iter)->HpPercent(), 1), spike_packet.Length() + 1);
                    spike_packet.Insert(EncodeNumber(died, 1), spike_packet.Length() + 1);
                    spike_packet.Insert(EncodeNumber(dmg, 2), spike_packet.Length() + 1);
                    server->Server_BroadcastNearby(*player_iter, AC_CAST, OP_EFFECT, spike_packet);
                    if (died)
                        server->Player_Respawn(*player_iter);
                }
            }
            else
                server->Client_SendEncoded(*player_iter, AC_PUBLIC, OP_EFFECT, "S");
        }
    }
    if (broadcast_gate < 1)
    {
        for (vector<Player *>::iterator broadcast_iter = players->players.begin();
            broadcast_iter != players->players.end(); broadcast_iter++)
        {
            if (!(*broadcast_iter)->map_has_hp_drain)
                continue;
            map_control->maps[(*broadcast_iter)->map_id - 1].hp_drain_others_sent = true;
            String pkt = EncodeNumber((*broadcast_iter)->item_change_count, 2);
            pkt.Insert(EncodeNumber((*broadcast_iter)->hp, 2), pkt.Length() + 1);
            pkt.Insert(EncodeNumber((*broadcast_iter)->max_hp, 2), pkt.Length() + 1);
            pkt.Insert(map_control->maps[(*broadcast_iter)->map_id - 1].hp_drain_others, pkt.Length() + 1);
            server->Client_SendEncoded(*broadcast_iter, AC_TARGET, OP_EFFECT, pkt);
        }
    }
    // .. a quake level that went off gets a new random countdown and strength
    for (int reroll_slot = 0; reroll_slot < 4; reroll_slot++)
    {
        if (quake_countdowns[reroll_slot] >= 1)
            continue;
        int base = 4 - reroll_slot;
        quake_countdowns[reroll_slot] = random(50 - 10 * (reroll_slot + 1)) + (base << 2) + 2;
        quake_elapsed[reroll_slot] = quake_elapsed[reroll_slot] + quake_countdowns[reroll_slot];
        if (280 - 40 * reroll_slot < quake_elapsed[reroll_slot])
        {
            int roll = random(7);
            if (roll > 4 || quake_elapsed[reroll_slot] > 600)
            {
                quake_countdowns[reroll_slot] = base + 3; quake_elapsed[reroll_slot] = 0;
            }
        }
        quake_strengths[reroll_slot] = 2 * reroll_slot + random(3);
        if (reroll_slot == 1)
            quake_strengths[reroll_slot] -= 2;
    }
    if (broadcast_gate < 1)
        broadcast_gate = 10;
}

DEFINE_ENCODE_NUMBER(EffectController)
