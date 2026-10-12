#ifndef ChestcontrolH
#define ChestcontrolH

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

struct MapContainer;
struct Packets;
struct Players;
struct Settings;


struct ChestController;

// The chests: refills their slots and shows the change to the players standing at them.
struct ChestController
{
    char *encode_scratch;
    Settings *settings;
    MapContainer *map_control;
    Players *players;
    Packets *server;

    ChestController(MapContainer *map_control, Players *players, Packets *server, Settings *settings);
    ~ChestController();
    void Tick();
    String EncodeNumber(unsigned int value, signed int width);
    char InRange(int x, int y, int player_x, int player_y);
};


#endif
