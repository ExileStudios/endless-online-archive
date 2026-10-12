#include <vcl.h>
#pragma hdrstop

#include "Doorcontrol.h"
#include "Map.h"
#include "Mapcontrol.h"
#include "Mapobject.h"
#include "Constants.h"

#pragma package(smart_init)

DoorController::DoorController(MapContainer *map_control)
{
    this->map_control = map_control;
}

DoorController::~DoorController()
{
}

// Counts down every open door and closes the ones that ran out; after a gap of more than a second all close at once.
void DoorController::Tick()
{
    TTimeStamp now;
    now = DateTimeToTimeStamp(Now());
    int days = now.Date - last_tick.Date;
    int ms = now.Time - last_tick.Time;
    bool elapsed = 0;
    if (ms / MS_PER_SECOND + days * SECONDS_PER_DAY > 1)
        elapsed = 1;
    for (vector<MapItem>::iterator map_iter = map_control->maps.begin(); map_iter != map_control->maps.end();
        map_iter++)
    {
        if (!map_iter->has_open_doors)
            continue;
        char found = 0;
        for (vector<MapObject>::iterator it = map_iter->tile_specs.begin(); it != map_iter->tile_specs.end(); it++)
        {
            if (!(it->value == TileSpec_DoorOpen || it->value == TileSpec_PassableDoorOpen))
                continue;
            found = 1;
            if (elapsed)
                it->ticks = 0;
            else
                it->ticks--;
            if (it->ticks <= 0)
            {
                if (it->value == TileSpec_DoorOpen)
                    it->value = TileSpec_Door;
                if (it->value == TileSpec_PassableDoorOpen)
                    it->value = TileSpec_PassableDoor;
            }
        }
        if (!found)
            map_iter->has_open_doors = false;
    }
    last_tick = now;
}
