#ifndef JukeboxH
#define JukeboxH

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

// A map's jukebox: the last track and when it played.
struct JukeBox
{
    short map_id;
    bool active;
    TDateTime last_play;
    String player_name;

    JukeBox(int map_id);
    ~JukeBox();
};


#endif
