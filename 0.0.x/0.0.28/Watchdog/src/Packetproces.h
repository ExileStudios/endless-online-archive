#ifndef PacketprocesH
#define PacketprocesH

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



struct PacketProcessor;

// Reads the game server's packets into the pages' lists and counts.
struct PacketProcessor
{
    char pad_0x0[8];

    PacketProcessor();
    ~PacketProcessor();
    void HandlePacket(int action, int family, String data);
    void HandleTopPlayers(String data, int count);
    void HandleTopGuilds(String data, int count);
    void HandleOnlineList(String data);
    int DecodeNumber(String data);
    char GetBreakByte(unsigned char value);
};

#endif
