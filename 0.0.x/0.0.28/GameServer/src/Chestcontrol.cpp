#include <vcl.h>
#pragma hdrstop

#include "Chestcontrol.h"
#include "Itemchest.h"
#include "Map.h"
#include "Mapchest.h"
#include "Mapcontrol.h"
#include "Packets.h"
#include "Player.h"
#include "Players.h"
#include "Settings.h"

#pragma package(smart_init)

#include <stdlib.h>
#include "Constants.h"

ChestController::ChestController(MapContainer *map_control, Players *players, Packets *server, Settings *settings)
{
    this->encode_scratch = new char[8];
    this->map_control = map_control;
    this->settings = settings;
    this->players = players;
    this->server = server;
}

ChestController::~ChestController()
{
}

// Refills the emptied chest slots whose time is up, and shows the players next to a changed chest its contents.
void ChestController::Tick()
{
    for (vector<MapItem>::iterator map = map_control->maps.begin(); map != map_control->maps.end(); map++)
    {
        map->chests_dirty = false;
        for (vector<MapChest>::iterator chest = map->chest_list.begin(); chest != map->chest_list.end(); chest++)
        {
            for (vector<ChestItem>::iterator item = chest->slots.begin(); item != chest->slots.end(); item++)
            {
                if (!item->respawn_enabled)
                    continue;
                if (!item->item_present)
                {
                    item->respawn_countdown--;
                    if (item->respawn_countdown <= 0)
                    {
                        map->chests_dirty = true;
                        chest->updated = true;
                        item->item_present = true;
                        item->respawn_countdown = item->respawn_delay;
                        // .. the refill is one of up to four alternatives, picked at random
                        int max_items = 1;
                        if (max_items == 1 && item->alt_item_id1 > 0)
                            max_items++;
                        if (max_items == 2 && item->alt_item_id2 > 0)
                            max_items++;
                        if (max_items == 3 && item->alt_item_id3 > 0)
                            max_items++;
                        int item_index = random(max_items);
                        if (!item_index)
                        {
                            item->item_id = item->alt_item_id0;
                            item->amount = item->alt_amount0;
                        }
                        if (item_index == 1)
                        {
                            item->item_id = item->alt_item_id1;
                            item->amount = item->alt_amount1;
                        }
                        if (item_index == 2)
                        {
                            item->item_id = item->alt_item_id2;
                            item->amount = item->alt_amount2;
                        }
                        if (item_index == 3)
                        {
                            item->item_id = item->alt_item_id3;
                            item->amount = item->alt_amount3;
                        }
                    }
                }
            }
        }
    }
    for (vector<Player *>::iterator it = players->players.begin(); it != players->players.end(); it++)
    {
        if (!(*it)->logged_in)
            continue;
        if ((*it)->map_id <= 0)
            continue;
        if ((int)map_control->maps.size() < (*it)->map_id)
            continue;
        if (!map_control->maps[(*it)->map_id - 1].chests_dirty)
            continue;
        try
        {
            String packet = "";
            for (vector<MapChest>::iterator map_chest = map_control->maps[(*it)->map_id - 1].chest_list.begin();
                map_chest != map_control->maps[(*it)->map_id - 1].chest_list.end(); map_chest++)
            {
                if (!map_chest->updated)
                    continue;
                map_chest->updated = false;
                if (InRange(map_chest->x, map_chest->y, (*it)->x, (*it)->y))
                {
                    packet = "";
                    vector<ChestItem>::iterator chest_slot;
                    for (chest_slot = map_chest->slots.begin(); chest_slot != map_chest->slots.end(); chest_slot++)
                    {
                        if (chest_slot->item_present)
                        {
                            packet.Insert(EncodeNumber(chest_slot->item_id, 2), packet.Length() + 1);
                            packet.Insert(EncodeNumber(chest_slot->amount, 3), packet.Length() + 1);
                        }
                    }
                    server->Client_SendEncoded(*it, AC_UPDATE, OP_CHEST, packet);
                }
            }
        }
        catch (...)
        {
        }
    }
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(ChestController)

// True when the player stands on the chest or next to it, not diagonally.
char ChestController::InRange(int x, int y, int player_x, int player_y)
{
    char result = 0;
    int dx = player_x - x;
    int dy = player_y - y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 1)
        result = 1;
    return result;
}
