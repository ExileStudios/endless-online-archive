#include <vcl.h>
#pragma hdrstop

#include "Map.h"

#pragma package(smart_init)

MapItem::MapItem(int map_id, int width, int height)
{
    this->map_id = map_id;
    this->width = width;
    this->height = height;
    this->field_0x4c = 0;
    this->buf_copied = false;
    this->npc_dirty = false;
    this->boss_alive = false;
    this->chests_dirty = false;
    this->has_open_doors = false;
    this->no_scroll = false;
    this->arena_enabled = false;
    this->child_npc_id = 0;
    this->player_count = 0;
    this->npc_act_ticks = 0;
    this->next_ground_item_id = 0;
    this->quest_cooldown = 0;
    this->evac_countdown = 0;
    this->has_quakes = false;
    this->has_hp_drain = false;
    this->has_tp_drain = false;
    this->has_spikes = false;
    this->relog_x = 0;
    this->relog_y = 0;
    this->hp_drain_others_sent = false;
    this->hp_drain_others = "";
    this->tile_bits.resize(this->width * this->height * 2, false);
    this->legacy_door_key_list.clear();
    this->chest_list.clear();
}

MapItem::~MapItem()
{
}
