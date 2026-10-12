#include <vcl.h>
#pragma hdrstop

#include "Jukeboxcontrol.h"
#include "Jukebox.h"
#include "Constants.h"

#pragma package(smart_init)

#define JUKEBOX_TRACK_SECONDS 90

JukeBoxController::JukeBoxController()
{
    encode_scratch = new char[8];
    jukeboxes.clear();
}

JukeBoxController::~JukeBoxController()
{
    jukeboxes.clear();
}

void JukeBoxController::Add(int map_id)
{
    JukeBox record(map_id);
    jukeboxes.insert(jukeboxes.end(), record);
}

void JukeBoxController::RemoveMap(int map_id)
{
    for (vector<JukeBox>::iterator it = jukeboxes.begin(); it != jukeboxes.end(); it++)
    {
        if (it->map_id == map_id)
        {
            jukeboxes.erase(it);
            return;
        }
    }
}

// The map's jukebox state: its id, and the name of the player who started the last track while it still plays.
String JukeBoxController::BuildOpenData(unsigned int map_id)
{
    String result = "";
    for (vector<JukeBox>::iterator it = jukeboxes.begin(); it != jukeboxes.end(); it++)
    {
        if ((unsigned int)it->map_id != map_id)
            continue;
        result.Insert(EncodeNumber(map_id, 2), result.Length() + 1);
        if (it->active)
        {
            TDateTime now = Now();
            TTimeStamp play_stamp = DateTimeToTimeStamp(it->last_play);
            TTimeStamp now_stamp = DateTimeToTimeStamp(now);
            int days = now_stamp.Date - play_stamp.Date;
            int ms = now_stamp.Time - play_stamp.Time;
            int secs = ms / MS_PER_SECOND + days * SECONDS_PER_DAY;
            if (secs < JUKEBOX_TRACK_SECONDS)
                result.Insert(it->player_name, result.Length() + 1);
            else
                it->active = false;
        }
    }
    return result;
}

// Starts a track on the map's jukebox and keeps the player's name.
// False while the last one still plays ( JUKEBOX_TRACK_SECONDS ).
char JukeBoxController::TryPlayTrack(int map_id, String player_name)
{
    char played = 0;
    for (vector<JukeBox>::iterator it = jukeboxes.begin(); it != jukeboxes.end(); it++)
    {
        if (it->map_id == map_id)
        {
            if (it->active)
            {
                TDateTime now = Now();
                TTimeStamp play_stamp = DateTimeToTimeStamp(it->last_play);
                TTimeStamp now_stamp = DateTimeToTimeStamp(now);
                int days = now_stamp.Date - play_stamp.Date;
                int ms = now_stamp.Time - play_stamp.Time;
                int secs = ms / MS_PER_SECOND + days * SECONDS_PER_DAY;
                if (secs > JUKEBOX_TRACK_SECONDS)
                    it->active = false;
            }
            if (!it->active)
            {
                played = 1;
                it->active = true;
                it->last_play = Now();
                it->player_name = player_name;
            }
            break;
        }
    }
    return played;
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(JukeBoxController)
