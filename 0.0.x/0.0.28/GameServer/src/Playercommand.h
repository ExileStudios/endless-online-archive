#ifndef PlayercommandH
#define PlayercommandH

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

enum PacketFamily
{
    OP_SECURITY = 1,
    OP_ACCOUNT = 2,
    OP_CHARACTER = 3,
    OP_LOGIN = 4,
    OP_PLAY = 5,
    OP_MOVE = 6,
    OP_DIR = 7,
    OP_SIT = 8,
    OP_EMOTE = 9,
    OP_ATTACK = 11,
    OP_MAGIC = 12,
    OP_SHOP = 13,
    OP_ITEM = 14,
    OP_SKILL = 16,
    OP_PCHAT = 17,
    OP_CHAT = 18,
    OP_MAP = 19,
    OP_MUSIC = 21,
    OP_PLAYERS = 22,
    OP_PLAYER = 23,
    OP_GROUP = 24,
    OP_REFRESH = 25,
    OP_NPC = 26,
    OP_PLAYERRANGE = 27,
    OP_NPCRANGE = 28,
    OP_RANGE = 29,
    OP_PAPERDOLL = 30,
    OP_EFFECT = 31,
    OP_TRADE = 32,
    OP_CHEST = 33,
    OP_DOOR = 34,
    OP_SYSTEM = 35,
    OP_BANK = 36,
    OP_LOCKER = 37,
    OP_BARBER = 38,
    OP_GUILD = 39,
    OP_SOUND = 40,
    OP_REST = 41,
    OP_GAMEBAR = 42,
    OP_MSGBOARD = 43,
    OP_M_NPC = 44,
    OP_ARENA = 45,
    OP_PRIEST = 46,
    OP_LAW = 47,
    OP_ADMIN = 48,
    OP_INN = 49,
    OP_QUEST = 50,
    OP_BOOK = 51
};



struct PlayerCommand;

// A packet held back in a player's action queue until its turn.
struct PlayerCommand
{
    int family;
    int action;
    String text;

    PlayerCommand(int family, int action, String text);
    ~PlayerCommand();
};


#endif
