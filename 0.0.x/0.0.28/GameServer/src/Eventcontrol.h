#ifndef EventcontrolH
#define EventcontrolH

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


struct EventController;

// The map events: the evacuations and the arenas.
struct EventController
{
    char *encode_scratch;
    Settings *settings;
    MapContainer *map_control;
    Players *players;
    Packets *server;

    EventController(MapContainer *map_control, Players *players, Packets *server, Settings *settings);
    ~EventController();
    void Tick();
    String EncodeNumber(unsigned int value, signed int width);
};


#endif
