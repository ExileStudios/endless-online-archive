#ifndef QueststateH
#define QueststateH

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

// The actions a quest state can take.
enum QuestActionId
{
    QuestActionId_AddNpcText = 1,
    QuestActionId_AddNpcInput = 2,
    QuestActionId_AddNpcChat = 3,
    QuestActionId_SetMap = 4,
    QuestActionId_GiveItem = 5,
    QuestActionId_RemoveItem = 6,
    QuestActionId_End = 7,
    QuestActionId_Reset = 8,
    QuestActionId_SetClass = 9,
    QuestActionId_PlayMusic = 10,
    QuestActionId_PlaySound = 11,
    QuestActionId_ShowHint = 12,
    QuestActionId_GiveExp = 13,
    QuestActionId_RemoveExp = 14,
    QuestActionId_GiveKarma = 15,
    QuestActionId_RemoveKarma = 16,
    QuestActionId_Quake = 17,
    QuestActionId_EffectOnPlayer = 18,
    QuestActionId_EffectOnCoord = 19,
    QuestActionId_ResetDaily = 20
};
#define QUEST_ACTION_ARGS 4
#define QUEST_RULE_ARGS 4



struct QuestAction;
struct QuestRule;
struct QuestState;

struct QuestAction
{
    int action;
    int args[4];
    String data[QUEST_ACTION_ARGS];

    QuestAction(int action);
    ~QuestAction();
};

struct QuestRule
{
    int rule;
    int args[QUEST_RULE_ARGS];
    String data[4];
    String goto_name;
    short goto_state_index;

    QuestRule(int rule);
    ~QuestRule();
};

// One state of a quest: its actions, and the rules that move the player to another state.
struct QuestState
{
    int state_index;
    String name;
    String description;
    int progress_rule_index;
    int progress_condition_type;
    vector<QuestAction *> actions;
    vector<QuestRule *> rules;

    QuestState(int state_index, String name);
    ~QuestState();
};


#endif
