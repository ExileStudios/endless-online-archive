#ifndef EffectcontrolH
#define EffectcontrolH

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


struct EffectController;

// The map effects: quakes, hp and tp drain, and spikes.
struct EffectController
{
    int quake_countdowns[4];
    int quake_elapsed[4];
    int quake_strengths[4];
    int broadcast_gate;
    char *encode_scratch;
    Settings *settings;
    MapContainer *map_control;
    Players *players;
    Packets *server;

    EffectController(MapContainer *map_control, Players *players, Packets *server, Settings *settings);
    ~EffectController();
    void Tick();
    String EncodeNumber(unsigned int value, signed int width);
};


#endif
