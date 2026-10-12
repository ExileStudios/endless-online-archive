#ifndef PacketoperationsH
#define PacketoperationsH

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



struct PacketOperator;

struct PacketOperator
{
    char pad_0x0[8];

    PacketOperator();
    ~PacketOperator();
    void RequestServerStats();
    void RequestTopPlayers();
    void RequestTopGuilds();
    char GetBreakByte(unsigned char value);
    void RequestOnlineList();
    void SendPing();
};

#endif
