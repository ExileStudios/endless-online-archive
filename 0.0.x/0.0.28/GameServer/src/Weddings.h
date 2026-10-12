#ifndef WeddingsH
#define WeddingsH

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

struct Packets;
struct Players;
struct Wedding;

#include "Wedding.h"

struct WeddingController;

// The weddings in progress, one a priest; each ceremony runs as a script on a countdown.
struct WeddingController
{
    char *encode_scratch;
    Players *players;
    Packets *server;
    vector<Wedding *> active_weddings;

    WeddingController(Players *players, Packets *server);
    ~WeddingController();
    char Has(int map_id, int priest_npc_index);
    void Add(int map_id, int priest_npc_index, int player1_id, String player1_name, int player2_id,
        String player2_name);
    void Confirm(int map_id, int priest_npc_index, int player_id);
    void Tick();
    void BroadcastPriestLine(Wedding *record, String text);
    bool BothPresent(Wedding *record);
    String EncodeNumber(unsigned int value, signed int width);
};


#endif
