#include <vcl.h>
#pragma hdrstop

#include "Packetproces.h"
#include "Packetcontrols.h"
#include "Packetoperations.h"
#include "Tmainform.h"
#include "Stats_count.h"
#include "Stats_guilds.h"
#include "Stats_players.h"
#include "Stats_server.h"
#include "Stats_top100.h"

#pragma package(smart_init)

#include <stdlib.h>

#define EO_BREAK_BYTE 255

PacketProcessor::PacketProcessor()
{
}

PacketProcessor::~PacketProcessor()
{
}

// Handles one packet from the game server by its family and action.
void PacketProcessor::HandlePacket(int action, int family, String data)
{
    // .. Connection Player: the ping, with the new sequence start and the server's date and time
    if (family == OP_SECURITY && action == AC_SET)
    {
        int seq1 = DecodeNumber(data.SubString(1, 2));
        int seq2 = DecodeNumber(data.SubString(3, 1));
        int sequence = seq1 - seq2;
        MainForm->packet_controls->SetSequence(sequence);
        if (data.Length() > 10)
        {
            int server_date = DecodeNumber(data.SubString(4, 4));
            int server_time = DecodeNumber(data.SubString(8, 4));
            TTimeStamp now_stamp;
            now_stamp = DateTimeToTimeStamp(Now());
            int time_diff = abs(now_stamp.Time - server_time);
            int date_diff = abs(now_stamp.Date - server_date);
            // the first ping keeps how far the server's clock is from ours
            if (MainForm->packet_controls->time_offset == -1)
            {
                MainForm->packet_controls->date_offset = date_diff;
                MainForm->packet_controls->time_offset = time_diff;
            }
            else if (MainForm->packet_controls->date_offset != date_diff)
                time_diff = abs(time_diff - 86400000);
        }
        MainForm->packet_controls->packet_operator->SendPing();
    }
    // .. Message Reply: the answers to the requests ( "S" server stats, "P" top players, "G" top guilds )
    if (family == OP_SYSTEM && action == AC_RESULT)
    {
        if (data.SubString(1, 1) == "S")
        {
            int connections = DecodeNumber(data.SubString(2, 2));
            int players = DecodeNumber(data.SubString(4, 2));
            int most_players = DecodeNumber(data.SubString(6, 2));
            int uptime_minutes = DecodeNumber(data.SubString(8, 4));
            int accounts = DecodeNumber(data.SubString(12, 4));
            int characters = DecodeNumber(data.SubString(16, 4));
            int guilds = DecodeNumber(data.SubString(20, 4));
            MainForm->packet_controls->ReaderInit(data, GetBreakByte(EO_BREAK_BYTE));
            // skips the numbers above; the sent and received counts follow as text
            MainForm->packet_controls->ReaderGetBreakString();
            if (uptime_minutes < 60)
                MainForm->server_stats->uptime = IntToStr(uptime_minutes) + " minutes";
            else
                MainForm->server_stats->uptime = IntToStr(uptime_minutes / 60) + "h. " + IntToStr(uptime_minutes % 60) +
                    "m.";
            MainForm->server_stats->accounts = IntToStr(accounts);
            MainForm->server_stats->characters = IntToStr(characters);
            MainForm->server_stats->guilds = IntToStr(guilds);
            MainForm->server_stats->connections = IntToStr(connections);
            MainForm->server_stats->players = IntToStr(players);
            MainForm->server_stats->most_players = IntToStr(most_players);
            MainForm->server_stats->sent = MainForm->packet_controls->ReaderGetBreakString();
            MainForm->server_stats->received = MainForm->packet_controls->ReaderGetBreakString();
            MainForm->count_stats->players = IntToStr(players);
        }
        if (data.SubString(1, 1) == "P")
        {
            int top_player_count = DecodeNumber(data.SubString(2, 2));
            data.Delete(1, 3);
            HandleTopPlayers(data, top_player_count);
        }
        if (data.SubString(1, 1) == "G")
        {
            int top_guild_count = DecodeNumber(data.SubString(2, 2));
            data.Delete(1, 3);
            HandleTopGuilds(data, top_guild_count);
        }
    }
    // .. Players List: the players online
    if (family == OP_PLAYERS && action == AC_LIST)
        HandleOnlineList(data);
}

// Fills the top 100 list from the "P" answer: name, title, and the level, experience, gender and admin level.
void PacketProcessor::HandleTopPlayers(String data, int count)
{
    MainForm->top100_stats->ClearList();
    MainForm->top100_stats->player_count = count;
    MainForm->packet_controls->ReaderInit(data, GetBreakByte(EO_BREAK_BYTE));
    for (int i = 0; i < count; i++)
    {
        String name = MainForm->packet_controls->ReaderGetBreakString();
        String title = MainForm->packet_controls->ReaderGetBreakString();
        String info = MainForm->packet_controls->ReaderGetBreakString();
        int level = DecodeNumber(info.SubString(1, 1));
        int experience = DecodeNumber(info.SubString(2, 4));
        int gender = DecodeNumber(info.SubString(6, 1));
        int admin_level = DecodeNumber(info.SubString(7, 1));
        MainForm->top100_stats->AddPlayer(name, title, experience, level, gender, admin_level);
    }
}

// Fills the guild list from the "G" answer: tag, name, experience and the member count.
void PacketProcessor::HandleTopGuilds(String data, int count)
{
    MainForm->guild_stats->ClearList();
    MainForm->guild_stats->guild_count = count;
    MainForm->packet_controls->ReaderInit(data, GetBreakByte(EO_BREAK_BYTE));
    for (int i = 0; i < count; i++)
    {
        String tag = MainForm->packet_controls->ReaderGetBreakString();
        String guild_name = MainForm->packet_controls->ReaderGetBreakString();
        String info = MainForm->packet_controls->ReaderGetBreakString();
        int experience = DecodeNumber(info.SubString(1, 4));
        int exp_high = DecodeNumber(info.SubString(5, 1));
        int members = DecodeNumber(info.SubString(6, 2));
        MainForm->guild_stats->AddGuild(tag, guild_name, experience, exp_high, members);
    }
}

// Fills the online list from the Players List packet; its count is the page's player count too.
void PacketProcessor::HandleOnlineList(String data)
{
    MainForm->player_stats->ClearList();
    int count = DecodeNumber(data.SubString(1, 2));
    MainForm->player_stats->player_count = count;
    try
    {
        MainForm->count_stats->players = IntToStr(count);
    }
    catch (...)
    {
    }
    MainForm->packet_controls->ReaderInit(data, GetBreakByte(EO_BREAK_BYTE));
    // skips the count
    MainForm->packet_controls->ReaderGetBreakString();
    for (int i = 0; i < count; i++)
    {
        String name = MainForm->packet_controls->ReaderGetBreakString();
        String title = MainForm->packet_controls->ReaderGetBreakString();
        String info = MainForm->packet_controls->ReaderGetBreakString();
        int level = DecodeNumber(info.SubString(1, 1));
        int experience = DecodeNumber(info.SubString(2, 4));
        int gender = DecodeNumber(info.SubString(6, 1));
        int admin_level = DecodeNumber(info.SubString(7, 1));
        MainForm->player_stats->AddPlayer(name, title, experience, level, gender, admin_level);
    }
}

// Reads a number of up to four bytes, the same way as Client::DecodeNumber.
DEFINE_DECODE_NUMBER(PacketProcessor)

char PacketProcessor::GetBreakByte(unsigned char value)
{
    char byte_value = value;
    char result = byte_value;
    return result;
}
