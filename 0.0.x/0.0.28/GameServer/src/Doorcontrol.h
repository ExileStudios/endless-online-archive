#ifndef DoorcontrolH
#define DoorcontrolH

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


struct DoorController;

// Closes the open doors again.
struct DoorController
{
    TTimeStamp last_tick;
    MapContainer *map_control;

    DoorController(MapContainer *map_control);
    ~DoorController();
    void Tick();
};


#endif
