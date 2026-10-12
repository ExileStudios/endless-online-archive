#ifndef QuestengineH
#define QuestengineH

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

enum QuestCondition
{
    QuestCondition_None = 0,
    QuestCondition_TalkedToNpc = 1,
    QuestCondition_InputNpc = 2,
    QuestCondition_GotItems = 3,
    QuestCondition_LostItems = 4,
    QuestCondition_Die = 5,
    QuestCondition_Disconnect = 6,
    QuestCondition_TimeElapsed = 7,
    QuestCondition_KilledNpcs = 8,
    QuestCondition_KilledPlayers = 9,
    QuestCondition_EnterCoord = 10,
    QuestCondition_EnterMap = 11,
    QuestCondition_LeaveMap = 12,
    QuestCondition_Always = 13,
    QuestCondition_DoneDaily = 14,
    QuestCondition_AnyEvent = 400
};

struct Quest;
struct QuestAction;
struct QuestRule;
struct QuestState;
struct QuestType;
struct Settings;

#include "Quest.h"
#include "Questtype.h"

struct QuestContainer;

// The quests, read from quests/00001.txt and up: each quest's states, their actions, and the rules that move on.
struct QuestContainer
{
    unsigned short max_quests;
    vector<Quest *> quest_list;
    Settings *settings;
    char *encode_scratch;
    QuestState *current_state;
    QuestAction *current_action;
    QuestRule *current_rule;
    bool expect_brace;
    bool expect_state_name;
    bool in_state;
    int state_count;
    bool expect_quest_name;
    bool expect_version;
    bool expect_desc;
    int action_type;
    bool in_action_args;
    int rule_type;
    bool in_rule_args;
    bool expect_goto_name;
    bool expect_goto;
    vector<QuestType> action_names;
    vector<QuestType> cond_names;

    QuestContainer(Settings *settings);
    ~QuestContainer();
    void LoadQuests();
    bool LoadQuest(int quest_id);
    void ParseToken(Quest *quest, String token);
    char GetQuestLoaded(int quest_id);
    int GetQuestVersion(int quest_id);
    String GetQuestName(int quest_id);
    String GetActionData(int quest_id, int state_index, int behavior_id);
    String GetActionData2(int quest_id, int state_index, int behavior_id);
    int GetRuleValue(int quest_id, int state_index, int behavior_id);
    int GetRuleValue2(int quest_id, int state_index, int input_id);
    QuestState *GetState(int quest_id, int state_index);
    void RegisterAction(int action_id, String name);
    void RegisterCondition(int condition_id, String name);
    int GetActionType(String name);
    int GetConditionType(String name);
    int ParseInt(String token);
    String EncodeNumber(unsigned int value, signed int width);
};


#endif
