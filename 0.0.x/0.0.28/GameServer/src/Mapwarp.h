#ifndef MapwarpH
#define MapwarpH

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



struct MapWarp;

// A warp tile: stepping on from_x, from_y goes to dest_map at to_x, to_y.
struct MapWarp
{
    unsigned short from_x;
    unsigned short from_y;
    int dest_map;
    int level;
    unsigned short to_x;
    unsigned short to_y;

    MapWarp(int from_x, int from_y, int dest_map, int level, int to_x, int to_y);
    ~MapWarp();
};


#endif
