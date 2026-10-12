#include <vcl.h>
#pragma hdrstop

#include "Packetoperations.h"
#include "Tmainform.h"
#include "Packetcontrols.h"

#pragma package(smart_init)

PacketOperator::PacketOperator()
{
}

PacketOperator::~PacketOperator()
{
}

// Asks the game server for its statistics ( the "S" answer in PacketProcessor::HandlePacket ).
void PacketOperator::RequestServerStats()
{
    MainForm->packet_controls->SendPacket(AC_REQUEST, OP_SYSTEM, "S");
}

// Asks the game server for its top players ( the "P" answer ).
void PacketOperator::RequestTopPlayers()
{
    MainForm->packet_controls->SendPacket(AC_REQUEST, OP_SYSTEM, "P");
}

// Asks the game server for its top guilds ( the "G" answer ).
void PacketOperator::RequestTopGuilds()
{
    MainForm->packet_controls->SendPacket(AC_REQUEST, OP_SYSTEM, "G");
}

char PacketOperator::GetBreakByte(unsigned char value)
{
    char byte_value = value;
    char result = byte_value;
    return result;
}

// Asks the game server who is online ( answered by the Players List packet, see PacketProcessor::HandleOnlineList ).
void PacketOperator::RequestOnlineList()
{
    String break_text = GetBreakByte(255);
    if (break_text.Length() >= 1)
        MainForm->packet_controls->SendPacket(AC_LIST, OP_SYSTEM, break_text);
}

// Answers the server's ping.
void PacketOperator::SendPing()
{
    MainForm->packet_controls->SendPacket(AC_SYN1, OP_SECURITY, "k");
}
