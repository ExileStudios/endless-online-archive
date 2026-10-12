#ifndef JukeboxcontrolH
#define JukeboxcontrolH

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

struct JukeBox;

#include "Jukebox.h"

struct JukeBoxController;

// The jukeboxes: the track playing on each map that has one.
struct JukeBoxController
{
    char *encode_scratch;
    vector<JukeBox> jukeboxes;

    JukeBoxController();
    ~JukeBoxController();
    void Add(int map_id);
    void RemoveMap(int map_id);
    String BuildOpenData(unsigned int map_id);
    char TryPlayTrack(int map_id, String player_name);
    String EncodeNumber(unsigned int value, signed int width);
};


#endif
